// UStream.c: EA's streaming asset loader (the CTRL / SHOC / SONO chunk files: .hog, .gcb,
// .ngc). The file name comes from the assert string at 0x801868A0; the field names and the
// UStream_ function names are ours, the Stream_ ones EA's (TW07). CodeWarrior GC/2.5, -O4,p.
// See docs/formats/ctrl-container.md.
//
// Shape of the system:
//   - a stream is one of the .hog/.gcb files (up to 8 file names per stream, played in order);
//   - the DVD is read asynchronously into 18 fixed 24 KiB buffers that move through rings:
//     free -> in flight -> ready -> current (being parsed) -> free again;
//   - Stream_ParseBufs walks the chunks of the current buffers and builds objects: an SHDR
//     chunk allocates the object, SDAT / Rdat chunks fill it (Rdat through UStream_Decompress);
//   - finished objects are queued and handed to the handler registered for their type
//     (Stream_RegisterLoadChunkCallback), e.g. the texture manager for 'txf '.

#include "game_types.h"
#include "engine.h"
#include "ustream.h"
#include "endian.h"
#include "core/startup.h"

// ---- state -------------------------------------------------------------------------------
// Everything here is private to the file. CodeWarrior lays out a file's static data in the
// reverse of the order it is declared, so it is declared last-first: the original has these
// arrays at 0x801A2AC0.. and the scalars at 0x80281C18.. in the opposite order. The scalars
// written from the DVD-read callback are volatile (the original reloads them after every
// statement; the main-thread-only ones it does not).

static char             gSWVRName[0x14];                        // 0x801A3424
static UStreamHandler   gHandlers[USTREAM_MAX_HANDLERS];        // 0x801A3124
static UStreamBuffer*   gReadyRing[USTREAM_NUM_BUFFERS];        // 0x801A30DC
static UStreamBuffer*   gFreeRing[USTREAM_NUM_BUFFERS];         // 0x801A3094
static UStream          gStreams[USTREAM_MAX_STREAMS];          // 0x801A2AD4
static UStreamSound     gSoundHeader;                           // 0x801A2AC0 (SONO state)

static uptr gRPNSBase;            // 0x80281C5C  data address of the last RPNS object
static UStreamBuffer* volatile gpReadBuffer;  // 0x80281C58  buffer of the read in flight
static volatile s8 gReadyRingHead;       // 0x80281C57
static volatile s8 gFreeRingTail;        // 0x80281C56
static s8   gReadyRingTail;       // 0x80281C55
static s8   gFreeRingHead;        // 0x80281C54
static UStreamBuffer* gpFreeList; // 0x80281C50  released buffers not yet back in the ring
static UStreamBuffer* gpCurList;  // 0x80281C4C  buffers being parsed
static UStreamBuffer* gpUsedList; // 0x80281C48  parsed buffers that objects still reference
static UStreamFill gFill;         // 0x80281C40  object being filled and how far
static UMemPool* gpNodePool;      // 0x80281C3C
static UStreamNode* gpDoneList;   // 0x80281C38  objects finished by the parser
static void* gpBufferMemory;      // 0x80281C34
static int  gnCurStream;          // 0x80281C30  -1 = none
static int  gnNumStreams;         // 0x80281C2C
static volatile s8 gbReadPending;        // 0x80281C28
static UStreamObject* gpQueueHead;   // 0x80281C24
static UStreamObject* gpQueueTail;   // 0x80281C20
static int  gnReadErrors;         // 0x80281C1C
static u8   gbAutoRead;           // 0x80281C19  keep reading after each completed read
static s8   gbPaused;             // 0x80281C18

int gnNumHandlers = -1;           // 0x80280DB8 (.sdata): -1 until UStream_Init

// ---- other files' functions -----------------------------------------------------------

void  LLVideo_HandleChunk(void* pChunk);                             // MPG2
void  fn_800A8AD4(void* pChunk);                             // DSPM / VAGM / XADP
void* fn_800A8FB4(u32 uSize, int nMemory);
void  fn_800A8FFC(u32 uMemory);
void* fn_800A925C(u32 uSize, u32 uType);
void  fn_800A929C(u32 uType);
void* fn_800A9374(u32 uSize);
void  fn_800A93AC(void);
void  fn_8000E708(UStreamParams* p);
void  fn_8015A7FC(char* pDst, const char* pSrc);             // strcpy

static void UStream_ReadDone(int nBytes, int nError);
static void UStream_StartRead(void);
u8* UStream_Fill(u8* pDst, u32 value, u32 uCount);
u8* UStream_Copy(u8* pDst, const u8* pSrc, u32 uCount);
void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize);
int UStream_Close(int nStream);

// ---- buffers ------------------------------------------------------------------------------

// Give a parsed buffer back: append it to the released list (the ring is refilled later).
static void UStream_ReleaseBuffer(UStreamBuffer* pBuffer) {
    UStreamBuffer* p;
    if (gnCurStream == -1) return;
    pBuffer->nRefs = 0;
    if (gpFreeList == NULL) {
        gpFreeList = pBuffer;
        return;
    }
    for (p = gpFreeList; p->pNext != NULL; p = p->pNext) {}
    p->pNext = pBuffer;
}

