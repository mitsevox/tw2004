// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800C6CB0();
s32 fn_8011EE4C();
s32 fn_8011EF88();
s32 fn_8011F374();
s32 fn_8011F3AC();
extern void* lbl_80281900;

void fn_8011E974(void);
void fn_8011E974(void) {
    if (((u32) (*(u32*)((u8*)(lbl_80281900) + 0x370)) != 0U) && ((s32) (*(s32*)((u8*)(lbl_80281900) + 0x3E0)) != 0) && (fn_800C6CB0() == 0)) {
        fn_8011EF88();
        fn_8011F3AC();
        fn_8011F374();
        if ((s32) (*(s32*)((u8*)(lbl_80281900) + 0x3CC)) != 0) {
            fn_8011EE4C();
        }
    }
}