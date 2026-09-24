// EASBStorage.c (our name): the storage half of the EA Sports Bio library (the player profile
// shared across EA games), under EASB.c's public calls: checking, sorting and packing the product
// records, the level formula, and the memory-card steps through the tag-file library (state at
// lbl_802825B0). It spans SkinBurn.c's end to EASB.c; EA may have split it into several files.

#include "core/easb.h"
#include "platform.h"

// The two-letter language codes the library knows (EASBInitParams.uGamesPlayedTypeLanguage).
#define EASB_LANGUAGE(a, b) (((a) << 8) | (b))

// The upper-case form of a-z; other characters stay as they are.
#define EASB_TO_UPPER(c) (((c) >= 'a' && (c) <= 'z') ? (c) - ('a' - 'A') : (c))

EASBErrorE fn_8012830C(char* sz, u32 uSize, u32* puLength);
s32 fn_80128DD0(u16* szA, u16* szB);

// The order of product records (fn_80128624): valid records first, then the most recently
// updated, then by name, ignoring case. Negative when pA goes first.
s32 fn_80127D84(EASBProduct* pA, EASBProduct* pB) {
    s32 nOrder;
    u8 bAFirst;

    bAFirst = 1;
    if (fn_80127F88(pA) != EASB_ERROR_NONE) {
        bAFirst = 0;
    } else if (fn_80127F88(pB) != EASB_ERROR_NONE) {
        bAFirst = 1;
    } else if (pA->uTime < pB->uTime) {
        bAFirst = 0;
    } else if (pB->uTime < pA->uTime) {
        bAFirst = 1;
    } else if (fn_80128CA0(pA->szName, pB->szName, 0) > 0) {
        bAFirst = 0;
    }
    nOrder = 1;
    if (bAFirst) {
        nOrder = -1;
    }
    return nOrder;
}

// The order of accomplishments (fn_8012872C): valid ones first, then (unless nSort is set) the
// higher u86, then the more recent, then by name. Negative when pA goes first.
s32 fn_80127E44(EASBAccomplishment* pA, EASBAccomplishment* pB, s32 nSort) {
    s32 nOrder;
    u8 bAFirst;

    if (fn_80128054(pA) != EASB_ERROR_NONE) {
        bAFirst = 0;
    } else if (fn_80128054(pB) != EASB_ERROR_NONE) {
        bAFirst = 1;
    } else if (pA->u86 > pB->u86 && nSort == 0) {
        bAFirst = 1;
    } else if (pB->u86 > pA->u86 && nSort == 0) {
        bAFirst = 0;
    } else if (pA->uTime > pB->uTime) {
        bAFirst = 1;
    } else if (pB->uTime > pA->uTime) {
        bAFirst = 0;
    } else if (fn_80128DD0(pA->szName, pB->szName) > 0) {
        bAFirst = 1;
    } else {
        bAFirst = 0;
    }
    nOrder = 1;
    if (bAFirst) {
        nOrder = -1;
    }
    return nOrder;
}

// Whether uYear is a leap year.
u8 fn_80127F40(u32 uYear) {
    if ((uYear & 3) || (uYear % 100 == 0 && uYear % 400 != 0)) {
        return 0;
    }
    return 1;
}

