// UStream.c: the stream loader. Reads a stream's files (.hog, .gcb) from the disc into 18 buffers
// of 24 KiB, builds the objects their SHOC chunks hold and hands each to the handler registered for
// its type; SONO chunks fill the sound memory and movie chunks go to the movie player.

#include "game_types.h"
#include "engine.h"
#include "ustream.h"
#include "endian.h"
#include "llvideo.h"
#include "core/startup.h"
#include "core/audtrack.h"

// ---- state -------------------------------------------------------------------------------
// A buffer goes from the free ring to the read in flight, then to the ready ring once read, to the
// current list while its chunks are parsed, and back to the free ring (through gpFreeList), or to
// the used list first while the movie player still holds chunks in it. The chunks are described in
// docs/game/formats/ctrl-container.md. The DVD-read callback (myAsyncReadCallback) writes the
// volatile ones while the main thread runs.

static char             gSWVRName[0x14];                // the stream's name from its SWVR chunk,
                                                        // without ".stream"; nothing reads it afterwards
static UStreamHandler   gHandlers[USTREAM_MAX_HANDLERS];        // by type (Stream_RegisterLoadChunkCallback)
static CDBuf*           gReadyRing[USTREAM_NUM_BUFFERS];        // read, waiting to be parsed
static CDBuf*           gFreeRing[USTREAM_NUM_BUFFERS];         // waiting for a read
static StrmCB           gStreams[USTREAM_MAX_STREAMS];
static UStreamSound     gCurSound;                              // the SONO sound being loaded

static uptr gRPNSBase;            // the data of the last 'RPNS' object delivered: later objects'
                                  // script offsets are from it
static CDBuf* volatile gpReadBuffer;    // the buffer of the read in flight
static volatile s8 gReadyRingHead;      // where the next read buffer goes
static volatile s8 gFreeRingTail;       // the next buffer to read into
static s8   gReadyRingTail;       // the next buffer to parse
static s8   gFreeRingHead;        // where the next released buffer goes
static CDBuf* gpFreeList;         // released buffers, not yet back in the free ring
static CDBuf* gpCurList;          // the buffers being parsed
static CDBuf* gpUsedList;         // parsed buffers the movie player still holds chunks of
static ParseInProg gFill;         // the object being filled and how far
static UMemPool* gpNodePool;      // the TStreamQueue nodes of gpDoneList
static TStreamQueue* gpDoneList;  // objects the parser finished, oldest first
static void* gpBufferMemory;      // the USTREAM_NUM_BUFFERS buffers (UStream_AllocBuffers)
static int  gnCurStream;          // the stream being read; -1: none
static int  gnNumStreams;         // streams open
static volatile s8 gbReadPending; // a read is in flight
static TStreamData* gpQueueHead;  // the oldest object waiting to be handed over
static TStreamData* gpQueueTail;  // the newest
static int  gnReadErrors;         // failed reads in a row; nothing tests it
static u8   gbAutoRead;           // each completed read starts the next (UStream_SetAutoRead)
static s8   gbPaused;             // no read starts while set; nothing sets it

static int gnNumHandlers = -1;    // entries of gHandlers in use; -1 before UStream_Init and after
                                  // UStream_CloseAll

static void myAsyncReadCallback(int bytesRead, int error);
static void _readBufAsync(void);
static u8* UStream_Fill(u8* pDst, u32 value, u32 uCount);
static u8* UStream_Copy(u8* pDst, const u8* pSrc, u32 uCount);
static void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize);
static void fn_8000E708(StreamFileList* p);

// ---- buffers ------------------------------------------------------------------------------

// Gives a buffer back: it goes on the end of the released list, which fillBuffers empties into the
// free ring.
static void addToFreeList(CDBuf* cdp) {
    CDBuf* walkp;
    if (gnCurStream == -1) return;
    cdp->nRefs = 0;
    if (gpFreeList == NULL) {
        gpFreeList = cdp;
        return;
    }
    for (walkp = gpFreeList; walkp->pNext != NULL; walkp = walkp->pNext) {}
    walkp->pNext = cdp;
}

