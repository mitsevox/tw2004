// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(s8*);
extern s8* gpSaveData;
extern s8* lbl_80281DF4;

void fn_80056454(void);
void fn_80056454(void) {
    fn_80009E70(gpSaveData);
    fn_80009E70(lbl_80281DF4);
}