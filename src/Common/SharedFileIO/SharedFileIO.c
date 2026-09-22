// EA shared file library, SN ProDG (GCC 2.95) without optimization.
// Original path: ../../../Source/Common/SharedFileIO/SharedFileIO.c
// Line numbers are reproduced with #line so the assert calls match.

#include "Common/SharedFileIO.h"

// Library state (allocated by SFIOCreate). Fields are named as they are learned.
typedef struct {
    int   eState;            // 0x00  3 or 6 = idle/ready; 0xB/0xC/0xD = operation in progress
    u32   eOperation;        // 0x04  which asynchronous step is running (unsigned: switch uses cmplwi)
    int   eLastError;        // 0x08
    int   uHandle;           // 0x0C  result of the last mount/open; passed to the device layer
    int   eDevice;           // 0x10  current device index (read as a short by SFIONextDeviceFromMask)
    u8    uInfo14;           // 0x14  filled by the device layer (fn_80171744)
    u8    szInfo15[0x20];    // 0x15
    u8    szName35[0x1B];    // 0x35  name buffer handed to the device layer
    int   uSearchDirection;  // 0x50  passed to SFIONextDeviceFromMask
    int   uExpected54;       // 0x54  compared with the close result
} SFIOData;

// The 17 platform functions the host passes to SFIOInit (llSharedFileIO.c on GameCube).
typedef struct {
    int (*pfnProbe)(void* pParams, int eDevice);                        // 0x04 in SFIODevice
    void* pfn08;
    void* pfn0C;
    int (*pfnStartProbe)(int eDevice);                                  // 0x10
    int (*pfnSelectDevice)(int eDevice);                                // 0x14
    int (*pfnMount)(u8* pInfo14, u8* pInfo15, int eDevice, u32 uFlags); // 0x18
    int (*pfnOp19)(int uHandle);                                        // 0x1C
    void* pfn20;
    void* pfn24;
    void* pfn28;
    int (*pfnRead)(int uHandle, void* pBuffer, u32 uSize);         // 0x2C
    int (*pfnWrite)(int uHandle, void* pBuffer, u32 uSize);        // 0x30
    int (*pfnSeek)(int uHandle, u32 uOffset, u32 uWhence);         // 0x34
    int (*pfnOp18)(int uHandle);                                        // 0x38
    void* pfn3C;
    int (*pfnUpdate)(int* pProcess, int* pResult);                    // 0x40
    void* pfn44;
} SFIOFuncTable;

// Platform-layer state, 0x6C bytes, allocated by SFIOInit.
typedef struct {
    u16           uAvailableMask;   // 0x00
    u8            pad2[2];
    SFIOFuncTable fn;               // 0x04 .. 0x47 (17 entries)
    void*         pData48;          // 0x48  -> gSFIOData210
    void*         pData4C;          // 0x4C  -> gSFIOData248
    u8            uData50[0x18];    // 0x50
    void*         pAllocator;       // 0x68
} SFIODevice;

extern SFIOData* _SFIO_pData;
extern void fn_801715B8(void* pParams, void* pDeviceData, void* pName);
extern void fn_80171744(u8* pInfo14, u8* pInfo15, void* pDeviceData, void* pName);
int SFIOStartSelectDevice(int eDevice, int* pProcess);
int SFIOStartOp18(int* pHandle, int* pProcess);
int SFIOStartOp19(int* pHandle, int* pProcess);
int SFIONextDeviceFromMask(u16 uDeviceMask, int uDirection);
extern SFIODevice* _SFIO_pDevice;
extern u8 lbl_8019D210[];
extern u8 lbl_8019D248[];

enum { SFIO_STATE_BUSY_A = 0xB, SFIO_STATE_BUSY_B = 0xC, SFIO_STATE_BUSY_C = 0xD };

// Host (CodeWarrior side) and TagFile functions. Signatures inferred from the calls.
extern void  fn_8012C8D0(void* pLock, int unused);
extern void* fn_80175C88(void* pParams);
extern int   fn_8012C98C(void* p);
extern void* memcpy(void* pDst, const void* pSrc, u32 uLen);
extern void* memset(void* pDst, int c, u32 uLen);
extern char* strcpy(char* pDst, const char* pSrc);
extern u32   strlen(const char* p);
extern void  fn_80172FA4(int);
extern void  fn_80171308(int eDevice, void* pInfo);
extern void  fn_8017124C(void* pDescriptor, void* pA, void* pB);
extern int   fn_80173024(void* pDescriptor);
extern BOOL  fn_80172D54(const char* pFilename);
u32 SFIOGetHeaderSize(void);
int SFIONumDevicesInMask(u16 uDeviceMask);
int SFIOFirstDeviceFromMask(u16 uDeviceMask);
int SFIOLastDeviceFromMask(u16 uDeviceMask);
void SFIOSetLastError(int eError);
extern void* fn_801220D4(void* pAllocator, u32 uSize, u32 uAlign, const char* pFile, int uLine);
extern void  fn_80122128(void* pAllocator, void* p, u32 uSize, u32 uAlign);

