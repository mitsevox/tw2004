// GameMode10.c (our name): game mode 10, playing back a saved shot (gReplayData), then the code the
// target games (modes 13..17) share: a list of target points, the target a player aims at, the
// shot multiplier, and dispatchers into each mode's own file.

#include "golfer.h"
#include "ball.h"

// A saved shot (gReplayData, 0x801D6030): the seed, player 0 as it was, and the conditions.
typedef struct Replay {
    u32    nSeed;               // 0x000
    u8     unk4[4];
    Player player;              // 0x008  player 0 before the shot
    s32    nCourse;             // 0xF00
    s16    nHole;               // 0xF04
    s8     nTeeSet;             // 0xF06
    s8     nF07;                // 0xF07
    f32    fF08;                // 0xF08
    f32    fF0C;                // 0xF0C
    u8     bF10;                // 0xF10  in-flight replays are on (GameManager.c)
    u8     unkF11;
    s16    nF12;                // 0xF12  1..3: fn_800ED6F8 is set from nF14
    s16    nF14;                // 0xF14  hundredths
    s16    nWindDir;            // 0xF16
    s16    nWindSpeed;          // 0xF18
    s16    nF1A;                // 0xF1A  -> fn_80055C40
    s16    nF1C;                // 0xF1C  -> fn_80055CAC
    s16    nF1E;                // 0xF1E  -> fn_80055CD0
    s16    nStrokes;            // 0xF20  strokes on the hole before the shot
} Replay;
extern Replay gReplayData;

// The target list of the target games: up to lbl_80282360 points (w = 1).
extern f32 lbl_80211D38[][4];
extern s8  lbl_80282360;
#define TARGETS lbl_80211D38
#define NUM_TARGETS lbl_80282360

// Prize rows (0x1C bytes) at lbl_80200538 + 0x710; the row with id 999 holds the target games' prizes.
typedef struct MiniPrize {
    s32 nId;                    // 0x00
    s32 nMode13;                // 0x04
    s32 nMode16;                // 0x08
    s32 nMode17;                // 0x0C
    u8  unk10[0xC];
} MiniPrize;
typedef struct PrizeTable {
    u8        unk0[0x710];
    MiniPrize mini[20];         // 0x710
} PrizeTable;
extern PrizeTable lbl_80200538;
#define MINI_PRIZES lbl_80200538.mini

extern f32 lbl_80192810[];
extern f32 lbl_80192844[];
extern f32 lbl_80192880[];
extern u8 gNumPlayersSetUp;

void  fn_800E1480(int nHole);
void  fn_800E14E0(int nCourse);
void  fn_800E1260(int nPreset);
void  fn_800E1404(int nHole);
void  Session_SetNumPlayers(int n);
void  fn_800ED6F8(f32 x0);
void  Wind_Set(int nDir, f32 fSpeed);
void  fn_80055C40(int n);
void  fn_80055CAC(int n);
void  fn_80055CD0(int n);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);
void  Vec_Copy(f32* pSrc, f32* pDst);
f32   Vec_Distance(f32* pA, f32* pB);
int   Game_CurHoleIndex(void);
void  fn_80055AA8(Ball* pBall, f32* pPos, int nPlayer);
u8    Physics_DropBall(Ball* pBall, f32* pPos);
void  fn_8001C774(int nHandle, int nClub);
void  fn_8001C724(int nHandle, int nKind);
void  fn_8006BF60(int nPlayer);
void  fn_8000B1D4(int nStream, u32 nSeed);
void  GOLFERSTATE_Switch(int nState, int nPlayer);
CourseInfo* fn_8000C594(void);
int   fn_8001D324(int n);
void  Character_SetPosition(int nHandle, f32* pPos, int a);
void  AI_DefaultTarget(int nPlayer);
void  Shot_Prepare(int nPlayer, u8 bNotify);
void  fn_8001C804(int nPlayer, int a, int b);
void  fn_800957D8(int nHandle);
void  fn_80095744(int nHandle, int nAnim);
void  fn_80062C38(void);
u32   Rand_Next(int nStream);
void  fn_800A631C(void);
int   fn_80015464(void);
void  fn_800A7664();
s32   fn_800F7DE8();
s32   fn_800F3828();
s32   fn_800F48C4();
s32   fn_800F37F8(s32);
s32   fn_800F59CC(s32);
s32   fn_800F6A00(s32);
s32   fn_800F7D94(s32);
s32   fn_800F7D9C(s32);
s32   fn_800F59D4(s32);
s32   fn_800F80A0(s32);
s32   fn_800F6A34(s32);