// Starts reading the current stream's next 24 KiB into the next free buffer, unless there is none,
// the file is read to its end or not open. A read that cannot start gives the buffer back.
static void _readBufAsync(void) {
    CDBuf* freeBuf;
    StrmCB* gs;
    if (gbPaused) return;
    if (gnCurStream == -1) return;
    gs = &gStreams[gnCurStream];
    freeBuf = gFreeRing[gFreeRingTail];
    if (freeBuf == NULL) return;
    if (gs->bEOF) return;
    if (gs->hFile < 0) return;
    gFreeRing[gFreeRingTail] = NULL;
    if (++gFreeRingTail == USTREAM_NUM_BUFFERS) {
        gFreeRingTail = 0;
    }
    gpReadBuffer = freeBuf;
    gpReadBuffer->nFileOffset = gs->nFilePos;
    gbReadPending = 1;
    if (File_ReadAsync(gs->hFile, freeBuf->aData, USTREAM_BUFFER_SIZE, gs->nFilePos,
                    myAsyncReadCallback) < 0) {
        gbReadPending = 0;
        gnReadErrors++;
        addToFreeList(gpReadBuffer);
    } else {
        gnReadErrors = 0;
    }
}

// A read finished (the DVD callback). A full buffer, or a short one that ends the file, moves the
// file position on and goes in the ready ring; a failed read, or one from a position the stream
// has since left, gives the buffer back. A buffer that starts with an SWVR chunk (a stream's
// name) ends the reading and is given back too. With gbAutoRead set, the next read starts at
// once.
static void myAsyncReadCallback(int bytesRead, int error) {
    StrmCB* gs;
    int nStatus;
    u8 skipChunk;
    int startWithSWVR;
    if (gnCurStream == -1) return;
    gs = &gStreams[gnCurStream];
    if (gs->hFile < 0) {
        gbReadPending = 0;
        return;
    }
    nStatus = error;
    if (error != 0) {
        nStatus = USTREAM_READ_FAILED;
    } else if (bytesRead != USTREAM_BUFFER_SIZE) {
        nStatus = USTREAM_READ_FAILED;
    }
    if (gs->nFilePos != gpReadBuffer->nFileOffset) {
        nStatus = USTREAM_READ_STALE;
    } else if (gs->nFileSize == bytesRead + gs->nFilePos) {
        gs->bEOF = 1;
        nStatus = 0;
    }
    if (nStatus == 0) {
        gs->nFilePos += bytesRead;
        if (gs->nReadBudget > 0) {
            gs->nReadBudget--;
            if (gs->nReadBudget == 0) {
                gs->nReadBudget = -1;
                gs->bEOF = 1;
            }
        }
        skipChunk = 0;
        startWithSWVR = (BE32(gpReadBuffer->aData) == TAG('S', 'W', 'V', 'R')) ? 1 : 0;
        // the else never runs: bEndAtSWVR is set whenever a file is open
        if (startWithSWVR) {
            if (gs->bEndAtSWVR) {
                gs->bEOF = 1;
                skipChunk = 1;
                addToFreeList(gpReadBuffer);
            } else {
                gs->bEndAtSWVR = 1;
            }
        }
        if (!skipChunk) {
            gReadyRing[gReadyRingHead] = gpReadBuffer;
            if (++gReadyRingHead == USTREAM_NUM_BUFFERS) {
                gReadyRingHead = 0;
            }
        }
        gnReadErrors = 0;
    } else {
        addToFreeList(gpReadBuffer);
        gnReadErrors++;
    }
    gpReadBuffer = NULL;
    gbReadPending = 0;
    if (gbAutoRead && !gbReadPending && !gs->bEOF) {
        _readBufAsync();
    }
}

// The movie player sets it while a movie plays: each read then starts the next as it finishes,
// instead of waiting for fillBuffers.
void UStream_SetAutoRead(u8 bAuto) {
    gbAutoRead = bAuto;
}

