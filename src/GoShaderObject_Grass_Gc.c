// GoShaderObject_Grass_Gc.c (our name, after TW06's goshaderobject_grass_xbox.c and its _Gc
// siblings): the grass shader object, one row of the shader object table. Builds the shells of
// grass over the hole's terrain into a vertex buffer (the GrassPacket calls) and draws them through
// GX. GoGrass.c hands it the grass parameters once per hole (SD_vSetGrassParamsOnce).
// Partly decompiled: the builder (SD_vShaderObject_Grass_Static_Init) is still asm.

#include "grassshader.h"
#include "camera.h"
#include "gx.h"

void SD_vShaderObject_Grass_Type_Init(void);
void SD_vShaderObject_Grass_Type_Close(void);
void SD_vShaderObject_Grass_Type_SetParameters(void* pParams);
void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts);
s32 GrassPacket_iEndPacket(void);
void fn_80120AB4(f32* pA, f32* pB, f32* pOut, int bAlongZ, f32 fAt);
void GrassPacket_vFlushRow(void);
void GrassPacket_vAddVert(f32* pPos, int nInRow);
void fn_80120C2C(f32 (*aPoints)[3], u8* aFlags, f32* pOut, u16 nIndex, u8 nStep, u8* pFlag, int bAlongX,
                 f32 fAt);
void GrassPacket_vBeginPacket(GrassWord** ppStart);
void SD_vSetGrassParamsOnce(f32* pUnused0, f32* pUnused1, f32 (*a2)[4], f32* p8, f32 (*b2)[4],
                            f32 (*a16)[4], f32 fA, f32 fB);
void SD_vShaderObject_Grass_Static_Render(SD_SShaderObject_Static* pObject);
GrassWord* GrassPacket_pGetNextAvailableVertSlot(void);
void GrassPacket_vSetNewRow(void);
void GrassPacket_vSetBuffer(GrassWord* pBuffer, int nVerts);
void SD_vShaderObject_Grass_Static_Close(SD_SShaderObject_Static* pObject);
void fn_801213F0(void);
void fn_801213F4(f32 fS, f32 fT);
void fn_80121404(s32 nR, s32 nG, s32 nB, s32 nA);
void fn_8012141C(f32 fX, f32 fY, f32 fZ);

// The grass type starts: a pool of 32 render records of 32 bytes.
void SD_vShaderObject_Grass_Type_Init(void) {
    SD_gpGrassTypeData->pPool = fn_8000AFA0(32, 32, 2, 16);
}

void SD_vShaderObject_Grass_Type_Close(void) {
    fn_8000B058(SD_gpGrassTypeData->pPool);
}

void SD_vShaderObject_Grass_Type_SetParameters(void* pParams) {
    SD_gpGrassTypeData->pParams = pParams;
}

// Copies nVerts ready-made vertices into the packet.
void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts) {
    Mem_cpy(SD_gpGrassTypeData->pCur, pVerts, nVerts * 16);
    SD_gpGrassTypeData->pCur += nVerts * 4;
}

// The packet ends: its vertex count.
s32 GrassPacket_iEndPacket(void) {
    return (SD_gpGrassTypeData->pCur - SD_gpGrassTypeData->pBase) / 4;
}

// The point on the segment pA-pB at fAt along x (bAlongZ clear) or along z: y is interpolated, the
// other axis taken from pA. A segment with no length there gives pA itself.
void fn_80120AB4(f32* pA, f32* pB, f32* pOut, int bAlongZ, f32 fAt) {
    f32 fFrom;
    f32 fTo;
    if (!bAlongZ) {
        fFrom = pA[0];
        fTo = pB[0];
        if (fFrom == fTo) {
            fn_800B5918(pA, pOut);
            return;
        }
        pOut[0] = fAt;
        pOut[1] = (fAt - fFrom) / (fTo - fFrom) * (pB[1] - pA[1]) + pA[1];
        pOut[2] = pA[2];
    } else {
        fFrom = pA[2];
        fTo = pB[2];
        if (fFrom == fTo) {
            fn_800B5918(pA, pOut);
            return;
        }
        pOut[0] = pA[0];
        pOut[1] = (fAt - fFrom) / (fTo - fFrom) * (pB[1] - pA[1]) + pA[1];
        pOut[2] = fAt;
    }
}

// The current row ends: its first vertex records how many vertices it has.
void GrassPacket_vFlushRow(void) {
    GrassWord* pCur = SD_gpGrassTypeData->pCur;
    if (pCur != SD_gpGrassTypeData->pBase) {
        GrassWord* pRow = SD_gpGrassTypeData->pRow;
        pRow[3].b[2] = (pCur - pRow) / 4;
    }
}

// One vertex at pPos; nInRow 0 starts a new row (closing the one before).
void GrassPacket_vAddVert(f32* pPos, int nInRow) {
    GrassWord* pVert = SD_gpGrassTypeData->pCur;
    if (nInRow == 0) {
        GrassWord* pRow = SD_gpGrassTypeData->pRow;
        pRow[3].b[2] = (pVert - pRow) / 4;
        SD_gpGrassTypeData->pRow = SD_gpGrassTypeData->pCur;
        pVert[3].b[2] = 0;
    }
    pVert[0].f = pPos[0];
    pVert[1].f = pPos[1];
    pVert[2].f = pPos[2];
    pVert[3].b[0] = SD_gpGrassTypeData->n374;
    pVert[3].b[1] = 1;
    pVert[3].b[3] = (nInRow == 0);
    SD_gpGrassTypeData->pCur += 4;
}

