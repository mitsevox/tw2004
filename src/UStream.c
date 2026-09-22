// UStream.c: EA's streaming asset loader (the CTRL / SHOC / SONO chunk files: .hog, .gcb,
// .ngc). The file name comes from the assert string at 0x801868A0; function and field names
// are ours. CodeWarrior GC/2.5, -O4,p. See docs/formats/ctrl-container.md.
//
// Shape of the system:
//   - a stream is one of the .hog/.gcb files (up to 8 file names per stream, played in order);
//   - the DVD is read asynchronously into 18 fixed 24 KiB buffers that move through rings:
//     free -> in flight -> ready -> current (being parsed) -> free again;
//   - UStream_ParseChunks walks the chunks of the current buffers and builds objects: an SHDR
//     chunk allocates the object, SDAT / Rdat chunks fill it (Rdat through UStream_Decompress);
//   - finished objects are queued and handed to the handler registered for their type
//     (UStream_RegisterHandler), e.g. the texture manager for 'txf '.

#include "game_types.h"

#define USTREAM_BUFFER_SIZE   0x6000
#define USTREAM_NUM_BUFFERS   18
#define USTREAM_MAX_STREAMS   2
#define USTREAM_MAX_HANDLERS  64
#define USTREAM_MAX_FILES     8

#define TAG(a, b, c, d) (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(c) << 8) | (u32)(d))

// One read buffer: 0x40 bytes of bookkeeping followed by 0x6000 bytes of file data.
typedef struct UStreamBuffer {
    struct UStreamBuffer* pNext;   // 0x00  list link
    int  nFileOffset;              // 0x04  where in the file this buffer came from
    u32  uPos;                     // 0x08  parse position inside data[]
    int  nRefs;                    // 0x0C  objects still reading from data[]
    u8   pad[0x30];
    u8   data[USTREAM_BUFFER_SIZE];
} UStreamBuffer;

// What the caller passes to UStream_Open (0x284 bytes) - the file list and per-file callbacks.
typedef struct {
    int   nNumFiles;                                 // 0x000
    char  aszName[USTREAM_MAX_FILES][0x40];          // 0x004
    void  (*apfnOpened[USTREAM_MAX_FILES])(void*);   // 0x204
    void  (*apfnClosed[USTREAM_MAX_FILES])(void*);   // 0x224
    void* apOpenedArg[USTREAM_MAX_FILES];            // 0x244
    void* apClosedArg[USTREAM_MAX_FILES];            // 0x264
} UStreamParams;

// A stream (0x2E0 bytes).
typedef struct {
    int   hFile;             // 0x00  -1 when nothing is open
    int   nFileSize;         // 0x04
    int   nFilePos;          // 0x08  next read offset
    int   nUnkC;             // 0x0C  -2 at open
    u8    bEOF;              // 0x10
    u8    bWaitingForSWVR;   // 0x11
    u8    pad12[2];
    int   nChunkBudget;      // 0x14  -1 = unlimited; counts reads down to EOF
    int   nFileIndex;        // 0x18
    UStreamParams params;    // 0x1C
    u8    pad2A0[0x40];
} UStream;

// An object built from SHOC chunks. The header is 0x34 bytes, then the copied chunk header
// (from SHDR chunk offset 0x14) and, 0x80-aligned, the data.
typedef struct UStreamObject {
    u8*   pData;                  // 0x00
    u32   uUnk4;                  // 0x04
    u32   uUnk8;                  // 0x08
    struct UStreamObject* pPrev;  // 0x0C  finished-object queue
    struct UStreamObject* pNext;  // 0x10
    int   nUnk14;                 // 0x14
    u32   uFlags;                 // 0x18  chunk+0x14; set to 1 for txf / Cpyr / Cact / txf2
    u32   uType;                  // 0x1C  chunk+0x18, e.g. 'ter '
    u32   uHash;                  // 0x20  chunk+0x1C
    u32   uSize;                  // 0x24  chunk+0x20 decompressed size
    u32   uRef28;                 // 0x28  chunk+0x24 } rebased by the RPNS value when the
    u32   uRef2C;                 // 0x2C  chunk+0x28 } object is delivered
    u32   uRef30;                 // 0x30  chunk+0x2C }
    u32   uUnk34;                 // 0x34  chunk+0x30
    u32   uNameLen;               // 0x38  chunk+0x34
    u32   uUnk3C;                 // 0x3C  chunk+0x38
    char  szName[4];              // 0x40  chunk+0x3C
} UStreamObject;

