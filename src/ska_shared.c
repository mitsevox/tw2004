// ska_shared.c (TW06's golf/animation/ska_shared.c): the skeletal animation code the golfer's
// character shares: blending animation clips into bone rotations (quaternions) and reading the
// clip banks. Not yet decompiled; the code below is the matched small functions.

#include "game_types.h"
#include "core/goaram.h"

// ---- sweep code (not yet cleaned up) ----

ARAMTransfer* fn_8001FCA8(u32 uAram, void* pDst, u32 uSize);
void fn_8001FCD4(ARAMTransfer* pTransfer);
extern u8 lbl_80281CC0;
void fn_80021978(u8 v);
f32 fn_800205F8();
extern f32 lbl_80282CC0;
f32 fn_80021A98(void* arg0);

// Starts copying uSize bytes from ARAM address uAram to pDst; fn_8001FCD4 waits for it.
ARAMTransfer* fn_8001FCA8(u32 uAram, void* pDst, u32 uSize) {
    return fn_800B68B4(pDst, uAram, uSize);
}

void fn_8001FCD4(ARAMTransfer* pTransfer) {
    fn_800B67EC(pTransfer);
}

void fn_80021978(u8 v) {
    lbl_80281CC0 = v;
}

f32 fn_80021A98(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 0xD8));
    if (temp_r31 != NULL) {
        return fn_800205F8() / (*(f32*)((u8*)(temp_r31) + 0x1EC));
    }
    return lbl_80282CC0;
}

// ---- end of sweep code ----
