// ChecksumCRC32.c: the CRC32 checksum of EA's shared file library. The tag file checks each record of
// the EA Sports Bio save and its map of records with it, and EASBStorage.c the save's banner and icon.

#include "Common/SharedFileIO.h"
#include "Common/TibExtMem.h"

static CheckSumCRC32InfoT _ChecksumCRC32_State = { NULL, { CHECKSUMCRC32_SIZE, NULL }, 0xFFFFFFFF, NULL };

static ChecksumErrorE _ChecksumCRC32_Init(void* pAllocator);
static ChecksumErrorE _ChecksumCRC32_Shutdown(void);
static BOOL _ChecksumCRC32_IsInitialized(void);
static ChecksumErrorE _ChecksumCRC32_Start(void);
static ChecksumErrorE _ChecksumCRC32_CalculateBuffer(const u8* pBuffer, u32 uLength);
static ChecksumErrorE _ChecksumCRC32_Stop(ChecksumResultT** ppResult);
static ChecksumErrorE _ChecksumCRC32_GetSize(u32* pSize);

static const ChecksumTypeT _ChecksumCRC32_Type = {
    _ChecksumCRC32_Init,
    _ChecksumCRC32_Shutdown,
    _ChecksumCRC32_IsInitialized,
    _ChecksumCRC32_Start,
    _ChecksumCRC32_CalculateBuffer,
    _ChecksumCRC32_Stop,
    _ChecksumCRC32_GetSize,
};

// The lookup table's entry for one byte value: the value shifted through eight rounds of the
// reflected CRC32 polynomial 0xEDB88320.
static u32 _ChecksumCRC32_Calculate(u32 uValue) {
    u8 uLoop;
    u32 uCRC = uValue;
    for (uLoop = 8; uLoop != 0; uLoop--) {
        if (uCRC & 1) {
            uCRC = (uCRC >> 1) ^ 0xEDB88320;
        } else {
            uCRC = uCRC >> 1;
        }
    }
    return uCRC;
}

static void _ChecksumCRC32_CreateLookupTable(u32* pTable) {
    u32 uLoop;
    for (uLoop = 0; uLoop < CHECKSUMCRC32_NUM_ENTRIES; uLoop++) {
        pTable[uLoop] = _ChecksumCRC32_Calculate(uLoop);
    }
}

// Allocates and fills the lookup table and starts a checksum.
static ChecksumErrorE _ChecksumCRC32_Init(void* pAllocator) {
    if (_ChecksumCRC32_State.pLookupTable != NULL) return CHECKSUM_ERROR_INITIALIZED;
#line 213 "../../../Source/Common/Checksum/ChecksumCRC32.c"
    _ChecksumCRC32_State.pLookupTable = TibExtMemAlloc(pAllocator, CHECKSUMCRC32_NUM_ENTRIES * sizeof(u32), 4,
                                                       __FILE__, __LINE__);
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_OUTOFMEMORY;
    _ChecksumCRC32_State.pAllocator = pAllocator;
    _ChecksumCRC32_CreateLookupTable(_ChecksumCRC32_State.pLookupTable);
    _ChecksumCRC32_State.uCRC = 0xFFFFFFFF;
    _ChecksumCRC32_State.Result.uSize = CHECKSUMCRC32_SIZE;
    _ChecksumCRC32_State.Result.pChecksum = NULL;
    return CHECKSUM_ERROR_NONE;
}

// Frees the lookup table.
static ChecksumErrorE _ChecksumCRC32_Shutdown(void) {
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_NOTINITIALIZED;
    TibExtMemFree(_ChecksumCRC32_State.pAllocator, _ChecksumCRC32_State.pLookupTable,
                  CHECKSUMCRC32_NUM_ENTRIES * sizeof(u32), 4);
    _ChecksumCRC32_State.pLookupTable = NULL;
    return CHECKSUM_ERROR_NONE;
}

static BOOL _ChecksumCRC32_IsInitialized(void) {
    return _ChecksumCRC32_State.pLookupTable != NULL;
}

// Starts a new checksum.
static ChecksumErrorE _ChecksumCRC32_Start(void) {
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_NOTINITIALIZED;
    _ChecksumCRC32_State.uCRC = 0xFFFFFFFF;
    return CHECKSUM_ERROR_NONE;
}

// Adds uLength bytes to the running CRC, one table lookup per byte. A buffer can be added in
// pieces, and adding after Stop carries on from where Stop left off.
static ChecksumErrorE _ChecksumCRC32_CalculateBuffer(const u8* pBuffer, u32 uLength) {
    u32 uIndex;
    u32 uGen;
    u32 uCRC = _ChecksumCRC32_State.uCRC;
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_NOTINITIALIZED;
    if (pBuffer == NULL) return CHECKSUM_ERROR_INVALIDPARAMETERS;
    uIndex = 0;
    while (uIndex < uLength) {
        uGen = (pBuffer[uIndex] ^ uCRC) & 0xFF;
        uCRC = ((uCRC >> 8) & 0x00FFFFFF) ^ _ChecksumCRC32_State.pLookupTable[uGen];
        uIndex++;
    }
    _ChecksumCRC32_State.uCRC = uCRC;
    return CHECKSUM_ERROR_NONE;
}

// The finished CRC (the running value inverted), in *ppResult. The result points at one static
// copy, which the next Stop overwrites. The running value is kept until the next Start.
static ChecksumErrorE _ChecksumCRC32_Stop(ChecksumResultT** ppResult) {
    static u32 uCRCResult;
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_NOTINITIALIZED;
    if (ppResult == NULL) return CHECKSUM_ERROR_INVALIDPARAMETERS;
    uCRCResult = ~_ChecksumCRC32_State.uCRC;
    _ChecksumCRC32_State.Result.pChecksum = (u8*)&uCRCResult;
    _ChecksumCRC32_State.Result.uSize = CHECKSUMCRC32_SIZE;
    *ppResult = &_ChecksumCRC32_State.Result;
    return CHECKSUM_ERROR_NONE;
}

// The size of the result in bytes: 4.
static ChecksumErrorE _ChecksumCRC32_GetSize(u32* pSize) {
    if (_ChecksumCRC32_State.pLookupTable == NULL) return CHECKSUM_ERROR_NOTINITIALIZED;
    if (pSize == NULL) return CHECKSUM_ERROR_INVALIDPARAMETERS;
    *pSize = CHECKSUMCRC32_SIZE;
    return CHECKSUM_ERROR_NONE;
}

// The CRC32's functions as a table, which the shared file library (llSFIOGetSignatureMethod) and
// EASBStorage.c take.
const ChecksumTypeT* ChecksumCRC32QueryType(void) {
    return &_ChecksumCRC32_Type;
}
