// GoShaderObject_Grass_Gc.c (our name, after TW06's goshaderobject_grass_xbox.c and its _Gc
// siblings): the grass shader object, one row of the shader object table. Builds the shells of
// grass over the hole's terrain into a vertex buffer (the GrassPacket calls) and draws them through
// GX. GoGrass.c hands it the grass parameters once per hole (SD_vSetGrassParamsOnce).
// Partly decompiled: the builder, the drawing and their two interpolation helpers are still asm.

#include "grassshader.h"

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
void fn_800B5918(f32* pSrc, f32* pDst);
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
