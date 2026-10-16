#ifndef COMMON_TAGFILE_H
#define COMMON_TAGFILE_H

// EA shared file library: the tag file, the save-data container on top of SharedFileIO
// (TagFile.c, EA's file name). The asserts give _TagFile_pData; the TagFile_ function names are ours.

#include "Common/SharedFileIO.h"

// What a cipher's functions return. A cipher's status is one of these too: a call made in the
// wrong state fails with the status itself.
typedef enum {
    CRYPT_ERROR_NONE = 0,
    CRYPT_ERROR_INVALIDPARAMETERS = 2,   // a null key or buffer, or a size of 0
    CRYPT_ERROR_INITIALIZED = 3,         // Init on a cipher already initialised
    CRYPT_ERROR_NOTINITIALIZED = 4,      // before Init, or after Shutdown
    CRYPT_ERROR_STARTED = 5,             // a key is set and back at its first byte (Start, Stop)
    CRYPT_ERROR_NOTSTARTED = 6,          // initialised, but no key set
    CRYPT_ERROR_ENCRYPTING = 7,          // the last call encrypted
    CRYPT_ERROR_DECRYPTING = 8           // the last call decrypted
} CryptErrorE;

// A cipher's functions, as CryptXORQueryType hands them out.
typedef struct CryptType_t {
    CryptErrorE (*Init)(void* pAllocator);
    CryptErrorE (*Shutdown)(void);
    u8 (*IsInitialized)(void);
    CryptErrorE (*Start)(const u8* pKey, u32 uKeySize);
    CryptErrorE (*Encrypt)(void* pBuffer, u32 uBufferSize);
    CryptErrorE (*Decrypt)(void* pBuffer, u32 uBufferSize);
    CryptErrorE (*Stop)(void);
} CryptTypeT;

// The caller's session block (0x48 bytes): the SFIO session plus the map's checksum.
typedef struct {
    SFIOSession Sfio;     // 0x00
    u32  uChecksum;       // 0x44  checksum of the map, written by TagFile_End
} TagSession;
LAYOUT_ASSERT(TagSession, 0x48);

typedef struct {
    u32                      uMaxEntries;   // 0x00
    int*                     pDevices;      // 0x04
    const SFIOFuncTable*     pFuncs;        // 0x08
    const u8*                pKey;          // 0x0C
    u32                      uKeyLen;       // 0x10
    const CryptTypeT*        pCipher;       // 0x14
    void*                    pAllocator;    // 0x18
} TagFileInitParams;

// CryptXOR.c
const CryptTypeT* CryptXORQueryType(void);

int TagFile_IsInitialised(void);
int TagFile_Init(const TagFileInitParams* pParams);
int TagFile_Shutdown(void);
int TagFile_Create(const char* pName, int eDevice, int uSearchDirection);
int fn_80174DF0_Delete(const char* pName, int eDevice, int uSearchDirection);
int TagFile_Reopen(TagSession* pSession);
int TagFile_Open(const char* pName, int eDevice, int uSearchDirection);
int TagFile_End(TagSession* pSession);
int TagFile_Write(TagSession* pSession, u32 uTag, u32 uIndex, void* pBuffer, u32 uSize);
int TagFile_Read(TagSession* pSession, u32 uTag, u32 uIndex, void* pBuffer, u32 uSize);
int TagFile_Count(TagSession* pSession, u32 uTag, u32* pCount);
int TagFile_GetSize(TagSession* pSession, u32 uTag, u32 uIndex, u32* pSize);
int TagFile_GetSession(TagSession* pSession);
int TagFile_Update(int* pProcess, int* pResult);
int TagFile_AllocBuffer(void** ppBuffer, void* pAllocator, u32 uSize, int eType);
int TagFile_FreeBuffer(void* pBuffer, void* pAllocator, u32 uSize, int eType);
int TagFile_GetBufferSizeForPayload(u32 uSize, u32* pSize);

#endif
