// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 DVDGetCurrentDiskID();

s32 fn_8011027C(void);
s32 fn_8011027C(void) {
    s32 t0;
    t0 = DVDGetCurrentDiskID();
    return ((u32)((-*(u8*)(((u8*)t0) + 0x6)) | *(u8*)(((u8*)t0) + 0x6)) >> 31);
}