// Checks a product record: in use, updated within 2003-2023, u58 at least u5C, and both names
// of a usable length.
EASBErrorE fn_80127F88(EASBProduct* pProduct) {
    u32 uLength;

    if (pProduct == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (pProduct->bValid == 0) return EASB_ERROR_INVALID_PRODUCT;
    if (pProduct->uTime < EASB_TIME_FIRST || pProduct->uTime > EASB_TIME_LAST) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    if (pProduct->u58 < pProduct->u5C) return EASB_ERROR_INVALID_PRODUCT;
    if (fn_8012835C(pProduct->szGamesPlayedType, EASB_GAMES_PLAYED_TYPE_SIZE, &uLength) != EASB_ERROR_NONE) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    if (fn_8012830C(pProduct->szName, EASB_PRODUCT_NAME_SIZE, &uLength) != EASB_ERROR_NONE) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    return EASB_ERROR_NONE;
}

// Checks an accomplishment: in use, set within 2003-2023, u86 in 1-250 and a usable name.
EASBErrorE fn_80128054(const EASBAccomplishment* pAccomplishment) {
    u32 uLength;

    if (pAccomplishment == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (pAccomplishment->bValid == 0) return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    if (pAccomplishment->uTime < EASB_TIME_FIRST || pAccomplishment->uTime > EASB_TIME_LAST) {
        return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    if (pAccomplishment->u86 < 1 || pAccomplishment->u86 > 250) {
        return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    if (fn_8012835C(pAccomplishment->szName, EASB_ACCOMPLISHMENT_NAME_SIZE, &uLength) != EASB_ERROR_NONE) {
        return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    return EASB_ERROR_NONE;
}

// Whether the library knows the language uLanguage.
u8 fn_801280F8(u16 uLanguage) {
    switch (uLanguage) {
    case EASB_LANGUAGE('b', 'z'):
    case EASB_LANGUAGE('d', 'e'):
    case EASB_LANGUAGE('e', 'n'):
    case EASB_LANGUAGE('e', 's'):
    case EASB_LANGUAGE('f', 'i'):
    case EASB_LANGUAGE('f', 'r'):
    case EASB_LANGUAGE('h', 'e'):
    case EASB_LANGUAGE('i', 't'):
    case EASB_LANGUAGE('j', 'a'):
    case EASB_LANGUAGE('k', 'o'):
    case EASB_LANGUAGE('n', 'l'):
    case EASB_LANGUAGE('p', 'l'):
    case EASB_LANGUAGE('p', 't'):
    case EASB_LANGUAGE('s', 'v'):
        return 1;
    }
    return 0;
}

u8 fn_801281B4(u16 uLanguage, u16* aLanguages, u8 nLanguages) {
    u8 bFound;
    u8 i;

    bFound = 0;
    if (aLanguages != NULL) {
        for (i = 0; i < nLanguages; i++) {
            if (uLanguage == aLanguages[i]) {
                bFound = 1;
                break;
            }
        }
    }
    return bFound;
}

#define EASB_ICON_SIZE 0x2000       // the save's banner and icon block: one card block
#define EASB_ICON_CRC 0x8F73F54A    // the CRC32 the block must have

// Checks the save's banner and icon block against its CRC32, starting the checksum library
// for the check if it is not running.
EASBErrorE fn_80128200(void* pIcon, u32 uHeapID) {
    const ChecksumInterface* pCrc;
    ChecksumResult* pResult;
    EASBErrorE eError;
    int eCrcError;
    BOOL bRunning;

    eError = EASB_ERROR_NONE;
    eCrcError = 0;
    pCrc = (const ChecksumInterface*)CRC32_GetInterface();
    bRunning = pCrc->pfnIsInitialised();
    if (!bRunning) {
        eCrcError = pCrc->pfnInit((void*)uHeapID);
    }
    if (eCrcError == 0) {
        eCrcError = pCrc->pfnReset();
        if (eError == EASB_ERROR_NONE) {
            eCrcError = pCrc->pfnUpdate(pIcon, EASB_ICON_SIZE);
            if (eCrcError == 0) {
                eCrcError = pCrc->pfnFinalise(&pResult);
                if (eCrcError == 0 && *(u32*)pResult->pData != EASB_ICON_CRC) {
                    eError = EASB_ERROR_INVALID_ICON;
                }
            }
        }
        if (eCrcError != 0) {
            eError = EASB_ERROR_UNKNOWN;
        }
        if (!bRunning) {
            eCrcError = pCrc->pfnShutdown();
        }
    }
    if (eCrcError != 0) {
        eError = EASB_ERROR_UNKNOWN;
    }
    return eError;
}

// The length of sz in *puLength: too large when it fills uSize, too small when empty.
EASBErrorE fn_8012830C(char* sz, u32 uSize, u32* puLength) {
    *puLength = 0;
    while ((u8)sz[*puLength] != '\0' && *puLength < uSize) {
        *puLength = *puLength + 1;
    }
    if (*puLength >= uSize) return EASB_ERROR_STRING_TOO_LARGE;
    if (*puLength == 0) return EASB_ERROR_STRING_TOO_SMALL;
    return EASB_ERROR_NONE;
}

// The same for wide text, uSize in characters.
EASBErrorE fn_8012835C(const u16* sz, u32 uSize, u32* puLength) {
    *puLength = 0;
    while (sz[*puLength] != 0 && *puLength < uSize) {
        *puLength = *puLength + 1;
    }
    if (*puLength >= uSize) return EASB_ERROR_STRING_TOO_LARGE;
    if (*puLength == 0) return EASB_ERROR_STRING_TOO_SMALL;
    return EASB_ERROR_NONE;
}

// Checks the game's start-up parameters (fn_8012D394).
EASBErrorE fn_801283B0(EASBInitParams* pParams) {
    u32 uTypeLength;
    u32 uNameLength;
    EASBErrorE eError;

    if (pParams == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (pParams->szProductName == NULL || pParams->szGamesPlayedType == NULL || pParams->pCallbacks == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (!fn_801280F8(pParams->uGamesPlayedTypeLanguage)) return EASB_ERROR_INVALID_LANGUAGE;
    eError = fn_8012830C(pParams->szProductName, EASB_PRODUCT_NAME_SIZE, &uNameLength);
    if (eError != EASB_ERROR_NONE) return eError;
    eError = fn_8012835C(pParams->szGamesPlayedType, EASB_GAMES_PLAYED_TYPE_SIZE, &uTypeLength);
    if (eError != EASB_ERROR_NONE) return eError;
    return EASB_ERROR_NONE;
}

// Adds x and y, saturating at 0xFFFFFFFF.
u32 fn_80128468(u32 x, u32 y) {
    u32 uSum;

    uSum = x + y;
    if (uSum < x || uSum < y) {
        uSum = 0xFFFFFFFF;
    }
    return uSum;
}

// Starts a product record: the game's names, level 1, nothing played, updated now.
void fn_80128488(EASBProduct* pProduct, u8 bValid, char* szName, u16* szGamesPlayedType, u16 uLanguage) {
    fn_80128C4C(pProduct->szGamesPlayedType, szGamesPlayedType, EASB_GAMES_PLAYED_TYPE_SIZE - 1);
    fn_80128BF8(pProduct->szName, szName, EASB_PRODUCT_NAME_SIZE - 1);
    pProduct->uGamesPlayedTypeLanguage = uLanguage;
    pProduct->bValid = bValid;
    pProduct->u58 = 0;
    pProduct->u5C = 0;
    pProduct->u1160 = 1;
    pProduct->uLevel = 1;
    pProduct->u50 = 0;
    pProduct->u54 = 0;
    pProduct->uTime = fn_80128BC4(TibExtCurrentTimeGet());
}

// Starts the totals from one product record (all zero when it is not in use).
void fn_80128528(EASBTotals* pTotals, EASBProduct* pProduct) {
    if (pProduct->bValid) {
        pTotals->u0 = pProduct->u50;
        pTotals->u4 = pProduct->u54;
        pTotals->u8 = pProduct->u58;
        pTotals->uC = pProduct->u5C;
        pTotals->nProducts = 1;
        pProduct->u1160 = 1;
        return;
    }
    pTotals->u0 = 0;
    pTotals->u4 = 0;
    pTotals->u8 = 0;
    pTotals->uC = 0;
    pTotals->nProducts = 0;
}

// Adds one product record into the totals.
void fn_80128580(EASBTotals* pTotals, const EASBProduct* pProduct) {
    if (pProduct->bValid) {
        pTotals->u0 = fn_80128468(pTotals->u0, pProduct->u50);
        pTotals->u4 = fn_80128468(pTotals->u4, pProduct->u54);
        pTotals->u8 = fn_80128468(pTotals->u8, pProduct->u58);
        pTotals->uC = fn_80128468(pTotals->uC, pProduct->u5C);
        pTotals->nProducts = fn_80128468(pTotals->nProducts, 1);
        if (pTotals->uC > pTotals->u8) {
            pTotals->u8 = pTotals->uC;
        }
    }
}

// Sorts nCount product records (a shell sort, gaps 1, 4, 13, ...).
void fn_80128624(EASBProduct* aProducts, u32 nCount) {
    EASBProduct product;
    s32 i;
    s32 nGap;
    s32 j;

    nGap = 1;
    while (nGap <= (s32)(nCount / 9)) {
        nGap = nGap * 3 + 1;
    }
    for (; nGap > 0; nGap /= 3) {
        for (i = nGap; i < (s32)nCount; i++) {
            memcpy(&product, &aProducts[i], sizeof(EASBProduct));
            for (j = i - nGap; j >= 0; j -= nGap) {
                if (fn_80127D84(&product, &aProducts[j]) >= 0) break;
                memcpy(&aProducts[j + nGap], &aProducts[j], sizeof(EASBProduct));
            }
            memcpy(&aProducts[j + nGap], &product, sizeof(EASBProduct));
        }
    }
}

// Sorts a list of nCount accomplishments the same way (fn_80127E44's order).
void fn_8012872C(EASBAccomplishment** apList, u32 nCount, s32 nSort) {
    s32 i;
    s32 nGap;
    s32 j;
    EASBAccomplishment* pAccomplishment;

    nGap = 1;
    while (nGap <= (s32)(nCount / 9)) {
        nGap = nGap * 3 + 1;
    }
    for (; nGap > 0; nGap /= 3) {
        for (i = nGap; i < (s32)nCount; i++) {
            pAccomplishment = apList[i];
            for (j = i - nGap; j >= 0; j -= nGap) {
                if (fn_80127E44(pAccomplishment, apList[j], nSort) >= 0) break;
                apList[j + nGap] = apList[j];
            }
            apList[j + nGap] = pAccomplishment;
        }
    }
}

// Splits a number of seconds into days, hours, minutes and seconds.
EASBErrorE fn_8012881C(u32 uTime, u16* pnDays, u8* pnHours, u8* pnMinutes, u8* pnSeconds) {
    if (pnDays == NULL || pnHours == NULL || pnMinutes == NULL || pnSeconds == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    *pnSeconds = uTime % 60;
    *pnMinutes = (uTime / 60) % 60;
    *pnHours = (uTime / 3600) % 24;
    *pnDays = uTime / 86400;
    return EASB_ERROR_NONE;
}

// Converts a time in seconds since 1970 to a calendar date and time of day.
EASBErrorE fn_801288DC(u32 uTime, u16* pnYear, u8* pnMonth, u8* pnDay, u8* pnHours, u8* pnMinutes,
                       u8* pnSeconds) {
    s32 aDaysBefore[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    s32 aDaysBeforeLeap[12] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
    u16 nDays;
    u32 uDay;
    s32 nDayOfYear;
    EASBErrorE eError;

    if (pnYear == NULL || pnMonth == NULL || pnDay == NULL || pnHours == NULL || pnMinutes == NULL ||
        pnSeconds == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    eError = fn_8012881C(uTime, &nDays, pnHours, pnMinutes, pnSeconds);
    if (eError != EASB_ERROR_NONE) return eError;
    // Days since the year 0, with the ten days the Gregorian calendar dropped.
    uDay = nDays + 719165;
    if (uDay > 577737) {
        uDay += 10;
    }
    *pnYear = uDay / 365;
    nDayOfYear = uDay % 365;
    nDayOfYear -= *pnYear / 4;
    nDayOfYear += *pnYear / 100;
    nDayOfYear -= *pnYear / 400;
    nDayOfYear -= 12;
    while (nDayOfYear <= 0) {
        if (fn_80127F40(*pnYear) == 1) {
            nDayOfYear += 366;
        } else {
            nDayOfYear += 365;
        }
        (*pnYear)--;
    }
    (*pnYear)++;
    if (fn_80127F40(*pnYear) == 1) {
        for (*pnMonth = 12; *pnMonth != 0; (*pnMonth)--) {
            if (nDayOfYear > aDaysBeforeLeap[*pnMonth - 1]) {
                *pnDay = nDayOfYear - aDaysBeforeLeap[*pnMonth - 1];
                break;
            }
        }
    } else {
        for (*pnMonth = 12; *pnMonth != 0; (*pnMonth)--) {
            if (nDayOfYear > aDaysBefore[*pnMonth - 1]) {
                *pnDay = nDayOfYear - aDaysBefore[*pnMonth - 1];
                break;
            }
        }
    }
    return EASB_ERROR_NONE;
}

// Clamps a time to 2003-01-01..2023-01-01.
u32 fn_80128BC4(u32 uTime) {
    u32 uClamped;

    if (uTime < EASB_TIME_FIRST) {
        return EASB_TIME_FIRST;
    }
    uClamped = EASB_TIME_LAST;
    if (uTime <= EASB_TIME_LAST) {
        uClamped = uTime;
    }
    return uClamped;
}

// Copies szSrc into szDest, at most uSize - 1 characters, always ending the text. Returns szDest.
char* fn_80128BF8(char* szDest, char* szSrc, u32 uSize) {
    char* pDest;
    u8 c;

    if (szSrc != NULL && szDest != NULL && uSize != 0 && szSrc != szDest) {
        pDest = szDest;
        for (;;) {
            if (--uSize == 0) {
                *pDest = '\0';
                return szDest;
            }
            c = *szSrc++;
            *pDest++ = c;
            if (c == '\0') return szDest;
        }
    }
    return szDest;
}

// The same for wide text.
u16* fn_80128C4C(u16* szDest, const u16* szSrc, u32 uLength) {
    u16* pDest;
    u16 c;

    if (szSrc != NULL && szDest != NULL && uLength != 0 && szSrc != szDest) {
        pDest = szDest;
        for (;;) {
            if (--uLength == 0) {
                *pDest = 0;
                return szDest;
            }
            c = *szSrc++;
            *pDest++ = c;
            if (c == 0) return szDest;
        }
    }
    return szDest;
}

// Compares two texts like strcmp; bCase 0 ignores the case of a-z.
s32 fn_80128CA0(char* szA, char* szB, u8 bCase) {
    u8* pA;
    u8* pB;

    pA = (u8*)szA;
    pB = (u8*)szB;
    if (pA == pB || pA == NULL || pB == NULL) return 0;
    if (bCase) {
        while (*pA == *pB++) {
            if (*pA++ == '\0') return 0;
        }
        return *pA - pB[-1];
    }
    while (EASB_TO_UPPER(*pA) == EASB_TO_UPPER(*pB)) {
        if (*pA == '\0') return 0;
        pA++;
        pB++;
    }
    return EASB_TO_UPPER(*pA) - EASB_TO_UPPER(*pB);
}

// Compares two wide texts like strcmp.
s32 fn_80128DD0(u16* szA, u16* szB) {
    if (szA == szB || szA == NULL || szB == NULL) return 0;
    while (*szA == *szB++) {
        if (*szA++ == 0) return 0;
    }
    return *szA - szB[-1];
}

// Makes a character safe for the Bio: the end of text and printable Latin-1 stay as they are,
// anything else becomes 0xAC.
u8 fn_80128E28(u8 c) {
    u8 aMap[256] = {
        0x00, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
        0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0xAC,
        0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
        0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
        0xAC, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAC, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };

    return aMap[c];
}

// Maps a wide character through fn_80128E28's table, for English text only (anything past 0xFF
// maps as 0xFF).
u16 fn_80128E6C(u16 c, u16 uLanguage) {
    if (uLanguage != EASB_LANGUAGE('e', 'n')) {
        return c;
    }
    if (c > 0xFF) {
        return fn_80128E28(0xFF);
    }
    return fn_80128E28(c);
}

// Maps every character of sz through fn_80128E28's table.
void fn_80128EC0(char* sz) {
    while ((u8)*sz != '\0') {
        *sz = fn_80128E28(*sz);
        sz++;
    }
}

// The same for wide text (fn_80128E6C).
void fn_80128F04(u16* sz, u16 uLanguage) {
    u16* pc;

    pc = sz;
    while (*pc != 0) {
        *pc = fn_80128E6C(*pc, uLanguage);
        pc++;
    }
}

// uValue / uDivisor * uScaleA plus uValue * uScaleB, each saturating at 0xFFFFFFFF.
u32 fn_80128F58(u32 uValue, u32 uScaleB, u32 uDivisor, u32 uScaleA) {
    u32 uQuotient;
    u32 uB;
    u32 uA;

    uB = 0;
    uA = 0;
    if (uScaleA != 0) {
        uQuotient = uValue / uDivisor;
        if (0xFFFFFFFF / uScaleA >= uQuotient) {
            uA = uQuotient * uScaleA;
        } else {
            uA = 0xFFFFFFFF;
        }
    }
    if (uScaleB != 0) {
        if (0xFFFFFFFF / uScaleB >= uValue) {
            uB = uValue * uScaleB;
        } else {
            uB = 0xFFFFFFFF;
        }
    }
    return fn_80128468(uA, uB);
}

// The Bio's level from its totals: points for the games in it, the hours played and the two
// counters, then level n needs 20 * n + 1000 points more than level n - 1. *pfProgress is how
// far the points have got towards the next level (0 to 1).
EASBErrorE fn_80128FD4(const EASBTotals* pTotals, u16* puLevel, f32* pfProgress) {
    u32 nHours0;
    u32 nHours4;
    u32 uProducts;
    u32 uHours0;
    u32 uHours4;
    u32 u8Points;
    u32 uCPoints;
    u32 uPoints;
    u32 uLevelEnd;
    u32 uLevelStart;
    u16 nLevel;

    if (puLevel == NULL || pfProgress == NULL) return EASB_ERROR_NULL_PARAMETERS;
    *puLevel = 0;
    *pfProgress = 0.0f;
    nHours0 = pTotals->u0 / 3600;
    nHours4 = pTotals->u4 / 3600;
    uProducts = fn_80128F58(pTotals->nProducts, 600, 5, 1500);
    uHours0 = fn_80128F58(nHours0, 50, 50, 150);
    uHours4 = fn_80128F58(nHours4, 5, 1, 0);
    u8Points = fn_80128F58(pTotals->u8, 50, 1, 0);
    uCPoints = fn_80128F58(pTotals->uC, 150, 1, 0);
    uPoints = fn_80128468(uProducts, uHours0);
    uPoints = fn_80128468(uPoints, uHours4);
    uPoints = fn_80128468(uPoints, u8Points);
    uPoints = fn_80128468(uPoints, uCPoints);
    uLevelStart = 0;
    uLevelEnd = 0;
    for (nLevel = 1; nLevel <= EASB_MAX_LEVEL; nLevel++) {
        if (uPoints < uLevelEnd) break;
        uLevelStart = uLevelEnd;
        uLevelEnd = (nLevel + 1) * 20 + uLevelEnd + 1000;
    }
    *puLevel = nLevel - 1;
    *pfProgress = (f32)(uPoints - uLevelStart) / (f32)(uLevelEnd - uLevelStart);
    if (*pfProgress >= 1.0f) {
        *pfProgress = 1.0f;
    }
    if (*pfProgress < 0.0f) {
        *pfProgress = 0.0f;
    }
    return EASB_ERROR_NONE;
}

// The level after a drop: uLevel less (u1160 - 5), kept within 1..EASB_MAX_LEVEL; the top level
// never drops.
EASBErrorE fn_801291A8(u16 uLevel, u16 u1160, u16* puLevel) {
    s32 nDrop;
    s32 nLevel;

    if (puLevel == NULL) return EASB_ERROR_NULL_PARAMETERS;
    nDrop = u1160 - 5;
    if (nDrop < 0) {
        nDrop = 0;
    }
    nLevel = uLevel - nDrop;
    if (nLevel < 1) {
        *puLevel = 1;
    } else if (nLevel > EASB_MAX_LEVEL) {
        *puLevel = EASB_MAX_LEVEL;
    } else {
        *puLevel = nLevel;
    }
    if (uLevel == EASB_MAX_LEVEL) {
        *puLevel = EASB_MAX_LEVEL;
    }
    return EASB_ERROR_NONE;
}

// The level after a rise: uLevel plus (u1160 - 5), kept within 1..EASB_MAX_LEVEL; EASB_MAX_LEVEL + 1
// stays as it is.
EASBErrorE fn_80129218(u16 uLevel, u16 u1160, u16* puLevel) {
    s32 nRise;
    s32 nLevel;

    if (puLevel == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (uLevel == EASB_MAX_LEVEL + 1) {
        *puLevel = EASB_MAX_LEVEL + 1;
        return EASB_ERROR_NONE;
    }
    nRise = u1160 - 5;
    if (nRise < 0) {
        nRise = 0;
    }
    nLevel = uLevel + nRise;
    if (nLevel < 1) {
        *puLevel = 1;
    } else if (nLevel > EASB_MAX_LEVEL) {
        *puLevel = EASB_MAX_LEVEL;
    } else {
        *puLevel = nLevel;
    }
    return EASB_ERROR_NONE;
}

// The save file's byte packing. Each call works at pBuffer + *pnOffset and moves *pnOffset past
// what it wrote or read.

// Writes uValue, kept within uMin..uMax, as nBytes bytes, lowest first.
void fn_80129290(u8* pBuffer, s32* pnOffset, u8 nBytes, u32 uValue, u32 uMin, u32 uMax) {
    u8 i;

    if (uValue < uMin) {
        uValue = uMin;
    } else if (uValue > uMax) {
        uValue = uMax;
    }
    for (i = 0; i < nBytes; i++) {
        pBuffer[*pnOffset + i] = uValue;
        uValue >>= 8;
    }
    *pnOffset += nBytes;
}

// Reads a value of nBytes bytes, lowest first, and keeps it within uMin..uMax.
u32 fn_801293F8(u8* pBuffer, s32* pnOffset, u8 nBytes, u32 uMin, u32 uMax) {
    u32 uValue;
    u8 i;

    uValue = 0;
    for (i = 0; i < nBytes; i++) {
        uValue |= pBuffer[*pnOffset + i] << (i * 8);
    }
    *pnOffset += nBytes;
    if (uValue < uMin) {
        uValue = uMin;
    } else if (uValue > uMax) {
        uValue = uMax;
    }
    return uValue;
}

// Writes uLength characters of sz, each through fn_80128E28's table.
void fn_8012956C(u8* pBuffer, s32* pnOffset, u32 uLength, char* sz) {
    char* pc;
    u32 i;
    u8 c;

    pc = sz;
    for (i = 0; i < uLength; i++) {
        c = fn_80128E28(*pc);
        pc++;
        pBuffer[*pnOffset + i] = c;
    }
    *pnOffset += uLength;
}

// Reads uLength characters into sz, each through fn_80128E28's table.
void fn_801295D8(u8* pBuffer, s32* pnOffset, u32 uLength, char* sz) {
    char* pc;
    u32 i;

    pc = sz;
    for (i = 0; i < uLength; i++, pc++) {
        *pc = fn_80128E28(pBuffer[*pnOffset + i]);
    }
    *pnOffset += uLength;
}

// Writes uSize bytes of wide text, each character through fn_80128E6C, lowest byte first.
void fn_80129644(u8* pBuffer, s32* pnOffset, u32 uSize, u16* sz, u16 uLanguage) {
    u16* pc;
    u32 i;
    u16 c;

    pc = sz;
    for (i = 0; i < uSize; i += 2) {
        c = fn_80128E6C(*pc, uLanguage);
        pc++;
        pBuffer[*pnOffset + i] = c;
        pBuffer[*pnOffset + i + 1] = c >> 8;
    }
    *pnOffset += uSize;
}

// Reads uSize bytes of wide text into sz, each character through fn_80128E6C.
void fn_801296CC(u8* pBuffer, s32* pnOffset, u32 uSize, u16* sz, u16 uLanguage) {
    u16* pc;
    u32 i;

    pc = sz;
    for (i = 0; i < uSize; i += 2) {
        *pc = fn_80128E6C((pBuffer[*pnOffset + i + 1] << 8) + pBuffer[*pnOffset + i], uLanguage);
        pc++;
    }
    *pnOffset += uSize;
}

// Packs the totals into pBuffer (uSize bytes, cleared first): the save file's 'HEAD' record.
void fn_80129754(EASBTotals* pTotals, u8* pBuffer, u32 uSize) {
    s32 nOffset;

    nOffset = 0;
    memset(pBuffer, 0, uSize);
    fn_80129290(pBuffer, &nOffset, 4, pTotals->u0, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pTotals->u4, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pTotals->u8, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pTotals->uC, 0, pTotals->u8);
    fn_80129290(pBuffer, &nOffset, 1, pTotals->nProducts, 1, 250);
}

// Unpacks the totals from a 'HEAD' record (uSize is not used).
void fn_80129828(EASBTotals* pTotals, u8* pBuffer, u32 uSize) {
    s32 nOffset;

    nOffset = 0;
    memset(pTotals, 0, sizeof(EASBTotals));
    pTotals->u0 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pTotals->u4 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pTotals->u8 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pTotals->uC = fn_801293F8(pBuffer, &nOffset, 4, 0, pTotals->u8);
    pTotals->nProducts = fn_801293F8(pBuffer, &nOffset, 1, 0, 250);
}

// Packs a product record into pBuffer (uSize bytes, cleared first): names without their ends,
// each value within its range, then every accomplishment.
void fn_801298FC(EASBProduct* pProduct, u8* pBuffer, u32 uSize) {
    s32 nOffset;
    u32 i;
    EASBAccomplishment* pAccomplishment;

    nOffset = 0;
    memset(pBuffer, 0, uSize);
    fn_8012956C(pBuffer, &nOffset, EASB_PRODUCT_NAME_SIZE - 1, pProduct->szName);
    fn_80129290(pBuffer, &nOffset, 2, pProduct->uGamesPlayedTypeLanguage, 0, 0xFFFF);
    fn_80129644(pBuffer, &nOffset, (EASB_GAMES_PLAYED_TYPE_SIZE - 1) * sizeof(u16),
                pProduct->szGamesPlayedType, pProduct->uGamesPlayedTypeLanguage);
    fn_80129290(pBuffer, &nOffset, 4, pProduct->uTime, EASB_TIME_FIRST, EASB_TIME_LAST);
    fn_80129290(pBuffer, &nOffset, 4, pProduct->u50, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pProduct->u54, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pProduct->u58, 0, 0xFFFFFFFF);
    fn_80129290(pBuffer, &nOffset, 4, pProduct->u5C, 0, pProduct->u58);
    fn_80129290(pBuffer, &nOffset, 2, pProduct->u1160, 0, EASB_MAX_LEVEL);
    fn_80129290(pBuffer, &nOffset, 2, pProduct->uLevel, 0, EASB_MAX_LEVEL + 1);
    fn_80129290(pBuffer, &nOffset, 1, pProduct->bValid, 0, 1);
    for (i = 0; i < EASB_MAX_ACCOMPLISHMENTS; i++) {
        pAccomplishment = &pProduct->aAccomplishments[i];
        fn_80129290(pBuffer, &nOffset, 2, pAccomplishment->uLanguage, 0, 0xFFFF);
        fn_80129644(pBuffer, &nOffset, (EASB_ACCOMPLISHMENT_NAME_SIZE - 1) * sizeof(u16),
                    pAccomplishment->szName, pAccomplishment->uLanguage);
        fn_80129290(pBuffer, &nOffset, 4, pAccomplishment->uTime, EASB_TIME_FIRST, EASB_TIME_LAST);
        if (pAccomplishment->bValid == 1) {
            fn_80129290(pBuffer, &nOffset, 1, pAccomplishment->u86, 1, 250);
        } else {
            fn_80129290(pBuffer, &nOffset, 1, 0, 0, 0);
        }
    }
}

// Unpacks a product record from pBuffer (fn_801298FC's layout), ending the names; an
// accomplishment is in use when its u86 is not 0. uSize is not used.
void fn_80129B30(EASBProduct* pProduct, u8* pBuffer, u32 uSize) {
    s32 nOffset;
    u32 i;
    EASBAccomplishment* pAccomplishment;

    nOffset = 0;
    memset(pProduct, 0, sizeof(EASBProduct));
    fn_801295D8(pBuffer, &nOffset, EASB_PRODUCT_NAME_SIZE - 1, pProduct->szName);
    pProduct->szName[EASB_PRODUCT_NAME_SIZE - 1] = '\0';
    pProduct->uGamesPlayedTypeLanguage = fn_801293F8(pBuffer, &nOffset, 2, 0, 0xFFFF);
    fn_801296CC(pBuffer, &nOffset, (EASB_GAMES_PLAYED_TYPE_SIZE - 1) * sizeof(u16),
                pProduct->szGamesPlayedType, pProduct->uGamesPlayedTypeLanguage);
    pProduct->szGamesPlayedType[EASB_GAMES_PLAYED_TYPE_SIZE - 1] = 0;
    pProduct->uTime = fn_801293F8(pBuffer, &nOffset, 4, EASB_TIME_FIRST, EASB_TIME_LAST);
    pProduct->u50 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pProduct->u54 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pProduct->u58 = fn_801293F8(pBuffer, &nOffset, 4, 0, 0xFFFFFFFF);
    pProduct->u5C = fn_801293F8(pBuffer, &nOffset, 4, 0, pProduct->u58);
    pProduct->u1160 = fn_801293F8(pBuffer, &nOffset, 2, 0, EASB_MAX_LEVEL);
    pProduct->uLevel = fn_801293F8(pBuffer, &nOffset, 2, 0, EASB_MAX_LEVEL + 1);
    pProduct->bValid = fn_801293F8(pBuffer, &nOffset, 1, 0, 1);
    for (i = 0; i < EASB_MAX_ACCOMPLISHMENTS; i++) {
        pAccomplishment = &pProduct->aAccomplishments[i];
        pAccomplishment->uLanguage = fn_801293F8(pBuffer, &nOffset, 2, 0, 0xFFFF);
        fn_801296CC(pBuffer, &nOffset, (EASB_ACCOMPLISHMENT_NAME_SIZE - 1) * sizeof(u16),
                    pAccomplishment->szName, pAccomplishment->uLanguage);
        pProduct->aAccomplishments[i].szName[EASB_ACCOMPLISHMENT_NAME_SIZE - 1] = 0;
        pAccomplishment->uTime = fn_801293F8(pBuffer, &nOffset, 4, EASB_TIME_FIRST, EASB_TIME_LAST);
        pAccomplishment->u86 = fn_801293F8(pBuffer, &nOffset, 1, 0, 250);
        if (pAccomplishment->u86 == 0) {
            pAccomplishment->bValid = 0;
        } else {
            pAccomplishment->bValid = 1;
        }
    }
}

// Packs a picture into pBuffer (uSize bytes, cleared first): bLoaded, the colour table as red,
// green, blue, then the pixels.
void fn_80129D70(EASBImage* pImage, u8 bLoaded, u8* pBuffer, u32 uSize) {
    s32 nOffset;
    u32 i;
    u32 j;

    nOffset = 0;
    memset(pBuffer, 0, uSize);
    fn_80129290(pBuffer, &nOffset, 1, bLoaded, 0, 1);
    for (i = 0; i < 256; i++) {
        fn_80129290(pBuffer, &nOffset, 1, pImage->aColorTable[i * 4 + 2], 0, 0xFF);
        fn_80129290(pBuffer, &nOffset, 1, pImage->aColorTable[i * 4 + 1], 0, 0xFF);
        fn_80129290(pBuffer, &nOffset, 1, pImage->aColorTable[i * 4 + 0], 0, 0xFF);
    }
    for (j = 0; j < sizeof(pImage->aData); j++) {
        fn_80129290(pBuffer, &nOffset, 1, pImage->aData[j], 0, 0xFF);
    }
}

// Unpacks a picture from pBuffer (fn_80129D70's layout); uSize is not used.
void fn_80129E88(EASBImageSlot* pSlot, u8* pBuffer, u32 uSize) {
    s32 nOffset;
    u32 i;
    u32 j;

    nOffset = 0;
    memset(pSlot, 0, sizeof(EASBImageSlot));
    pSlot->bLoaded = fn_801293F8(pBuffer, &nOffset, 1, 0, 1);
    for (i = 0; i < 256; i++) {
        pSlot->aColorTable[i * 4 + 2] = fn_801293F8(pBuffer, &nOffset, 1, 0, 0xFF);
        pSlot->aColorTable[i * 4 + 1] = fn_801293F8(pBuffer, &nOffset, 1, 0, 0xFF);
        pSlot->aColorTable[i * 4 + 0] = fn_801293F8(pBuffer, &nOffset, 1, 0, 0xFF);
    }
    for (j = 0; j < sizeof(pSlot->aData); j++) {
        pSlot->aData[j] = fn_801293F8(pBuffer, &nOffset, 1, 0, 0xFF);
    }
}

// Adds pAdd into pTotals, keeping u8 at least uC; the record count comes from pAdd, one more
// (up to 250) unless nMode is 3.
void fn_80129F98(EASBTotals* pTotals, const EASBTotals* pAdd, s32 nMode) {
    pTotals->u0 = fn_80128468(pTotals->u0, pAdd->u0);
    pTotals->u4 = fn_80128468(pTotals->u4, pAdd->u4);
    pTotals->u8 = fn_80128468(pTotals->u8, pAdd->u8);
    pTotals->uC = fn_80128468(pTotals->uC, pAdd->uC);
    if (pTotals->uC > pTotals->u8) {
        pTotals->u8 = pTotals->uC;
    }
    pTotals->nProducts = pAdd->nProducts;
    if (nMode != 3 && pTotals->nProducts < 250) {
        pTotals->nProducts++;
    }
}

// Adds pFrom's record into pInto when both are in use: the counters, every valid
// accomplishment (through fn_8012DB30), b1167, u1160 and the higher level.
void fn_8012A050(EASBProduct* pInto, const EASBProduct* pFrom) {
    u32 i;

    if (!pInto->bValid || !pFrom->bValid) return;
    pInto->u50 = fn_80128468(pInto->u50, pFrom->u50);
    pInto->u54 = fn_80128468(pInto->u54, pFrom->u54);
    pInto->u58 = fn_80128468(pInto->u58, pFrom->u58);
    pInto->u5C = fn_80128468(pInto->u5C, pFrom->u5C);
    if (pInto->u5C > pInto->u58) {
        pInto->u58 = pInto->u5C;
    }
    for (i = 0; i < EASB_MAX_ACCOMPLISHMENTS; i++) {
        if (fn_80128054(&pFrom->aAccomplishments[i]) == EASB_ERROR_NONE) {
            fn_8012DB30(pFrom->aAccomplishments[i].szName, pFrom->aAccomplishments[i].u86,
                        pFrom->aAccomplishments[i].uLanguage, pFrom->aAccomplishments[i].uTime);
        }
    }
    pInto->b1167 = pFrom->b1167;
    pInto->u1160 = pFrom->u1160;
    if (pInto->uLevel < pFrom->uLevel) {
        pInto->uLevel = pFrom->uLevel;
    }
}

// ---- The memory-card steps, through the tag-file library ----
// The Bio file holds a HEAD record (the totals) and a PROD record per product; no operation
// writes an IMAG (picture) record. Each operation (EASBStorageOp) runs as a list of steps; a
// step is called with the process state until it reports EASB_PROCESS_COMPLETE.

#define EASB_TAG_HEAD 0x48454144    // 'HEAD'
#define EASB_TAG_PROD 0x50524F44    // 'PROD'
#define EASB_TAG_IMAG 0x494D4147    // 'IMAG'

// The packed size of each record (fn_80129754, fn_801298FC, fn_80129D70).
#define EASB_HEAD_SIZE 0x11
#define EASB_PROD_SIZE 0x1104
#define EASB_IMAG_SIZE 0x4301

EASBErrorE fn_8012A4C4(EASBProcessE* peProcess);
EASBErrorE fn_8012C98C(int eTagError);
EASBErrorE fn_8012CAA8(SFIOFuncTable* pCallbacks, int* pDevices);
EASBErrorE fn_8012CB98(EASBProcessE* peProcess);
EASBErrorE fn_8012CC48(void);

// Checks that the file holds nCount records tagged uTag, each uSize bytes.
EASBErrorE fn_8012A164(TagSession* pSession, u32 uTag, u32 nCount, u32 uSize) {
    u32 nFound;
    u32 uFound;
    u32 i;

    if (fn_8012C98C(TagFile_Count(pSession, uTag, &nFound)) != EASB_ERROR_NONE || nFound != nCount) {
        return EASB_ERROR_FILE_CORRUPT;
    }
    for (i = 0; i < nCount; i++) {
        if (fn_8012C98C(TagFile_GetSize(pSession, uTag, i, &uFound)) != EASB_ERROR_NONE || uFound != uSize) {
            return EASB_ERROR_FILE_CORRUPT;
        }
    }
    return EASB_ERROR_NONE;
}

// Whether the HEAD record or any PROD record is in state nState.
u8 fn_8012A20C(s32 nState) {
    s32 i;

    if (lbl_802825B0->nHeadState == nState) {
        return 1;
    }
    for (i = 0; i < EASB_MAX_PRODUCTS; i++) {
        if (lbl_802825B0->anProductState[i] == nState) {
            return 1;
        }
    }
    return 0;
}

// Step: opens the Bio file and reads its record map; when that completes, keeps its session and
// marks the file open.
EASBErrorE fn_8012A2A8(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8012C98C(TagFile_Open("EASB", SFIO_DEVICE_INVALID, 0));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
        if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
            eError = fn_8012C98C(TagFile_GetSession(&lbl_802825B0->session));
            lbl_802825B0->bFileOpen = 1;
            lbl_802825B0->b92 = 1;
            lbl_802825B0->b90 = 1;
        }
    }
    return eError;
}

// Step: reopens the Bio file of the kept session; when that completes, keeps it and marks it open.
EASBErrorE fn_8012A364(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->b92 == 0) {
        return EASB_ERROR_CANNOT_REOPEN;
    }
    if (lbl_802825B0->b90 == 0) {
        return EASB_ERROR_CANNOT_REOPEN;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8012C98C(TagFile_Reopen(&lbl_802825B0->session));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
        if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
            eError = fn_8012C98C(TagFile_GetSession(&lbl_802825B0->session));
            lbl_802825B0->bFileOpen = 1;
        }
    }
    return eError;
}

// Step: closes the Bio file.
EASBErrorE fn_8012A434(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8012C98C(TagFile_End(&lbl_802825B0->session));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE) {
        lbl_802825B0->bFileOpen = 0;
    }
    return eError;
}

// Step: reads the HEAD record and every PROD record to pick the game's slot: its own record
// (same name) if there is one, else the first empty record, else the oldest record.
EASBErrorE fn_8012A4C4(EASBProcessE* peProcess) {
    EASBProduct product;
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        memset(&lbl_802825B0->nHeadState, 0, sizeof(lbl_802825B0->nHeadState));
        memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
        lbl_802825B0->n94 = 0;
        lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        lbl_802825B0->nFoundKind = 0;
        lbl_802825B0->nFoundSlot = EASB_PRODUCT_NONE;
        lbl_802825B0->uOldestTime = EASB_TIME_LAST;
        lbl_802825B0->nRecord = EASB_PRODUCT_NONE;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_HEAD, 0, lbl_802825B0->pBuffer,
                                          EASB_HEAD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && lbl_802825B0->nRecord == EASB_PRODUCT_NONE) {
        if (eError == EASB_ERROR_NONE) {
            lbl_802825B0->nHeadState = 1;
        } else if (eError == EASB_ERROR_SECTION_CORRUPT) {
            lbl_802825B0->nHeadState = 2;
        } else {
            return eError;
        }
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                          lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_COMPLETE && lbl_802825B0->nRecord != EASB_PRODUCT_NONE) {
        if (eError == EASB_ERROR_NONE) {
            fn_80129B30(&product, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 1;
        } else if (eError == EASB_ERROR_SECTION_CORRUPT) {
            eError = EASB_ERROR_NONE;
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 2;
        } else {
            return eError;
        }
        if (lbl_802825B0->anProductState[lbl_802825B0->nRecord] == 1 && lbl_802825B0->nFoundKind != 3) {
            if (product.bValid == 1 && fn_80128CA0(product.szName, lbl_802825B0->args.szName, 1) == 0) {
                lbl_802825B0->nFoundSlot = lbl_802825B0->nRecord;
                lbl_802825B0->nFoundKind = 3;
            } else if (product.bValid == 0
                       && (lbl_802825B0->nFoundKind == 1 || lbl_802825B0->nFoundKind == 0)) {
                lbl_802825B0->nFoundSlot = lbl_802825B0->nRecord;
                lbl_802825B0->nFoundKind = 2;
            } else if (product.bValid == 1 && product.uTime <= lbl_802825B0->uOldestTime
                       && lbl_802825B0->nFoundKind != 2) {
                lbl_802825B0->uOldestTime = product.uTime;
                lbl_802825B0->nFoundSlot = lbl_802825B0->nRecord;
                lbl_802825B0->nFoundKind = 1;
            }
        }
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                              lbl_802825B0->pBuffer, EASB_PROD_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        } else if (lbl_802825B0->nRecord == EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nSlot = lbl_802825B0->nFoundSlot;
            lbl_802825B0->n94 = lbl_802825B0->nFoundKind;
        }
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE && fn_8012A20C(2)) {
        eError = EASB_ERROR_SECTION_CORRUPT;
    }
    return eError;
}

// Step: checks the file holds one HEAD record, 25 PROD records and no IMAG record, of the
// packed sizes.
EASBErrorE fn_8012A848(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012A164(&lbl_802825B0->session, EASB_TAG_HEAD, 1, EASB_HEAD_SIZE);
        }
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012A164(&lbl_802825B0->session, EASB_TAG_PROD, EASB_MAX_PRODUCTS, EASB_PROD_SIZE);
        }
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012A164(&lbl_802825B0->session, EASB_TAG_IMAG, 0, EASB_IMAG_SIZE);
        }
        *peProcess = EASB_PROCESS_COMPLETE;
    }
    return eError;
}

// Step: sets the totals from the game's record alone (fn_80128528).
EASBErrorE fn_8012A900(EASBProcessE* peProcess) {
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        fn_80128528(lbl_802825B0->args.pTotals, lbl_802825B0->args.pProduct);
        *peProcess = EASB_PROCESS_COMPLETE;
    }
    return EASB_ERROR_NONE;
}

