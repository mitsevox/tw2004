#include "game_types.h"

extern u8 gSession[];
s32 fn_80017314();
s32 fn_8001731C();
void fn_8003DCE8();
void fn_800C0804(u8* p0, s32 p1);
void fn_8006351C();
void fn_800C3EB8(s32 p0, s32 p1);

void fn_800C0804(u8* p0, s32 p1) {
    s32 t0;
    s32 t1;
    t0 = fn_8001731C();
    t1 = fn_80017314(p0);
    fn_8003DCE8(p1, t0, t1, (p0 + 0x84), (p0 + 0x19C), 0, *(f32*)(gSession + 0x18));
}

void fn_800C3EB8(s32 p0, s32 p1) {
    fn_8006351C(p0, p1, 10);
}
