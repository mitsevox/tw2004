// GoAnimalActors.c (TW06's golf/hi-rendering/goanimalactors.c; our spelling): the animals on the
// course (ActAnimal_*). Not yet decompiled; the unit covers the file's certain core, whose
// functions share its constant block (0x80283218-0x80283280).

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80048894(u8*);
s32 fn_80049820();
s32 fn_8004A24C(u8*, s32);
s32 fn_8004A578(u8*, s32);
s32 fn_8004AAEC(s32 arg0, u8* arg1, s32 arg2);
s32 fn_8004ABA4(u8* p0, s32 p1);
s32 fn_8004ABB4(u8* p);

s32 fn_8004AAEC(s32 arg0, u8* arg1, s32 arg2) {
    switch (arg0) {
    case 1:
        return 0x1C0;
    case 2:
        fn_8004A24C(arg1, arg2);
        return 0;
    case 6:
        fn_8004A578(arg1, arg2);
        return 0;
    case 3:
        if ((u32) (*(u32*)((u8*)(arg1) + 0x100)) != 0U) {
            (*(s32*)((u8*)(arg1) + 0x118)) = (s32) (*(s32*)((u8*)(arg1) + 0x1AC));
            (*(f32*)((u8*)(arg1) + 0x11C)) = (f32) (*(f32*)((u8*)(arg1) + 0x1B4));
            fn_80048894(arg1 + 0x10);
        }
        return 0;
    default:
        return fn_80049820();
    }
}

s32 fn_8004ABA4(u8* p0, s32 p1) {
    return *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + (p1 << 2));
}

s32 fn_8004ABB4(u8* p) {
    return *(s32*)(p + 0xEC);
}

// ---- end of sweep code ----
