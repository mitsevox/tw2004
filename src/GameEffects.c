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
    u8   b47;                   // 0x47  u48 holds a sound to stop
    u16  u48;                   // 0x48
    u8   b4A;                   // 0x4A  u4C holds a sound to stop
    u8   unk4B;
    u16  u4C;                   // 0x4C
    u8   b4E;                   // 0x4E
    u8   n4F;                   // 0x4F  the music to go back to
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
u8    fn_800DCB08(void);
u8    fn_800DCB00(void);
u8    fn_8005D2DC(void);
void  GM_vCloseModuleONCE(void);
u8    fn_800C6CCC(void);
f32   fn_8000AD9C(f32 x);                  // fabsf
u8    fn_800C6CB0(void);
u8    fn_800B4AE0(void);
int   Game_GetCourse(void);
f32   AI_MaxDistance(int nPlayer, int nKind, int nClub);
f32   fn_800510EC(u8* pBall);
f32   fn_8005B64C(int nPlayer);
void  fn_80045494();                      // (bOn, nPlayer); EA calls it with one argument too
void  fn_80045558(int a, int nPlayer);
void* fn_80017028(int nView);
int   fn_8003BDBC(int nPlayer, int nLie, int a, int b, int c, f32 fDist);
void  fn_800DBFAC(void);
void  fn_800DC18C(void);
void  fn_800DC290(f32 fHeight);
int   fn_800E17AC(int nPlayer);           // the player's total strokes
f32   fn_80009744(f32* pVec);               // dot with itself
void  fn_800DCB84(u8* pA, u8* pB, f32* pOut);
int   fn_800F354C(int nPlayer);
int   fn_800F20C0(int nPlayer);
int   fn_800F1D34(int nPlayer);
CourseInfo* fn_8000C594(void);
void* fn_800CC190(CourseInfo* pCourse, f32* pPos);   // surface type under a point
f32   fn_800D0478(int nPlayer);             // the ball's distance from the pin (yards)
extern u8* gpSaveData;
int   fn_800D8750(int a, int b, int c, u8* pProfile, int nPlayer);
u8    fn_800DCB10(int nPlayer);
u8    fn_800BCD24(int nPlayer);
int   fn_800D0620(int nPlayer, int a, int b);
int   fn_800D089C(int nPlayer, int a);
int   Hole_ScoreAfterTapIn(int nPlayer);
void  fn_800BD83C(int nSound, int a);
void  fn_800A6DCC(int nMusic, int a);
u32   fn_8003A950(int nCam, int a, s32* pKind, f32* pTime, s32* p3, s32* p4, s32* p5, int nPlayer);
u8    fn_800451A8(void* pList, u32 uTarget, int nPlayer);
u8    fn_8003DC78(u32 uTarget);
int   fn_80095780(int nHandle);             // the golfer's current animation
void  fn_80095744(int nHandle, int nAnim);  // play an animation
extern u8 gNumPlayersSetUp;                 // 0x80281D48 (Golfer.c)

// A course's records (the 'rcrd' block at gSession + 0xF00, 0x320 bytes per course).
#define COURSE_RECORD(off) (*(s32*)((u8*)&gSession + 0xF00 + Game_GetCourse() * 0x320 + (off)))

// Starts a scripted GameBreaker for nPlayer, for reason nReason (a bit in uFlags).
#define GB_START(nPlayer, nReason)                                                                     if (!lbl_80202898.bGameBreaker || lbl_80202898.bClosing || lbl_80202898.nGBType != 0) {                lbl_80202898.bClosing = 0;                                                                         lbl_80202898.bGameBreaker = 1;                                                                     lbl_80202898.fGBTime = 0.0f;                                                                       lbl_80202898.f24 = 0.0f;                                                                           lbl_80202898.b19 = 0;                                                                              lbl_80202898.nGBType = 0;                                                                          lbl_80202898.nPlayer = nPlayer;                                                                    lbl_80202898.bPaused = 0;                                                                          lbl_80202898.uFlags = 1 << (nReason);                                                              lbl_80202898.nHeartbeats = 0;                                                                      EVENT_Trigger(nPlayer, 0x3D, 0, -1);                                                           }

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

