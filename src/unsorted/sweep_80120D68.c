// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "grassshader.h"

GrassWord* GrassPacket_pGetNextAvailableVertSlot(void);

GrassWord* GrassPacket_pGetNextAvailableVertSlot(void) {
    return SD_gpGrassTypeData->pCur;
}