// Step: reads the HEAD record into the totals.
EASBErrorE fn_8012A95C(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (lbl_802825B0->nSlot == EASB_PRODUCT_NONE) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        lbl_802825B0->nHeadState = 0;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_HEAD, 0, lbl_802825B0->pBuffer,
                                          EASB_HEAD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
        fn_80129828(&lbl_802825B0->totals, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        lbl_802825B0->nHeadState = 1;
    } else if (eError == EASB_ERROR_SECTION_CORRUPT && *peProcess == EASB_PROCESS_COMPLETE) {
        lbl_802825B0->nHeadState = 2;
    }
    return eError;
}

// Step: reads the game's PROD record from its slot and merges it into the game's record, with
// the totals; a record with another name is refused. If the slot is not the game's own (n94 not
// 3), only the totals merge and the record takes the slot and, in u1160, the file's level.
EASBErrorE fn_8012AA7C(EASBProcessE* peProcess) {
    EASBProduct product;
    f32 fProgress;
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (lbl_802825B0->nSlot == EASB_PRODUCT_NONE) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(0)) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(2)) {
        return EASB_ERROR_SECTION_CORRUPT;
    }
    if (lbl_802825B0->n94 != 3) {
        fn_80129F98(lbl_802825B0->args.pTotals, &lbl_802825B0->totals, lbl_802825B0->n94);
        lbl_802825B0->args.pProduct->b1167 = lbl_802825B0->nSlot;
        eError = fn_80128FD4(&lbl_802825B0->totals, &lbl_802825B0->args.pProduct->u1160, &fProgress);
        *peProcess = EASB_PROCESS_COMPLETE;
        return eError;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nSlot,
                                          lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
        fn_80129B30(&product, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        if (product.bValid == 1 && fn_80128CA0(product.szName, lbl_802825B0->args.szName, 1) == 0) {
            product.b1167 = lbl_802825B0->nSlot;
            fn_80129F98(lbl_802825B0->args.pTotals, &lbl_802825B0->totals, lbl_802825B0->n94);
            fn_8012A050(lbl_802825B0->args.pProduct, &product);
        } else {
            eError = EASB_ERROR_INVALID_PRODUCT;
        }
    }
    return eError;
}

