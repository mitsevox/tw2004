// GoShaderObject_Rain_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the
// rain shader object: falling streaks drawn around the camera and splashes on the ground.

#include "game_types.h"
#include "engine.h"
#include "rain.h"
#include "gx.h"
#include "camera.h"
#include "terrain.h"
#include "ball.h"
#include "unsorted/cull.h"
#include "core/startup.h"

void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);      // UMemPool.c: copy a 4x4 matrix
void fn_80070168(void);                                 // calls a display list (see fn_800B4FA4)
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_800124A8(void);                                // LLFont.c: end the primitive
void fn_800B58B4(s32 p0);
void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2);
void fn_800B58D4(f32 farg0, f32 farg1);
void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800B58FC(f32* pA, f32* pOut);

void fn_800B4B5C(void);
void fn_800B4BB0(void);
void fn_800B4BB4(void);
void fn_800B4BB8(void);
void fn_800B4BD8(void);
void fn_800B4BFC(void);
void fn_800B4C00(RainList* pList, int nDrops);
void SD_vShaderObject_Rain_Dynamic_Init(RainObject* pRain, f32* pStrength);
void fn_800B4F24(RainObject* pRain);
void fn_800B4FA4(RainObject* pRain);
void fn_800B52D4(RainObject* pRain, f32* pTime);

void fn_800B4B5C(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 0;
    lbl_802814B8->f8 = fn_80029B64(3075.0f);
    lbl_802814B8->pBank = NULL;
    lbl_802814B8->pTex = NULL;
}

void fn_800B4BB0(void) {
}

void fn_800B4BB4(void) {
}

void fn_800B4BB8(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 1 - lbl_802814B8->n4;
}

void fn_800B4BD8(void) {
    lbl_802814B8->n0 = (lbl_802814B8->n0 + 1) % 2;
}

void fn_800B4BFC(void) {
}

// Build the display list of nDrops raindrops: each a line from a random point in a 35 x 25 x 35
// box to 0.2 to 0.3 below it, coloured from palette entries 0 and 1.
void fn_800B4C00(RainList* pList, int nDrops) {
    u32 uSize;
    void* pBuf;
    f32 fX;
    f32 fY;
    f32 fZ;
    int i;

    uSize = ((nDrops * 32 + 31) & ~31) + 0x400;
    pBuf = fn_80009B34(uSize, 1, 0x20, "GoShaderObject_Rain_Gc.c", 218);
    DCInvalidateRange(pBuf, uSize);
    GXBeginDisplayList(pBuf, uSize);
    GXResetWriteGatherPipe();
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);         // position, direct
    GXSetVtxDesc(11, 2);        // colour 0, an 8-bit index
    GXInvalidateVtxCache();
    fn_80012520(0xA8, 4, nDrops * 2);   // lines
    for (i = 0; i < nDrops; i++) {
        fX = 35.0f * Misc_RandFuncf(1) - 17.5f;
        fY = 25.0f * Misc_RandFuncf(1) - 12.5f;
        fZ = 35.0f * Misc_RandFuncf(1) - 17.5f;
        fn_800B58C0(fX, fY, fZ);
        fn_800B58B4(0);
        fn_800B58C0(fX, (fY - 0.2f) - 0.1f * Misc_RandFuncf(1), fZ);
        fn_800B58B4(1);
    }
    fn_800124A8();
    uSize = GXEndDisplayList();
    pList->pList = fn_80009B34(uSize, 2, 0x20, "GoShaderObject_Rain_Gc.c", 251);
    Mem_cpy(pList->pList, pBuf, uSize);
    DCFlushRange(pBuf, uSize);
    fn_80009E70(pBuf);
    pList->uSize = uSize;
}

