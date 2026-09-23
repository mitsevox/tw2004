// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 CPUFifo;

s32 GXGetCPUFifo(void);
s32 GXGetCPUFifo(void) {
    return CPUFifo;
}