// The point at fAt between points nIndex and nIndex + nStep + 1 of aPoints, along z (bAlongX clear)
// or along x, and that point's flag: bit 2 for a step of 0, bit 4 otherwise.
void fn_80120C2C(f32 (*aPoints)[3], u8* aFlags, f32* pOut, u16 nIndex, u8 nStep, u8* pFlag, int bAlongX,
                 f32 fAt) {
    u16 nOther = nIndex + nStep + 1;
    f32 fT;
    if (!bAlongX) {
        f32 fLen = aPoints[nOther][2] - aPoints[nIndex][2];
        if (0.0f == fLen) {
            fT = 0.0f;
        } else {
            fT = (fAt - aPoints[nIndex][2]) / fLen;
        }
        pOut[0] = fT * (aPoints[nOther][0] - aPoints[nIndex][0]) + aPoints[nIndex][0];
        pOut[1] = fT * (aPoints[nOther][1] - aPoints[nIndex][1]) + aPoints[nIndex][1];
        pOut[2] = fAt;
    } else {
        f32 fLen;
        fT = 0.0f;
        fLen = aPoints[nOther][0] - aPoints[nIndex][0];
        if (fT == fLen) {
        } else {
            fT = (fAt - aPoints[nIndex][0]) / fLen;
        }
        pOut[0] = fAt;
        pOut[2] = fT * (aPoints[nOther][2] - aPoints[nIndex][2]) + aPoints[nIndex][2];
        pOut[1] = fT * (aPoints[nOther][1] - aPoints[nIndex][1]) + aPoints[nIndex][1];
    }
    if (nStep == 0) {
        *pFlag = aFlags[nIndex] & 2;
    } else {
        *pFlag = aFlags[nIndex] & 4;
    }
}

GrassWord* GrassPacket_pGetNextAvailableVertSlot(void) {
    return SD_gpGrassTypeData->pCur;
}

// A new row of vertices starts at the next free vertex.
void GrassPacket_vSetNewRow(void) {
    GrassWord* pCur = SD_gpGrassTypeData->pCur;
    SD_gpGrassTypeData->pRow = pCur;
    SD_gpGrassTypeData->p35C = pCur;
}

// A packet starts at the next 64-byte boundary; *ppStart gets its first vertex.
void GrassPacket_vBeginPacket(GrassWord** ppStart) {
    while ((u32)SD_gpGrassTypeData->pCur & 0x3F) {
        SD_gpGrassTypeData->pCur++;
    }
    *ppStart = SD_gpGrassTypeData->pCur;
    SD_gpGrassTypeData->pCur[3].b[2] = 0;
    SD_gpGrassTypeData->pBase = SD_gpGrassTypeData->pCur;
}

// The packets are built into pBuffer, which holds nVerts vertices.
void GrassPacket_vSetBuffer(GrassWord* pBuffer, int nVerts) {
    SD_gpGrassTypeData->pCur = pBuffer;
    SD_gpGrassTypeData->pBase = pBuffer;
    SD_gpGrassTypeData->pEnd = SD_gpGrassTypeData->pBase + nVerts * 4;
}

// A grass object goes: its render record back to the pool.
void SD_vShaderObject_Grass_Static_Close(SD_SShaderObject_Static* pObject) {
    fn_8000B0D4(SD_gpGrassTypeData->pPool, pObject->pData);
}

// GoGrass.c hands over the hole's grass parameters: copies of its vectors and eight floats, and
// fA/fB with 1/16 and 16 beside them.
void SD_vSetGrassParamsOnce(f32* pUnused0, f32* pUnused1, f32 (*a2)[4], f32* p8, f32 (*b2)[4],
                            f32 (*a16)[4], f32 fA, f32 fB) {
    // port: pUnused0 and pUnused1 are passed by the only caller and never read
    int i;
    for (i = 0; i < 2; i++) {
        Vec_Copy(a2[i], lbl_80260920[i]);
    }
    for (i = 0; i < 2; i++) {
        Vec_Copy(b2[i], lbl_80260900[i]);
    }
    for (i = 0; i < 8; i++) {
        lbl_802608E0[i] = p8[i];
    }
    lbl_802608D0[0] = fA;
    lbl_802608D0[1] = 1.0f / 16.0f;
    lbl_802608D0[2] = fB;
    lbl_802608D0[3] = 16.0f;
    for (i = 0; i < 16; i++) {
        Vec_Copy(a16[i], lbl_802607D0[i]);
    }
}

