// ustream.h: the stream loader's own types and calls (UStream.c). The objects it delivers
// (TStreamData) and the calls their handlers make (register, unregister, load) are in engine.h;
// the rest are declared here, with the stream manager's file lists and calls (StreamManager.c).
// The file format is described in docs/game/formats/ctrl-container.md.

#ifndef USTREAM_H
#define USTREAM_H

#include "engine.h"

#define USTREAM_BUFFER_SIZE   0x6000
#define USTREAM_NUM_BUFFERS   18
#define USTREAM_MAX_STREAMS   2
#define USTREAM_MAX_HANDLERS  64
#define USTREAM_MAX_FILES     8
#define USTREAM_DATA_ALIGN    0x80      // an object's data starts on this boundary
#define USTREAM_SHDR_KEPT_SIZE 0x1C     // the SHDR fields every object keeps: StreamChunk.uFlags on,
                                        // TStreamData.uFlags to its end
#define USTREAM_SONO_STREAM_HDR 2       // a SONO 'shdr' with this id is the audio stream file's
                                        // header; any other id is a bank (Ses_AllocBankHdr)

// myAsyncReadCallback's verdict on a read other than 0 (kept)
#define USTREAM_READ_FAILED   5         // an error, or short of a buffer: never produced, as the
                                        // reader always reports a full buffer and no error
#define USTREAM_READ_STALE    0xDEAD    // read from a position the stream has since left

// Stream_OpenStreamFiles's answers when it cannot open a stream
#define USTREAM_ERR_TOO_MANY_STREAMS  (-0x65)   // USTREAM_MAX_STREAMS are open
#define USTREAM_ERR_BUFFERS_HELD      (-0x66)   // the movie player still holds buffers (gpUsedList)
#define USTREAM_ERR_PARSING           (-0x67)   // buffers are being parsed (gpCurList)
#define USTREAM_ERR_READ_PENDING      (-0x68)   // a read is in flight
#define USTREAM_ERR_READ_WAITING      (-0x69)   // read buffers wait to be parsed (gReadyRing)

#define USTREAM_NUM_QUEUE_NODES 0x200   // finished objects waiting at once (gpDoneList's node pool)

#define TAG(a, b, c, d) (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(c) << 8) | (u32)(d))

// The header of a chunk in a stream file (0x40 bytes). Every chunk starts with its tag, its
// length (header included) and 8 zero bytes. SHOC and SONO chunks go on with a sub-tag; an SHDR's
// fields from uFlags on are the ones TStreamData keeps from its uFlags on (loadStreamChunk
// copies them), and an SDAT's data or an Rdat's packed piece follows the header.
typedef struct StreamChunkS {
    u32  uTag;                  // 0x00  'SHOC', 'SONO', 'FILL', 'DSPM', ...
    u32  uLength;               // 0x04  header included
    u8   unk8[8];
    u32  uSubTag;               // 0x10  'SHDR', 'SDAT', 'Rdat'
    u32  uFlags;                // 0x14  SHDR: where the object's block comes from (a TTypeAcces;
                                //       TStreamData.uFlags)
    u32  uType;                 // 0x18  SHDR: the object's type ('ter ', 'txf ', ...); SONO: 'shdr' / 'samp'
    u32  uId;                   // 0x1C  SHDR: the object's id (TStreamData.uId); SONO: which sound memory
                                //       (USTREAM_SONO_STREAM_HDR: Ses_AllocStreamFileHdr)
    u32  uSize;                 // 0x20  SHDR: the object's (unpacked) size
    u32  aScriptRefs[3];        // 0x24  SHDR: TStreamData.aScriptRefs
    u8   unk30[4];
    int  nScriptLen;            // 0x34  SHDR: the first script's length; a 'Csac' runs no scripts
                                //       and keeps nScriptLen + nKeepLen bytes of the header's tail
    int  nKeepLen;              // 0x38  SHDR of a 'Csac': the rest of the tail it keeps
    u8   aScripts[4];           // 0x3C  SHDR: two LLTex scripts (RPN_RunScript), the first nScriptLen
                                //       bytes long (they run on past the header)
} StreamChunk;

// The SONO sound being loaded (0x14 bytes): where its data goes and how far it has got.
typedef struct UStreamSound {
    u8*  pDst;                  // 0x00  the sound memory it is read into; NULL: the data is skipped
    u32  uSize;                 // 0x04
    u32  uPos;                  // 0x08  bytes so far
    u32  uKind;                 // 0x0C  'shdr' or 'samp' (StreamChunk.uType)
    u32  uMemory;               // 0x10  StreamChunk.uId: which sound memory
} UStreamSound;

