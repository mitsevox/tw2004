// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
s32 fn_800CD56C();
s32 fn_801127C4();
s32 fn_8011CD84();

void fn_80037CD8(void* arg0);
void fn_80037CD8(void* arg0) {
    void* temp_r3;

    fn_80037708();
    fn_8011CD84(arg0);
    fn_800CD56C(arg0);
    temp_r3 = (*(void**)((u8*)(arg0) + 0));
    if (temp_r3 != NULL) {
        if ((void* ) (*(void**)((u8*)(temp_r3) + 0x48)) != NULL) {
            fn_801127C4((void* ) (*(void**)((u8*)(temp_r3) + 0x48)));
            fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)));
        }
        fn_80009E70((*(void**)((u8*)(arg0) + 0)));
    }
    if ((u32) (*(u32*)((u8*)(arg0) + 0x1088)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x1088)));
    }
    fn_80009E70(arg0);
}