// Draws a grass object: every row of its current vertex run as a strip of blades, each vertex twice
// (at the ground, then raised and blown by one of the 16 wind offsets). The grass fades with the
// distance from the camera.
void SD_vShaderObject_Grass_Static_Render(SD_SShaderObject_Static* pObject) {
    GrassRenderData* pData = pObject->pData;
    GrassParams* pParams = SD_gpGrassTypeData->pParams;
    s32 nSet = pParams->n24;
    s32 nVerts = pData->anVerts[nSet][pParams->a18[nSet]];
    GrassWord* pVert = pData->apVerts[nSet][pParams->a18[nSet]];
    f32 vEye[3];
    f32 vAt[3];
    f32 fDist;
    f32 fPerX;
    f32 fPerZ;
    f32 fBase;
    f32 fInvScale;
    f32 fTexS;
    f32 fTexT;
    f32 fShade;
    f32 fBladeT;
    f32 fFade;
    GrassWord* pAxis;
    int nAxis;
    int nWind;
    int nDone;
    int nInRow;
    int nPass;
    s8 nRow;
    u8 nAlpha;

    if (nVerts <= 0) {
        return;
    }
    GXInvalidateVtxCache();
    GXSetVtxAttrFmt(3, 9, 1, 4, 0);
    GXSetVtxAttrFmt(3, 13, 1, 4, 9);
    GXSetVtxAttrFmt(3, 14, 1, 4, 9);
    GXSetVtxAttrFmt(3, 11, 1, 5, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxDesc(14, 1);

    // How fast the camera distance changes along x and along z, from 2.5-unit steps.
    Vec_Copy(fn_8001F004()->v34, vEye);
    vAt[0] = pParams->a04[0];
    vAt[1] = pVert[1].f;
    vAt[2] = pParams->a04[1];
    fDist = Vec_Distance(vEye, vAt);
    vAt[0] = 2.5f + pParams->a04[0];
    fPerX = Vec_Distance(vEye, vAt) - fDist;
    vAt[0] = pParams->a04[0];
    vAt[2] = 2.5f + pParams->a04[1];
    fPerZ = Vec_Distance(vEye, vAt) - fDist;
    fPerX /= 2.5f;
    fPerZ /= 2.5f;

    nAxis = (nSet == 0) ? 0 : 2;
    fBase = pParams->a04[nAxis];
    fInvScale = 1.0f / pParams->a04[2];
    nWind = 0;
    nDone = 0;
    while (nDone < nVerts) {
        nRow = pVert[3].b[2];
        if (nRow < 2) {
            // Too short to draw: skipped.
            for (nInRow = nRow; nInRow > 0; nInRow--) {
                pVert += 4;
                nDone++;
            }
            continue;
        }
        GXBegin(0x98, 3, nRow * 2);
        pAxis = pVert + nAxis;
        for (nInRow = 0; nInRow < nRow; nInRow++) {
            fFade = (fDist + (fPerX * (pVert[0].f - pParams->a04[0]) +
                              fPerZ * (pVert[2].f - pParams->a04[1]))) *
                        lbl_80260900[1][3] +
                    lbl_80260900[0][3];
            if (fFade < 0.0f) {
                fFade = 0.0f;
            }
            if (fFade > 1.0f) {
                fFade = 1.0f;
            }
            fTexS = pVert[0].f * lbl_80260920[1][0] + lbl_80260920[0][0];
            fTexT = pVert[2].f * lbl_80260920[1][1] + lbl_80260920[0][1];
            fShade = fInvScale * ((pAxis->f - fBase) + lbl_802608E0[(s8)pVert[3].b[0]]);
            nAlpha = 255.0f * pParams->a10[nSet] * fFade;
            for (nPass = 0; nPass < 2; nPass++) {
                if (nPass == 1) {
                    fn_8012141C(pVert[0].f + lbl_802607D0[nWind][0], pVert[1].f + pParams->f20,
                                pVert[2].f + lbl_802607D0[nWind][2]);
                    fBladeT = 0.025f;
                    nWind = (nWind + 1) & 15;
                } else {
                    fn_8012141C(pVert[0].f, pVert[1].f, pVert[2].f);
                    fBladeT = 1.0f;
                }
                fn_80121404(0x80, 0x80, 0x80, nAlpha);
                fn_801213F4(fShade, fBladeT);
                fn_801213F4(fTexS, fTexT);
            }
            pVert += 4;
            pAxis += 4;
            nDone++;
        }
        fn_801213F0();
    }
}

// GX's small inline calls, compiled out of line into this file: the end of a primitive (nothing to
// do), then writes to the graphics FIFO of a texture coordinate, a colour and a position.
void fn_801213F0(void) {
}

void fn_801213F4(f32 fS, f32 fT) {
    *(f32*)0xCC008000 = fS;
    *(f32*)0xCC008000 = fT;
}

void fn_80121404(s32 nR, s32 nG, s32 nB, s32 nA) {
    *(volatile u8*)0xCC008000 = nR;
    *(volatile u8*)0xCC008000 = nG;
    *(volatile u8*)0xCC008000 = nB;
    *(volatile u8*)0xCC008000 = nA;
}

void fn_8012141C(f32 fX, f32 fY, f32 fZ) {
    *(f32*)0xCC008000 = fX;
    *(f32*)0xCC008000 = fY;
    *(f32*)0xCC008000 = fZ;
}
