// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_8001EFFC();
s32 fn_8001F004();
extern f32 lbl_80282BC0;
extern f32 lbl_80282BF8;

f32 fn_8001EE00(void* arg0, s32 arg1);
f32 fn_8001EE00(void* arg0, s32 arg1) {
    if (arg1 != 0) {
        fn_8001F004();
        return (*(f32*)((u8*)(arg0) + 0x165C)) * (lbl_80282BF8 / fn_8001EFFC());
    }
    fn_8001F004();
    return (*(f32*)((u8*)(arg0) + 0x165C)) * (lbl_80282BC0 / fn_8001EFFC());
}