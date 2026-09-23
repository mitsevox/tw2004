// GameEffects.c (TW06's name, GameEffects_*): slow motion, the "GameBreaker" (letterbox and
// slow-down while a shot may drop), the heartbeat rumble, the time rate. TW06's copy of the file
// keeps the same function order, which gives the names.

#include "golfer.h"

// The effects state (lbl_80202898).
typedef struct GameEffects {
    u8   bSlowMo;               // 0x00  super slow motion is on
    u8   unk1[3];
    f32  fSlowMo;               // 0x04  its rate (below 1 slows down)
    u8   unk8;
    u8   b9;                    // 0x09
    u8   unkA[2];
    f32  fC;                    // 0x0C
    u8   b10;                   // 0x10  half speed
    u8   b11;                   // 0x11  quarter-ish speed, counted in n28
    u8   bGameBreaker;          // 0x12  the letterbox is up
    u8   bGB12pad;
    s32  nGBType;               // 0x14  0 scripted, 1 predicted
    u8   bClosing;              // 0x18  the letterbox is closing
    u8   b19;                   // 0x19
    u8   unk1A[2];
    f32  fGBTime;               // 0x1C  seconds the letterbox has been opening (or left closing)
    u8   unk20[4];
    f32  f24;                   // 0x24
    s32  n28;                   // 0x28  frames at the slow rate
    s32  n2C;                   // 0x2C  every n2C-th of them the ball moves
    s32  nPlayer;               // 0x30  whose shot
    u8   unk34[8];
    u8   bSpinWindowDone;       // 0x3C
    u8   unk3D[3];
    s32  nRumbleFrames;         // 0x40
    u8   bRumble;               // 0x44
    u8   nHeartbeats;           // 0x45
    u8   bPaused;               // 0x46
    u8   unk47[3];
    u8   b4A;                   // 0x4A
    u8   unk4B[5];
    u32  uFlags;                // 0x50  bit 0x4000: an eagle on a par 5 counts
    f32  f54;                   // 0x54
} GameEffects;

extern GameEffects lbl_80202898;

void  EVENT_Trigger(int nPlayer, int nEvent, int a, int b);
void  fn_800DCAD8(void);
u8    Controller_IsPad(int nController);
void  fn_800131C4(int nController);
void  fn_80013130(int nController, int nStrength);
u8    fn_800C714C(void);
int   GOLFERSTATE_GetCurrentState(int nPlayer);
u8    fn_800DCB74(void);
u8    fn_800DCB3C(void);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);   // memcpy
void  fn_8001425C(int a);
void  fn_80012F34(int a);
void  fn_80012F18(int a);
void  fn_80012F50(int a, int b, int c);
void  fn_80014118(int a);
void  fn_80012EF8(void);
void  fn_800141F8(f32* pA, f32* pB, f32 x0, f32 y0, f32 x1, f32 y1);
void  fn_80014194(f32* pColour);
void  fn_8001644C(int a, f32* pA, int b, f32* pB, int c);
int   fn_800D2B08(void);                    // the hole's par
int   Game_CurHoleIndex(void);
int   fn_800D7660(int nPlayer, u8* pBall, u8 b);
int   fn_800D782C(int nPlayer, u8* pBall, int a, u8 b, int c);
u8    fn_800E5344(void);
u8    fn_800E23B0(int nPlayer, int nStrokes);
u8    fn_800DC818(u8* pBall, int nPlayer, u8 bNext);
GameEffects* fn_800DAF74(void);

// TW06: GameEffects_InitGameEffectSettings (by position and size).
void fn_800DAE44(void) {
    lbl_80202898.b10 = 0;
    lbl_80202898.b11 = 0;
    lbl_80202898.bSlowMo = 0;
    lbl_80202898.b9 = 0;
    lbl_80202898.n28 = 0;
    lbl_80202898.bGameBreaker = 0;
    lbl_80202898.b19 = 0;
    lbl_80202898.bPaused = 0;
    lbl_80202898.bRumble = 0;
    lbl_80202898.nHeartbeats = 0;
    lbl_80202898.f54 = 1.0f;
}