// Start the next asynchronous read into a free buffer.
static void UStream_StartRead(void) {
    UStreamBuffer* pBuffer;
    UStream* pStream;
    if (gbPaused) return;
    if (gnCurStream == -1) return;
    pStream = &gStreams[gnCurStream];
    pBuffer = gFreeRing[gFreeRingTail];
    if (pBuffer == NULL) return;
    if (pStream->bEOF) return;
    if (pStream->hFile < 0) return;
    gFreeRing[gFreeRingTail] = NULL;
    if (++gFreeRingTail == USTREAM_NUM_BUFFERS) {
        gFreeRingTail = 0;
    }
    gpReadBuffer = pBuffer;
    gpReadBuffer->nFileOffset = pStream->nFilePos;
    gbReadPending = 1;
    if (fn_80006444(pStream->hFile, pBuffer->data, USTREAM_BUFFER_SIZE, pStream->nFilePos,
                    UStream_ReadDone) < 0) {
        gbReadPending = 0;
        gnReadErrors++;
        UStream_ReleaseBuffer(gpReadBuffer);
    } else {
        gnReadErrors = 0;
    }
}

// Async read completion. Checks the read, advances the file position, queues the buffer in
// the ready ring, and starts the next read if reading is continuous.
static void UStream_ReadDone(int nBytes, int nError) {
    UStream* pStream;
    int nStatus;
    u8 bDropped;
    if (gnCurStream == -1) return;
    pStream = &gStreams[gnCurStream];
    if (pStream->hFile < 0) {
        gbReadPending = 0;
        return;
    }
    nStatus = nError;
    if (nError != 0) {
        nStatus = 5;
    } else if (nBytes != USTREAM_BUFFER_SIZE) {
        nStatus = 5;
    }
    if (pStream->nFilePos != gpReadBuffer->nFileOffset) {
        nStatus = 0xDEAD;
    } else if (pStream->nFileSize == nBytes + pStream->nFilePos) {
        pStream->bEOF = 1;
        nStatus = 0;
    }
    if (nStatus == 0) {
        pStream->nFilePos += nBytes;
        if (pStream->nChunkBudget > 0) {
            pStream->nChunkBudget--;
            if (pStream->nChunkBudget == 0) {
                pStream->nChunkBudget = -1;
                pStream->bEOF = 1;
            }
        }
        bDropped = 0;
        // fake match: the original turns the test into a 0/1 value (cntlzw) before branching
        if ((BE32(gpReadBuffer->data) == TAG('S', 'W', 'V', 'R')) ? 1 : 0) {
            if (pStream->bWaitingForSWVR) {
                pStream->bEOF = 1;
                bDropped = 1;
                UStream_ReleaseBuffer(gpReadBuffer);
            } else {
                pStream->bWaitingForSWVR = 1;
            }
        }
        if (!bDropped) {
            gReadyRing[gReadyRingHead] = gpReadBuffer;
            if (++gReadyRingHead == USTREAM_NUM_BUFFERS) {
                gReadyRingHead = 0;
            }
        }
        gnReadErrors = 0;
    } else {
        UStream_ReleaseBuffer(gpReadBuffer);
        gnReadErrors++;
    }
    gpReadBuffer = NULL;
    gbReadPending = 0;
    if (gbAutoRead && !gbReadPending && !pStream->bEOF) {
        UStream_StartRead();
    }
}

void UStream_SetAutoRead(u8 bAuto) {
    gbAutoRead = bAuto;
}

// Object allocation from an SHDR chunk: header + a 'Csac' chunk's extra bytes + 0x80-aligned data.
static int UStream_BeginObject(UStreamFill* pFill, UStreamChunk* pChunk) {
    u32 uExtra;
    int nWanted;
    u32 uPad;
    UStreamObject* pObject;
    if (pChunk->uType == TAG('C', 's', 'a', 'c')) {
        nWanted = 1;
        uExtra = ((pChunk->nScriptLen + pChunk->n38 + 3) & ~3) + 8;
    } else {
        // The first of the two scripts says whether the object is wanted; the second runs only
        // when it is.
        u8* pCode = pChunk->aScripts;
        u8* pNext = pCode + pChunk->nScriptLen;

        uExtra = 0;
        nWanted = fn_8000EA1C(pCode, 0, -1, NULL);
        if (nWanted) {
            fn_8000EA1C(pNext, 0, -1, NULL);
        }
    }
    if (nWanted) {
        if (pChunk->uType == TAG('t', 'x', 'f', ' ')) {
            pChunk->uFlags = 1;
        }
        if (pChunk->uType == TAG('C', 'p', 'y', 'r')) {
            pChunk->uFlags = 1;
        }
        if (pChunk->uType == TAG('C', 'a', 'c', 't')) {
            pChunk->uFlags = 1;
        }
        if (pChunk->uType == TAG('t', 'x', 'f', '2')) {
            pChunk->uFlags = 1;
        }
        uPad = (uExtra + sizeof(UStreamObject)) & 0x7F;
        uPad = uPad ? 0x80 - uPad : 0;
        pObject = fn_80009B34(sizeof(UStreamObject) + uExtra + uPad + pChunk->uSize, pChunk->uFlags,
                              0x80, "UStream.c", 732);
        pFill->pObject = pObject;
        pObject->nUnk14 = 0;
        pFill->uPos = 0;
        Mem_cpy(&pObject->uFlags, &pChunk->uFlags, uExtra + 0x1C);
        pObject->pData = (u8*)(pObject + 1) + uExtra + uPad;
        pObject->uUnk4 = 0;
        pObject->pfn8 = NULL;
        pObject->pPrev = NULL;
        pObject->pNext = NULL;
    }
    return 0;
}

