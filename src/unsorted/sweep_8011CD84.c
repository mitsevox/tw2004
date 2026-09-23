// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(void*);

void fn_8011CD84(void* arg0);
void fn_8011CD84(void* arg0) {
    void* temp_r3;

    temp_r3 = (*(void**)((u8*)(arg0) + 0x10C8));
    if (temp_r3 != NULL) {
        fn_80009E70((*(void**)((u8*)(temp_r3) + 4)));
        fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x10C8))) + 8)));
        fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x10C8))) + 0xC)));
        fn_80009E70((*(void**)((u8*)(arg0) + 0x10C8)));
        (*(void**)((u8*)(arg0) + 0x10C8)) = NULL;
    }
}