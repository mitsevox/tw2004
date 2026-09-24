// GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"
#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80009E70();
void fn_800137B0(void* pCamera);    // camera.h
void fn_80012EF8();
void fn_80013EA0();
void fn_80013EEC();
void fn_800137D0(s32 p0);
s32 fn_8000A0E8();
s32 fn_8000A714();
s32 fn_800BADF8();
void fn_80013CCC(u8* arg0);
extern u8* lbl_80280DF0;
void fn_80013D58(s32 p0);
void fn_80013D5C(s32 v);
void RC_vUpdateRenderCtxScreenMatricesAndInfo();
void fn_80013DD0(u8* arg0, f32 (*arg1)[4]);
void fn_80013D68(s32 p0);
void fn_80013D9C(void* pCamera, f32 (*pMtx)[4]);
s32 fn_8000ADC0();
void fn_80013E28(u8* p, s32 v);
void fn_80013E30(u8* p, s32 v);
void fn_80013E38(u8* p, s32 v);
s32 fn_80013E40(u8* p);
f32 fn_80014134(u8* p);
f32 fn_8001413C(u8* p);
f32 fn_80014144(u8* p);
f32 fn_8001414C(u8* p);
f32 fn_80014154(u8* p);
f32 fn_8001415C(u8* p);
f32 fn_80014164(u8* p);
f32 fn_8001416C(u8* p);
f32 fn_80014174(u8* p);
f32 fn_8001417C(u8* p);
f32 fn_80014184(u8* p);
f32 fn_8001418C(u8* p);

void fn_800137B0(void* pCamera) {
    fn_80009E70(pCamera);
}

void fn_800137D0(s32 p0) {
    fn_80013EEC();
    fn_80012EF8();
    fn_80013EA0(p0);
}

void fn_80013CCC(u8* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0x1DC)) != 0) {
        fn_8000A0E8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x11C);
        fn_8000A0E8(arg0 + 0xDC, arg0 + 0x19C);
    } else {
        fn_800BADF8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x1C, arg0 + 0x11C, 4);
        fn_800BADF8(arg0 + 0xDC, arg0 + 0x1C, arg0 + 0x19C, 4);
    }
    fn_8000A714(arg0 + 0x11C, arg0 + 0x15C);
}

void fn_80013D58(s32 p0) {
}

void fn_80013D5C(s32 v) {
    *(s32*)(lbl_80280DF0 + 0x0) = v;
}

void fn_80013D68(s32 p0) {
    RC_vUpdateRenderCtxScreenMatricesAndInfo();
    fn_80013D58(p0);
}

// Gives the camera the view matrix pMtx (NULL: the identity).
void fn_80013D9C(void* pCamera, f32 (*pMtx)[4]) {
    fn_80013DD0(pCamera, pMtx);
    fn_80013CCC(pCamera);
}

void fn_80013DD0(u8* arg0, f32 (*arg1)[4]) {
    if (arg1 == NULL) {
        fn_8000ADC0(arg0 + 0x1C);
        (*(s8*)((u8*)(arg0) + 0x1DC)) = 1;
        return;
    }
    fn_8000A0E8(arg1, arg0 + 0x1C);
    (*(s8*)((u8*)(arg0) + 0x1DC)) = 0;
}

void fn_80013E28(u8* p, s32 v) {
    *(s32*)(p + 0x14) = v;
}

void fn_80013E30(u8* p, s32 v) {
    *(s32*)(p + 0x18) = v;
}

void fn_80013E38(u8* p, s32 v) {
    *(s32*)(p + 0x10) = v;
}

s32 fn_80013E40(u8* p) {
    return *(s32*)(p + 0x18);
}

f32 fn_80014134(u8* p) {
    return *(f32*)(p + 0x14);
}

f32 fn_8001413C(u8* p) {
    return *(f32*)(p + 0x10);
}

