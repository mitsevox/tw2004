// LLDisp_Gc.c (EA's name, from its asserts): the display. Sets up the video mode and the two
// image buffers, keeps the CPU in step with the GPU through FIFO break points, sets the viewport
// and copies each finished frame out.

#include "engine.h"

volatile DispSync lbl_801A2350;       // volatile: the GX and VI callbacks change it
GXRenderModeObj lbl_801A2464;          // the video mode
u8  lbl_80281B8C;                      // cleared when a frame ends, set when the viewport is set
u8  lbl_80281B8D;                      // copy the frame's colour, not only its alpha
u32 lbl_80281B90;                      // one image buffer's size in bytes

u32  VIGetNextField(void);
void fn_80007328(void);
void fn_80007368(void);
void fn_800392D0(void);

void fn_80006DD0(void);
void fn_80006DF4(void);
void fn_80006EC8(void);
void fn_800070DC(void);
s32  fn_8000724C(void);
void fn_80007254(void);
u8   fn_80007258(void);
void fn_80007260(void);

// Clears the frame sync: no break points queued, the GPU not waited on.
void fn_80006DD0(void) {
    lbl_801A2350.bBusy = 0;
    lbl_801A2350.nPending = 0;
    lbl_801A2350.nNext = 0;
    lbl_801A2350.b0D = 0;
    lbl_801A2350.bBreak = 0;
}

// Moves the GPU on to the next queued break point, or lets it run and ends the wait.
void fn_80006DF4(void) {
    void* pBreak;

    if (lbl_801A2350.nPending) {
        pBreak = lbl_801A2350.aBreakPt[lbl_801A2350.nNext++];
        lbl_801A2350.nPending--;
        if (lbl_801A2350.nNext == 3) {
            lbl_801A2350.nNext = 0;
        }
        GXEnableBreakPt(pBreak);
        return;
    }
    GXDisableBreakPt();
    lbl_801A2350.bBusy = 0;
}

// VI's post-retrace callback: after a break point was hit, move on to the next one.
void fn_80006E78(void) {
    if (lbl_801A2350.bBreak) {
        lbl_801A2350.n11++;
        fn_80006DF4();
        lbl_801A2350.bBreak = 0;
    }
}

// GX's break-point callback.
void fn_80006EC8(void) {
    lbl_801A2350.bBreak = 1;
}

// Sets the viewport to the whole frame; in field rendering it jitters by a line each field.
void fn_80006EDC(void) {
    lbl_80281B8C = 1;
    if (lbl_801A2464.field_rendering) {
        GXSetViewportJitter(0.0f, 0.0f, lbl_801A2464.fbWidth, lbl_801A2464.efbHeight, 0.0f, 1.0f,
                            ((lbl_801A2350.n12 - lbl_801A2350.n11) & 1) ^ VIGetNextField());
        lbl_801A2350.n12++;
    } else {
        GXSetViewport(0.0f, 0.0f, lbl_801A2464.fbWidth, lbl_801A2464.efbHeight, 0.0f, 1.0f);
    }
    GXInvalidateVtxCache();
}

// Ends the frame: waits for the last one to be drawn, sets a break point at the end of this one's
// commands and copies the frame into the current image buffer.
void fn_80006FE8(void) {
    void* pRead;
    void* pWrite;

    if (fn_80007258()) {
        lbl_80281B8C = 0;
        while (lbl_801A2350.bBusy) {
        }
        GXSetDrawDone();
        GXFlush();
        GXGetFifoPtrs(GXGetCPUFifo(), &pRead, &pWrite);
        fn_80007368();
        lbl_801A2350.bBusy = 1;
        GXEnableBreakPt(pWrite);
        fn_80007328();
        fn_800392D0();
        GXSetZMode(1, 3, 1);
        GXSetAlphaUpdate(1);
        if (lbl_80281B8D) {
            GXSetColorUpdate(1);
        } else {
            GXSetColorUpdate(0);
        }
        GXCopyDisp(lbl_80281BA4[lbl_801A2350.nBuf], 1);
        GXSetColorUpdate(1);
        GXSetAlphaUpdate(0);
        lbl_80281B88++;
    }
}

// Waits until the GPU has drawn up to the last break point.
void fn_800070DC(void) {
    while (lbl_801A2350.bBusy) {
    }
}

// Clears an image buffer to black (YUV 16, 128), through its uncached address.
void fn_800070F4(void* pBuf) {
    u32* p;
    u32 n;

    p = (u32*)((uptr)pBuf + 0x40000000);
    for (n = lbl_80281B90 / 4; n != 0; n--) {
        *p++ = 0x10801080;
    }
}

// Clears both image buffers and puts them on screen.
void fn_80007160(void) {
    fn_800070F4(lbl_80281BA4[0]);
    if (lbl_80281BA4[0] != lbl_80281BA4[1]) {
        fn_800070F4(lbl_80281BA4[1]);
    }
    GXCopyDisp(lbl_80281BA4[0], 1);
    GXCopyDisp(lbl_80281BA4[1], 1);
}

s32 fn_8000724C(void) {
    return 0;
}

void fn_80007254(void) {
}

u8 fn_80007258(void) {
    return lbl_80281B8C;
}

void fn_80007260(void) {
}
