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

GrassWord* GrassPacket_pGetNextAvailableVertSlot(void) {
    return SD_gpGrassTypeData->pCur;
}

// A new row of vertices starts at the next free vertex.
void GrassPacket_vSetNewRow(void) {
    GrassWord* pCur = SD_gpGrassTypeData->pCur;
    SD_gpGrassTypeData->pRow = pCur;
    SD_gpGrassTypeData->p35C = pCur;
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