typedef struct {
    u32   uType;
    void  (*pfnHandler)(UStreamObject* pObject);
    int   nRefs;
} UStreamHandler;

typedef struct UStreamNode {
    struct UStreamNode* pNext;
    UStreamObject* pObject;
} UStreamNode;

// ---- state -------------------------------------------------------------------------------
// The arrays are not static: CodeWarrior folds static data into one section symbol and
// addresses it from a single base register, while the original takes each array's address
// separately. The scalars written from the DVD-read callback are volatile (the original
// reloads them after every statement; the main-thread-only ones it does not).

UStream          gStreams[USTREAM_MAX_STREAMS];          // lbl_801A2AD4
UStreamBuffer*   gFreeRing[USTREAM_NUM_BUFFERS];         // lbl_801A3094
UStreamBuffer*   gReadyRing[USTREAM_NUM_BUFFERS];        // lbl_801A30DC
UStreamHandler   gHandlers[USTREAM_MAX_HANDLERS];        // lbl_801A3124
u32              gSoundHeader[5];                        // lbl_801A2AC0 (SONO state)
char             gSWVRName[0x14];                        // lbl_801A3424

static s8   gbPaused;             // lbl_80281C18
static s8   gbAutoRead;           // lbl_80281C19  keep reading after each completed read
static int  gnReadErrors;         // lbl_80281C1C
static UStreamObject* gpQueueTail;   // lbl_80281C20
static UStreamObject* gpQueueHead;   // lbl_80281C24
static volatile s8 gbReadPending;        // lbl_80281C28
static int  gnNumStreams;         // lbl_80281C2C
static int  gnCurStream;          // lbl_80281C30  -1 = none
static void* gpBufferMemory;      // lbl_80281C34
static UStreamNode* gpDoneList;   // lbl_80281C38  objects finished by the parser
static void* gpNodePool;          // lbl_80281C3C
static UStreamObject* gpCurObject;   // lbl_80281C40  object being filled
static u32  gCurObjectPos;        // lbl_80281C44
static UStreamBuffer* gpUsedList; // lbl_80281C48  parsed buffers that objects still reference
static UStreamBuffer* gpCurList;  // lbl_80281C4C  buffers being parsed
static UStreamBuffer* gpFreeList; // lbl_80281C50  released buffers not yet back in the ring
static s8   gFreeRingHead;        // lbl_80281C54
static s8   gReadyRingTail;       // lbl_80281C55
static volatile s8 gFreeRingTail;        // lbl_80281C56
static volatile s8 gReadyRingHead;       // lbl_80281C57
static UStreamBuffer* volatile gpReadBuffer;  // lbl_80281C58  buffer of the read in flight
static u32  gRPNSBase;            // lbl_80281C5C  value of the last RPNS object

int gnNumHandlers = 0;            // lbl_80280DB8 (.sdata)

// ---- externals ---------------------------------------------------------------------------