// Set up a rain object: the drops' display list and its cleared buffers, and find the "splash"
// texture. pStrength is unused (PsMgr.c, the only caller, passes it).
void SD_vShaderObject_Rain_Dynamic_Init(RainObject* pRain, f32* pStrength) {
    RainData* pData = &pRain->data;
    u64 uSplash;
    int i;

    fn_800B4C00(&pData->list, 900);
    for (i = 0; i < 4; i++) {
        pData->apA[i] = fn_80009B34(RAIN_BUF_A_SIZE, 2, 0x20, "GoShaderObject_Rain_Gc.c", 280);
        memset(pData->apA[i], 0, RAIN_BUF_A_SIZE);
    }
    for (i = 0; i < 2; i++) {
        pData->apB[i] = fn_80009B34(RAIN_BUF_B_SIZE, 2, 0x20, "GoShaderObject_Rain_Gc.c", 292);
        memset(pData->apB[i], 0, RAIN_BUF_B_SIZE);
    }
    uSplash = fn_8000BEE4("splash");
    fn_800102DC(uSplash, &lbl_802814B8->pBank, &lbl_802814B8->pTex);
}

// Free a rain object's list and buffers.
void fn_800B4F24(RainObject* pRain) {
    RainData* pData = &pRain->data;
    int i;

    fn_80009E70(pData->list.pList);
    for (i = 0; i < 4; i++) {
        fn_80009E70(pData->apA[i]);
    }
    for (i = 0; i < 2; i++) {
        fn_80009E70(pData->apB[i]);
    }
}

// Draw a rain object: the drops' display list once at each lit point of the current buffer, then
// the splash triangles, textured with "splash".
void fn_800B4FA4(RainObject* pRain) {
    int i;
    Vec4 v;
    f32 mPos[4][4];
    f32 mView[4][4];
    Camera* pCamera;
    int nBuf;
    RainData* pData;
    int j;
    int k;
    int nHalf;
    RainSplash* pSplash;
    int nAlpha;
    RainPoint* pPoint;

    pCamera = fn_8001614C();
    pData = &pRain->data;
    nBuf = lbl_802814B8->n0;
    nHalf = lbl_802814B8->n4;
    fn_80035118(4, 5);
    fn_80014118(0x40);
    fn_80012F50(0, 7, 0);
    fn_80012F18(3);
    fn_8005CC64(NULL, NULL);
    fn_80012EF8();
    fn_8000A0E8(pCamera->viewMtx, mView);
    mView[3][0] = mView[3][1] = mView[3][2] = 0.0f;
    fn_8000A0E8(((Camera*)fn_8001614C())->m15C, mPos);
    fn_800B58FC(mPos[0], mPos[0]);
    fn_800B58FC(mPos[2], mPos[2]);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 3; k++) {
                pPoint = &pData->apA[nBuf * 2 + nHalf][i * 6 + j * 3 + k];
                if (pPoint->aColor[3] != 0 || pPoint->a10[3] != 0) {
                    GXSetArray(11, pPoint->aColor, 4);      // colour 0
                    v.x = pPoint->vPos[0];
                    v.y = pPoint->vPos[1];
                    v.z = pPoint->vPos[2];
                    v.w = 1.0f;
                    fn_800BAD60(mView, &v, &v);
                    mPos[0][3] = -v.x;
                    mPos[1][3] = v.y;
                    mPos[2][3] = -v.z;
                    GXLoadPosMtxImm(mPos, 0);
                    // port: EA passes the list and its size, which fn_80070168 hands on unread
                    ((void (*)(void*, u32))fn_80070168)(pData->list.pList, pData->list.uSize);
                }
            }
        }
    }

    fn_80014118(0x50);
    fn_8005CC64(lbl_802814B8->pBank, lbl_802814B8->pTex);
    fn_80012EF8();
    fn_8000A0E8(((Camera*)fn_8001614C())->m15C, mPos);
    fn_800B58FC(mPos[0], mPos[0]);
    fn_800B58FC(mPos[2], mPos[2]);
    GXLoadPosMtxImm(mPos, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);         // position, colour, texture coordinates: direct
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);
    GXInvalidateVtxCache();
    fn_80012520(0x90, 4, RAIN_NUM_SPLASHES * 3);    // triangles
    for (i = 0; i < RAIN_NUM_SPLASHES; i++) {
        nAlpha = 128.0f * pData->apB[nBuf][i].fAlpha;
        pSplash = &pData->apB[nBuf][i];
        fn_800B58C0(pSplash->av[0][0], pSplash->av[0][1], pSplash->av[0][2]);
        fn_800B58E4(0x80, 0x80, 0x80, nAlpha & 0xFF);
        fn_800B58D4(0.0f, 0.0f);
        fn_800B58C0(pSplash->av[1][0], pSplash->av[1][1], pSplash->av[1][2]);
        fn_800B58E4(0x80, 0x80, 0x80, nAlpha & 0xFF);
        fn_800B58D4(0.0f, 1.0f);
        fn_800B58C0(pSplash->av[2][0], pSplash->av[2][1], pSplash->av[2][2]);
        fn_800B58E4(0x80, 0x80, 0x80, nAlpha & 0xFF);
        fn_800B58D4(1.0f, 0.0f);
    }
    fn_800124A8();
}