// Step: reads all 25 PROD records into the records; a corrupt one comes back cleared.
EASBErrorE fn_8012AC40(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(0)) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                          lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE) {
        if (eError == EASB_ERROR_NONE) {
            fn_80129B30(&lbl_802825B0->args.pProduct[lbl_802825B0->nRecord], lbl_802825B0->pBuffer,
                        lbl_802825B0->uBufferSize);
            lbl_802825B0->args.pProduct[lbl_802825B0->nRecord].b1167 = lbl_802825B0->nRecord;
        } else if (eError == EASB_ERROR_SECTION_CORRUPT) {
            memset(&lbl_802825B0->args.pProduct[lbl_802825B0->nRecord], 0, sizeof(EASBProduct));
            eError = EASB_ERROR_NONE;
            lbl_802825B0->args.pProduct[lbl_802825B0->nRecord].bValid = 0;
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 2;
        } else {
            return eError;
        }
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                              lbl_802825B0->pBuffer, EASB_PROD_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        }
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE && fn_8012A20C(2)) {
        eError = EASB_ERROR_SECTION_CORRUPT;
    }
    return eError;
}

// Step: reads all 25 IMAG records into the picture slots; one that reads back corrupt comes back
// cleared; a missing record, no file or no device ends the step with the error.
EASBErrorE fn_8012AE40(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(0)) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(2)) {
        return EASB_ERROR_SECTION_CORRUPT;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_IMAG, lbl_802825B0->nRecord,
                                          lbl_802825B0->pBuffer, EASB_IMAG_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE) {
        if (eError == EASB_ERROR_NONE) {
            fn_80129E88(&lbl_802825B0->args.pImageSlots[lbl_802825B0->nRecord], lbl_802825B0->pBuffer,
                        lbl_802825B0->uBufferSize);
        } else if (eError == EASB_ERROR_NOFILE || eError == EASB_ERROR_NODEVICE) {
            return eError;
        } else {
            memset(&lbl_802825B0->args.pImageSlots[lbl_802825B0->nRecord], 0, sizeof(EASBImageSlot));
            eError = EASB_ERROR_NONE;
            lbl_802825B0->args.pImageSlots[lbl_802825B0->nRecord].bLoaded = 0;
        }
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_IMAG, lbl_802825B0->nRecord,
                                              lbl_802825B0->pBuffer, EASB_IMAG_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        }
    }
    return eError;
}

