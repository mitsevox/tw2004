// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "engine.h"

void fn_8006FF80(void* arg0, void* arg1);
void fn_8006FF80(void* arg0, void* arg1) {
    if (arg1 != NULL) {
        (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C((*(s32*)((u8*)(arg1) + 0)), (*(s32*)((u8*)(arg1) + 4)));
        return;
    }
    (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C(0x32, 1);
}