// Move the rain with the camera: the points sit on a 35 x 25 x 35 grid around it (hidden when
// culled), the drops fall, and each splash grows until it restarts at a random spot on the ground
// near the lens, tilted to the ground's slope.
void fn_800B52D4(RainObject* pRain, f32* pTime) {
    Vec4 vCenter;
    Vec4 vPos;                  // also the ground's normal at a splash
    Sphere sphere;
    f32 mView[4][4];
    Camera* pCamera;
    int i;
    CamLens* pLens;
    RainSplash* pSplash;
    int nHalf;
    int j;
    RainData* pData;
    int nBuf;
    RainPoint* pPoint;
    int k;
    f32 fOff;
    f32 fY;
    f32 fX;
    f32 fFade;
    f32 fSize;

    pCamera = fn_8001614C();
    pLens = fn_8001F004();
    pData = &pRain->data;
    nBuf = lbl_802814B8->n0;
    nHalf = lbl_802814B8->n4;
    sphere.radius = lbl_802814B8->f8;
    vCenter.x = -pCamera->unk10->m4[3][0];
    vCenter.y = pCamera->unk10->m4[3][1];
    vCenter.z = -pCamera->unk10->m4[3][2];
    vCenter.w = 1.0f;
    fOff = (vCenter.x >= 0.0f) ? 17.5f : -17.5f;
    vCenter.x = vCenter.x - 35.0f * (int)(vCenter.x / 35.0f) - fOff;
    vCenter.y = 0.0f;
    fOff = (vCenter.z >= 0.0f) ? 17.5f : -17.5f;
    vCenter.z = vCenter.z - 35.0f * (int)(vCenter.z / 35.0f) - fOff;
    fn_8000A0E8(pCamera->viewMtx, mView);
    mView[3][0] = mView[3][1] = mView[3][2] = 0.0f;
    for (i = 0; i < 3; i++) {
        fX = 35.0f * (i - 1);
        for (j = 0; j < 2; j++) {
            fY = 25.0f * j;
            for (k = 0; k < 3; k++) {
                pPoint = &pData->apA[nBuf * 2 + nHalf][i * 6 + j * 3 + k];
                pPoint->f14 = 40.0f * *pTime + pPoint->f14;
                if (pPoint->f14 >= 25.0f) {
                    pPoint->f14 -= 25.0f;
                }
                pPoint->vPos[0] = vCenter.x + fX;
                pPoint->vPos[1] = (vCenter.y + fY) - pPoint->f14;
                pPoint->vPos[2] = 35.0f * (k - 1) + vCenter.z;
                vPos.x = pPoint->vPos[0];
                vPos.y = pPoint->vPos[1];
                vPos.z = pPoint->vPos[2];
                vPos.w = 1.0f;
                fn_800BAD60(mView, &vPos, &vPos);
                sphere.x = vPos.x;
                sphere.y = vPos.y;
                sphere.z = vPos.z;
                if (fn_80007D74(&sphere, pCamera, 0) == 2) {
                    pPoint->aColor[3] = 0;
                    pPoint->a10[3] = 0;
                } else {
                    pPoint->aColor[0] = 0xAF;
                    pPoint->aColor[1] = 0xAF;
                    pPoint->aColor[2] = 0xD7;
                    pPoint->aColor[3] = 5;
                    pPoint->a10[0] = 0x5F;
                    pPoint->a10[1] = 0x63;
                    pPoint->a10[2] = 0x7D;
                    if (i != 1 || k != 1) {
                        fFade = 2.0f * (sphere.z - 28.0f);
                        pPoint->a10[3] = 64.0f - ((fFade < 0.0f) ? 0.0f : ((fFade > 59.0f) ? 59.0f : fFade));
                    } else {
                        pPoint->a10[3] = 0x37;  // the middle column
                    }
                }
            }
        }
    }

    for (i = 0; i < RAIN_NUM_SPLASHES; i++) {
        pSplash = &pData->apB[nBuf][i];
        if (pSplash->fAlpha >= 0.05f && pSplash->fAlpha < 0.7f) {
            pSplash->fAlpha = 5.0f * *pTime + pSplash->fAlpha;
            continue;
        }
        if (pSplash->fAlpha <= 0.0f) {
            // fake match: through the buffer again, not pSplash (the original reloads apB here)
            pData->apB[nBuf][i].fAlpha = 0.9f * (0.65f * Misc_RandFuncf(1)) + 0.05f;
        } else {
            pSplash->fAlpha = 0.05f;
        }
        fSize = 0.05f * Misc_RandFuncf(1) + 0.05f;
        vCenter.x = (20.0f * Misc_RandFuncf(1) + pLens->m4[3][0]) - 10.0f;
        vCenter.y = pLens->m4[3][1];
        vCenter.z = (20.0f * Misc_RandFuncf(1) + pLens->m4[3][2]) - 10.0f;
        vCenter.y = 0.1f + Ter_GetHighestGroundHeight(fn_8000C594(), &vCenter.x);
        Ter_GetSupportingGroundNormal(fn_8000C594(), &vCenter.x, &vPos.x);
        fn_800B5918(&vCenter.x, pSplash->av[0]);
        pSplash->av[1][0] = pSplash->av[0][0] + fSize;
        pSplash->av[1][2] = pSplash->av[0][2];
        pSplash->av[2][0] = pSplash->av[0][0];
        pSplash->av[2][2] = pSplash->av[0][2] + fSize;
        if (0.0f != vPos.y) {
            // the corners' heights on the ground's plane through the first
            pSplash->av[1][1] = (-vPos.x * (pSplash->av[1][0] - pSplash->av[0][0]) -
                                 vPos.z * (pSplash->av[1][2] - pSplash->av[0][2]) +
                                 vPos.y * pSplash->av[0][1]) / vPos.y;
            pSplash->av[2][1] = (-vPos.x * (pSplash->av[2][0] - pSplash->av[0][0]) -
                                 vPos.z * (pSplash->av[2][2] - pSplash->av[0][2]) +
                                 vPos.y * pSplash->av[0][1]) / vPos.y;
        } else {
            pSplash->av[1][1] = pSplash->av[2][1] = 0.0f;
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_800B58B4(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}

void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_800B58D4(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

// ---- end of sweep code ----

// -a into out (four floats)
#ifdef __MWERKS__
asm void fn_800B58FC(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_800B58FC(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
    pOut[3] = -pA[3];
}
#endif

// Copy three floats. Like the GX writes above, it sits at the end of this file's code and every
// other caller (GoGolfCam.c, GoShaderObject_Grass_Gc.c) comes later in the link order.
void fn_800B5918(const f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}