// Step: writes the totals as the HEAD record.
EASBErrorE fn_8012B004(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        fn_80129754(lbl_802825B0->args.pTotals, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_HEAD, 0, lbl_802825B0->pBuffer,
                                           EASB_HEAD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE) {
        lbl_802825B0->nHeadState = 1;
    }
    return eError;
}

// Step: writes the game's picture as the IMAG record of its slot.
EASBErrorE fn_8012B0D8(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        fn_80129D70(lbl_802825B0->args.pImage, 1, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_IMAG, lbl_802825B0->nSlot,
                                           lbl_802825B0->pBuffer, EASB_IMAG_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    return eError;
}

// Step: writes the game's record as the PROD record of its slot.
EASBErrorE fn_8012B190(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        fn_801298FC(lbl_802825B0->args.pProduct, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nSlot,
                                           lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE) {
        lbl_802825B0->n94 = 3;
        lbl_802825B0->anProductState[lbl_802825B0->nSlot] = 1;
    }
    return eError;
}

// Step: creates the Bio file on the chosen device; when that completes, keeps the session and
// starts with no records read.
EASBErrorE fn_8012B27C(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 1) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8016CFF8_SetSaveDescriptor(lbl_802825B0->args.pHeader);
        if (eError == EASB_ERROR_NONE) {
            eError = fn_8012C98C(TagFile_Create("EASB", lbl_802825B0->args.eDevice, 0));
        }
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
        eError = fn_8012C98C(TagFile_GetSession(&lbl_802825B0->session));
        memset(&lbl_802825B0->nHeadState, 0, sizeof(lbl_802825B0->nHeadState));
        memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
        lbl_802825B0->n94 = 0;
        lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        lbl_802825B0->b90 = 1;
        lbl_802825B0->bFileOpen = 0;
        lbl_802825B0->b92 = 1;
    }
    return eError;
}

