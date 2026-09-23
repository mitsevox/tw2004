// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281900;

s32 fn_8012022C(void);
s32 fn_8012022C(void) {
    return ((u32)((-*(s32*)(((u8*)lbl_80281900) + 0x370)) | *(s32*)(((u8*)lbl_80281900) + 0x370)) >> 31);
}
