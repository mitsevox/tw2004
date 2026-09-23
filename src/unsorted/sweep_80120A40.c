// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts);

// Copies nVerts ready-made vertices into the packet.
void GrassPacket_vAddVerts(GrassWord* pVerts, int nVerts) {
    Mem_cpy(SD_gpGrassTypeData->pCur, pVerts, nVerts * 16);
    SD_gpGrassTypeData->pCur += nVerts * 4;
}
