// GoDynObjTypes.c (our name): two more types of the course's dynamic objects (dynobj.h), types 6
// (fn_8004AD54) and 9 (fn_8004AF2C, which has no model to draw). Its constant block is
// 0x80283280-0x80283288. Not decompiled yet beyond the functions below.

#include "game_types.h"

s32 fn_80048894(u8*);
s32 fn_8004ABBC(u8*, s32);
s32 fn_8004AD54(s32 arg0, u8* arg1, s32 arg2);
void fn_8004AF28(void);

s32 fn_8004AD54(s32 arg0, u8* arg1, s32 arg2) {
    switch (arg0) {
    case 1:
        return 0x16C;
    case 2:
        fn_8004ABBC(arg1, arg2);
        return 0;
    case 6:
        return 0;
    case 3:
        if ((u32) (*(u32*)((u8*)(arg1) + 0x100)) != 0U) {
            fn_80048894(arg1 + 0x10);
        }
        return 0;
    default:
        return 0;
    }
}

void fn_8004AF28(void) {
}