// Step: deletes the Bio file; when that completes, forgets the session, every record read and
// the game's slot.
EASBErrorE fn_8012B3B0(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 1) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        eError = fn_8012C98C(fn_80174DF0_Delete("EASB", SFIO_DEVICE_INVALID, 0));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
        if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
            memset(&lbl_802825B0->session, 0, sizeof(TagSession));
            memset(&lbl_802825B0->nHeadState, 0, sizeof(lbl_802825B0->nHeadState));
            memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
            lbl_802825B0->bFileOpen = 0;
            lbl_802825B0->b92 = 0;
            lbl_802825B0->n94 = 0;
            lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        }
    }
    return eError;
}

// Step: reads the HEAD record, then every PROD record, adding each valid record into the
// totals. A corrupt record is marked (state 2) and skipped.
EASBErrorE fn_8012B4C0(EASBProcessE* peProcess) {
    EASBProduct product;
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        memset(&lbl_802825B0->nHeadState, 0, sizeof(lbl_802825B0->nHeadState));
        memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
        memset(&lbl_802825B0->totals, 0, sizeof(EASBTotals));
        lbl_802825B0->nRecord = EASB_PRODUCT_NONE;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_HEAD, 0, lbl_802825B0->pBuffer,
                                          EASB_HEAD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && lbl_802825B0->nRecord == EASB_PRODUCT_NONE) {
        if (eError == EASB_ERROR_NONE) {
            lbl_802825B0->nHeadState = 1;
        } else if (eError == EASB_ERROR_SECTION_CORRUPT) {
            lbl_802825B0->nHeadState = 2;
        } else {
            return eError;
        }
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                          lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_COMPLETE && lbl_802825B0->nRecord != EASB_PRODUCT_NONE) {
        if (eError == EASB_ERROR_NONE) {
            fn_80129B30(&product, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 1;
            fn_80128580(&lbl_802825B0->totals, &product);
        } else if (eError == EASB_ERROR_SECTION_CORRUPT) {
            eError = EASB_ERROR_NONE;
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 2;
        } else {
            return eError;
        }
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            eError = fn_8012C98C(TagFile_Read(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                              lbl_802825B0->pBuffer, EASB_PROD_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        }
    }
    return eError;
}

// Step: rewrites the records marked corrupt: the HEAD record from the totals, a PROD record
// empty.
EASBErrorE fn_8012B708(EASBProcessE* peProcess) {
    EASBErrorE eError;
    u8 nNext;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(0)) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        lbl_802825B0->nRecord = EASB_PRODUCT_NONE;
        if (lbl_802825B0->nHeadState == 2) {
            fn_80129754(&lbl_802825B0->totals, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
            eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_HEAD, 0,
                                               lbl_802825B0->pBuffer, EASB_HEAD_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        } else {
            *peProcess = EASB_PROCESS_COMPLETE;
        }
        if (*peProcess == EASB_PROCESS_NONE) {
            *peProcess = EASB_PROCESS_COMPLETE;
        }
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE) {
        nNext = lbl_802825B0->nRecord;
        if (nNext == EASB_PRODUCT_NONE) {
            lbl_802825B0->nHeadState = 1;
            nNext = 0;
        } else {
            lbl_802825B0->anProductState[nNext] = 1;
            nNext++;
        }
        lbl_802825B0->nRecord = nNext;
        while (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS) {
            if (lbl_802825B0->anProductState[lbl_802825B0->nRecord] == 2) {
                memset(lbl_802825B0->pBuffer, 0, EASB_PROD_SIZE);
                eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_PROD,
                                                   lbl_802825B0->nRecord, lbl_802825B0->pBuffer,
                                                   EASB_PROD_SIZE));
                *peProcess = EASB_PROCESS_CONTINUE;
                break;
            }
            lbl_802825B0->nRecord++;
        }
    }
    return eError;
}

// Step: rewrites the PROD records marked corrupt, the game's own from its record and the
// others empty.
EASBErrorE fn_8012B8E4(EASBProcessE* peProcess) {
    EASBErrorE eError;
    u8 nNext;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (fn_8012A20C(0)) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (*peProcess == EASB_PROCESS_NONE
        || (*peProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE)) {
        if (*peProcess == EASB_PROCESS_NONE) {
            nNext = 0;
        } else {
            nNext = lbl_802825B0->nRecord;
            lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 1;
            nNext++;
        }
        lbl_802825B0->nRecord = nNext;
        while (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS) {
            if (lbl_802825B0->anProductState[lbl_802825B0->nRecord] == 2) {
                if (lbl_802825B0->nRecord == lbl_802825B0->nSlot) {
                    fn_801298FC(lbl_802825B0->args.pProduct, lbl_802825B0->pBuffer,
                                lbl_802825B0->uBufferSize);
                } else {
                    memset(lbl_802825B0->pBuffer, 0, EASB_PROD_SIZE);
                }
                eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_PROD,
                                                   lbl_802825B0->nRecord, lbl_802825B0->pBuffer,
                                                   EASB_PROD_SIZE));
                *peProcess = EASB_PROCESS_CONTINUE;
                break;
            }
            lbl_802825B0->nRecord++;
        }
    }
    return eError;
}

// Step: writes the 25 PROD records of a new file: the game's record in slot 0, the others
// empty.
EASBErrorE fn_8012BA58(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
        fn_801298FC(lbl_802825B0->args.pProduct, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                           lbl_802825B0->pBuffer, EASB_PROD_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
        lbl_802825B0->anProductState[lbl_802825B0->nRecord] = 1;
        if (lbl_802825B0->nRecord == 0) {
            lbl_802825B0->n94 = 3;
            lbl_802825B0->nSlot = 0;
        }
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            memset(lbl_802825B0->pBuffer, 0, EASB_PROD_SIZE);
            eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_PROD, lbl_802825B0->nRecord,
                                               lbl_802825B0->pBuffer, EASB_PROD_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        }
    }
    return eError;
}

// Step: writes 25 IMAG records: the game's picture in slot 0, the others empty. No operation
// lists this step.
EASBErrorE fn_8012BBD8(EASBProcessE* peProcess) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0->bFileOpen == 0) {
        return EASB_ERROR_INTERNAL;
    }
    if (*peProcess == EASB_PROCESS_NONE) {
        fn_80129D70(lbl_802825B0->args.pImage, 1, lbl_802825B0->pBuffer, lbl_802825B0->uBufferSize);
        lbl_802825B0->nRecord = 0;
        eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_IMAG, lbl_802825B0->nRecord,
                                           lbl_802825B0->pBuffer, EASB_IMAG_SIZE));
        *peProcess = EASB_PROCESS_CONTINUE;
    } else if (*peProcess == EASB_PROCESS_CONTINUE) {
        eError = fn_8012CB98(peProcess);
    }
    if (eError == EASB_ERROR_NONE && *peProcess == EASB_PROCESS_COMPLETE) {
        if (lbl_802825B0->nRecord < EASB_MAX_PRODUCTS - 1) {
            lbl_802825B0->nRecord++;
            memset(lbl_802825B0->pBuffer, 0, EASB_IMAG_SIZE);
            eError = fn_8012C98C(TagFile_Write(&lbl_802825B0->session, EASB_TAG_IMAG, lbl_802825B0->nRecord,
                                               lbl_802825B0->pBuffer, EASB_IMAG_SIZE));
            *peProcess = EASB_PROCESS_CONTINUE;
        }
    }
    return eError;
}

