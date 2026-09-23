// FE_CrAPDB.c (EA's name, from its asserts; TW06): the Create-A-Player database, every asset a
// created golfer can wear or carry (hair, faces, shirts, hats, clubs...). The assets arrive in the
// 'CR_A' stream object and the names they use in 'CR_S'; each asset belongs to one of the
// CRAP_NUM_PARTS parts, has a category and a lock kind, and raises up to two attributes.

#include "game_types.h"
#include "charstate.h"
#include "frontend/fe.h"
#include "game/frontend.h"

s32  fn_800CCA40(Skin* pSkin);          // SkinPart.c: how many choices aParts[3] holds
s32  fn_800CCEA0(Skin* pSkin);          // SkinPart.c: and aSets[3]

// This file, in address order.
void fn_80103920(void);
void fn_80103EFC(CrAPAsset* pAsset);
int  fn_80104F7C(CrAPAsset* pAsset);
void fn_80105188(UStreamObject* pObject);
void fn_801051F4(UStreamObject* pObject);
void fn_80105240(void);
void fn_80105B80(CrAPAsset* pAsset, char* pName);
void fn_80105DAC(void);
void fn_80105EFC(void);
u8   fn_801061F8(s16 nPart, int nCategory, int nWanted);
int  fn_8010766C(MsgArg* pArg, char* sz);

// UISScreen.c's sender, with the front end's view of its arguments (as GameMessages.c declares it;
// uistudio.h has UIStudio* and const s32*, and game/frontend.h cannot be included with it).
void fn_8016B09C(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

int fn_80103B28(int nAsset) {
    CrAPAsset* pAsset = &lbl_80282460->pAssets[nAsset];

    if (pAsset->nLockKind == 28) {
        return pAsset->nLock;
    }
    return nAsset;
}

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

void fn_80103B8C(s8 n) {
    lbl_80282460->n4 = n;
    fn_80103920();
}

s8 fn_80103BB4(void) {
    return lbl_80282460->n4;
}

s8 fn_80103BC0(int nAsset) {
    return lbl_80282460->pAssets[nAsset].n40;
}

// Empty the profile's slot of the asset.
void fn_80103BD8(CrAPAsset* pAsset) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nSlot = pAsset->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        pProfile->aAF80[nSlot] = -1;
    }
}

// Put the asset in its slot of the profile.
void fn_80103C2C(CrAPAsset* pAsset) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nSlot = pAsset->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        pProfile->aAF80[nSlot] = fn_80104F7C(pAsset);
    }
}

// The asset (the one it takes its attributes from) is the one in its slot of the profile.
u8 fn_80103C98(CrAPAsset* pAsset) {
    SaveProfile* pProfile = fn_80077ACC();
    CrAPAsset* pBase = fn_80103B4C(pAsset);
    s16 nSlot = pBase->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        return pProfile->aAF80[nSlot] == fn_80104F7C(pBase);
    }
    return 0;
}

int fn_80103D14(s16 nSlot) {
    SaveProfile* pProfile = fn_80077ACC();

    if (nSlot >= 0 && nSlot < 53) {
        return pProfile->aAF80[nSlot];
    }
    return -1;
}

// Save the created golfer's body skin entries in the profile.
void fn_80103D6C(void) {
    SaveProfile* pProfile = fn_80077ACC();
    Skin* pSkin = lbl_80281EE0->pB4->pChar->pSkin;

    Mem_cpy(pProfile->a5614, pSkin->aParts[3], fn_800CCA40(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pProfile->a5754, pSkin->aSets[3], fn_800CCEA0(pSkin) * sizeof(SkinChoice));
}

// And the entries of its six other skins.
void fn_80103DE0(void) {
    SaveProfile* pProfile = fn_80077ACC();
    Skin* pSkin;
    int i;

    for (i = 0; i < 6; i++) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[i];
        Mem_cpy(pProfile->a5AF4[i], pSkin->aParts[3], fn_800CCA40(pSkin) * sizeof(SkinChoice));
        Mem_cpy(pProfile->a5CD4[i], pSkin->aSets[3], fn_800CCEA0(pSkin) * sizeof(SkinChoice));
    }
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

// The lowest nLock above nAfter among the assets of lock kind nKind (-1: none).
s32 fn_80107084(s32 nKind, s32 nAfter) {
    int i;
    int nBest = 999999999;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nKind == lbl_80282460->pAssets[i].nLockKind) {
            if (lbl_80282460->pAssets[i].nLock > nAfter && lbl_80282460->pAssets[i].nLock < nBest) {
                nBest = lbl_80282460->pAssets[i].nLock;
            }
        }
    }
    if (nBest == 999999999) {
        return -1;
    }
    return nBest;
}

