// EASBStorage.c (our name): the storage half of the EA Sports Bio library (the player profile
// shared across EA games), under EASB.c's public calls: checking, sorting and packing the product
// records, the level table, and the memory-card steps through the tag-file library (state at
// lbl_802825B0). It spans SkinBurn.c's end to EASB.c; EA may have split it into several files.

#include "core/easb.h"
#include "platform.h"

// The two-letter language codes the library knows (EASBInitParams.uGamesPlayedTypeLanguage).
#define EASB_LANGUAGE(a, b) (((a) << 8) | (b))

// The upper-case form of a-z; other characters stay as they are.
#define EASB_TO_UPPER(c) (((c) >= 'a' && (c) <= 'z') ? (c) - ('a' - 'A') : (c))

EASBErrorE fn_8012830C(char* sz, u32 uSize, u32* puLength);
s32 fn_80128DD0(u16* szA, u16* szB);
u8 fn_80128E28(u8 c);

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
u8 fn_80127F40(u16 uYear) {
    if ((uYear & 3) || (uYear % 100 == 0 && uYear % 400 != 0)) {
        return 0;
    }
    return 1;
}

// Checks a product record: in use, updated within 2003-2023, u58 at least u5C, and both names
// of a usable length.
EASBErrorE fn_80127F88(EASBProduct* pProduct) {
    u32 uLength;
    EASBErrorE eError;

    if (pProduct == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (pProduct->bValid == 0) return EASB_ERROR_INVALID_PRODUCT;
    if (pProduct->uTime < EASB_TIME_FIRST || pProduct->uTime > EASB_TIME_LAST) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    if (pProduct->u58 < pProduct->u5C) return EASB_ERROR_INVALID_PRODUCT;
    if (fn_8012835C(pProduct->szGamesPlayedType, EASB_GAMES_PLAYED_TYPE_SIZE, &uLength) != EASB_ERROR_NONE) {
        return EASB_ERROR_INVALID_PRODUCT;
    }
    eError = EASB_ERROR_NONE;
    if (fn_8012830C(pProduct->szName, EASB_PRODUCT_NAME_SIZE, &uLength) != EASB_ERROR_NONE) {
        eError = EASB_ERROR_INVALID_PRODUCT;
    }
    return eError;
}

// Checks an accomplishment: in use, set within 2003-2023, u86 in 1-250 and a usable name.
EASBErrorE fn_80128054(EASBAccomplishment* pAccomplishment) {
    u32 uLength;
    EASBErrorE eError;

    if (pAccomplishment == NULL) return EASB_ERROR_NULL_PARAMETERS;
    if (pAccomplishment->bValid == 0) return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    if (pAccomplishment->uTime < EASB_TIME_FIRST || pAccomplishment->uTime > EASB_TIME_LAST) {
        return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    if (pAccomplishment->u86 < 1 || pAccomplishment->u86 > 250) {
        return EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    eError = EASB_ERROR_NONE;
    if (fn_8012835C(pAccomplishment->szName, EASB_ACCOMPLISHMENT_NAME_SIZE, &uLength) != EASB_ERROR_NONE) {
        eError = EASB_ERROR_INVALID_ACCOMPLISHMENT;
    }
    return eError;
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

// The length of sz in *puLength: too large when it fills uSize, too small when empty.
EASBErrorE fn_8012830C(char* sz, u32 uSize, u32* puLength) {
    *puLength = 0;
    while (sz[*puLength] != '\0' && *puLength < uSize) {
        (*puLength)++;
    }
    if (*puLength >= uSize) return EASB_ERROR_STRING_TOO_LARGE;
    if (*puLength == 0) return EASB_ERROR_STRING_TOO_SMALL;
    return EASB_ERROR_NONE;
}

// The same for wide text, uSize in characters.
EASBErrorE fn_8012835C(u16* sz, u32 uSize, u32* puLength) {
    *puLength = 0;
    while (sz[*puLength] != 0 && *puLength < uSize) {
        (*puLength)++;
    }
    if (*puLength >= uSize) return EASB_ERROR_STRING_TOO_LARGE;
    if (*puLength == 0) return EASB_ERROR_STRING_TOO_SMALL;
    return EASB_ERROR_NONE;
}

// Checks the game's start-up parameters (fn_8012D394).
EASBErrorE fn_801283B0(EASBInitParams* pParams) {
    u32 uNameLength;
    u32 uTypeLength;
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

// Adds x and y, saturating at 0xFFFFFFFF.
u32 fn_80128468(u32 x, u32 y) {
    u32 uSum;

    uSum = x + y;
    if (uSum < x || uSum < y) {
        uSum = 0xFFFFFFFF;
    }
    return uSum;
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
void fn_80128580(EASBTotals* pTotals, EASBProduct* pProduct) {
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
void fn_80128624(EASBProduct* aProducts, s32 nCount) {
    EASBProduct product;
    s32 nGap;
    s32 i;
    s32 j;

    nGap = 1;
    while (nGap <= nCount / 9) {
        nGap = nGap * 3 + 1;
    }
    for (; nGap > 0; nGap /= 3) {
        for (i = nGap; i < nCount; i++) {
            memcpy(&product, &aProducts[i], sizeof(EASBProduct));
            for (j = i - nGap; j >= 0 && fn_80127D84(&product, &aProducts[j]) < 0; j -= nGap) {
                memcpy(&aProducts[j + nGap], &aProducts[j], sizeof(EASBProduct));
            }
            memcpy(&aProducts[j + nGap], &product, sizeof(EASBProduct));
        }
    }
}

// Sorts a list of nCount accomplishments the same way (fn_80127E44's order).
void fn_8012872C(EASBAccomplishment** apList, s32 nCount, s32 nSort) {
    EASBAccomplishment* pAccomplishment;
    s32 nGap;
    s32 i;
    s32 j;

    nGap = 1;
    while (nGap <= nCount / 9) {
        nGap = nGap * 3 + 1;
    }
    for (; nGap > 0; nGap /= 3) {
        for (i = nGap; i < nCount; i++) {
            pAccomplishment = apList[i];
            for (j = i - nGap; j >= 0 && fn_80127E44(pAccomplishment, apList[j], nSort) < 0; j -= nGap) {
                apList[j + nGap] = apList[j];
            }
            apList[j + nGap] = pAccomplishment;
        }
    }
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

// Copies szSrc into szDest, at most uSize - 1 characters, always ending the text.
void fn_80128BF8(char* szDest, char* szSrc, u32 uSize) {
    if (szSrc != NULL && szDest != NULL && uSize != 0 && szSrc != szDest) {
        while (--uSize != 0) {
            if ((*szDest++ = *szSrc++) == '\0') return;
        }
        *szDest = '\0';
    }
}

// The same for wide text.
void fn_80128C4C(u16* szDest, u16* szSrc, u32 uLength) {
    if (szSrc != NULL && szDest != NULL && uLength != 0 && szSrc != szDest) {
        while (--uLength != 0) {
            if ((*szDest++ = *szSrc++) == 0) return;
        }
        *szDest = 0;
    }
}

// Compares two texts like strcmp; bCase 0 ignores the case of a-z.
s32 fn_80128CA0(char* szA, char* szB, u8 bCase) {
    u8 cA;
    u8 cB;

    if (szA == szB || szA == NULL || szB == NULL) return 0;
    if (bCase) {
        while (*szA == *szB++) {
            if (*szA++ == '\0') return 0;
        }
        return (u8)*szA - (u8)szB[-1];
    }
    for (;;) {
        cB = *szB;
        cA = *szA;
        if (EASB_TO_UPPER(cA) != EASB_TO_UPPER(cB)) {
            return EASB_TO_UPPER(cA) - EASB_TO_UPPER(cB);
        }
        if (cA == '\0') return 0;
        szA++;
        szB++;
    }
}

// Compares two wide texts like strcmp.
s32 fn_80128DD0(u16* szA, u16* szB) {
    if (szA == szB || szA == NULL || szB == NULL) return 0;
    while (*szA == *szB++) {
        if (*szA++ == 0) return 0;
    }
    return *szA - szB[-1];
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
    while (*sz != '\0') {
        *sz = fn_80128E28(*sz);
        sz++;
    }
}

// The same for wide text (fn_80128E6C).
void fn_80128F04(u16* sz, u16 uLanguage) {
    while (*sz != 0) {
        *sz = fn_80128E6C(*sz, uLanguage);
        sz++;
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

// ---- sweep code (not yet cleaned up) ----

s32 TagFile_Delete(s32*, s32, s32);
s32 TagFile_GetSession(u8*);
s32 fn_8012C98C();
s32 fn_8012CB98();
extern s32 lbl_802819B0;
extern u8* lbl_802825B0;
s32 TagFile_DeleteSession(u8*);
s32 TagFile_End(u8*);
s32 TagFile_Write(u8*, s32, u8, s32*, s32);
s32 fn_80129754();
s32 fn_80129D70(s32, s32, s32*, s32);
s32 TagFile_FreeBuffer();
s32 TagFile_Shutdown();
s32 fn_8012CCC0(void);
void fn_8012CCCC(s32 v);

s32 fn_8012A2A8(s32* arg0) {
    s32 temp_r0;
    s32 temp_r3;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_Delete(&lbl_802819B0, -1, 0);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
        if ((var_r4 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            temp_r3 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
            var_r4 = temp_r3;
            (*(s8*)((u8*)(lbl_802825B0) + 0x92)) = 1;
            (*(s8*)((u8*)(lbl_802825B0) + 0x90)) = 1;
        }
    }
    return var_r4;
}

s32 fn_8012A364(s32* arg0) {
    s32 temp_r0;
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x92)) == 0) {
        return 0x11;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x90)) == 0) {
        return 0x11;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_DeleteSession(lbl_802825B0 + 0x120);
        var_r5 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r5 = fn_8012CB98(arg0);
        if ((var_r5 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            var_r5 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
        }
    }
    return var_r5;
}

s32 fn_8012A434(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_End(lbl_802825B0 + 0x120);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    if ((s32) *arg0 == 2) {
        (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 0;
    }
    return var_r4;
}

s32 fn_8012A900(s32* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((s32) *arg0 == 0) {
        fn_80128528((*(void**)((u8*)(lbl_802825B0) + 0x184)), (*(void**)((u8*)(lbl_802825B0) + 0x188)));
        *arg0 = 2;
    }
    return 0;
}

s32 fn_8012B004(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x91)) == 0) {
        return 0x21;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        fn_80129754((*(u8**)((u8*)(lbl_802825B0) + 0x184)), (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)));
        TagFile_Write(lbl_802825B0 + 0x120, 0x48454144, 0U, (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), 0x11);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    if (((s32) *arg0 == 2) && (var_r4 == 0)) {
        (*(s32*)((u8*)(lbl_802825B0) + 0xA4)) = 1;
    }
    return var_r4;
}

s32 fn_8012B0D8(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x91)) == 0) {
        return 0x21;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        fn_80129D70((*(s32*)((u8*)(lbl_802825B0) + 0x190)), 1, (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)));
        TagFile_Write(lbl_802825B0 + 0x120, 0x494D4147, (*(u8*)((u8*)(lbl_802825B0) + 0x98)), (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), 0x4301);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    return var_r4;
}

u8 fn_8012C83C(void) {
    return *(u8*)(lbl_802825B0 + 0x91);
}

u8 fn_8012C848(void) {
    return *(u8*)(lbl_802825B0 + 0x92);
}

EASBErrorE fn_8012C854(u8* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(u8*)((u8*)(lbl_802825B0) + 0x98));
    return 0;
}

EASBErrorE fn_8012C888(u32* arg0) {
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(s32*)((u8*)(lbl_802825B0) + 0x94));
    if ((s32) *arg0 == 0) {
        var_r5 = 1;
    }
    return var_r5;
}

void fn_8012CC48(void) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = 0;
    if ((u32) (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) != 0U) {
        var_r31 = TagFile_FreeBuffer((*(u32*)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0x8C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)), 0);
        var_r3 = fn_8012C98C();
        (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) = 0U;
    } else {
        var_r3 = 0x22;
    }
    if (var_r3 == 0) {
        TagFile_Shutdown();
        fn_8012C98C(var_r31);
    }
}

s32 fn_8012CCC0(void) {
    return *(s32*)(lbl_802825B0 + 0x17C);
}

void fn_8012CCCC(s32 v) {
    *(s32*)(lbl_802825B0 + 0x120) = v;
}

// ---- end of sweep code ----
