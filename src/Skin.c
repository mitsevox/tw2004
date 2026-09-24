// Skin.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "lighting.h"
#include "dynobj.h"
#include "charstate.h"
#include "endian.h"
#include "platform.h"

void  fn_80008380(void);
void  fn_80016978(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);
void  fn_8006E7A4(LightGroup* pGroup);         // GoLighting.c: load the group's lights
void  fn_8006EADC(UObject* pObj);              // GoLighting.c: light the object
void  fn_8006ED70(void);                       // GoLighting.c
void  fn_801127A0(void* pDesc);                // hwsMaterial_Gc.c
void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c
void  fn_800CEE88(u8 b);                       // SkinPart.c
void  fn_800CEF04(SkinDesc* pDesc);            // SkinPart.c: offsets to pointers
void  fn_800CD404(Skin* pSkin);                // SkinPart.c
s32   fn_800CD700(Skin* pSkin);                // SkinPart.c
void  fn_800CE164(void);                       // SkinPart.c
void  fn_8011C9B0(Skin* pSkin);                // SkinMorph.c
void  fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight);     // SkinMorph.c
s32   fn_8011CDE8(Skin* pSkin);                // SkinMorph.c
void  fn_8011CE58(Skin* pSkin);                // SkinMorph.c
HwsMemBlock* fn_801128C8(SkinDesc* pDesc, s32 nSize);         // hwsOverride_Gc.c
HwsOverrideTable* fn_80112A10(SkinDesc* pDesc, s32 nMeshes);  // hwsOverride_Gc.c
void  fn_80112B18(HwsOverrideTable* pTable, int i, void* p);   // hwsOverride_Gc.c

u8    fn_8001EC48(Character* pChar);           // char.c
void  fn_80035D10(Character* pChar, int nView);
void  fn_80035F40(void* pCamera);
void  fn_80036054(void* pMesh, int n, s32* pDesc);
void  fn_800360A0(void* pMesh);
void  fn_8003612C(LightGroup* pGroup);
void  fn_80036278(SkinModel44* pEntries, s32 nEntries);
void  fn_80036344(SkinModel44* pEntries, s32 nEntries);
void  fn_800363B4(SkinModel54* pEntries, s32 nEntries);
void  fn_800364AC(SkinModel* pModel);
void  fn_8003682C(SkinModel* pModel);
void  fn_80036894(SkinDesc* pDesc);
void  fn_800368FC(SkinDesc* pDesc);
void  fn_80037574(BonePose* pBones, s32 nBones);
void  fn_80037D5C(SkinDesc* pDesc);

// ---- sweep code (tidied) ----

void fn_80035F1C(void) {
    fn_80035F40(fn_8001614C());
}

// Hands the camera's screen rectangle on to fn_80016978.
void fn_80035F40(void* pCamera) {
    f32* pRect;

    pRect = fn_80012EF0(pCamera);
    fn_80016978(fn_80012EE8(pRect), fn_80012EE0(pRect), fn_80012ED8(pRect), fn_80012ED0(pRect));
}

// Sets up the triangles' mesh objects, one per view.
void fn_80035C58(void) {
    s32 aDesc[2];
    int i;

    aDesc[0] = 3;
    aDesc[1] = 1;
    for (i = 0; i < 2; i++) {
        fn_80036054(lbl_801D4E78.aMesh[i], 0x13, aDesc);
    }
}

// And frees them.
void fn_80035CC0(void) {
    int i;

    for (i = 0; i < 2; i++) {
        fn_800360A0(lbl_801D4E78.aMesh[i]);
    }
}

// Runs fn_80035D10 for view nView on every character made so far, except those fn_8001EC48 picks
// and those with flag 0x40 or 1.
void fn_80035E98(int nView) {
    int i;

    for (i = 0; i < lbl_80281CA8; i++) {
        if (!fn_8001EC48(lbl_801B9624[i]) && !(lbl_801B9624[i]->u10 & 0x41)) {
            fn_80035D10(lbl_801B9624[i], nView);
        }
    }
}

