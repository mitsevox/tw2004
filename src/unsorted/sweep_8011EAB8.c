// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281900;
void fn_800137B0();
void fn_8006E214();
void fn_8007644C();
void fn_80076B18();

void fn_8011EAB8(void);
void fn_8011EAB8(void) {
    fn_8006E214(*(s32*)(((u8*)lbl_80281900) + 0x78));
    fn_8007644C(*(s32*)(((u8*)lbl_80281900) + 0x74));
    fn_80076B18(*(s32*)(((u8*)lbl_80281900) + 0x7C));
    fn_800137B0(*(s32*)(((u8*)lbl_80281900) + 0x70));
}
