// ustream.h (our name): the stream loader's own types (UStream.c). The objects it delivers
// (UStreamObject) and its public calls are in engine.h. The file format is described in
// docs/formats/ctrl-container.md.

#ifndef USTREAM_H
#define USTREAM_H

#include "engine.h"

#define USTREAM_BUFFER_SIZE   0x6000
#define USTREAM_NUM_BUFFERS   18
#define USTREAM_MAX_STREAMS   2
#define USTREAM_MAX_HANDLERS  64
#define USTREAM_MAX_FILES     8

#define TAG(a, b, c, d) (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(c) << 8) | (u32)(d))

// The header of a chunk in a stream file (0x40 bytes). Every chunk starts with its tag, its
// length (header included) and 8 zero bytes. SHOC and SONO chunks go on with a sub-tag; an SHDR's
// fields from uFlags on are the ones UStreamObject keeps from its uFlags on (UStream_BeginObject
// copies them), and an SDAT's data or an Rdat's packed piece follows the header.
typedef struct UStreamChunk {
    u32  uTag;                  // 0x00  'SHOC', 'SONO', 'FILL', 'DSPM', ...
    u32  uLength;               // 0x04  header included
    u8   unk8[8];
    u32  uSubTag;               // 0x10  'SHDR', 'SDAT', 'Rdat'
    u32  uFlags;                // 0x14  SHDR: the object's allocation flags (UStreamObject.uFlags)
    u32  uType;                 // 0x18  SHDR: the object's type ('ter ', 'txf ', ...); SONO: 'shdr' / 'samp'
    u32  uId;                   // 0x1C  SHDR: the object's id (UStreamObject.uId); SONO: which sound memory
                                //       (2: fn_800A9374)
    u32  uSize;                 // 0x20  SHDR: the object's (unpacked) size
    u8   unk24[0x34 - 0x24];
    int  nNameLen;              // 0x34  SHDR: the name's length
    int  n38;                   // 0x38  SHDR of a 'Csac': a second length after the name
    char szName[4];             // 0x3C  SHDR: the name, nNameLen bytes (it runs on past the header)
} UStreamChunk;

// The SONO sound being loaded (0x14 bytes): where its data goes and how far it has got.
typedef struct UStreamSound {
    u8*  pDst;                  // 0x00  the sound memory it is read into; NULL: the data is skipped
    u32  uSize;                 // 0x04
    u32  uPos;                  // 0x08  bytes so far
    u32  uKind;                 // 0x0C  'shdr' or 'samp' (UStreamChunk.uType)
    u32  uMemory;               // 0x10  UStreamChunk.uId: which sound memory
} UStreamSound;

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

// The stream manager's file lists (lbl_80280DF8): streammanagerhole.c and FEgolferanim.c open
// one of them with UStream_Open and keep the stream; LoadData.c empties list 2. Only the part
// the code reaches is known.
typedef struct StreamLists {
    UStreamParams aParams[7];   // 0x000
    int  nStream;               // 0x119C  the open stream (UStream_Open)
} StreamLists;
extern StreamLists* lbl_80280DF8;

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

typedef struct {
    int   nType;             // a 4-character tag; compared signed (cmpw) in the register calls
    void  (*pfnHandler)(UStreamObject* pObject);
    int   nRefs;
} UStreamHandler;

typedef struct UStreamNode {
    struct UStreamNode* pNext;
    UStreamObject* pObject;
} UStreamNode;

// The calls the movie player (LLVideo.c) makes on the loader's buffers.
u8   UStream_Update(void);              // runs the loader once; 0 when nothing is left to read
void UStream_AddBufferRef(UStreamBuffer** ppList);          // a chunk of the buffer is kept
void UStream_ReleaseObjectBuffer(UStreamBuffer** ppBuffer);  // a kept chunk is done with

#endif
