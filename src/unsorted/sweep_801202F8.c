// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

void SD_vShaderObject_Grass_Type_SetParameters(void* pParams);

void SD_vShaderObject_Grass_Type_SetParameters(void* pParams) {
    SD_gpGrassTypeData->pParams = pParams;
}
