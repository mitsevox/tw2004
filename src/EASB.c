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

EASBErrorE fn_8012D744(s32 arg0) {
    EASBErrorE eError;

    if (arg0 == 0) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_8012C888(arg0);
}

s32 fn_8012D7F0(void) {
    return 32;
}

void fn_8012DD24(s32 arg0, s32 arg1, s32 arg2) {
    fn_8012DB30(arg0, arg1, arg2, fn_80122150());
}

EASBErrorE fn_8012DDE0(s32* pOut) {
    EASBErrorE eResult;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eResult = fn_8012D1A0();
    *pOut = lbl_802825B8->n54;
    return eResult;
}

EASBErrorE fn_8012DE38(s32* pOut) {
    EASBErrorE eResult;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eResult = fn_8012D1A0();
    *pOut = lbl_802825B8->n58;
    return eResult;
}

s32 fn_8012E818(void) {
    return 32;
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
