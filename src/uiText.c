// uiText.c (our name): a menu UI text element (UIText): its string drawn in a font (UFont.c), in
// its own colour or one of the front end's colour table, aligned and optionally shadowed
// (fn_800922A8), and the message handler that sets it up (fn_800929E4). Its extent is its data:
// it is the only user of the .data 0x80189C38-0x80189CA0, .sbss 0x80281F30-0x80281F38 and .sdata2
// 0x80283BF0-0x80283C20 blocks, between fe_movies.c's and uiTransform.c's.

#include "unsorted/cull.h"
#include "game/frontend.h"
#include "frontend/uisvec.h"

// The file's globals: the UI Studio's colour multiply and add (fn_8016C198, fn_8016C18C), read
// again at each draw.
UISVec4* lbl_80281F34;
UISVec4* lbl_80281F30;

// UFont.c's text state setters.
void UFont_SetFont(s32 nFont);
void fn_80012B6C(f32 f);
void fn_80012B9C(f32 fX, f32 fY);
void fn_80012C84_SetFlags(s32 uFlags);                           // 1/2: the alignment
void fn_80012CB4_SetWordWrapBox(f32 fX, f32 fY, f32 fW, f32 fH);

void fn_800760B0(s32 nX, s32 nY, s32 nW, s32 nH);

void fn_80092BE8(f32* pColor);
void fn_80092C38(f32 x0, f32 x1);
void fn_80092C78(f32 x0, f32 x1);
void fn_80092CB8(f32 x0);

