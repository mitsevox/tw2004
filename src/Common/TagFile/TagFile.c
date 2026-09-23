// EA shared file library, SN ProDG (GCC 2.95) without optimization.
// Original path: ../../../Source/Common/TagFile/TagFile.c
// Line numbers are reproduced with #line so the assert calls match.
//
// A tag file is the save-data container: a sequence of records, each a 12-byte header
// (tag, size, checksum) followed by the payload, padded to the device block size. An unused
// header slot holds TAG_SENTINEL. Every record is CRC32-checked and XOR-ciphered through the
// interfaces exposed by ChecksumCRC32.c and CipherXOR.c. A map of the records (tag, offset,
// size, checksum) is kept in memory and its checksum is written into the session at TagFile_End.

#include "Common/SharedFileIO.h"

#define TAG_BUFFERSIZE 12
#define TAG_ERROR_NONE 0
#define SFIO_ERROR_NONE 0

// Defined before the functions: in the binary it precedes the __FILE__ string in .rodata.
static const char TAG_SENTINEL[] = "TAG SENTINEL" "TAG SENTINEL" "TAG SENTINEL";

// Error codes returned by this module. Errors from the layers below are re-based so callers
// can tell them apart: SFIO + 100, cipher + 200, checksum + 300.
enum {
    TAG_ERROR_ALREADY_INITIALISED = 1,
    TAG_ERROR_NOT_INITIALISED = 2,
    TAG_ERROR_BAD_PARAM = 4,
    TAG_ERROR_MAP_FULL = 5,
    TAG_ERROR_NO_MEMORY = 6,
    TAG_ERROR_NOT_FOUND = 7,
    TAG_ERROR_BUSY = 8,
    TAG_ERROR_BAD_MAP_CHECKSUM = 9,
    TAG_ERROR_BAD_CHECKSUM = 0xA,
    TAG_ERROR_BAD_HEADER = 0xB,
    TAG_ERROR_SIZE_MISMATCH = 0x6E
};

typedef struct {
    u32   uSize;   // 4 for CRC32
    void* pData;   // points at the value
} ChecksumResult;

typedef struct {
    int (*pfnInit)(void* pAllocator);
    int (*pfnShutdown)(void);
    int (*pfnIsInitialised)(void);
    int (*pfnReset)(void);
    int (*pfnUpdate)(const void* pData, u32 uSize);
    int (*pfnFinalise)(ChecksumResult** ppResult);
    int (*pfnGetResultSize)(u32* pSize);
} ChecksumInterface;

typedef struct {
    int (*pfnInit)(void* pAllocator);
    int (*pfnShutdown)(void);
    int (*pfnIsInitialised)(void);
    int (*pfnSetKey)(const u8* pKey, u32 uLen);
    int (*pfnEncode)(void* pData, u32 uSize);
    int (*pfnDecode)(void* pData, u32 uSize);
    int (*pfnReset)(void);
} CipherInterface;

// The caller's session block (0x48 bytes), also what SharedFileIO calls the process/session.
typedef struct {
    int  uProcess;        // 0x00  SFIO process state; 1 = running, 2 = complete
    int  eDevice;         // 0x04
    u8   unk8[0x21];      // 0x08
    char szName[0x1B];    // 0x29
    u32  uChecksum;       // 0x44  checksum of the map, written by TagFile_End
} TagSession;

typedef struct {
    u32 uTag;
    u32 uOffset;
    u32 uSize;
    u32 uChecksum;
} TagMapEntry;

typedef struct {
    u32          uMaxEntries;   // 0x78
    u32          uNumEntries;   // 0x7C
    u32          uNextOffset;   // 0x80  file offset where the next record goes
    TagMapEntry* pList;         // 0x84
} TagMap;

// Library state, 0x94 bytes, allocated by TagFile_Init.
typedef struct {
    u32                      uChecksum;     // 0x00  map checksum, compared with Session.uChecksum
    TagSession               Session;       // 0x04
    u8*                      pKey;          // 0x4C
    u32                      uKeyLen;       // 0x50
    const CipherInterface*   pCipher;       // 0x54
    const ChecksumInterface* pChecksum;     // 0x58
    int                      eOperation;    // 0x5C  1 save, 2 load, 3 end, 4/5 delete, 6 write, 8 read
    int                      eStep;         // 0x60  step within the operation
    void*                    pBuffer;       // 0x64  current record payload
    u8*                      pHeader;       // 0x68  scratch header buffer
    u32                      uSize;         // 0x6C  current payload size
    u32                      uRecordChecksum; // 0x70
    u32                      uTag;          // 0x74
    TagMap                   Map;           // 0x78
    void*                    pAllocator;    // 0x88
    u32                      uReadOffset;   // 0x8C
    u8                       bBusy;         // 0x90
} TagFileData;

typedef struct {
    u32                      uMaxEntries;   // 0x00
    int*                     pDevices;      // 0x04
    const void*              pFuncs;        // 0x08
    const u8*                pKey;          // 0x0C
    u32                      uKeyLen;       // 0x10
    const CipherInterface*   pCipher;       // 0x14
    void*                    pAllocator;    // 0x18
} TagFileInitParams;