typedef struct {
    void* pAllocator;
    u32   uSize;
    u32   uUnk8;
    u32   uLock;
} SFIOCreateParams;

// Default descriptor handed to callers. Starts with 'BM6D'; meaning not yet known.
static const u32 gSFIODefaultDescriptor[14] = {
    0x424D3644, 0x00000000, 0x00003604, 0x00002800, 0x00008000, 0x00008000, 0x00000100,
    0x08000000, 0x00000040, 0x0000220B, 0x0000220B, 0x00000001, 0x00000001, 0x00000000,
};

int SFIOCreate(void* pAllocator) {
    SFIOCreateParams params;
    void* p = NULL;
    int eError = 0;
    if (pAllocator == NULL) return 3;
    params.pAllocator = pAllocator;
    params.uSize = 0x2000;
    params.uUnk8 = 0;
    fn_8012C8D0(&params.uLock, 0);
    p = fn_80175C88(&params);
    eError = fn_8012C98C(p);
    return eError;
}

const u32* SFIOGetDefaultDescriptor(void) {
    return gSFIODefaultDescriptor;
}

#line 222 "../../../Source/Common/SharedFileIO/SharedFileIO.c"
int SFIOFirstDeviceFromMask(u16 uDeviceMask) {
    int eDevice = 0;
    SFIO_ASSERT(SFIOIsInitialized());
    SFIO_ASSERT(uDeviceMask != 0);
    for (eDevice = SFIO_DEVICE_FIRST; eDevice <= SFIO_DEVICE_LAST; eDevice++) {
        if (SFIO_DEVICE_MASK(eDevice) & uDeviceMask) break;
    }
#line 240
    SFIO_ASSERT(eDevice <= SFIO_DEVICE_LAST);
    return eDevice;
}

#line 277
int SFIOLastDeviceFromMask(u16 uDeviceMask) {
    int eDevice = SFIO_DEVICE_LAST;
    SFIO_ASSERT(SFIOIsInitialized());
    SFIO_ASSERT(uDeviceMask != 0);
    for (eDevice = SFIO_DEVICE_LAST; eDevice >= SFIO_DEVICE_FIRST; eDevice--) {
        if (SFIO_DEVICE_MASK(eDevice) & uDeviceMask) break;
    }
#line 295
    SFIO_ASSERT(eDevice >= SFIO_DEVICE_FIRST);
    return eDevice;
}

// Find the next (uDirection 0) or previous (1) device in the mask after the current one.
#line 335
int SFIONextDeviceFromMask(u16 uDeviceMask, int uDirection) {
    int eResult = SFIO_DEVICE_INVALID;
    s16 sDevice = 0;
    SFIO_ASSERT(SFIOIsInitialized());
    SFIO_ASSERT(uDeviceMask != 0);
    sDevice = _SFIO_pData->eDevice;
    if (uDirection == 0) {
        sDevice++;
        while (sDevice <= SFIO_DEVICE_LAST) {
            if ((1 << sDevice) & uDeviceMask) break;
            sDevice++;
        }
        if (sDevice > SFIO_DEVICE_LAST) eResult = SFIO_DEVICE_INVALID;
        else eResult = sDevice;
    } else if (uDirection == 1) {
        sDevice--;
        while (sDevice >= SFIO_DEVICE_FIRST) {
            if ((1 << sDevice) & uDeviceMask) break;
            sDevice--;
        }
        if (sDevice < SFIO_DEVICE_FIRST) eResult = SFIO_DEVICE_INVALID;
        else eResult = sDevice;
    } else {
        return 0x12;
    }
    return eResult;
}

#line 425
int SFIONumDevicesInMask(u16 uDeviceMask) {
    u32 uNumDevices = 0;
    int eDevice = 0;
    SFIO_ASSERT(SFIOIsInitialized());
    SFIO_ASSERT(uDeviceMask != 0);
    for (eDevice = SFIO_DEVICE_FIRST; eDevice <= SFIO_DEVICE_LAST; eDevice++) {
        if (SFIO_DEVICE_MASK(eDevice) & uDeviceMask) uNumDevices++;
    }
#line 442
    SFIO_ASSERT(uNumDevices > 0);
    return uNumDevices;
}