// TW06: GameEffects_AdjustTimeRate (by position). The game's time step for a frame that took
// fFrameTime: rounded to whole 60 Hz ticks, then scaled by the effects.
f32 fn_800DAF98(f32 fFrameTime) {
    f32 fTicks = 1.0f;
    f32 fBest = 10000.0f;
    int i;
    f32 d;
    if (fn_800DCB08() && 0.0f != fFrameTime) {
        fFrameTime = 1.0f / 59.94f;
    }
    if (fn_8005D2DC()) {
        fFrameTime = 0.0f;
    }
    if (fn_800DCB00()) {
        if (0.0f != fFrameTime) {
            fFrameTime = 1.0f / 59.94f;
        }
        GM_vCloseModuleONCE();
    }
    if (fn_800C6CCC()) {
        return 0.0f;
    }
    for (i = 0; i < 5; i++) {
        d = fn_8000AD9C(i / 59.94f - fFrameTime);
        if (d < fBest) {
            fBest = d;
        } else if (i > 0) {
            fTicks = i - 1;
            break;
        }
    }
    if (fn_800C6CB0()) {
        if (!fn_800B4AE0()) {
            fTicks *= 0.75f;
        }
    } else if (lbl_80202898.b9) {
        fTicks *= 2.0f;
        lbl_80202898.fC -= fFrameTime;
        if (lbl_80202898.fC < 0.0f) {
            lbl_80202898.b9 = 0;
        }
    } else {
        if (lbl_80202898.b10) {
            fTicks *= 2.0f;
        }
        if (lbl_80202898.b11) {
            fTicks *= 0.5f;
            lbl_80202898.n28++;
        }
    }
    if (lbl_80202898.bSlowMo) {
        return 1.0f / 59.94f * fTicks * lbl_80202898.fSlowMo;
    }
    return 1.0f / 59.94f * fTicks;
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

// The player's current target (an inline in EA's source; calling fn_800F1D34 directly does not match).
inline int GE_CurrentTarget(int nPlayer) {
    return fn_800F1D34(nPlayer);
}

// TW06: GameEffects_CheckScriptedGB (by position). A scripted GameBreaker for a record chance:
// reason 12 while the round can still beat the course record, reason 15 when the drive beats the
// longest-drive record (the record is in feet). Only for a human, one view, not in a replay, and
// only with the game's GameBreaker option on.
void fn_800DB30C(int nPlayer, int nReason) {
    if ((!(gSession.uFlags & 0x4000) || !(gSession.uFlags & 0x8000)) && !gSession.bReplay &&
        !gSession.nSplitScreen && !gSession.unk8[0]) {
        if (!gpGame->b285) {
            return;
        }
        if (lbl_80202898.bGameBreaker != 1 && !Player_IsCPU(nPlayer)) {
            if (nReason == 12) {
                if (fn_800E17AC(nPlayer) + 1 >= COURSE_RECORD(0)) {
                    return;
                }
            } else if (nReason == 15 && !(3.0f * gPlayers[nPlayer].fA64 > COURSE_RECORD(0xC8))) {
                return;
            }
            GB_START(nPlayer, nReason);
        }
    }
}

// The same for the challenge modes on course 7: mode 14 (reason 17), mode 15 when every other
// player is out (22), modes 16 and 17 on the last target (23).
void fn_800DB4E8(int nPlayer) {
    u8 bStart = 0;
    int nReason;
    int i;
    int n;
    if (!gSession.bReplay && !gSession.nSplitScreen && !gSession.unk8[0]) {
        switch (Game_GetCourse()) {
        case 7:
            break;
        default:
            return;
        }
        if (lbl_80202898.bGameBreaker != 1) {
            if (Game_GetMode() == 14) {
                if (fn_800F354C(nPlayer) == 4) {
                    bStart = 1;
                    nReason = 17;
                }
            } else if (Game_GetMode() == 15) {
                n = 0;
                for (i = 0; i < gNumPlayersSetUp; i++) {
                    if (i != nPlayer && gPlayers[i].nE88 < 5) {
                        n++;
                    }
                }
                if (n == 0) {
                    bStart = 1;
                    nReason = 22;
                }
            } else if (Game_GetMode() == 17) {
                if (fn_800F20C0(nPlayer) == 39) {
                    bStart = 1;
                    nReason = 23;
                }
            } else if ((Game_GetMode() == 16 || Game_GetMode() == 16) && fn_800F20C0(nPlayer) == 39 &&
                       gPlayers[nPlayer].nDE4[GE_CurrentTarget(nPlayer)] == 0) {
                bStart = 1;
                nReason = 23;
            }
            if (bStart) {
                GB_START(nPlayer, nReason);
            }
        }
    }
}

// TW06: GameEffects_IsGBPossible (by position). Whether this lie is worth a GameBreaker: on the
// green putting for two under par or better, or one of the other big-putt checks, or a birdie or
// eagle putt (by the score after a tap-in).
int fn_800DB86C(int nPlayer) {
    int bPossible = 0;
    int nPar;
    int nStrokes;
    CourseInfo* pCourse;
    void* pSurface;
    f32 fDist;
    pCourse = fn_8000C594();
    if (!pCourse) {
        return 0;
    }
    if (!gpGame->b285) {
        return 0;
    }
    nPar = fn_800D2B08();
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    pSurface = fn_800CC190(pCourse, gPlayers[nPlayer].vTarget2);
    fDist = fn_800D0478(nPlayer);
    if (!pSurface) {
        return 0;
    }
    if (gPlayers[nPlayer].nLie == LIE_GREEN && nPar - nStrokes >= 2) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].nLie == LIE_GREEN &&
               fn_800D8750(2, fDist, 0, gpSaveData + nPlayer * 0x10600 + 1, nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].nLie == LIE_GREEN && fn_800DCB10(nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].nLie == LIE_GREEN && fn_800BCD24(nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].nLie == LIE_GREEN && fn_800D0620(nPlayer, 0, 0) == 11 &&
               Hole_ScoreAfterTapIn(nPlayer) < 0) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].nLie == LIE_GREEN && fn_800D089C(nPlayer, 0) == 1 &&
               Hole_ScoreAfterTapIn(nPlayer) < -1) {
        bPossible = 1;
    }
    return bPossible;
}