// One read buffer: 0x40 bytes of bookkeeping followed by 0x6000 bytes of file data.
typedef struct CDBufS {
    struct CDBufS* pNext;          // 0x00  list link
    int  nFileOffset;              // 0x04  where in the file this buffer came from
    u32  uPos;                     // 0x08  parse position inside aData
    int  nRefs;                    // 0x0C  chunks of aData the movie player holds
    u8   pad[0x30];
    u8   aData[USTREAM_BUFFER_SIZE];
} CDBuf;
LAYOUT_ASSERT(CDBuf, 0x6040);

// What UStream calls when it opens a stream's file and when it is done with it, with that file's
// apOpenedArg / apClosedArg.
typedef void (*StreamBeginCallback)(void* pData);
typedef void (*StreamEndCallback)(void* pData);

// What the caller passes to Stream_OpenStreamFiles (0x284 bytes) - the file list and per-file callbacks.
typedef struct StreamFileList {
    int   nNumFiles;                                 // 0x000
    char  aszName[USTREAM_MAX_FILES][0x40];          // 0x004
    StreamBeginCallback apfnOpened[USTREAM_MAX_FILES];   // 0x204
    StreamEndCallback   apfnClosed[USTREAM_MAX_FILES];   // 0x224
    void* apOpenedArg[USTREAM_MAX_FILES];            // 0x244
    void* apClosedArg[USTREAM_MAX_FILES];            // 0x264
} StreamFileList;
LAYOUT_ASSERT(StreamFileList, 0x284);

int  Stream_OpenStreamFiles(const StreamFileList* streamFileList);  // its index, or USTREAM_ERR_
int  Stream_OpenStreamFile(const char* streamFileName);    // a stream of that one file
int  Stream_CloseStreamFile(int whichFile);                 // whichFile, or -1
int  Stream_StopLoading(void);          // no more reads; 1 while one is still in flight
void Stream_InitOnce(void);             // start-up: the read buffers
void Stream_CloseOnce(void);
void Stream_InitModule(void);           // the main loop's start: the loader's state
void Stream_CloseModule(void);          // the main loop's end

// ---- the stream manager (StreamManager.c) -----------------------------------------------------

// The kinds of stream the stream manager keeps a file list for (StreamLists.aParams).
typedef enum StreamFileType {
    STREAM_FILE_INGAME,         // the round: GlbData.gcb, GlbChar.gcb, the golfers' character
                                // files, then their sac files
    STREAM_FILE_FRONTEND,       // the front end: FEnd.gcb, FEChar.gcb
    STREAM_FILE_LOADSCREEN,     // a loading screen's picture: Load<n>.gcb
    STREAM_FILE_FECHAR,         // the menu golfer being loaded: his FEChars file
    STREAM_FILE_GLOBALS,        // the global data: LoadOnce.gcb
    STREAM_FILE_STARTUP,        // the start-up: LoadOnce.gcb and startup.gcb
    STREAM_FILE_HOLE,           // a hole: data/<course>/<hole>/hole.hog
    STREAM_NUM_FILE_TYPES
} StreamFileType;

// The stream manager's file lists (gpStreamManagerLists, 0x11A0 bytes) and the stream open on one
// of them. FEgolferanim.c opens and closes the FE character list itself.
typedef struct StreamLists {
    StreamFileList aParams[STREAM_NUM_FILE_TYPES];  // 0x000  by StreamFileType
    int  nStream;               // 0x119C  the open stream (Stream_OpenStreamFiles)
} StreamLists;
LAYOUT_ASSERT(StreamLists, 0x11A0);
extern StreamLists* gpStreamManagerLists;

// The file list of stream kind eType (a StreamFileType).
#define STREAM_LIST(eType) (gpStreamManagerLists->aParams[eType])