void fn_800F1388(void);
void fn_800F1404(void);
void fn_800F1424(void);
void fn_800F15AC(void);
void fn_800F18C8(void);
u8   fn_800F193C(int nPlayer, int a);
u8   fn_800F1944(int a);
void fn_800F194C(void);
void fn_800F1ABC(int nPlayer, s8 n);
void fn_800F2030(void);
u8   fn_800F2358(int nPlayer);
void fn_800F2958(s32 p0, s32 p1);

// Mode 10 starts: one player, no mulligans, the saved shot's hole.
void fn_800F125C(void) {
    gpGame->pfn1C8 = fn_800F125C;
    gpGame->pfn1D0 = fn_800F15AC;
    gpGame->pfn1D8 = fn_800F193C;
    gpGame->pfn1DC = fn_800F1944;
    gpGame->pfn1EC = fn_800F1424;
    gpGame->pfn1F4 = fn_800F194C;
    gpGame->pfn1E4 = fn_800F1388;
    gpGame->pfn224 = fn_800F1404;
    gpGame->b273 = 0;
    gpGame->b276 = 0;
    gpGame->b27B = 0;
    gpGame->b27C = 0;
    gpGame->b27D = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
    gSession.nNumPlayers = 1;
}

// Hole start: the saved wind and conditions.
void fn_800F1388(void) {
    Wind_Set(gReplayData.nWindDir, gReplayData.nWindSpeed);
    fn_80055C40(gReplayData.nF1A);
    fn_80055CAC(gReplayData.nF1C);
    fn_80055CD0(gReplayData.nF1E);
}

void fn_800F1404(void) {
    fn_800F18C8();
}

// Sets the session's nF07 from the replay and returns it (an inline in EA's source).
static inline s8 Replay_SetF07(void) {
    return gSession.unk5B38 = gReplayData.nF07;
}

// Round setup: the saved course, hole and tees.
void fn_800F1424(void) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->holeOrder[i] = Replay_SetF07();
    }
    fn_800E14E0(gReplayData.nCourse);
    fn_800E1260(0);
    fn_800E1404(gReplayData.nHole);
    Session_SetNumPlayers(1);
    gSession.bReplay = 1;
    SESSION_OPTIONS->unkC = gReplayData.nF12;
    if (gReplayData.nF12 == 3) {
        fn_800ED6F8(gReplayData.nF14 / 100.0f);
    }
    gSession.nTeeSet[0] = gReplayData.nTeeSet;
    Replay_SetF07();
}

