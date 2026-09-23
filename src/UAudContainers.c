#include "game_types.h"

void fn_800ADE70(u8* p0, s32 p1);
void fn_800ADEC8(void* arg0, void* arg1);
void fn_800ADF6C(void* arg0, void* arg1);
void fn_800AE00C(u8* p0, s32 p1, s32 p2, s32 p3);
u32 fn_800AE03C(void* arg0);
s32* fn_800AE1AC(void* arg0);

void fn_800ADE70(u8* p0, s32 p1) {
    *(s32*)p0 = 0;
    *(s32*)(p0 + 0x4) = 0;
    *(u16*)(p0 + 0x8) = p1;
    *(u16*)(p0 + 0xA) = 0;
}

void fn_800ADEC8(void* arg0, void* arg1) {
    if ((u16) (*(u16*)((u8*)(arg0) + 0xA)) != 0) {
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 4)) = arg1;
    } else {
        (*(void**)((u8*)(arg0) + 0)) = arg1;
    }
    (*(void**)((u8*)(arg1) + 0)) = (void* ) (*(void**)((u8*)(arg0) + 4));
    (*(s32*)((u8*)(arg1) + 4)) = 0;
    (*(void**)((u8*)(arg0) + 4)) = arg1;
    (*(u16*)((u8*)(arg0) + 0xA)) = (u16) ((*(u16*)((u8*)(arg0) + 0xA)) + 1);
}

void fn_800ADF6C(void* arg0, void* arg1) {
    void* temp_r0;
    void** temp_r0_2;

    if (arg1 == (void* ) (*(void**)((u8*)(arg0) + 4))) {
        temp_r0 = (*(void**)((u8*)(arg1) + 0));
        if (temp_r0 != NULL) {
            (*(void**)((u8*)(arg0) + 4)) = temp_r0;
            (*(void***)((u8*)((*(void**)((u8*)(arg1) + 0))) + 4)) = NULL;
        } else {
            (*(void**)((u8*)(arg0) + 4)) = NULL;
            (*(void***)((u8*)(arg0) + 0)) = NULL;
        }
    } else if (arg1 == (void** ) (*(void***)((u8*)(arg0) + 0))) {
        temp_r0_2 = (*(void***)((u8*)(arg1) + 4));
        if (temp_r0_2 != NULL) {
            (*(void***)((u8*)(arg0) + 0)) = temp_r0_2;
            *(*(void***)((u8*)(arg1) + 4)) = NULL;
        } else {
            (*(void**)((u8*)(arg0) + 4)) = NULL;
            (*(void***)((u8*)(arg0) + 0)) = NULL;
        }
    } else {
        (*(void***)((u8*)((*(void**)((u8*)(arg1) + 0))) + 4)) = (void** ) (*(void***)((u8*)(arg1) + 4));
        *(*(void***)((u8*)(arg1) + 4)) = (*(void**)((u8*)(arg1) + 0));
    }
    (*(u16*)((u8*)(arg0) + 0xA)) = (u16) ((*(u16*)((u8*)(arg0) + 0xA)) - 1);
}

void fn_800AE00C(u8* p0, s32 p1, s32 p2, s32 p3) {
    *(s32*)p0 = p1;
    *(s32*)(p0 + 0x4) = (p1 + ((p2 & 0xFFFF) * p3));
    *(s32*)(p0 + 0xC) = p1;
    *(s32*)(p0 + 0x8) = p1;
    *(u16*)(p0 + 0x10) = p2;
    *(u16*)(p0 + 0x12) = 0;
    *(s32*)(p0 + 0x14) = p3;
}

u32 fn_800AE03C(void* arg0) {
    u32 temp_r4;
    u32 temp_r5;
    u32 temp_r6;

    temp_r6 = (*(u32*)((u8*)(arg0) + 0xC));
    (*(u16*)((u8*)(arg0) + 0x12)) = (u16) ((*(u16*)((u8*)(arg0) + 0x12)) + 1);
    (*(u32*)((u8*)(arg0) + 0xC)) = (u32) ((*(u32*)((u8*)(arg0) + 0xC)) + (*(s32*)((u8*)(arg0) + 0x14)));
    temp_r4 = (*(u32*)((u8*)(arg0) + 0xC));
    temp_r5 = (*(u32*)((u8*)(arg0) + 4));
    if (temp_r4 >= temp_r5) {
        (*(u32*)((u8*)(arg0) + 0xC)) = (u32) (temp_r4 - (temp_r5 - (*(s32*)((u8*)(arg0) + 0))));
    }
    return temp_r6;
}

s32* fn_800AE1AC(void* arg0) {
    s32* temp_r5;

    temp_r5 = (*(s32**)((u8*)(arg0) + 0));
    if (temp_r5 == arg0) {
        return NULL;
    }
    (*(s32**)((u8*)(arg0) + 0)) = (s32* ) *temp_r5;
    (*(s32*)((u8*)(arg0) + 4)) = (s32) ((*(s32*)((u8*)(arg0) + 4)) - 1);
    return temp_r5;
}
