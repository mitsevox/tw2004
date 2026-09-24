// GameAudio.c (our name): the game's side of the sound engine. fn_800A3E3C starts the engine
// (memory stack, ARAM, sound table, movie sound, banks) one step after another; the rest drives
// the sound emitters (hlaudemitter.c's fn_800AD*) from the game: the course, the game mode and the
// pin set. Its extent is proven by its data: every section starts and ends on 8-byte boundaries
// shared with no other file (.rodata 0x80183AD8-0x80183B08, .data 0x8018E988-0x8018EB30,
// .bss 0x801F1708-0x801F17D0, .sdata 0x80281418-0x80281460, .sbss 0x80282010-0x80282058,
// .sdata2 0x80283F48-0x80283F88), and all its functions share those globals.

#include "core/gameaudio.h"
#include "core/audtrack.h"

// hlaudmovie.c
void fn_800A874C(s32 n);
void fn_800A87B4(u8 a, u8 n);
void fn_800A8F68(u8 b);
void fn_800A8700(u8 n);
void fn_800A8708(u8 nCurve, f32 fVolume);
void fn_800A871C(u8 nCurves, f32* pVolumes);

// hlaudemitter.c
void fn_800AD698(u8 nId, u8 nTrack, u8 bOn);
u8   fn_800AD618(u8 nId, u8 nTrack);
void fn_800AD790(u8 nId, u8 nTrack, u32 uParams);
void fn_800AD9AC(u8 nId, u8 nTrack, u8 n);
void fn_800AD800(u8 nId, f32* pPos, f32* pLast, u8 b);
void fn_800ADAF0(u8 nId, u8 nTrack, f32 fPitch);
void fn_800ADBC0(s16 nKind, f32* pPos, f32* pLast, u8 b);  // types unproven

f32  fn_8006E118(u64 tEnd, u64 tStart);    // GameManager.c: seconds between two time stamps
void fn_8010D3D8(int nPlayer);
u8   fn_8006BEA4(void);                    // GoGolfCam.c: the GameBreaker letterbox is up
void fn_800ADA94(u8 nId, u8 nTrack, f32 fVolume);

void fn_800DC6E8(int nPlayer);
u8   fn_8006BAD8(int nPlayer, s32* pOut);
f32  fn_8006C630(void);
// hlaudemitter.c: makes an emitter for sound nSound; the callback is told when a track stops
// (fn_800ADDC8). The other types are unproven.
u8   fn_800AD280(s16 nSound, s16 nKind, int a, int b, void (*pfnCallback)(u8 nId, u8 nTrack, s32 n));
void fn_800ADA08(s16 nSound, u8 nTrack, u8 n);
void fn_800AD734(u8 nId, int n);
void fn_8001EB8C(Character* pChar, int nBone, f32* pPos);
void fn_800A6BA8(u8 nPlayer);
void fn_800A6854(u8 nPlayer);
void fn_800A6D48(u8 nPlayer);
void fn_800A714C(void);
void fn_800A71E4(void);
void fn_800A7294(void);
void fn_800BA734(int n, s8 nTrack);

void fn_800A3F38(u8 b, u8 b2);
void fn_800A3FB4(u8 nCurve, f32 fVolume);
void fn_800A3FD4(u8 nCurves, f32* pVolumes);
void fn_800A4084(void);
void fn_800A41A4(void);
void fn_800A43DC(void);
void fn_800A47A0(void);
void fn_800A484C(void);
void fn_800A4928(void);
void fn_800A42B0(u8 n);
void fn_800A7220(f32 fAmount);
void fn_800A3F58(u8 bLow, u8 bHigh);
void fn_800A3F94(u8 n);
void fn_800A4044(u8 nIndex, u8 nValue);
void fn_800A7AD0(s16 nSound, u8 nTrack, u8 bOn);
void fn_800A4170(u8 nId, u8 nTrack, s32 n);
void fn_800A5980(u8 nPlayer);
void fn_800A6C98(u8 nPlayer, u8 n);
void fn_800A6660(u8 nPlayer);
void fn_800AD950(u8 nId, u8 nTrack, u8 n);
u8   fn_800A4A24(s32 nCourse, int n);
u8   fn_800A4A88(void);
void fn_800A7968(u8 nId, u8 nTrack, u8 a, u16 b, s32 c);
u8   fn_800A7720(void);
u8   fn_800A7748(void);
void fn_800A70E4(int n);
void fn_800A7198(int n);

