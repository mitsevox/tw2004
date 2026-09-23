// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80269080[];
extern s32 lbl_802826F8;

void fn_80136DF4(s32 v);
u8* __AXGetStudio(void);
void fn_80136DF4(s32 v) {
    lbl_802826F8 = v;
}

u8* __AXGetStudio(void) {
    return lbl_80269080;
}
