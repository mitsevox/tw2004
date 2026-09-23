// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80128528(void* arg0, void* arg1);
void fn_80128528(void* arg0, void* arg1) {
    if ((u8) (*(u8*)((u8*)(arg1) + 0x1166)) != 0) {
        (*(s32*)((u8*)(arg0) + 0)) = (s32) (*(s32*)((u8*)(arg1) + 0x50));
        (*(s32*)((u8*)(arg0) + 4)) = (s32) (*(s32*)((u8*)(arg1) + 0x54));
        (*(s32*)((u8*)(arg0) + 8)) = (s32) (*(s32*)((u8*)(arg1) + 0x58));
        (*(s32*)((u8*)(arg0) + 0xC)) = (s32) (*(s32*)((u8*)(arg1) + 0x5C));
        (*(s8*)((u8*)(arg0) + 0x10)) = 1;
        (*(s16*)((u8*)(arg1) + 0x1160)) = 1;
        return;
    }
    (*(s32*)((u8*)(arg0) + 0)) = 0;
    (*(s32*)((u8*)(arg0) + 4)) = 0;
    (*(s32*)((u8*)(arg0) + 8)) = 0;
    (*(s32*)((u8*)(arg0) + 0xC)) = 0;
    (*(s8*)((u8*)(arg0) + 0x10)) = 0;
}