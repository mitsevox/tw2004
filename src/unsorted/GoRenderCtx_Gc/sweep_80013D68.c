// Small functions found by the sweep (sweep.py). Original file: GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

void RC_vUpdateRenderCtxScreenMatricesAndInfo();
void fn_80013CCC();
void fn_80013D58();
void fn_80013DD0();

void fn_80013D68(s32 p0);
void fn_80013D9C(s32 p0);
void fn_80013D68(s32 p0) {
    RC_vUpdateRenderCtxScreenMatricesAndInfo();
    fn_80013D58(p0);
}

void fn_80013D9C(s32 p0) {
    fn_80013DD0();
    fn_80013CCC(p0);
}