void SFIOSetLastError(int eError) {
    if (_SFIO_pData->eState != SFIO_STATE_BUSY_A && _SFIO_pData->eState != SFIO_STATE_BUSY_B &&
        _SFIO_pData->eState != SFIO_STATE_BUSY_C) {
        _SFIO_pData->eLastError = eError;
    }
}

int SFIOGetLastError(void) {
    return _SFIO_pData->eLastError;
}

#line 535
int SFIOStartOp18(int* pDescriptor, int* pProcess) {
    SFIO_ASSERT(NULL != pDescriptor);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_A;
    _SFIO_pData->eOperation = 0x18;
    _SFIO_pDevice->fn.pfnOp18(_SFIO_pData->uHandle);
    return 0;
}

#line 581
int SFIOStartOp19(int* pDescriptor, int* pProcess) {
    SFIO_ASSERT(NULL != pDescriptor);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_B;
    _SFIO_pData->eOperation = 0x19;
    _SFIO_pDevice->fn.pfnOp19(_SFIO_pData->uHandle);
    return 0;
}

#line 629
int SFIOStartSelectDevice(int eDevice, int* pProcess) {
    SFIO_ASSERT(SFIO_DEVICE_INVALID != eDevice);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_C;
    _SFIO_pData->eOperation = 0x1A;
    _SFIO_pDevice->fn.pfnSelectDevice(eDevice);
    return 0;
}

int SFIOReturnFirstArg(int a, int b, int c) {
    return a;
}

// Completion handlers: one per operation family. Each checks that the operation it belongs to is
// the one in progress, then passes through the error codes that operation may legitimately return.
// Anything else is a bug (SFIO_ASSERT(0)) and becomes 0x12 (wrong operation) or 0x10 (bad error).

int SFIOValidateErrorOp01(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x1)) {
#line 719
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x11)) {
#line 727
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp02(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x2)) {
#line 767
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x4 || eError == 0x3 || eError == 0x9 || eError == 0x11)) {
#line 777
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOpMisc(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x1B || _SFIO_pData->eOperation == 0x1D || _SFIO_pData->eOperation == 0x1C || _SFIO_pData->eOperation == 0x7 || _SFIO_pData->eOperation == 0xB || _SFIO_pData->eOperation == 0x18)) {
#line 822
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0xE || eError == 0xD || eError == 0x11 || eError == 0xA)) {
#line 836
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp18(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x18 || _SFIO_pData->eOperation == 0x8 || _SFIO_pData->eOperation == 0xC || _SFIO_pData->eOperation == 0x10)) {
#line 879
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0x11)) {
#line 889
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp19(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x19 || _SFIO_pData->eOperation == 0x9 || _SFIO_pData->eOperation == 0xD || _SFIO_pData->eOperation == 0x11)) {
#line 932
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0x11)) {
#line 942
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp1A(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x1A)) {
#line 982
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x9 || eError == 0x11)) {
#line 991
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp05(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x5)) {
#line 1031
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x9 || eError == 0xE || eError == 0x8 || eError == 0x11)) {
#line 1042
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp16(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x16 || _SFIO_pData->eOperation == 0x6 || _SFIO_pData->eOperation == 0xE || _SFIO_pData->eOperation == 0xA)) {
#line 1085
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0xD || eError == 0x11)) {
#line 1096
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp14(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x14 || _SFIO_pData->eOperation == 0x15 || _SFIO_pData->eOperation == 0x13)) {
#line 1138
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0xD || eError == 0x6 || eError == 0x5 || eError == 0x11)) {
#line 1151
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp12(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x12)) {
#line 1191
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0xD || eError == 0x11)) {
#line 1202
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp03(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x3)) {
#line 1242
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x11)) {
#line 1250
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp04(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x4)) {
#line 1290
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x11)) {
#line 1298
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorOp17(int eError, void* pArg1, void* pArg2) {
    if (!(_SFIO_pData->eOperation == 0x17)) {
#line 1338
        SFIO_ASSERT(0);
        return 0x12;
    }
    if (!(eError == 0x0 || eError == 0x3 || eError == 0x4 || eError == 0x9 || eError == 0x11)) {
#line 1348
        SFIO_ASSERT(0);
        return 0x10;
    }
    return eError;
}

int SFIOValidateErrorPassThrough(int eError, void* pArg1, void* pArg2) {
    return eError;
}

// Asynchronous probe / mount / select sequence. Called with the result of the previous step.
// *pProcess: 1 = keep calling, 2 = finished.
int SFIOContinueSelect(int eError, int* pProcess, int* pResult) {
    u8 params[0x20];
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == 3 || _SFIO_pData->eState == 6)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 1:
        if (eError == 0) {
            fn_801715B8(params, _SFIO_pDevice->uData50, _SFIO_pData->szName35);
            _SFIO_pData->eOperation = 2;
            _SFIO_pDevice->fn.pfnProbe(params, _SFIO_pData->eDevice);
        } else if (eError == 3) {
            if (_SFIO_pData->eState == 3) {
                _SFIO_pData->eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->eDevice);
                    return 0;
                }
            }
            *pProcess = 2;
            return eError;
        } else {
            *pProcess = 2;
            return eError;
        }
        break;
    case 2:
        if (eError == 0) {
            if (*pResult == 1) {
                fn_80171744(&_SFIO_pData->uInfo14, _SFIO_pData->szInfo15, _SFIO_pDevice->uData50, _SFIO_pData->szName35);
                _SFIO_pData->eOperation = 0x16;
                _SFIO_pDevice->fn.pfnMount(&_SFIO_pData->uInfo14, _SFIO_pData->szInfo15, _SFIO_pData->eDevice, 0x80000004);
            } else if (*pResult == 0) {
                SFIOSetLastError(4);
                _SFIO_pData->eOperation = 0x1A;
                _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->eDevice);
            } else {
                *pProcess = 2;
                return 0xA;
            }
        } else {
            SFIOSetLastError(eError);
            return SFIOStartSelectDevice(_SFIO_pData->eDevice, pProcess);
        }
        break;
    case 0x16:
        if (eError == 0) {
            if (*pResult >= 0) {
                _SFIO_pData->uHandle = *pResult;
                *pProcess = 2;
                return 0;
            } else {
                _SFIO_pData->uHandle = *pResult;
                *pProcess = 2;
                return 0xA;
            }
        } else {
            SFIOSetLastError(eError);
            return SFIOStartSelectDevice(_SFIO_pData->eDevice, pProcess);
        }
        break;
    case 0x1A:
        if (eError == 0) {
            if (_SFIO_pData->eState == 4 || _SFIO_pData->eState == 6) {
                *pProcess = 2;
                return SFIOGetLastError();
            } else if (SFIOGetLastError() == 0) {
                *pProcess = 2;
                return 0;
            } else {
                _SFIO_pData->eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->eDevice);
                    return 0;
                } else {
                    *pProcess = 2;
                    return SFIOGetLastError();
                }
            }
        } else {
            *pProcess = 2;
            return eError;
        }
        break;
    default:
        *pProcess = 2;
        return 0x12;
        break;
    }
    return 0;
}

