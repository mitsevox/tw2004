// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 TagFile_Write(u8*, s32, u8, s32*, s32);
s32 fn_8012C98C();
s32 fn_8012CB98();
s32 fn_80129D70(s32, s32, s32*, s32);
extern u8* lbl_802825B0;

s32 fn_8012B0D8(s32* arg0);
s32 fn_8012B0D8(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x91)) == 0) {
        return 0x21;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        fn_80129D70((*(s32*)((u8*)(lbl_802825B0) + 0x190)), 1, (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)));
        TagFile_Write(lbl_802825B0 + 0x120, 0x494D4147, (*(u8*)((u8*)(lbl_802825B0) + 0x98)), (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), 0x4301);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    return var_r4;
}