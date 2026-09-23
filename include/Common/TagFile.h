#ifndef COMMON_TAGFILE_H
#define COMMON_TAGFILE_H

// EA shared file library: the tag file, the save-data container on top of SharedFileIO
// (TagFile.c). Names with the TagFile prefix come from assert strings in the binary.

#include "Common/SharedFileIO.h"

typedef struct {
    int (*pfnInit)(void* pAllocator);
    int (*pfnShutdown)(void);
    int (*pfnIsInitialised)(void);
    int (*pfnSetKey)(const u8* pKey, u32 uLen);
    int (*pfnEncode)(void* pData, u32 uSize);
    int (*pfnDecode)(void* pData, u32 uSize);
    int (*pfnReset)(void);
} CipherInterface;

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
    const CipherInterface*   pCipher;       // 0x14
    void*                    pAllocator;    // 0x18
} TagFileInitParams;

// CipherXOR.c: the cipher's functions, read as a CipherInterface.
void* const* Cipher_GetInterface(void);

int TagFile_IsInitialised(void);
int TagFile_Init(const TagFileInitParams* pParams);
int TagFile_Shutdown(void);
int TagFile_DeleteSession(TagSession* pSession);
int TagFile_Delete(const char* pName, int eDevice, int uSearchDirection);
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