// Every effect off: a GameBreaker still up is closed (its end events), the rumble stopped.
void GameEffects_ResetGameEffectSettings(void) {
    int i;
    fn_800DAF74();
    lbl_80202898.bPaused = 0;
    lbl_80202898.bRumble = 0;
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.nGBType == 0) {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
        } else {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
        }
        fn_800DCAD8();
    }
    lbl_80202898.bGameBreaker = 0;
    lbl_80202898.bSpinWindowDone = 0;
    lbl_80202898.b19 = 0;
    lbl_80202898.b4A = 0;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (Controller_IsPad(gSession.nController[i])) {
            fn_800131C4(gSession.nController[i]);
        }
    }
}

// TW06: GameEffects_ResetGameEffectTimeSettings (by position and size).
GameEffects* fn_800DAF74(void) {
    lbl_80202898.b10 = 0;
    lbl_80202898.b11 = 0;
    lbl_80202898.bSlowMo = 0;
    lbl_80202898.b9 = 0;
    lbl_80202898.n28 = 0;
    return &lbl_80202898;
}

// How many physics steps the ball takes this frame: one per 1/59.94 s of frame time (rounded;
// twice that in mode 26), none while paused (no frame time), one outside the ball's flight.
// With the slow-down on, it moves only on every n2C-th frame.
int GameEffects_BallUpdatesThisFrame(int nPlayer) {
    if (gSession.nGameType == 3 || fn_800C714C()) {
        return 1;
    }
    if (0.0f == gSession.fFrameTime) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SIMULATE) {
        return 1;
    }
    if (fn_800DCB74() && gSession.fFrameTime < 1.0f / 59.94f) {
        return fn_800DCB3C() != 0;
    }
    if (Game_GetMode() == 26) {
        if (gSession.fFrameTime <= 1.0f / 59.94f) {
            return 2;
        }
        return 0.5f + 2.0f * gSession.fFrameTime / (1.0f / 59.94f);
    }
    if (gSession.fFrameTime <= 1.0f / 59.94f) {
        return 1;
    }
    return 0.5f + gSession.fFrameTime / (1.0f / 59.94f);
}

// TW06: GameEffects_FieldOfViewChange (by position). The letterbox's field-of-view change: up to
// 0.349 (20 degrees) over the first 0.8 seconds.
f32 fn_800DC3A4(void) {
    if (!lbl_80202898.bGameBreaker) {
        return 0.0f;
    }
    if (gSession.unk14 != 0) {
        return 0.0f;
    }
    if (lbl_80202898.bPaused) {
        return 0.0f;
    }
    if (lbl_80202898.nGBType == 1) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.34906587f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    if (lbl_80202898.nGBType == 0) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.34906587f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    return 0.0f;
}

// TW06: GameEffects_DepthOfFieldChange (by position).
f32 fn_800DC45C(void) {
    return 0.0f;
}

// TW06: GameEffects_SimulateBall (by position). A putt always; otherwise once the spin window is done.
u8 fn_800DC464(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == SHOT_PUTT) {
        return 1;
    }
    return lbl_80202898.bSpinWindowDone;
}

// TW06: GameEffects_SpinWindowDone (by position). The look-ahead copy restarts from the ball.
void fn_800DC498(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (!gSession.bReplay || *(s32*)(p->ballBefore + 0x64) != 0) {
        Mem_cpy(p->ballBefore, p->ball, 0xBC);
        *(s32*)(p->ballBefore + 0x94) = -1;
    }
    lbl_80202898.bSpinWindowDone = 1;
}

// TW06: GameEffects_IsSlowDownSwingOn (by position).
s32 fn_800DC514(void) {
    return lbl_80202898.bSlowMo;
}

// Super slow motion on (with its rate) or off, with the sound events (0x35/0x37 on, 0x36/0x38 off,
// by whether it slows down or speeds up).
void GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate) {
    if (bOn) {
        if (!lbl_80202898.bSlowMo) {
            if (fRate < 1.0f) {
                EVENT_Trigger(nPlayer, 0x35, (int)&gPlayers[nPlayer].fBallX, -1);
            } else {
                EVENT_Trigger(nPlayer, 0x37, (int)&gPlayers[nPlayer].fBallX, -1);
            }
            lbl_80202898.bSlowMo = bOn;
        }
        lbl_80202898.fSlowMo = fRate;
        lbl_80202898.b11 = 0;
        lbl_80202898.b10 = 0;
        return;
    }
    if (lbl_80202898.bSlowMo) {
        lbl_80202898.bSlowMo = bOn;
        if (lbl_80202898.fSlowMo < 1.0f) {
            EVENT_Trigger(nPlayer, 0x36, (int)&gPlayers[nPlayer].fBallX, -1);
            return;
        }
        EVENT_Trigger(nPlayer, 0x38, (int)&gPlayers[nPlayer].fBallX, -1);
    }
}

