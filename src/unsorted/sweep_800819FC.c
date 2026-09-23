// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800DFC6C();
s32 fn_800E1434();
extern void* gpGame;

void fn_800819FC(void* arg0);
void fn_800819FC(void* arg0) {
    (*(u8*)((u8*)(gpGame) + 0x136)) = (u8) (*(s32*)((u8*)(arg0) + 0));
    (*(s32*)((u8*)(gpGame) + 0x13C)) = (s32) (*(s32*)((u8*)(arg0) + 4));
    (*(s32*)((u8*)(gpGame) + 0x140)) = (s32) (*(s32*)((u8*)(arg0) + 8));
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x136)) != 0) {
        fn_800DFC6C();
        fn_800E1434();
    }
}