// An SHDR chunk starts an object: runs the chunk's first script, which says whether the object is
// wanted, and if it is the second, then allocates the object (its header, the chunk's tail for a
// 'Csac', then its data at USTREAM_DATA_ALIGN) and makes it the one curParseInfo fills. A 'Csac'
// runs no script and is always wanted. Returns 0.
static int loadStreamChunk(ParseInProg* curParseInfo, StreamChunk* cp) {
    u32 uExtra;
    int MustLoad;
    u32 uPad;
    TStreamData* curParseStreamData;
    if (cp->uType == TAG('C', 's', 'a', 'c')) {
        MustLoad = 1;
        uExtra = ((cp->nScriptLen + cp->nKeepLen + 3) & ~3) + 8;
    } else {
        u8* pCode = cp->aScripts;
        u8* pNext = pCode + cp->nScriptLen;

        uExtra = 0;
        MustLoad = fn_8000EA1C(pCode, 0, -1, NULL);
        if (MustLoad) {
            fn_8000EA1C(pNext, 0, -1, NULL);
        }
    }
    if (MustLoad) {
        if (cp->uType == TAG('t', 'x', 'f', ' ')) {
            cp->uFlags = MEMTYPE_HIGH;
        }
        if (cp->uType == TAG('C', 'p', 'y', 'r')) {
            cp->uFlags = MEMTYPE_HIGH;
        }
        if (cp->uType == TAG('C', 'a', 'c', 't')) {
            cp->uFlags = MEMTYPE_HIGH;
        }
        if (cp->uType == TAG('t', 'x', 'f', '2')) {
            cp->uFlags = MEMTYPE_HIGH;
        }
        uPad = (uExtra + sizeof(TStreamData)) & (USTREAM_DATA_ALIGN - 1);
        uPad = uPad ? USTREAM_DATA_ALIGN - uPad : 0;
        curParseStreamData = Circular_MemAllocFunc(sizeof(TStreamData) + uExtra + uPad + cp->uSize,
                                                   cp->uFlags, USTREAM_DATA_ALIGN, "UStream.c", 732);
        curParseInfo->pObject = curParseStreamData;
        curParseStreamData->nReserved = 0;
        curParseInfo->uPos = 0;
        Mem_cpy(&curParseStreamData->uFlags, &cp->uFlags, uExtra + USTREAM_SHDR_KEPT_SIZE);
        curParseStreamData->pData = (u8*)(curParseStreamData + 1) + uExtra + uPad;
        curParseStreamData->uHandlerData = 0;
        curParseStreamData->pfnFree = NULL;
        curParseStreamData->pPrev = NULL;
        curParseStreamData->pNext = NULL;
    }
    return 0;
}

// The buffer at the head of the current list is parsed: it goes on the end of the used list while
// the movie player holds chunks in it, else back to the free list.
static void moveFinishedDataToFreeList(void) {
    CDBuf* cdp;
    CDBuf* walkp;
    if (gnCurStream == -1) return;
    cdp = gpCurList;
    gpCurList = cdp->pNext;
    cdp->pNext = NULL;
    cdp->uPos = 0;
    if (cdp->nRefs > 0) {
        if (gpUsedList == NULL) {
            gpUsedList = cdp;
        } else {
            for (walkp = gpUsedList; walkp->pNext != NULL; walkp = walkp->pNext) {}
            walkp->pNext = cdp;
        }
    } else {
        addToFreeList(cdp);
    }
}

// An object is complete: it goes on the end of the done list, for Stream_GetDataElement to hand
// out. Dropped (never handed over or freed) when the node pool is full.
static void addToDrive3DQueue(TStreamData* tsp) {
    TStreamQueue* newElemPtr;
    TStreamQueue* walkp;
    if (gnCurStream == -1) return;
    newElemPtr = AllocPoolMem(gpNodePool);
    if (newElemPtr == NULL) return;
    newElemPtr->pNext = NULL;
    newElemPtr->pObject = tsp;
    if (gpDoneList == NULL) {
        gpDoneList = newElemPtr;
        return;
    }
    for (walkp = gpDoneList; walkp->pNext != NULL; walkp = walkp->pNext) {}
    walkp->pNext = newElemPtr;
}

