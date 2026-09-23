// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8001EE88(u8* p);
s32 fn_8001EE90(u8* p);
s32 fn_8001EE88(u8* p) {
    return *(s32*)(p + 0x1658);
}

s32 fn_8001EE90(u8* p) {
    return *(s32*)(p + 0x1654);
}