// Each volume curve's volume (fn_800A4A88 hands them to hlaudmovie.c).
f32 lbl_8018E988[32] = {
    0.3f, 0.7f, 0.8f, 1.0f, 0.7f, 0.4f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.4f,
    1.5f, 1.5f, 1.5f, 1.4f, 1.0f, 1.0f, 0.4f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

GameAudioCourseSound lbl_8018EA08[9] = {
    { 12, 3, 0 },
    { 12, 4, 0 },
    { 12, 9, 0 },
    { 12, 10, 0 },
    { 12, 11, 0 },
    { 15, 1, 1 },
    { 15, 18, 1 },
    { 18, 2, 2 },
    { 18, 18, 2 },
};

// fn_800A5980: the swing sound per club (Player.nClub) and per lie (Ball.nLie).
const u8 lbl_80183AD8[28] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0,
};
const u8 lbl_80183AF4[20] = {
    0, 0, 0, 1, 2, 3, 4, 4, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GameAudioView lbl_801F1790[2];

u8 lbl_80281418 = 0xFF;
u8 lbl_80281419 = 0xFF;
u8 lbl_8028141A = 0xFF;
u8 lbl_8028141B = 0xFF;
u8 lbl_8028141C = 0xFF;
u8 lbl_8028141D = 0xFF;
u8 lbl_8028141E = 0xFF;
u8 lbl_8028141F = 0xFF;
u8 lbl_80281420 = 0xFF;
s32 lbl_80281424 = -1;
s32 lbl_80281428 = -1;
u8 lbl_8028142C = 0xFF;
u8 lbl_8028142D = 0xFF;
f32 lbl_80281430 = 1.0f;
f32 lbl_80281434 = 0.6f;
f32 lbl_80281438 = 4.0f;
f32 lbl_8028143C = 4.0f;
f32 lbl_80281440 = 0.8f;
f32 lbl_80281444 = 0.75f;
f32 lbl_80281448 = 1.5f;
f32 lbl_8028144C = 0.03f;
f32 lbl_80281450 = 25.0f;
f32 lbl_80281454 = 2.0f;
f32 lbl_80281458 = 21.0f;

u32 lbl_80282054;                       // frames left before the queued sound starts (fn_800A4BDC)
u8 lbl_80282052;                        // } the queued sound: fn_800A7968's arguments
u16 lbl_80282050;                       // }
s32 lbl_8028204C;                       // }
u32 lbl_80282048;                       // fn_800A6070: the frame it last played
f32 lbl_80282044;
u8 lbl_80282042;
u8 lbl_80282041;
u8 lbl_80282040;
s32 lbl_8028203C;                       // what fn_800A44A0 plays: 0 nothing, 1 music, 2 ambience
u8 lbl_80282038;                        // a sound is queued
s32 lbl_80282034;
u8 lbl_80282033;
u8 lbl_80282032;
u8 lbl_80282031;
u8 lbl_80282030;
u8 lbl_8028202F;
u8 lbl_8028202E;
u8 lbl_8028202D;
u8 lbl_8028202C;
u8 lbl_8028202B;
u8 lbl_8028202A;
u8 lbl_80282029;
u8 lbl_80282028;
u8 lbl_80282024[2];
u8 lbl_80282020;

// startUp.c: the sound engine's start-up steps, each nonzero when it worked
u8   fn_800AFAB0(void);
u8   fn_800B0438(void);
u8   fn_800B0568(void);
u8   fn_800B0798(void);
void fn_800B07A0(void);
void fn_800B0858(u8 nSound);

// hlaudmovie.c
u8   fn_800A8604(void);
void fn_800A86BC(u8 nRate);
u8   fn_800A8754(void);
u8   fn_800A8824(void);
u8   fn_800A8D2C(void);
void fn_800A8D88(void);
u8   fn_800A8DC8(u8 a, u8 b, u8 nListeners);
void Mov_Init(void);
void Mov_Exit(void);
void Mov_Start(void);
void Mov_Tick(void);

u8   fn_800AC470(void);                   // hlaudvoice.c
u8   fn_800ACECC(void);                   // no C yet; returns 1
u8   fn_800AF224(void);                   // no C yet; returns 1
void fn_800AD1C8(void);                   // hlaudemitter.c, no C yet
void fn_800AD1C4(void);                   // hlaudemitter.c
void fn_800B5B80(void);                   // UAudMemStack.c
void fn_800ADA28(u8 nId, u8 nTrack, u8 n, int bCheck);

u8   fn_800A3FF4(void);
void fn_800A75B4(void);

// Starts the sound engine one step after another; stops at the first step that fails and
// returns 0, else 1.
u8 fn_800A3E3C(u8 nRate) {
    u8 bOk;

    fn_800B5B80();
    if ((bOk = fn_800AFAB0()) && (bOk = fn_800B0438()) && (bOk = fn_800B0568())
        && (bOk = fn_800AF224()) && (bOk = fn_800B0798()) && (bOk = fn_800A8604())
        && (bOk = fn_800A8D2C()) && (bOk = fn_800A7AF0()) && (bOk = fn_800A98B4())
        && (bOk = fn_800AAD18()) && (bOk = fn_800ABBC8()) && (bOk = fn_800A8754())
        && (bOk = fn_800AC470()) && (bOk = fn_800A8824()) && (bOk = fn_800ACECC())
        && (bOk = fn_800A3FF4())) {
        fn_800B07A0();
        fn_800A86BC(nRate);
        bOk = 1;
    }
    return bOk;
}

s32 fn_800A402C(u8 n) {
    lbl_80282020 = n;
    return 1;
}

void fn_800A4038(void) {
    lbl_80282020 = 0;
}

void fn_800A4080(void) {
}

void fn_800A4374(void) {
    if (lbl_80282040) {
        fn_800AD698(lbl_8028141C, 2, 0);
        fn_800AD698(lbl_8028141D, 2, 0);
        fn_800AD698(lbl_8028141C, 3, 0);
        fn_800AD698(lbl_8028141D, 3, 0);
    }
}

// Stops the ambience's tracks; track 0 too unless bKeepFirst.
void fn_800A49A4(u8 bKeepFirst) {
    if (lbl_8028203C == 2) {
        if (!bKeepFirst) {
            fn_800AD698(lbl_8028141A, 0, 0);
        }
        fn_800AD698(lbl_8028141A, 1, 0);
        fn_800AD698(lbl_8028141A, 2, 0);
        fn_800AD698(lbl_8028141A, 3, 0);
        fn_800AD698(lbl_8028141A, 4, 0);
    }
}

int fn_800A4BAC(void) {
    return fn_800A4A88() != 0;
}

// Once a frame: the emitters, then the queued sound once its wait runs out.
void fn_800A4BDC(void) {
    fn_800AD1C8();
    fn_800A4080();
    if (lbl_80282054 != 0) {
        if (--lbl_80282054 == 0 && lbl_80282038) {
            fn_800A7968(lbl_80281419, 0, lbl_80282052, lbl_80282050, lbl_8028204C);
            fn_800AD698(lbl_80281419, 0, 1);
            lbl_80282038 = 0;
        }
    }
}

void fn_800A4C54(void) {
    if (lbl_8028202A) {
        lbl_8028202A = 0;
        fn_800A4084();
    }
    if (lbl_80282029) {
        if (lbl_8028202C) {
            fn_800A44A0();
            lbl_8028202C = 0;
        }
        fn_800A43DC();
        fn_800A41A4();
        fn_800A4928();
    }
}

void fn_800A4FD8(void) {
    fn_800A75B4();
    lbl_8028141B = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_80282029 = 0;
}

void fn_800A5620(void) {
    lbl_8028202D = 1;
}

void fn_800A5E94(u8 nPlayer) {
    fn_8006BAA8(nPlayer);
    fn_800A4374();
    fn_800A707C();
}

void fn_800A624C(void) {
    fn_800ADB4C(3, 1, 1);
}

void fn_800A6278(void) {
    fn_800ADB4C(3, 1, 0);
}

void fn_800A62A4(void) {
    fn_800ADC44(3, 0, 0);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A62E0(void) {
    fn_800ADC44(3, 0, 1);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A631C(void) {
    fn_800ADC44(3, 0, 2);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6358(void) {
    fn_800ADC44(3, 0, 3);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6394(void) {
    fn_800ADC44(3, 0, 4);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A63D0(void) {
    fn_800ADC44(3, 0, 6);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A640C(void) {
    fn_800ADC44(3, 0, 7);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6448(void) {
}

void fn_800A644C(void) {
}

void fn_800A6EC8(void) {
    if (lbl_80282040) {
        fn_800A4374();
        fn_800A707C();
        fn_800AD698(lbl_8028141C, 0, 0);
        fn_800AD698(lbl_8028141C, 1, 0);
        fn_800AD698(lbl_8028141D, 0, 0);
        fn_800AD698(lbl_8028141D, 1, 0);
    }
}

void fn_800A6F38(void) {
    if (lbl_80282040) {
        fn_800AD9AC(lbl_8028141C, 0, 2);
        fn_800AD9AC(lbl_8028141C, 1, 2);
        fn_800AD9AC(lbl_8028141D, 0, 2);
        fn_800AD9AC(lbl_8028141D, 1, 2);
        fn_800AD698(lbl_8028141C, 0, 1);
        fn_800AD698(lbl_8028141C, 1, 1);
        fn_800AD698(lbl_8028141D, 0, 1);
        fn_800AD698(lbl_8028141D, 1, 1);
    }
}

void fn_800A707C(void) {
    if (lbl_80282040) {
        fn_800AD698(lbl_8028141C, 4, 0);
        fn_800AD698(lbl_8028141D, 4, 0);
        fn_800AD698(lbl_8028141C, 5, 0);
        fn_800AD698(lbl_8028141D, 5, 0);
    }
}

// Switches the game's sounds off (bOff) or back on; bMusic restarts the music too.
void fn_800A72EC(u8 bOff, u8 bMusic) {
    if (lbl_8028202E ^ bOff) {
        lbl_8028202E = bOff;
        if (bOff) {
            fn_800A49A4(0);
            fn_800A75B4();
            fn_800A6EC8();
            fn_800A6660(0);
            return;
        }
        if (bMusic) {
            fn_800A44A0();
            fn_800A47A0();
        }
    }
}

// a is unused.
void fn_800A73C0(u8 a, int n) {
    fn_800ADA28(lbl_8028141B, 0, n, 0);
}

void fn_800A73F0(s32 n) {
    switch (n) {
    case 1:
        fn_800ADA28(lbl_8028141B, 1, 0, 0);
        break;
    case 6:
        fn_800ADA28(lbl_8028141B, 1, 1, 0);
        break;
    default:
        fn_800ADA28(lbl_8028141B, 0, n, 0);
        break;
    }
}

s32 fn_800A7528(void) {
    return fn_800A7720();
}

void fn_800A75B4(void) {
    if (lbl_8028203C == 1) {
        fn_800AD698(lbl_80281418, 0, 0);
        lbl_8028203C = 0;
    }
}

u8 fn_800A75F4(void) {
    int bResult;

    bResult = 0;
    if (lbl_8028203C == 1 && fn_800AD618(lbl_80281418, 0)) {
        bResult = 1;
    }
    return bResult;
}

void fn_800A7644(void) {
    fn_800A44A0();
}

void fn_800A76E4(void) {
    fn_800AD698(lbl_80281419, 0, 0);
    lbl_80282054 = 15;
    lbl_80282038 = 0;
}

u8 fn_800A7720(void) {
    return fn_800AD618(lbl_80281419, 0);
}

void fn_800A7924(f32 f) {
    fn_800A44A0();
}

void fn_800A7944(void) {
    fn_800A44A0();
    fn_800A47A0();
}

void fn_800A7994(void) {
    Mov_Init();
}

void fn_800A79B4(void) {
    Mov_Exit();
}

void fn_800A79D4(void) {
    Mov_Start();
}

void fn_800A79F4(void) {
    Mov_Tick();
}

void fn_800A7A14(u8 nSound) {
    fn_800B0858(nSound);
}

// Every caller passes a fourth argument; nothing here reads it.
s32 fn_800A7A34(u8 a, u8 b, u8 nListeners, int nUnused) {
    fn_800AD0C4();
    fn_800A402C(nListeners);
    // port: EA passes an argument fn_800A8DC8 ignores
    ((u8 (*)(u8, u8, u8, int))fn_800A8DC8)(a, b, nListeners, 0);
    return 1;
}

void fn_800A7A98(s32 n) {
    fn_800AD1C4();
    fn_800A4038();
    // port: EA passes an argument fn_800A8D88 ignores
    ((void (*)(s32))fn_800A8D88)(n);
}


// Every caller passes a second flag (DiscError.c 1, this file 0); nothing here reads it.
void fn_800A3F38(u8 b, u8 b2) {
    fn_800A8F68(b);
}

void fn_800A3F58(u8 bLow, u8 bHigh) {
    s32 nMask;

    nMask = 0;
    if (bLow) {
        nMask |= 0xFFFF;
    }
    if (bHigh) {
        nMask |= 0xFFFF0000;
    }
    fn_800A874C(nMask);
}

void fn_800A3F94(u8 n) {
    fn_800A8700(n);
}

void fn_800A3FB4(u8 nCurve, f32 fVolume) {
    fn_800A8708(nCurve, fVolume);
}

void fn_800A3FD4(u8 nCurves, f32* pVolumes) {
    fn_800A871C(nCurves, pVolumes);
}

u8 fn_800A3FF4(void) {
    fn_80005AE8(lbl_80282024, 0, sizeof(lbl_80282024));
    lbl_80282020 = 0;
    return 1;
}

void fn_800A4044(u8 nIndex, u8 nValue) {
    if (lbl_80282024[nIndex] != nValue) {
        lbl_80282024[nIndex] = nValue;
        fn_800A87B4(nIndex, nValue);
    }
}

// An emitter callback (hlaudemitter.c's pfnCallback shape): when track 2 stops, tells GameEffects.c.
void fn_800A4170(u8 nId, u8 nTrack, s32 n) {
    if (nTrack == 2) {
        fn_800DC6E8(fn_8001707C(0));
    }
}

void fn_800A42B0(u8 n) {
    if (lbl_8028202F || !lbl_80282040) return;
    fn_800AD9AC(lbl_8028141C, 2, n);
    fn_800AD9AC(lbl_8028141D, 2, n);
    fn_800AD9AC(lbl_8028141C, 3, n);
    fn_800AD9AC(lbl_8028141D, 3, n);
    fn_800AD698(lbl_8028141C, 2, 1);
    fn_800AD698(lbl_8028141D, 2, 1);
    fn_800AD698(lbl_8028141C, 3, 1);
    fn_800AD698(lbl_8028141D, 3, 1);
}

void fn_800A4928(void) {
    switch (lbl_8028203C) {
    case 2:
        if (lbl_80282041 != 0 && lbl_8028202D == 0 && !fn_800A7748()) {
            lbl_80282041 = 0;
            fn_800A484C();
        }
        break;
    case 1:
        if (!fn_800A75F4()) {
            fn_800A47A0();
        }
        break;
    }
}

// The sound for a course and n (fn_800A484C passes fn_80015464()), or 0xFF when it has none.
u8 fn_800A4A24(s32 nCourse, int n) {
    u8 nSound;
    int i;

    nSound = 0xFF;
    for (i = 0; i < 9; i++) {
        if (nCourse == lbl_8018EA08[i].nCourse && (u8)(n + 1) == lbl_8018EA08[i].n4) {
            nSound = lbl_8018EA08[i].nSound;
            break;
        }
    }
    return nSound;
}

// Starts the course's sounds (not in modes 0, 1 and 3): the wind and the pin's emitter.
void fn_800A4084(void) {
    u8 nWind;

    if (gSession.nGameType != 3 && gSession.nGameType != 1 && gSession.nGameType != 0) {
        fn_80015464();
        nWind = gSession.options.nWind;
        fn_800A6DCC(0, 0);
        fn_800A70E4(nWind);
        fn_800A7198(nWind);
        if (fn_80035574()) {
            fn_800A7220(fn_8006C630());
        }
        lbl_80281420 = fn_800AD280(9, -1, 1, 0, NULL);
        fn_800AD800(lbl_80281420, &gPlayers[0].ball.pCourse->pin[Game_CurrentPinSet()].x, NULL, 0);
        fn_800ADA08(9, 0, nWind);
    }
    lbl_80282029 = 1;
}

void fn_800A41A4(void) {
    if (fn_800A7720()) {
        if (lbl_80282031 == 0) {
            if (lbl_8028203C == 1) {
                fn_800A3FB4(15, lbl_8018E988[15] * (0.2f * (s8)gSession.options.a0[1] * lbl_80281434));
            }
            lbl_80282031 = 1;
        }
    } else if (lbl_80282031 != 0) {
        if (lbl_8028203C == 1) {
            fn_800A3FB4(15, 0.2f * (s8)gSession.options.a0[1] * lbl_8018E988[15]);
        }
        lbl_80282031 = 0;
    }
}

void fn_800A43DC(void) {
    s32 n;

    if (lbl_80282040 && lbl_80282033 && !lbl_80282030 && !lbl_8028202F && !lbl_80282032
        && lbl_80281428 == -1) {
        if (fn_8006BAD8(lbl_80282278, &n)) {
            fn_800A6DCC((u8)(n + 4), 1);
            lbl_80282030 = 1;
        } else if (n >= 0) {
            if (--n < 0) {
                n = 0;
            }
            fn_800A42B0(n);
        }
    }
}

// Applies the sound options: the volumes, then the music. Game types 3 and 10 play music 2 on
// row 0; otherwise music 6 plays (two players at most, not in a replay) on the row the game mode
// picks, if that row is switched on. Without music the ambience (sound 8) plays instead.
void fn_800A44A0(void) {
    int nMode;
    f32 fVolume;
    u8 bFixed;
    u8 bNoBreaker;
    s32 nState;
    s16 nSound;
    u8 bOn;
    u8 bMusic;
    u8 nRow;
    f32 vPos[3];

    nMode = Game_GetMode();
    fVolume = 0.2f * (s8)gSession.options.a0[1];
    bFixed = gSession.nGameType == 3 || gSession.nGameType == 10;
    bNoBreaker = !bFixed && gSession.a8[0];
    nState = 0;
    bOn = fVolume > 0.0f;
    fn_800A3FB4(15, fVolume * lbl_8018E988[15]);
    fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
    if (bFixed) {
        nSound = 2;
        bMusic = 1;
        nRow = 0;
    } else {
        nSound = 6;
        bMusic = gSession.nNumPlayers <= 2 && !gSession.bReplay;
        bOn = bOn && bMusic;
        switch (nMode) {
        case 0:
        case 1:
        case 2:
        case 9:
        case 18:
        case 19:
        case 20:
        case 21:
            nRow = 1;
            break;
        case 6:
        case 7:
        case 8:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 22:
        case 25:
        case 26:
            nRow = 2;
            break;
        default:
            nRow = 3;
            break;
        }
    }
    if (!bNoBreaker || (gSession.uFlags & 0x4000)) {
        bOn = bOn && gSession.options.abRowOn[nRow];
    }
    if (bOn) {
        if (lbl_8028203C == 2) {
            fn_800AD450(lbl_8028141A);
            lbl_8028141A = 0xFF;
        }
        if (bMusic) {
            nState = 1;
            if (lbl_80281418 == 0xFF) {
                lbl_80281418 = fn_800AD280(nSound, -1, 1, 1, NULL);
            }
            lbl_8028142C = nRow;
        }
    } else {
        if (lbl_8028203C == 1) {
            fn_800AD450(lbl_80281418);
            lbl_80281418 = 0xFF;
        }
        if (bMusic && !bFixed && gpGame->b288 && (s8)gSession.options.a0[0] > 0) {
            nState = 2;
            if (lbl_8028141A == 0xFF) {
                vPos[0] = 0.0f;
                vPos[1] = 0.0f;
                vPos[2] = 12.0f;
                lbl_8028141A = fn_800AD280(8, -1, 1, 1, NULL);
                fn_800AD800(lbl_8028141A, vPos, NULL, 0);
            }
            if (lbl_8028203C != 2) {
                lbl_80282041 = 1;
            }
        }
    }
    lbl_8028203C = nState;
}

void fn_800A484C(void) {
    int nCourse;
    u8 n;
    u8 nSound;

    if (lbl_8028203C == 2) {
        nCourse = Game_GetCourse();
        n = fn_80015464();
        fn_800ADA94(lbl_8028141A, 0, 1.0f);
        fn_800AD698(lbl_8028141A, 1, 1);
        if (fn_80035574()) {
            fn_800AD698(lbl_8028141A, 2, 1);
        }
        fn_800AD9AC(lbl_8028141A, 3, nCourse);
        fn_800AD698(lbl_8028141A, 3, 1);
        nSound = fn_800A4A24(nCourse, n);
        if (nSound != 0xFF) {
            fn_800AD9AC(lbl_8028141A, 4, nSound);
            fn_800AD698(lbl_8028141A, 4, 1);
        }
    }
}

// Steps to the next track switched on in the options' row lbl_8028142C (19 tracks) and plays it.
void fn_800A47A0(void) {
    u8 i;
    int nCount = 0;

    if (lbl_8028203C == 1) {
        // fake match: the original keeps an empty 19-step loop here; a count nothing reads
        // reproduces it (what EA's loop did is lost)
        for (i = 0; i < 19; i++) {
            if (gSession.options.rows[lbl_8028142C][i]) {
                nCount++;
            }
        }
        for (i = 0; i < 19; i++) {
            if (++lbl_8028142D >= 19) {
                lbl_8028142D = 0;
            }
            if (gSession.options.rows[lbl_8028142C][lbl_8028142D]) {
                fn_800A754C(13, lbl_8028142D);
                return;
            }
        }
    }
}

u8 fn_800A4A88(void) {
    int i;

    for (i = 0; i < 2; i++) {
        lbl_801F1790[i].n0 = 0xFF;
        lbl_801F1790[i].n1 = 0xFF;
        lbl_801F1790[i].n2 = 0xFF;
        lbl_801F1790[i].n3 = 0xFF;
        lbl_801F1790[i].n4 = 0;
        lbl_801F1790[i].n5 = 0;
        lbl_801F1790[i].f8 = 0.0f;
        lbl_801F1790[i].fC = 0.0f;
        lbl_801F1790[i].tLast = 0;
    }
    lbl_80281418 = 0xFF;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_8028141B = 0xFF;
    lbl_8028141C = 0xFF;
    lbl_8028141D = 0xFF;
    lbl_8028141E = 0xFF;
    lbl_8028141F = 0xFF;
    lbl_80281420 = 0xFF;
    lbl_80282028 = 0;
    lbl_8028202A = 1;
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202D = 0;
    lbl_8028202E = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    lbl_8028203C = 0;
    lbl_80282040 = 0;
    lbl_80282041 = 0;
    lbl_80281430 = 1.0f;
    lbl_80281434 = 0.6f;
    lbl_80281458 = 21.0f;
    fn_800A3E3C(60);
    fn_800A3FD4(32, lbl_8018E988);
    return 1;
}

// Starts a world object's sound. Kinds 0, 3 and 5 play as a pair of emitters lbl_80281450 either
// side of the listener (kind 0 only once, lbl_80282042); the others play at the object. Kinds
// below 3 need gpGame->b288.
void fn_800A4CB8(GameAudioSource** ppSource, int n) {
    GameAudioSource* pSource;
    u32 nKind;
    u8 bPlay;
    u8 nParam;
    u8 nId;
    f32 vPos[3];

    pSource = *ppSource;
    nKind = pSource->nKind;
    if ((nKind >= 3 || gpGame->b288) && pSource->nSound != 0) {
        nParam = n;
        switch (nKind) {
        case 0:
        case 3:
        case 5:
            bPlay = 1;
            if (nKind == 0) {
                if (lbl_80282042) {
                    bPlay = 0;
                } else {
                    lbl_80282042 = 1;
                }
            }
            if (bPlay) {
                nId = fn_800AD280(pSource->nSound, pSource->nKind, nParam, 1, NULL);
                vPos[0] = -lbl_80281450;
                vPos[1] = 0.0f;
                vPos[2] = 0.0f;
                fn_800AD800(nId, vPos, NULL, 0);
                vPos[0] = lbl_80281450;
                nId = fn_800AD280(pSource->nSound, pSource->nKind, nParam, 1, NULL);
                fn_800AD800(nId, vPos, NULL, gSession.nSplitScreen != 0);
            }
            break;
        default:
            nId = fn_800AD280(pSource->nSound, nKind, nParam, 0, NULL);
            vPos[0] = pSource->vPos[0];
            vPos[1] = pSource->vPos[1];
            vPos[2] = pSource->vPos[2];
            fn_800AD800(nId, vPos, NULL, 0);
            break;
        }
    }
}

void fn_800A4E34(void) {
    f32 vPos[3];

    lbl_8028203C = 0;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_80281418 = 0xFF;
    if (lbl_80282028 == 0) {
        // the options' volumes, 0.2 per step
        fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
        fn_800A78F0(0.2f * (s8)gSession.options.a0[4]);
        fn_800A3FB4(15, 0.2f * (s8)gSession.options.a0[1] * lbl_8018E988[15]);
        fn_800A3F94(2);
        lbl_80282028 = 1;
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    lbl_8028141B = fn_800AD280(1, -1, 1, 1, NULL);
    fn_800AD800(lbl_8028141B, vPos, NULL, 0);
    fn_800AD698(lbl_8028141B, 0, 1);
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202E = 0;
    lbl_8028202D = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    lbl_8028202A = 1;
    lbl_80282041 = 0;
    lbl_80282040 = 0;
}

// Stops the game's sounds and clears every view's emitters.
void fn_800A5428(void) {
    int nViews;
    int i;

    nViews = gSession.nSplitScreen ? 2 : 1;
    lbl_80282042 = 0;
    if (lbl_8028203C == 2) {
        fn_800AD450(lbl_8028141A);
        lbl_8028141A = 0xFF;
    } else {
        fn_800A75B4();
    }
    fn_800AD450(lbl_80281419);
    lbl_80281419 = 0xFF;
    fn_800A6EC8();
    fn_800A6854(0);
    fn_800A6660(0);
    fn_800A6D48(0);
    fn_800A6BA8(0);
    fn_800A707C();
    fn_800A714C();
    fn_800A7294();
    fn_800A71E4();
    for (i = 0; i < nViews; i++) {
        lbl_801F1790[i].n0 = 0xFF;
        lbl_801F1790[i].n1 = 0xFF;
        lbl_801F1790[i].n2 = 0xFF;
        lbl_801F1790[i].n3 = 0xFF;
        lbl_801F1790[i].n4 = 0;
        lbl_801F1790[i].n5 = 0;
        lbl_801F1790[i].f8 = 0.0f;
        lbl_801F1790[i].fC = 0.0f;
        lbl_801F1790[i].tLast = 0;
    }
    lbl_80281419 = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_8028141C = 0xFF;
    lbl_8028141D = 0xFF;
    lbl_8028141E = 0xFF;
    lbl_8028141F = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_8028141B = 0xFF;
    lbl_80281420 = 0xFF;
    lbl_80282029 = 0;
    fn_800A3F58(0, 0);
}

// Sets up the round's sounds: each view's emitters (0 the swing, 1 the ball, 2 and 3 the crowd
// either side), the music and the ambience.
void fn_800A500C(void) {
    GameAudioView* pView;
    Player* pPlayer;
    int nViews;
    int nCourse;
    int nMode;
    u8 n;
    int i;
    f32 vPos[3];

    nViews = (gSession.nSplitScreen != 0) + 1;
    nCourse = Game_GetCourse();
    nMode = Game_GetMode();
    n = fn_80015464();
    pView = lbl_801F1790;
    pPlayer = gPlayers;
    if (lbl_80282028 == 0) {
        fn_800A78F0(0.2f * (s8)gSession.options.a0[4]);
        fn_800A3F94(2);
        lbl_80282028 = 1;
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    for (i = 0; i < nViews; i++, pPlayer++, pView++) {
        pView->n0 = fn_800AD280(1, -1, 0, 0, NULL);
        fn_800AD800(pView->n0, pPlayer->vBall, NULL, 0);
        pView->n1 = fn_800AD280(2, -1, 0, 0, NULL);
        fn_800AD800(pView->n1, pPlayer->ball.vPos, NULL, 0);
        fn_800AD698(pView->n1, 0, 1);
        pView->n2 = fn_800AD280(4, -1, 1, 1, fn_800A4170);
        pView->n3 = fn_800AD280(4, -1, 1, 1, NULL);
        vPos[0] = -lbl_80281454;
        vPos[1] = 0.0f;
        vPos[2] = 0.0f;
        fn_800AD800(pView->n2, vPos, NULL, 0);
        vPos[0] = lbl_80281454;
        fn_800AD800(pView->n3, vPos, NULL, 0);
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 1.0f;
    lbl_8028141B = fn_800AD280(10, -1, 1, 1, NULL);
    fn_800AD800(lbl_8028141B, vPos, NULL, 0);
    lbl_8028203C = 0;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_80282041 = 0;
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 10.0f;
    lbl_80281419 = fn_800AD280(5, -1, 1, 1, NULL);
    fn_800AD800(lbl_80281419, vPos, NULL, 0);
    fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
    fn_800A44A0();
    lbl_8028141C = fn_800AD280(7, -1, 1, 1, NULL);
    vPos[0] = lbl_80281458;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    fn_800AD800(lbl_8028141C, vPos, NULL, 0);
    lbl_8028141D = fn_800AD280(7, -1, 1, 1, NULL);
    vPos[0] = -lbl_80281458;
    fn_800AD800(lbl_8028141D, vPos, NULL, 0);
    if (nCourse == 7 && n == 2) {
        fn_800A4044(0, 17);
        fn_800A7AD0(2, 0, 1);
        fn_800A7AD0(1, 0, 1);
        fn_800A7AD0(1, 2, 1);
        fn_800A7AD0(1, 1, 1);
    }
    fn_800A3F38(0, 0);
    lbl_8028202A = 1;
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202E = 0;
    lbl_8028202D = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    // the modes 0-2, 4, 5, 10, 18-21, 23-25 (bit mask 0x03BC0437)
    lbl_80282040 = ((1 << nMode) & 0x03BC0437) != 0;
}

void fn_800A562C(u8 nPlayer) {
    s32* pnView;
    GameAudioView* pView;
    Player* pPlayer;
    u8 nId;
    f32 vPos[3];

    pPlayer = &gPlayers[nPlayer];
    pnView = pPlayer->nView;
    lbl_80282030 = 0;
    pView = &lbl_801F1790[*pnView];
    lbl_80282032 = 0;
    nId = pView->n0;
    lbl_80282034 = 0;
    lbl_8028202F = 0;
    lbl_80281424 = -1;
    fn_800A6EC8();
    fn_800A6BA8(nPlayer);
    fn_800AD734(pView->n2, 0);
    fn_800AD734(pView->n3, 0);
    if (fn_80016CFC(*pnView)->bFlagOut) {
        fn_800AD450(lbl_80281420);
        lbl_80281420 = 0xFF;
    }
    fn_8001EB8C(pPlayer->pChar, 0x53, vPos);
    fn_800AD800(nId, vPos, NULL, 0);
    fn_800ADA28(nId, 0, 1, 1);
    pView->f8 = 0.0f;
    pView->fC = 1.0f / FRAME_RATE;
    pView->n18 = 0;
}

// The swish of the club while the swing meter runs (swing states 1 and 3): its pitch and volume
// follow how fast the club head (bone 0x53) moves.
void fn_800A573C(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    u8 nId;
    int nState;
    s32 nLastState;
    f32 fSpeed;
    f32 fPitch;
    f32 fVolume;
    f32 vPos[4];
    f32 vLast[4];

    pPlayer = &gPlayers[nPlayer];
    pView = &lbl_801F1790[pPlayer->nView[0]];
    nId = pView->n0;
    if (nId != 0xFF) {
        fn_8001EB8C(pPlayer->pChar, 0x53, vPos);
        fn_800AD800(nId, vPos, vLast, 0);
        nState = pPlayer->swing.nState;
        if (nState == 1 || nState == 3) {
            if (pView->fC > 0.0f) {
                nLastState = pView->n18;
                if (nLastState == 1 && nLastState != nState) {
                    fn_800ADA28(nId, 0, 1, 1);
                } else {
                    fSpeed = Vec_Distance(vPos, vLast) / (FRAME_RATE * pView->fC);
                    fPitch = fSpeed * lbl_80281438;
                    fVolume = fSpeed * lbl_8028143C;
                    fPitch = (fPitch <= lbl_80281444) ? lbl_80281444 : fPitch;
                    fPitch = (fPitch <= lbl_80281448) ? fPitch : lbl_80281448;
                    fVolume = (fVolume <= lbl_80282044) ? lbl_80282044 : fVolume;
                    fVolume = (fVolume <= lbl_80281440) ? fVolume : lbl_80281440;
                    if (pPlayer->nClub > 17) {
                        fVolume *= 0.35f;
                    }
                    if (pPlayer->swing.nState == 1) {
                        fVolume *= 0.35f;
                    }
                    fn_800ADA28(nId, 0, 0, 1);
                    fn_800ADA94(nId, 0, fVolume);
                    fn_800ADAF0(nId, 0, fPitch);
                }
                pView->n18 = pPlayer->swing.nState;
            }
        } else {
            fn_800ADA28(nId, 0, 1, 1);
        }
        if (lbl_80202898.bSlowMo) {
            pView->fC = gSession.fFrameTime / lbl_80202898.fSlowMo;
        } else {
            pView->fC = gSession.fFrameTime;
        }
    }
}

// The swing: its sound by club and lie on the view's emitter 0, the crowd's reaction for a few
// calls after fn_800A68C0 set lbl_80282034.
void fn_800A5980(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    Clip* pClip;
    u64 uName;
    int nKind;
    int nMode;
    u8 nIdSwing;
    u8 nIdA;
    u8 nIdB;
    u8 bRestore;
    u8 n;
    u8 nCrowd;

    pPlayer = &gPlayers[nPlayer];
    pView = &lbl_801F1790[pPlayer->nView[0]];
    nIdSwing = pView->n0;
    nIdA = pView->n2;
    nIdB = pView->n3;
    nKind = fn_800C7138(fn_80017028(pPlayer->nView[0]));
    bRestore = 1;
    nMode = Game_GetMode();
    if (fn_8006BEA4()) {
        fn_800A6C98(nPlayer, 0);
        bRestore = 0;
    }
    if (lbl_80282034 > 0) {
        n = 1;
        if (lbl_80282034 == 1) {
            switch (nKind) {
            case 2:
                n = 2;
                break;
            case 5:
            case 8:
                n = 3;
                break;
            case 11:
                nCrowd = 1;
                if (gPlayers[nPlayer].pChar != NULL) {
                    pClip = gPlayers[nPlayer].pChar->pCurClip;
                    if (pClip != NULL) {
                        // port: the clip name's first 8 characters read as one big-endian u64
                        uName = *(u64*)pClip->name;
                        if (uName == 0x67646C66756C3332ULL || uName == 0x66646C66756C3332ULL) {
                            nCrowd = 2;
                        }
                    }
                }
                fn_800A6C98(nPlayer, nCrowd);
                break;
            default:
                n = 0;
                break;
            }
        }
        if (n) {
            fn_800AD9AC(nIdA, 4, n);
            fn_800AD9AC(nIdB, 4, n);
            fn_800AD698(nIdA, 4, 1);
            fn_800AD698(nIdB, 4, 1);
        }
        lbl_80282034--;
        return;
    }
    if (lbl_80282032) {
        if (nKind == 11) {
            fn_800AD9AC(nIdA, 5, 2);
            fn_800AD9AC(nIdB, 5, 2);
        } else {
            fn_800AD9AC(nIdA, 5, 0);
            fn_800AD9AC(nIdB, 5, 0);
        }
        fn_800ADA94(nIdSwing, 1, 2.0f);
        fn_800AD698(nIdA, 5, 1);
        fn_800AD698(nIdB, 5, 1);
        lbl_80282032 = 0;
    } else {
        fn_800ADA94(nIdSwing, 1, 1.0f);
    }
    fn_800AD800(nIdSwing, pPlayer->ball.vPos, NULL, 0);
    fn_800AD9AC(nIdSwing, 1, lbl_80183AD8[pPlayer->nClub]);
    if ((nMode == 22 || nMode == 26) && fn_8005C280(nPlayer) > 1.0f) {
        fn_800AD950(nIdSwing, 1, 5);
    } else {
        fn_800AD950(nIdSwing, 1, lbl_80183AF4[pPlayer->ball.nLie]);
    }
    fn_800AD698(nIdSwing, 1, 1);
    if (bRestore && lbl_80281428 != -1) {
        fn_800A6DCC(lbl_80281428, 1);
        lbl_80281428 = -1;
        lbl_8028202F = 0;
    }
    lbl_80282033 = 1;
    pView->tLast = fn_800954A4(1);
}

// The ball's impact sound, by the surface it hit (its nSoundId) and scaled by its speed; on
// course 7's hole 2 a surface with a swing sound plays that instead.
void fn_800A5CA4(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    SurfaceType* pSurface;
    u64 tNow;
    f32 fElapsed;
    f32 fSpeed;
    f32 fVolume;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    pSurface = pPlayer->ball.pHitSurface;
    pView = &lbl_801F1790[pPlayer->nView[0]];
    tNow = fn_800954A4(1);
    fElapsed = fn_8006E118(tNow, pView->tLast);
    pView->tLast = tNow;
    if ((pPlayer->ball.nCollideCount == 0
         || (pPlayer->ball.nCollideCount > 0 && fElapsed >= 0.2f))
        && pSurface != NULL) {
        fSpeed = pPlayer->ball.fSpeed;
        if (fSpeed < 0.0f) {
            fSpeed = -fSpeed;
        }
        fVolume = lbl_8028144C * fSpeed;
        fVolume = fVolume * fVolume;
        if (fVolume > 0.1f) {
            if (Game_GetCourse() == 7 && fn_80015464() == 2 && pSurface->nSwingSoundId != 0) {
                fVolume *= 2.0f;
                if (fVolume > 2.0f) {
                    fVolume = 2.0f;
                }
                fn_800ADBC0(4, pPlayer->ball.vPos, NULL, 0);
                fn_800ADCD0(4, 0, pSurface->nSwingSoundId - 1, 0);
                fn_800ADD54(4, 0, fVolume);
            } else {
                nId = lbl_801F1790[pPlayer->nView[0]].n1;
                if (pSurface->nSoundId == 4) {
                    fVolume = 1.0f;
                }
                if (fVolume > 1.0f) {
                    fVolume = 1.0f;
                }
                fn_800AD800(nId, pPlayer->ball.vPos, NULL, 0);
                fn_800ADA28(nId, 0, pSurface->nSoundId, 0);
                fn_800ADA94(nId, 0, fVolume);
            }
            if (Game_GetMode() == 26 || Game_GetMode() == 22) {
                fn_8010D3D8(nPlayer);
            }
        }
    }
    fn_800A707C();
}

void fn_800A5EC0(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;
    u8 nRand;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    fn_800AD800(nId, pPlayer->ball.vPos, NULL, 0);
    fn_800ADA94(nId, 0, 2.0f);
    nRand = Rand_Next(2) & 1;   // one of two sounds at random
    fn_800ADA28(nId, 0, nRand == 0 ? 0x1A : 0x1C, 0);
}

void fn_800A5F60(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    fn_800AD800(nId, pPlayer->ball.vPos, NULL, 0);
    fn_800ADA94(nId, 0, 1.0f);
    fn_800ADA28(nId, 0, 0x17, 0);
}

void fn_800A5FE8(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    fn_800AD800(nId, pPlayer->ball.vPos, NULL, 0);
    fn_800ADA94(nId, 0, 1.0f);
    fn_800ADA28(nId, 0, 0x17, 0);
}

// Plays sound 3 on the view's emitters' track 1, at most once every 300 frames when bLimit is set.
void fn_800A6070(u8 nPlayer, u8 bLimit) {
    Player* pPlayer;
    u8 nIdA;
    u8 nIdB;

    pPlayer = &gPlayers[nPlayer];
    nIdA = lbl_801F1790[pPlayer->nView[0]].n2;
    nIdB = lbl_801F1790[pPlayer->nView[0]].n3;
    if (!bLimit || lbl_80282048 == 0 || lbl_80282048 + 300 < gSession.nFrameCount) {
        lbl_80282048 = gSession.nFrameCount;
        fn_800AD9AC(nIdA, 1, 3);
        fn_800AD9AC(nIdB, 1, 3);
        fn_800AD698(nIdA, 1, 1);
        fn_800AD698(nIdB, 1, 1);
    }
}

void fn_800A6148(void) {
    u8 nIdA;
    u8 nIdB;

    nIdA = lbl_801F1790[0].n2;
    nIdB = lbl_801F1790[0].n3;
    fn_800AD9AC(nIdA, 1, 3);
    fn_800AD9AC(nIdB, 1, 3);
    fn_800AD698(nIdA, 1, 1);
    fn_800AD698(nIdB, 1, 1);
}

void fn_800A61C4(int n) {
    u8 nIdA;
    u8 nIdB;

    nIdA = lbl_801F1790[0].n2;
    nIdB = lbl_801F1790[0].n3;
    fn_800AD9AC(nIdA, 7, n);
    fn_800AD9AC(nIdB, 7, n);
    fn_800AD698(nIdA, 7, 1);
    fn_800AD698(nIdB, 7, 1);
}

void fn_800A6450(u8 nPlayer) {
    u8 nId;

    nId = lbl_801F1790[gPlayers[nPlayer].nView[0]].n0;
    if (nId != 0xFF) {
        fn_800AD698(nId, 2, 1);
    }
}

// The calls below only sound when gpGame->b288 is set.
void fn_800A64A8(u8 nPlayer, u8 b) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (lbl_8028202B || lbl_8028202F) return;
    fn_800A4170(pView->n2, 2, 1);
    fn_800AD698(pView->n2, 2, 1);
    fn_800AD698(pView->n3, 2, 1);
    fn_800A3F58(0, 1);
    fn_800ADA94(lbl_80281420, 0, 0.0f);
    fn_800ADA94(lbl_8028141C, 0, 0.0f);
    fn_800ADA94(lbl_8028141C, 1, 0.0f);
    fn_800ADA94(lbl_8028141C, 2, 0.0f);
    fn_800ADA94(lbl_8028141C, 3, 0.0f);
    fn_800ADA94(lbl_8028141C, 4, 0.0f);
    fn_800ADA94(lbl_8028141C, 5, 0.0f);
    fn_800ADA94(lbl_8028141D, 0, 0.0f);
    fn_800ADA94(lbl_8028141D, 1, 0.0f);
    fn_800ADA94(lbl_8028141D, 2, 0.0f);
    fn_800ADA94(lbl_8028141D, 3, 0.0f);
    fn_800ADA94(lbl_8028141D, 4, 0.0f);
    fn_800ADA94(lbl_8028141D, 5, 0.0f);
    fn_800ADA94(lbl_8028141A, 0, 0.0f);
    if (b) {
        fn_800A6C98(nPlayer, 0);
    }
    lbl_8028202F = 1;
}

void fn_800A6660(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (lbl_8028202B == 0) {
        fn_800AD698(pView->n2, 2, 0);
        fn_800AD698(pView->n3, 2, 0);
        fn_800A3F58(0, 0);
        fn_800ADA94(lbl_80281420, 0, 1.0f);
        fn_800ADA94(lbl_8028141C, 0, lbl_80281430);
        fn_800ADA94(lbl_8028141C, 1, lbl_80281430);
        fn_800ADA94(lbl_8028141C, 2, lbl_80281430);
        fn_800ADA94(lbl_8028141C, 3, lbl_80281430);
        fn_800ADA94(lbl_8028141C, 4, lbl_80281430);
        fn_800ADA94(lbl_8028141C, 5, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 0, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 1, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 2, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 3, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 4, lbl_80281430);
        fn_800ADA94(lbl_8028141D, 5, lbl_80281430);
        fn_800ADA94(lbl_8028141A, 0, 1.0f);
        lbl_8028202F = 0;
        if (lbl_80281428 != -1) {
            fn_800A6DCC(lbl_80281428, 1);
            lbl_80281428 = -1;
        }
    }
}

void fn_800A67E8(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    fn_800AD698(pView->n2, 0, 1);
    fn_800AD698(pView->n3, 0, 1);
}

void fn_800A6854(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    fn_800AD698(pView->n2, 0, 0);
    fn_800AD698(pView->n3, 0, 0);
}

void fn_800A68C0(u8 nPlayer) {
    GameAudioView* pView;
    int nKind;
    u8 bPlay;
    u8 n;

    pView = &lbl_801F1790[0];
    nKind = fn_800C7138(fn_80017028(0));
    bPlay = 1;
    n = 0;
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        fn_800A3F58(0, 1);
        switch (nKind) {
        case 1:
        case 6:
            lbl_80282034 = 1;
            break;
        case 13:
        case 14:
            n = 2;
            lbl_80282034 = 1;
            break;
        case 15:
        case 16:
            fn_800A6C98(nPlayer, 2);
            lbl_80282034 = 1;
            break;
        case 4:
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 7:
            fn_800AD698(pView->n2, 2, 1);
            fn_800AD698(pView->n3, 2, 1);
            fn_800AD9AC(pView->n2, 6, 5);
            fn_800AD9AC(pView->n3, 6, 5);
            fn_800AD950(pView->n2, 6, 0);
            fn_800AD950(pView->n3, 6, 1);
            fn_800AD698(pView->n2, 6, 1);
            fn_800AD698(pView->n3, 6, 1);
            fn_800A6C98(nPlayer, 2);
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 9:
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 10:
            n = 1;
            lbl_80282034 = 1;
            break;
        case 2:
        case 8:
        case 11:
            bPlay = 0;
            lbl_80282034 = 3;
            break;
        case 5:
            bPlay = 0;
            lbl_80282034 = 2;
            break;
        default:
            bPlay = 0;
            lbl_80282034 = 0;
            break;
        }
        if (bPlay) {
            fn_800AD9AC(pView->n2, 1, n);
            fn_800AD9AC(pView->n3, 1, n);
            fn_800AD698(pView->n2, 1, 1);
            fn_800AD698(pView->n3, 1, 1);
        }
        lbl_80282032 = 1;
    }
}

void fn_800A6AC8(u8 nPlayer, u8 n) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    switch (fn_800C7138(fn_80017028(0))) {
    case 4:
        n += 4;
        // fall through
    case 9:
        fn_800AD9AC(pView->n2, 6, n);
        fn_800AD9AC(pView->n3, 6, n);
        fn_800AD950(pView->n2, 6, 0);
        fn_800AD950(pView->n3, 6, 1);
        fn_800AD698(pView->n2, 6, 1);
        fn_800AD698(pView->n3, 6, 1);
        break;
    }
}

void fn_800A6BA8(u8 nPlayer) {
    GameAudioView* pView;
    int nKind;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    nKind = fn_800C7138(fn_80017028(0));
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        fn_800A3F58(0, 0);
        fn_800AD698(pView->n2, 1, 0);
        fn_800AD698(pView->n3, 1, 0);
        if (nKind == 7) {
            fn_800AD698(pView->n2, 2, 0);
            fn_800AD698(pView->n3, 2, 0);
        }
        if (lbl_80282032) {
            fn_800A5980(nPlayer);
        }
    }
}

void fn_800A6C98(u8 nPlayer, u8 n) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        fn_800AD9AC(pView->n2, 3, n);
        fn_800AD9AC(pView->n3, 3, n);
        fn_800AD698(pView->n2, 3, 1);
        fn_800AD698(pView->n3, 3, 1);
    }
}

void fn_800A6DCC(int nMusic, int a) {
    u8 n = nMusic;

    fn_800A4374();
    fn_800A707C();
    if (a == 1) {
        lbl_80282033 = 0;
    }
    if (lbl_8028202F || lbl_80282032) {
        lbl_80281428 = nMusic;
        return;
    }
    fn_800AD9AC(lbl_8028141C, 0, n);
    fn_800AD9AC(lbl_8028141C, 1, n);
    fn_800AD9AC(lbl_8028141D, 0, n);
    fn_800AD9AC(lbl_8028141D, 1, n);
    fn_800AD698(lbl_8028141C, 0, 1);
    fn_800AD698(lbl_8028141C, 1, 1);
    fn_800AD698(lbl_8028141D, 0, 1);
    fn_800AD698(lbl_8028141D, 1, 1);
    lbl_80281424 = nMusic;
}

void fn_800A6D48(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        fn_800AD698(pView->n2, 3, 0);
        fn_800AD698(pView->n3, 3, 0);
    }
}

void fn_800A6FE0(void) {
    if (lbl_8028202F || !lbl_80282040) return;
    if (gPlayers[lbl_80282278].ball.b99 == 0) {
        fn_800A6EC8();
    }
    fn_800AD698(lbl_8028141C, 4, 1);
    fn_800AD698(lbl_8028141D, 4, 1);
    fn_800AD698(lbl_8028141C, 5, 1);
    fn_800AD698(lbl_8028141D, 5, 1);
}

void fn_800A70E4(int n) {
    if (gpGame->b288) {
        fn_800ADB4C(0, 0, 1);
        fn_800ADC44(0, 1, n);
        fn_800ADB4C(0, 1, 1);
    }
}

void fn_800A714C(void) {
    if (gpGame->b288) {
        fn_800ADB4C(0, 0, 0);
        fn_800ADB4C(0, 1, 0);
    }
}

void fn_800A7198(int n) {
    if (gpGame->b288) {
        fn_800ADC44(1, 0, n);
        fn_800ADB4C(1, 1, 1);
    }
}

void fn_800A71E4(void) {
    if (gpGame->b288) {
        fn_800ADB4C(1, 1, 0);
    }
}

void fn_800A7220(f32 fAmount) {
    u32 n;

    n = (u8)(int)(3.0f * fAmount);
    if (gpGame->b288) {
        fn_800ADC44(0, 2, (n <= 2) ? n : 2);
        fn_800ADB4C(0, 2, 1);
    }
}

void fn_800A7294(void) {
    if (gpGame->b288) {
        fn_800ADB4C(0, 2, 0);
        if (lbl_8028203C == 2) {
            fn_800AD698(lbl_8028141A, 2, 0);
        }
    }
}

void fn_800A7350(u8 bOn) {
    if ((lbl_8028202B ^ bOn) != 0) {
        lbl_8028202B = bOn;
        if (lbl_8028202E == 0) {
            fn_800A3F38(bOn, 0);
            if (bOn) {
                fn_800A73C0(0, 1);
            } else {
                lbl_8028202C = 1;
            }
        }
    }
}

void fn_800A746C(s32 nKind, int nTrack, int n) {
    switch (nKind) {
    case 0:
        fn_800ADC44(5, nTrack, n);
        fn_800ADB4C(5, nTrack, 1);
        break;
    case 1:
        fn_800ADCD0(6, 0, n, 0);
        break;
    }
}

void fn_800A74E4(s32 nKind, int nTrack) {
    switch (nKind) {
    case 0:
        fn_800ADB4C(5, nTrack, 0);
        break;
    case 1:
        break;
    }
}

void fn_800A754C(u8 a, u16 b) {
    if (lbl_8028203C == 1) {
        fn_800A7968(lbl_80281418, 0, a, b, 2);
        fn_800AD698(lbl_80281418, 0, 1);
        fn_800BA734(1, b);
    }
}

// port: the callers pass nKind and nMsg as full ints (their prototype takes int), but this body
// was compiled for a u8 nKind and a u16 nMsg: it stores and passes them on without masking. Kept
// as int to match the callers, so this function stays at 91% (masks where the original has none).
void fn_800A7664(int nKind, int nMsg, int a) {
    if ((s8)gSession.options.a0[4] != 0) {
        if (lbl_80282054 != 0) {
            lbl_80282052 = nKind;
            lbl_80282038 = 1;
            lbl_80282050 = nMsg;
            lbl_8028204C = a;
            return;
        }
        // port: EA passes nKind and nMsg as ints, unmasked, to fn_800A7968's u8 and u16 parameters
        ((void (*)(u8, u8, int, int, s32))fn_800A7968)(lbl_80281419, 0, nKind, nMsg, a);
        fn_800AD698(lbl_80281419, 0, 1);
    }
}

u8 fn_800A7748(void) {
    return fn_800AD618(lbl_8028141A, 0);
}

u8 fn_800A7770(void) {
    int bResult;

    if (gSession.nGameType == 3) {
        return fn_800A75F4();
    }
    bResult = 0;
    if (fn_800A7720() || fn_800A7748() || fn_800A75F4()) {
        bResult = 1;
    }
    return bResult;
}

// Scales the volume curves 1-6, 13 and 16-31 and emitter lbl_8028141B's tracks 0 and 1 (the
// options menu passes 0.2 x options.a0[0]).
void fn_800A77E0(f32 fVolume) {
    u8 i;

    for (i = 1; i < 5; i++) {
        fn_800A3FB4(i, fVolume * lbl_8018E988[i]);
    }
    fn_800A3FB4(13, lbl_8018E988[13] * fVolume);
    fn_800A3FB4(5, lbl_8018E988[5] * fVolume);
    fn_800A3FB4(6, lbl_8018E988[6] * fVolume);
    fn_800ADA94(lbl_8028141B, 0, fVolume);
    fn_800ADA94(lbl_8028141B, 1, fVolume);
    for (i = 16; i < 32; i++) {
        fn_800A3FB4(i, fVolume * lbl_8018E988[i]);
    }
}

void fn_800A78F0(f32 fVolume) {
    fVolume *= lbl_8018E988[14];
    fn_800A3FB4(14, fVolume);
}

void fn_800A7AD0(s16 nSound, u8 nTrack, u8 bOn) {
    fn_800A94F4(nSound, nTrack, bOn);
}

void fn_800A7968(u8 nId, u8 nTrack, u8 a, u16 b, s32 c) {
    fn_800AD790(nId, nTrack, (c << 24) | (a << 16) | b);
}
