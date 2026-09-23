// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010F2FC();
s32 fn_8010F3A4();
s32 fn_8011DF90();

void fn_80084B88(s32* arg0);
void fn_80084B88(s32* arg0) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        fn_8010F2FC();
        return;
    case 1:
        fn_8010F3A4();
        return;
    case 3:
        fn_8011DF90();
        /* fallthrough */
    case 2:
        return;
    }
}