// Finished parsing the current list head: move it to the used list while objects still reference
// it, else release it.
static void UStream_RetireCurrentBuffer(void) {
    UStreamBuffer* pBuffer;
    UStreamBuffer* p;
    if (gnCurStream == -1) return;
    pBuffer = gpCurList;
    gpCurList = pBuffer->pNext;
    pBuffer->pNext = NULL;
    pBuffer->uPos = 0;
    if (pBuffer->nRefs > 0) {
        if (gpUsedList == NULL) {
            gpUsedList = pBuffer;
        } else {
            for (p = gpUsedList; p->pNext != NULL; p = p->pNext) {}
            p->pNext = pBuffer;
        }
    } else {
        UStream_ReleaseBuffer(pBuffer);
    }
}

// An object is complete: put it on the done list for UStream_Update to deliver.
static void UStream_FinishObject(UStreamObject* pObject) {
    UStreamNode* pNode;
    UStreamNode* p;
    if (gnCurStream == -1) return;
    pNode = AllocPoolMem(gpNodePool);
    if (pNode == NULL) return;
    pNode->pNext = NULL;
    pNode->pObject = pObject;
    if (gpDoneList == NULL) {
        gpDoneList = pNode;
        return;
    }
    for (p = gpDoneList; p->pNext != NULL; p = p->pNext) {}
    p->pNext = pNode;
}

// Recycle released buffers into the free ring, kick a read, and (with bTakeReady) move ready
// buffers onto the current list. Returns the current list head.
static UStreamBuffer* UStream_PumpBuffers(u8 bTakeReady) {
    UStream* pStream;
    if (gnCurStream == -1) return NULL;
    pStream = &gStreams[gnCurStream];
    if (!pStream->bEOF) {
        while (gpFreeList != NULL) {
            UStreamBuffer* p = gpFreeList;
            gpFreeList = p->pNext;
            p->pNext = NULL;
            gFreeRing[gFreeRingHead] = p;
            if (++gFreeRingHead == USTREAM_NUM_BUFFERS) {
                gFreeRingHead = 0;
            }
        }
        if (gFreeRing[gFreeRingTail] != NULL && !gbReadPending) {
            UStream_StartRead();
        }
    }
    if (bTakeReady) {
        UStreamBuffer* p;
        while ((p = gReadyRing[gReadyRingTail]) != NULL) {
            UStreamBuffer* q;
            gReadyRing[gReadyRingTail] = NULL;
            if (++gReadyRingTail == USTREAM_NUM_BUFFERS) {
                gReadyRingTail = 0;
            }
            p->pNext = NULL;
            q = gpCurList;
            if (q == NULL) {
                gpCurList = p;
            } else {
                while (q->pNext != NULL) {
                    q = q->pNext;
                }
                q->pNext = p;
            }
        }
    }
    return gpCurList;
}

// Cut a ".stream" extension (any case) off a name, in place.
static void UStream_StripStreamExt(char* pName) {
    const char* pPattern = ".stream";
    char* pStart = NULL;
    const char* pMatch = pPattern;
    char c;
    while ((c = *pName) != 0) {
        if (c >= 'A' && c <= 'Z') {
            c += 0x20;
        }
        if (c == *pMatch) {
            if (pStart == NULL) {
                pStart = pName;
            }
            if (*++pMatch == 0) {
                *pStart = 0;
                return;
            }
        } else {
            pMatch = pPattern;
            pStart = NULL;
        }
        pName++;
    }
    if (pStart != NULL) {
        *pStart = 0;
    }
}

u8* UStream_Fill(u8* pDst, u32 value, u32 uCount) {
    u8 v = (u8)value;
    u8* p = pDst;
    while (uCount--) {
        *p++ = v;
    }
    return pDst;
}

