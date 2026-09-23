// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 TagFile_FreeBuffer();
s32 TagFile_Shutdown();
s32 fn_8012C98C();
extern void* lbl_802825B0;

void fn_8012CC48(void);
void fn_8012CC48(void) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = 0;
    if ((u32) (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) != 0U) {
        var_r31 = TagFile_FreeBuffer((*(u32*)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0x8C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)), 0);
        var_r3 = fn_8012C98C();
        (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) = 0U;
    } else {
        var_r3 = 0x22;
    }
    if (var_r3 == 0) {
        TagFile_Shutdown();
        fn_8012C98C(var_r31);
    }
}