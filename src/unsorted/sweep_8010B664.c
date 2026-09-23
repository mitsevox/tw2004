// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010C458(s16);

s32 fn_8010B664(void* arg0);
s32 fn_8010B664(void* arg0) {
    s32 var_r3;

    if ((s16) (*(s16*)((u8*)(arg0) + 8)) > 0x10) {
        var_r3 = 0x40;
        if (fn_8010C458((*(s16*)((u8*)(arg0) + 0xA))) == 0x10) {
            return 0x20;
        }
        /* Duplicate return node #4. Try simplifying control flow for better match */
        return var_r3;
    }
    var_r3 = 4;
    return var_r3;
}