static TagFileData* _TagFile_pData;
// The file's .sbss is 8 bytes: a second, never-referenced static follows the state pointer.
// GCC 2.95 emits unused statics even at -O0. Name and type unknown; only its size matters.
static u32 _TagFile_uUnused;

// SharedFileIO.c
int  SFIOInit(int* pDevices, const void* pFuncs, void* pAllocator);
int  SFIOShutdown(void);
int  SFIOGetSessionInfo(void* pOut);
int  SFIOBeginLoad(const char* pName, int eDevice, int uSearchDirection);
int  SFIOBeginSave(const char* pName, int eDevice, int uSearchDirection);
int  SFIOBeginDelete(const char* pName, int eDevice, int uSearchDirection);
int  SFIOEnd(TagSession* pSession);
int  SFIOSeek(TagSession* pSession, u32 uOffset, u32 uWhence);
int  SFIORead(TagSession* pSession, void* pBuffer, u32 uSize);
int  SFIOWrite(TagSession* pSession, void* pBuffer, u32 uSize);
int  SFIOUpdate(int* pProcess, int* pResult);
int  SFIOSetDescriptor(void* pDescriptor);
void SFIOPlatformCall80172F48(const ChecksumInterface** ppInterface);
// Host memory functions (CodeWarrior side).
extern void* TibExtMemAlloc(void* pAllocator, u32 uSize, u32 uAlign, const char* pFile, int uLine);
extern void  TibExtMemFree(void* pAllocator, void* p, u32 uSize, u32 uAlign);

int TagFile_AllocBuffer(void** ppBuffer, void* pAllocator, u32 uSize, int eType);
int TagFile_FreeBuffer(void* pBuffer, void* pAllocator, u32 uSize, int eType);

// Error re-basing --------------------------------------------------------------------------

static int TagFile_SFIOError(int eError) {
    if (eError == 0) {
        return 0;
    } else {
        return eError + 100;
    }
}

static int TagFile_CipherError(int eError) {
    if (eError == 0) {
        return 0;
    } else {
        return eError + 200;
    }
}

static int TagFile_ChecksumError(int eError) {
    if (eError == 0) {
        return 0;
    } else {
        return eError + 300;
    }
}

// Buffers ----------------------------------------------------------------------------------

// Address alignment and block size for each buffer type.
// eType is unsigned: the switch compiles to cmplwi.
static void TagFile_GetAlignment(u32 eType, u32* pAlignAddr, u32* pAlignSize) {
    u32 uAlignAddr = 0x20;
    u32 uAlignSize = 0x2000;
#line 331 "../../../Source/Common/TagFile/TagFile.c"
    SFIO_ASSERT(NULL != pAlignAddr);
    SFIO_ASSERT(NULL != pAlignSize);
    switch (eType) {
    case 0:
    case 3:
        uAlignAddr = 0x20;
        uAlignSize = 0x2000;
        break;
    case 1:
        uAlignAddr = 0x20;
        uAlignSize = 0x200;
        break;
    case 2:
        uAlignAddr = 0x20;
        uAlignSize = 0x2000;
        break;
    case 4:
        uAlignAddr = 4;
        uAlignSize = 1;
        break;
    default:
#line 359
        SFIO_ASSERT(0);
        break;
    }
    *pAlignAddr = uAlignAddr;
    *pAlignSize = uAlignSize;
}

// Size of a record (header + payload) rounded up to the block size.
static u32 TagFile_GetBufferSize(u32 uSize, int eType) {
    u32 uAlignAddr = 0x20;
    u32 uAlignSize = 0x2000;
    u32 uRemainder = 0;
    TagFile_GetAlignment(eType, &uAlignAddr, &uAlignSize);
    if (uAlignSize > uSize + TAG_BUFFERSIZE) {
        return uAlignSize;
    } else {
        uRemainder = (uSize + TAG_BUFFERSIZE) % uAlignSize;
        return uSize + TAG_BUFFERSIZE + ((uAlignSize - uRemainder) % uAlignSize);
    }
}

// The header sits in the 12 bytes before the payload. With bVerify, NULL unless the slot is
// still unused (holds the sentinel).
static u8* TagFile_GetHeader(void* pBuffer, BOOL bVerify) {
    u8* pHeader = NULL;
#line 448
    SFIO_ASSERT(NULL != pBuffer);
    pHeader = (u8*)pBuffer - TAG_BUFFERSIZE;
    if (bVerify) {
        if (memcmp(pHeader, TAG_SENTINEL, TAG_BUFFERSIZE) != 0) {
            pHeader = NULL;
        }
    }
    return pHeader;
}

static void TagFile_SetHeader(u8* pTagBuffer, u32 uTag, u32 uSize, const void* pChecksum) {
    u8* p = pTagBuffer;
#line 494
    SFIO_ASSERT(NULL != pTagBuffer);
    SFIO_ASSERT(NULL != pChecksum);
#line 498
    SFIO_ASSERT(0 == memcmp(pTagBuffer, TAG_SENTINEL, TAG_BUFFERSIZE));
    memcpy(p, &uTag, 4);
    p += 4;
    memcpy(p, &uSize, 4);
    p += 4;
    memcpy(p, pChecksum, 4);
}

