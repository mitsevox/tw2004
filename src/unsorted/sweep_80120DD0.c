// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

void GrassPacket_vSetBuffer(GrassWord* pBuffer, int nVerts);
void SD_vShaderObject_Grass_Static_Close(SD_SShaderObject_Static* pObject);

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