// Forward copy that may overlap (source before destination); copies halfwords when both
// pointers have the same alignment. Used by the decompressor for back-references.
// The eight-at-a-time blocks read into temporaries first: that is what lets the compiler
// hoist the loads above the stores.
u8* UStream_Copy(u8* pDst, const u8* pSrc, u32 uCount) {
    if (((uptr)pDst & (uptr)pSrc) & 1) {
        *pDst++ = *pSrc++;
        uCount--;
    }
    if ((((uptr)pDst | (uptr)pSrc) & 1) == 0) {
        s16* d = (s16*)pDst;
        const s16* s = (const s16*)pSrc;
        u32 uOdd = uCount & 1;
        uCount >>= 1;
        while (uCount > 7) {
            {
                s16 t0 = s[0], t1 = s[1], t2 = s[2], t3 = s[3], t4 = s[4], t5 = s[5], t6 = s[6], t7 = s[7];
                d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3; d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                d += 8; s += 8;
            }
            uCount -= 8;
        }
        while (uCount--) {
            *d++ = *s++;
        }
        if (uOdd) {
            *(u8*)d = *(const u8*)s;
        }
    } else {
        u8* d = pDst;
        while (uCount > 7) {
            {
                u8 t0 = pSrc[0], t1 = pSrc[1], t2 = pSrc[2], t3 = pSrc[3];
                u8 t4 = pSrc[4], t5 = pSrc[5], t6 = pSrc[6], t7 = pSrc[7];
                d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3; d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                d += 8; pSrc += 8;
            }
            uCount -= 8;
        }
        while (uCount--) {
            *d++ = *pSrc++;
        }
    }
    return pDst;
}

// Decompress an 'Rdat' payload. Two-byte command words, big-endian:
//   1nnn1ooo cccccccc  nnn != 0: fill (c + 3) bytes with the byte at dst - (ooo<<3 | nnn)
//   10001lll llllllll  literal run of l bytes (l = low 11 bits)
//   mlll dddd dddddddd  back-reference: length lll (+ next byte if 7) + 3, distance d;
//                       m clear: forward copy; m set: mirrored copy (bytes read backwards)
void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize) {
    u8* pEnd;
    u8* d = (u8*)pDst;
    const u8* s = (const u8*)pSrc;
    s32 nLen;   // long, not int: with int CodeWarrior folds the +3 into every use (see decomp-notes.md)
    pEnd = d + uSize;
    do {
        u32 uCode;
        uCode = s[0] << 8 | s[1];
        s += 2;
        if ((uCode & 0x8800) != 0x8800) {
            u32 uDist = uCode & 0xFFF;
            const u8* pFrom;
            nLen = (uCode >> 12) & 7;
            if (nLen == 7) {
                nLen += *s++;
            }
            nLen += 3;
            pFrom = d - uDist;
            if ((uCode & 0x8000) == 0) {
                UStream_Copy(d, pFrom, nLen);
                d += nLen;
            } else {
                // Mirrored copy: the reference is read backwards, so a run of bytes comes
                // out reversed. Eight at a time through temporaries, then the remainder.
                int nBlocks;
                pFrom += 2;
                if (nLen >= 8) {
                    for (nBlocks = nLen / 8; nBlocks > 0; nBlocks--) {
                        u8 t7 = pFrom[-7], t6 = pFrom[-6], t5 = pFrom[-5], t4 = pFrom[-4];
                        u8 t3 = pFrom[-3], t2 = pFrom[-2], t1 = pFrom[-1], t0 = pFrom[0];
                        d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3;
                        d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                        d += 8; pFrom -= 8;
                    }
                }
                nBlocks = nLen & 7;
                while (nBlocks > 0) {
                    *d++ = *pFrom--;
                    nBlocks--;
                }
            }
        } else {
            nLen = (uCode >> 12) & 7;
            if (nLen == 0) {
                nLen = uCode & 0x7FF;
                while (nLen > 0) {
                    *d++ = *s++;
                    nLen--;
                }
            } else {
                u32 uDist = nLen | ((uCode >> 5) & 0x38);
                u32 uCount = uCode & 0xFF;
                UStream_Fill(d, d[-(int)uDist], uCount + 3);
                d += uCount + 3;
            }
        }
    } while (d < pEnd);
}

// A buffer's data was handed to an object: count the reference.
void UStream_AddBufferRef(UStreamBuffer** ppList) {
    if (gnCurStream == -1) return;
    (*ppList)->nRefs++;
}

static void UStream_NullCallback(u32 n) {
}

