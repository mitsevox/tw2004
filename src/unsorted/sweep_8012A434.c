// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 TagFile_End(u8*);
s32 fn_8012C98C();
s32 fn_8012CB98();
extern u8* lbl_802825B0;

s32 fn_8012A434(s32* arg0);
s32 fn_8012A434(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_End(lbl_802825B0 + 0x120);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    if ((s32) *arg0 == 2) {
        (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 0;
    }
    return var_r4;
}