static void TagFile_ClearHeader(u8* pTagBuffer) {
#line 543
    SFIO_ASSERT(NULL != pTagBuffer);
    memcpy(pTagBuffer, TAG_SENTINEL, TAG_BUFFERSIZE);
#line 548
    SFIO_ASSERT(0 == memcmp(pTagBuffer, TAG_SENTINEL, TAG_BUFFERSIZE));
}

static void TagFile_GetHeaderFields(const u8* pTagBuffer, u32* pTag, u32* pSize, void* pChecksum) {
    const u8* p = pTagBuffer;
#line 582
    SFIO_ASSERT(NULL != pTagBuffer);
    if (pTag != NULL) {
        memcpy(pTag, p, 4);
    }
    p += 4;
    if (pSize != NULL) {
        memcpy(pSize, p, 4);
    }
    p += 4;
    if (pChecksum != NULL) {
        memcpy(pChecksum, p, 4);
    }
}

// Map ---------------------------------------------------------------------------------------

// The uIndex'th record with this tag, or NULL.
static TagMapEntry* TagFile_FindEntry(u32 uTag, u32 uIndex) {
    TagMapEntry* pEntry;
    TagMapEntry* pFirst;
    TagMapEntry* pEnd;
    u32 uCount = 0;
#line 641
    SFIO_ASSERT(NULL != _TagFile_pData->Map.pList);
    pFirst = _TagFile_pData->Map.pList;
    pEnd = _TagFile_pData->Map.pList + _TagFile_pData->Map.uNumEntries;
    for (pEntry = pFirst; pEntry < pEnd; pEntry++) {
        if (uTag == pEntry->uTag) {
            if (uIndex == uCount) {
                return pEntry;
            }
            uCount++;
        }
    }
    return NULL;
}

static void TagFile_UpdateEntryChecksum(u32 uTag, u32 uIndex, const void* pChecksum) {
    TagMapEntry* pEntry;
    TagMapEntry* pFirst;
    TagMapEntry* pEnd;
    u32 uCount = 0;
#line 692
    SFIO_ASSERT(NULL != _TagFile_pData->Map.pList);
    pFirst = _TagFile_pData->Map.pList;
    pEnd = _TagFile_pData->Map.pList + _TagFile_pData->Map.uNumEntries;
    for (pEntry = pFirst; pEntry < pEnd; pEntry++) {
        if (uTag == pEntry->uTag) {
            if (uIndex == uCount) {
                memcpy(&pEntry->uChecksum, pChecksum, 4);
            }
            uCount++;
        }
    }
}

// Append a record to the map. Returns its file offset; records are placed on 8 KiB boundaries.
static u32 TagFile_AddEntry(u32 uTag, u32 uSize, const void* pChecksum) {
    TagMapEntry* pEntry;
    TagMapEntry* pFirst;
    TagMapEntry* pEnd;
    u32 uBlockSize = 0;
#line 746
    SFIO_ASSERT(NULL != _TagFile_pData->Map.pList);
    if (_TagFile_pData->Map.uNumEntries == _TagFile_pData->Map.uMaxEntries) {
        return _TagFile_pData->Map.uNextOffset;
    }
    pFirst = _TagFile_pData->Map.pList;
    pEnd = _TagFile_pData->Map.pList + _TagFile_pData->Map.uNumEntries;
    if (_TagFile_pData->Map.uNumEntries == 0) {
        pFirst->uOffset = 0;
    }
    pEntry = pEnd;
    pEntry->uTag = uTag;
    pEntry->uSize = uSize;
    memcpy(&pEntry->uChecksum, pChecksum, 4);
    _TagFile_pData->Map.uNumEntries++;
    uBlockSize = uSize + TAG_BUFFERSIZE;
    if (uBlockSize < 0x2000) {
        uBlockSize = 0x2000;
    } else if ((uBlockSize % 0x2000) == 0) {
    } else {
        // Round up to the next 8 KiB. Written through a pointer cast (an alignment macro in
        // the original, most likely): as plain integer arithmetic GCC reassociates it to
        // (uBlockSize + 0x2000) - remainder, which is not what the binary has.
        uBlockSize = (u32)((u8*)uBlockSize - ((uBlockSize % 0x2000) - 0x2000));
    }
    _TagFile_pData->Map.uNextOffset = pEntry->uOffset + uBlockSize;
    if (_TagFile_pData->Map.uNumEntries != _TagFile_pData->Map.uMaxEntries) {
        pEnd++;
        pEnd->uOffset = _TagFile_pData->Map.uNextOffset;
    }
    return pEntry->uOffset;
}

// Checksum and cipher ---------------------------------------------------------------------

static int TagFile_Checksum(const void* pData, u32 uSize, ChecksumResult** ppResult) {
    const ChecksumInterface* pChecksum = _TagFile_pData->pChecksum;
    int eError = 0;
    eError = pChecksum->pfnReset();
    if (eError == 0) {
        eError = pChecksum->pfnUpdate(pData, uSize);
        if (eError == 0) {
            eError = pChecksum->pfnFinalise(ppResult);
            if (eError == 0) {
                return 0;
            }
        }
    }
    return TagFile_ChecksumError(eError);
}

