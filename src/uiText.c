// uiText.c (our name; no TW06 or TW07 file has this code): the menu UI's text element (UIText, the
// UI studio's plugin 7: UIText_ProcessMessage): its string drawn in a font (UFont.c), in its own
// colour or one of the front end's colour table, placed, turned and scaled by the UI transform,
// aligned and optionally shadowed (UIText_Draw); and the font settings it sets for that (the
// shadow, the align point, the angle, drawing at once or queued). Its extent is its data: it is
// the only user of the .data 0x80189C38-0x80189CA0, .sbss 0x80281F30-0x80281F38 and .sdata2
// 0x80283BF0-0x80283C20 blocks, between fe_movies.c's and uiTransform.c's.

#include "unsorted/cull.h"
#include "game/frontend.h"
#include "frontend/uisvec.h"

// The UI studio's colour add and multiply (UISGetColorAdditive, UISGetColorMultipler), taken again
// at each UIText_Draw.
UISColorVectorT* gpUITextColourAdd;
UISColorVectorT* gpUITextColourMul;

// UFont.c's text state setters.
void FO_vSetCurrentAddMode(s32 nMode);  // 1: strings are drawn at once, 0: queued
void UFont_SetFont(s32 nFont);
void fn_80012B6C(f32 f);
void fn_80012B9C(f32 fX, f32 fY);
void fn_80012C84_SetFlags(s32 uFlags);                           // 1/2: the alignment
void fn_80012CB4_SetWordWrapBox(f32 fX, f32 fY, f32 fW, f32 fH);

void fn_800760B0(s32 nX, s32 nY, s32 nW, s32 nH);

void UIText_SetFontShadowColour(f32* pColor);
void UIText_SetFontShadowOffset(f32 fX, f32 fY);
void UIText_SetFontAlignPoint(f32 fX, f32 fY);
void UIText_SetFontAngle(f32 fAngle);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0, before the 512 and 448 UIText_Draw uses first; its body is unknown, this one only
// reproduces the order.
static f32 uiText_StrippedFn(f32 f) {
    return f + 1.0f;
}

// Draw pText, a text element of the menu UI: its string (nText, an offset from the element) in font
// n4, placed and sized through the current UI transform (screen units of 512 x 448) and turned by
// the transform's angle (f60) about its point (f64, f68). Flags 0x100 / 0x200 give it a box f30
// wide / f34 high to word-wrap in; flag 1 centres it (across its box with 0x100), flag 2 aligns it
// right; flag 0x10 draws a shadow in aShadowColor, moved by f24, f28. Its colour is colour-table
// colour n8 (not -1) or aColor, tinted by the UI studio's multiply and add, the alpha offset by the
// transform's (f5C) and held to 0..0.5; nothing is drawn at alpha 0.
void UIText_Draw(UIText* pText) {
    UITransform t;
    f32 m[4][4];
    f32 aColor[4];
    Vec4 vPos;
    Vec4 vOut;
    Vec4 vEnd;
    Vec4 vEndOut;
    char* szText;
    s32 uFlags;
    f32 fInvH;
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
    t = *UITransform_GetCurrent();
    gpUITextColourMul = UISGetColorMultipler();
    gpUITextColourAdd = UISGetColorAdditive();
    t.m[3][0] = 512.0f * (t.m[3][0] / 512.0f);
    t.m[3][1] = 448.0f * (t.m[3][1] / 448.0f);
    LLMath_CopyVec(t.m[0], m[0]);
    LLMath_CopyVec(t.m[1], m[1]);
    LLMath_CopyVec(t.m[2], m[2]);
    LLMath_CopyVec(t.m[3], m[3]);
    vPos.x = 512.0f * (pText->v18[0] / 512.0f);
    vPos.y = 448.0f * (pText->v18[1] / 448.0f);
    vPos.z = 1.0f;
    vPos.w = 1.0f;
    fn_80012B9C(t.f6C, t.f70);
    fn_80012B6C(m[2][2]);
    UIText_SetFontAngle(-t.f60);
    UIText_SetFontAlignPoint(t.f64 / 512.0f, t.f68 / 448.0f);
    if (t.f60 > 0.0f) {
        uFlags |= 4;
        uFlags |= 0x400;
    }
    LLMath_mat44fltMultiply(m, &vPos, &vOut);
    LLMath_mat44fltMultiply(m, &vPos, &vOut);
    UFont_SetFont(pText->n4);
    fW = 1.0f;
    fH = fW;
    fX = vOut.x / 512.0f;
    vEnd.x = 0.0f;
    vEnd.y = 0.0f;
    vEnd.z = 0.0f;
    vEnd.w = fW;
    // fake match: the factor assigned to a local inside the product loads vOut.y first (a
    // constant operand is put first in fmuls)
    fY = vOut.y * (fInvH = 1.0f / 448.0f);
    if (pText->nFlags & 0x100) {
        vEnd.x = 512.0f * (pText->v18[0] / 512.0f) + 512.0f * (pText->f30 / 512.0f);
    }
    if (pText->nFlags & 0x200) {
        vEnd.y = 448.0f * (pText->v18[1] / 448.0f) + 448.0f * (pText->f34 / 448.0f);
    }
    // EA bug: tests 0x200 twice (0x100 was surely meant), so with only 0x100 set vEndOut is read
    // below without being set
    if ((pText->nFlags & 0x200) || (pText->nFlags & 0x200)) {
        LLMath_mat44fltMultiply(m, &vEnd, &vEndOut);
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
        fX += (vEndOut.x - fX) / 2.0f;
    }
    if (pText->nFlags & 0x10) {
        aColor[0] = (u8)(gpUITextColourMul->r * (pText->aShadowColor[0] + gpUITextColourAdd->r)) / 255.0f;
        aColor[1] = (u8)(gpUITextColourMul->g * (pText->aShadowColor[1] + gpUITextColourAdd->g)) / 255.0f;
        aColor[2] = (u8)(gpUITextColourMul->b * (pText->aShadowColor[2] + gpUITextColourAdd->b)) / 255.0f;
        aColor[3] = t.f5C +
                    (u8)(gpUITextColourMul->a * (pText->aShadowColor[3] + gpUITextColourAdd->a)) / 255.0f;
        if (aColor[3] < 0.0f) {
            aColor[3] = 0.0f;
        }
        if (aColor[3] > 0.5f) {
            aColor[3] = 0.5f;
        }
        UIText_SetFontShadowOffset(pText->f24 / 512.0f, pText->f28 / 512.0f);
        uFlags |= 0x10000;
        UIText_SetFontShadowColour(aColor);
    }
    nColor = pText->n8;
    pTable = gpFrontEnd->p14;
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
    aColor[0] = (u8)(gpUITextColourMul->r * (fR + gpUITextColourAdd->r)) / 255.0f;
    aColor[1] = (u8)(gpUITextColourMul->g * (fG + gpUITextColourAdd->g)) / 255.0f;
    aColor[2] = (u8)(gpUITextColourMul->b * (fB + gpUITextColourAdd->b)) / 255.0f;
    aColor[3] = (u8)(gpUITextColourMul->a * (fA + gpUITextColourAdd->a)) / 512.0f + t.f5C;
    if (aColor[3] < 0.0f) {
        aColor[3] = 0.0f;
    }
    if (aColor[3] > 0.5f) {
        aColor[3] = 0.5f;
    }
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
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
        RenderState_Flush();
    }
}