// Continuation for the close / verify operations (states 8, 9, 0xA).
int SFIOContinueClose(int eError, int* pProcess, int* pResult) {
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == 8 || _SFIO_pData->eState == 0xA || _SFIO_pData->eState == 9)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 0x1B:
        if (eError == 0) {
            *pProcess = 2;
            return 0;
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp18(&_SFIO_pData->uHandle, pProcess);
        }
        break;
    case 0x1C:
    case 0x1D:
        if (eError == 0) {
            if (_SFIO_pData->uExpected54 == *pResult) {
                *pProcess = 2;
                return 0;
            } else {
                *pProcess = 2;
                return 0xA;
            }
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp18(&_SFIO_pData->uHandle, pProcess);
        }
        break;
    default:
        *pProcess = 2;
        return 0x12;
        break;
    }
    return 0;
}

// Continuation for the unmount sequence (state 7): op 0x18 -> 0x19 -> 0x1A.
int SFIOContinueUnmount(int eError, int* pProcess, int* pResult) {
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == 7)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 0x18:
        if (eError == 0) {
            _SFIO_pData->eOperation = 0x19;
            _SFIO_pDevice->fn.pfnOp19(_SFIO_pData->uHandle);
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp19(&_SFIO_pData->uHandle, pProcess);
        }
        break;
    case 0x19:
        if (eError == 0) {
            _SFIO_pData->eOperation = 0x1A;
            _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->eDevice);
        } else {
            SFIOSetLastError(eError);
            return SFIOStartSelectDevice(_SFIO_pData->eDevice, pProcess);
        }
        break;
    case 0x1A:
        if (eError == 0) {
            *pProcess = 2;
            return 0;
        } else {
            *pProcess = 2;
            return eError;
        }
        break;
    default:
        *pProcess = 2;
        return 0x12;
        break;
    }
    return 0;
}

// Continuation after a failure while busy (states 0xB, 0xC, 0xD): unwind op 0x18 -> 0x19 -> 0x1A.
int SFIOContinueAbort(int eError, int* pProcess, int* pResult) {
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == SFIO_STATE_BUSY_A || _SFIO_pData->eState == SFIO_STATE_BUSY_B ||
          _SFIO_pData->eState == SFIO_STATE_BUSY_C)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 0x18:
        _SFIO_pData->eOperation = 0x19;
        _SFIO_pDevice->fn.pfnOp19(_SFIO_pData->uHandle);
        break;
    case 0x19:
        _SFIO_pData->eOperation = 0x1A;
        _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->eDevice);
        break;
    case 0x1A:
        *pProcess = 2;
        return SFIOGetLastError();
        break;
    default:
        *pProcess = 2;
        return 0x12;
        break;
    }
    return 0;
}

