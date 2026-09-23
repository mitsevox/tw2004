// AnimStream.c (EA's name, from its asserts): streams animation clips for the reaction groups
// (1 and 5) per player, style and club class. Only partly decompiled; the sweep code below is the
// matched small functions. The types are in character.h.

#include "character.h"

AnimStreamGroup lbl_80191490[2] = {
    { 1, 0 },
    { 5, 1 },
};

// The index of an animation group in the stream's tables, -1 when it is not streamed.
int fn_800C98DC(int nGroup) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nGroup == lbl_80191490[i].nGroup) {
            return lbl_80191490[i].nIndex;
        }
    }
    return -1;
}

// The animation group at an index of the stream's tables, -1 for none.
int fn_800C9928(int nIndex) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nIndex == lbl_80191490[i].nIndex) {
            return lbl_80191490[i].nGroup;
        }
    }
    return -1;
}

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80282230;
void fn_800C9EFC(s32 p0);
void fn_800CB550(s32 p0);

void fn_800C9EFC(s32 p0) {
    *(s32*)(((u8*)lbl_80282230) + 0x1CC0) = 2;
    *(s32*)(((u8*)lbl_80282230) + 0x1CC4) = p0;
}

void fn_800CB550(s32 p0) {
    *(u8*)(((u8*)lbl_80282230) + 0x1CCC) = 1;
    *(s32*)(((u8*)lbl_80282230) + 0x1CC4) = p0;
}

// ---- end of sweep code ----
