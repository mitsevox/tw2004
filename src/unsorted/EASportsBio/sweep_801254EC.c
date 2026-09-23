// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8012D710();
s32 fn_80124A98();

s32 fn_801254EC(void);
s32 fn_801254EC(void) {
    if (fn_8012D710() != 0) {
        return fn_80124A98();
    }
    return 0;
}