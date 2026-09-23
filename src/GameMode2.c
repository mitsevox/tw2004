#include "game_types.h"

extern s32 lbl_802823C0;
extern s32 lbl_802823C4;
void fn_800F81EC(void);
extern void* gpGame;
s32 fn_800F9308(void);

void fn_800F81EC(void) {
    lbl_802823C4 = 0;
    lbl_802823C0 = 0;
}

s32 fn_800F9308(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0xD4)) != 0) {
        return lbl_802823C0;
    }
    return lbl_802823C0 + 1;
}
