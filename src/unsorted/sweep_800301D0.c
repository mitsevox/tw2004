// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800301D0(void* arg0);
s32 fn_800301D0(void* arg0) {
    return (*(s32*)((u8*)(arg0) + 0x60)) + ((s32) ((*(s32*)((u8*)(arg0) + 0x64)) * (*(s32*)((u8*)(arg0) + 0x68)) * 5) / 4);
}