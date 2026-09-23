// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80077B08();
s32 fn_80118664();

void fn_8010E85C(s32 p0, u8* p1);
void fn_8010E85C(s32 p0, u8* p1) {
    s32 t1;
    fn_80077B08();
    t1 = fn_80118664();
    *(s32*)p1 = t1;
}
