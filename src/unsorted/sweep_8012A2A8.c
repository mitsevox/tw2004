// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 TagFile_Delete(s32*, s32, s32);
s32 TagFile_GetSession(u8*);
s32 fn_8012C98C();
s32 fn_8012CB98();
extern s32 lbl_802819B0;
extern u8* lbl_802825B0;

s32 fn_8012A2A8(s32* arg0);
s32 fn_8012A2A8(s32* arg0) {
    s32 temp_r0;
    s32 temp_r3;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_Delete(&lbl_802819B0, -1, 0);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
        if ((var_r4 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            temp_r3 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
            var_r4 = temp_r3;
            (*(s8*)((u8*)(lbl_802825B0) + 0x92)) = 1;
            (*(s8*)((u8*)(lbl_802825B0) + 0x90)) = 1;
        }
    }
    return var_r4;
}