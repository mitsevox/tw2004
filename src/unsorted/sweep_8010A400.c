// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80104FA8(s16, s32, s32);
u8 fn_801074D4();

void fn_8010A400(void* arg0, s32* arg1);
void fn_8010A400(void* arg0, s32* arg1) {
    fn_80104FA8((s16) (*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)(arg0) + 8)));
    *arg1 = (s32) fn_801074D4();
}