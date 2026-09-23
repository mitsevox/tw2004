// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80189560[];
extern s32 lbl_80281EC8;
void Mem_cpy();
s32 fn_80009B34();
void fn_80009E70();

void fn_80076F80(u8* p0);
void fn_80076F80(u8* p0) {
    s32 t0;
    t0 = fn_80009B34(*(s32*)(p0 + 0x24), 2, 32, lbl_80189560, 285);
    lbl_80281EC8 = t0;
    Mem_cpy(t0, *(s32*)p0, *(s32*)(p0 + 0x24));
    fn_80009E70(p0);
}
