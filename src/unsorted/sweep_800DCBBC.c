// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E0A84(s32);
s32 fn_800E1074();
s32 fn_800E1434();
u8 fn_800EC550();
extern void* gpGame;

void fn_800DCBBC(void);
void fn_800DCBBC(void) {
    if (fn_800EC550() == 0) {
        fn_800E1074();
        fn_800E1434();
    }
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1EC))(gpGame);
    fn_800E0A84(1);
}