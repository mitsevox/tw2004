// Small functions found by the sweep (sweep.py). Original file: EASportsBio.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_80106ED8(s32, s32);
s32 fn_80107084(s32, s32);
s32 fn_801258E8();
s32 fn_8012DAB8(u16);

u8 fn_801257A0(void);
u8 fn_801257A0(void) {
    s32 temp_r31;
    s32 temp_r3;
    s32 var_r3;

    temp_r3 = fn_801258E8();
    temp_r31 = fn_80106ED8(0xC, temp_r3);
    var_r3 = fn_80107084(0xC, temp_r3);
    if (var_r3 == -1) {
        var_r3 = 0x4E3;
    }
    fn_8012DAB8((u16) var_r3);
    return temp_r31 > 0;
}