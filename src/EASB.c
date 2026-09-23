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
    TibExtMemFree(lbl_802825B8->uHeapID, lbl_802825B8->pProductBuffer, EASB_PRODUCT_BUFFER_SIZE, 4);
    lbl_802825B8->pProductBuffer = NULL;
    lbl_802825B8->b11D0 = EASB_PRODUCT_NONE;
    return EASB_ERROR_NONE;
}

// Puts this game's record into its slot in the loaded Bio, sorts the records, then finds where
// this game's record ended up.
EASBErrorE fn_8012CF64(void) {
    EASBErrorE eError;
    u8 nSlot;
    u32 i;

    eError = fn_8012C854(&nSlot);
    if (eError == EASB_ERROR_NONE) {
        if (nSlot == EASB_PRODUCT_NONE) return eError;
        memcpy(&lbl_802825B8->pProductBuffer[nSlot], &lbl_802825B8->product, sizeof(EASBProduct));
        fn_80128624(lbl_802825B8->pProductBuffer, EASB_MAX_PRODUCTS);
        lbl_802825B8->b11D0 = EASB_PRODUCT_NONE;
        for (i = 0; i < EASB_MAX_PRODUCTS; i++) {
            if (fn_80128CA0(lbl_802825B8->pProductBuffer[i].szName, lbl_802825B8->product.szName, 1) == 0) {
                lbl_802825B8->b11D0 = i;
                break;
            }
        }
        return EASB_ERROR_NONE;
    }
    return eError;
}

// Copies this game's picture into the picture slot its record names.
EASBErrorE fn_8012D030(void) {
    u32 nImage;

    nImage = EASB_PRODUCT_NONE;
    if (lbl_802825B8->b11D0 != EASB_PRODUCT_NONE) {
        nImage = lbl_802825B8->pProductBuffer[lbl_802825B8->b11D0].b1167;
    }
    if (nImage != EASB_PRODUCT_NONE) {
        memcpy(lbl_802825B8->pImageBuffer[nImage].aColorTable, lbl_802825B8->pImage->aColorTable,
               sizeof(lbl_802825B8->pImage->aColorTable));
        memcpy(lbl_802825B8->pImageBuffer[nImage].aData, lbl_802825B8->pImage->aData,
               sizeof(lbl_802825B8->pImage->aData));
        lbl_802825B8->pImageBuffer[nImage].bLoaded = 1;
    }
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
        memcpy(lbl_802825B8->pImageBuffer->aColorTable, lbl_802825B8->pImage->aColorTable,
               sizeof(lbl_802825B8->pImage->aColorTable));
        memcpy(lbl_802825B8->pImageBuffer->aData, lbl_802825B8->pImage->aData,
               sizeof(lbl_802825B8->pImage->aData));
        lbl_802825B8->pImageBuffer->bLoaded = 1;
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
        uNow = TibExtCurrentTimeGet();
        uStamp = fn_80128BC4(uNow);
        if (lbl_802825B8->uLastTime <= uNow) {
            uElapsed = uNow - lbl_802825B8->uLastTime;
            if (lbl_802825B8->b11E0) {
                lbl_802825B8->totals.u0 = fn_80128468(lbl_802825B8->totals.u0, uElapsed);
                lbl_802825B8->product.u50 = fn_80128468(lbl_802825B8->product.u50, uElapsed);
            } else {
                lbl_802825B8->totals.u4 = fn_80128468(lbl_802825B8->totals.u4, uElapsed);
                lbl_802825B8->product.u54 = fn_80128468(lbl_802825B8->product.u54, uElapsed);
            }
            lbl_802825B8->uLastTime = uNow;
        }
        lbl_802825B8->product.uTime = uStamp;
        return eError;
    }
}

