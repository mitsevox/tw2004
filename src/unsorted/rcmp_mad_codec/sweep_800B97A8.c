// Small functions found by the sweep (sweep.py). Original file: rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

void* fn_800B9760();

void* fn_800B97A8(s32 arg0);
void* fn_800B97A8(s32 arg0) {
    void* temp_r3;

    temp_r3 = fn_800B9760(arg0 + 0x38);
    if (temp_r3 == NULL) {
        return NULL;
    }
    fn_800B9624(arg0 + 0x20, temp_r3);
    (*(s32*)((u8*)(temp_r3) + 0xC)) = 1;
    return temp_r3;
}