// uiText.c (our name): a menu UI text element (UIText): its string drawn in a font (UFont.c), in
// its own colour or one of the front end's colour table, aligned and optionally shadowed
// (fn_800922A8), and the message handler that sets it up (fn_800929E4). Its extent is its data:
// it is the only user of the .data 0x80189C38-0x80189CA0, .sbss 0x80281F30-0x80281F38 and .sdata2
// 0x80283BF0-0x80283C20 blocks, between fe_movies.c's and uiTransform.c's.

#include "game/frontend.h"

void fn_800922A8(UIText* pText);

void fn_800929E0(UIText* pText) {
}

// The element's messages: -2 draws it, -1 does nothing here, the others set or get its fields.
void fn_800929E4(UIText* pText, int nMsg, s32 n, MsgArg* pArgs, MsgArg* pResult) {
    u32 uValue;

    switch (nMsg) {
    case -1:
        fn_800929E0(pText);
        return;
    case -2:
        fn_800922A8(pText);
        return;
    case 0:
        pText->aColor[0] = pArgs[0].i;
        pText->aColor[1] = pArgs[1].i;
        pText->aColor[2] = pArgs[2].i;
        pText->aColor[3] = pArgs[3].i;
        return;
    case 1:
        pText->aShadowColor[0] = pArgs[0].i;
        pText->aShadowColor[1] = pArgs[1].i;
        pText->aShadowColor[2] = pArgs[2].i;
        pText->aShadowColor[3] = pArgs[3].i;
        return;
    case 3:
        // port: kept as an offset from the element, which fits 32 bits only within one block
        pText->nText = (u8*)pArgs[0].p - (u8*)pText;
        return;
    case 4:
        pResult->p = (u8*)pText + pText->nText;
        return;
    case 5:
        pText->nFlags &= ~3;
        if (pArgs[0].i == 0) {
            pText->nFlags |= 1;
        } else if (pArgs[0].i == 1) {
            pText->nFlags |= 2;
        }
        return;
    case 6:
        pText->v18[0] = pArgs[0].f;
        pText->v18[1] = pArgs[1].f;
        pText->v18[2] = pArgs[2].f;
        return;
    case 8:
        uValue = pArgs[0].i;
        pText->n8 = uValue;
        pText->nA = uValue >> 16;
        return;
    case 16:
        pText->nE = pArgs[0].i;
        return;
    case 17:
        pResult->i = pText->nE;
        return;
    case 18:
        pText->f30 = pArgs[0].i;
        return;
    case 19:
        pText->f34 = pArgs[0].i;
        return;
    case 20:
        pResult->i = pText->f30;
        return;
    case 21:
        pResult->i = pText->f34;
        return;
    case -3:                    // taken, and ignored
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80012898();
void fn_80092BA0(void);
void fn_80092BC4(void);
s32 fn_80012EC4();
void fn_80092C38(f32 x0, f32 x1);
void fn_80092C78(f32 x0, f32 x1);
void fn_80092CB8(f32 x0);

void fn_80092BA0(void) {
    fn_80012898(1);
}

void fn_80092BC4(void) {
    fn_80012898(0);
}

void fn_80092C38(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xCC) = x0;
    *(f32*)(((u8*)t0) + 0xD0) = x1;
}

void fn_80092C78(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xBC) = x0;
    *(f32*)(((u8*)t0) + 0xC0) = x1;
}

void fn_80092CB8(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xB8) = x0;
}

// ---- end of sweep code ----
