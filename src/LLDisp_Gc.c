// LLDisp_Gc.c (EA's name, from its asserts): the display. Sets up the video mode and the image
// buffer (both buffer slots share one), keeps the CPU in step with the GPU through FIFO break
// points, sets the viewport and copies each finished frame out.

#include "engine.h"
#include "game.h"
#include "core/card.h"
#include "discerror.h"
#include "core/startup.h"

// .bss and .sbss are defined in reverse address order (CodeWarrior lays them out last-defined-first).
GXRenderModeObj lbl_801A2464;          // the video mode
GXTexRegion lbl_801A2364[16];          // the texture cache, in 32 KB regions
volatile DispSync lbl_801A2350;       // volatile: the GX and VI callbacks change it
u8  lbl_80281BAC;                      // set once fn_800072E8 has stopped the disc and the audio
void* lbl_80281BA4[2];
struct GXFifoObj* lbl_80281BA0;
u32 lbl_80281B9C;
u32 lbl_80281B98;                      // bits per pixel of the frame: 16 or 24
void* lbl_80281B94;                    // GX's command FIFO memory
u32 lbl_80281B90;                      // one image buffer's size in bytes
u8  lbl_80281B8E;
u8  lbl_80281B8D;                      // copy the frame's colour, not only its alpha
u8  lbl_80281B8C;                      // cleared when a frame ends, set when the viewport is set
s32 lbl_80281B88;

void VIInit(void);                      // SDK
void VIConfigure(GXRenderModeObj* pMode);
void VISetNextFrameBuffer(void* pBuf);
void VISetPostRetraceCallback(void (*pCallback)(u32 nRetrace));
u32  VIGetNextField(void);
int  VIGetTvFormat(void);
void VISetBlack(int bBlack);
void VIFlush(void);
void VIWaitForRetrace(void);
void GXSetBreakPtCallback(void (*pCallback)(void));
void GXSetDispCopyGamma(int eGamma);
int  PADRecalibrate(u32 uMask);
s32  OSResetSystem(s32, s32, s32);
int  OSGetResetButtonState(void);
int  OSEnableInterrupts(void);
int  DVDCancelAll(void);
void AISetStreamVolLeft(u8 uVol);
void AISetStreamVolRight(u8 uVol);
void fn_80006A98(void);
void fn_80006B4C(void);
void fn_80007328(void);
void fn_80007368(void);
void fn_800073A8(void);
void fn_800392D0(void);

void fn_800067E4(GXRenderModeObj* pMode);
void fn_80006DD0(void);
void fn_80006DF4(void);
void fn_80006E78(u32 nRetrace);
void fn_80006EC8(void);
void fn_80007160(void);
void fn_80007264(f32* pGamma);
void fn_800070DC(void);
s32  fn_8000724C(void);
void fn_80007254(void);
u8   fn_80007258(void);
void fn_80007260(void);

// Resets the console with OSResetSystem's arguments, unless a memory card is busy. With
// bOnRelease, only once the reset button has been pressed and let go again.
void fn_800066E4(u8 bOnRelease, s32 nReset, s32 nCode, u8 bMenu) {
    if (CARDGetResultCode(0) == -1 || CARDGetResultCode(1) == -1) return;
    if (bOnRelease) {
        if (lbl_80281B8E) {
            if (OSGetResetButtonState()) return;
        } else {
            lbl_80281B8E = OSGetResetButtonState();
            return;
        }
    }
    fn_800070DC();
    GXDrawDone();
    VISetPostRetraceCallback(NULL);
    VISetBlack(1);
    VIFlush();
    VIWaitForRetrace();
    VISetBlack(1);
    VIFlush();
    VIWaitForRetrace();
    PADRecalibrate(0x80000000);
    PADRecalibrate(0x40000000);
    PADRecalibrate(0x20000000);
    PADRecalibrate(0x10000000);
    OSResetSystem(nReset, nCode, bMenu);
}

// Picks the video mode: pMode if given, else the interlaced mode for the console's TV format,
// 512 pixels wide with 16 lines of overscan (EA's name for it, in its panic message:
// Displ_InitRenderMode).
void fn_800067E4(GXRenderModeObj* pMode) {
    if (pMode != NULL) {
        lbl_801A2464 = *pMode;
        return;
    }
    switch (VIGetTvFormat()) {
    case 0:
        lbl_801A2464 = GXNtsc480IntDf;
        break;
    case 1:
        lbl_801A2464 = GXPal528IntDf;
        break;
    case 2:
        lbl_801A2464 = GXMpal480IntDf;
        break;
    default:
        OSPanic("LLDisp_Gc.c", 250, "Displ_InitRenderMode: Invalid TV format\n");
        break;
    }
    lbl_801A2464.fbWidth = 512;
    GXAdjustForOverscan(&lbl_801A2464, &lbl_801A2464, 0, 16);
}

// Takes the image buffer from the bottom of the arena (both buffer slots share it: YUV, two bytes a
// pixel, rows rounded up to 16 pixels) and gives it to the disc-error screens.
void fn_80006A98(void) {
    void* pLo;
    u32 uSize;
    u32 uArg;

    pLo = OSGetArenaLo();
    OSGetArenaHi();
    uSize = (u16)((lbl_801A2464.fbWidth + 15) & ~15) * lbl_801A2464.xfbHeight * 2;
    lbl_80281BA4[0] = (void*)(((uptr)pLo + 31) & ~31);
    lbl_80281BA4[1] = lbl_80281BA4[0];
    lbl_801A2350.n11 = 0;
    lbl_801A2350.n12 = 0;
    lbl_801A2350.nBuf = 0;
    // fake match: uArg, a copy of uSize for the call, gives EA's mr r30 after the size's store
    uArg = lbl_80281B90 = uSize;
    OSSetArenaLo((void*)(((uptr)lbl_80281BA4[1] + uSize + 31) & ~31));
    fn_800B694C(lbl_801A2464.fbWidth, lbl_801A2464.xfbHeight, uArg);
    fn_800B6C0C(lbl_80281BA4[0]);
}

