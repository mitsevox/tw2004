#include "game_types.h"

void fn_800D27CC(u8* p0, s32 p1);
s32 fn_800D27E0(u16* arg0);
void fn_800D2884(s32 arg0, s32 arg1, s32* arg2, s32* arg3);
void fn_800D28B0(s32 arg0, s32 arg1, s32* arg2, s32* arg3);

void fn_800D27CC(u8* p0, s32 p1) {
    *(u16*)p0 = (*(u16*)p0 + (p1 & 0xFFFF));
}

s32 fn_800D27E0(u16* arg0) {
    return ((*arg0 - 1) % 7) + 1;
}

void fn_800D2884(s32 arg0, s32 arg1, s32* arg2, s32* arg3) {
    if (arg0 == 1) {
        *arg2 = 0xC;
        *arg3 = arg1 - 1;
        return;
    }
    *arg2 = arg0 - 1;
    *arg3 = arg1;
}

void fn_800D28B0(s32 arg0, s32 arg1, s32* arg2, s32* arg3) {
    if (arg0 == 0xC) {
        *arg2 = 1;
        *arg3 = arg1 + 1;
        return;
    }
    *arg2 = arg0 + 1;
    *arg3 = arg1;
}