// Checksum of the whole map, 4 bytes into pOut.
static void TagFile_MapChecksum(void* pOut) {
    ChecksumResult* pResult;
    int eError = 0;
    u32 uSize = 0;
    uSize = _TagFile_pData->Map.uNumEntries * sizeof(TagMapEntry);
    if (uSize == 0) {
        eError = 0;
        memset(pOut, 0, 4);
    } else {
        eError = TagFile_Checksum(_TagFile_pData->Map.pList, uSize, &pResult);
#line 899
        SFIO_ASSERT(TAG_ERROR_NONE == eError);
        memcpy(pOut, pResult->pData, 4);
    }
}

static int TagFile_Encode(void* pData, u32 uSize) {
    int eError = 0;
    if (_TagFile_pData->pCipher == NULL) {
        return 0;
    }
    eError = _TagFile_pData->pCipher->pfnSetKey(_TagFile_pData->pKey, _TagFile_pData->uKeyLen);
    if (eError == 0) {
        eError = _TagFile_pData->pCipher->pfnEncode(pData, uSize);
        if (eError == 0) {
            eError = _TagFile_pData->pCipher->pfnReset();
            if (eError == 0) {
                return 0;
            }
        }
    }
    return TagFile_CipherError(eError);
}

static int TagFile_Decode(void* pData, u32 uSize) {
    int eError = 0;
    if (_TagFile_pData->pCipher == NULL) {
        return 0;
    }
    eError = _TagFile_pData->pCipher->pfnSetKey(_TagFile_pData->pKey, _TagFile_pData->uKeyLen);
    if (eError == 0) {
        eError = _TagFile_pData->pCipher->pfnDecode(pData, uSize);
        if (eError == 0) {
            eError = _TagFile_pData->pCipher->pfnReset();
            if (eError == 0) {
                return 0;
            }
        }
    }
    return TagFile_CipherError(eError);
}

// Operation steps (each starts one asynchronous SFIO call) -----------------------------------

static int TagFile_StartDelete(const char* pName, int eDevice, int uSearchDirection, int eOperation) {
    int eError = 0;
    eError = SFIOBeginDelete(pName, eDevice, uSearchDirection);
    _TagFile_pData->eOperation = eOperation;
    _TagFile_pData->eStep = 4;
    _TagFile_pData->Map.uNumEntries = 0;
    _TagFile_pData->Map.uNextOffset = 0;
    memset(_TagFile_pData, 0, 4);
    return TagFile_SFIOError(eError);
}

static int TagFile_SeekToStart(void) {
    int eError = 0;
    _TagFile_pData->eStep = 9;
    eError = SFIOSeek(&_TagFile_pData->Session, 0, 0);
    return TagFile_SFIOError(eError);
}

static int TagFile_ReadHeader(void) {
    int eError = 0;
    _TagFile_pData->eStep = 7;
    eError = SFIORead(&_TagFile_pData->Session, _TagFile_pData->pHeader, TagFile_GetBufferSize(0, 1));
    return TagFile_SFIOError(eError);
}

// After a header has been read: decode it, add it to the map, seek to the next record.
static int TagFile_ProcessHeader(void) {
    int eError = 0;
    int eSFIOError = 0;
    u32 uTag;
    u32 uSize = 0;
    u32 uOffset = 0;
    _TagFile_pData->eStep = 0xB;
    eError = TagFile_Decode(_TagFile_pData->pHeader, TAG_BUFFERSIZE);
    if (eError != 0) {
        return eError;
    }
    TagFile_GetHeaderFields(_TagFile_pData->pHeader, &uTag, &uSize, &_TagFile_pData->uRecordChecksum);
    uOffset = TagFile_AddEntry(uTag, uSize, &_TagFile_pData->uRecordChecksum);
    if (_TagFile_pData->Map.uNextOffset == uOffset) {
        return TAG_ERROR_MAP_FULL;
    }
    eSFIOError = SFIOSeek(&_TagFile_pData->Session, _TagFile_pData->Map.uNextOffset, 0);
    return TagFile_SFIOError(eSFIOError);
}

