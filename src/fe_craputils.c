#include "game_types.h"

extern s32 gpSaveData;
void fn_80058208(u8* p0);
void fn_8001E9CC();
void fn_80058304(s32 p0);
extern s32 lbl_80281DF0;
void fn_800588D4(s16 arg0);
s16 fn_800588E8(void);

void fn_80058208(u8* p0) {
    *(u8*)(p0 + 0x5EB4) = 50;
    *(u8*)(p0 + 0x5EB5) = 50;
    *(u8*)(p0 + 0x5EB6) = 50;
    *(u8*)(p0 + 0x5EB7) = 50;
    *(u8*)(p0 + 0x5EB8) = 50;
    *(u8*)(p0 + 0x5EB9) = 50;
    *(u8*)(p0 + 0x5EBA) = 50;
    *(u8*)(p0 + 0x5EBB) = 50;
    *(u8*)(p0 + 0x5EBC) = 50;
    *(u8*)(p0 + 0x5EBD) = 50;
    *(u8*)(p0 + 0x5EBE) = 50;
    *(u8*)(p0 + 0x5EBF) = 50;
    *(u8*)(p0 + 0x5EC0) = 50;
    *(u8*)(p0 + 0x5EC1) = 50;
    *(u8*)(p0 + 0x5EC2) = 50;
    *(u8*)(p0 + 0x5EC3) = 50;
    *(u8*)(p0 + 0x5EC4) = 50;
    *(u8*)(p0 + 0x5EC5) = 50;
    *(u8*)(p0 + 0x5EC6) = 50;
    *(u8*)(p0 + 0x5EC7) = 50;
    *(u8*)(p0 + 0x5EC8) = 50;
    *(u8*)(p0 + 0x5EC9) = 50;
    *(u8*)(p0 + 0x5ECA) = 50;
    *(u8*)(p0 + 0x5ECB) = 50;
    *(u8*)(p0 + 0x5ECD) = 50;
    *(u8*)(p0 + 0x5ECC) = 50;
}

void fn_80058304(s32 p0) {
    fn_8001E9CC(((p0 + 0x10000) + 1352));
}

void fn_800588D4(s16 arg0) {
    (*(s8*)((u8*)(&lbl_80281DF0) + 0)) = 1;
    (*(s16*)((u8*)(&lbl_80281DF0) + 2)) = arg0;
}

s16 fn_800588E8(void) {
    return (*(s16*)((u8*)(&lbl_80281DF0) + 2));
}