// Walk the chunks of the current buffers. SHOC chunks build objects, SONO chunks build sound
// objects (through the audio module), the stream / movie tags go to their players.
static void Stream_ParseBufs(void) {
    UStreamChunk* pChunk;
    UStreamBuffer* pBuffer;
    u32 uLen;
    u32 uTag;
    u32 uCopy;
    UStreamFill* pFill;
    if (UStream_PumpBuffers(1) == NULL) return;
    if (gnCurStream == -1) return;
    while ((pBuffer = gpCurList) != NULL) {
        // fake match: a volatile signed read of uPos makes the loop top load it again, as the original does
        while (*(volatile s32*)&pBuffer->uPos < USTREAM_BUFFER_SIZE) {
            // fake match: indexing data through a u8* (not the array) adds uPos to pBuffer before the 0x40
            pChunk = (UStreamChunk*)&((u8*)pBuffer->data)[pBuffer->uPos];
            // port: the chunk header is big-endian and read through UStreamChunk (and copied into the
            // object by UStream_BeginObject): a little-endian port converts its 0x40 bytes here
            uTag = pChunk->uTag;
            uLen = pChunk->uLength;
            switch (uTag) {
            case TAG('S', 'W', 'V', 'R'):
                // the stream's name: the 16 bytes after the header
                {
                    u32* pName = (u32*)gSWVRName;

                    pName[0] = ((u32*)(pChunk + 1))[0];
                    pName[1] = ((u32*)(pChunk + 1))[1];
                    pName[2] = ((u32*)(pChunk + 1))[2];
                    pName[3] = ((u32*)(pChunk + 1))[3];
                    pName[4] = 0;
                }
                UStream_StripStreamExt(gSWVRName);
                break;
            case TAG('S', 'H', 'O', 'C'):
                if (pChunk->uSubTag == TAG('S', 'H', 'D', 'R')) {
                    if (UStream_BeginObject(&gFill, pChunk) != 0) return;
                } else if (pChunk->uSubTag == TAG('S', 'D', 'A', 'T')) {
                    uLen = pChunk->uLength - 0x40;
                    pBuffer->uPos += 0x40;
                    pFill = &gFill;
                    if (pFill->pObject != NULL) {
                        uCopy = uLen;
                        if ((int)(pFill->uPos + uLen) > (int)pFill->pObject->uSize) {
                            uCopy = pFill->pObject->uSize - pFill->uPos;
                        }
                        Mem_cpy(pFill->pObject->pData + pFill->uPos, (u8*)(pChunk + 1), uCopy);
                        pFill->uPos += uCopy;
                        if (pFill->uPos == pFill->pObject->uSize) {
                            UStream_FinishObject(pFill->pObject);
                            pFill->pObject = NULL;
                        }
                    }
                } else if (pChunk->uSubTag == TAG('R', 'd', 'a', 't')) {
                    uLen = pChunk->uLength - 0x40;
                    pBuffer->uPos += 0x40;
                    pFill = &gFill;
                    if (pFill->pObject != NULL) {
                        // the piece's unpacked size, then the packed bytes
                        u32 uUnpacked = BE32(pChunk + 1);
                        UStream_Decompress((u32*)(pChunk + 1) + 1, pFill->pObject->pData + pFill->uPos,
                                           uUnpacked);
                        pFill->uPos += uUnpacked;
                        if (pFill->uPos == pFill->pObject->uSize) {
                            UStream_FinishObject(pFill->pObject);
                            pFill->pObject = NULL;
                        }
                    }
                }
                break;
            case TAG('F', 'I', 'L', 'L'):
                pBuffer->uPos = USTREAM_BUFFER_SIZE;
                continue;
            case TAG('D', 'S', 'P', 'M'):
            case TAG('V', 'A', 'G', 'M'):
            case TAG('X', 'A', 'D', 'P'):
                fn_800A8AD4(pChunk);
                break;
            case TAG('S', 'O', 'N', 'O'):
                if (pChunk->uSubTag == TAG('S', 'H', 'D', 'R')) {
                    switch (pChunk->uType) {
                    case TAG('s', 'h', 'd', 'r'):
                        if (pChunk->uId == 2) {
                            gSoundHeader.pDst = fn_800A9374(pChunk->uSize);
                        } else {
                            gSoundHeader.pDst = fn_800A8FB4(pChunk->uSize, pChunk->uId);
                        }
                        break;
                    case TAG('s', 'a', 'm', 'p'):
                        gSoundHeader.pDst = fn_800A925C(pChunk->uSize, pChunk->uId);
                        break;
                    default:
                        gSoundHeader.pDst = NULL;
                        break;
                    }
                    gSoundHeader.uSize = pChunk->uSize;
                    gSoundHeader.uPos = 0;
                    gSoundHeader.uKind = pChunk->uType;
                    gSoundHeader.uMemory = pChunk->uId;
                } else if (pChunk->uSubTag == TAG('S', 'D', 'A', 'T')) {
                    u32 uCopy = pChunk->uLength - 0x40;
                    if (gSoundHeader.pDst != NULL) {
                        u8* pSrc = (u8*)(pChunk + 1);
                        u8* pDst = gSoundHeader.pDst + gSoundHeader.uPos;
                        if (gSoundHeader.uPos + uCopy > gSoundHeader.uSize) {
                            uCopy = gSoundHeader.uSize - gSoundHeader.uPos;
                        }
                        switch (gSoundHeader.uKind) {
                        case TAG('s', 'h', 'd', 'r'):
                            Mem_cpy(pDst, pSrc, uCopy);
                            break;
                        case TAG('s', 'a', 'm', 'p'):
                            // port: for sample data pDst holds an ARAM address, not a pointer
                            fn_800B044C((u32)(uptr)pDst, pSrc, uCopy, UStream_NullCallback, 0);
                            break;
                        }
                    }
                    gSoundHeader.uPos += uCopy;
                    if (gSoundHeader.uPos >= gSoundHeader.uSize) {
                        switch (gSoundHeader.uKind) {
                        case TAG('s', 'h', 'd', 'r'):
                            if (gSoundHeader.uMemory == 2) {
                                fn_800A93AC();
                            } else {
                                fn_800A8FFC(gSoundHeader.uMemory);
                            }
                            break;
                        case TAG('s', 'a', 'm', 'p'):
                            fn_800A929C(gSoundHeader.uMemory);
                            break;
                        }
                        gSoundHeader.uSize = 0;
                        gSoundHeader.uPos = 0;
                        gSoundHeader.uKind = 0;
                        gSoundHeader.pDst = NULL;
                    }
                }
                break;
            case TAG('M', 'P', 'G', '2'):
                pBuffer->nRefs++;
                // port: the movie player gets its buffer through the chunk's first word (a 32-bit pointer)
                *(UStreamBuffer**)pChunk = pBuffer;
                LLVideo_HandleChunk(pChunk);
                break;
            case TAG('C', 'T', 'R', 'L'):
            case TAG('P', 'A', 'D', 'D'):
            default:
                break;
            }
            pBuffer->uPos += uLen;
        }
        UStream_RetireCurrentBuffer();
        UStream_PumpBuffers(0);
    }
}

