// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 TagFile_DeleteSession(u8*);
s32 TagFile_GetSession(u8*);
s32 fn_8012C98C();
s32 fn_8012CB98();
extern u8* lbl_802825B0;

s32 fn_8012A364(s32* arg0);
s32 fn_8012A364(s32* arg0) {
    s32 temp_r0;
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x92)) == 0) {
        return 0x11;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x90)) == 0) {
        return 0x11;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_DeleteSession(lbl_802825B0 + 0x120);
        var_r5 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r5 = fn_8012CB98(arg0);
        if ((var_r5 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            var_r5 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
        }
    }
    return var_r5;
}