// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010F7E4(s32);

void fn_801088C8(s32* arg0);
void fn_801088C8(s32* arg0) {
    if ((s32) *arg0 == 0) {
        fn_8010F7E4(0);
        return;
    }
    fn_8010F7E4(1);
}