BOOL SFIOIsInitialized(void) {
    return _SFIO_pDevice != NULL;
}

// pDevices: list of device types terminated by -1 (0..1 = a slot, 3 = both slots).
int SFIOInit(int* pDevices, const SFIOFuncTable* pFuncs, void* pAllocator) {
    int* pEntry = NULL;
    u8 uCount = 0;
    if (SFIOIsInitialized()) return 1;
    if (pDevices == NULL) return 0xC;
    if (*pDevices == -1) return 0xC;
    if (pFuncs == NULL) return 0xC;
    if (!(pFuncs->pfnProbe && pFuncs->pfn08 && pFuncs->pfn0C && pFuncs->pfnStartProbe &&
          pFuncs->pfnSelectDevice && pFuncs->pfnMount && pFuncs->pfnOp19 && pFuncs->pfn20 && pFuncs->pfn24 &&
          pFuncs->pfn28 && pFuncs->pfnRead && pFuncs->pfnWrite && pFuncs->pfnSeek && pFuncs->pfnOp18 &&
          pFuncs->pfn3C && pFuncs->pfnUpdate && pFuncs->pfn44)) return 0xC;
    if (_SFIO_pDevice != NULL || _SFIO_pData != NULL) return 1;
#line 2157
    _SFIO_pDevice = fn_801220D4(pAllocator, sizeof(SFIODevice), 4, __FILE__, __LINE__);
    if (_SFIO_pDevice == NULL) return 7;
#line 2162
    _SFIO_pData = fn_801220D4(pAllocator, sizeof(SFIOData), 4, __FILE__, __LINE__);
    if (_SFIO_pData == NULL) return 7;
    _SFIO_pDevice->pAllocator = pAllocator;
    _SFIO_pDevice->uAvailableMask = 0;
    pEntry = pDevices;
    while (*pEntry != -1) {
        if (!((*pEntry <= SFIO_DEVICE_LAST && *pEntry >= SFIO_DEVICE_FIRST) || *pEntry == 3)) return 9;
        if (uCount == 2) return 0xC;
        if (*pEntry == 3) {
            signed char i = 0;
            for (i = 0; i <= SFIO_DEVICE_LAST; i++) {
                _SFIO_pDevice->uAvailableMask |= SFIO_DEVICE_MASK(i);
            }
        } else {
            _SFIO_pDevice->uAvailableMask |= SFIO_DEVICE_MASK(*pEntry);
        }
        pEntry++;
        uCount++;
    }
    _SFIO_pDevice->fn.pfnProbe = pFuncs->pfnProbe;
    _SFIO_pDevice->fn.pfn08 = pFuncs->pfn08;
    _SFIO_pDevice->fn.pfn0C = pFuncs->pfn0C;
    _SFIO_pDevice->fn.pfnStartProbe = pFuncs->pfnStartProbe;
    _SFIO_pDevice->fn.pfnSelectDevice = pFuncs->pfnSelectDevice;
    _SFIO_pDevice->fn.pfnMount = pFuncs->pfnMount;
    _SFIO_pDevice->fn.pfnOp19 = pFuncs->pfnOp19;
    _SFIO_pDevice->fn.pfn20 = pFuncs->pfn20;
    _SFIO_pDevice->fn.pfn24 = pFuncs->pfn24;
    _SFIO_pDevice->fn.pfn28 = pFuncs->pfn28;
    _SFIO_pDevice->fn.pfnRead = pFuncs->pfnRead;
    _SFIO_pDevice->fn.pfnWrite = pFuncs->pfnWrite;
    _SFIO_pDevice->fn.pfnSeek = pFuncs->pfnSeek;
    _SFIO_pDevice->fn.pfnOp18 = pFuncs->pfnOp18;
    _SFIO_pDevice->fn.pfn3C = pFuncs->pfn3C;
    _SFIO_pDevice->fn.pfnUpdate = pFuncs->pfnUpdate;
    _SFIO_pDevice->fn.pfn44 = pFuncs->pfn44;
    _SFIO_pDevice->pData48 = lbl_8019D210;
    _SFIO_pDevice->pData4C = lbl_8019D248;
    _SFIO_pData->eState = 0;
    _SFIO_pData->eOperation = 0;
    return 0;
}