// Finds a product record: one of the games in the loaded Bio (0 to EASB_MAX_PRODUCTS - 1) or
// EASB_PRODUCT_OURS. This game's slot in the Bio gives its live record.
EASBErrorE fn_8012D290(u8 nProduct, EASBProduct** ppProduct) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    *ppProduct = NULL;
    if (nProduct == EASB_PRODUCT_NONE) {
        eError = EASB_ERROR_PRODUCT_NOT_LOADED;
    } else if (nProduct == EASB_PRODUCT_OURS) {
        *ppProduct = &lbl_802825B8->product;
    } else if (lbl_802825B8->pProductBuffer == NULL) {
        eError = EASB_ERROR_PRODUCT_NOT_LOADED;
    } else if (nProduct >= EASB_MAX_PRODUCTS) {
        eError = EASB_ERROR_PRODUCT_NOT_LOADED;
    } else if (lbl_802825B8->b11D0 == EASB_PRODUCT_NONE) {
        eError = EASB_ERROR_PRODUCT_NOT_LOADED;
    } else if (fn_80127F88(&lbl_802825B8->pProductBuffer[nProduct]) != EASB_ERROR_NONE) {
        eError = EASB_ERROR_INVALID_PRODUCT;
    } else if (lbl_802825B8->b11D0 == nProduct) {
        *ppProduct = &lbl_802825B8->product;
    } else {
        *ppProduct = &lbl_802825B8->pProductBuffer[nProduct];
    }
    return eError;
}

EASBErrorE fn_8012D560(void) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_NO_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012D1A0();
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C5F8(&lbl_802825B8->totals,&lbl_802825B8->product, (char*)lbl_802825B8->unk4);
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

EASBErrorE fn_8012D794(void* pImage) {
    EASBErrorE eError;

    eError = fn_8012CCD8(EASB_NEED_FILE);
    if (eError != EASB_ERROR_NONE) {
        return eError;
    } else {
        eError = fn_8012D1A0();
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C7BC(&lbl_802825B8->totals,&lbl_802825B8->product, pImage);
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
        lbl_802825B8->totals.u8 = fn_80128468(lbl_802825B8->totals.u8, uCount);
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
        lbl_802825B8->totals.uC = fn_80128468(lbl_802825B8->totals.uC, uCount);
        return eError;
    }
}

// The level the Bio's totals are worth for this game.
EASBErrorE fn_8012D9B4(u16* puLevel) {
    EASBProduct* pProduct;
    f32 fProgress;
    u16 uLevel;
    EASBErrorE eError;

    if (puLevel == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(EASB_PRODUCT_OURS, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        eError = fn_8012DF4C(&uLevel, &fProgress);
    }
    if (eError == EASB_ERROR_NONE) {
        eError = fn_801291A8(uLevel, pProduct->u1160, puLevel);
    }
    return eError;
}

// This game's own level (the one fn_8012DAB8 raises).
EASBErrorE fn_8012DA38(u16* puNextLevel) {
    EASBProduct* pProduct;
    EASBErrorE eError;
    u16 uLevel;

    if (puNextLevel == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(EASB_PRODUCT_OURS, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        uLevel = pProduct->uLevel;
        if (uLevel > EASB_MAX_LEVEL && uLevel != EASB_MAX_LEVEL + 1) return EASB_ERROR_INVALID_LEVEL;
        *puNextLevel = uLevel;
    }
    return eError;
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
    return fn_8012DB30(szName, arg1, nLanguage, TibExtCurrentTimeGet());
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
    *pOut = lbl_802825B8->totals.u0;
    return eResult;
}

EASBErrorE fn_8012DE38(u32* pOut) {
    EASBErrorE eResult;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eResult = fn_8012D1A0();
    *pOut = lbl_802825B8->totals.u4;
    return eResult;
}

// Counts the valid game records in the loaded Bio.
EASBErrorE fn_8012DE90(u8* pnProducts) {
    EASBErrorE eError;
    u32 i;

    if (pnProducts == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    if (lbl_802825B8->pProductBuffer == NULL) return EASB_ERROR_PRODUCT_NOT_LOADED;
    *pnProducts = 0;
    for (i = 0; i < EASB_MAX_PRODUCTS; i++) {
        if (fn_80127F88(&lbl_802825B8->pProductBuffer[i]) == EASB_ERROR_NONE) {
            (*pnProducts)++;
        }
    }
    return eError;
}

// The level and the progress towards the next that the Bio's totals are worth.
EASBErrorE fn_8012DF4C(u16* puLevel, f32* pfProgress) {
    EASBErrorE eError;

    if (puLevel == NULL || pfProgress == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    *puLevel = 0;
    *pfProgress = 0.0f;
    eError = fn_8012D1A0();
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_80128FD4(&lbl_802825B8->totals,puLevel, pfProgress);
}

// Copies a game's name.
EASBErrorE fn_8012DFDC(u8 nProduct, char* szName, u32 uSize) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (szName == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (uSize < sizeof(pProduct->szName)) return EASB_ERROR_STRING_TOO_SMALL;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        if (pProduct == NULL) {
            eError = EASB_ERROR_PRODUCT_NOT_LOADED;
        } else {
            fn_80128BF8(szName, pProduct->szName, uSize);
        }
    }
    return eError;
}

// Copies what a game counts its games in, if its language is one of aLanguages.
EASBErrorE fn_8012E084(u8 nProduct, u16* szGamesPlayedType, u32 uLength, u16* aLanguages, u8 nLanguages,
                       u16* puLanguage) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (szGamesPlayedType == NULL || aLanguages == NULL || puLanguage == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (uLength < sizeof(pProduct->szGamesPlayedType) / sizeof(u16)) return EASB_ERROR_STRING_TOO_SMALL;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        if (pProduct->szGamesPlayedType == NULL) {
            eError = EASB_ERROR_PRODUCT_NOT_LOADED;
        } else if (!fn_801281B4(pProduct->uGamesPlayedTypeLanguage, aLanguages, nLanguages)) {
            eError = EASB_ERROR_INVALID_LANGUAGE;
        } else {
            fn_80128C4C(szGamesPlayedType, pProduct->szGamesPlayedType, uLength);
            *puLanguage = pProduct->uGamesPlayedTypeLanguage;
        }
    }
    return eError;
}

// A game's last update time.
EASBErrorE fn_8012E16C(u8 nProduct, u32* puTime) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (puTime == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        *puTime = pProduct->uTime;
    }
    return eError;
}

// A game's level.
EASBErrorE fn_8012E1E0(u8 nProduct, u16* puLevel) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (puLevel == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        eError = fn_80129218(pProduct->uLevel, pProduct->u1160, puLevel);
    }
    return eError;
}

