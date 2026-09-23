// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_800B5BD8();
extern void* lbl_80282070;

void* fn_800A9374(void);
void* fn_800A9374(void) {
    void* temp_r3;

    if ((void* ) lbl_80282070 == NULL) {
        temp_r3 = fn_800B5BD8();
        lbl_80282070 = temp_r3;
        return temp_r3;
    }
    return NULL;
}