// The GameBreaker camera: none on course 7; otherwise a camera at the shot's full distance.
void fn_800DB714(int nPlayer) {
    int nLie;
    f32 fDist;
    void* pView;
    if ((!(gSession.uFlags & 0x4000) || !(gSession.uFlags & 0x8000)) && lbl_80202898.bGameBreaker) {
        if (Game_GetCourse() == 7) {
            lbl_80202898.b19 = 1;
            return;
        }
        nLie = gPlayers[nPlayer].nLie;
        fDist = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        fDist *= fn_800510EC(gPlayers[nPlayer].ball);
        fDist *= fn_8005B64C(nPlayer);
        fn_80045494(0, nPlayer);
        fn_80045558(0, nPlayer);
        pView = fn_80017028(gPlayers[nPlayer].nView0);
        *(s32*)((u8*)pView + 0x74) = fn_8003BDBC(nPlayer, nLie, 3, 0xC, 1, fDist);
        lbl_80202898.b19 = 1;
        if (lbl_80202898.f24 > 0.8f) {
            lbl_80202898.f24 = 0.8f;
        }
    }
}

// TW06: GameEffects_StartPredictedGB (by position). The look-ahead ball says the shot drops: a
// predicted GameBreaker starts, for a human's shot that went far enough (1 with the putter, 10 for
// a chip, 5 otherwise), with its own camera; a golfer mid-swing may get a reaction animation.
void fn_800DBA50(int nPlayer) {
    int nClass;
    int nLie;
    void* pView;
    int nCam;
    u32 uTarget;
    f32 fDist;
    f32 fTime;
    s32 n14;
    s32 nKind;
    s32 nC;
    s32 n8;
    f32 v2[4];
    f32 v[4];
    if ((!(gSession.uFlags & 0x4000) || !(gSession.uFlags & 0x8000)) && !gSession.bReplay &&
        !gSession.nSplitScreen && !gSession.unk8[0]) {
        if (!gpGame->b285) {
            return;
        }
        if (!(gPlayers[nPlayer].uFlags & 8) && !Player_IsCPU(nPlayer) && lbl_80202898.bGameBreaker != 1) {
            fn_800DCB84(gPlayers[nPlayer].ball, gPlayers[nPlayer].ballBefore, v);
            v[1] = 0.0f;
            fDist = fn_80009680(fn_80009744(v));
            if (gPlayers[nPlayer].nClub == 25) {
                if (fDist < 1.0f) {
                    return;
                }
            } else if (gPlayers[nPlayer].nShotKind == 1) {
                if (fDist < 10.0f) {
                    return;
                }
            } else if (fDist < 5.0f) {
                return;
            }
            if (!gPlayers[nPlayer].unk30C[1] && fn_8000C594()) {
                fn_800DCB84(gPlayers[nPlayer].ball + 0x40, gPlayers[nPlayer].ballBefore, v2);
                v2[1] = 0.0f;
                fDist = fn_80009680(fn_80009744(v2));
                nLie = gPlayers[nPlayer].nLie;
                if (*(s32*)(gPlayers[nPlayer].ballBefore + 0x74) >= 0) {
                    nClass = gSurfaceTypes[*(s32*)(gPlayers[nPlayer].ballBefore + 0x74)].nClass;
                } else {
                    nClass = 10;
                }
                lbl_80202898.bClosing = 0;
                lbl_80202898.bGameBreaker = 1;
                lbl_80202898.fGBTime = 0.0f;
                lbl_80202898.f24 = 0.0f;
                lbl_80202898.nGBType = 1;
                lbl_80202898.nPlayer = nPlayer;
                lbl_80202898.bPaused = 0;
                lbl_80202898.nHeartbeats = 0;
                fn_80045494(0, nPlayer);
                fn_80045558(0, nPlayer);
                pView = fn_80017028(gPlayers[nPlayer].nView0);
                nCam = fn_8003BDBC(nPlayer, nLie, nClass, 0xB, 1, fDist);
                uTarget = fn_8003A950(nCam, 0, &nKind, &fTime, &n14, &nC, &n8, nPlayer);
                if (uTarget && *(u32*)((u8*)pView + 0x130) != uTarget && *(u32*)((u8*)pView + 0x134) != uTarget &&
                    !fn_800451A8((u8*)pView + 0x84, uTarget, nPlayer)) {
                    if (nKind == 5 && fn_8003DC78(uTarget)) {
                        if (gPlayers[nPlayer].nShotKind != SHOT_PUTT && fn_80095780(gPlayers[nPlayer].nShotHandle) != 9) {
                            fn_80095744(gPlayers[nPlayer].nShotHandle, 14);
                            if (0.0f == fTime) {
                                fTime = 1.0f / 59.94f;
                            }
                        }
                    }
                    *(s32*)((u8*)pView + 0x74) = nCam;
                    *(s32*)((u8*)pView + 0x148) = 0;
                    *(s32*)((u8*)pView + 0x14C) = 25;
                }
                EVENT_Trigger(nPlayer, 0x3F, 0, -1);
            }
        }
    }
}

