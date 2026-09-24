// ScreenClear.c (our name): draws the screen as one flat colour (black, or the other colour when
// asked) for one to thirty frames, e.g. to blank it between game states.

#include "game_types.h"
#include "engine.h"
#include "terrain.h"

void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_80008380(void);
void fn_800162A8(void);
void fn_800760B0(int x, int y, int nWidth, int nHeight);
void fn_80007254(void);
void fn_800083A0(void);
void fn_80007260(void);

// Only read here (0x802821F0, 8 bytes in .sbss); nothing in the game writes it, so it stays 0.
s32 lbl_802821F0;

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f, 0.0f and 0.5f (0x80284238), before the 0.0f fn_800BA74C uses first; its body is unknown.
static f32 ScreenClear_StrippedFn(f32 x) {
    x += 1.0f;
    if (x < 0.0f) {
        return 0.0f;
    }
    return x * 0.5f;
}

// Draws a black full-screen quad for a number of frames: with bFade 30 frames at alpha 0.1, the
// last two at 0.5; otherwise one frame at alpha 0.5 (two while lbl_802821F0 is not 1).
void fn_800BA74C(u8 bFade) {
    f32 colour[4];
    f32 xy[8];
    u32 bOther;
    int i;
    int nFrames;

    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
    bOther = lbl_802821F0 != 1;
    colour[3] = bFade ? 0.1f : 0.5f;
    fn_80014194(colour);
    fn_800141F8(xy, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80008380();
    if (bFade) {
        nFrames = 30;
    } else {
        nFrames = bOther ? 2 : 1;
    }
    for (i = 0; i < nFrames; i++) {
        if (bFade && i >= nFrames - 2) {
            colour[3] = 0.5f;
            fn_80014194(colour);
        }
        fn_800162A8();
        fn_80006EDC();
        // both cases set the same 512 x 448 area
        if (bOther) {
            fn_800760B0(0, 0, 512, 448);
        } else {
            fn_800760B0(0, 0, 512, 448);
        }
        fn_80035118(4, 5);
        fn_8001425C(0);
        fn_80014118(64);
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(7);
        fn_80012EF8();
        fn_8001644C(0xA1, xy, 0, NULL, 2);
        fn_80013400();
        fn_800A4BDC();
        if (!bOther) {
            fn_80007254();
        }
        fn_800083A0();
        fn_80006FE8();
        fn_80008380();
        if (bOther) {
            fn_80007260();
        }
    }
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(3);
    fn_80012EF8();
    fn_800083A0();
    fn_80008380();
}