f32 fn_80014144(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_8001414C(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_80014154(u8* p) {
    return *(f32*)(p + 0x28);
}

f32 fn_8001415C(u8* p) {
    return *(f32*)(p + 0xC);
}

f32 fn_80014164(u8* p) {
    return *(f32*)(p + 0x4);
}

f32 fn_8001416C(u8* p) {
    return *(f32*)(p + 0x8);
}

f32 fn_80014174(u8* p) {
    return *(f32*)(p + 0x0);
}

f32 fn_8001417C(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_80014184(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_8001418C(u8* p) {
    return *(f32*)(p + 0x28);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

f32 fn_80014268(u8* p);
f32 fn_80014270(u8* p);
f32 fn_80014278(u8* p);
double tan();
f32 fn_80014280(f32 x0);
void fn_800142A4(s8 v);
void fn_800131C4(int nController);
void fn_8001437C(void);

void fn_8001425C(int a) {
    lbl_80280E08->nD0 = a;
}

f32 fn_80014268(u8* p) {
    return *(f32*)(p + 0xAC);
}

f32 fn_80014270(u8* p) {
    return *(f32*)(p + 0xA8);
}

f32 fn_80014278(u8* p) {
    return *(f32*)(p + 0xA4);
}

f32 fn_80014280(f32 x0) {
    f32 t0;
    t0 = tan(x0);
    return t0;
}

void fn_800142A4(s8 v) {
    lbl_80281C98 = v;
}

// A button's mask in the row in use; bShift moves it up 16 bits.
u32 fn_800142AC(int nButton, u8 bShift) {
    if (bShift) {
        return lbl_80186AF0[lbl_80281C98][nButton] << 16;
    }
    return lbl_80186AF0[lbl_80281C98][nButton];
}

void fn_8001437C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_800131C4(var_r31);
        var_r31 += 1;
    } while (var_r31 < 4);
}

// ---- end of sweep code ----

void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c) {
    lbl_801B8980.nE4 = a;
    lbl_801B8980.nE8 = nWidth;
    lbl_801B8980.nEC = nHeight;
    lbl_801B8980.nF0 = nField;
    lbl_801B8980.nF4 = b;
    lbl_801B8980.nF8 = c;
    lbl_801B8980.u110 |= 0x1000;
}

void fn_80014118(int a) {
    lbl_801B8980.u20 = a;
    lbl_801B8980.u110 |= 0x20;
}

// Set the colour of the view's vertices (r, g, b; alpha is kept); NULL: the default grey.
void fn_80014194(f32* pColour) {
    if (pColour == NULL) {
        fn_800141CC();
        return;
    }
    Vec_Copy(pColour, lbl_80280E08->aColour);
}

// The default vertex colour: half grey, opaque.
void fn_800141CC(void) {
    lbl_80280E08->aColour[0] = 0.5f;
    lbl_80280E08->aColour[1] = 0.5f;
    lbl_80280E08->aColour[2] = 0.5f;
    lbl_80280E08->aColour[3] = 1.0f;
}

// Whether any of the four pads has any of the buttons in uMask (0: any button at all).
u8 fn_80014300(u32 uMask) {
    u8 bPressed = 0;
    int nController = 0;

    do {
        if ((uMask == 0 && fn_800136DC(nController) != 0) || (uMask & fn_800136DC(nController))) {
            bPressed = 1;
        }
        nController++;
    } while (nController < 4);
    return bPressed;
}

// Fill a screen quad's two corners (x0, y0)-(x1, y1) and its texture coordinates (0,0)-(1,1),
// four floats per vertex.
void fn_800141F8(f32* pXY, f32* pUV, f32 x0, f32 y0, f32 x1, f32 y1) {
    if (pXY != NULL) {
        pXY[0] = x0;
        pXY[1] = y0;
        pXY[2] = 0.0f;
        pXY[3] = 1.0f;
        pXY[4] = x1;
        pXY[5] = y1;
        pXY[6] = 0.0f;
        pXY[7] = 1.0f;
    }
    if (pUV != NULL) {
        pUV[0] = 0.0f;
        pUV[1] = 0.0f;
        pUV[2] = 0.0f;
        pUV[3] = 1.0f;
        pUV[4] = 1.0f;
        pUV[5] = 1.0f;
        pUV[6] = 0.0f;
        pUV[7] = 1.0f;
    }
}