int SFIOShutdown(void) {
    if (!SFIOIsInitialized()) return 2;
    if (!(_SFIO_pDevice != NULL && _SFIO_pData != NULL)) return 2;
    _SFIO_pDevice->fn.pfnProbe = NULL;
    _SFIO_pDevice->fn.pfn08 = NULL;
    _SFIO_pDevice->fn.pfn0C = NULL;
    _SFIO_pDevice->fn.pfnStartProbe = NULL;
    _SFIO_pDevice->fn.pfnSelectDevice = NULL;
    _SFIO_pDevice->fn.pfnMount = NULL;
    _SFIO_pDevice->fn.pfnOp19 = NULL;
    _SFIO_pDevice->fn.pfn20 = NULL;
    _SFIO_pDevice->fn.pfn24 = NULL;
    _SFIO_pDevice->fn.pfn28 = NULL;
    _SFIO_pDevice->fn.pfnRead = NULL;
    _SFIO_pDevice->fn.pfnWrite = NULL;
    _SFIO_pDevice->fn.pfnSeek = NULL;
    _SFIO_pDevice->fn.pfnOp18 = NULL;
    _SFIO_pDevice->fn.pfn3C = NULL;
    _SFIO_pDevice->fn.pfnUpdate = NULL;
    _SFIO_pDevice->fn.pfn44 = NULL;
    _SFIO_pData->eState = 0;
    _SFIO_pData->eOperation = 0;
    fn_80122128(_SFIO_pDevice->pAllocator, _SFIO_pData, sizeof(SFIOData), 4);
    _SFIO_pData = NULL;
    fn_80122128(_SFIO_pDevice->pAllocator, _SFIO_pDevice, sizeof(SFIODevice), 4);
    _SFIO_pDevice = NULL;
    return 0;
}

// Copies the session part of the state (uHandle .. uExpected54) out to the caller.
int SFIOGetSessionInfo(void* pOut) {
    if (pOut == NULL) return 0xC;
    memcpy(pOut, &_SFIO_pData->uHandle, 0x44);
    return 0;
}

// Begin a save session: pick the device (or start searching for one) and probe it.
int SFIOBeginLoad(const char* pName, int eDevice, int uSearchDirection) {
    if (!SFIOIsInitialized()) return 2;
    if (pName == NULL) return 0xC;
    if (!(eDevice == SFIO_DEVICE_INVALID || (eDevice <= SFIO_DEVICE_LAST && eDevice >= SFIO_DEVICE_FIRST))) return 9;
    if (eDevice != SFIO_DEVICE_INVALID) {
        if (!(u16)((1 << eDevice) & _SFIO_pDevice->uAvailableMask)) return 0xB;
    }
    if (_SFIO_pData->eState != 0) return 0xD;
    memset(&_SFIO_pData->uHandle, 0, 0x44);
    if (eDevice == SFIO_DEVICE_INVALID) {
        if (SFIONumDevicesInMask(_SFIO_pDevice->uAvailableMask) == 1) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 5;
        } else if (uSearchDirection == 0) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 2;
        } else if (uSearchDirection == 1) {
            _SFIO_pData->eDevice = SFIOLastDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 2;
        } else {
            return 0xC;
        }
    } else {
        _SFIO_pData->eDevice = eDevice;
        _SFIO_pData->eState = 5;
    }
    SFIOSetLastError(0);
    strcpy((char*)_SFIO_pData->szName35, pName);
    _SFIO_pData->uSearchDirection = uSearchDirection;
    fn_80172FA4(1);
    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->eDevice);
    _SFIO_pData->eOperation = 1;
    return 0;
}

// Three entry points share one body; they differ only in the state they leave behind
// (single device / searching): Load 5/2, Save 4/1, Delete 6/3. Save also limits the name length.
int SFIOBeginSave(const char* pName, int eDevice, int uSearchDirection) {
    if (!SFIOIsInitialized()) return 2;
    if (pName == NULL) return 0xC;
    if (strlen(pName) > 25) return 0xC;
    if (!(eDevice == SFIO_DEVICE_INVALID || (eDevice <= SFIO_DEVICE_LAST && eDevice >= SFIO_DEVICE_FIRST))) return 9;
    if (eDevice != SFIO_DEVICE_INVALID) {
        if (!(u16)((1 << eDevice) & _SFIO_pDevice->uAvailableMask)) return 0xB;
    }
    if (_SFIO_pData->eState != 0) return 0xD;
    memset(&_SFIO_pData->uHandle, 0, 0x44);
    if (eDevice == SFIO_DEVICE_INVALID) {
        if (SFIONumDevicesInMask(_SFIO_pDevice->uAvailableMask) == 1) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 4;
        } else if (uSearchDirection == 0) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 1;
        } else if (uSearchDirection == 1) {
            _SFIO_pData->eDevice = SFIOLastDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 1;
        } else {
            return 0xC;
        }
    } else {
        _SFIO_pData->eDevice = eDevice;
        _SFIO_pData->eState = 4;
    }
    SFIOSetLastError(0);
    strcpy((char*)_SFIO_pData->szName35, pName);
    _SFIO_pData->uSearchDirection = uSearchDirection;
    fn_80172FA4(1);
    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->eDevice);
    _SFIO_pData->eOperation = 1;
    return 0;
}

