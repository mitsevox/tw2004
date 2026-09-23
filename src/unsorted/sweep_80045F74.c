// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_RegisterHandler(s32, void (*)(void*), s32);
s32 fn_8000B4B8(void*);
u8 fn_8000B508();
s32 fn_80045D80(s32);
void fn_800460F8(void* arg0);

void fn_80045F74(void* arg0);
void fn_80045F74(void* arg0) {
    if (fn_8000B508() == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800460F8;
        fn_8000B4B8(arg0);
    }
}