// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281AB0;

s32 OSSetCurrentHeap(s32 arg0);
s32 OSSetCurrentHeap(s32 arg0) {
    s32 temp_r0;

    temp_r0 = lbl_80281AB0;
    lbl_80281AB0 = arg0;
    return temp_r0;
}