int SFIOBeginDelete(const char* pName, int eDevice, int uSearchDirection) {
    if (!SFIOIsInitialized()) return 2;
    if (pName == NULL) return 0xC;

    if (!(eDevice == SFIO_DEVICE_INVALID || (eDevice <= SFIO_DEVICE_LAST && eDevice >= SFIO_DEVICE_FIRST))) return 9;
    if (eDevice != SFIO_DEVICE_INVALID) {
        if (!(u16)((1 << eDevice) & _SFIO_pDevice->uAvailableMask)) return 0xB;
    }
    if (_SFIO_pData->eState != 0) return 0xD;
    memset(&_SFIO_pData->uHandle, 0, 0x44);
    if (eDevice == SFIO_DEVICE_INVALID) {
        if (SFIONumDevicesInMask(_SFIO_pDevice->uAvailableMask) == 1) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 6;
        } else if (uSearchDirection == 0) {
            _SFIO_pData->eDevice = SFIOFirstDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 3;
        } else if (uSearchDirection == 1) {
            _SFIO_pData->eDevice = SFIOLastDeviceFromMask(_SFIO_pDevice->uAvailableMask);
            _SFIO_pData->eState = 3;
        } else {
            return 0xC;
        }
    } else {
        _SFIO_pData->eDevice = eDevice;
        _SFIO_pData->eState = 6;
    }
    SFIOSetLastError(0);
    strcpy((char*)_SFIO_pData->szName35, pName);
    _SFIO_pData->uSearchDirection = uSearchDirection;
    fn_80172FA4(1);
    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->eDevice);
    _SFIO_pData->eOperation = 1;
    return 0;
}

// Close the session: copy the caller's session info back and start the unmount (op 0x18).
int SFIOEnd(int* pSession) {
    if (!SFIOIsInitialized()) return 2;
    if (pSession == NULL) return 0xC;
    if (_SFIO_pData->eState != 0) return 0xD;
    memcpy(&_SFIO_pData->uHandle, pSession, 0x44);
    _SFIO_pData->eState = 7;
    _SFIO_pData->eOperation = 0x18;
    _SFIO_pDevice->fn.pfnOp18(*pSession);
    return 0;
}

// Seek within the open save. uWhence 0 = relative to the header, otherwise absolute.
int SFIOSeek(int* pSession, u32 uOffset, u32 uWhence) {
    if (!SFIOIsInitialized()) return 2;
    if (pSession == NULL) return 0xC;
    if (_SFIO_pData->eState != 0) return 0xD;
    _SFIO_pData->eState = 8;
    _SFIO_pData->eOperation = 0x1B;
    SFIOSetLastError(0);
    memcpy(&_SFIO_pData->uHandle, pSession, 0x44);
    if (uWhence == 0) {
        _SFIO_pData->uExpected54 = uOffset + SFIOGetHeaderSize();
        _SFIO_pDevice->fn.pfnSeek(_SFIO_pData->uHandle, uOffset + SFIOGetHeaderSize(), uWhence);
    } else {
        _SFIO_pData->uExpected54 = uOffset;
        _SFIO_pDevice->fn.pfnSeek(_SFIO_pData->uHandle, uOffset, uWhence);
    }
    return 0;
}

int SFIORead(int* pSession, void* pBuffer, u32 uSize) {
    if (!SFIOIsInitialized()) return 2;
    if (pSession == NULL) return 0xC;
    if (_SFIO_pData->eState != 0) return 0xD;
    _SFIO_pData->eState = 0xA;
    _SFIO_pData->eOperation = 0x1D;
    SFIOSetLastError(0);
    memcpy(&_SFIO_pData->uHandle, pSession, 0x44);
    _SFIO_pData->uExpected54 = uSize;
    _SFIO_pDevice->fn.pfnRead(*pSession, pBuffer, uSize);
    return 0;
}

int SFIOWrite(int* pSession, void* pBuffer, u32 uSize) {
    if (!SFIOIsInitialized()) return 2;
    if (pSession == NULL) return 0xC;
    if (_SFIO_pData->eState != 0) return 0xD;
    _SFIO_pData->eState = 9;
    _SFIO_pData->eOperation = 0x1C;
    SFIOSetLastError(0);
    memcpy(&_SFIO_pData->uHandle, pSession, 0x44);
    _SFIO_pData->uExpected54 = uSize;
    _SFIO_pDevice->fn.pfnWrite(*pSession, pBuffer, uSize);
    return 0;
}