void fn_80035FBC(void) {
    fn_8006ED70();
}

void fn_80035FDC(UObject* pObj) {
    fn_8006EADC(pObj);
}

// Loads the current light set's lights.
void fn_80035FFC(void) {
    fn_8003612C(&fn_8003532C()->group);
}

void fn_80036024(f32 f) {
    fn_8003532C()->group.v18[0] = f;
}

void fn_800360A0(void* arg0) {
    s32 (*temp_r12)();

    temp_r12 = (*(s32 (**)())((u8*)((*(void**)((u8*)(arg0) + 0x24))) + 4));
    if (temp_r12 != NULL) {
        temp_r12(arg0);
    }
}

void fn_8003612C(LightGroup* pGroup) {
    fn_8006E7A4(pGroup);
}

// pObj's type is not known yet: the one fn_800358E0 is handed.
void fn_8003614C(void* pObj, f32* pOut) {
    if (pObj != NULL) {
        Vec_Copy((f32*)(*(u8**)(*(u8**)((u8*)pObj + 0x38) + 4) + 0x1C), pOut);
    }
}

// Byte-swaps nEntries SkinModel44 entries.
void fn_80036278(SkinModel44* pEntries, s32 nEntries) {
    SwapField aFormat[5] = { { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 2 } };
    void* pSrc;
    void* pDst;
    int i;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntries;
        fn_8001F08C(&pSrc, &pDst, aFormat, 5, 1);
        pEntries++;
    }
}

// Byte-swaps each SkinModel44 entry's p4 array.
void fn_80036344(SkinModel44* pEntries, s32 nEntries) {
    u8* pData;
    u8* pSrc;
    int i;

    for (i = 0; i < nEntries; i++) {
        pData = (u8*)pEntries->p4;
        pSrc = pData;
        fn_80076158(&pSrc, pData, pEntries->n8 * 4, 4);
        pEntries++;
    }
}

// Byte-swaps nEntries SkinModel54 entries.
void fn_800363B4(SkinModel54* pEntries, s32 nEntries) {
    SwapField aFormat[3] = { { 2, 2 }, { 6, 2 }, { 12, 4 } };
    void* pSrc;
    void* pDst;
    int i;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntries;
        fn_8001F08C(&pSrc, &pDst, aFormat, 3, 1);
        pEntries++;
    }
}

void fn_80036460(int n) {
}

void fn_80036464(void) {
    if (lbl_80281D70 != NULL) {
        fn_80009E70(lbl_80281D70);
    }
    lbl_80281D70 = NULL;
    lbl_80281D78 = 0;
    lbl_80281D74 = 0;
}

void fn_800364A0(void) {
    lbl_80281D74 = 0;
}

// Turns a model's offsets into pointers and byte-swaps its tables, once (bit 31 of u30). A model
// that is not version 4 is cleared to an empty one.
void fn_800364AC(SkinModel* pModel) {
    int i;

    if (pModel->u30 & 0x80000000) {
        return;
    }
    if (pModel->n00 != 4) {
        memset(pModel, 0, sizeof(SkinModel));
        pModel->n00 = 4;
        pModel->n04 = 0;
        pModel->n18 = pModel->n1A = pModel->n1C = pModel->n1E = -1;
        pModel->n20 = pModel->n24 = -1;
        pModel->n28 = pModel->n2C = -1;
        pModel->u30 = 0x80000000;
        return;
    }
    if (pModel->p34 != NULL) {
        pModel->p34 = (u8*)pModel + (uptr)pModel->p34;
    }
    if (pModel->p38 != NULL) {
        pModel->p38 = (u8*)pModel + (uptr)pModel->p38;
    }
    if (pModel->p3C != NULL) {
        pModel->p3C = (u8*)pModel + (uptr)pModel->p3C;
    }
    if (pModel->p44 != NULL) {
        pModel->p44 = (SkinModel44*)((u8*)pModel + (uptr)pModel->p44);
    }
    if (pModel->pDesc != NULL) {
        pModel->pDesc = (SkinDesc*)((u8*)pModel + (uptr)pModel->pDesc);
    }
    if (pModel->p54 != NULL) {
        pModel->p54 = (SkinModel54*)((u8*)pModel + (uptr)pModel->p54);
    }
    fn_80036278(pModel->p44, pModel->n40);
    fn_800363B4(pModel->p54, pModel->n50);
    for (i = 0; i < pModel->n40; i++) {
        if (pModel->p44[i].p4 != NULL) {
            pModel->p44[i].p4 = (s32*)((u8*)pModel + (uptr)pModel->p44[i].p4);
        }
        // EA bug: swaps every entry's array once per entry, and reaches entries whose p4 is
        // still an offset; right only for a single entry.
        fn_80036344(pModel->p44, pModel->n40);
    }
    pModel->u30 = pModel->u30 | 0x80000000;
}