// ---- files ------------------------------------------------------------------------------

static void fn_8000D9A4(void) {
}

// Open the stream's current file and tell the owner.
static void UStream_OpenFile(UStream* pStream) {
    int i;
    pStream->hFile = fn_800060E0(pStream->params.aszName[pStream->nFileIndex]);
    pStream->nFileSize = fn_800065B0(pStream->hFile);
    pStream->nFilePos = 0;
    pStream->bEOF = 0;
    pStream->bWaitingForSWVR = 1;
    i = pStream->nFileIndex;
    if (pStream->params.apfnOpened[i] != NULL) {
        pStream->params.apfnOpened[i](pStream->params.apOpenedArg[i]);
    }
}

// Close the current file, tell the owner, and open the next one.
static void UStream_NextFile(UStream* pStream) {
    int i;
    fn_8000633C(pStream->hFile);
    i = pStream->nFileIndex;
    if (pStream->params.apfnClosed[i] != NULL) {
        pStream->params.apfnClosed[i](pStream->params.apClosedArg[i]);
    }
    pStream->nFileIndex++;
    UStream_OpenFile(pStream);
}

// Parse what has arrived and return the next finished object, or NULL.
static UStreamObject* UStream_NextObject(u8 bParse) {
    UStream* pStream;
    UStreamNode* pNode;
    UStreamObject* pObject;
    if (bParse) {
        Stream_ParseBufs();
    }
    if (gnCurStream == -1) return NULL;
    pStream = &gStreams[gnCurStream];
    if (gpCurList == NULL && bParse) {
        Stream_ParseBufs();
    }
    if (gpCurList == NULL) {
        if (UStream_PumpBuffers(1) == NULL && pStream->bEOF) {
            if (pStream->nFileIndex + 1 < pStream->params.nNumFiles) {
                UStream_NextFile(pStream);
            }
        }
    }
    pNode = gpDoneList;
    if (pNode == NULL) return NULL;
    pObject = pNode->pObject;
    gpDoneList = pNode->pNext;
    ReturnPoolMem(gpNodePool, pNode);
    return pObject;
}

// The per-frame call: pull finished objects, rebase their references, and deliver each to the
// handler registered for its type. Returns 0 when the stream has nothing left to do.
u8 UStream_Update(void) {
    UStreamObject* pObject;
    int i;
    UStreamObject* pNext;
    UStream* pStream;
    fn_800B7490();
    for (pObject = UStream_NextObject(1); pObject != NULL; pObject = UStream_NextObject(0)) {
        pObject->pPrev = NULL;
        pObject->pNext = NULL;
        if (gpQueueTail != NULL) {
            gpQueueTail->pPrev = pObject;
            pObject->pNext = gpQueueTail;
            gpQueueTail = pObject;
        } else {
            gpQueueHead = pObject;
            gpQueueTail = pObject;
        }
    }
    pObject = gpQueueHead;
    if (pObject == NULL) {
        if (gnCurStream == -1) return 0;
        pStream = &gStreams[gnCurStream];
        (void)*(volatile UStream*)pStream;   // fake match: sequence pointer evaluation before fn_800A4BDC
        fn_800A4BDC();
        if (pStream->bEOF &&!gbReadPending && gReadyRing[gReadyRingTail] == NULL && gpCurList == NULL
            && gpUsedList == NULL) {
            return 0;
        }
        return 1;
    }
    do {
        pNext = pObject->pPrev;
        if (pObject->pNext != NULL) {
            pObject->pNext->pPrev = pObject->pPrev;
        } else {
            gpQueueHead = pObject->pPrev;
            if (gpQueueHead != NULL) {
                gpQueueHead->pNext = NULL;
            }
        }
        if (pObject->pPrev != NULL) {
            pObject->pPrev->pNext = pObject->pNext;
        } else {
            gpQueueTail = pObject->pNext;
            if (gpQueueTail != NULL) {
                gpQueueTail->pPrev = NULL;
            }
        }
        pObject->uRef28 += gRPNSBase;
        pObject->uRef2C += gRPNSBase;
        pObject->uRef30 += gRPNSBase;
        switch ((int)pObject->uType) {   // the original compares the tag signed (cmpw)
        case TAG('C', 'c', 't', 'r'):
            fn_80009E70(pObject);
            break;
        case TAG('R', 'P', 'N', 'S'):
            if (fn_8000B508(pObject)) {
                UStreamObject* pOld = fn_8000B70C(pObject->uType, pObject->uId);
                if (pOld != NULL) {
                    if (pObject->uSize == pOld->uSize
                        && fn_80005BC8(pObject->pData, pOld->pData, pObject->uSize) == 0) {
                        fn_80009E70(pObject);
                        break;
                    }
                    fn_8000B588(pOld);
                }
            }
            fn_8000B4B8(pObject);
            // port: the references of later objects are rebased by this object's data address
            gRPNSBase = (uptr)pObject->pData;
            break;
        default:
            for (i = 0; i < gnNumHandlers; i++) {
                if (gHandlers[i].nType == pObject->uType) {
                    gHandlers[i].pfnHandler(pObject);
                    break;
                }
            }
            if (i >= gnNumHandlers) {
                fn_80009E70(pObject);
            }
            break;
        }
        pObject = pNext;
    } while (pObject != NULL);
    return 1;
}