// Starts the storage code: the state, with no file, slot or operation, and a copy of the
// memory-card callbacks.
EASBErrorE fn_8012BD0C(u32 uHeapID, SFIOFuncTable* pCallbacks) {
    if (lbl_802825B0 != NULL) {
        return EASB_ERROR_INITIALIZED;
    }
    lbl_802825B0 = TibExtMemAlloc(uHeapID, sizeof(EASBStorage), 4);
    if (lbl_802825B0 == NULL) {
        return EASB_ERROR_OUT_OF_MEMORY;
    }
    memset(lbl_802825B0, 0, sizeof(EASBStorage));
    lbl_802825B0->uHeapID = uHeapID;
    lbl_802825B0->b90 = 0;
    lbl_802825B0->bFileOpen = 0;
    lbl_802825B0->b92 = 0;
    lbl_802825B0->nOperation = EASB_OPERATION_NONE;
    lbl_802825B0->eStepProcess = EASB_PROCESS_NONE;
    lbl_802825B0->pnSteps = NULL;
    lbl_802825B0->n94 = 0;
    lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
    lbl_802825B0->pBuffer = NULL;
    memset(&lbl_802825B0->nHeadState, 0, sizeof(lbl_802825B0->nHeadState));
    memset(lbl_802825B0->anProductState, 0, sizeof(lbl_802825B0->anProductState));
    // The buffer holds the biggest record.
    lbl_802825B0->uBufferSize = EASB_HEAD_SIZE;
    if (lbl_802825B0->uBufferSize < EASB_PROD_SIZE) {
        lbl_802825B0->uBufferSize = EASB_PROD_SIZE;
    }
    if (lbl_802825B0->uBufferSize < EASB_IMAG_SIZE) {
        lbl_802825B0->uBufferSize = EASB_IMAG_SIZE;
    }
    lbl_802825B0->callbacks = *pCallbacks;
    return EASB_ERROR_NONE;
}

// Stops the storage code; not while the file is open.
EASBErrorE fn_8012BE74(void) {
    EASBErrorE eError;

    if (lbl_802825B0 == NULL) {
        return EASB_ERROR_NOT_INITIALIZED;
    }
    if (lbl_802825B0->bFileOpen == 1) {
        return EASB_ERROR_FILE_OPEN;
    }
    if (lbl_802825B0->pBuffer != NULL) {
        eError = fn_8012C98C(TagFile_FreeBuffer(lbl_802825B0->pBuffer, (void*)lbl_802825B0->uHeapID,
                                                lbl_802825B0->uBufferSize, 0));
        lbl_802825B0->pBuffer = NULL;
    } else {
        eError = EASB_ERROR_NONE;
    }
    TibExtMemFree(lbl_802825B0->uHeapID, lbl_802825B0, sizeof(EASBStorage), 4);
    lbl_802825B0 = NULL;
    return eError;
}

// Starts operation nOperation with pArgs, first starting the tag-file library if it needs it.
EASBErrorE fn_8012BF18(s32 nOperation, EASBStorageArgs* pArgs) {
    int aDevices[2] = {SFIO_DEVICE_FIRST, SFIO_DEVICE_INVALID};
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (lbl_802825B0->nOperation == EASB_OPERATION_NONE) {
        if (lbl_80195520[nOperation].bStartTagFile) {
            eError = fn_8012CAA8(&lbl_802825B0->callbacks, aDevices);
        }
        if (eError == EASB_ERROR_NONE) {
            lbl_802825B0->nOperation = nOperation;
            lbl_802825B0->eStepProcess = EASB_PROCESS_NONE;
            lbl_802825B0->pnSteps = lbl_80195520[nOperation].anSteps;
            lbl_802825B0->nResult = EASB_ERROR_NONE;
            lbl_802825B0->nLastOperation = EASB_OPERATION_NONE;
            if (lbl_802825B0->nOperation != EASB_OPERATION_ERROR) {
                memcpy(&lbl_802825B0->session, &lbl_802825B0->savedSession, sizeof(TagSession));
            }
            lbl_802825B0->nRecord = 0;
            memcpy(&lbl_802825B0->args, pArgs, sizeof(EASBStorageArgs));
        }
    } else {
        eError = EASB_ERROR_PROCESS_IN_PROGRESS;
    }
    return eError;
}

// Ends operation nOperation, shutting the tag-file library down if the table says so. After the
// clean-up operation (EASB_OPERATION_ERROR) the result is the failed operation's error.
EASBErrorE fn_8012C03C(s32 nOperation) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (nOperation == lbl_802825B0->nOperation) {
        lbl_802825B0->nOperation = EASB_OPERATION_NONE;
        lbl_802825B0->eStepProcess = EASB_PROCESS_NONE;
        lbl_802825B0->pnSteps = NULL;
        lbl_802825B0->nRecord = 0;
        if (lbl_80195520[nOperation].bStopTagFile) {
            eError = fn_8012CC48();
            lbl_802825B0->bFileOpen = 0;
        }
        if (nOperation == EASB_OPERATION_ERROR) {
            if (lbl_802825B0->nLastOperation == 0 || lbl_802825B0->nLastOperation == 4) {
                memset(&lbl_802825B0->session, 0, sizeof(TagSession));
                lbl_802825B0->bFileOpen = 0;
                lbl_802825B0->b90 = 0;
                lbl_802825B0->b92 = 0;
                lbl_802825B0->n94 = 0;
                lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
            }
            eError = lbl_802825B0->nResult;
        } else if (nOperation == 2) {
            lbl_802825B0->bFileOpen = 0;
            lbl_802825B0->b90 = 0;
            lbl_802825B0->b92 = 0;
            lbl_802825B0->n94 = 0;
            lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        } else {
            memcpy(&lbl_802825B0->savedSession, &lbl_802825B0->session, sizeof(TagSession));
        }
    } else if (lbl_802825B0->nOperation == EASB_OPERATION_NONE) {
        eError = EASB_ERROR_NO_PROCESS_IN_PROGRESS;
    } else {
        eError = EASB_ERROR_PROCESS_IN_PROGRESS;
    }
    return eError;
}

// After a step fails. A card error (100-118) in operations 0, 4 and 5, and any error in
// operation 6 or the clean-up itself, ends the operation there; any other failure starts the
// clean-up operation, which later returns the error.
void fn_8012C1AC(EASBErrorE* peError, EASBProcessE* peProcess) {
    EASBStorageArgs args;
    s32 nOperation;
    u8 bCleanUp;

    nOperation = lbl_802825B0->nOperation;
    if (nOperation == EASB_OPERATION_NONE) {
        return;
    }
    if (nOperation != EASB_OPERATION_ERROR && nOperation != 6) {
        if (nOperation == 0 || nOperation == 4 || nOperation == 5) {
            if (lbl_802825B0->nLastError < 100 || lbl_802825B0->nLastError > 118) {
                bCleanUp = 1;
            } else {
                bCleanUp = 0;
            }
        } else {
            bCleanUp = 1;
        }
    } else {
        bCleanUp = 0;
    }
    if (bCleanUp == 1) {
        lbl_802825B0->nOperation = EASB_OPERATION_NONE;
        memset(&args, 0, sizeof(EASBStorageArgs));
        fn_8012BF18(EASB_OPERATION_ERROR, &args);
        *peProcess = EASB_PROCESS_CONTINUE;
        lbl_802825B0->nLastOperation = nOperation;
        lbl_802825B0->nResult = *peError;
        *peError = EASB_ERROR_NONE;
        return;
    }
    *peProcess = EASB_PROCESS_COMPLETE;
    lbl_802825B0->nOperation = EASB_OPERATION_NONE;
    lbl_802825B0->eStepProcess = EASB_PROCESS_NONE;
    lbl_802825B0->pnSteps = NULL;
    lbl_802825B0->nRecord = 0;
    if (lbl_80195520[nOperation].bStartTagFile || lbl_80195520[nOperation].bStopTagFile) {
        if ((BOOL)TagFile_IsInitialised()) {  // fake match: tested as a byte (clrlwi.)
            fn_8012CC48();
        }
        lbl_802825B0->bFileOpen = 0;
    }
    if (nOperation == EASB_OPERATION_ERROR) {
        if (lbl_802825B0->nLastOperation == 0 || lbl_802825B0->nLastOperation == 4) {
            memset(&lbl_802825B0->session, 0, sizeof(TagSession));
            lbl_802825B0->bFileOpen = 0;
            lbl_802825B0->b90 = 0;
            lbl_802825B0->b92 = 0;
            lbl_802825B0->n94 = 0;
            lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        }
        *peError = lbl_802825B0->nResult;
    }
}

// Runs the current operation's step once. *peProcess says whether the operation goes on,
// *pnOperation which operation it is.
EASBErrorE fn_8012C388(EASBProcessE* peProcess, s32* pnOperation) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (lbl_802825B0->nOperation != EASB_OPERATION_NONE && lbl_802825B0->pnSteps == NULL) {
        return EASB_ERROR_INTERNAL;
    }
    *pnOperation = lbl_802825B0->nOperation;
    if (lbl_802825B0->nOperation == EASB_OPERATION_NONE) {
        *peProcess = EASB_PROCESS_NONE;
        return EASB_ERROR_NONE;
    }
    switch (*lbl_802825B0->pnSteps) {
    case 0:
        eError = fn_8012A2A8(&lbl_802825B0->eStepProcess);
        break;
    case 1:
        eError = fn_8012A364(&lbl_802825B0->eStepProcess);
        break;
    case 2:
        eError = fn_8012A434(&lbl_802825B0->eStepProcess);
        break;
    case 3:
        eError = fn_8012A4C4(&lbl_802825B0->eStepProcess);
        break;
    case 4:
        eError = fn_8012A95C(&lbl_802825B0->eStepProcess);
        break;
    case 5:
        eError = fn_8012AA7C(&lbl_802825B0->eStepProcess);
        break;
    case 6:
        eError = fn_8012AC40(&lbl_802825B0->eStepProcess);
        break;
    case 9:
        eError = fn_8012B190(&lbl_802825B0->eStepProcess);
        break;
    case 7:
        eError = fn_8012AE40(&lbl_802825B0->eStepProcess);
        break;
    case 10:
        eError = fn_8012B0D8(&lbl_802825B0->eStepProcess);
        break;
    case 8:
        eError = fn_8012B004(&lbl_802825B0->eStepProcess);
        break;
    case 11:
        eError = fn_8012B27C(&lbl_802825B0->eStepProcess);
        break;
    case 12:
        eError = fn_8012A900(&lbl_802825B0->eStepProcess);
        break;
    case 15:
        eError = fn_8012B3B0(&lbl_802825B0->eStepProcess);
        break;
    case 17:
        eError = fn_8012B4C0(&lbl_802825B0->eStepProcess);
        break;
    case 18:
        eError = fn_8012B708(&lbl_802825B0->eStepProcess);
        break;
    case 19:
        eError = fn_8012B8E4(&lbl_802825B0->eStepProcess);
        break;
    case 13:
        eError = fn_8012BA58(&lbl_802825B0->eStepProcess);
        break;
    case 14:
        eError = fn_8012BBD8(&lbl_802825B0->eStepProcess);
        break;
    case 16:
        eError = fn_8012A848(&lbl_802825B0->eStepProcess);
        break;
    default:
        eError = EASB_ERROR_UNKNOWN;
        break;
    }
    *peProcess = lbl_802825B0->eStepProcess;
    if (lbl_802825B0->eStepProcess == EASB_PROCESS_COMPLETE && eError == EASB_ERROR_NONE) {
        lbl_802825B0->pnSteps++;
        if (*lbl_802825B0->pnSteps != EASB_STEP_END) {
            lbl_802825B0->eStepProcess = EASB_PROCESS_NONE;
            *peProcess = EASB_PROCESS_CONTINUE;
        } else {
            eError = fn_8012C03C(lbl_802825B0->nOperation);
        }
    }
    if (eError != EASB_ERROR_NONE) {
        fn_8012C1AC(&eError, peProcess);
    }
    return eError;
}