static int TagFile_WriteRecord(void) {
    int eError = 0;
    int eSFIOError = 0;
    u8* pHeader = NULL;
    _TagFile_pData->eStep = 6;
    pHeader = TagFile_GetHeader(_TagFile_pData->pBuffer, TRUE);
    if (pHeader == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    TagFile_SetHeader(pHeader, _TagFile_pData->uTag, _TagFile_pData->uSize, &_TagFile_pData->uRecordChecksum);
    eError = TagFile_Encode(pHeader, TAG_BUFFERSIZE);
    if (eError != 0) {
        return eError;
    }
    eError = TagFile_Encode(_TagFile_pData->pBuffer, _TagFile_pData->uSize);
    if (eError != 0) {
        return eError;
    }
    eSFIOError = SFIOWrite(&_TagFile_pData->Session, pHeader, TagFile_GetBufferSize(_TagFile_pData->uSize, 2));
    return TagFile_SFIOError(eSFIOError);
}

static int TagFile_ReadRecord(void) {
    u8* pHeader = NULL;
    int eSFIOError = 0;
    _TagFile_pData->eStep = 0xA;
    pHeader = TagFile_GetHeader(_TagFile_pData->pBuffer, FALSE);
    eSFIOError = SFIORead(&_TagFile_pData->Session, pHeader, TagFile_GetBufferSize(_TagFile_pData->uSize, 1));
    return TagFile_SFIOError(eSFIOError);
}

// After a record has been read: decode header and payload, check tag, size and checksum.
static int TagFile_VerifyRecord(void) {
    int eError = 0;
    ChecksumResult* pResult = NULL;
    u8* pHeader = NULL;
    u32 uSize = 0;
    u32 uTag;
    _TagFile_pData->eStep = 0xA;
    pHeader = TagFile_GetHeader(_TagFile_pData->pBuffer, FALSE);
    eError = TagFile_Decode(pHeader, TAG_BUFFERSIZE);
    if (eError != 0) {
        return eError;
    }
    TagFile_GetHeaderFields(pHeader, &uTag, &uSize, &_TagFile_pData->uRecordChecksum);
    if (uTag != _TagFile_pData->uTag) {
        return TAG_ERROR_BAD_HEADER;
    }
    if (uSize != _TagFile_pData->uSize) {
        return TAG_ERROR_SIZE_MISMATCH;
    }
    eError = TagFile_Decode(_TagFile_pData->pBuffer, _TagFile_pData->uSize);
    if (eError != 0) {
        return eError;
    }
    eError = TagFile_Checksum(_TagFile_pData->pBuffer, _TagFile_pData->uSize, &pResult);
    if (eError != 0) {
        return eError;
    }
    if (pResult->uSize != 4) {
        return TAG_ERROR_BAD_HEADER;
    }
    if (memcmp(pResult->pData, &_TagFile_pData->uRecordChecksum, 4) != 0) {
        return TAG_ERROR_BAD_CHECKSUM;
    }
    return 0;
}

// Public interface --------------------------------------------------------------------------

int TagFile_IsInitialised(void) {
    return _TagFile_pData != NULL;
}

int TagFile_Init(const TagFileInitParams* pParams) {
    int eError = 0;
    int eSFIOError = 0;
    int eChecksumError = 0;
    int eCipherError = 0;
    if (TagFile_IsInitialised()) {
        return TAG_ERROR_ALREADY_INITIALISED;
    }
#line 1633
    SFIO_ASSERT(sizeof(TAG_SENTINEL) >= TAG_BUFFERSIZE);
    eSFIOError = SFIOInit(pParams->pDevices, pParams->pFuncs, pParams->pAllocator);
    if (eSFIOError != 0) {
        return TagFile_SFIOError(eSFIOError);
    }
#line 1644
    _TagFile_pData = TibExtMemAlloc(pParams->pAllocator, sizeof(TagFileData), 4, __FILE__, __LINE__);
    if (_TagFile_pData == NULL) {
        return TAG_ERROR_NO_MEMORY;
    }
    SFIOPlatformCall80172F48(&_TagFile_pData->pChecksum);
    eChecksumError = _TagFile_pData->pChecksum->pfnInit(pParams->pAllocator);
    if (eChecksumError != 0) {
        return TagFile_ChecksumError(eChecksumError);
    }
    _TagFile_pData->pCipher = pParams->pCipher;
    if (_TagFile_pData->pCipher != NULL) {
        eCipherError = _TagFile_pData->pCipher->pfnInit(pParams->pAllocator);
        if (eCipherError != 0) {
            return TagFile_CipherError(eCipherError);
        }
        _TagFile_pData->uKeyLen = pParams->uKeyLen;
#line 1687
        _TagFile_pData->pKey = TibExtMemAlloc(pParams->pAllocator, _TagFile_pData->uKeyLen, 4, __FILE__, __LINE__);
        if (_TagFile_pData->pKey == NULL) {
            return TAG_ERROR_NO_MEMORY;
        }
        memcpy(_TagFile_pData->pKey, pParams->pKey, _TagFile_pData->uKeyLen);
    }
    _TagFile_pData->Map.uMaxEntries = pParams->uMaxEntries;
    _TagFile_pData->Map.uNumEntries = 0;
    _TagFile_pData->Map.uNextOffset = 0;
#line 1704
    _TagFile_pData->Map.pList = TibExtMemAlloc(pParams->pAllocator, pParams->uMaxEntries * sizeof(TagMapEntry), 4, __FILE__, __LINE__);
    if (_TagFile_pData->Map.pList == NULL) {
        return TAG_ERROR_NO_MEMORY;
    }
    // Note: pAllocator is read here before it is assigned below (as in the original).
    eError = TagFile_AllocBuffer((void**)&_TagFile_pData->pHeader, _TagFile_pData->pAllocator, 0, 1);
    if (eError != 0) {
        return eError;
    }
    _TagFile_pData->pHeader = _TagFile_pData->pHeader - TAG_BUFFERSIZE;
    _TagFile_pData->pAllocator = pParams->pAllocator;
    _TagFile_pData->bBusy = FALSE;
    return 0;
}

int TagFile_Shutdown(void) {
    int eError = 0;
    int eSFIOError = 0;
    int eChecksumError = 0;
    int eCipherError = 0;
    if (!TagFile_IsInitialised()) {
        return TAG_ERROR_NOT_INITIALISED;
    }
    TagFile_ClearHeader(_TagFile_pData->pHeader);
    _TagFile_pData->pHeader = _TagFile_pData->pHeader + TAG_BUFFERSIZE;
    eError = TagFile_FreeBuffer(_TagFile_pData->pHeader, _TagFile_pData->pAllocator, 0, 1);
    if (eError != 0) {
        return eError;
    }
    TibExtMemFree(_TagFile_pData->pAllocator, _TagFile_pData->Map.pList, _TagFile_pData->Map.uMaxEntries * sizeof(TagMapEntry), 4);
    _TagFile_pData->Map.pList = NULL;
    if (_TagFile_pData->pCipher != NULL) {
        eCipherError = _TagFile_pData->pCipher->pfnShutdown();
        if (eCipherError != 0) {
            return TagFile_CipherError(eCipherError);
        }
        TibExtMemFree(_TagFile_pData->pAllocator, _TagFile_pData->pKey, _TagFile_pData->uKeyLen, 4);
        _TagFile_pData->pKey = NULL;
    }
    _TagFile_pData->pCipher = NULL;
    eChecksumError = _TagFile_pData->pChecksum->pfnShutdown();
    if (eChecksumError != 0) {
        return TagFile_ChecksumError(eChecksumError);
    }
    _TagFile_pData->pChecksum = NULL;
    eSFIOError = SFIOShutdown();
    if (eSFIOError != 0) {
        return TagFile_SFIOError(eSFIOError);
    }
    TibExtMemFree(_TagFile_pData->pAllocator, _TagFile_pData, sizeof(TagFileData), 4);
    _TagFile_pData = NULL;
    return 0;
}

int TagFile_BeginSave(const char* pName, int eDevice, int uSearchDirection) {
    int eError = 0;
    _TagFile_pData->eOperation = 1;
    _TagFile_pData->eStep = 1;
    eError = SFIOBeginSave(pName, eDevice, uSearchDirection);
    return TagFile_SFIOError(eError);
}

int TagFile_BeginLoad(const char* pName, int eDevice, int uSearchDirection) {
    int eError = 0;
    _TagFile_pData->eOperation = 2;
    _TagFile_pData->eStep = 2;
    eError = SFIOBeginLoad(pName, eDevice, uSearchDirection);
    return TagFile_SFIOError(eError);
}

// Delete the file an existing session refers to.
int TagFile_DeleteSession(TagSession* pSession) {
    memcpy(&_TagFile_pData->Session, pSession, sizeof(TagSession));
    return TagFile_StartDelete(pSession->szName, pSession->eDevice, -1, 5);
}

int TagFile_Delete(const char* pName, int eDevice, int uSearchDirection) {
    return TagFile_StartDelete(pName, eDevice, uSearchDirection, 4);
}

int TagFile_End(TagSession* pSession) {
    int eError = 0;
    TagFile_MapChecksum(&pSession->uChecksum);
    _TagFile_pData->Map.uNumEntries = 0;
    _TagFile_pData->Map.uNextOffset = 0;
    _TagFile_pData->eOperation = 3;
    _TagFile_pData->eStep = 3;
    eError = SFIOEnd(pSession);
    return TagFile_SFIOError(eError);
}

int TagFile_Write(TagSession* pSession, u32 uTag, u32 uIndex, void* pBuffer, u32 uSize) {
    ChecksumResult* pResult = NULL;
    int eError = 0;
    int eSFIOError = 0;
    TagMapEntry* pEntry = NULL;
    u32 uOffset = 0;
    _TagFile_pData->eOperation = 6;
    _TagFile_pData->eStep = 5;
    memcpy(&_TagFile_pData->Session, pSession, sizeof(TagSession));
    _TagFile_pData->uTag = uTag;
    _TagFile_pData->pBuffer = pBuffer;
    _TagFile_pData->uSize = uSize;
    pEntry = TagFile_FindEntry(uTag, uIndex);
    eError = TagFile_Checksum(_TagFile_pData->pBuffer, _TagFile_pData->uSize, &pResult);
    if (eError != 0) {
        return eError;
    }
    memcpy(&_TagFile_pData->uRecordChecksum, pResult->pData, 4);
    if (pEntry == NULL) {
        uOffset = _TagFile_pData->Map.uNextOffset;
        TagFile_AddEntry(uTag, _TagFile_pData->uSize, &_TagFile_pData->uRecordChecksum);
    } else {
        uOffset = pEntry->uOffset;
        TagFile_UpdateEntryChecksum(uTag, uIndex, &_TagFile_pData->uRecordChecksum);
    }
    eSFIOError = SFIOSeek(pSession, uOffset, 0);
    return TagFile_SFIOError(eSFIOError);
}

int TagFile_Read(TagSession* pSession, u32 uTag, u32 uIndex, void* pBuffer, u32 uSize) {
    int eSFIOError = 0;
    TagMapEntry* pEntry = NULL;
    u32 uOffset = 0;
    _TagFile_pData->eOperation = 8;
    _TagFile_pData->eStep = 5;
    memcpy(&_TagFile_pData->Session, pSession, sizeof(TagSession));
    _TagFile_pData->uTag = uTag;
    _TagFile_pData->pBuffer = pBuffer;
    _TagFile_pData->uSize = uSize;
    pEntry = TagFile_FindEntry(uTag, uIndex);
    if (pEntry == NULL) {
        return TAG_ERROR_NOT_FOUND;
    }
    if (uSize != pEntry->uSize) {
        return TAG_ERROR_SIZE_MISMATCH;
    }
    uOffset = pEntry->uOffset;
    _TagFile_pData->uReadOffset = uOffset;
    eSFIOError = SFIOSeek(pSession, uOffset, 0);
    return TagFile_SFIOError(eSFIOError);
}

// Number of records with this tag.
int TagFile_Count(TagSession* pSession, u32 uTag, u32* pCount) {
    TagMapEntry* pEntry;
    TagMapEntry* pFirst;
    TagMapEntry* pEnd;
    if (!TagFile_IsInitialised()) {
        return TAG_ERROR_NOT_INITIALISED;
    }
    if (pSession == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    if (pCount == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
#line 2308
    SFIO_ASSERT(NULL != _TagFile_pData->Map.pList);
    *pCount = 0;
    pFirst = _TagFile_pData->Map.pList;
    pEnd = _TagFile_pData->Map.pList + _TagFile_pData->Map.uNumEntries;
    for (pEntry = pFirst; pEntry < pEnd; pEntry++) {
        if (uTag == pEntry->uTag) {
            (*pCount)++;
        }
    }
    return 0;
}

// Size of the uIndex'th record with this tag.
int TagFile_GetSize(TagSession* pSession, u32 uTag, u32 uIndex, u32* pSize) {
    int eError = 0;
    TagMapEntry* pEntry;
    if (!TagFile_IsInitialised()) {
        return TAG_ERROR_NOT_INITIALISED;
    }
    if (pSession == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    if (pSize == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
#line 2379
    SFIO_ASSERT(NULL != _TagFile_pData->Map.pList);
    *pSize = 0;
    pEntry = TagFile_FindEntry(uTag, uIndex);
    if (pEntry != NULL) {
        *pSize = pEntry->uSize;
    } else {
        eError = TAG_ERROR_NOT_FOUND;
    }
    return eError;
}

int TagFile_GetSession(TagSession* pSession) {
    if (pSession == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    memcpy(pSession, &_TagFile_pData->Session, sizeof(TagSession));
    return 0;
}

// Drive the current operation. *pProcess: 1 while running, 2 when complete. Returns 0 or an
// error; *pResult receives the record size after a read.
int TagFile_Update(int* pProcess, int* pResult) {
    int eError = 0;
    int eSFIOError = 0;
    u32 eCipherError = 0;   // unsigned: the int result is stored straight from r3
    u8* pHeader;
    if (!TagFile_IsInitialised()) {
        return TAG_ERROR_NOT_INITIALISED;
    }
    if (pProcess == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    if (pResult == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    eSFIOError = SFIOUpdate(pProcess, pResult);
    if (*pProcess == 0 && _TagFile_pData->eOperation != 0) {
        return TAG_ERROR_BUSY;
    }
    if (*pProcess == 2) {
        if (eSFIOError != 0 && _TagFile_pData->eOperation != 4 && _TagFile_pData->eOperation != 5) {
            return TagFile_SFIOError(eSFIOError);
        }
        if (_TagFile_pData->eOperation == 1 && eSFIOError == 0) {
            _TagFile_pData->bBusy = TRUE;
        }
        if (_TagFile_pData->eOperation != 1 && _TagFile_pData->eOperation != 4) {
            _TagFile_pData->bBusy = FALSE;
        }
        if (_TagFile_pData->eOperation == 6) {
            if (_TagFile_pData->eStep == 5) {
                *pProcess = 1;
                return TagFile_WriteRecord();
            } else if (_TagFile_pData->eStep == 6) {
                pHeader = TagFile_GetHeader(_TagFile_pData->pBuffer, FALSE);
                TagFile_ClearHeader(pHeader);
                if (TagFile_GetBufferSize(_TagFile_pData->uSize, 2) == *pResult) {
                    *pResult = _TagFile_pData->uSize;
                } else {
                    return TAG_ERROR_SIZE_MISMATCH;
                }
                eCipherError = TagFile_Decode(_TagFile_pData->pBuffer, _TagFile_pData->uSize);
                if (eCipherError != 0) {
                    return TagFile_CipherError(eCipherError);
                }
            } else {
#line 2616
                SFIO_ASSERT(0);
            }
        }
        if (_TagFile_pData->eOperation == 8) {
            if (_TagFile_pData->eStep == 5) {
                *pProcess = 1;
                return TagFile_ReadRecord();
            } else if (_TagFile_pData->eStep == 0xA) {
                pHeader = TagFile_GetHeader(_TagFile_pData->pBuffer, FALSE);
                if (TagFile_GetBufferSize(_TagFile_pData->uSize, 1) == *pResult) {
                    *pResult = _TagFile_pData->uSize;
                    eError = TagFile_VerifyRecord();
                } else {
                    eError = TAG_ERROR_SIZE_MISMATCH;
                }
                TagFile_ClearHeader(pHeader);
                return eError;
            } else {
#line 2670
                SFIO_ASSERT(0);
            }
        }
        if (_TagFile_pData->eOperation == 4 || _TagFile_pData->eOperation == 5) {
            if (_TagFile_pData->eStep == 4) {
                if (eSFIOError != 0) {
                    _TagFile_pData->bBusy = FALSE;
                    return TagFile_SFIOError(eSFIOError);
                }
                eSFIOError = SFIOGetSessionInfo(&_TagFile_pData->Session);
#line 2686
                SFIO_ASSERT(SFIO_ERROR_NONE == eSFIOError);
                *pProcess = 1;
                return TagFile_SeekToStart();
            } else if (_TagFile_pData->eStep == 9) {
                if (eSFIOError != 0) {
                    _TagFile_pData->bBusy = FALSE;
                    return TagFile_SFIOError(eSFIOError);
                }
                if (_TagFile_pData->bBusy) {
                    _TagFile_pData->bBusy = FALSE;
                    *pProcess = 2;
                    return 0;
                } else {
                    _TagFile_pData->bBusy = FALSE;
                    *pProcess = 1;
                    return TagFile_ReadHeader();
                }
            } else if (_TagFile_pData->eStep == 7) {
                if (eSFIOError != 0 && eSFIOError != 0xA) {
                    _TagFile_pData->bBusy = FALSE;
                    return TagFile_SFIOError(eSFIOError);
                }
                if (eSFIOError == 0xA || _TagFile_pData->Map.uNumEntries == _TagFile_pData->Map.uMaxEntries) {
                    *pProcess = 2;
                    TagFile_ClearHeader(_TagFile_pData->pHeader);
                    TagFile_MapChecksum(_TagFile_pData);
                    if (_TagFile_pData->eOperation == 5) {
                        if (memcmp(_TagFile_pData, &_TagFile_pData->Session.uChecksum, 4) == 0) {
                            return 0;
                        } else {
                            return TAG_ERROR_BAD_MAP_CHECKSUM;
                        }
                    }
                    return 0;
                } else {
                    *pProcess = 1;
                    return TagFile_ProcessHeader();
                }
            } else if (TRUE) {
                // A folded-away condition, as in TagFile_FreeBuffer: the binary keeps the
                // end-of-block jump and the unreachable assert of a trailing else.
                if (eSFIOError != 0) {
                    return TagFile_SFIOError(eSFIOError);
                }
                *pProcess = 1;
                return TagFile_ReadHeader();
            } else {
#line 2778
                SFIO_ASSERT(0);
            }
        }
        if (*pProcess == 2) {
            _TagFile_pData->eOperation = 0;
            _TagFile_pData->eStep = 0;
        }
    }
    return TagFile_SFIOError(eSFIOError);
}

int TagFile_SetDescriptor(void* pDescriptor) {
    int eError = 0;
    if (!TagFile_IsInitialised()) {
        return TAG_ERROR_NOT_INITIALISED;
    }
    eError = SFIOSetDescriptor(pDescriptor);
    return TagFile_SFIOError(eError);
}

// Allocate a record buffer: header slot (marked unused) followed by uSize bytes of payload,
// rounded up to the block size. *ppBuffer receives the payload address.
int TagFile_AllocBuffer(void** ppBuffer, void* pAllocator, u32 uSize, int eType) {
    u32 uAlignAddr = 0x20;
    u32 uAlignSize = 0x2000;
    u32 uTotal;
    TagFile_GetAlignment(eType, &uAlignAddr, &uAlignSize);
    uTotal = TagFile_GetBufferSize(uSize, eType);
    if (ppBuffer == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
#line 2917
    *ppBuffer = TibExtMemAlloc(pAllocator, uTotal, uAlignAddr, __FILE__, __LINE__);
    if (*ppBuffer != NULL) {
        memset(*ppBuffer, 0, uTotal);
        memcpy(*ppBuffer, TAG_SENTINEL, TAG_BUFFERSIZE);
        *ppBuffer = (u8*)*ppBuffer + TAG_BUFFERSIZE;
        return 0;
    } else {
        return TAG_ERROR_NO_MEMORY;
    }
    return TAG_ERROR_BAD_HEADER;
}

int TagFile_FreeBuffer(void* pBuffer, void* pAllocator, u32 uSize, int eType) {
    u32 uAlignAddr = 0x20;
    u32 uAlignSize = 0x2000;
    u32 uTotal;
    TagFile_GetAlignment(eType, &uAlignAddr, &uAlignSize);
    uTotal = TagFile_GetBufferSize(uSize, eType);
    if (pBuffer == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    // A copy of TagFile_AllocBuffer's shape with the condition folded away: the binary keeps the
    // dead else (return 4) and the unreachable trailing return, exactly as a constant-true if
    // produces.
    if (TRUE) {
        pBuffer = (u8*)pBuffer - TAG_BUFFERSIZE;
        TibExtMemFree(pAllocator, pBuffer, uTotal, uAlignAddr);
        pBuffer = NULL;
        return 0;
    } else {
        return TAG_ERROR_BAD_PARAM;
    }
    return TAG_ERROR_BAD_HEADER;
}

int TagFile_GetBufferSizeForPayload(u32 uSize, u32* pSize) {
    if (pSize == NULL) {
        return TAG_ERROR_BAD_PARAM;
    }
    *pSize = TagFile_GetBufferSize(uSize, 2);
    return 0;
}