// Put player 0 back as they were before the shot, then start it.
void fn_800F15AC(void) {
    Ball ball;
    f32 fF08;
    f32 fF0C;
    u32 nSeed;
    s16 nWindDir;
    s16 nWindSpeed;
    s16 nF12;
    s16 nF14;

    Mem_cpy(&gPlayers[0].golfer, &gReplayData.player.golfer, 0x140);
    Mem_cpy(gPlayers[0].attrMod, gReplayData.player.attrMod, 0xC);
    Mem_cpy(gPlayers[0].nStrokes, gReplayData.player.nStrokes, 0x1B8);
    Mem_cpy(gPlayers[0].unk30C, gReplayData.player.unk30C, 0x48);
    Mem_cpy(&gPlayers[0].nClub, &gReplayData.player.nClub, 0x5C);
    Mem_cpy(&gPlayers[0].nShotKind2, &gReplayData.player.nShotKind2, 4);
    Mem_cpy(&gPlayers[0].swing, &gReplayData.player.swing, 0x630);
    gPlayers[0].uFlags = gReplayData.player.uFlags;
    Vec_Copy(&gReplayData.player.fBallX, &gPlayers[0].fBallX);
    Vec_Copy(gReplayData.player.vPreShot, gPlayers[0].vPreShot);
    Vec_Copy(&gReplayData.player.fTargetX, &gPlayers[0].fTargetX);
    Vec_Copy(gReplayData.player.vTargetCopy, gPlayers[0].vTargetCopy);
    Vec_Copy(gReplayData.player.vTarget2, gPlayers[0].vTarget2);
    Vec_Copy(gReplayData.player.vA44, gPlayers[0].vA44);
    gPlayers[0].nStrokes[Game_CurHoleIndex()] = gReplayData.nStrokes;
    gPlayers[0].fDistance = gReplayData.player.fDistance;
    gPlayers[0].fDistance2 = gReplayData.player.fDistance2;
    Mem_cpy(gPlayers[0].ball, gReplayData.player.ball, 0xBC);
    fn_80055AA8(&ball, (f32*)gReplayData.player.ball, 0);
    gPlayers[0].pBallCourse = ball.pCourse;
    gPlayers[0].nBallOwner = 0;
    Physics_DropBall(&ball, (f32*)gReplayData.player.ball);
    fn_8001C774(gPlayers[0].nShotHandle, gPlayers[0].nClub);
    fn_8001C724(gPlayers[0].nShotHandle, gPlayers[0].nShotKind);
    gPlayers[0].swing.bUIInit = 0;
    fF08 = gReplayData.fF08;
    gSession.bReplay = 0;
    fF0C = gReplayData.fF0C;
    nSeed = gReplayData.nSeed;
    nWindDir = gReplayData.nWindDir;
    nWindSpeed = gReplayData.nWindSpeed;
    nF12 = gReplayData.nF12;
    nF14 = gReplayData.nF14;
    fn_8006BF60(0);
    gSession.bReplay = 1;
    gReplayData.fF08 = fF08;
    gReplayData.fF0C = fF0C;
    gSession.nSeed = nSeed;
    gReplayData.nSeed = nSeed;
    gReplayData.nWindDir = nWindDir;
    gReplayData.nWindSpeed = nWindSpeed;
    gReplayData.nF12 = nF12;
    gReplayData.nF14 = nF14;
    fn_8000B1D4(0, nSeed);
    GOLFERSTATE_Switch(1, 0);
}

void fn_800F18C8(void) {
    if (gReplayData.nF12 == 1 || gReplayData.nF12 == 2 || gReplayData.nF12 == 3) {
        fn_800ED6F8(gReplayData.nF14 / 100.0f);
    }
}

u8 fn_800F193C(int nPlayer, int a) {
    return 1;
}

u8 fn_800F1944(int a) {
    return 1;
}

void fn_800F194C(void) {
    gSession.unk11[1] = 1;
}

// The target games' target list.
int fn_800F1960(void) {
    return NUM_TARGETS;
}

void fn_800F196C(int i, f32* pOut) {
    Vec_Copy(TARGETS[i], pOut);
}

void fn_800F199C(f32 x, f32 y, f32 z) {
    TARGETS[NUM_TARGETS][0] = x;
    TARGETS[NUM_TARGETS][1] = y;
    TARGETS[NUM_TARGETS][2] = z;
    TARGETS[NUM_TARGETS][3] = 1.0f;
    NUM_TARGETS++;
}

// Sort the targets by distance from the tee, nearest first.
void fn_800F19D4(void) {
    f32 tmp[4];
    int i;
    int j;
    f32* pTee = (f32*)((u8*)fn_8000C594() + gSession.nTeeSet[0] * 0x10 + 0xB0);
    for (i = 0; i < NUM_TARGETS - 1; i++) {
        for (j = i + 1; j < NUM_TARGETS; j++) {
            if (Vec_Distance(TARGETS[i], pTee) > Vec_Distance(TARGETS[j], pTee)) {
                Vec_Copy(TARGETS[i], tmp);
                Vec_Copy(TARGETS[j], TARGETS[i]);
                Vec_Copy(tmp, TARGETS[j]);
            }
        }
    }
}

// Aim the player at target n (wrapping round): the pin moves there.
void fn_800F1ABC(int nPlayer, s8 n) {
    u32 h;
    gPlayers[nPlayer].nTarget = n % NUM_TARGETS;
    Vec_Copy(TARGETS[gPlayers[nPlayer].nTarget], (f32*)fn_8000C594()->pin);
    h = fn_8001D324(100);
    if (h) {
        Character_SetPosition(h, TARGETS[gPlayers[nPlayer].nTarget], 1);
    }
}

