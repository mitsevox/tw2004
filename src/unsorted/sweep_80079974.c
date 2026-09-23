// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077ACC();
s32 fn_80103B74(s32);
s32 FE_CrAP_TurnOnPart(s16, s32, s32);
s32 fn_8007975C(s32, s32, s32);
extern void* lbl_80281ED4;

void fn_80079974(void);
void fn_80079974(void) {
    s32 temp_r31;
    s32 var_r31;

    temp_r31 = fn_80077ACC();
    if ((u8) (*(u8*)((u8*)(lbl_80281ED4) + 0x1063F)) != 0) {
        fn_80103B74(0);
        fn_8007975C(temp_r31, 9, 0);
        FE_CrAP_TurnOnPart(3, 0, 0);
        FE_CrAP_TurnOnPart(0xE, 0, 0);
        FE_CrAP_TurnOnPart(0xF, 0, 0);
        FE_CrAP_TurnOnPart(0x10, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 1, 0);
        FE_CrAP_TurnOnPart(0xC, 2, 0);
        FE_CrAP_TurnOnPart(0xC, 3, 0);
        FE_CrAP_TurnOnPart(0xC, 5, 0);
        FE_CrAP_TurnOnPart(0xC, 6, 0);
        FE_CrAP_TurnOnPart(0xC, 7, 0);
        FE_CrAP_TurnOnPart(0xC, 4, 0);
        var_r31 = 0;
        do {
            FE_CrAP_TurnOnPart(0xD, 0, var_r31);
            FE_CrAP_TurnOnPart(0xD, 1, var_r31);
            var_r31 += 1;
        } while (var_r31 < 8);
        FE_CrAP_TurnOnPart(0xD, 2, 0);
        fn_80103B74(1);
    }
}