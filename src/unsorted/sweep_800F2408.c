// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800F37F8(s32);
s32 fn_800F59CC(s32);
s32 fn_800F6A00(s32);
s32 fn_800F7D94(s32);

s32 fn_800F2408(s32 arg0);
s32 fn_800F2408(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D94(arg0);
    }
    if (Game_GetMode() == 0xE) {
        return fn_800F37F8(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59CC(arg0);
    }
    if (Game_GetMode() == 0x11) {
        return fn_800F6A00(arg0);
    }
    return 0;
}