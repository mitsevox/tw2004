// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801945E8[];
extern u8 lbl_801945F8[];
extern u8 lbl_80194618[];
extern u8 lbl_8026038C[];
void GXLoadTexObj();
void GXSetNumTevStages();
void GXSetNumTexGens();
void GXSetTevAlphaIn();
void GXSetTevAlphaOp();
void GXSetTevColorIn();
void GXSetTevColorOp();
void GXSetTevOrder();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();
void fn_80014118();
void fn_80014194();
void fn_8001425C();
void fn_8001644C();

void fn_8011EE4C(void);
void fn_8011EE4C(void) {
    fn_8001425C(0);
    fn_80014118(16);
    fn_80014194(lbl_801945E8);
    fn_80012F34(0);
    fn_80012F50(0, 6, 128);
    GXLoadTexObj(lbl_8026038C, 0);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetNumTevStages(1);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 1, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    fn_80012F18(7);
    fn_80012EF8();
    fn_8001644C(161, lbl_801945F8, 0, lbl_80194618, 2);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}
