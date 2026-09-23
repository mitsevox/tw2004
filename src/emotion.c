#include "game_types.h"

extern u8 lbl_801D5F78[];
s32 fn_8006AA70(s32 p0);

s32 fn_8006AA70(s32 p0) {
    return *(s32*)(lbl_801D5F78 + (p0 * 36));
}
