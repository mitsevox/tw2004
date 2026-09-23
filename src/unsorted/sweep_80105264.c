// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8* fn_80104E84();

u8* fn_80105264(void);
u8* fn_80105264(void) {
    u8* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return NULL;
    }
    return temp_r3 + 4;
}