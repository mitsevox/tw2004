// Code8009A928.c (our name): made by fold.py from 4 sweep files; not yet described.

#include "game_types.h"
#include "glows.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800988D8(void);
void fn_8009AA00(int n);
void fn_8009AF30(s32 nViews);
void fn_8009B604(void);
void fn_8009A928(s32 p0);
void fn_80098910(void);
void fn_8009B0D0(void);
void fn_8009B610(void);
void fn_8009A968(void);
void fn_8009AA18(void);
void fn_8009B18C(s32 nView);
void fn_8009B528(f32 fTime);
extern f32 lbl_80283E90;
void fn_8009A990(s32 arg0);

void fn_8009A928(s32 p0) {
    fn_800988D8();
    fn_8009AA00(0);
    fn_8009B604();
    fn_8009AF30(p0);
}

void fn_8009A968(void) {
    fn_8009B0D0();
    fn_8009B610();
    fn_80098910();
}

void fn_8009A990(s32 arg0) {
    fn_8009AA18();
    fn_8009B18C(arg0);
    fn_8009B528(lbl_80283E90);
}

void fn_8009AA18(void) {
    lbl_80281F80->nCount = 0;
}

// ---- end of sweep code ----
