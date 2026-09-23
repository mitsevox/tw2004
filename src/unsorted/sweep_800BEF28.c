// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
s32 fn_80017314();
s32 fn_8001731C();
void fn_8003DCE8();
void fn_800C4724();

void fn_800BEF28(u8* p0, s32 p1);
void fn_800BEF28(u8* p0, s32 p1) {
    s32 t1;
    s32 t2;
    fn_800C4724();
    t1 = fn_8001731C(p0);
    t2 = fn_80017314(p0);
    fn_8003DCE8(p1, t1, t2, (p0 + 0x84), (p0 + 0x19C), 0, *(f32*)(gSession + 0x18));
}