// Starts GX: its FIFO from the arena, sixteen 32 KB texture cache regions, the viewport, the frame
// copy for the video mode, and the pixel format (16 bits with anti-aliasing, else 24).
void fn_80006B4C(void) {
    GXColor cClear = {0, 0, 0, 0};
    f32 aGamma[4] = {1.0f, 1.0f, 1.0f, 0.0f};
    void* pLo;
    u32 i;

    pLo = OSGetArenaLo();
    OSGetArenaHi();
    lbl_80281B94 = (void*)(((uptr)pLo + 31) & ~31);
    OSSetArenaLo((void*)(((uptr)lbl_80281B94 + 0x64000 + 31) & ~31));
    lbl_80281BA0 = GXInit(lbl_80281B94, 0x64000);
    for (i = 0; i < 16; i++) {
        GXInitTexCacheRegion(&lbl_801A2364[i], 0, i * 0x8000, 0, 0x80000 + i * 0x8000, 0);
    }
    GXSetViewport(0.0f, 0.0f, lbl_801A2464.fbWidth, lbl_801A2464.efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, lbl_801A2464.fbWidth, lbl_801A2464.efbHeight);
    GXSetDispCopySrc(0, 0, lbl_801A2464.fbWidth, lbl_801A2464.efbHeight);
    GXSetDispCopyDst(lbl_801A2464.fbWidth, lbl_801A2464.xfbHeight);
    GXSetDispCopyYScale((f32)lbl_801A2464.xfbHeight / (f32)lbl_801A2464.efbHeight);
    GXSetCopyFilter(lbl_801A2464.aa, lbl_801A2464.sample_pattern, 1, lbl_801A2464.vfilter);
    GXSetZMode(1, 3, 1);
    GXSetColorUpdate(1);
    GXSetCopyClear(cClear, 0xFFFFFF);
    GXSetNumChans(1);
    GXSetCullMode(0);
    if (lbl_801A2464.aa) {
        GXSetDither(1);
        GXSetPixelFmt(2, 0);
        lbl_80281B98 = 16;
    } else {
        GXSetDither(1);
        GXSetPixelFmt(1, 1);
        lbl_80281B98 = 24;
    }
    GXCopyDisp(lbl_80281BA4[lbl_801A2350.nBuf], 1);
    fn_80007264(aGamma);
}

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
void fn_80006E78(u32 nRetrace) {        // nRetrace: VI's retrace count, unused
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

// Starts a frame (the flag fn_80006FE8 checks) and sets the viewport to the whole frame, jittered
// for the field in field rendering.
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

// Clears both image buffers to black, then copies the frame (EFB) into each.
void fn_80007160(void) {
    fn_800070F4(lbl_80281BA4[0]);
    if (lbl_80281BA4[0] != lbl_80281BA4[1]) {
        fn_800070F4(lbl_80281BA4[1]);
    }
    GXCopyDisp(lbl_80281BA4[0], 1);
    GXCopyDisp(lbl_80281BA4[1], 1);
}

// Starts the display: the video mode, the image buffer, GX, the frame sync and its callbacks;
// then, a retrace later (two for some TV modes), clears the image buffers and turns the picture on.
s32 fn_800071BC(void) {
    VIInit();
    fn_800067E4(NULL);
    fn_80006A98();
    VIConfigure(&lbl_801A2464);
    fn_80006B4C();
    fn_80006DD0();
    VISetPostRetraceCallback(fn_80006E78);
    GXSetBreakPtCallback(fn_80006EC8);
    VISetNextFrameBuffer(lbl_80281BA4[0]);
    VIWaitForRetrace();
    if (lbl_801A2464.viTVmode & 1) {
        VIWaitForRetrace();
    }
    fn_80007160();
    VISetBlack(0);
    VIFlush();
    fn_800073A8();
    return 0;
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

// Picks the frame copy's gamma from the average of three values: 1.0 below 1.7, 1.7 up to 2.2,
// else 2.2 (GX's gamma 0, 1, 2).
void fn_80007264(f32* pGamma) {
    int eGamma;
    f32 fAvg;

    eGamma = 0;
    fAvg = (pGamma[2] + (pGamma[0] + pGamma[1])) / 3.0f;
    if (fAvg >= 1.7f && fAvg < 2.2f) {
        eGamma = 1;
    } else if (fAvg >= 2.2f) {
        eGamma = 2;
    }
    GXSetDispCopyGamma(eGamma);
}

s32 fn_800072E0(void) {
    return lbl_80281B98;
}

// Stops the disc and silences the streamed audio, then flags it (fn_80007320).
void fn_800072E8(void) {
    DVDCancelAll();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    lbl_80281BAC = 1;
}

u8 fn_80007320(void) {
    return lbl_80281BAC;
}

// Leaves one level of fn_80007368: interrupts come back on when the last level is left.
void fn_80007328(void) {
    *lbl_80281510 -= 1;
    if (*lbl_80281510 == 0) {
        OSEnableInterrupts();
    }
}

// Turns interrupts off, counting the levels so that calls can nest.
void fn_80007368(void) {
    if (*lbl_80281510 == 0) {
        OSDisableInterrupts();
    }
    *lbl_80281510 += 1;
}

// Resets the FIFO high-water mark (fn_800124CC raises it).
void fn_800073A8(void) {
    lbl_80281B9C = 0;
}
