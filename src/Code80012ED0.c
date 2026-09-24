// Code80012ED0.c (our name; what the file is is not known yet): small render helpers. Getters for
// a render camera's screen rectangle, a call that hands GX the changed renderer state, and three
// setters of the renderer state (lbl_801B8980) that each write one group of fields and set that
// group's bit in u110, like GoTerrain.c's. Its extent is the space between UFont.c and
// Controller_Gc.c; whether it is one original file is not proven.

#include "engine.h"
#include "camera.h"

void fn_80012FA0(void);

f32 fn_80012ED0(f32* pRect) {
    return pRect[3];
}

f32 fn_80012ED8(f32* pRect) {
    return pRect[2];
}

f32 fn_80012EE0(f32* pRect) {
    return pRect[1];
}

f32 fn_80012EE8(f32* pRect) {
    return pRect[0];
}

f32* fn_80012EF0(void* pCamera) {
    return ((RenderCamera*)pCamera)->pRect;
}

void fn_80012EF8(void) {
    fn_80015624();
}

void fn_80012F18(int a) {
    lbl_801B8980.n0 = a;
    lbl_801B8980.u110 |= 0x1;
}

void fn_80012F34(int a) {
    lbl_801B8980.b4 = a;
    lbl_801B8980.u110 |= 0x2;
}

// DepthField.c passes (0 or 1, 6, 0x80). c becomes c * 2 + 1, kept to a byte.
void fn_80012F50(int a, int b, int c) {
    int n;

    n = c * 2 + 1;
    lbl_801B8980.n8 = b;
    lbl_801B8980.bC = (n < 0) ? 0 : ((n <= 0xFF) ? n : 0xFF);
    lbl_801B8980.bD = a;
    lbl_801B8980.u110 |= 0x4;
}

void fn_80012FA0(void) {
}