extern void* fn_80005628(void* pDst, const void* pSrc, u32 uLen);   // memcpy
extern int   fn_80005BC8(const void* pA, const void* pB);           // string/name compare
extern int   fn_800060E0(const char* pName);                        // file open
extern int   fn_8000633C(int hFile);                                // file close
extern int   fn_80006444(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int, int)); // async read
extern u32   fn_800065B0(int hFile);                                // file size
extern void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
extern void  fn_80009E70(void* p);                                  // free
extern void* fn_8000AFA0(u32 uNodeSize, u32 uAlign, int a, int b);  // UMemPool create
extern void  fn_8000B058(void* pPool);                              // UMemPool destroy
extern void* fn_8000B078(void* pPool);                              // UMemPool take node
extern void  fn_8000B0D4(void* pPool, void* pNode);                 // UMemPool return node
extern void  fn_8000B4B8(UStreamObject* p);
extern u8    fn_8000B508(UStreamObject* p);
extern void  fn_8000B588(UStreamObject* p);
extern UStreamObject* fn_8000B70C(u32 uType, u32 uHash);
extern int   fn_8000EA1C(const char* pName, int a, int b, int c);
extern void  fn_8007593C(void* pChunk);                             // MPG2
extern void  fn_800A4BDC(void);
extern void  fn_800A8AD4(void* pChunk);                             // DSPM / VAGM / XADP
extern void* fn_800A8FB4(u32 uSize);
extern void  fn_800A8FFC(void);
extern void* fn_800A925C(u32 uSize, u32 uType);
extern void  fn_800A929C(u32 uType);
extern void* fn_800A9374(u32 uSize);
extern void  fn_800A93AC(void);
extern void  fn_800B044C(void* pDst, const void* pSrc, u32 uLen, void (*pfn)(void), int);
extern void  fn_800B7490(void);                                     // yield / pump
extern void* fn_800951A0(u32 uSize, u32 uAlign, int);
extern void  fn_8009527C(void* p);
extern void  fn_8000E708(UStreamParams* p);
extern void  fn_8015A7FC(char* pDst, const char* pSrc);             // strcpy
extern void* memset(void* p, int c, u32 n);

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
    if (++gFreeRingTail == USTREAM_NUM_BUFFERS) gFreeRingTail = 0;
    gpReadBuffer = pBuffer;
    gpReadBuffer->nFileOffset = pStream->nFilePos;
    gbReadPending = 1;
    if (fn_80006444(pStream->hFile, pBuffer->data, USTREAM_BUFFER_SIZE, pStream->nFilePos, UStream_ReadDone) < 0) {
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
        if (*(u32*)gpReadBuffer->data == TAG('S', 'W', 'V', 'R')) {
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
            if (++gReadyRingHead == USTREAM_NUM_BUFFERS) gReadyRingHead = 0;
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

// Object allocation from an SHDR chunk: header + name + 0x80-aligned data.
static int UStream_BeginObject(UStreamObject** ppObject, u8* pChunk) {
    u32 uExtra;
    int nWanted;
    u32 uPad;
    UStreamObject* pObject;
    if (*(u32*)(pChunk + 0x18) == TAG('C', 's', 'a', 'c')) {
        nWanted = 1;
        uExtra = ((*(int*)(pChunk + 0x38) + 3 + *(int*)(pChunk + 0x34)) & ~3) + 8;
    } else {
        const char* pName = (const char*)(pChunk + 0x3C) + *(int*)(pChunk + 0x34);
        uExtra = 0;
        nWanted = fn_8000EA1C(pName, 0, -1, 0);
        if (nWanted) {
            fn_8000EA1C(pName, 0, -1, 0);
        }
    }
    if (nWanted) {
        if (*(u32*)(pChunk + 0x18) == TAG('t', 'x', 'f', ' ')) *(u32*)(pChunk + 0x14) = 1;
        if (*(u32*)(pChunk + 0x18) == TAG('C', 'p', 'y', 'r')) *(u32*)(pChunk + 0x14) = 1;
        if (*(u32*)(pChunk + 0x18) == TAG('C', 'a', 'c', 't')) *(u32*)(pChunk + 0x14) = 1;
        if (*(u32*)(pChunk + 0x18) == TAG('t', 'x', 'f', '2')) *(u32*)(pChunk + 0x14) = 1;
        uPad = (uExtra + 0x34) & 0x7F;
        uPad = uPad ? 0x80 - uPad : 0;
        {
            u32 uDataSize = *(u32*)(pChunk + 0x20);
            pObject = fn_80009B34((uExtra + uPad) + uDataSize + 0x34, *(u32*)(pChunk + 0x14), 0x80, "UStream.c", 732);
        }
        *ppObject = pObject;
        pObject->nUnk14 = 0;
        ppObject[1] = NULL;
        fn_80005628(&pObject->uFlags, pChunk + 0x14, uExtra + 0x1C);
        pObject->pData = (u8*)pObject + uExtra + uPad + 0x34;
        pObject->uUnk4 = 0;
        pObject->uUnk8 = 0;
        pObject->pPrev = NULL;
        pObject->pNext = NULL;
    }
    return 0;
}

// Finished parsing the current list head: move it to the used list (its data is still referenced).
static void UStream_RetireCurrentBuffer(void) {
    UStreamBuffer* pBuffer;
    UStreamBuffer* p;
    if (gnCurStream == -1) return;
    pBuffer = gpCurList;
    gpCurList = pBuffer->pNext;
    pBuffer->pNext = NULL;
    pBuffer->uPos = 0;
    if (pBuffer->nRefs <= 0) {
        UStream_ReleaseBuffer(pBuffer);
        return;
    }
    if (gpUsedList == NULL) {
        gpUsedList = pBuffer;
        return;
    }
    for (p = gpUsedList; p->pNext != NULL; p = p->pNext) {}
    p->pNext = pBuffer;
}

// An object is complete: put it on the done list for UStream_Update to deliver.
static void UStream_FinishObject(UStreamObject* pObject) {
    UStreamNode* pNode;
    UStreamNode* p;
    if (gnCurStream == -1) return;
    pNode = fn_8000B078(gpNodePool);
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
            if (++gFreeRingHead == USTREAM_NUM_BUFFERS) gFreeRingHead = 0;
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
            if (++gReadyRingTail == USTREAM_NUM_BUFFERS) gReadyRingTail = 0;
            p->pNext = NULL;
            if (gpCurList == NULL) {
                gpCurList = p;
                continue;
            }
            for (q = gpCurList; q->pNext != NULL; q = q->pNext) {}
            q->pNext = p;
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
        if (c >= 'A' && c <= 'Z') c += 0x20;
        if (c == *pMatch) {
            if (pStart == NULL) pStart = pName;
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
    if (pStart != NULL) *pStart = 0;
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
    if (((u32)pDst & (u32)pSrc) & 1) {
        *pDst++ = *pSrc++;
        uCount--;
    }
    if ((((u32)pDst | (u32)pSrc) & 1) == 0) {
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
                u8 t0 = pSrc[0], t1 = pSrc[1], t2 = pSrc[2], t3 = pSrc[3], t4 = pSrc[4], t5 = pSrc[5], t6 = pSrc[6], t7 = pSrc[7];
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
//                       m = 0 forward copy, m = 1 mirrored copy (bytes read backwards)
void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize) {
    u8* pEnd;
    u8* d = (u8*)pDst;
    const u8* s = (const u8*)pSrc;
    int nLen;
    pEnd = d + uSize;
    do {
        u32 uCode;
        uCode = s[0] << 8 | s[1];
        s += 2;
        if ((uCode & 0x8800) != 0x8800) {
            u32 uDist = uCode & 0xFFF;
            const u8* pFrom;
            // NOT YET EXACT (3 instructions): the original keeps this length in r28 before and
            // after the +3 and adds in place. Every plain spelling (nLen += 3 after an if) makes
            // CodeWarrior fold the +3 into each later use instead; the ternary keeps one add but
            // lets the pre-add value sit in a scratch register. Same behaviour either way.
            nLen = (uCode >> 12) & 7;
            nLen = ((nLen == 7) ? (nLen + *s++) : nLen) + 3;
            pFrom = d - uDist;
            if ((uCode & 0x8000) == 0) {
                UStream_Copy(d, pFrom, nLen);
                d += nLen;
            } else {
                // Mirrored copy: the reference is read backwards, so a run of bytes comes out
                // reversed. Eight at a time through temporaries, then the remainder.
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
static void UStream_AddBufferRef(UStreamBuffer** ppList) {
    if (gnCurStream == -1) return;
    (*ppList)->nRefs++;
}

static void UStream_NullCallback(void) {
}

// Walk the chunks of the current buffers. SHOC chunks build objects, SONO chunks build sound
// objects (through the audio module), the stream / movie tags go to their players.
static void UStream_ParseChunks(void) {
    UStreamBuffer* pBuffer;
    u8* pChunk;
    u32 uLen;
    u32 uTag;
    if (UStream_PumpBuffers(1) == NULL) return;
    if (gnCurStream == -1) return;
    pBuffer = gpCurList;
    while (pBuffer != NULL) {
        while (pBuffer->uPos < USTREAM_BUFFER_SIZE) {
            pChunk = pBuffer->data + pBuffer->uPos;
            uTag = *(u32*)pChunk;
            uLen = *(u32*)(pChunk + 4);
            switch (uTag) {
            case TAG('S', 'W', 'V', 'R'):
                *(u32*)&gSWVRName[0] = *(u32*)pChunk;
                *(u32*)&gSWVRName[4] = *(u32*)(pChunk + 4);
                *(u32*)&gSWVRName[8] = *(u32*)(pChunk + 8);
                *(u32*)&gSWVRName[12] = *(u32*)(pChunk + 12);
                *(u32*)&gSWVRName[16] = 0;
                UStream_StripStreamExt(gSWVRName);
                break;
            case TAG('S', 'H', 'O', 'C'):
                if (*(u32*)(pChunk + 0x10) == TAG('S', 'H', 'D', 'R')) {
                    if (UStream_BeginObject(&gpCurObject, pChunk) != 0) return;
                } else if (*(u32*)(pChunk + 0x10) == TAG('S', 'D', 'A', 'T')) {
                    pBuffer->uPos += 0x40;
                    uLen -= 0x40;
                    if (gpCurObject != NULL) {
                        u32 uCopy = uLen;
                        if (gCurObjectPos + uLen > gpCurObject->uSize) uCopy = gpCurObject->uSize - gCurObjectPos;
                        fn_80005628(gpCurObject->pData + gCurObjectPos, pChunk + 0x40, uCopy);
                        gCurObjectPos += uCopy;
                        if (gCurObjectPos == gpCurObject->uSize) {
                            UStream_FinishObject(gpCurObject);
                            gpCurObject = NULL;
                        }
                    }
                } else if (*(u32*)(pChunk + 0x10) == TAG('R', 'd', 'a', 't')) {
                    pBuffer->uPos += 0x40;
                    uLen -= 0x40;
                    if (gpCurObject != NULL) {
                        u32 uUnpacked = *(u32*)(pChunk + 0x40);
                        UStream_Decompress(pChunk + 0x44, gpCurObject->pData + gCurObjectPos, uUnpacked);
                        gCurObjectPos += uUnpacked;
                        if (gCurObjectPos == gpCurObject->uSize) {
                            UStream_FinishObject(gpCurObject);
                            gpCurObject = NULL;
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
                if (*(u32*)(pChunk + 0x10) == TAG('S', 'H', 'D', 'R')) {
                    u32 uKind = *(u32*)(pChunk + 0x18);
                    if (uKind == TAG('s', 'h', 'd', 'r')) {
                        if (*(u32*)(pChunk + 0x1C) == 2) gSoundHeader[0] = (u32)fn_800A9374(*(u32*)(pChunk + 0x20));
                        else gSoundHeader[0] = (u32)fn_800A8FB4(*(u32*)(pChunk + 0x20));
                    } else if (uKind == TAG('s', 'a', 'm', 'p')) {
                        gSoundHeader[0] = (u32)fn_800A925C(*(u32*)(pChunk + 0x20), *(u32*)(pChunk + 0x1C));
                    } else {
                        gSoundHeader[0] = 0;
                    }
                    gSoundHeader[1] = *(u32*)(pChunk + 0x20);
                    gSoundHeader[2] = 0;
                    gSoundHeader[3] = *(u32*)(pChunk + 0x18);
                    gSoundHeader[4] = *(u32*)(pChunk + 0x1C);
                } else if (*(u32*)(pChunk + 0x10) == TAG('S', 'D', 'A', 'T')) {
                    u32 uCopy = uLen - 0x40;
                    if (gSoundHeader[0] != 0) {
                        u8* pDst = (u8*)gSoundHeader[0] + gSoundHeader[2];
                        if (gSoundHeader[2] + uCopy > gSoundHeader[1]) uCopy = gSoundHeader[1] - gSoundHeader[2];
                        if (gSoundHeader[3] == TAG('s', 'h', 'd', 'r')) {
                            fn_80005628(pDst, pChunk + 0x40, uCopy);
                        } else if (gSoundHeader[3] == TAG('s', 'a', 'm', 'p')) {
                            fn_800B044C(pDst, pChunk + 0x40, uCopy, UStream_NullCallback, 0);
                        }
                    }
                    gSoundHeader[2] += uCopy;
                    if (gSoundHeader[2] >= gSoundHeader[1]) {
                        if (gSoundHeader[3] == TAG('s', 'h', 'd', 'r')) {
                            if (gSoundHeader[4] == 2) fn_800A93AC();
                            else fn_800A8FFC();
                        } else if (gSoundHeader[3] == TAG('s', 'a', 'm', 'p')) {
                            fn_800A929C(gSoundHeader[4]);
                        }
                        gSoundHeader[1] = 0;
                        gSoundHeader[2] = 0;
                        gSoundHeader[3] = 0;
                        gSoundHeader[0] = 0;
                    }
                }
                break;
            case TAG('M', 'P', 'G', '2'):
                pBuffer->nRefs++;
                *(UStreamBuffer**)pChunk = pBuffer;
                fn_8007593C(pChunk);
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
        pBuffer = gpCurList;
    }
}

// ---- files ------------------------------------------------------------------------------

static void UStream_Idle(void) {
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
    if (bParse) UStream_ParseChunks();
    if (gnCurStream == -1) return NULL;
    pStream = &gStreams[gnCurStream];
    if (gpCurList == NULL && bParse) UStream_ParseChunks();
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
    fn_8000B0D4(gpNodePool, pNode);
    return pObject;
}

// The per-frame call: pull finished objects, rebase their references, and deliver each to the
// handler registered for its type. Returns 0 when the stream has nothing left to do.
int UStream_Update(void) {
    UStreamObject* pObject;
    UStreamObject* pNext;
    UStream* pStream;
    int i;
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
        fn_800A4BDC();
        if (pStream->bEOF && !gbReadPending && gReadyRing[gReadyRingTail] == NULL && gpCurList == NULL && gpUsedList == NULL) {
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
            if (gpQueueHead != NULL) gpQueueHead->pNext = NULL;
        }
        if (pObject->pPrev != NULL) {
            pObject->pPrev->pNext = pObject->pNext;
        } else {
            gpQueueTail = pObject->pNext;
            if (gpQueueTail != NULL) gpQueueTail->pPrev = NULL;
        }
        pObject->uRef28 += gRPNSBase;
        pObject->uRef2C += gRPNSBase;
        pObject->uRef30 += gRPNSBase;
        if (pObject->uType == TAG('R', 'P', 'N', 'S')) {
            if (fn_8000B508(pObject)) {
                UStreamObject* pOld = fn_8000B70C(pObject->uType, pObject->uHash);
                if (pOld != NULL) {
                    if (pOld->uSize == pObject->uSize && fn_80005BC8(pObject->pData, pOld->pData) == 0) {
                        fn_80009E70(pObject);
                        goto next;
                    }
                    fn_8000B588(pOld);
                }
            }
            fn_8000B4B8(pObject);
            gRPNSBase = *(u32*)pObject->pData;
        } else if (pObject->uType == TAG('C', 'c', 't', 'r')) {
            fn_80009E70(pObject);
        } else {
            for (i = 0; i < gnNumHandlers; i++) {
                if (gHandlers[i].uType == pObject->uType) {
                    gHandlers[i].pfnHandler(pObject);
                    break;
                }
            }
            if (i >= gnNumHandlers) {
                fn_80009E70(pObject);
            }
        }
    next:
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
    for (p = pPrev->pNext; p != NULL; p = pPrev->pNext) {
        if (p->nRefs == 0) {
            pPrev->pNext = p->pNext;
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

// Stop the current stream at the end of what has been read. Returns whether a read is pending.
int UStream_Stop(void) {
    int nStream = gnCurStream;
    if (nStream < 0) nStream = 0;
    gStreams[nStream].bEOF = 1;
    return gbReadPending != 0;
}

// Close everything: the open file, the streams, the node pool.
void UStream_CloseAll(void) {
    UStream* pStream;
    int hFile;
    gnNumHandlers = -1;
    if (gnNumStreams > 0) {
        if (gnCurStream == -1) return;
        pStream = &gStreams[gnCurStream];
        hFile = pStream->hFile;
        pStream->hFile = -1;
        UStream_Idle();
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
        fn_8000B058(gpNodePool);
        gpNodePool = NULL;
    }
}

// Close stream nStream. Only the most recently opened stream can be closed, and only when
// nothing is in flight. Returns nStream or -1.
int UStream_Close(int nStream) {
    UStream* pStream;
    int i;
    if (gnCurStream == -1) return -1;
    if (nStream > gnNumStreams || nStream < 0) return -1;
    if (nStream < gnCurStream) return -1;
    if (gbReadPending) return -1;
    if (gpCurList != NULL && nStream > 0) return -1;
    if (gpUsedList != NULL && nStream > 0) return -1;
    if (fn_8000633C(gStreams[nStream].hFile) != 0) return -1;
    if (gpCurObject != NULL) {
        fn_80009E70(gpCurObject);
        gpCurObject = NULL;
    }
    pStream = &gStreams[nStream];
    gnCurStream--;
    gnNumStreams--;
    i = pStream->nFileIndex;
    if (pStream->params.apfnClosed[i] != NULL) {
        pStream->params.apfnClosed[i](pStream->params.apClosedArg[i]);
    }
    return nStream;
}

// Open a stream from a parameter block. Returns the stream index or a negative error.
int UStream_Open(const UStreamParams* pParams) {
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
    fn_80005628(&pStream->params, pParams, sizeof(UStreamParams));
    UStream_OpenFile(pStream);
    gnCurStream = nStream;
    gnNumStreams++;
    return nStream;
}

// Open a single file by name.
int UStream_OpenFileByName(const char* pName) {
    UStreamParams params;
    memset(&params, 0, sizeof(params));
    fn_8000E708(&params);
    params.nNumFiles = 1;
    fn_8015A7FC(params.aszName[0], pName);
    return UStream_Open(&params);
}

// Register (or reference again) the handler for an object type.
int UStream_RegisterHandler(u32 uType, void (*pfnHandler)(UStreamObject*)) {
    int i;
    int n = gnNumHandlers;
    if (n >= USTREAM_MAX_HANDLERS) return 0;
    for (i = 0; i < n; i++) {
        if (gHandlers[i].uType == uType && gHandlers[i].pfnHandler == pfnHandler) {
            gHandlers[i].nRefs++;
            return 1;
        }
    }
    gHandlers[n].uType = uType;
    gHandlers[n].pfnHandler = pfnHandler;
    gHandlers[n].nRefs = 1;
    gnNumHandlers = n + 1;
    return 1;
}

// Drop one reference to a type's handler; remove it when the count reaches zero.
int UStream_UnregisterHandler(u32 uType) {
    int i;
    int n = gnNumHandlers;
    for (i = 0; i < n; i++) {
        if (gHandlers[i].uType == uType) {
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
    int i;
    UStreamBuffer* p;
    UStreamBuffer* pPrev;
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
    for (i = USTREAM_NUM_BUFFERS; i > 0; i--) {
        p->pNext = pPrev;
        p->uPos = 0;
        p->nRefs = 0;
        pPrev = p;
        p++;
    }
    gpFreeList = pPrev;
    gpCurList = NULL;
    gpUsedList = NULL;
    gpCurObject = NULL;
    gpDoneList = NULL;
    gpNodePool = fn_8000AFA0(0x200, 8, 1, 0x10);
    gnNumStreams = 0;
    gnNumHandlers = 0;
    for (i = 0; i < USTREAM_MAX_HANDLERS; i++) {
        gHandlers[i].nRefs = 0;
    }
}