// An object is done with its source buffer: drop the reference and recycle drained buffers.
void UStream_ReleaseObjectBuffer(UStreamBuffer** ppBuffer) {
    UStreamBuffer* p;
    UStreamBuffer* pPrev;
    if (gnCurStream == -1) return;
    (*ppBuffer)->nRefs--;
    pPrev = gpUsedList;
    if (pPrev == NULL) return;
    while ((p = pPrev->pNext) != NULL) {
        if (p->nRefs == 0) {
            pPrev->pNext = p->pNext;
            p->pNext = NULL;
            UStream_ReleaseBuffer(p);
        } else {
            pPrev = p;
        }
    }
    if (gpUsedList->nRefs == 0) {
        p = gpUsedList;
        gpUsedList = p->pNext;
        p->pNext = NULL;
        UStream_ReleaseBuffer(p);
    }
}

// Stop the current stream at the end of what has been read. Returns whether a read is still
// pending (0 when no stream is open).
int UStream_Stop(void) {
    UStream* pStream;
    if (gnCurStream < 0) return 0;
    pStream = &gStreams[gnCurStream];
    pStream->bEOF = 1;
    return gbReadPending ? 1 : 0;
}

// Shut the loader down: no handlers, the current stream closed once no read is pending, the node
// pool deleted.
void UStream_CloseAll(void) {
    UStream* pStream;
    int hFile;
    gnNumHandlers = -1;
    if (gnNumStreams > 0) {
        if (gnCurStream == -1) return;
        pStream = &gStreams[gnCurStream];
        hFile = pStream->hFile;
        pStream->hFile = -1;
        fn_8000D9A4();
        pStream->nFileSize = 0;
        pStream->nFilePos = 0;
        while (gbReadPending) {
            fn_800B7490();
        }
        pStream->hFile = hFile;
        UStream_Close(gnCurStream);
        pStream->hFile = -1;
    }
    if (gpNodePool != NULL) {
        DeleteMemPool(gpNodePool);
        gpNodePool = NULL;
    }
}

// Close stream nStream. Only the most recently opened stream can be closed, and only when
// nothing is in flight. Returns nStream or -1.
int UStream_Close(int nStream) {
    int i;
    if (gnCurStream == -1) return -1;
    if (nStream > gnNumStreams || nStream < 0) return -1;
    if (nStream < gnCurStream) return -1;
    if (gbReadPending) return -1;
    if (gpCurList != NULL && nStream > 0) return -1;
    if (gpUsedList != NULL && nStream > 0) return -1;
    if (fn_8000633C(gStreams[(u32)nStream].hFile) != 0) return -1;
    if (gFill.pObject != NULL) {
        fn_80009E70(gFill.pObject);
        gFill.pObject = NULL;
    }
    gnCurStream--;
    gnNumStreams--;
    i = gStreams[(u32)nStream].nFileIndex;
    if (gStreams[(u32)nStream].params.apfnClosed[i] != NULL) {
        gStreams[(u32)nStream].params.apfnClosed[i](gStreams[(u32)nStream].params.apClosedArg[i]);
    }
    return nStream;
}

// Open a stream from a parameter block. Returns the stream index or a negative error.
int Stream_OpenStreamFiles(const UStreamParams* pParams) {
    int nStream;
    UStream* pStream;
    if (gbReadPending) return -0x68;
    if (gReadyRing[gReadyRingTail] != NULL) return -0x69;
    if (gpCurList != NULL) return -0x67;
    if (gpUsedList != NULL) return -0x66;
    if (gnNumStreams > 0) {
        if (gnNumStreams >= USTREAM_MAX_STREAMS) return -0x65;
        nStream = gnNumStreams;
    } else {
        nStream = 0;
    }
    pStream = &gStreams[nStream];
    pStream->nChunkBudget = -1;
    pStream->nUnkC = -2;
    pStream->nFileIndex = 0;
    Mem_cpy(&pStream->params, pParams, sizeof(UStreamParams));
    UStream_OpenFile(pStream);
    gnCurStream = nStream;
    gnNumStreams++;
    return nStream;
}

