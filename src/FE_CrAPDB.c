// FE_CrAPDB.c (EA's name, from its asserts; TW06): the Create-A-Player database, every asset a
// created golfer can wear or carry (hair, faces, shirts, hats, clubs...). The assets arrive in the
// 'CR_A' stream object and the names they use in 'CR_S'; each asset belongs to one of the
// CRAP_NUM_PARTS parts, has a category and a lock kind, and raises up to two attributes.

#include "game_types.h"
#include "charstate.h"
#include "frontend/fe.h"

s32  fn_800CCA40(Skin* pSkin);          // SkinPart.c: how many choices aParts[3] holds
s32  fn_800CCEA0(Skin* pSkin);          // SkinPart.c: and aSets[3]

// This file, in address order.
int  fn_80104F7C(CrAPAsset* pAsset);
void fn_80105188(UStreamObject* pObject);
void fn_801051F4(UStreamObject* pObject);
void fn_80105240(void);
void fn_80105B80(CrAPAsset* pAsset, char* pName);
void fn_80105DAC(void);
void fn_80105EFC(void);
u8   fn_801061F8(s16 nPart, int nCategory, int nWanted);

// The asset an asset takes its attributes from.
CrAPAsset* fn_80103B4C(CrAPAsset* pAsset) {
    return fn_80104F68(fn_80103B28(fn_80104F7C(pAsset)));
}

void fn_80103B74(u8 b) {
    lbl_80282460->b14 = b;
}

u8 fn_80103B80(void) {
    return lbl_80282460->b14;
}

s8 fn_80103BB4(void) {
    return lbl_80282460->n4;
}

// Save the created golfer's body skin entries in the profile.
void fn_80103D6C(void) {
    SaveProfile* pProfile = fn_80077ACC();
    Skin* pSkin = lbl_80281EE0->pB4->pChar->pSkin;

    Mem_cpy(pProfile->a5614, pSkin->aParts[3], fn_800CCA40(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pProfile->a5754, pSkin->aSets[3], fn_800CCEA0(pSkin) * sizeof(SkinChoice));
}

// The asset may be picked: it was not locked when last checked, and its aB1CC bit is set.
u8 fn_80104020(int nAsset) {
    SaveProfile* pProfile = fn_80077ACC();
    if (!fn_8001E9CC(pProfile->aAssetLocked, nAsset) && fn_8001E9CC(pProfile->aB1CC, nAsset)) {
        return 1;
    }
    return 0;
}

// The parts whose choices are grouped by category, with an "All ..." entry: headwear, shirts,
// pants and shorts, shoes, eyewear, watches and jewelry, miscellaneous; and part 12.
u8 fn_801048B0(int nPart) {
    if ((u32)nPart <= 2 || (u32)(nPart - 7) <= 1 || nPart == 12 || nPart == 19 || nPart == 20) {
        return 1;
    }
    return 0;
}

CrAPAsset* fn_80104F68(int nAsset) {
    return &lbl_80282460->pAssets[nAsset];
}

int fn_80104F7C(CrAPAsset* pAsset) {
    return pAsset - lbl_80282460->pAssets;
}

// Take the database's stream objects as they load.
void fn_8010508C(void) {
    UStream_RegisterHandler('CR_A', fn_80105188);
    UStream_RegisterHandler('CR_S', fn_801051F4);
}

// Find each part's first asset (the assets are sorted by part; 0 when a part has none).
void fn_801050D0(void) {
    int nPart;
    int i;
    for (nPart = 0; nPart < CRAP_NUM_PARTS; nPart++) {
        lbl_80282474[nPart] = 0;
        for (i = 0; i < lbl_80282460->nAssets; i++) {
            if (nPart == lbl_80282460->pAssets[i].nPart) {
                lbl_80282474[nPart] = i;
                break;
            }
        }
    }
}

int fn_80105140(s16 nPart) {
    return lbl_80282474[nPart];
}

void fn_80105154(void) {
    UStream_UnregisterHandler('CR_A');
    UStream_UnregisterHandler('CR_S');
}

// The 'CR_A' handler: the assets.
void fn_80105188(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pAssets = (CrAPAsset*)pObject->pData;
        lbl_80282460->nAssets = pObject->uSize / sizeof(CrAPAsset);
        fn_80105DAC();
        lbl_80282464 = pObject;
        fn_80105240();
    }
}