// Until the current file is read to its end: moves the released buffers into the free ring and
// starts a read if none is in flight. With lookAtFilledBuffers, the buffers read since go on the
// end of the current list. Returns the current list (NULL: nothing to parse, or no stream).
static CDBuf* fillBuffers(u8 lookAtFilledBuffers) {
    StrmCB* gs;
    if (gnCurStream == -1) return NULL;
    gs = &gStreams[gnCurStream];
    if (!gs->bEOF) {
        while (gpFreeList != NULL) {
            CDBuf* freeBuf = gpFreeList;
            gpFreeList = freeBuf->pNext;
            freeBuf->pNext = NULL;
            gFreeRing[gFreeRingHead] = freeBuf;
            if (++gFreeRingHead == USTREAM_NUM_BUFFERS) {
                gFreeRingHead = 0;
            }
        }
        if (gFreeRing[gFreeRingTail] != NULL && !gbReadPending) {
            _readBufAsync();
        }
    }
    if (lookAtFilledBuffers) {
        CDBuf* filledBuf;
        while ((filledBuf = gReadyRing[gReadyRingTail]) != NULL) {
            CDBuf* walkp;
            gReadyRing[gReadyRingTail] = NULL;
            if (++gReadyRingTail == USTREAM_NUM_BUFFERS) {
                gReadyRingTail = 0;
            }
            filledBuf->pNext = NULL;
            walkp = gpCurList;
            if (walkp == NULL) {
                gpCurList = filledBuf;
            } else {
                while (walkp->pNext != NULL) {
                    walkp = walkp->pNext;
                }
                walkp->pNext = filledBuf;
            }
        }
    }
    return gpCurList;
}

// Cuts the name s off, in place, at the first ".stream" in it (in any case), or at a start of
// ".stream" that runs to its end (".str").
static void Stream_RemoveExtensionInStreamName(char* s) {
    const char* ext = ".stream";
    char* pes = NULL;
    const char* pe = ext;
    char c;
    while ((c = *s) != 0) {
        if (c >= 'A' && c <= 'Z') {
            c += 0x20;
        }
        if (c == *pe) {
            if (pes == NULL) {
                pes = s;
            }
            if (*++pe == 0) {
                *pes = 0;
                return;
            }
        } else {
            pe = ext;
            pes = NULL;
        }
        s++;
    }
    if (pes != NULL) {
        *pes = 0;
    }
}

