#include "grassshader.h"
#include "game_types.h"

void SD_vShaderObject_Grass_Type_Init(void);
void SD_vShaderObject_Grass_Type_Close(void);
void SD_vShaderObject_Grass_Type_SetParameters(void* pParams);
void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts);
GrassWord* GrassPacket_pGetNextAvailableVertSlot(void);
void GrassPacket_vSetNewRow(void);
void GrassPacket_vSetBuffer(GrassWord* pBuffer, int nVerts);
void SD_vShaderObject_Grass_Static_Close(SD_SShaderObject_Static* pObject);
void fn_801213F0(void);
void fn_801213F4(f32 farg0, f32 farg1);
void fn_80121404(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_8012141C(f32 farg0, f32 farg1, f32 farg2);

void SD_vShaderObject_Grass_Type_Init(void) {
    SD_gpGrassTypeData->pPool = fn_8000AFA0(32, 32, 2, 16);
}

void SD_vShaderObject_Grass_Type_Close(void) {
    fn_8000B058(SD_gpGrassTypeData->pPool);
}

void SD_vShaderObject_Grass_Type_SetParameters(void* pParams) {
    SD_gpGrassTypeData->pParams = pParams;
}

void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts) {
    Mem_cpy(SD_gpGrassTypeData->pCur, pVerts, nVerts * 16);
    SD_gpGrassTypeData->pCur += nVerts * 4;
}

GrassWord* GrassPacket_pGetNextAvailableVertSlot(void) {
    return SD_gpGrassTypeData->pCur;
}

void GrassPacket_vSetNewRow(void) {
    GrassWord* pCur = SD_gpGrassTypeData->pCur;
    SD_gpGrassTypeData->pRow = pCur;
    SD_gpGrassTypeData->p35C = pCur;
}

void GrassPacket_vSetBuffer(GrassWord* pBuffer, int nVerts) {
    SD_gpGrassTypeData->pCur = pBuffer;
    SD_gpGrassTypeData->pBase = pBuffer;
    SD_gpGrassTypeData->pEnd = SD_gpGrassTypeData->pBase + nVerts * 4;
}

void SD_vShaderObject_Grass_Static_Close(SD_SShaderObject_Static* pObject) {
    fn_8000B0D4(SD_gpGrassTypeData->pPool, pObject->pData);
}

void fn_801213F0(void) {
}

void fn_801213F4(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_80121404(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_8012141C(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}