// The text element's step when its screen loads (message -1 of UIText_ProcessMessage); empty in
// this build.
void UIText_OnScreenLoad(UIText* pText) {
}

// The text element's messages (the UI studio's plugin 7): -1 (its screen loads) does nothing here,
// -2 draws it, -3 is taken and ignored; 0 sets its colour and 1 its shadow's colour (pArgs[0..3],
// red, green, blue, alpha); 3 sets its string (kept as an offset from the element) and 4 answers
// it; 5 its alignment (0 centred, 1 right, else left); 6 its position (pArgs[0..2]); 8 its
// colour-table colour n8 (the low 16 bits of pArgs[0]) and nA (the high 16); 16 / 17 set / answer
// nE; 18 / 19 set its box's width / height (f30, f34) from an integer, 20 / 21 answer them as
// integers.
void UIText_ProcessMessage(UIText* pText, int nMsg, s32 n, MsgArg* pArgs, MsgArg* pResult) {
    u32 uValue;

    switch (nMsg) {
    case -1:
        UIText_OnScreenLoad(pText);
        return;
    case -2:
        UIText_Draw(pText);
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

// Strings are drawn at once from now on, not queued (FO_vSetCurrentAddMode 1); uiProcessInterface.c
// calls it when it resets the UI.
void UIText_SetFontDrawAtOnce(void) {
    FO_vSetCurrentAddMode(1);
}

// Strings are queued again (FO_vSetCurrentAddMode 0); uiProcessInterface.c calls it as the UI
// closes.
void UIText_SetFontDrawQueued(void) {
    FO_vSetCurrentAddMode(0);
}

// The text shadow's colour: pColor packed into the font settings' uC8, with colour mode 0x12 (nC4),
// which draws in that one colour.
void UIText_SetFontShadowColour(f32* pColor) {
    FO_spGetCurrentPacket()->nC4 = 0x12;
    UFont_PackColor(pColor, (u8*)&FO_spGetCurrentPacket()->uC8);
}

// How far the text shadow is moved (the font settings' fCC, fD0): fX across, fY down.
void UIText_SetFontShadowOffset(f32 fX, f32 fY) {
    UFontContext* pCtx;
    pCtx = FO_spGetCurrentPacket();
    pCtx->fCC = fX;
    pCtx->fD0 = fY;
}

// The point text is aligned on with alignment flags 4 and 0x400 (the font settings' fBC, fC0, as
// fractions of the text's size): UIText_Draw sets it to the pivot of a turned text.
void UIText_SetFontAlignPoint(f32 fX, f32 fY) {
    UFontContext* pCtx;
    pCtx = FO_spGetCurrentPacket();
    pCtx->fBC = fX;
    pCtx->fC0 = fY;
}

// The angle text is turned by (the font settings' fB8; 0: not turned).
void UIText_SetFontAngle(f32 fAngle) {
    UFontContext* pCtx;
    pCtx = FO_spGetCurrentPacket();
    pCtx->fB8 = fAngle;
}
