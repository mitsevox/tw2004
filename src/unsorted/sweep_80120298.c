// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

void SD_vShaderObject_Grass_Type_Init(void);
void SD_vShaderObject_Grass_Type_Close(void);

// The grass type starts: a pool of 32 render records of 32 bytes.
void SD_vShaderObject_Grass_Type_Init(void) {
    SD_gpGrassTypeData->pPool = fn_8000AFA0(32, 32, 2, 16);
}

void SD_vShaderObject_Grass_Type_Close(void) {
    fn_8000B058(SD_gpGrassTypeData->pPool);
}
