// EA shared file library, SN ProDG (GCC 2.95) without optimization.
// Original path: ../../../Source/NGC/SharedFileIO/llSharedFileIO.c
// Line numbers are reproduced with #line so the assert calls match.
//
// The GameCube layer under SharedFileIO.c: maps SFIO devices to memory card slots, builds the
// save's file name, sets the banner and icon of a new save, and runs the save and load sequences
// (probe, check free space, mount, write the header block). The library itself is cross-platform
// (the icon names "View.ico" and "icon.sys" are the PlayStation 2's).

#include "Common/SharedFileIO.h"
#include "core/card.h"

#define SFIO_HEADER_SIZE 0x2000     // the header block at the start of every save: one card block

// The CARD channel (slot A or B) of an SFIO device.
void fn_80171308(int eDevice, u8* pDeviceInfo) {
#line 104 "../../../Source/NGC/SharedFileIO/llSharedFileIO.c"
    SFIO_ASSERT(NULL != pDeviceInfo);
    switch (eDevice) {
    case SFIO_DEVICE_FIRST:
        *pDeviceInfo = 0;
        break;
    case SFIO_DEVICE_LAST:
        *pDeviceInfo = 1;
        break;
    default:
#line 109
        SFIO_ASSERT(FALSE);
        break;
    }
}

// Set up a new save's directory entry: no banner, one RGB5A3 icon frame shown slowly, the icon
// image at 0x40 and the comment at the start of the file.
void fn_80171400(s32 nFileNo, int eDevice) {
    u8 uChan;
    s32 uErr = 0;
    CARDStat stat;
    fn_80171308(eDevice, &uChan);
    uErr = CARDGetStatus(uChan, nFileNo, &stat);
#line 122
    SFIO_ASSERT(CARD_RESULT_READY == uErr);
    // The CARD library's setter macros, expanded: clear the field, OR in the value (even a 0; the
    // match needs the OR). n in "<< (2 * n)" is the icon frame.
    stat.bannerFormat = (u8)((stat.bannerFormat & ~3) | 0);                            // no banner
    stat.iconFormat = (u16)((stat.iconFormat & ~(3 << (2 * 0))) | (2 << (2 * 0)));  // frame 0: RGB5A3
    stat.iconSpeed = (u16)((stat.iconSpeed & ~(3 << (2 * 0))) | (3 << (2 * 0)));    // frame 0: slow
    stat.iconSpeed = (u16)((stat.iconSpeed & ~(3 << (2 * 1))) | (0 << (2 * 1)));    // frame 1: the end
    stat.bannerFormat = (u8)((stat.bannerFormat & ~4) | 0);                            // loop the icon
    stat.iconAddr = 0x40;
    stat.commentAddr = 0;
    uErr = CARDSetStatus(uChan, nFileNo, &stat);
#line 133
    SFIO_ASSERT(CARD_RESULT_READY == uErr);
}

// The name to look for on the card: the save's file name. Returns the end of the name.
char* fn_801715B8(char* pSearchName, SFIODescriptor* pSysData, const char* pFilename) {
    char* p = pSearchName;
#line 161
    SFIO_ASSERT(NULL != pSearchName);
    SFIO_ASSERT(NULL != pSysData);
    SFIO_ASSERT(NULL != pFilename);
    memset(p, 0, 0x20);
    memcpy(p, pFilename, strlen(pFilename));
    p = p + strlen(pFilename);
    return p;
}

// The directory and file name a save is created under: no directory, the host's name as it is.
void fn_80171744(char* pDirName, char* pFileName, SFIODescriptor* pSysData, const char* pSharedName) {
    char* p = NULL;
#line 185
    SFIO_ASSERT(NULL != pDirName);
    SFIO_ASSERT(NULL != pFileName);
    SFIO_ASSERT(NULL != pSysData);
    SFIO_ASSERT(NULL != pSharedName);
    p = pDirName;
    memset(p, 0, 1);
    p = pFileName;
    memset(p, 0, 0x20);
    memcpy(p, pSharedName, strlen(pSharedName));
    p = p + strlen(pSharedName);
}