// Operations 4 and 5 (fn_8012D560's): 4 when no file has been opened yet, else 5, which needs
// b92.
EASBErrorE fn_8012C5F8(EASBTotals* pTotals, EASBProduct* pProduct, char* szName) {
    EASBStorageArgs args;

    memset(&args, 0, sizeof(EASBStorageArgs));
    if (lbl_802825B0->b90 == 0) {
        lbl_802825B0->nSlot = EASB_PRODUCT_NONE;
        args.pTotals = pTotals;
        args.pProduct = pProduct;
        args.szName = szName;
        return fn_8012BF18(4, &args);
    }
    if (lbl_802825B0->b92 == 0) {
        return EASB_ERROR_CANNOT_REOPEN;
    }
    return fn_8012BF18(5, &args);
}

EASBErrorE fn_8012C69C(void) {
    EASBStorageArgs args;

    memset(&args, 0, sizeof(EASBStorageArgs));
    return fn_8012BF18(6, &args);
}

// Operation 0: creates the Bio file on eDevice.
EASBErrorE fn_8012C6D4(EASBTotals* pTotals, EASBProduct* pProduct, int eDevice, void* pHeader,
                       EASBImage* pImage) {
    EASBStorageArgs args;

    memset(&args, 0, sizeof(EASBStorageArgs));
    args.pTotals = pTotals;
    args.pProduct = pProduct;
    args.eDevice = eDevice;
    args.pHeader = pHeader;
    args.pImage = pImage;
    return fn_8012BF18(0, &args);
}

EASBErrorE fn_8012C73C(void) {
    EASBStorageArgs args;

    memset(&args, 0, sizeof(EASBStorageArgs));
    return fn_8012BF18(1, &args);
}

EASBErrorE fn_8012C774(EASBProduct* pProducts) {
    EASBStorageArgs args;

    memset(&args, 0, sizeof(EASBStorageArgs));
    args.pProduct = pProducts;
    return fn_8012BF18(8, &args);
}

// Operation 7; not without a slot for the game.
EASBErrorE fn_8012C7BC(EASBTotals* pTotals, EASBProduct* pProduct, EASBImage* pImage) {
    EASBStorageArgs args;

    if (lbl_802825B0->nSlot >= EASB_MAX_PRODUCTS) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    memset(&args, 0, sizeof(EASBStorageArgs));
    args.pTotals = pTotals;
    args.pProduct = pProduct;
    args.pImage = pImage;
    return fn_8012BF18(7, &args);
}

u8 fn_8012C83C(void) {
    return lbl_802825B0->bFileOpen;
}

u8 fn_8012C848(void) {
    return lbl_802825B0->b92;
}

EASBErrorE fn_8012C854(u8* pnSlot) {
    if (pnSlot == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0 == NULL) {
        return EASB_ERROR_NOT_INITIALIZED;
    }
    *pnSlot = lbl_802825B0->nSlot;
    return EASB_ERROR_NONE;
}

EASBErrorE fn_8012C888(u32* pOut) {
    EASBErrorE eError;

    eError = EASB_ERROR_NONE;
    if (pOut == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    if (lbl_802825B0 == NULL) {
        return EASB_ERROR_NOT_INITIALIZED;
    }
    *pOut = lbl_802825B0->n94;
    if ((s32)*pOut == 0) {  // fake match: the original compares signed (cmpwi)
        eError = EASB_ERROR_NOFILE;
    }
    return eError;
}

// The size of the Bio file: a HEAD record and 25 PROD records as the tag file pads them; with
// bCard set, what that takes on the card (fn_8017124C, which reads only the sizes it is given).
EASBErrorE fn_8012C8D0(u32* puSize, u8 bCard) {
    u32 uRecordSize;
    u32 uEntries;
    SFIODescriptor desc;
    u32 i;

    *puSize = 0;
    TagFile_GetBufferSizeForPayload(EASB_HEAD_SIZE, &uRecordSize);
    *puSize += uRecordSize;
    for (i = 0; i < EASB_MAX_PRODUCTS; i++) {
        TagFile_GetBufferSizeForPayload(EASB_PROD_SIZE, &uRecordSize);
        *puSize += uRecordSize;
    }
    if (bCard == 1) {
        desc.uDataSize = *puSize;
        desc.uSize04 = 0x2000;
        fn_8017124C(&desc, puSize, &uEntries);
    }
    return EASB_ERROR_NONE;
}

// The EASB error for a tag-file library error; the library's own code is kept for later.
EASBErrorE fn_8012C98C(int eTagError) {
    EASBErrorE eError;

    switch (eTagError) {
    case 0:
        eError = EASB_ERROR_NONE;
        break;
    case 1:
        eError = EASB_ERROR_INITIALIZED;
        break;
    case 2:
        eError = EASB_ERROR_NOT_INITIALIZED;
        break;
    case 4:
        eError = EASB_ERROR_INVALID_PARAMETERS;
        break;
    case 6:
    case 0x6B:
        eError = EASB_ERROR_OUT_OF_MEMORY;
        break;
    case 0x68:
        eError = EASB_ERROR_NOFILE;
        break;
    case 0x67:
        eError = EASB_ERROR_NODEVICE;
        break;
    case 0x72:
        eError = EASB_ERROR_NOT_ENOUGH_SPACE;
        break;
    case 9:
        eError = EASB_ERROR_WRONG_FILE;
        break;
    case 0x6C:
        eError = EASB_ERROR_FILE_EXISTS;
        break;
    case 0xA:
        eError = EASB_ERROR_SECTION_CORRUPT;
        break;
    case 3:
    case 7:
    case 0x6E:
        eError = EASB_ERROR_FILE_CORRUPT;
        break;
    default:
        eError = EASB_ERROR_UNKNOWN;
        break;
    }
    lbl_802825B0->nLastError = eTagError;
    return eError;
}

// Starts the tag-file library with the memory-card callbacks and the cipher (keyed with EA's
// copyright line), and allocates the record buffer.
EASBErrorE fn_8012CAA8(SFIOFuncTable* pCallbacks, int* pDevices) {
    char szKey[] = "Copyright Electronic Arts";
    TagFileInitParams params;
    EASBErrorE eError;

    params.uMaxEntries = 26;
    params.pDevices = pDevices;
    params.pFuncs = pCallbacks;
    params.pKey = (u8*)szKey;
    params.uKeyLen = sizeof(szKey);
    params.pCipher = (const CipherInterface*)Cipher_GetInterface();
    params.pAllocator = (void*)lbl_802825B0->uHeapID;
    eError = fn_8012C98C(TagFile_Init(&params));
    if (eError == EASB_ERROR_NONE && lbl_802825B0->pBuffer == NULL) {
        eError = TagFile_AllocBuffer((void**)&lbl_802825B0->pBuffer, (void*)lbl_802825B0->uHeapID,
                                     lbl_802825B0->uBufferSize, 0);
        if (eError == 0) {
            if (lbl_802825B0->pBuffer == NULL) {
                eError = EASB_ERROR_OUT_OF_MEMORY;
            } else {
                eError = fn_8012C98C(eError);
            }
        } else {
            eError = fn_8012C98C(eError);
        }
    }
    return eError;
}

// Runs the tag-file library's current operation once: *peProcess becomes where it has got to,
// or EASB_PROCESS_COMPLETE on an error.
EASBErrorE fn_8012CB98(EASBProcessE* peProcess) {
    int nProcess;
    int nResult;
    EASBProcessE eProcess;
    EASBErrorE eError;

    eProcess = EASB_PROCESS_NONE;
    if (peProcess == NULL) {
        return EASB_ERROR_NULL_PARAMETERS;
    }
    eError = fn_8012C98C(TagFile_Update(&nProcess, &nResult));
    if (eError == EASB_ERROR_NONE) {
        switch (nProcess) {
        case 0:
            eProcess = EASB_PROCESS_NONE;
            break;
        case 1:
            eProcess = EASB_PROCESS_CONTINUE;
            break;
        case 2:
            eProcess = EASB_PROCESS_COMPLETE;
            break;
        default:
            eError = EASB_ERROR_UNKNOWN;
            break;
        }
    }
    *peProcess = eProcess;
    if (eError != EASB_ERROR_NONE) {
        *peProcess = EASB_PROCESS_COMPLETE;
    }
    return eError;
}

// Frees the record buffer and shuts the tag-file library down.
EASBErrorE fn_8012CC48(void) {
    int eTagError;
    EASBErrorE eError;

    eTagError = 0;
    if (lbl_802825B0->pBuffer != NULL) {
        eTagError = TagFile_FreeBuffer(lbl_802825B0->pBuffer, (void*)lbl_802825B0->uHeapID,
                                       lbl_802825B0->uBufferSize, 0);
        eError = fn_8012C98C(eTagError);
        lbl_802825B0->pBuffer = NULL;
    } else {
        eError = EASB_ERROR_UNKNOWN;
    }
    if (eError == EASB_ERROR_NONE) {
        // The shutdown's own error is dropped: the free's is converted again.
        TagFile_Shutdown();
        eError = fn_8012C98C(eTagError);
    }
    return eError;
}

s32 fn_8012CCC0(void) {
    return lbl_802825B0->nLastOperation;
}

void fn_8012CCCC(int uHandle) {
    lbl_802825B0->session.Sfio.uHandle = uHandle;
}