void fn_80107294(s16 n, char* pDst) {
    strcpy(pDst, lbl_801935C8[n]);
}

// Take the asset in the profile's slot nSlot off the golfer being edited.
void fn_801073DC(s16 nSlot) {
    int nAsset;

    if (lbl_80281EE0->pB4 != NULL) {
        if (lbl_80281EE0->pB4->pChar == NULL) {
            return;
        }
        nAsset = fn_80103D14(nSlot);
        if (nAsset >= 0) {
            fn_80103EFC(fn_80104F68(nAsset));
        }
    }
}

// The part an asset is a choice for.
s16 fn_8010742C(int nAsset) {
    return lbl_80282460->pAssets[nAsset].nPart;
}

int fn_80107444(int nAsset) {
    return lbl_80282460->pAssets[nAsset].n38;
}

// Copy the name of an asset's category into pDst.
void fn_8010745C(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pStrings + lbl_80282460->pAssets[nAsset].nCategory);
}

// Copy an asset's name into pDst.
void fn_8010749C(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pAssets[nAsset].szName);
}

u8 fn_801074D4(int nAsset) {
    s16 n2E = fn_80104F68(nAsset)->n2E;

    if (fn_8010742C(nAsset) == 13) {
        return 1;
    }
    switch (n2E) {
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return 1;
    }
    return 0;
}

// Senders the EA Sports Bio screens (EASportsBio.c) use: message nMsg with its values to the front
// end's handler, when there is a front end. A string value goes as a MsgString.

void fn_80107554(int nMsg, s32 nA) {
    MsgArg arg;

    if (lbl_80281F1C != NULL) {
        arg.i = nA;
        fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    }
}

void fn_80107594(int nMsg, s32 nA, char* szB) {
    MsgString str;
    MsgArg args[2];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        fn_8010766C(&args[1], szB);
        fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 2, args);
    }
}

void fn_801075F8(int nMsg, s32 nA, char* szB, s32 nC) {
    MsgArg args[3];
    MsgString str;

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        fn_8010766C(&args[1], szB);
        args[2].i = nC;
        fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 3, args);
    }
}

// Point the string value pArg holds at sz.
int fn_8010766C(MsgArg* pArg, char* sz) {
    ((MsgString*)pArg->p)->pStr = sz;
    ((MsgString*)pArg->p)->nLen = strlen(sz);
    return 0;
}

int fn_801076B0(char* sz, int nMsg) {
    MsgArg arg;
    MsgString str;

    if (lbl_80281F1C == NULL) {
        return -1;
    }
    arg.p = &str;
    fn_8010766C(&arg, sz);
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    return 0;
}

void fn_8010771C(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, f32 fG) {
    MsgArg args[7];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].i = nB;
        args[2].i = nC;
        args[3].i = nD;
        args[4].i = nE;
        args[5].i = nF;
        args[6].f = fG;
        fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 7, args);
    }
}

void fn_80107774(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, s32 nG, s32 nH, s32 nI,
                 s32 nJ) {
    MsgArg args[10];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].i = nB;
        args[2].i = nC;
        args[3].i = nD;
        args[4].i = nE;
        args[5].i = nF;
        args[6].i = nG;
        args[7].i = nH;
        args[8].i = nI;
        args[9].i = nJ;
        fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 10, args);
    }
}
