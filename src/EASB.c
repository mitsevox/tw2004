// EASB.c (our name, after the library's "EASB" file tag): the public calls of the EA Sports Bio
// library, the ones EASportsBio.c makes. Each checks the library's state (lbl_802825B8) and the
// memory-card session first, then hands the work to the storage code before it.

#include "core/easb.h"
#include "platform.h"

// Checks that the library is running with a valid product, and that the Bio file is open or
// closed as the call needs (EASB_NEED_*).
EASBErrorE fn_8012CCD8(s32 nNeed) {
    if (lbl_802825B8 == NULL) return EASB_ERROR_NOT_INITIALIZED;
    if (lbl_802825B8->product.bValid == 0) return EASB_ERROR_INVALID_PRODUCT;
    if (fn_8012C83C() == 1 && !fn_8012C848()) return EASB_ERROR_UNKNOWN;
    if (nNeed == EASB_NEED_FILE && !fn_8012C83C()) return EASB_ERROR_NOFILE;
    if (nNeed == EASB_NEED_NO_FILE && fn_8012C83C() == 1) return EASB_ERROR_FILE_OPEN;
    return EASB_ERROR_NONE;
}

// Frees the product buffer.
EASBErrorE fn_8012CF00(void) {
    if (lbl_802825B8->pProductBuffer == NULL) return EASB_ERROR_PRODUCT_NOT_LOADED;
    fn_80122128(lbl_802825B8->pAllocator, lbl_802825B8->pProductBuffer, EASB_PRODUCT_BUFFER_SIZE, 4);
    lbl_802825B8->pProductBuffer = NULL;
    lbl_802825B8->b11D0 = 27;
    return EASB_ERROR_NONE;
}

// Copies this game's record and picture into the buffers that get saved.
EASBErrorE fn_8012D0D4(void) {
    lbl_802825B8->product.b1167 = 0;
    if (lbl_802825B8->pProductBuffer != NULL) {
        memset(lbl_802825B8->pProductBuffer, 0, EASB_PRODUCT_BUFFER_SIZE);
        memcpy(lbl_802825B8->pProductBuffer, &lbl_802825B8->product, sizeof(EASBProduct));
        lbl_802825B8->b11D0 = 0;
    }
    if (lbl_802825B8->pImageBuffer != NULL) {
        // EA bug: clears 0 bytes; the copies below fill the buffer anyway
        memset(lbl_802825B8->pImageBuffer, 0, 0);
        memcpy(lbl_802825B8->pImageBuffer, lbl_802825B8->pImage->aColorTable,
               sizeof(lbl_802825B8->pImage->aColorTable));
        memcpy(lbl_802825B8->pImageBuffer + sizeof(lbl_802825B8->pImage->aColorTable),
               lbl_802825B8->pImage->aData, sizeof(lbl_802825B8->pImage->aData));
        lbl_802825B8->pImageBuffer[0x4400] = 1;
    }
    return EASB_ERROR_NONE;
}

// Adds the time since the last call to the play-time counters (the Bio's and this game's) and
// stamps the product record.
EASBErrorE fn_8012D1A0(void) {
    EASBErrorE eError;
    u32 uNow;
    u32 uStamp;
    u32 uElapsed;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        uNow = fn_80122150();
        uStamp = fn_80128BC4(uNow);
        if (lbl_802825B8->uLastTime <= uNow) {
            uElapsed = uNow - lbl_802825B8->uLastTime;
            if (lbl_802825B8->b11E0) {
                lbl_802825B8->u54 = fn_80128468(lbl_802825B8->u54, uElapsed);
                lbl_802825B8->product.u50 = fn_80128468(lbl_802825B8->product.u50, uElapsed);
            } else {
                lbl_802825B8->u58 = fn_80128468(lbl_802825B8->u58, uElapsed);
                lbl_802825B8->product.u54 = fn_80128468(lbl_802825B8->product.u54, uElapsed);
            }
            lbl_802825B8->uLastTime = uNow;
        }
        lbl_802825B8->product.uTime = uStamp;
        return eError;
    }
}

