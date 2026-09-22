// EA shared file library, SN ProDG (GCC 2.95) without optimization.
// Original path: ../../../Source/Common/SharedFileIO/SharedFileIO.c
// Line numbers are reproduced with #line so the assert calls match.

#include "Common/SharedFileIO.h"

// Library state (allocated by SFIOCreate). Fields are named as they are learned.
typedef struct {
    int   eState;            // 0x00  0xB/0xC/0xD while an operation is in progress
    int   eOperation;        // 0x04  0x18/0x19/0x1A
    void* pUser;             // 0x08  set by SFIOSetUserData; blocked while busy
    void* pDescriptor;       // 0x0C
    u8    pad10[2];
    s16   sCurrentDevice;    // 0x12
} SFIOData;

// Function table of the platform layer (llSharedFileIO.c).
typedef struct {
    u8 pad0[0x14];
    int (*pfnSelectDevice)(int eDevice);   // 0x14
    u8 pad18[4];
    int (*pfnOp19)(void* pDescriptor);     // 0x1C
    u8 pad20[0x18];
    int (*pfnOp18)(void* pDescriptor);     // 0x38
} SFIODeviceFuncs;

extern SFIOData* _SFIO_pData;
extern SFIODeviceFuncs* _SFIO_pDevice;

enum { SFIO_STATE_BUSY_A = 0xB, SFIO_STATE_BUSY_B = 0xC, SFIO_STATE_BUSY_C = 0xD };

// Host (CodeWarrior side) and TagFile functions. Signatures inferred from the calls.
extern void  fn_8012C8D0(void* pLock, int unused);
extern void* fn_80175C88(void* pParams);
extern int   fn_8012C98C(void* p);

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
#line 336
int SFIONextDeviceFromMask(u16 uDeviceMask, int uDirection) {
    int eResult = SFIO_DEVICE_INVALID;
    s16 sDevice = 0;
    SFIO_ASSERT(SFIOIsInitialized());
    SFIO_ASSERT(uDeviceMask != 0);
    sDevice = _SFIO_pData->sCurrentDevice;
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

#line 426
int SFIONumDevicesInMask(u16 uDeviceMask) {
    int uNumDevices = 0;
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

void SFIOSetUserData(void* pUser) {
    if (_SFIO_pData->eState != SFIO_STATE_BUSY_A && _SFIO_pData->eState != SFIO_STATE_BUSY_B &&
        _SFIO_pData->eState != SFIO_STATE_BUSY_C) {
        _SFIO_pData->pUser = pUser;
    }
}

void* SFIOGetUserData(void) {
    return _SFIO_pData->pUser;
}

#line 535
int SFIOStartOp18(void* pDescriptor, int* pProcess) {
    SFIO_ASSERT(NULL != pDescriptor);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_A;
    _SFIO_pData->eOperation = 0x18;
    _SFIO_pDevice->pfnOp18(_SFIO_pData->pDescriptor);
    return 0;
}

#line 581
int SFIOStartOp19(void* pDescriptor, int* pProcess) {
    SFIO_ASSERT(NULL != pDescriptor);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_B;
    _SFIO_pData->eOperation = 0x19;
    _SFIO_pDevice->pfnOp19(_SFIO_pData->pDescriptor);
    return 0;
}

#line 629
int SFIOStartSelectDevice(int eDevice, int* pProcess) {
    SFIO_ASSERT(SFIO_DEVICE_INVALID != eDevice);
    SFIO_ASSERT(NULL != pProcess);
    *pProcess = 1;
    _SFIO_pData->eState = SFIO_STATE_BUSY_C;
    _SFIO_pData->eOperation = 0x1A;
    _SFIO_pDevice->pfnSelectDevice(eDevice);
    return 0;
}

int SFIOReturnFirstArg(int a, int b, int c) {
    return a;
}