// Draws the element's string: its place and size through the UI transform (screen units of
// 512 x 448), its colour through the UI Studio's multiply and add, then the string itself.
void fn_800922A8(UIText* pText) {
    UITransform t;
    f32 m[4][4];
    f32 aColor[4];
    Vec4 vPos;
    Vec4 vOut;
    Vec4 vEnd;
    Vec4 vEndOut;
    char* szText;
    s32 uFlags;
    f32 fX;
    f32 fY;
    f32 fW;
    f32 fH;
    f32 fR;
    f32 fG;
    f32 fA;
    f32 fB;
    UIColorTable* pTable;
    s16 nColor;
    u8* pRGBA;

    uFlags = 0;
    szText = ((MsgString*)((u8*)pText + pText->nText))->pStr;
    t = *fn_80093274();
    lbl_80281F30 = fn_8016C198();
    lbl_80281F34 = fn_8016C18C();
    t.m[3][0] = 512.0f * (t.m[3][0] / 512.0f);
    t.m[3][1] = 448.0f * (t.m[3][1] / 448.0f);
    Vec_Copy(t.m[0], m[0]);
    Vec_Copy(t.m[1], m[1]);
    Vec_Copy(t.m[2], m[2]);
    Vec_Copy(t.m[3], m[3]);
    vPos.x = 512.0f * (pText->v18[0] / 512.0f);
    vPos.y = 448.0f * (pText->v18[1] / 448.0f);
    vPos.z = 1.0f;
    vPos.w = 1.0f;
    fn_80012B9C(t.f6C, t.f70);
    fn_80012B6C(m[2][2]);
    fn_80092CB8(-t.f60);
    fn_80092C78(t.f64 / 512.0f, t.f68 / 448.0f);
    if (t.f60 > 0.0f) {
        uFlags |= 4;
        uFlags |= 0x400;
    }
    fn_800BAD60(m, &vPos, &vOut);
    fn_800BAD60(m, &vPos, &vOut);
    UFont_SetFont(pText->n4);
    fW = 1.0f;
    vEnd.x = 0.0f;
    fH = fW;
    vEnd.y = 0.0f;
    fX = vOut.x / 512.0f;
    vEnd.z = 0.0f;
    vEnd.w = fW;
    fY = vOut.y * (1.0f / 448.0f);
    if (pText->nFlags & 0x100) {
        vEnd.x = 512.0f * (pText->v18[0] / 512.0f) + 512.0f * (pText->f30 / 512.0f);
    }
    if (pText->nFlags & 0x200) {
        vEnd.y = 448.0f * (pText->v18[1] / 448.0f) + 448.0f * (pText->f34 / 448.0f);
    }
    // EA bug: tests 0x200 twice (0x100 was surely meant), so with only 0x100 set vEndOut is read
    // below without being set
    if ((pText->nFlags & 0x200) || (pText->nFlags & 0x200)) {
        fn_800BAD60(m, &vEnd, &vEndOut);
    }
    if (pText->nFlags & 0x100) {
        vEndOut.x /= 512.0f;
    }
    if (pText->nFlags & 0x200) {
        vEndOut.y *= 1.0f / 448.0f;
    }
    if (pText->nFlags & 0x100) {
        fW = vEndOut.x - fX;
    }
    if (pText->nFlags & 0x200) {
        fH = vEndOut.y - fY;
    }
    if ((pText->nFlags & 1) && (pText->nFlags & 0x100)) {
        fX += (vEndOut.x - fX) * 0.5f;
    }
    if (pText->nFlags & 0x10) {
        aColor[0] = (u8)(lbl_80281F30->a[0] * (pText->aShadowColor[0] + lbl_80281F34->a[0])) / 255.0f;
        aColor[1] = (u8)(lbl_80281F30->a[1] * (pText->aShadowColor[1] + lbl_80281F34->a[1])) / 255.0f;
        aColor[2] = (u8)(lbl_80281F30->a[2] * (pText->aShadowColor[2] + lbl_80281F34->a[2])) / 255.0f;
        aColor[3] = t.f5C +
                    (u8)(lbl_80281F30->a[3] * (pText->aShadowColor[3] + lbl_80281F34->a[3])) / 255.0f;
        if (aColor[3] < 0.0f) {
            aColor[3] = 0.0f;
        }
        if (aColor[3] > 0.5f) {
            aColor[3] = 0.5f;
        }
        fn_80092C38(pText->f24 / 512.0f, pText->f28 / 512.0f);
        uFlags |= 0x10000;
        fn_80092BE8(aColor);
    }
    nColor = pText->n8;
    pTable = lbl_80281F1C->p14;
    if (pTable != NULL && nColor < (s16)pTable->nCount && nColor != -1) {
        pRGBA = pTable->apEntries[nColor]->p8;
        fA = pRGBA[0];
        fB = pRGBA[1];
        fG = pRGBA[2];
        fR = pRGBA[3];
    } else {
        fR = pText->aColor[0];
        fG = pText->aColor[1];
        fB = pText->aColor[2];
        fA = pText->aColor[3];
    }
    aColor[0] = (u8)(lbl_80281F30->a[0] * (fR + lbl_80281F34->a[0])) / 255.0f;
    aColor[1] = (u8)(lbl_80281F30->a[1] * (fG + lbl_80281F34->a[1])) / 255.0f;
    aColor[2] = (u8)(lbl_80281F30->a[2] * (fB + lbl_80281F34->a[2])) / 255.0f;
    aColor[3] = (u8)(lbl_80281F30->a[3] * (fA + lbl_80281F34->a[3])) / 512.0f + t.f5C;
    if (aColor[3] < 0.0f) {
        aColor[3] = 0.0f;
    }
    if (aColor[3] > 0.5f) {
        aColor[3] = 0.5f;
    }
    fn_80013EEC(fn_8001614C());
    fn_8006A9AC(aColor);
    fn_80012CB4_SetWordWrapBox(fX, fY, fW, fH);
    if (pText->nFlags & 1) {
        uFlags |= 2;
    } else if (pText->nFlags & 2) {
        uFlags |= 1;
    }
    fn_80012C84_SetFlags(uFlags);
    if (0.0f != aColor[3]) {
        UFont_DrawString(szText, 0.0f, 0.0f);
        fn_800760B0(0, 0, 0x200, 0x1C0);
        fn_80012EF8();
    }
}

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

void UFont_SetMode();
void fn_80092BA0(void);
void fn_80092BC4(void);
void fn_80092BA0(void) {
    UFont_SetMode(1);
}

void fn_80092BC4(void) {
    UFont_SetMode(0);
}

// ---- end of sweep code ----

// Sets up the text shadow: nC4 0x12, and uC8 packed from the colour pColor.
void fn_80092BE8(f32* pColor) {
    UFont_GetContext()->nC4 = 0x12;
    UFont_PackColor(pColor, (u8*)&UFont_GetContext()->uC8);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80092C38(f32 x0, f32 x1) {
    UFontContext* pCtx;
    pCtx = UFont_GetContext();
    pCtx->fCC = x0;
    pCtx->fD0 = x1;
}

void fn_80092C78(f32 x0, f32 x1) {
    UFontContext* pCtx;
    pCtx = UFont_GetContext();
    pCtx->fBC = x0;
    pCtx->fC0 = x1;
}

void fn_80092CB8(f32 x0) {
    UFontContext* pCtx;
    pCtx = UFont_GetContext();
    pCtx->fB8 = x0;
}

// ---- end of sweep code ----