void fn_800F1B60(int nPlayer, s8 n) {
    fn_800F1ABC(nPlayer, n);
    AI_DefaultTarget(nPlayer);
    Shot_Prepare(nPlayer, 1);
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].nShotHandle);
    fn_80095744(gPlayers[nPlayer].nShotHandle, 5);
    fn_80062C38();
}

// Previous target.
u8 fn_800F1BD8(int nPlayer) {
    if (gPlayers[nPlayer].nTarget == 0) {
        fn_800F1ABC(nPlayer, NUM_TARGETS - 1);
    } else {
        fn_800F1ABC(nPlayer, gPlayers[nPlayer].nTarget - 1);
    }
    return 1;
}

// Next target.
u8 fn_800F1C34(int nPlayer) {
    fn_800F1ABC(nPlayer, gPlayers[nPlayer].nTarget + 1);
    return 1;
}

// The target nearest the ball.
s8 fn_800F1C74(int nPlayer) {
    f32* pBall = (f32*)gPlayers[nPlayer].ball;
    s8 i;
    s8 nBest = 0;
    f32 fBest = Vec_Distance(TARGETS[0], pBall);
    for (i = 1; i < NUM_TARGETS; i++) {
        f32 f = Vec_Distance(TARGETS[i], pBall);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

// The target nearest the player's aim point.
int fn_800F1D34(int nPlayer) {
    f32* pTarget = &gPlayers[nPlayer].fTargetX;
    int i;
    int nBest = 0;
    f32 fBest = Vec_Distance(TARGETS[0], pTarget);
    for (i = 1; i < NUM_TARGETS; i++) {
        f32 f = Vec_Distance(TARGETS[i], pTarget);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

void fn_800F1DF0(void) {
    if (Game_GetMode() == 0xD) {
        fn_800F7DE8();
    }
}

void fn_800F1E1C(void) {
    if (Game_GetMode() == 0xE) {
        fn_800F3828();
    }
    if (Game_GetMode() == 0xF) {
        fn_800F48C4();
    }
}

s32 fn_800F1E58(s32 n) {
    switch (n) {
    case 0x85: return 0;
    case 0x86: return 1;
    case 0x87: return 2;
    case 0x88: return 0;
    case 0x89: return 1;
    case 0x8A: return 2;
    case 0x8B: return 3;
    case 0x8C: return 0;
    case 0x8D: return 1;
    case 0x8E: return 2;
    case 0x8F: return 3;
    case 0x90: return 4;
    default: return 5;
    }
}

// Every player's target-game state is cleared and aimed at the first target.
void fn_800F1EE4(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        Player* p = PLAYER(i);
        p->nDC0 = 0;
        for (j = 0; j < 5; j++) {
            p->aDC4[j] = 0;
        }
        p->nDD8 = 0;
        p->nDDC = 0;
        p->nDE0 = 0;
        p->nE88 = 0;
        p->nE8C = 0;
        p->nE90 = 0;
        p->nE94 = 0;
        p->nE98 = 0;
        p->bE9D = 0;
        p->bE9E = 0;
        for (j = 0; j < 40; j++) {
            p->nDE4[j] = 0;
        }
        fn_800F1ABC(i, 0);
    }
    fn_800F2030();
}

void fn_800F2030(void) {
    int i;
    int j;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nCD0 = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nDB8 = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aCD4[j] = 0;
        }
    }
}

// How many targets the player has hit.
s32 fn_800F20C0(int nPlayer) {
    int i;
    s32 n = 0;
    for (i = 0; i < 40; i++) {
        if (gPlayers[nPlayer].nDE4[i] != 0) {
            n++;
        }
    }
    return n;
}

// After a shot, maybe a multiplier for the next one: x5, x3 or x2, more often after 10 shots
// without one.
void fn_800F21B4(int nPlayer) {
    s32 nMsg = -1;
    s32 r;
    if (!fn_800F2358(nPlayer)) {
        if (gPlayers[nPlayer].nE98 >= 10) {
            r = Rand_Next(0) % 20;
        } else {
            r = Rand_Next(0) % 100;
        }
        if (r <= 5) {
            gPlayers[nPlayer].nDBC = 5;
        } else if (r <= 10) {
            gPlayers[nPlayer].nDBC = 3;
        } else if (r <= 20) {
            gPlayers[nPlayer].nDBC = 2;
        }
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].nE98 = 0;
    } else {
        gPlayers[nPlayer].nE98++;
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        fn_800A631C();
        switch (gPlayers[nPlayer].nDBC) {
        case 2:
            nMsg = 0x3A;
            break;
        case 3:
            nMsg = 0x3B;
            break;
        case 4:
            break;
        case 5:
            nMsg = 0x3D;
            break;
        }
    }
    if (nMsg != -1) {
        fn_800F2958((u16)nMsg, 0);
    }
}