// Hands table n (Skin.a10A0) every p44 entry whose bit is set in p10D0, with the matrices.
void fn_80036790(Skin* pSkin, int n) {
    int i;
    s32 nEntries;
    HwsOverrideTable* pTable;

    if (pSkin->a10A0[n] != NULL && (pSkin->u10D4 & 2)) {
        pTable = pSkin->a10A0[n];
        nEntries = pSkin->pModel->n40;
        for (i = 0; i < nEntries; i++) {
            if (fn_8001E9CC(pSkin->p10D0, i)) {
                fn_80112B18(pTable, pSkin->pModel->p44[i].n0, pSkin->p108C);
            }
        }
    }
}

// Byte-swaps a model's header.
void fn_8003682C(SkinModel* pModel) {
    SwapField aFormat[26] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 },
                              { 2, 2 }, { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 8, 1 }, { 224, 4 } };
    void* pSrc;
    void* pDst;

    pDst = pSrc = pModel;
    fn_8001F08C(&pSrc, &pDst, aFormat, 26, 1);
}

// Byte-swaps a skin description's header.
void fn_80036894(SkinDesc* pDesc) {
    SwapField aFormat[48] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 100, 4 } };
    void* pSrc;
    void* pDst;

    pDst = pSrc = pDesc;
    fn_8001F08C(&pSrc, &pDst, aFormat, 48, 1);
}