// TW06: GameEffects_EndGB (by position). The letterbox starts closing, with the end event; the
// GameBreaker music stops, or (a scripted one that failed) the old music comes back.
void fn_800DBDA8(void) {
    if (lbl_80202898.bGameBreaker) {
        lbl_80202898.bClosing = 1;
        if (lbl_80202898.fGBTime > 0.8f) {
            lbl_80202898.fGBTime = 0.8f;
        }
        switch (lbl_80202898.nGBType) {
        case 1:
            EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
            if (lbl_80202898.b4A) {
                fn_800BD83C(lbl_80202898.u4C, 0);
                lbl_80202898.b4A = 0;
            }
            fn_800A6DCC(3, 1);
            return;
        case 0:
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
            if (fn_800DC818(gPlayers[lbl_80202898.nPlayer].ball, lbl_80202898.nPlayer, 0)) {
                if (lbl_80202898.b4A) {
                    fn_800BD83C(lbl_80202898.u4C, 0);
                    lbl_80202898.b4A = 0;
                }
                fn_800A6DCC(3, 1);
            } else {
                if (lbl_80202898.b47) {
                    fn_800BD83C(lbl_80202898.u48, 0);
                }
                fn_800A6DCC(lbl_80202898.n4F, 1);
                lbl_80202898.b4E = 0;
            }
            lbl_80202898.b47 = 0;
            break;
        }
    }
}