// The file names the lists are made of; the Fmt ones go through sprintf.
extern char gszStreamFrontendFile[];        // "data/FEnd/FEnd.gcb"
extern char gszStreamFECharFile[];          // "FEChar.gcb"
extern char gszStreamLoadOnceFile[];        // "LoadOnce.gcb"
extern char gszStreamStartupFile[];         // "startup.gcb"
extern char gszStreamLoadScreenFileFmt[];   // "data/Load/Load%d.gcb"
extern char gszStreamCourseDirFmt[];        // "data/%s/"
extern char gszStreamHoleFileName[];        // "/hole.hog"
extern char gszStreamGlbDataFile[];         // "GlbData.gcb"
extern char gszStreamGlbCharFile[];         // "GlbChar.gcb"
extern char gszStreamCharFileFmt[];         // "data/Chars/%02dchar.gcb"
extern char gszStreamMaleSacFile[];         // "malesac.gcb"
extern char gszStreamFemaleSacFile[];       // "femsac.gcb"
extern char gszStreamCharSacFileFmt[];      // "data/CharSac/%02dchrsac.gcb"
extern char gszStreamFECharFileFmt[];       // "data/FEChars/%02dcharfe.gcb"

// One flag per character, which the list set-ups clear (the first STREAM_NUM_CHAR_ADDED of
// them) and nothing reads.
#define STREAM_NUM_CHAR_ADDED 30
extern u8   gStreamManagerCharAdded[0x38];

void StreamManager_InitOnce(void);
void StreamManager_CloseOnce(void);
void StreamManager_AddLoadScreenFile(int nFile);
void StreamManagerIngame_RegisterStreamClients(void);
void StreamManagerIngame_UnregisterStreamClients(void);
void StreamManagerIngame_StreamFiles(void);
void StreamManagerFrontend_RegisterStreamClients(void);
void StreamManagerFrontend_UnregisterStreamClients(void);
void StreamManagerFrontend_StreamFiles(void);
void StreamManagerLoadScreen_StreamFiles(void);
void StreamManagerStartup_RegisterStreamClients(void);
void StreamManagerStartup_UnregisterStreamClients(void);
void StreamManagerStartup_StreamFiles(void);
void StreamManagerGlobals_RegisterStreamClients(void);
void StreamManagerGlobals_UnregisterStreamClients(void);
void StreamManagerGlobals_StreamFiles(void);
void StreamManagerHole_RegisterStreamClients(void);
void StreamManagerHole_UnregisterStreamClients(void);
void StreamManagerHole_StreamFiles(void);
void StreamManagerIngame_SetupFileStream(void);
void StreamManagerIngame_SetupSacFiles(void);
void StreamManagerIngame_SetupCurSlotSacFiles(void);
void StreamManagerIngame_StreamSacFiles(void);
void StreamManagerFEChar_SetupFileStream(s32 nGolferId, s32 nIndex);

// A stream (0x2E0 bytes).
typedef struct StrmCBS {
    int   hFile;             // 0x00  -1 when nothing is open
    int   nFileSize;         // 0x04
    int   nFilePos;          // 0x08  next read offset
    int   nUnread;           // 0x0C  -2 at open; nothing reads it
    u8    bEOF;              // 0x10  the file is read to its end (or reading was stopped)
    u8    bEndAtSWVR;        // 0x11  a buffer starting with an SWVR chunk ends the reading; set
                             //       whenever a file is open
    u8    pad12[2];
    int   nReadBudget;       // 0x14  -1: no limit; else buffers left to read before bEOF (nothing
                             //       in this build sets one)
    int   nFileIndex;        // 0x18
    StreamFileList params;   // 0x1C
    u8    pad2A0[0x40];
} StrmCB;
LAYOUT_ASSERT(StrmCB, 0x2E0);

// A handler table entry (gHandlers, Stream_RegisterLoadChunkCallback).
typedef struct {
    int   nType;             // the objects' type, a 4-character tag (TStreamData.uType)
    LoadChunkCallbackPtr pfnHandler;   // what Stream_Progress hands each such object to
    int   nRefs;             // registrations of this pair not yet taken back
} UStreamHandler;

typedef struct TStreamQueueS {
    struct TStreamQueueS* pNext;
    TStreamData* pObject;
} TStreamQueue;

// The object the parser is filling and how much of its data has arrived (loadStreamChunk sets
// both; SDAT and Rdat chunks fill it).
typedef struct ParseInProgS {
    TStreamData* pObject;
    u32 uPos;
} ParseInProg;

// Running the loader (the stream manager, the front end and the movie player call it), and the
// calls the movie player (LLVideo.c) makes on the loader's buffers.
u8   Stream_Progress(void);             // runs the loader once; 0 when nothing is left to read
void UStream_AddBufferRef(CDBuf** ppBuffer);                // a chunk of the buffer is kept
void UStream_SetAutoRead(u8 bAuto);     // set while a movie plays: each read starts the next
void UStream_ReleaseBufferRef(CDBuf** ppBuffer);            // a kept chunk is done with

#endif