// Dispatch tables (in .data): the continuation table is indexed by eState, the error-validation
// table by eOperation.
extern int (*const gSFIOContinueTable[])(int eError, int* pProcess, int* pResult);
extern int (*const gSFIOValidateTable[])(int eError, int uProcess, int uResult);

// Main pump. Drives the platform layer, filters the error, then runs this state's continuation.
int SFIOUpdate(int* pProcess, int* pResult) {
    int eError = 0;
    if (!SFIOIsInitialized()) return 2;
    if (_SFIO_pDevice->fn.pfnUpdate == NULL) return 0xD;
    if (pProcess == NULL) return 0xC;
    if (pResult == NULL) return 0xC;
    eError = _SFIO_pDevice->fn.pfnUpdate(pProcess, pResult);
    if (*pProcess == 0 && _SFIO_pData->eState != 0) return 0xF;
    if (*pProcess == 2) {
        eError = ((int (*const*)(int, int, int))_SFIO_pDevice->pData4C)[_SFIO_pData->eOperation](eError, *pProcess, *pResult);
        if (eError == 0xA) {
            _SFIO_pData->eState = 0;
            _SFIO_pData->eOperation = 0;
            return eError;
        }
        if (eError == 0x10) return eError;
        eError = ((int (*const*)(int, int*, int*))_SFIO_pDevice->pData48)[_SFIO_pData->eState](eError, pProcess, pResult);
        if (*pProcess == 2) {
            if (_SFIO_pData->eState == 5 || _SFIO_pData->eState == 2 || _SFIO_pData->eState == 7 ||
                _SFIO_pData->eState == SFIO_STATE_BUSY_A || _SFIO_pData->eState == SFIO_STATE_BUSY_B ||
                _SFIO_pData->eState == SFIO_STATE_BUSY_C) {
                fn_80172FA4(0);
            }
            _SFIO_pData->eState = 0;
            _SFIO_pData->eOperation = 0;
        }
    }
    return eError;
}

int SFIOGetDeviceInfo(int eDevice, void* pInfo) {
    if (pInfo == NULL) return 0xC;
    if (!SFIOIsInitialized()) return 2;
    if (!(eDevice <= SFIO_DEVICE_LAST && eDevice >= SFIO_DEVICE_FIRST)) return 9;
    fn_80171308(eDevice, pInfo);
    return 0;
}

int SFIOSetDescriptor(void* pDescriptor) {
    if (pDescriptor == NULL) return 0xC;
    if (!SFIOIsInitialized()) return 2;
    memcpy(_SFIO_pDevice->uData50, pDescriptor, 0x10);
    fn_8017124C(pDescriptor, &_SFIO_pDevice->uData50[0x10], &_SFIO_pDevice->uData50[0x14]);
    return fn_80173024(pDescriptor);
}

#line 3286
BOOL SFIOValidateFilename(const char* pFilename) {
    SFIO_ASSERT(NULL != pFilename);
    return fn_80172D54(pFilename);
}

// Convert an ASCII save name to full-width Shift-JIS (what the memory card directory stores).
// Letters and digits map arithmetically; punctuation goes through the table. Output is byte-swapped.
void SFIOAsciiToShiftJIS(const char* pSrc, u16* pDst) {
    u8 i = 0;
    u16 c;
    // An unsized array from a 32-character literal: 33 bytes are copied (the NUL included), and the
    // lookups can index the terminator. Any other spelling changes the copy sequence.
    u8 aTable[] = "\x40\x49\x68\x94\x90\x93\x95\x66\x69\x6A\x96\x7B\x43\x7C\x44\x5E"
                    "\x46\x47\x71\x81\x72\x48\x97\x6D\x8F\x6E\x4F\x51\x65\x6F\x62\x70";
    while ((c = *pSrc++) != 0) {
        if (c > 0x80 && c <= 0x98) {
            c = (c << 8) + *pSrc++;
        } else if (c > 0x7A) {
            c = aTable[c - 0x5E] + 0x8100;
        } else if (c > 0x60) {
            c = c + 0x8220;
        } else if (c > 0x5A) {
            c = aTable[c - 0x44] + 0x8100;
        } else if (c > 0x40) {
            c = c + 0x821F;
        } else if (c > 0x39) {
            c = aTable[c - 0x2A] + 0x8100;
        } else if (c > 0x2F) {
            c = c + 0x821F;
        } else {
            c = aTable[c - 0x20] + 0x8100;
        }
        c = (c >> 8) + ((c & 0xFF) << 8);
        pDst[i++] = c;
    }
}