// Byte-swaps a skin description read from its little-endian file, in place, and turns its mesh
// bit offsets into pointers. Version 8 descriptions with n04 == 0 have the old p14 layout
// (SkinDesc14Old) and are converted to the new one.
void fn_800368FC(SkinDesc* pDesc) {
    SwapField aDesc14[10] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 4 },
                              { 4, 4 }, { 8, 4 }, { 8, 4 } };
    SwapField aOld14[9] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                            { 8, 4 }, { 8, 4 } };
    SwapField aDesc18[3] = { { 4, 4 }, { 4, 4 }, { 16, 4 } };
    SwapField aDesc28[5] = { { 4, 4 }, { 4, 4 }, { 8, 1 }, { 4, 4 }, { 4, 4 } };
    SwapField aMesh[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc44[12] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aPart[3] = { { 8, -8 }, { 4, 4 }, { 4, 4 } };
    SwapField aVariant[5] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc74[5] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc7C[4] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc8C[5] = { { 8, -8 }, { 12, 4 }, { 12, 4 }, { 12, 4 }, { 4, 4 } };
    SwapField aLink[2] = { { 8, -8 }, { 4, 4 } };
    SwapField aDesc94[3] = { { 64, 4 }, { 12, 4 }, { 4, 4 } };
    SwapField aDescB8[2] = { { 8, 4 }, { 8, 4 } };
    SkinDesc14Old old;
    SkinDesc14* pEntry;
    SkinMesh* pMesh;
    u8* pData;
    u8* p;
    void* pSrc;                 // port: fn_80076158 takes it as a u8** too (one stack slot in EA's code)
    void* pDst;
    s32 n;
    int i;

    if (pDesc->nVersion > 8 || (pDesc->nVersion == 8 && pDesc->n04 == 1)) {
        p = (u8*)pDesc->p14;
        for (i = 0; i < pDesc->n10; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc14, 10, 1);
            p += sizeof(SkinDesc14);
        }
    } else if (pDesc->nVersion == 8 && pDesc->n04 == 0) {
        p = (u8*)pDesc->p14;
        if (p != NULL) {
            for (i = 0; i < pDesc->n10; i++) {
                pSrc = pDst = p;
                fn_8001F08C(&pSrc, &pDst, aOld14, 9, 1);
                p += sizeof(SkinDesc14);
            }
            for (i = 0; i < pDesc->n10; i++) {
                pEntry = &pDesc->p14[i];
                memcpy(&old, pEntry, sizeof(SkinDesc14Old));
                pEntry->uId = old.uId;
                pEntry->u08 = old.u08;
                pEntry->u0C = old.u0C;
                pEntry->u10 = old.u14;
                pEntry->n14 = old.n10;
                pEntry->n16 = old.n18;
                pEntry->n18 = -1;
                pEntry->n1C = old.n1C;
                pEntry->a20[0] = old.a20[0];
                pEntry->a20[1] = old.a20[1];
                pEntry->a20[2] = old.a20[2];
                pEntry->a20[3] = old.a20[3];
            }
            pDesc->n04 = 1;
        }
    }

    p = (u8*)pDesc->p18;
    if (p != NULL) {
        for (i = 0; i < pDesc->n10; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc18, 3, 1);
            p += sizeof(SkinDesc18);
        }
    }

    p = (u8*)pDesc->p20;
    if (p != NULL && pDesc->n1C != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n1C * 4, 4);
    }

    p = (u8*)pDesc->p28;
    if (p != NULL && pDesc->n24 != 0) {
        for (i = 0; i < pDesc->n24; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc28, 5, 1);
            p += sizeof(SkinDesc28);
        }
        for (i = 0; i < pDesc->n24; i++) {
            if (pDesc->p28[i].p10 != NULL) {
                pDesc->p28[i].p10 = (u8*)pDesc + (uptr)pDesc->p28[i].p10;
            }
        }
    }

    p = (u8*)pDesc->p34;
    if (p != NULL && pDesc->n2C != 0) {
        for (i = 0; i < pDesc->n2C; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aMesh, 4, 1);
            p += sizeof(SkinMesh);
        }
    }

    // Each mesh's bit data: its layout depends on the mesh's flags.
    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL) {
            pDesc->p34[i].pBits = (SkinMeshBit*)((u8*)pDesc + (uptr)pDesc->p34[i].pBits);
        }
        pMesh = &pDesc->p34[i];
        pData = (u8*)pMesh->pBits;
        n = pMesh->n8;
        if (pData == NULL) {
            pMesh->pBits = NULL;
        } else if (pMesh->uFlags & 0x30) {
            pSrc = pDst = pData;
            fn_80076158((u8**)&pSrc, pData, n * 8, 2);
            pData += n * 8;
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 4, 1);
            pData += n * 4;
            if (pMesh->uFlags & 0x1000) {
                pSrc = pData;
                fn_80076158((u8**)&pSrc, pData, n * 2, 2);
            } else if (pMesh->uFlags & 0x40) {
                pSrc = pData;
                fn_80076158((u8**)&pSrc, pData, n * 4, 2);
            }
        } else if (pMesh->uFlags & 0x40) {
            pSrc = pDst = pData;
            fn_80076158((u8**)&pSrc, pData, n * 4, 2);
        } else if (pMesh->uFlags & 0x1808) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 2, 2);
        } else if (pMesh->uFlags & 0x300004) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 64, 4);
        } else if (pMesh->uFlags & 2) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 2, 2);
        }
    }

    p = (u8*)pDesc->p3C;
    if (p != NULL && pDesc->n38 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n38 * 4, 4);
    }

    p = (u8*)pDesc->p44;
    for (i = 0; i < pDesc->n40; i++) {
        pSrc = pDst = p;
        fn_8001F08C(&pSrc, &pDst, aDesc44, 12, 1);
        p += sizeof(SkinDesc44);
    }

    p = (u8*)pDesc->pParts;
    if (p != NULL && pDesc->nParts != 0) {
        for (i = 0; i < pDesc->nParts; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aPart, 3, 1);
            p += sizeof(SkinPartDef);
        }
    }

    p = (u8*)pDesc->pVariants;
    if (p != NULL && pDesc->nVariants != 0) {
        for (i = 0; i < pDesc->nVariants; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aVariant, 5, 1);
            p += sizeof(SkinVariant);
        }
    }

    p = (u8*)pDesc->p5C;
    if (p != NULL) {
        if (pDesc->n58 != 0 && p != NULL && pDesc->n58 != 0) {
            pSrc = pDst = p;
            fn_80076158((u8**)&pSrc, p, pDesc->n58 * 8, 4);
        }
    }

    p = (u8*)pDesc->pLinks;
    if (p != NULL && pDesc->nLinks != 0) {
        for (i = 0; i < pDesc->nLinks; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aLink, 2, 1);
            p += sizeof(SkinLink);
        }
    }

    p = (u8*)pDesc->p6C;
    if (p != NULL && pDesc->n68 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n68 * 4, 4);
    }

    p = (u8*)pDesc->p74;
    if (p != NULL && pDesc->n70 != 0) {
        for (i = 0; i < pDesc->n70; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc74, 5, 1);
            p += sizeof(SkinDesc74);
        }
    }

    p = (u8*)pDesc->p7C;
    if (p != NULL && pDesc->n78 != 0) {
        for (i = 0; i < pDesc->n78; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc7C, 4, 1);
            p += sizeof(SkinDesc7C);
        }
    }

    p = (u8*)pDesc->p8C;
    if (p != NULL && pDesc->n88 != 0) {
        for (i = 0; i < pDesc->n88; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc8C, 5, 1);
            p += sizeof(SkinDesc8C);
        }
    }

    p = pDesc->p94;
    if (p != NULL && pDesc->n90 != 0) {
        for (i = 0; i < pDesc->n90; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc94, 3, 1);
            p += 0x50;
        }
    }

    p = (u8*)pDesc->pA4;
    if (p != NULL && pDesc->nA0 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->nA0 * 2, 2);
    }

    p = (u8*)pDesc->pAC;
    if (p != NULL && pDesc->nA8 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->nA8 * 2, 2);
    }

    p = (u8*)pDesc->pB8;
    if (p != NULL && pDesc->nB4 != 0) {
        for (i = 0; i < pDesc->nB4; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDescB8, 2, 1);
            p += sizeof(SkinDescB8);
        }
    }
}