// TW06: GameEffects_RenderGameBreakerEffects (by position).
void fn_800DBF34(void) {
    if (lbl_80202898.bGameBreaker && gSession.unk14 == 0 && !lbl_80202898.bPaused) {
        switch (lbl_80202898.nGBType) {
        case 1:
            fn_800DBFAC();
            return;
        case 0:
            fn_800DC18C();
            break;
        }
    }
}

// TW06: GameEffects_RenderPredictedGB (by position). While the look-ahead ball says the shot
// will drop: the letterbox, and the slow-down near the hole (within 2 of it for a putt, 4 otherwise)
// for a human.
void fn_800DBFAC(void) {
    f32 fHeight;
    f32 fDist;
    f32 v[3];
    GameEffects* pGE;
    if (lbl_80202898.fGBTime < 0.8f) {
        fHeight = 0.15f * (lbl_80202898.fGBTime / 0.8f);
    } else {
        fHeight = 0.15f;
    }
    pGE = &lbl_80202898;       // steers the register choice (found by the permuter)
    fn_800DCB84(gPlayers[pGE->nPlayer].ball, gPlayers[pGE->nPlayer].ballBefore, v);
    v[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(v));
    if (!Player_IsCPU(lbl_80202898.nPlayer)) {
        if (gPlayers[lbl_80202898.nPlayer].nShotKind == SHOT_PUTT) {
            if (fDist < 2.0f && !lbl_80202898.bClosing) {
                fn_80045494(1, lbl_80202898.nPlayer);
            } else {
                fn_80045494(0, pGE->nPlayer);
            }
        } else if (fDist < 4.0f && !lbl_80202898.bClosing) {
            fn_80045494(1, pGE->nPlayer);
        } else {
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        fn_80045494(0, pGE->nPlayer);
    }
    fn_800DC290(fHeight);
    if (lbl_80202898.bClosing) {
        lbl_80202898.fGBTime -= gSession.fFrameTime;
        if (lbl_80202898.fGBTime < 0.0f) {
            lbl_80202898.bGameBreaker = 0;
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        lbl_80202898.fGBTime += gSession.fFrameTime;
    }
}

// TW06: GameEffects_RenderScriptedGB (by position). The letterbox opens (or closes) at the
// game's pace; once closed the GameBreaker is over.
void fn_800DC18C(void) {
    f32 fHeight;
    if (lbl_80202898.b19) {
        lbl_80202898.f24 -= gSession.fFrameTime;
    } else {
        lbl_80202898.f24 += gSession.fFrameTime;
    }
    if (lbl_80202898.b19) {
        if (lbl_80202898.fGBTime < 0.8f) {
            fHeight = 0.15f * (lbl_80202898.fGBTime / 0.8f);
        } else {
            fHeight = 0.15f;
        }
        fn_800DC290(fHeight);
        if (lbl_80202898.bClosing) {
            lbl_80202898.fGBTime -= gSession.fFrameTime;
            if (lbl_80202898.fGBTime < 0.0f) {
                lbl_80202898.bGameBreaker = 0;
            }
        } else {
            lbl_80202898.fGBTime += gSession.fFrameTime;
        }
    }
}

// TW06: GameEffects_DrawLetterBoxes (by position). Two half-transparent black bars of fHeight
// (a fraction of the screen), at the top and bottom.
void fn_800DC290(f32 fHeight) {
    f32 colour[4];
    f32 xy[8];
    f32 uv[8];
    fn_8001425C(0);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012F50(0, 6, 0x80);
    fn_80014118(0);
    fn_80012EF8();
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
    colour[3] = 0.5f;
    fn_800141F8(xy, uv, 0.0f, 0.0f, 1.0f, fHeight);
    fn_80014194(colour);
    fn_8001644C(0xA1, xy, 0, uv, 2);
    fn_800141F8(xy, uv, 0.0f, 1.0f - fHeight, 1.0f, 1.0f);
    fn_80014194(colour);
    fn_8001644C(0xA1, xy, 0, uv, 2);
    fn_80012F34(1);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012EF8();
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
