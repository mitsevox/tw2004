// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8000B68C(s32);
s32 fn_80010608(s32);
s32 fn_8003467C();
s32 fn_80046664();
s32 fn_80048EF4();
s32 fn_80058E40();
s32 fn_80064EA4();
s32 fn_8006FBF8();
s32 fn_80098C28();
s32 fn_800A2B34(s32);
s32 fn_800C830C();
s32 fn_800C9CA0();
s32 fn_8011E3B4();
extern u8* lbl_802811E8;

void fn_8006F568(void);
void fn_8006F568(void) {
    s32 var_r31;

    fn_8006FBF8();
    fn_800C830C();
    fn_80058E40();
    fn_800C9CA0();
    if ((u8) *lbl_802811E8 != 0) {
        fn_8011E3B4();
        fn_8003467C();
        fn_80046664();
        fn_80064EA4();
        fn_80098C28();
        var_r31 = 0;
        do {
            fn_800A2B34(var_r31);
            var_r31 += 1;
        } while (var_r31 < 4);
        fn_80048EF4();
        fn_80010608(1);
        fn_8000B68C(1);
        *lbl_802811E8 = 0;
    }
}