// Frees what a loaded skin allocated (bit 2 of u10D4); 0 when it was not loaded.
s32 fn_80037708(Skin* pSkin) {
    SkinModel* pModel;

    if (!(pSkin->u10D4 & 2)) {
        return 0;
    }
    fn_80008380();
    pModel = pSkin->pModel;
    if (pModel != NULL && pModel->pDesc != NULL) {
        fn_80112910(pSkin->p1090);
        pSkin->p1090 = NULL;
        fn_80112910(pSkin->a1098[0]);
        pSkin->a1098[0] = NULL;
        fn_80112A58(pSkin->a10A0[0]);
        pSkin->a10A0[0] = NULL;
        fn_80037D5C(pSkin->pModel->pDesc);
    }
    ((void (*)(Skin*))fn_800CE168)(pSkin);  // port: EA passes an argument fn_800CE168 ignores
    if (pSkin->p108C != NULL) {
        fn_80009E70(pSkin->p108C);
    }
    pSkin->p108C = NULL;
    if (pSkin->p10CC != NULL) {
        fn_80009E70(pSkin->p10CC);
    }
    pSkin->p10CC = NULL;
    if (pSkin->p10D0 != NULL) {
        fn_80009E70(pSkin->p10D0);
    }
    pSkin->p10D0 = NULL;
    pSkin->u10D4 = pSkin->u10D4 & ~2;
    return 1;
}

