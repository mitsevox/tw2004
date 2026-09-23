// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8001EC48();
s32 fn_8001ECA8();

s32 fn_8001ECA8(void* arg0, s32 arg1);
s32 fn_8001ECA8(void* arg0, s32 arg1) {
    s32 var_r31;

    var_r31 = arg1;
    if (arg0 == NULL) {
        return 0;
    }
    if (fn_8001EC48() == 0) {
        var_r31 = 1;
    }
    return (*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0x38))) + 8)) + (var_r31 << 6);
}