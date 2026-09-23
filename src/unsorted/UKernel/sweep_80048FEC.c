// Small functions found by the sweep (sweep.py). Original file: UKernel.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern void* lbl_80281DB8;
extern void* lbl_80281DBC;

void fn_80048FEC(void* arg0);
void fn_80048FEC(void* arg0) {
    if ((void* ) lbl_80281DB8 != NULL) {
        (*(void**)((u8*)(lbl_80281DB8) + 0x128)) = arg0;
        lbl_80281DB8 = arg0;
    } else {
        lbl_80281DBC = arg0;
        lbl_80281DB8 = arg0;
    }
    (*(s32*)((u8*)(arg0) + 0x128)) = 0;
}