// ---- end of sweep code ----

// Byte-swaps the model's bone poses.
void fn_80037574(BonePose* pBones, s32 nBones) {
    u8* pSrc;

    pSrc = (u8*)pBones;
    fn_80076158(&pSrc, (u8*)pBones, nBones * sizeof(BonePose), 4);
}

// Allocates what a skin needs once loaded (bit 2 of u10D4; 0 if it already was): the matrices,
// the two bit arrays and, with a description, its morph memory. b: also calls fn_800CE164.
s32 fn_800375AC(Skin* pSkin, u8 b) {
    SkinModel* pModel;
    SkinDesc* pDesc;
    s32 nSize;

    if (pSkin->u10D4 & 2) {
        return 0;
    }
    pModel = pSkin->pModel;
    pDesc = pModel->pDesc;
    pSkin->p108C = fn_80009B34(pModel->n50 * sizeof(*pSkin->p108C), 2, 0x80, "Skin.c", 0x500);
    pSkin->p10CC = fn_80009B34((pModel->n50 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50C);
    pSkin->p10D0 = fn_80009B34((pModel->n40 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50D);
    fn_8001E938(pSkin->p10CC, pModel->n50);
    fn_8001E938(pSkin->p10D0, pModel->n40);
    if (pDesc != NULL) {
        fn_800CD700(pSkin);
        nSize = fn_8011CDE8(pSkin);
        if (nSize != 0) {
            pSkin->a1098[0] = fn_801128C8(pModel->pDesc, nSize);
        }
        pSkin->a10A0[0] = fn_80112A10(pModel->pDesc, 0);
        pSkin->u10D4 = 1;
        if (b) {
            ((void (*)(Skin*))fn_800CE164)(pSkin);  // port: EA passes an argument fn_800CE164 ignores
        }
    }
    fn_8011CE58(pSkin);
    pSkin->u10D4 = pSkin->u10D4 | 2;
    return 1;
}

// Makes a skin from its file: copies the model (and its description) into memory of its own,
// byte-swapping the file first if that has not been done, and starts the pose at the model's.
// b picks the allocation mode (1 instead of 2).
Skin* fn_800377FC(u8* pData, u8 b) {
    SkinModel* pModel;
    Skin* pSkin;
    SkinDesc* pDesc;
    BonePose* pBones;
    s32 nDescSize;
    s32 nSize;
    u8 bSwap;
    int i;
    u8 bOld;

    bOld = fn_800CEE90();
    fn_800CEE88(1);
    pSkin = fn_80009B34(sizeof(Skin), 2, 0x80, "Skin.c", 0x5C4);
    memset(pSkin, 0, sizeof(Skin));
    pModel = (SkinModel*)pData;     // the file's model, then the copy
    if ((((SkinModel*)pData)->u30 & 0x40000002) != 0x40000002) {
        fn_8003682C((SkinModel*)pData);
        bSwap = 1;
        ((SkinModel*)pData)->u30 = ((SkinModel*)pData)->u30 | 0x40000000 | 2;
    } else {
        bSwap = 0;
    }
    if (pModel->n00 == 4) {
        pDesc = pModel->pDesc;
        if (pDesc != NULL) {
            pDesc = (SkinDesc*)((u8*)pModel + (uptr)pDesc);
        }
        if (bSwap) {
            fn_80036894(pDesc);
        }
        nDescSize = (pDesc != NULL) ? pDesc->n08 : 0;
        nSize = pModel->n08 - nDescSize;
    } else {
        nSize = sizeof(SkinModel);
        nDescSize = 0;
    }
    if (b) {
        pSkin->pModel = fn_80009B34(nSize, 1, 0x80, "Skin.c", 0x5EF);
    } else {
        pSkin->pModel = fn_80009B34(nSize, 2, 0x80, "Skin.c", 0x5F3);
    }
    memcpy(pSkin->pModel, pModel, nSize);
    pModel = pSkin->pModel;
    fn_800364AC(pModel);
    pModel->n08 = nSize;
    if (nDescSize != 0) {
        if (b) {
            pModel->pDesc = fn_80009B34(nDescSize, 1, 0x80, "Skin.c", 0x604);
        } else {
            pModel->pDesc = fn_80009B34(nDescSize, 2, 0x80, "Skin.c", 0x608);
        }
        memcpy(pModel->pDesc, pDesc, nDescSize);
        fn_800CEF04(pModel->pDesc);
        fn_800368FC(pModel->pDesc);
    } else {
        pModel->pDesc = NULL;
    }
    pDesc = pModel->pDesc;
    pSkin->b1044 = 0;
    pBones = pModel->p34;
    fn_80037574(pBones, pModel->n14);
    fn_8001E8A4(pSkin->pose.a0, 0x80);
    fn_8001E8A4(pSkin->pose.a10, 0x80);
    fn_8001E8A4(pSkin->pose.a20, 0x80);
    fn_8001E8A4(pSkin->pose.a30, 0x80);
    for (i = 0; i < pModel->n14; i++) {
        fn_8001E85C(pBones[i].q0, pSkin->pose.aBones[i].q0);
        fn_8001E85C(pBones[i].v10, pSkin->pose.aBones[i].v10);
    }
    pSkin->p1088 = fn_80009B34(pModel->n14 * sizeof(*pSkin->p1088), 2, 0x80, "Skin.c", 0x62A);
    if (pDesc != NULL) {
        fn_801127A0(pDesc);
    }
    fn_800CD404(pSkin);
    fn_8011C9B0(pSkin);
    fn_800CEE88(bOld);
    fn_800375AC(pSkin, 0);
    return pSkin;
}

// Hands the skin the morph weights a format 1 pose buffer changed (bits 5..19 of its first block),
// then clears the block's bits.
void fn_80037C48(Skin* pSkin, SkelPose* pPose) {
    SkelPoseBlock* pBlock;
    int i;

    if (pSkin != NULL) {
        pBlock = &((SkelPose1*)pPose)->aBlocks[0];
        for (i = 5; i < 20; i++) {
            if (fn_8001E9CC(pBlock->aBits, i)) {
                fn_8011CADC(pSkin, i - 5, pBlock->af8[i]);
            }
        }
        fn_8001E938(pBlock->aBits, 20);
    }
}

// ---- sweep code (tidied) ----

void fn_80037CD8(Skin* pSkin) {
    SkinModel* pModel;

    fn_80037708(pSkin);
    fn_8011CD84(pSkin);
    fn_800CD56C(pSkin);
    pModel = pSkin->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            fn_801127C4(pModel->pDesc);
            fn_80009E70(pSkin->pModel->pDesc);
        }
        fn_80009E70(pSkin->pModel);
    }
    if (pSkin->p1088 != NULL) {
        fn_80009E70(pSkin->p1088);
    }
    fn_80009E70(pSkin);
}

// ---- end of sweep code ----

// Frees the mesh bit data a description allocated for itself (flag 0x400000).
void fn_80037D5C(SkinDesc* pDesc) {
    int i;

    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL && (pDesc->p34[i].uFlags & 0x400000)) {
            fn_80009E70(pDesc->p34[i].pBits);
        }
    }
}
