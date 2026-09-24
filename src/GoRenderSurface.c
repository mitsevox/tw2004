// GoRenderSurface.c (EA's name, from its asserts): the render surfaces, five slots of off-screen
// buffers (RenderSurface, lbl_801D3950). Surface 0 is made at start-up for the 512 x 448 screen;
// the others get a buffer of their own. The renderer selects one, with which of the colour and
// alpha channels are written, through fn_8002F38C.

#include "engine.h"
#include "gx.h"

RenderSurface lbl_801D3950[5];

s32 lbl_80281D50;

void fn_8002F32C(s32 nSurface);
s32  fn_8002F480(s32 nSurface);

// Clears every slot and makes surface 0, the screen.
void fn_8002F1D4(void) {
    int i;

    for (i = 0; i < 5; i++) {
        fn_80005AE8(&lbl_801D3950[i], 0, sizeof(RenderSurface));
        lbl_801D3950[i].n0 = 0;
    }
    lbl_80281D50 = 0;
    fn_8002F260(1, 512, 448, 1, 8, 0);
}

// Fills in slot nSurface and, unless n0 is 1, gives it a buffer. Returns 0 if there was no memory.
int fn_8002F260(s32 n0, s32 nWidth, s32 nHeight, s32 nKind, s32 n20, s32 nSurface) {
    lbl_801D3950[nSurface].n0 = n0;
    lbl_801D3950[nSurface].nWidth = nWidth;
    lbl_801D3950[nSurface].nHeight = nHeight;
    lbl_801D3950[nSurface].nC = nWidth;
    lbl_801D3950[nSurface].n10 = nHeight;
    lbl_801D3950[nSurface].n14 = 0;
    lbl_801D3950[nSurface].n18 = nKind;
    lbl_801D3950[nSurface].n1C = nKind;
    lbl_801D3950[nSurface].n20 = n20;
    lbl_801D3950[nSurface].nSize = fn_8002F480(nSurface);
    if (n0 != 1) {
        lbl_801D3950[nSurface].pBuffer =
            fn_80009B34(lbl_801D3950[nSurface].nSize, 2, 32, "GoRenderSurface.c", 198);
        // port: EA tested the pointer as a signed integer (cmpwi); a port tests it against NULL
        if ((s32)lbl_801D3950[nSurface].pBuffer == 0) {
            return 0;
        }
    }
    return 1;
}

// Frees slot nSurface's buffer and clears the slot.
void fn_8002F32C(s32 nSurface) {
    fn_80009E70(lbl_801D3950[nSurface].pBuffer);
    fn_80005AE8(&lbl_801D3950[nSurface], 0, sizeof(RenderSurface));
    lbl_801D3950[nSurface].n0 = 0;
}

// Selects surface nSurface. The lowest set bit of uFlags picks the channels drawing writes:
// 1 both, 2 neither, 4 alpha only, 8 colour only.
void fn_8002F38C(s32 nSurface, s32 nC, s32 n10, s32 n14, u32 uFlags, s32 n18) {
    lbl_80281D50 = nSurface;
    if (uFlags & 1) {
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(1);
    } else if (uFlags & 2) {
        GXSetAlphaUpdate(0);
        GXSetColorUpdate(0);
    } else if (uFlags & 4) {
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(0);
    } else if (uFlags & 8) {
        GXSetAlphaUpdate(0);
        GXSetColorUpdate(1);
    }
    lbl_801D3950[nSurface].nC = nC;
    lbl_801D3950[nSurface].n10 = n10;
    lbl_801D3950[nSurface].n14 = n14;
    lbl_801D3950[nSurface].n18 = n18;
}

s32 fn_8002F454(s32 nSurface) {
    if (lbl_801D3950[nSurface].n0 == 0) {
        return 0;
    }
    return lbl_801D3950[nSurface].nSize;
}

// The size of slot nSurface's buffer in bytes, from its size and pixel kind.
s32 fn_8002F480(s32 nSurface) {
    s32 nBytes = 0;

    if (lbl_801D3950[nSurface].n1C == 1) {
        nBytes = 4;
    } else if (lbl_801D3950[nSurface].n1C == 2) {
        nBytes = 4;
    } else if (lbl_801D3950[nSurface].n1C == 4) {
        nBytes = 2;
    } else if (lbl_801D3950[nSurface].n1C == 8 || lbl_801D3950[nSurface].n1C == 16) {
        nBytes = 1;
    }
    return nBytes * (lbl_801D3950[nSurface].nWidth * lbl_801D3950[nSurface].nHeight);
}
