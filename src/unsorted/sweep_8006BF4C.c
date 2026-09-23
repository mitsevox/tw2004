// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gReplayData[];

u8* fn_8006BF4C(void);
u8* fn_8006BF4C(void) {
    *(u8*)(gReplayData + 0xF10) = 0;
    return gReplayData;
}