// The next three build "<directory>/<file>" paths (PlayStation 2 style; unused on the GameCube).
char* fn_8017193C(char* pSaveName, const char* pDirectory) {
    char* p = pSaveName;
    u8 i = 0;
#line 277
    SFIO_ASSERT(NULL != pSaveName);
    SFIO_ASSERT(NULL != pDirectory);
    memset(p, 0, 0x20);
    memcpy(p, pDirectory, 1);
    for (i = 0; i < 1; i++) {
        if (*p == 0) {
            break;
        } else {
            p++;
        }
    }
    memcpy(p, "/", 1);
    p++;
    memcpy(p, pDirectory, 1);
    return p;
}

char* fn_80171ADC(char* pIconName, const char* pDirectory) {
    char* p = pIconName;
    char szIcon[] = "View.ico";
    u8 i = 0;
#line 338
    SFIO_ASSERT(NULL != pIconName);
    SFIO_ASSERT(NULL != pDirectory);
    memset(p, 0, 0x20);
    memcpy(p, pDirectory, 1);
    for (i = 0; i < 1; i++) {
        if (*p == 0) {
            break;
        } else {
            p++;
        }
    }
    memcpy(p, "/", 1);
    p++;
    memcpy(p, szIcon, strlen(szIcon));
    return p;
}

char* fn_80171CB8(char* pSystemName, const char* pDirectory) {
    char* p = pSystemName;
    char szSystem[] = "icon.sys";
    u8 i = 0;
#line 399
    SFIO_ASSERT(NULL != pSystemName);
    SFIO_ASSERT(NULL != pDirectory);
    memset(p, 0, 0x20);
    memcpy(p, pDirectory, 1);
    for (i = 0; i < 1; i++) {
        if (*p == 0) {
            break;
        } else {
            p++;
        }
    }
    memcpy(p, "/", 1);
    p++;
    memcpy(p, szSystem, strlen(szSystem));
    return p;
}

