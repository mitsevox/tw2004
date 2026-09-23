// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80193D28[];
extern s32 lbl_802824B8;
extern s32 lbl_802824BC;
extern u8 lbl_802824C0;
void fn_80005AE8();
s32 fn_80009B34();
void fn_80009E70();
void fn_8010FAF4();

void fn_8010F6C8(void);
void fn_8010F718(void);
void fn_8010F748(void);
void fn_8010F794(void);
void fn_8010F6C8(void) {
    s32 t0;
    t0 = fn_80009B34(512, 0, 0, lbl_80193D28, 47);
    lbl_802824BC = t0;
    fn_80005AE8(t0, 0, 512);
    lbl_802824C0 = 0;
}

void fn_8010F718(void) {
    fn_80009E70(lbl_802824BC);
    lbl_802824BC = 0;
    lbl_802824C0 = 0;
}

void fn_8010F748(void) {
    s32 t0;
    t0 = fn_80009B34(12, 2, 0, lbl_80193D28, 67);
    lbl_802824B8 = t0;
    fn_80005AE8(t0, 0, 12);
    fn_8010FAF4();
}

void fn_8010F794(void) {
    fn_80009E70(lbl_802824B8);
    lbl_802824B8 = 0;
}
