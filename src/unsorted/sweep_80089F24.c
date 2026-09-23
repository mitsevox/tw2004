// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E7F00(s32, s32);
s32 fn_800E7F68(s32, s32);

void fn_80089F24(void* arg0);
void fn_80089F24(void* arg0) {
    if ((s32) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        fn_800E7F68((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
        return;
    }
    fn_800E7F00((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
}