// Open a single file by name.
int Stream_OpenStreamFile(const char* pName) {
    UStreamParams params;
    memset(&params, 0, sizeof(params));
    fn_8000E708(&params);
    params.nNumFiles = 1;
    fn_8015A7FC(params.aszName[0], pName);
    return Stream_OpenStreamFiles(&params);
}

// Register (or reference again) the handler for an object type.
int Stream_RegisterLoadChunkCallback(int nType, void (*pfnHandler)(UStreamObject*)) {
    int i;
    int n = gnNumHandlers;
    if (n >= USTREAM_MAX_HANDLERS) return 0;
    for (i = 0; i < n; i++) {
        if (gHandlers[i].nType == nType && gHandlers[i].pfnHandler == pfnHandler) {
            gHandlers[i].nRefs++;
            return 1;
        }
    }
    gHandlers[n].nType = nType;
    gHandlers[n].pfnHandler = pfnHandler;
    gHandlers[n].nRefs = 1;
    gnNumHandlers++;
    return 1;
}

// Drop one reference to a type's handler; remove it when the count reaches zero.
int Stream_UnregisterLoadChunkCallback(int nType) {
    int n = gnNumHandlers;
    int i;
    for (i = 0; i < n; i++) {
        if (gHandlers[i].nType == nType) {
            gHandlers[i].nRefs--;
            if (gHandlers[i].nRefs == 0) {
                int k;
                for (k = i; k < n - 1; k++) {
                    gHandlers[k] = gHandlers[k + 1];
                }
                gnNumHandlers--;
            }
            return 1;
        }
    }
    return 0;
}

// One-time allocation of the 18 read buffers.
void UStream_AllocBuffers(void) {
    gpBufferMemory = fn_800951A0(USTREAM_NUM_BUFFERS * sizeof(UStreamBuffer), 0x40, 1);
    gbAutoRead = 0;
}

void UStream_FreeBuffers(void) {
    if (gpBufferMemory != NULL) {
        fn_8009527C(gpBufferMemory);
        gpBufferMemory = NULL;
    }
}

// Reset all state: rings, lists, the buffer free list, the node pool and the handler table.
void UStream_Init(void) {
    s32 i;
    UStreamBuffer* p;
    UStreamBuffer* pPrev;
    s32 n;
    gbReadPending = 0;
    gpReadBuffer = NULL;
    gReadyRingTail = 0;
    gReadyRingHead = 0;
    gRPNSBase = 0;
    gpQueueHead = NULL;
    gpQueueTail = NULL;
    gSWVRName[0] = 0;
    gFreeRingTail = 0;
    gFreeRingHead = 0;
    for (i = 0; i < USTREAM_NUM_BUFFERS; i++) {
        gReadyRing[i] = NULL;
        gFreeRing[i] = NULL;
    }
    gpNodePool = NULL;
    pPrev = NULL;
    p = (UStreamBuffer*)gpBufferMemory;
    n = USTREAM_NUM_BUFFERS;
    do {
        p->pNext = pPrev;
        p->uPos = 0;
        p->nRefs = 0;
        pPrev = p;
        p++;
    } while (--n);
    gpFreeList = pPrev;
    gpCurList = NULL;
    gpUsedList = NULL;
    gFill.pObject = NULL;
    gpDoneList = NULL;
    gpNodePool = UMemPool_Create(0x200, 8, 1, 0x10);
    gnNumStreams = 0;
    gnNumHandlers = 0;
    for (i = 0; i < USTREAM_MAX_HANDLERS; i++) {
        gHandlers[i].nRefs = 0;
    }
}

// Clears the per-file callbacks of a stream's open parameters.
void fn_8000E708(UStreamParams* p) {
    int i;

    for (i = 0; i < USTREAM_MAX_FILES; i++) {
        p->apfnOpened[i] = NULL;
        p->apfnClosed[i] = NULL;
        p->apOpenedArg[i] = NULL;
        p->apOpenedArg[i] = NULL;   // EA bug: clears apOpenedArg twice; apClosedArg is never cleared
    }
}

// Copies a delivered object's data into pDst (at most uMax bytes) and frees the object. Returns
// the number of bytes copied.
u32 Stream_StreamLoadFixedSize(UStreamObject* pObject, u32 uMax, void* pDst) {
    void* pData;
    u32 uSize;

    uSize = fn_8000E81C(pObject, &pData);
    if (uSize != 0) {
        if (uSize != uMax) {
            uSize = (uSize <= uMax) ? uSize : uMax;
        }
        Mem_cpy(pDst, pObject->pData, uSize);
    }
    fn_80009E70(pObject);
    return uSize;
}

// An object's data and its size.
u32 fn_8000E81C(UStreamObject* pObject, void** ppData) {
    u32 uSize;

    uSize = pObject->uSize;
    *ppData = pObject->pData;
    return uSize;
}