// A game's play time while b11E0 is set, brought up to date first.
EASBErrorE fn_8012E25C(u8 nProduct, u32* pOut) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        eError = fn_8012D1A0();
        *pOut = pProduct->u50;
    }
    return eError;
}

// A game's play time while b11E0 is clear, brought up to date first.
EASBErrorE fn_8012E2D4(u8 nProduct, u32* pOut) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        eError = fn_8012D1A0();
        *pOut = pProduct->u54;
    }
    return eError;
}

// A game's counter at 0x58 (fn_8012D8C4).
EASBErrorE fn_8012E34C(u8 nProduct, u32* pOut) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        *pOut = pProduct->u58;
    }
    return eError;
}

// A game's counter at 0x5C (fn_8012D93C).
EASBErrorE fn_8012E3C0(u8 nProduct, u32* pOut) {
    EASBProduct* pProduct;
    EASBErrorE eError;

    if (pOut == NULL) return EASB_ERROR_NULL_PARAMETERS;
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012D290(nProduct, &pProduct);
    if (eError == EASB_ERROR_NONE) {
        *pOut = pProduct->u5C;
    }
    return eError;
}

// Images are not supported on this platform.
EASBErrorE fn_8012E818(u8 n, void* pImage) {
    return EASB_ERROR_IMAGE_NOT_SUPPORTED;
}

// Splits a number of seconds into days, hours, minutes and seconds.
EASBErrorE fn_8012E820(u32 uTime, u16* pnDays, u8* pnHours, u8* pnMinutes, u8* pnSeconds) {
    EASBErrorE eError;

    if (pnDays == NULL || pnHours == NULL || pnMinutes == NULL || pnSeconds == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_8012881C(uTime, pnDays, pnHours, pnMinutes, pnSeconds);
}

// Converts a time in seconds since 1970 to a calendar date and time of day.
EASBErrorE fn_8012E8A8(u32 uTime, u16* pnYear, u8* pnMonth, u8* pnDay, u8* pnHours, u8* pnMinutes,
                       u8* pnSeconds) {
    EASBErrorE eError;

    if (pnYear == NULL || pnMonth == NULL || pnDay == NULL || pnHours == NULL || pnMinutes == NULL ||
        pnSeconds == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    eError = fn_8012CCD8(EASB_NEED_ANY);
    if (eError != EASB_ERROR_NONE) return eError;
    return fn_801288DC(uTime, pnYear, pnMonth, pnDay, pnHours, pnMinutes, pnSeconds);
}