// TW06: GameEffects_UpdateGameEffects (by position). A heartbeat rumble lasts 5 frames.
void fn_800DC664(int nPlayer) {
    int nController = gPlayers[nPlayer].nController;
    if (lbl_80202898.bRumble) {
        if (++lbl_80202898.nRumbleFrames == 5) {
            lbl_80202898.bRumble = 0;
            if (Controller_IsPad(nController)) {
                fn_80013130(nController, 0);
            }
        }
    }
}

// TW06: GameEffects_VibrateControllerForHeartbeat (by position). Up to 40 beats.
void fn_800DC6E8(int nPlayer) {
    int nController;
    if (lbl_80202898.nHeartbeats < 40) {
        lbl_80202898.nHeartbeats++;
        nController = gPlayers[nPlayer].nController;
        if (!lbl_80202898.bRumble) {
            if (Controller_IsPad(nController)) {
                fn_80013130(nController, 0xFF);
            }
            lbl_80202898.bRumble = 1;
            lbl_80202898.nRumbleFrames = 0;
        }
    }
}

// TW06: GameEffects_SkipOtherCommentary (by position). While a GameBreaker is up: always for a
// predicted one; while it closes, if the shot "did it"; otherwise once it is fully open.
u8 fn_800DC784(void) {
    if (!lbl_80202898.bGameBreaker) {
        return 0;
    }
    if (lbl_80202898.nGBType == 1) {
        return 1;
    }
    if (lbl_80202898.bClosing) {
        return fn_800DC818(gPlayers[lbl_80202898.nPlayer].ball, lbl_80202898.nPlayer, 0);
    }
    return lbl_80202898.fGBTime >= 0.8f;
}

// TW06: GameEffects_ScriptedGBDidIt (by position). Whether the shot earned its GameBreaker:
// holed within the stroke limit, a special eagle on a par 5 (flag 0x4000; on the green in 1 or 2),
// or one of two score checks.
u8 fn_800DC818(u8* pBall, int nPlayer, u8 bNext) {
    u8  bEagle;
    int a;
    int b;
    int nStrokes;
    if (*(s32*)(pBall + 0x68) != LIE_GREEN || !(lbl_80202898.uFlags & 0x4000)) {
        bEagle = 0;
    } else if (fn_800D2B08() != 5) {
        bEagle = 0;
    } else if (bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 1) {
        bEagle = 0;
    } else if (!bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 2) {
        bEagle = 0;
    } else {
        bEagle = 1;
    }
    a = fn_800D7660(nPlayer, pBall, bNext);
    b = fn_800D782C(nPlayer, pBall, 0, bNext, 1);
    if (!bNext) {
        if (b == 0 && a == 0) {
            b = fn_800E5344();
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    } else {
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    }
    if ((*(s32*)(pBall + 0x68) == LIE_HOLED && !fn_800E23B0(nPlayer, nStrokes - 1)) || bEagle || b || a) {
        return 1;
    }
    return 0;
}

// TW06: GameEffects_Pause (by position). Pauses or resumes a GameBreaker, with its sound events.
void fn_800DC9D4(void) {
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.bPaused) {
            lbl_80202898.bPaused = 0;
            if (lbl_80202898.nGBType == 0) {
                EVENT_Trigger(lbl_80202898.nPlayer, 0x3D, 0, -1);
                return;
            }
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3F, 0, -1);
            return;
        }
        lbl_80202898.bPaused = 1;
        if (lbl_80202898.nGBType == 0) {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
            return;
        }
        EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
    }
}

// The letterbox grows to 15% of the screen over its first 0.8 seconds.
f32 GameEffects_GetLetterboxHeight(void) {
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.15f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.15f;
    }
    return 0.0f;
}
