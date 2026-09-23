// Small functions found by the sweep (sweep.py). Original file: GoShaderObject_Particle_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_80189CC8[];
extern s32 lbl_802813A8;
s32 fn_80009B34();

void fn_8009414C(void);
void fn_8009414C(void) {
    s32 t0;
    s32 t1;
    s32 t2;
    s32 t3;
    t0 = fn_80009B34(90000, 2, 32, lbl_80189CC8, 109);
    *(s32*)((u8*)lbl_802813A8) = t0;
    t1 = fn_80009B34(90000, 2, 32, lbl_80189CC8, 111);
    *(s32*)(((u8*)lbl_802813A8) + 0x4) = t1;
    t2 = fn_80009B34(10000, 2, 32, lbl_80189CC8, 113);
    *(s32*)(((u8*)lbl_802813A8) + 0x8) = t2;
    t3 = fn_80009B34(10000, 2, 32, lbl_80189CC8, 115);
    *(s32*)(((u8*)lbl_802813A8) + 0xC) = t3;
    *(s32*)(((u8*)lbl_802813A8) + 0x14) = 0;
    *(u8*)(((u8*)lbl_802813A8) + 0x10) = 0;
}