// Continuation for a save (states 1 and 4): probe for the file, which must not exist yet, check the
// free space and directory entries, create and open it, write the header block, set its banner and
// icon, close it and unmount. When searching (state 1), a failure moves on to the next device.
// *pProcess: 1 = keep calling, 2 = finished.
int fn_80171E94(int eError, int* pProcess, int* pResult) {
    char szSearchName[0x20];
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == 1 || _SFIO_pData->eState == 4)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 1:
        if (eError == 0) {
            fn_801715B8(szSearchName, &_SFIO_pDevice->desc, _SFIO_pData->Session.szName);
            _SFIO_pData->eOperation = 2;
            _SFIO_pDevice->fn.pfnProbe(szSearchName, _SFIO_pData->Session.eDevice);
        } else if (eError == 3) {
            if (_SFIO_pData->eState == 1) {
                _SFIO_pData->Session.eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->Session.eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->Session.eDevice);
                    return 0;
                }
                *pProcess = 2;
                return SFIOGetLastError();
            } else {
                *pProcess = 2;
                return eError;
            }
        } else {
            *pProcess = 2;
            return eError;
        }
        break;
    case 2:
        if (eError == 0) {
            if (*pResult == 0) {
                _SFIO_pData->eOperation = 3;
                _SFIO_pDevice->fn.pfn08(_SFIO_pData->Session.eDevice);
            } else if (*pResult == 1) {
                SFIOSetLastError(8);    // the save already exists
                _SFIO_pData->eOperation = 0x1A;
                _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->Session.eDevice);
            } else {
                *pProcess = 2;
                return 0xA;
            }
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 3:
        if (eError == 0) {
            if (*pResult < 0) {
                *pProcess = 2;
                return 0xA;
            } else if (_SFIO_pDevice->uFileSize <= *pResult) {
                _SFIO_pData->eOperation = 4;
                _SFIO_pDevice->fn.pfn0C(_SFIO_pData->Session.eDevice);
            } else {
                SFIOSetLastError(0xE);  // not enough space
                _SFIO_pData->eOperation = 0x1A;
                _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->Session.eDevice);
            }
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 4:
        if (eError == 0) {
            if (*pResult < 0) {
                *pProcess = 2;
                return 0xA;
            } else if (_SFIO_pDevice->uNumFiles <= *pResult) {
                fn_80171744(_SFIO_pData->Session.szDirName, _SFIO_pData->Session.szFileName, &_SFIO_pDevice->desc,
                            _SFIO_pData->Session.szName);
                _SFIO_pData->eOperation = 5;
                _SFIO_pDevice->fn.pfn20(_SFIO_pData->Session.szFileName, _SFIO_pDevice->uFileSize, _SFIO_pData->Session.eDevice);
            } else {
                SFIOSetLastError(0xE);  // no free directory entry
                _SFIO_pData->eOperation = 0x1A;
                _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->Session.eDevice);
            }
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 5:
        if (eError == 0) {
            _SFIO_pData->eOperation = 6;
            _SFIO_pDevice->fn.pfnMount("", _SFIO_pData->Session.szFileName, _SFIO_pData->Session.eDevice, 4);
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 6:
        if (eError == 0) {
            _SFIO_pData->Session.uHandle = *pResult;
            _SFIO_pData->eOperation = 7;
            _SFIO_pDevice->fn.pfnWrite(_SFIO_pData->Session.uHandle, _SFIO_pDevice->desc.pHeader,
                                       _SFIO_pDevice->desc.uSize04 + _SFIO_pDevice->desc.uSize08);
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 7:
        if (eError == 0) {
            _SFIO_pData->eOperation = 0x17;
            _SFIO_pDevice->fn.pfn3C(_SFIO_pData->Session.uHandle, 0x44);
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp18(&_SFIO_pData->Session.uHandle, pProcess);
        }
        break;
    case 0x17:
        if (eError == 0) {
            fn_80171400(_SFIO_pData->Session.uHandle, _SFIO_pData->Session.eDevice);
            _SFIO_pData->eOperation = 8;
            _SFIO_pDevice->fn.pfnOp18(_SFIO_pData->Session.uHandle);
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp18(&_SFIO_pData->Session.uHandle, pProcess);
        }
        break;
    case 8:
        if (eError == 0) {
            _SFIO_pData->eOperation = 9;
            _SFIO_pDevice->fn.pfnOp19(_SFIO_pData->Session.uHandle);
        } else {
            SFIOSetLastError(eError);
            return SFIOStartOp19(&_SFIO_pData->Session.uHandle, pProcess);
        }
        break;
    case 9:
        if (eError == 0) {
            _SFIO_pData->eOperation = 0x1A;
            _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->Session.eDevice);
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 0x1A:
        if (eError == 0) {
            if (_SFIO_pData->eState == 4) {
                *pProcess = 2;
                return SFIOGetLastError();
            } else if (SFIOGetLastError() == 0) {
                *pProcess = 2;
                return 0;
            } else {
                _SFIO_pData->Session.eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->Session.eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->Session.eDevice);
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

// Continuation for a load (states 2 and 5): probe for the file, which must exist, open it and
// select the device again. When searching, a failure moves on to the next device.
int fn_801727C8(int eError, int* pProcess, int* pResult) {
    char szSearchName[0x20];
    if (pProcess == NULL) return 0x12;
    if (pResult == NULL) return 0x12;
    if (!(_SFIO_pData->eState == 2 || _SFIO_pData->eState == 5)) return 0x12;
    *pProcess = 1;
    switch (_SFIO_pData->eOperation) {
    case 1:
        SFIOSetLastError(0);
        if (eError == 0) {
            fn_801715B8(szSearchName, &_SFIO_pDevice->desc, _SFIO_pData->Session.szName);
            _SFIO_pData->eOperation = 2;
            _SFIO_pDevice->fn.pfnProbe(szSearchName, _SFIO_pData->Session.eDevice);
        } else if (eError == 3) {
            // EA bug: tests the save state (1) in the load continuation (states 2 and 5), so a
            // failed probe ends the search instead of moving on to the next device.
            if (_SFIO_pData->eState == 1) {
                _SFIO_pData->Session.eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->Session.eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->Session.eDevice);
                    return 0;
                }
                *pProcess = 2;
                return SFIOGetLastError();
            } else {
                *pProcess = 2;
                return eError;
            }
        } else {
            *pProcess = 2;
            return eError;
        }
        break;
    case 2:
        if (eError == 0) {
            if (*pResult == 1) {
                _SFIO_pDevice->fn.pfn44(_SFIO_pData->Session.szFileName);
                _SFIO_pData->eOperation = 0x15;
                _SFIO_pDevice->fn.pfn24(_SFIO_pData->Session.szDirName, _SFIO_pData->Session.szFileName, _SFIO_pData->Session.eDevice);
            } else if (*pResult == 0) {
                SFIOSetLastError(4);    // no such save
                return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
            } else {
                *pProcess = 2;
                return 0xA;
            }
        } else {
            SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 0x15:
        if (eError == 3) {
            if (SFIOGetLastError() == 0) SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        } else {
            if (eError != 4 && SFIOGetLastError() == 0) SFIOSetLastError(eError);
            _SFIO_pData->eOperation = 0x12;
            _SFIO_pDevice->fn.pfn28(_SFIO_pData->Session.szDirName, _SFIO_pData->Session.eDevice);
        }
        break;
    case 0x12:
        if (eError == 0) {
            if (SFIOGetLastError() == 0) SFIOSetLastError(eError);
            _SFIO_pData->eOperation = 0x1A;
            _SFIO_pDevice->fn.pfnSelectDevice(_SFIO_pData->Session.eDevice);
        } else {
            if (SFIOGetLastError() == 0) SFIOSetLastError(eError);
            return fn_8016DA84_StartUnmount(_SFIO_pData->Session.eDevice, pProcess);
        }
        break;
    case 0x1A:
        if (eError == 0) {
            if (_SFIO_pData->eState == 5) {
                *pProcess = 2;
                return SFIOGetLastError();
            } else if (SFIOGetLastError() == 0) {
                *pProcess = 2;
                return 0;
            } else {
                _SFIO_pData->Session.eDevice = SFIONextDeviceFromMask(_SFIO_pDevice->uAvailableMask, _SFIO_pData->uSearchDirection);
                if (_SFIO_pData->Session.eDevice != SFIO_DEVICE_INVALID) {
                    _SFIO_pData->eOperation = 1;
                    _SFIO_pDevice->fn.pfnStartProbe(_SFIO_pData->Session.eDevice);
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

// Any file name is valid on the GameCube.
BOOL fn_80172D54(const char* pFilename) {
    return TRUE;
}

// The size of a save on the card (the header block plus the data, in whole blocks) and the
// directory entries it takes.
void fn_80172D7C(SFIODescriptor* pData, u32* pSize, u32* pEntries) {
    u32 uDataSize = 0;
    u32 uHeaderSize = 0;
    u32 uSize = 0;
#line 1400
    SFIO_ASSERT(NULL != pData);
    SFIO_ASSERT(NULL != pSize);
    SFIO_ASSERT(NULL != pEntries);
    uDataSize = pData->uDataSize;
    uHeaderSize = SFIO_HEADER_SIZE;
    uSize = uDataSize + uHeaderSize;
    uSize = uSize % 0x2000 != 0 ? (uSize / 0x2000 + 1) * 0x2000 : uSize / 0x2000 * 0x2000;
    *pSize = uSize;
    *pEntries = 1;
}

// The checksum the library uses on this platform: CRC32.
void fn_80172F48(void* const** ppInterface) {
    void* const* pInterface = NULL;
    pInterface = CRC32_GetInterface();
    if (ppInterface != NULL) {
        *ppInterface = pInterface;
    }
}

// Allow or forbid access to other games' files for the length of an operation.
void fn_80172FA4(BOOL bEnable) {
    BOOL bPrevious = FALSE;
    bPrevious = __CARDEnableGlobal(bEnable);
}

u32 fn_80173000(void) {
    return SFIO_HEADER_SIZE;
}

// Check a save descriptor: the header image must be 32-byte aligned (the card is written by DMA)
// and exactly one block long.
int fn_80173024(SFIODescriptor* pDescriptor) {
    if (pDescriptor == NULL) return 0xC;
    if ((uptr)pDescriptor->pHeader % 32 != 0) return 0xC;
    if ((pDescriptor->uSize04 + pDescriptor->uSize08) % 0x2000 != 0) return 0xC;
    if (pDescriptor->uSize04 + pDescriptor->uSize08 != SFIO_HEADER_SIZE) return 0xC;
    return 0;
}