// The player's game is over (all shots taken).
u8 fn_800F2358(int nPlayer) {
    if (Game_GetMode() == 0x10) {
        if (gPlayers[nPlayer].nDC0 == 20) {
            return 1;
        }
    } else if (Game_GetMode() == 0x11) {
        if (gPlayers[nPlayer].nDC0 == 5) {
            return 1;
        }
    } else if (gPlayers[nPlayer].nDC0 == 0) {
        return 1;
    }
    return 0;
}

s32 fn_800F2408(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D94(arg0);
    }
    if (Game_GetMode() == 0xE) {
        return fn_800F37F8(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59CC(arg0);
    }
    if (Game_GetMode() == 0x11) {
        return fn_800F6A00(arg0);
    }
    return 0;
}

s32 fn_800F2494(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D9C(arg0);
    }
    return 0;
}

s32 fn_800F24D8(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F80A0(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59D4(arg0);
    }
    return 0;
}

s32 fn_800F2534(s32 arg0) {
    if (Game_GetMode() == 0x11) {
        return fn_800F6A34(arg0);
    }
    return 0;
}

// The target game's prize (from the prize row with id 999).
s32 fn_800F2578(void) {
    int i;
    for (i = 0; i < 20; i++) {
        if (MINI_PRIZES[i].nId == 999) {
            if (Game_GetMode() == 0xD) {
                return MINI_PRIZES[i].nMode13;
            }
            if (Game_GetMode() == 0x10) {
                return MINI_PRIZES[i].nMode16;
            }
            if (Game_GetMode() == 0x11) {
                return MINI_PRIZES[i].nMode17;
            }
        }
    }
    return 0;
}

void fn_800F263C(s32 p0) {
    fn_800F2958((p0 & 0xFFFF), 1);
}

void fn_800F2664(void) {
}

void fn_800F2668(void) {
}

// Scale n by table entry i; which table depends on fn_80015464 (0..2).
s32 fn_800F266C(s32 n, int i) {
    if (fn_80015464() == 0) {
        return n * lbl_80192810[i];
    }
    if (fn_80015464() == 1) {
        return n * lbl_80192844[i];
    }
    if (fn_80015464() == 2) {
        return n * lbl_80192880[i];
    }
    return n;
}

// Is f far enough for the player's tees (313 back, 300 middle, 293 front)?
u8 fn_800F2788(int nPlayer, f32 f) {
    switch (gSession.nTeeSet[nPlayer]) {
    case 0:
        if (f >= 313.0f) {
            return 1;
        }
        break;
    case 1:
        if (f >= 300.0f) {
            return 1;
        }
        break;
    case 2:
    case 3:
        if (f >= 293.0f) {
            return 1;
        }
        break;
    }
    return 0;
}

s32 fn_800F2810(s32 n) {
    if (fn_80015464() == 0) {
        switch (n) {
        case 0xD7: return 0;
        case 0xD5: return 1;
        case 0xD4: return 2;
        case 0xD6: return 3;
        case 0xD8: return 4;
        }
    } else if (fn_80015464() == 1) {
        switch (n) {
        case 0x3B: return 0;
        case 0x3C: return 1;
        case 0x3D: return 2;
        case 0x3E: return 3;
        case 0x3F: return 4;
        }
    } else if (fn_80015464() == 2) {
        switch (n) {
        case 24: return 0;
        case 25: return 1;
        case 26: return 2;
        case 27: return 3;
        case 28: return 4;
        }
    }
    return 4;
}

void fn_800F2958(s32 p0, s32 p1) {
    fn_800A7664(7, p0, p1);
}
