// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

void UStream_UnregisterHandler();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();
void fn_80035098();

void fn_80030818(void);
void fn_8003084C(void);
void fn_80030818(void) {
    UStream_UnregisterHandler(1952805408);
    UStream_UnregisterHandler(1952932896);
}

void fn_8003084C(void) {
    fn_80035098(0);
    fn_80012F50(1, 6, 128);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012EF8();
}
