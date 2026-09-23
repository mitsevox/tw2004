#ifndef COMMON_SHAREDFILEIO_H
#define COMMON_SHAREDFILEIO_H

// EA shared file library (memory-card save system). Names with the SFIO prefix and Hungarian
// notation come from assert strings in the binary; everything else is ours.

#include "game_types.h"
#include "platform.h"

typedef u8             BOOL;   // byte-sized: SFIOIsInitialized() is truncated to 8 bits after use
#define TRUE  1
#define FALSE 0

enum {
    SFIO_DEVICE_FIRST = 0,
    SFIO_DEVICE_LAST = 1,
    SFIO_DEVICE_INVALID = -1
};
#define SFIO_DEVICE_MASK(e) ((u16)(1 << (u16)(e)))

// Retail build: the assert handler is a 4-byte stub, but every call site survives.
extern void fn_8012214C(const char* pFile, int uLine, const char* pExpr, int unused);
#define SFIO_ASSERT(cond) { u8 bSkip = 0; if (!((cond) | bSkip)) fn_8012214C(__FILE__, __LINE__, #cond, 0); }

// What the host describes a save with (SFIOSetDescriptor). On the GameCube every save file starts
// with one card block holding the banner, icon and comment, followed by the data.
typedef struct {
    void* pHeader;           // 0x00  the header block's image; 32-byte aligned (fn_80173024)
    u32   uSize04;           // 0x04  \ the header block's two parts; together exactly one
    u32   uSize08;           // 0x08  / 8 KiB card block (fn_80173024)
    u32   uDataSize;         // 0x0C  the save data after the header block
} SFIODescriptor;

// An open save, as the caller keeps it between calls (SFIOGetSessionInfo hands it out; SFIOEnd,
// SFIOSeek, SFIORead and SFIOWrite take it back). 0x44 bytes.
typedef struct {
    int   uHandle;           // 0x00  result of the last mount/open; passed to the device layer
    int   eDevice;           // 0x04  current device index (read as a short by SFIONextDeviceFromMask)
    char  szDirName[1];      // 0x08  the save's directory: always empty on the GameCube (fn_80171744)
    char  szFileName[0x20];  // 0x09  the save's file name on the device (fn_80171744)
    char  szName[0x1B];      // 0x29  the name the host asked for
} SFIOSession;
LAYOUT_ASSERT(SFIOSession, 0x44);

// Library state (allocated by SFIOInit). Fields are named as they are learned.
typedef struct {
    int   eState;            // 0x00  3 or 6 = idle/ready; 0xB/0xC/0xD = operation in progress
    u32   eOperation;        // 0x04  which asynchronous step is running (unsigned: switch uses cmplwi)
    int   eLastError;        // 0x08
    SFIOSession Session;     // 0x0C
    int   uSearchDirection;  // 0x50  passed to SFIONextDeviceFromMask
    int   uExpected54;       // 0x54  compared with the close result
} SFIOData;

// The 17 platform functions the host passes to SFIOInit (llSharedFileIO.c on GameCube).
typedef struct {
    int (*pfnProbe)(char* pSearchName, int eDevice);                    // 0x04 in SFIODevice
    int (*pfn08)(int eDevice);                                          // 0x08  result: free space
    int (*pfn0C)(int eDevice);                                          // 0x0C  result: free entries
    int (*pfnStartProbe)(int eDevice);                                  // 0x10
    int (*pfnSelectDevice)(int eDevice);                                // 0x14
    int (*pfnMount)(const char* pDirName, char* pFileName, int eDevice, u32 uFlags); // 0x18
    int (*pfnOp19)(int uHandle);                                        // 0x1C
    int (*pfn20)(char* pFileName, u32 uSize, int eDevice);              // 0x20
    int (*pfn24)(char* pDirName, char* pFileName, int eDevice);         // 0x24
    int (*pfn28)(char* pDirName, int eDevice);                          // 0x28
    int (*pfnRead)(int uHandle, void* pBuffer, u32 uSize);              // 0x2C
    int (*pfnWrite)(int uHandle, void* pBuffer, u32 uSize);             // 0x30
    int (*pfnSeek)(int uHandle, u32 uOffset, u32 uWhence);              // 0x34
    int (*pfnOp18)(int uHandle);                                        // 0x38
    int (*pfn3C)(int uHandle, u32 uValue);                              // 0x3C
    int (*pfnUpdate)(int* pProcess, int* pResult);                      // 0x40
    int (*pfn44)(char* pFileName);                                      // 0x44
} SFIOFuncTable;

