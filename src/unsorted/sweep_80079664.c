// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Rand_Next(s32);
s32 fn_8008E6D4(s32);
s32 fn_80104700(s32, s32, s32);
s32 fn_80078E34(s32);
void fn_8007975C(s32 arg0, s32 arg1, s32 arg2);
s32 fn_800797E0(s32, s32, s32, s32);

void fn_80079664(s32 arg0);
void fn_80079664(s32 arg0) {
    fn_80104700(0xC, 1, 0);
    fn_80104700(0xC, 2, 0);
    fn_80104700(0xC, 3, 0);
    fn_80104700(0xC, 5, 0);
    fn_80104700(0xC, 6, 0);
    fn_80104700(0xC, 7, 0);
    fn_80104700(0xC, 0, 0);
    fn_8008E6D4(0);
    fn_8007975C(arg0, 1, 0);
    fn_8007975C(arg0, 2, 0);
    fn_8007975C(arg0, 7, 0);
    fn_800797E0(arg0, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_80078E34(arg0);
}