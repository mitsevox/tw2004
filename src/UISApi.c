#include "game_types.h"

void fn_80169B0C(u8* p0, s32 p1, s32 p2);
void fn_80169B28(u8* p, s32 v);
void fn_80169B30(u8* p0, s32 p1, s32 p2);
void fn_80169B3C(u8* p, s32 v);
void fn_80169B44(u8* p, s32 v);

void fn_80169B0C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x48)) + (p1 << 2)) = p2;
    *(s32*)(p0 + 0x44) = (*(s32*)(p0 + 0x44) + 1);
}

void fn_80169B28(u8* p, s32 v) {
    *(s32*)(p + 0x1C) = v;
}

void fn_80169B30(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x14) = p1;
    *(s32*)(p0 + 0x18) = p2;
}

void fn_80169B3C(u8* p, s32 v) {
    *(s32*)(p + 0x28) = v;
}

void fn_80169B44(u8* p, s32 v) {
    *(s32*)(p + 0xC) = v;
}