// Platform-layer state, 0x6C bytes, allocated by SFIOInit.
typedef struct {
    u16            uAvailableMask;   // 0x00
    u8             pad2[2];
    SFIOFuncTable  fn;               // 0x04 .. 0x47 (17 entries)
    void*          pData48;          // 0x48  -> gSFIOContinueTable
    void*          pData4C;          // 0x4C  -> gSFIOValidateTable
    SFIODescriptor desc;             // 0x50  copied in by SFIOSetDescriptor
    u32            uFileSize;        // 0x60  the save file's size on the device (fn_80172D7C)
    u32            uNumFiles;        // 0x64  directory entries a save needs (fn_80172D7C)
    void*          pAllocator;       // 0x68
} SFIODevice;

extern SFIOData*   _SFIO_pData;
extern SFIODevice* _SFIO_pDevice;

void* const* CRC32_GetInterface(void);

// SharedFileIO.c: what TagFile.c calls.
int  SFIOInit(int* pDevices, const SFIOFuncTable* pFuncs, void* pAllocator);
int  SFIOShutdown(void);
int  SFIOGetSessionInfo(SFIOSession* pOut);
int  SFIOBeginLoad(const char* pName, int eDevice, int uSearchDirection);
int  SFIOBeginSave(const char* pName, int eDevice, int uSearchDirection);
int  SFIOBeginDelete(const char* pName, int eDevice, int uSearchDirection);
int  SFIOEnd(SFIOSession* pSession);
int  SFIOSeek(SFIOSession* pSession, u32 uOffset, u32 uWhence);
int  SFIORead(SFIOSession* pSession, void* pBuffer, u32 uSize);
int  SFIOWrite(SFIOSession* pSession, void* pBuffer, u32 uSize);
int  SFIOUpdate(int* pProcess, int* pResult);
int  SFIOSetDescriptor(SFIODescriptor* pDescriptor);
void SFIOPlatformCall80172F48(void* const** ppInterface);

// SharedFileIO.c: shared with the platform layer.
BOOL SFIOIsInitialized(void);
int  SFIOGetLastError(void);
void SFIOSetLastError(int eError);
int  SFIONextDeviceFromMask(u16 uDeviceMask, int uDirection);
int  SFIOStartSelectDevice(int eDevice, int* pProcess);
int  SFIOStartOp18(int* pHandle, int* pProcess);
int  SFIOStartOp19(int* pHandle, int* pProcess);

// The GameCube platform layer (NGC/SharedFileIO/llSharedFileIO.c), called by SharedFileIO.c.
void fn_80171308(int eDevice, u8* pDeviceInfo);
char* fn_801715B8(char* pSearchName, SFIODescriptor* pSysData, const char* pFilename);
void fn_80171744(char* pDirName, char* pFileName, SFIODescriptor* pSysData, const char* pSharedName);
int  fn_80171E94(int eError, int* pProcess, int* pResult);
int  fn_801727C8(int eError, int* pProcess, int* pResult);
BOOL fn_80172D54(const char* pFilename);
void fn_80172D7C(SFIODescriptor* pData, u32* pSize, u32* pEntries);
void fn_80172F48(void* const** ppInterface);
void fn_80172FA4(BOOL bEnable);
u32  fn_80173000(void);
int  fn_80173024(SFIODescriptor* pDescriptor);

// TagFile.c, called by SharedFileIO.c.
int  TagFile_SetDescriptor(SFIODescriptor* pDescriptor);

#endif