EASBErrorE fn_8012D560(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_NO_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012D1A0();
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C5F8(&lbl_802825B8->u54, &lbl_802825B8->product, lbl_802825B8->unk4);
        }
        return eError;
    }
}

EASBErrorE fn_8012D5B0(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        return fn_8012C69C();
    }
}

EASBErrorE fn_8012D694(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_NO_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012C73C();
        return eError;
    }
}

EASBErrorE fn_8012D6C8(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012CD8C();
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C774(lbl_802825B8->pProductBuffer);
        }
        return eError;
    }
}

EASBErrorE fn_8012D710(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012CF00();
        return eError;
    }
}

EASBErrorE fn_8012D744(u32* pOut) {
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_8012C888(pOut);
}

EASBErrorE fn_8012D794(s32 arg0) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012D1A0();
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C7BC(&lbl_802825B8->u54, &lbl_802825B8->product, arg0);
        }
        return eError;
    }
}

// Images are not supported on this platform.
EASBErrorE fn_8012D7F0(void) {
    return EASB_ERROR_IMAGE_NOT_SUPPORTED;
}

// Adds uCount to this game's counter at 0x58 and the Bio's at 0x5C.
EASBErrorE fn_8012D8C4(u32 uCount) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        lbl_802825B8->product.u58 = fn_80128468(lbl_802825B8->product.u58, uCount);
        lbl_802825B8->u5C = fn_80128468(lbl_802825B8->u5C, uCount);
        return eError;
    }
}

// Adds uCount to this game's counter at 0x5C and the Bio's at 0x60.
EASBErrorE fn_8012D93C(u32 uCount) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        lbl_802825B8->product.u5C = fn_80128468(lbl_802825B8->product.u5C, uCount);
        lbl_802825B8->u60 = fn_80128468(lbl_802825B8->u60, uCount);
        return eError;
    }
}

// Raises this game's level; it may never go down or past EASB_MAX_LEVEL (+1).
EASBErrorE fn_8012DAB8(u16 uLevel) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        if (uLevel <= lbl_802825B8->product.uLevel) {
            eError = EASB_ERROR_INVALID_LEVEL;
        } else if (uLevel > EASB_MAX_LEVEL && uLevel != EASB_MAX_LEVEL + 1) {
            eError = EASB_ERROR_INVALID_LEVEL;
        } else {
            lbl_802825B8->product.uLevel = uLevel;
        }
        return eError;
    }
}

EASBErrorE fn_8012DD24(u16* szName, s32 arg1, s32 nLanguage) {
    return fn_8012DB30(szName, arg1, nLanguage, fn_80122150());
}

// Switches which play-time counter runs, adding up the time so far first.
EASBErrorE fn_8012DD7C(u8 bFlag) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        if (bFlag != lbl_802825B8->b11E0) {
            eError = fn_8012D1A0();
            lbl_802825B8->b11E0 = bFlag;
        }
        return eError;
    }
}

EASBErrorE fn_8012DDE0(u32* pOut) {
    EASBErrorE eResult;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eResult = fn_8012D1A0();
    *pOut = lbl_802825B8->u54;
    return eResult;
}

EASBErrorE fn_8012DE38(u32* pOut) {
    EASBErrorE eResult;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eResult = fn_8012D1A0();
    *pOut = lbl_802825B8->u58;
    return eResult;
}

// Images are not supported on this platform.
EASBErrorE fn_8012E818(u8 n, void* pImage) {
    return EASB_ERROR_IMAGE_NOT_SUPPORTED;
}

EASBErrorE fn_8012E820(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    EASBErrorE eError;

    if (arg1 == 0 || arg2 == 0 || arg3 == 0 || arg4 == 0) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_8012881C(arg0, arg1, arg2, arg3, arg4);
}

EASBErrorE fn_8012E8A8(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6) {
    EASBErrorE eError;

    if (arg1 == 0 || arg2 == 0 || arg3 == 0 || arg4 == 0 || arg5 == 0 || arg6 == 0) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_801288DC(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
}