// Unpacks an 'Rdat' piece from pSrc into pDst, uSize bytes. Two-byte command words, big-endian:
//   1nnn1ooo cccccccc  nnn != 0: fill (c + 3) bytes with the byte at dst - (ooo<<3 | nnn)
//   10001lll llllllll  literal run of l bytes (l = low 11 bits)
//   mlll dddd dddddddd  back-reference: length lll (+ next byte if 7) + 3, distance d;
//                       m clear: forward copy; m set (d's top bit is then clear): mirrored
//                       copy, read backwards from dst - d + 2
static void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize) {
    u8* pEnd;
    u8* d = (u8*)pDst;
    const u8* s = (const u8*)pSrc;
    s32 nLen;
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

// Sets uCount bytes at pDst to value's low byte; returns pDst.
static u8* UStream_Fill(u8* pDst, u32 value, u32 uCount) {
    u8 v = (u8)value;
    u8* p = pDst;
    while (uCount--) {
        *p++ = v;
    }
    return pDst;
}

// Forward copy that may overlap (source before destination); copies halfwords when both
// pointers have the same alignment, eight at a time. The decompressor's back-references. Returns
// pDst.
static u8* UStream_Copy(u8* pDst, const u8* pSrc, u32 uCount) {
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

// The movie player keeps a chunk of buffer *ppBuffer (a VideoChunk's pBuffer): one more
// reference, which holds the buffer on the used list once it is parsed.
void UStream_AddBufferRef(CDBuf** ppBuffer) {
    if (gnCurStream == -1) return;
    (*ppBuffer)->nRefs++;
}

// The DMA-done callback of a SONO sample block's copy to ARAM: nothing to do.
static void UStream_NullCallback(u32 n) {
}

// Parses the buffers read so far, chunk by chunk: SHOC chunks build objects (SHDR starts one,
// SDAT and Rdat fill it), SONO chunks load sound banks, samples and the audio stream file's header,
// movie sound chunks go to the movie sound and MPG2 chunks to the movie player. A FILL chunk ends
// its buffer. Each parsed buffer is retired and the reads kept going.
static void Stream_ParseBufs(void) {
    StreamChunk* cp;
    CDBuf* bp;
    u32 theSize;
    u32 uTag;
    u32 theCopySize;
    ParseInProg* curParseInfo;
    if (fillBuffers(1) == NULL) return;
    if (gnCurStream == -1) return;
    while ((bp = gpCurList) != NULL) {
        // fake match: a volatile signed read of uPos reloads it at the loop's top
        while (*(volatile s32*)&bp->uPos < USTREAM_BUFFER_SIZE) {
            cp = (StreamChunk*)&((u8*)bp->aData)[bp->uPos];
            uTag = cp->uTag;
            theSize = cp->uLength;
            switch (uTag) {
            case TAG('S', 'W', 'V', 'R'):
                // the stream's name: the 16 bytes after the header
                {
                    u32* pDstName = (u32*)gSWVRName;

                    pDstName[0] = ((u32*)(cp + 1))[0];
                    pDstName[1] = ((u32*)(cp + 1))[1];
                    pDstName[2] = ((u32*)(cp + 1))[2];
                    pDstName[3] = ((u32*)(cp + 1))[3];
                    pDstName[4] = 0;
                }
                Stream_RemoveExtensionInStreamName(gSWVRName);
                break;
            case TAG('S', 'H', 'O', 'C'):
                if (cp->uSubTag == TAG('S', 'H', 'D', 'R')) {
                    if (loadStreamChunk(&gFill, cp) != 0) return;
                } else if (cp->uSubTag == TAG('S', 'D', 'A', 'T')) {
                    theSize = cp->uLength - sizeof(StreamChunk);
                    bp->uPos += sizeof(StreamChunk);
                    curParseInfo = &gFill;
                    if (curParseInfo->pObject != NULL) {
                        theCopySize = theSize;
                        if ((int)(curParseInfo->uPos + theSize) > (int)curParseInfo->pObject->uSize) {
                            theCopySize = curParseInfo->pObject->uSize - curParseInfo->uPos;
                        }
                        Mem_cpy(curParseInfo->pObject->pData + curParseInfo->uPos, (u8*)(cp + 1),
                                theCopySize);
                        curParseInfo->uPos += theCopySize;
                        if (curParseInfo->uPos == curParseInfo->pObject->uSize) {
                            addToDrive3DQueue(curParseInfo->pObject);
                            curParseInfo->pObject = NULL;
                        }
                    }
                } else if (cp->uSubTag == TAG('R', 'd', 'a', 't')) {
                    theSize = cp->uLength - sizeof(StreamChunk);
                    bp->uPos += sizeof(StreamChunk);
                    curParseInfo = &gFill;
                    if (curParseInfo->pObject != NULL) {
                        // the piece's unpacked size, then the packed bytes
                        u32 uncompressedSize = BE32(cp + 1);
                        UStream_Decompress((u32*)(cp + 1) + 1,
                                           curParseInfo->pObject->pData + curParseInfo->uPos,
                                           uncompressedSize);
                        curParseInfo->uPos += uncompressedSize;
                        if (curParseInfo->uPos == curParseInfo->pObject->uSize) {
                            addToDrive3DQueue(curParseInfo->pObject);
                            curParseInfo->pObject = NULL;
                        }
                    }
                }
                break;
            case TAG('F', 'I', 'L', 'L'):
                bp->uPos = USTREAM_BUFFER_SIZE;
                continue;
            case TAG('D', 'S', 'P', 'M'):
            case TAG('V', 'A', 'G', 'M'):
            case TAG('X', 'A', 'D', 'P'):
                Mov_SendSoundBlock((MovieSoundBlock*)cp);
                break;
            case TAG('S', 'O', 'N', 'O'):
                if (cp->uSubTag == TAG('S', 'H', 'D', 'R')) {
                    switch (cp->uType) {
                    case TAG('s', 'h', 'd', 'r'):
                        if (cp->uId == USTREAM_SONO_STREAM_HDR) {
                            gCurSound.pDst = (u8*)Ses_AllocStreamFileHdr(cp->uSize);
                        } else {
                            gCurSound.pDst = Ses_AllocBankHdr(cp->uSize, cp->uId);
                        }
                        break;
                    case TAG('s', 'a', 'm', 'p'):
                        // the sample block's ARAM address
                        gCurSound.pDst = (u8*)(uptr)Ses_AllocSampleAram(cp->uSize, cp->uId);
                        break;
                    default:
                        gCurSound.pDst = NULL;
                        break;
                    }
                    gCurSound.uSize = cp->uSize;
                    gCurSound.uPos = 0;
                    gCurSound.uKind = cp->uType;
                    gCurSound.uMemory = cp->uId;
                } else if (cp->uSubTag == TAG('S', 'D', 'A', 'T')) {
                    u32 theCopySize = cp->uLength - sizeof(StreamChunk);
                    if (gCurSound.pDst != NULL) {
                        u8* dataStart = (u8*)(cp + 1);
                        u8* dest = gCurSound.pDst + gCurSound.uPos;
                        if (gCurSound.uPos + theCopySize > gCurSound.uSize) {
                            theCopySize = gCurSound.uSize - gCurSound.uPos;
                        }
                        switch (gCurSound.uKind) {
                        case TAG('s', 'h', 'd', 'r'):
                            Mem_cpy(dest, dataStart, theCopySize);
                            break;
                        case TAG('s', 'a', 'm', 'p'):
                            AudDma_ToAram((u32)(uptr)dest, dataStart, theCopySize, UStream_NullCallback, 0);
                            break;
                        }
                    }
                    gCurSound.uPos += theCopySize;
                    if (gCurSound.uPos >= gCurSound.uSize) {
                        switch (gCurSound.uKind) {
                        case TAG('s', 'h', 'd', 'r'):
                            if (gCurSound.uMemory == USTREAM_SONO_STREAM_HDR) {
                                Ses_ProcessStreamFileHdr();
                            } else {
                                Ses_ProcessArticulationData(gCurSound.uMemory);
                            }
                            break;
                        case TAG('s', 'a', 'm', 'p'):
                            Ses_ProcessSampleData(gCurSound.uMemory);
                            break;
                        }
                        gCurSound.uSize = 0;
                        gCurSound.uPos = 0;
                        gCurSound.uKind = 0;
                        gCurSound.pDst = NULL;
                    }
                }
                break;
            case TAG('M', 'P', 'G', '2'):
                bp->nRefs++;
                // the chunk's tag word now holds its buffer
                ((VideoChunk*)cp)->pBuffer = bp;
                LLVideo_HandleChunk((VideoChunk*)cp);
                break;
            case TAG('C', 'T', 'R', 'L'):
            case TAG('P', 'A', 'D', 'D'):
            default:
                break;
            }
            bp->uPos += theSize;
        }
        moveFinishedDataToFreeList();
        fillBuffers(0);
    }
}

// ---- files ------------------------------------------------------------------------------

// Empty in this build: UStream_CloseAll calls it when it shuts the loader down, once it has marked
// the current stream's file closed.
static void UStream_OnShutdown(void) {
}

// Opens the stream's current file (StreamFileList entry nFileIndex), reads it from the start, and
// calls the entry's apfnOpened.
static void Stream_OpenFileForStreaming(StrmCB* gs) {
    int i;
    gs->hFile = File_Open(gs->params.aszName[gs->nFileIndex]);
    gs->nFileSize = File_GetLength(gs->hFile);
    gs->nFilePos = 0;
    gs->bEOF = 0;
    gs->bEndAtSWVR = 1;
    i = gs->nFileIndex;
    if (gs->params.apfnOpened[i] != NULL) {
        gs->params.apfnOpened[i](gs->params.apOpenedArg[i]);
    }
}

// Closes the stream's current file, calls its entry's apfnClosed, and opens the next file.
static void Stream_OpenNextStreamFile(StrmCB* gs) {
    int i;
    File_Close(gs->hFile);
    i = gs->nFileIndex;
    if (gs->params.apfnClosed[i] != NULL) {
        gs->params.apfnClosed[i](gs->params.apClosedArg[i]);
    }
    gs->nFileIndex++;
    Stream_OpenFileForStreaming(gs);
}

// With tryToLoadData, parses what has been read. When nothing is left to parse and the current
// file is read to its end, moves on to the stream's next file. Returns the oldest finished object
// (taken off the done list), or NULL.
static TStreamData* Stream_GetDataElement(u8 tryToLoadData) {
    StrmCB* gs;
    TStreamQueue* qp;
    TStreamData* theStream;
    if (tryToLoadData) {
        Stream_ParseBufs();
    }
    if (gnCurStream == -1) return NULL;
    gs = &gStreams[gnCurStream];
    if (gpCurList == NULL && tryToLoadData) {
        Stream_ParseBufs();
    }
    if (gpCurList == NULL) {
        if (fillBuffers(1) == NULL && gs->bEOF) {
            if (gs->nFileIndex + 1 < gs->params.nNumFiles) {
                Stream_OpenNextStreamFile(gs);
            }
        }
    }
    qp = gpDoneList;
    if (qp == NULL) return NULL;
    theStream = qp->pObject;
    gpDoneList = qp->pNext;
    ReturnPoolMem(gpNodePool, qp);
    return theStream;
}

// Runs the loader once (the stream manager, the movie player and the front-end golfer loop on it):
// waits out a disc error, parses what has been read, then hands the finished objects over, oldest
// first. Each object's script offsets become pointers into the last 'RPNS' object's data. A 'Cctr'
// object is freed; an 'RPNS' object joins the resources (replacing an older one of its type and id,
// or freed when it holds the same data); any other goes to its type's handler, or is freed when
// none is registered. With nothing to hand over and a stream open it runs the audio's cycle
// instead. Returns 0 when no stream is open, or the stream is read to its end with nothing left
// in flight, to parse or held by the movie player; else 1.
u8 Stream_Progress(void) {
    TStreamData* Data;
    int i;
    TStreamData* nextData;
    StrmCB* gs;
    DiscError_WaitForDrive();
    for (Data = Stream_GetDataElement(1); Data != NULL; Data = Stream_GetDataElement(0)) {
        Data->pPrev = NULL;
        Data->pNext = NULL;
        if (gpQueueTail != NULL) {
            gpQueueTail->pPrev = Data;
            Data->pNext = gpQueueTail;
            gpQueueTail = Data;
        } else {
            gpQueueHead = Data;
            gpQueueTail = Data;
        }
    }
    Data = gpQueueHead;
    if (Data == NULL) {
        if (gnCurStream == -1) return 0;
        gs = &gStreams[gnCurStream];
        (void)*(volatile StrmCB*)gs;   // fake match: sequence pointer evaluation before Gaud_Cycle
        Gaud_Cycle();
        if (gs->bEOF && !gbReadPending && gReadyRing[gReadyRingTail] == NULL && gpCurList == NULL
            && gpUsedList == NULL) {
            return 0;
        }
        return 1;
    }
    do {
        nextData = Data->pPrev;
        if (Data->pNext != NULL) {
            Data->pNext->pPrev = Data->pPrev;
        } else {
            gpQueueHead = Data->pPrev;
            if (gpQueueHead != NULL) {
                gpQueueHead->pNext = NULL;
            }
        }
        if (Data->pPrev != NULL) {
            Data->pPrev->pNext = Data->pNext;
        } else {
            gpQueueTail = Data->pNext;
            if (gpQueueTail != NULL) {
                gpQueueTail->pPrev = NULL;
            }
        }
        Data->aScriptRefs[0] += gRPNSBase;
        Data->aScriptRefs[1] += gRPNSBase;
        Data->aScriptRefs[2] += gRPNSBase;
        switch ((int)Data->uType) {
        case TAG('C', 'c', 't', 'r'):
            Circular_MemFreeFunc(Data);
            break;
        case TAG('R', 'P', 'N', 'S'):
            if (Resources_IsAlreadyThere(Data)) {
                TStreamData* tempData = Resources_Get(Data->uType, Data->uId);
                if (tempData != NULL) {
                    if (Data->uSize == tempData->uSize
                        && Mem_compare(Data->pData, tempData->pData, Data->uSize) == 0) {
                        Circular_MemFreeFunc(Data);
                        break;
                    }
                    Resources_Del(tempData);
                }
            }
            Resources_Add(Data);
            // the script offsets of the objects after it are from its data
            gRPNSBase = (uptr)Data->pData;
            break;
        default:
            for (i = 0; i < gnNumHandlers; i++) {
                if (gHandlers[i].nType == Data->uType) {
                    gHandlers[i].pfnHandler(Data);
                    break;
                }
            }
            if (i >= gnNumHandlers) {
                Circular_MemFreeFunc(Data);
            }
            break;
        }
        Data = nextData;
    } while (Data != NULL);
    return 1;
}

// An object is done with its source buffer: drop the reference and recycle drained buffers.
void UStream_ReleaseObjectBuffer(CDBuf** ppBuffer) {
    CDBuf* p;
    CDBuf* pPrev;
    if (gnCurStream == -1) return;
    (*ppBuffer)->nRefs--;
    pPrev = gpUsedList;
    if (pPrev == NULL) return;
    while ((p = pPrev->pNext) != NULL) {
        if (p->nRefs == 0) {
            pPrev->pNext = p->pNext;
            p->pNext = NULL;
            addToFreeList(p);
        } else {
            pPrev = p;
        }
    }
    if (gpUsedList->nRefs == 0) {
        p = gpUsedList;
        gpUsedList = p->pNext;
        p->pNext = NULL;
        addToFreeList(p);
    }
}

// Stop the current stream at the end of what has been read. Returns whether a read is still
// pending (0 when no stream is open).
int UStream_Stop(void) {
    StrmCB* pStream;
    if (gnCurStream < 0) return 0;
    pStream = &gStreams[gnCurStream];
    pStream->bEOF = 1;
    return gbReadPending ? 1 : 0;
}

// Shut the loader down: no handlers, the current stream closed once no read is pending, the node
// pool deleted.
void UStream_CloseAll(void) {
    StrmCB* pStream;
    int hFile;
    gnNumHandlers = -1;
    if (gnNumStreams > 0) {
        if (gnCurStream == -1) return;
        pStream = &gStreams[gnCurStream];
        hFile = pStream->hFile;
        pStream->hFile = -1;
        UStream_OnShutdown();
        pStream->nFileSize = 0;
        pStream->nFilePos = 0;
        while (gbReadPending) {
            DiscError_WaitForDrive();
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
    if (File_Close(gStreams[(u32)nStream].hFile) != 0) return -1;
    if (gFill.pObject != NULL) {
        Circular_MemFreeFunc(gFill.pObject);
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
int Stream_OpenStreamFiles(const StreamFileList* pParams) {
    int nStream;
    StrmCB* pStream;
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
    pStream->nReadBudget = -1;
    pStream->nUnkC = -2;
    pStream->nFileIndex = 0;
    Mem_cpy(&pStream->params, pParams, sizeof(StreamFileList));
    Stream_OpenFileForStreaming(pStream);
    gnCurStream = nStream;
    gnNumStreams++;
    return nStream;
}

// Open a single file by name.
int Stream_OpenStreamFile(const char* pName) {
    StreamFileList params;
    memset(&params, 0, sizeof(params));
    fn_8000E708(&params);
    params.nNumFiles = 1;
    strcpy(params.aszName[0], pName);
    return Stream_OpenStreamFiles(&params);
}

// Register (or reference again) the handler for an object type.
int Stream_RegisterLoadChunkCallback(int nType, void (*pfnHandler)(TStreamData*)) {
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
    gpBufferMemory = Mem_AllocFunc(USTREAM_NUM_BUFFERS * sizeof(CDBuf), 0x40, 1);
    gbAutoRead = 0;
}

void UStream_FreeBuffers(void) {
    if (gpBufferMemory != NULL) {
        Mem_FreeFunc(gpBufferMemory);
        gpBufferMemory = NULL;
    }
}

// Reset all state: rings, lists, the buffer free list, the node pool and the handler table.
void UStream_Init(void) {
    s32 i;
    CDBuf* p;
    CDBuf* pPrev;
    s32 n;
    gbReadPending = 0;
    gpReadBuffer = NULL;
    // section note: a chain, as .sbss is laid out last-defined-first; two statements store in the
    // other order
    gReadyRingHead = gReadyRingTail = 0;
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
    p = (CDBuf*)gpBufferMemory;
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
    gpNodePool = CreateMemPool(0x200, 8, MEMTYPE_HIGH, 0x10);
    gnNumStreams = 0;
    gnNumHandlers = 0;
    for (i = 0; i < USTREAM_MAX_HANDLERS; i++) {
        gHandlers[i].nRefs = 0;
    }
}

// Clears the per-file callbacks of a stream's open parameters.
static void fn_8000E708(StreamFileList* p) {
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
u32 Stream_StreamLoadFixedSize(TStreamData* pObject, u32 uMax, void* pDst) {
    void* pData;
    u32 uSize;

    uSize = fn_8000E81C(pObject, &pData);
    if (uSize != 0) {
        if (uSize != uMax) {
            uSize = (uSize <= uMax) ? uSize : uMax;
        }
        Mem_cpy(pDst, pObject->pData, uSize);
    }
    Circular_MemFreeFunc(pObject);
    return uSize;
}

// An object's data and its size.
u32 fn_8000E81C(TStreamData* pObject, void** ppData) {
    u32 uSize;

    uSize = pObject->uSize;
    *ppData = pObject->pData;
    return uSize;
}
