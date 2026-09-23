// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

void GrassPacket_vSetNewRow(void);

// A new row of vertices starts at the next free vertex.
void GrassPacket_vSetNewRow(void) {
    GrassWord* pCur = SD_gpGrassTypeData->pCur;
    SD_gpGrassTypeData->pRow = pCur;
    SD_gpGrassTypeData->p35C = pCur;
}
