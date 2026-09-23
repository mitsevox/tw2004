// uiText.c (our name): made by fold.py from 3 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800929E0(void);
void fn_80012898();
void fn_80092BA0(void);
void fn_80092BC4(void);
s32 fn_80012EC4();
void fn_80092C38(f32 x0, f32 x1);
void fn_80092C78(f32 x0, f32 x1);
void fn_80092CB8(f32 x0);

void fn_800929E0(void) {
}

void fn_80092BA0(void) {
    fn_80012898(1);
}

void fn_80092BC4(void) {
    fn_80012898(0);
}

void fn_80092C38(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xCC) = x0;
    *(f32*)(((u8*)t0) + 0xD0) = x1;
}

void fn_80092C78(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xBC) = x0;
    *(f32*)(((u8*)t0) + 0xC0) = x1;
}

void fn_80092CB8(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xB8) = x0;
}

// ---- end of sweep code ----
