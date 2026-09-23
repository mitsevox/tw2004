// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001EED8(s32, s32);

s32 fn_8001DBF4(void* arg0);
s32 fn_8001DBF4(void* arg0) {
    if (((void* ) (*(void**)((u8*)(arg0) + 0x1788)) != NULL) && (fn_8001EED8((*(s32*)((u8*)(arg0) + 0x38)), 0x54) != 0xFF) && ((s32) (*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0x1788))) + 0x1C)) > fn_8001EED8((*(s32*)((u8*)(arg0) + 0x38)), 0x54))) {
        return 1;
    }
    return 0;
}