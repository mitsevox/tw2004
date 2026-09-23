// Code8009AA28.c (our name; what it does is not known yet): code that drives SunFlr_Gc.c's
// functions (fn_8009A250-fn_8009A754) from its own state (lbl_802813B8) and tables, and queues
// glows (fn_8009B260). Its extent so far is the functions that use its data: the .sdata
// 0x802813B8-0x802813C0, the .sdata2 0x80283E90-0x80283EC0 and the .data 0x80189E78-0x8018C6C8
// (fn_8009AA28-fn_8009B18C, and fn_8009B260 between them and fn_8009B314). Where it starts after
// SunFlr_Gc.c (0x8009A844 at the earliest) is not proven, and fn_8009B314 is still in a sweep file
// with the next file's first function.

#include "glows.h"

void fn_8009B260(f32* pPos, u32 uColorA, u32 uColorB, u8 n25, u8 n24, u8 n26, f32 f18, f32 f1C,
                 f32 f20) {
    GlowQueued* pGlow;

    if (lbl_80281F80->nCount < NUM_GLOW_QUEUE) {
        pGlow = &lbl_80281F80->a[lbl_80281F80->nCount++];
        pGlow->uColorA = uColorA;
        pGlow->uColorB = uColorB;
        Vec_Copy(pPos, pGlow->vPos);
        pGlow->f18 = f18;
        pGlow->f1C = f1C;
        pGlow->f20 = f20;
        pGlow->n24 = n24;
        pGlow->n26 = n26;
        pGlow->n25 = n25;
    }
}
