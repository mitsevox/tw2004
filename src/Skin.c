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
void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c

void  fn_80035F40(void* pCamera);
void  fn_8003612C(LightGroup* pGroup);
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