// The 'CR_S' handler: the names.
void fn_801051F4(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pStrings = (char*)pObject->pData;
        lbl_80282460->uStringsSize = pObject->uSize;
        fn_80105EFC();
        lbl_80282468 = pObject;
    }
}

// With the assets loaded: index the parts and pick the day's random assets.
void fn_80105240(void) {
    fn_801050D0();
    fn_80077B78();
}

// A part's choice i: its name, and the fields below (0 or -1 when there is no such choice).
char* fn_80105264(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return NULL;
    }
    return pAsset->szName;
}

s16 fn_80105298(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n44;
}

s16 fn_801052CC(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n46;
}

s16 fn_80105300(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n48;
}

s16 fn_80105334(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n2C;
}

s32 fn_80105368(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n30;
}

s32 fn_8010539C(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n34;
}

s32 fn_801053D0(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n38;
}

// A part's choice i: the attributes it raises and the tiers (see fn_80105494).
int fn_80105404(s16 nPart, int b, int i) {
    return fn_80105494(fn_80104FA8(nPart, b, i));
}

int fn_80105428(s16 nPart, int b, int i) {
    return fn_801054CC(fn_80104FA8(nPart, b, i));
}

int fn_8010544C(s16 nPart, int b, int i) {
    return fn_80105504(fn_80104FA8(nPart, b, i));
}

int fn_80105470(s16 nPart, int b, int i) {
    return fn_8010553C(fn_80104FA8(nPart, b, i));
}

// The attributes an asset raises (-1: none) and the tier it raises each to; an asset of lock kind
// 28 has those of the asset it names.
int fn_80105494(int nAsset) {
    nAsset = fn_80103B28(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrA;
}

int fn_801054CC(int nAsset) {
    nAsset = fn_80103B28(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierA;
}

int fn_80105504(int nAsset) {
    nAsset = fn_80103B28(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrB;
}

int fn_8010553C(int nAsset) {
    nAsset = fn_80103B28(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierB;
}

// A part's choice i: its lock kind and number (-1: no such choice).
s8 fn_80105574(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLockKind;
}

s16 fn_801055A8(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLock;
}

s8 fn_801055DC(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLockKind;
}

s16 fn_80105610(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLock;
}

int fn_80105644(s16 nPart, int b, int i, int n) {
    CrAPAsset* pAsset = fn_80104E84(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->a4A[n];
}

void fn_80105B4C(s16 nPart, int b, int i, char* pName) {
    fn_80105B80(fn_80104E84(nPart, b, i), pName);
}

s32 fn_80105C00(void) {
    return lbl_80282460->nAssets;
}

s32 fn_80105C0C(int nAsset) {
    CrAPAsset* pAsset = &lbl_80282460->pAssets[nAsset];
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n38;
}

u8 fn_80105C30(void) {
    return lbl_80282460 != NULL;
}

// Called when the names arrive; empty in this build.
void fn_80105EFC(void) {
}

// An asset with this n40 is offered: it matches the database's n4, or 2 (any).
u8 fn_801061C8(s8 n) {
    if (n == lbl_80282460->n4 || n == 2) {
        return 1;
    }
    return 0;
}

// An asset of category nCategory fits the category a part's list shows (nWanted); a part with an
// "All ..." entry shows every category for -1.
u8 fn_801061F8(s16 nPart, int nCategory, int nWanted) {
    if (lbl_801932C8[nPart][0] != '\0') {
        return nWanted == -1 || nCategory == nWanted;
    }
    return nCategory == nWanted;
}

// Copy the name at nOffset in the 'CR_S' strings into pDst ("" for "NONE").
u8 fn_8010645C(int nOffset, char* pDst) {
    char* pStrings = lbl_80282460->pStrings;
    if (pStrings == NULL) {
        return 0;
    }
    if (pDst == NULL) {
        return 0;
    }
    if (nOffset == -1) {
        return 0;
    }
    strcpy(pDst, pStrings + nOffset);
    if (stricmp(pDst, "NONE") == 0) {
        *pDst = '\0';
    }
    return 1;
}

char* fn_801064EC(int nCategory) {
    if (lbl_80282460->pStrings == NULL) {
        return NULL;
    }
    if (nCategory == -1) {
        return NULL;
    }
    return lbl_80282460->pStrings + nCategory;
}
