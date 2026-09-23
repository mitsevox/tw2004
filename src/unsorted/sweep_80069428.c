// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8000C140();
s32 fn_8004B65C();
s32 fn_8004B664();
extern void* lbl_80281E30;

s32 fn_80069428(s32 arg0);
s32 fn_80069428(s32 arg0) {
    if ((void* ) lbl_80281E30 != NULL) {
        if (fn_8000C140(arg0, (void* ) lbl_80281E30, (*(s16*)((u8*)(lbl_80281E30) + 2))) != 0) {
            return 1;
        }
        if (fn_8004B65C() > 0) {
            return fn_8004B664(arg0);
        }
        return 0;
    }
    return fn_8004B664();
}