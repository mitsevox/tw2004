// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80095798(void* arg0);
s32 fn_80095798(void* arg0) {
    if (arg0 == NULL) {
        return -1;
    }
    return (*(s32*)((u8*)(arg0) + 0x1C));
}