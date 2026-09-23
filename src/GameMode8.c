// GameMode8.c (our name): speed golf. Each hole scores the time taken (n290, in seconds) plus 30
// seconds per stroke. The golfer runs to the ball between shots (custom golfer states 12 and
// 24..26 replace the normal ones). Mode 8 is solo; modes 7 and 6 (GameMode7.c, GameMode6.c) are
// the two-player stroke and match versions.

#include "golfer.h"
#include "game.h"
#include "engine.h"

typedef struct SwingStateDef {
    void (*pfnEnter)(int nPlayer);  // 0x00
    void (*pfnUpdate)(int nPlayer); // 0x04
    void (*pfnExit)(int nPlayer);   // 0x08
} SwingStateDef;
extern SwingStateDef sGolferStateEngineTable[];        // 0x801883D8

void  STATEFUNC_SimulateInit(int nPlayer);
void  STATEFUNC_SimulateUpdate(int nPlayer);
void  STATEFUNC_SimulateExit(int nPlayer);
void  GM_MovePlayerToBall(int nPlayer);
void  Shot_Plan(int nPlayer, int a);
void  fn_8001D8DC(int nPlayer);
void  fn_80062CB0(int a, int b);
void  fn_80062F1C(void);
s32   fn_800E27C0(void);
void  fn_800E3C70(int a);
void  fn_800E3CD4(int a);
void  fn_800F80D4(s32 p0);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern u8* gpSaveData;
extern u8  lbl_8028227C;
extern u8  lbl_802823C9;
extern u8  lbl_802823CA;
extern s32 lbl_802823D0;
extern s32 lbl_802823D4;
extern u16 lbl_80192BA8[];                  // per event, a sound (0xFFFF = none)
// Three values per course, largest first (750, 675 and 600 for the first course).
typedef struct SGCourse {
    s32 n0;                     // 0x0
    s32 n4;                     // 0x4
    s32 n8;                     // 0x8
} SGCourse;
extern SGCourse lbl_80192C00[];

// The events of the two-player game: flags set on the player and points won from the other player.
typedef struct SGEvent {
    u64 uFlags;                 // 0x0  or'd into the player's uC48
    s32 nPoints;                // 0x8
    u8  unkC[4];
} SGEvent;
extern SGEvent lbl_80192908[];
// The last 100 events.
typedef struct SGLog {
    s32 nEvent;
    s32 nPlayer;
} SGLog;
extern SGLog lbl_802120F8[100];
extern s32 lbl_802823CC;
extern u8  lbl_802823C8;
void  GM_PlayerAddStroke(int nPlayer);
u8    GM_CheckForBallOOB(int nPlayer);
void  GM_ReplaceOOBBall(int nPlayer);
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, u8* pBall);
void  fn_800DEB5C(int nPlayer);
u8    fn_800A7720(void);
void  fn_800FE190(f32* pA, f32* pB, f32* pOut);
u8*   fn_800136C4(int nController);         // the pad's state: stick bytes at +0..+3
u32   fn_800136DC(int nController);         // buttons: held << 16 | pressed this frame
u32   fn_800142AC(int nButton, int a);      // a button's mask
extern f32 lbl_802816B0;
extern f32 lbl_802816B4;
extern f32 lbl_802816C0;
extern f32 lbl_802816C4;
extern f32 lbl_802816B8;
extern f32 lbl_802816BC;
f32   fn_8000AD78(f32 y, f32 x);                // atan2f
void  fn_800BAF04(f32* pSrc, f32* pDst);        // normalise
int   Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode);
void  GM_SimulateBallMovement(int nPlayer);
u8    fn_80058F5C(int nPlayer);                 // the per-frame swing poll: the ball was struck
void  fn_800ED710(s32 p0);
void  Vec_Normalize(f32* pSrc, f32* pDst);
u8    Ter_PointInOOBNetwork(u8* pBall);
void  fn_80069330(int nPlayer, f32* pPos);
void  fn_8006A6C4(int nPlayer);
void  PlaceBall_UpdateMomentums(int nPlayer, f32 f);
u8*   fn_80016CFC(int nView);
u8*   fn_80008370(u8* p);
extern f32 lbl_801D5888[4][4];                  // per player: where the ball was last on the course

// The tee positions follow the pins in the per-hole data (fn_8000C594).
typedef struct HoleTees {
    u8     unk0[0xB0];
    PinPos tee[4];                          // 0xB0  one per tee set
} HoleTees;
void  fn_8006ACF8(int nPlayer, int a);
void  fn_8006BAA8(int nPlayer);
void  fn_800FE100(s32 p0, s32 p1, s32 p2);
void  fn_800FA554(int nPlayer);

void  fn_800F9844(void);
u8    fn_800F9C00(int nPlayer, int a);
void  fn_800F9C48(void);
u8    fn_800F9D00(u8 bCheck);
void  fn_800F9E00(void);
u8    fn_800F9F04(u8 bCheck);
u8    fn_800FA148(int a);
u8    fn_800FA1CC(int nPlayer, int a);
u8    fn_800FA26C(int a);
s32   fn_800FA2C8(void);
void  fn_800FA2D0(void);
void  fn_800FA3AC(void);
void  fn_800FA410(void);
s32   fn_800FA48C(int nPlayer, int nHole);
s32   fn_800FA4B8(int nPlayer);
void  fn_800FA608(int nPlayer);
void  fn_800FA844(int nPlayer);
void  fn_800FA994(int nPlayer);
void  fn_800FA998(int nPlayer);
void  fn_800FA9E0(int nPlayer);
void  fn_800FB35C(int nPlayer, int nOther);
f32   fn_800FB41C(f32* pA, f32* pB);
void  fn_800FBD2C(int nPlayer);
void  fn_800FCBDC(int nPlayer);
void  fn_800FCCF0(void);
void  fn_800FD1C0(int nPlayer);
void  fn_800FD534(int nPlayer);
void  fn_800FD6A0(int nPlayer);
void  fn_800FDF38(void);
void  fn_800FDFC4(s32 p0, s32 p1, s32 p2);
void  fn_800FDFFC(s32 p0, s32 p1);
void  fn_800FE02C(void);
void  fn_800FE054(s32 p0, s32 p1);
void  fn_800FE080(s32 p0, s32 p1);
void  fn_800FE0AC(s32 p0, s32 p1);
void  fn_800FE0D8(void);
void  fn_800FE138(s32 p0, s32 p1);
void  fn_800FE164(s32 p0, s32 p1);

// Mode 8 starts: solo speed golf.
void fn_800F986C(void) {
    gpGame->pfn1C8 = fn_800F986C;
    gpGame->pfn1CC = fn_800F9A58;
    gpGame->pfn1EC = fn_800F9844;
    gpGame->pfn1D0 = fn_800F9B34;
    gpGame->pfn1D4 = fn_800F9BF8;
    gpGame->pfn1D8 = fn_800FA1CC;
    gpGame->pfn1DC = fn_800FA26C;
    gpGame->pfn1E0 = fn_800FA2C8;
    gpGame->pfn1E8 = fn_800FA2D0;
    gpGame->pfn1F4 = fn_800FA410;
    gpGame->pfn1E4 = fn_800F9824;
    gpGame->pfn220 = fn_800FDF38;
    gpGame->pfn230 = fn_800FDF58;
    gpGame->pfn234 = fn_800FDF60;
    gpGame->pfn25C = fn_800FDA30;
    gpGame->b271 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b273 = 0;
    gpGame->b277 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->b283 = 0;
    gpGame->b285 = 0;
    gpGame->b286 = 0;
    gpGame->b288 = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->nDC = 0;
    gpGame->n4 = 1;
    gSession.nSplitScreen = lbl_8028227C;
    Session_SetNumPlayers(1);
}

// Game finished: the normal golfer states go back.
void fn_800F9A58(void) {
    sGolferStateEngineTable[12].pfnEnter = STATEFUNC_SimulateInit;
    sGolferStateEngineTable[12].pfnUpdate = STATEFUNC_SimulateUpdate;
    sGolferStateEngineTable[12].pfnExit = STATEFUNC_SimulateExit;
    sGolferStateEngineTable[24].pfnEnter = NULL;
    sGolferStateEngineTable[24].pfnUpdate = NULL;
    sGolferStateEngineTable[24].pfnExit = NULL;
    sGolferStateEngineTable[25].pfnEnter = NULL;
    sGolferStateEngineTable[25].pfnUpdate = NULL;
    sGolferStateEngineTable[25].pfnExit = NULL;
    sGolferStateEngineTable[26].pfnEnter = NULL;
    sGolferStateEngineTable[26].pfnUpdate = NULL;
    sGolferStateEngineTable[26].pfnExit = NULL;
}

// Round setup: speed golf's golfer states.
void fn_800F9AB0(void) {
    sGolferStateEngineTable[12].pfnEnter = fn_800FA9E0;
    sGolferStateEngineTable[12].pfnUpdate = fn_800FBD2C;
    sGolferStateEngineTable[12].pfnExit = fn_800FCBDC;
    sGolferStateEngineTable[24].pfnEnter = fn_800FA9E0;
    sGolferStateEngineTable[24].pfnUpdate = fn_800FBD2C;
    sGolferStateEngineTable[24].pfnExit = fn_800FCBDC;
    sGolferStateEngineTable[25].pfnEnter = fn_800FA608;
    sGolferStateEngineTable[25].pfnUpdate = fn_800FA844;
    sGolferStateEngineTable[25].pfnExit = fn_800FA994;
    sGolferStateEngineTable[26].pfnEnter = fn_800FD1C0;
    sGolferStateEngineTable[26].pfnUpdate = fn_800FD534;
    sGolferStateEngineTable[26].pfnExit = fn_800FD6A0;
}

// Hole start: everyone not already playing starts (or waits, once holed).
void fn_800F9B34(void) {
    int i;
    s8 nState;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        nState = GOLFERSTATE_GetCurrentState(i);
        if (nState < 1 || (nState > 12 && !fn_800E4254(i))) {
            if (!Player_IsHoled(i)) {
                GOLFERSTATE_Set(1, i);
            } else {
                GOLFERSTATE_Set(19, i);
            }
        }
        PLAYER(i)->nC3C &= ~1;
    }
}

s32 fn_800F9BF8(int nPlayer) {
    return 5;
}

u8 fn_800F9C00(int nPlayer, int a) {
    if (Player_IsHoled(0) || Player_IsHoled(1)) {
        return 1;
    }
    return 0;
}

// Match version, end of hole: the player who holed out wins it.
void fn_800F9C48(void) {
    if (!Player_IsHoled(0) || !Player_IsHoled(1)) {
        if (Player_IsHoled(0)) {
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[0].nHolesWon++;
            return;
        }
        if (Player_IsHoled(1)) {
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[1].nHolesWon++;
        }
    }
}

// Match version: the game is over when a player is up by more than the holes left.
u8 fn_800F9D00(u8 bCheck) {
    int nLeft;
    int h;
    if (gpGame->bD4) {
        if (gPlayers[0].nHolesWon != gPlayers[1].nHolesWon) {
            return 1;
        }
        if (!bCheck) {
            fn_800E2BA4();
        }
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !fn_800F9F04(bCheck);
        }
        if (gPlayers[1].nHolesWon + nLeft < gPlayers[0].nHolesWon ||
            gPlayers[0].nHolesWon + nLeft < gPlayers[1].nHolesWon) {
            return 1;
        }
    }
    return 0;
}

// Match version, EndGame: a human winner with a profile gets the prize.
void fn_800F9E00(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (fn_800E1BBC()) {
        if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
            nMargin = gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
            nWinner = 0;
            nLoser = 1;
        } else {
            nMargin = gPlayers[1].nHolesWon - gPlayers[0].nHolesWon;
            nWinner = 1;
            nLoser = 0;
        }
        nMoney = fn_800D36E0(nWinner, nLoser, nMargin, &nPrize);
        if (!Player_IsCPU(nWinner)) {
            nProfile = gPlayers[nWinner].nIndex;
            if (gpSaveData[nProfile * 0x10600]) {
                if (nPrize) {
                    fn_800E4364(0, 0x6B, nPrize, nProfile);
                }
                fn_800D3548(nWinner, nMoney, NULL);
                gPlayers[nWinner].n330 += nMoney;
            }
        }
    }
}

// Match version, GoToPlayoff: all square after the last hole.
u8 fn_800F9F04(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (gPlayers[0].nHolesWon == gPlayers[1].nHolesWon) {
        if (bCheck) {
            return 1;
        }
        gpGame->bD5 = 1;
        for (h = 0; h < 18; h++) {
            if (!gpGame->bHoleSelected[h]) {
                gpGame->bD5 = 0;
            }
        }
        fn_800E2BA4();
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                PLAYER(i)->nStrokes[h] = 0;
                PLAYER(i)->nModePoints[h] = 0;
            }
        }
        gpGame->bD4 = 1;
        fn_800E45C0();
        return 1;
    }
    return 0;
}

// Stroke version: the hole is over when both have finished (bit 3).
u8 fn_800FA118(int nPlayer, int a) {
    if ((gPlayers[0].nC3C & 8) && (gPlayers[1].nC3C & 8)) {
        return 1;
    }
    return 0;
}

// Stroke version: the game is over when a player quit (bit 13) or no hole is left.
u8 fn_800FA148(int a) {
    int h;
    if ((gPlayers[0].nC3C & 0x2000) || (gPlayers[1].nC3C & 0x2000)) {
        return 1;
    }
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// Solo: once holed, the hole's time is taken; the hole is over when holed (or given up).
u8 fn_800FA1CC(int nPlayer, int a) {
    if (gPlayers[0].n290[Game_CurHoleIndex()] == 0 && Player_IsHoled(0)) {
        gPlayers[0].n290[Game_CurHoleIndex()] = fn_800E27C0();
    }
    if (Player_IsHoled(0) || (gPlayers[0].nC3C & 0x04000000)) {
        return 1;
    }
    return 0;
}

u8 fn_800FA26C(int a) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

s32 fn_800FA2C8(void) {
    return 0;
}

// End of hole: the time scores, or (match version) the hole winner.
void fn_800FA2D0(void) {
    if (gpGame->n4 == 0) {
        fn_800FA48C(0, Game_CurHoleIndex());
        if (gNumPlayersSetUp > 1) {
            fn_800FA48C(1, Game_CurHoleIndex());
        }
    } else if (gpGame->n4 == 1) {
        if (Player_IsHoled(0)) {
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[0].nHolesWon++;
        }
        if (Player_IsHoled(1)) {
            gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[1].nHolesWon++;
        }
    }
}

void fn_800FA3AC(void) {
    gPlayers[0].nC6C[Game_CurHoleIndex()] = gPlayers[0].nC44;
    gPlayers[1].nC6C[Game_CurHoleIndex()] = gPlayers[1].nC44;
}

// Game finished: the totals.
void fn_800FA410(void) {
    if (gpGame->n4 == 0) {
        fn_800FA4B8(0);
        if (gNumPlayersSetUp > 1) {
            fn_800FA4B8(1);
        }
        if (gNumPlayersSetUp == 1) {
            if (gPlayers[0].nC44 > 0) {
                fn_80125910(1);
            }
        } else {
            fn_80125910(1);
        }
    }
}

// A hole's score: the seconds taken plus 30 per stroke.
s32 fn_800FA48C(int nPlayer, int nHole) {
    return gPlayers[nPlayer].n290[nHole] + gPlayers[nPlayer].nStrokes[nHole] * 30;
}

s32 fn_800FA4B8(int nPlayer) {
    int h;
    s32 n = 0;
    for (h = 0; h < 18; h++) {
        n += fn_800FA48C(nPlayer, h);
    }
    return n;
}

// Counts the timer down; returns 1 while it runs, then the count itself (0, or -1 when off).
s32 fn_800FA518(int nPlayer) {
    if (gPlayers[nPlayer].nC38 != -1) {
        gPlayers[nPlayer].nC38--;
    }
    if (gPlayers[nPlayer].nC38 <= 0) {
        return gPlayers[nPlayer].nC38;
    }
    return 1;
}

void fn_800FA554(int nPlayer) {
    gPlayers[nPlayer].nC38 = 59;
}

// Hole start: everyone into the ready state (25).
void fn_800FA570(void) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nC3C = 0;
        PLAYER(i)->uC48 = 0;
        fn_8001C804(i, 1, 1);
        fn_80095744(PLAYER(i)->nShotHandle, 1);
        Emotion_UpdatePlayerEmotion(i);
        GOLFERSTATE_Set(25, i);
    }
}

// State 25, enter: the countdown before the clock starts.
void fn_800FA608(int nPlayer) {
    int i;
    if (nPlayer == 0) {
        gPlayers[nPlayer].nC58 = 2;
    } else {
        gPlayers[nPlayer].nC58 = 3;
    }
    fn_800FE0AC(gPlayers[nPlayer].nC58, 0);
    fn_800FE080(gPlayers[nPlayer].nC58, 0);
    fn_800FE054(gPlayers[nPlayer].nC58, 0);
    gPlayers[nPlayer].nC3C &= ~0x02000000;
    if (lbl_802823C9) {
        gPlayers[nPlayer].nC3C |= 0x600000;
    } else if (lbl_802823CA) {
        gPlayers[nPlayer].nC3C |= 0x800000;
    }
    GM_MovePlayerToBall(nPlayer);
    Shot_Plan(nPlayer, 1);
    for (i = 0; i < 2; i++) {
        fn_8001704C((&gPlayers[nPlayer].nView0)[i], nPlayer);
    }
    fn_8001D8DC(nPlayer);
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].nShotHandle);
    fn_80095744(gPlayers[nPlayer].nShotHandle, 5);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80017028(gPlayers[nPlayer].nView0);
    fn_80062F1C();
    i = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(i), 12, nPlayer, i);
    gPlayers[nPlayer].nC54 = 74;
    gPlayers[nPlayer].nC3C |= 2;
    fn_800FE02C();
    fn_800F80D4(3);
    if (gPlayers[nPlayer].nC58 == 2) {
        fn_800E3C70(1);
    } else {
        fn_800E3CD4(1);
    }
    fn_800FDFFC(gPlayers[nPlayer].nC58, 1);
    fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    fn_80062C80(gPlayers[nPlayer].nC58, 0);
    gPlayers[nPlayer].nC3C &= ~0x100001;
    fn_800FDFC4(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 1);
    lbl_802823D0 = 0;
}

// State 25, update: at the end of the countdown, go.
void fn_800FA844(int nPlayer) {
    gPlayers[nPlayer].nC54--;
    if (gPlayers[nPlayer].nC54 == 59 && nPlayer == 0) {
        fn_800FE164(0x27, 2);
    }
    if (gPlayers[nPlayer].nC54 <= 0) {
        if (gPlayers[nPlayer].nC3C & 2) {
            if (nPlayer == 0) {
                fn_800FE0D8();
                fn_800F80D4(2);
                fn_800FE164(0x26, 2);
                EVENT_Trigger(nPlayer, 0x45, 0, 0);
            }
            gPlayers[nPlayer].nC54 = 44;
            gPlayers[nPlayer].nC3C ^= 6;
            GOLFERSTATE_Set(2, nPlayer);
            fn_80062C80(gPlayers[nPlayer].nC58, 1);
        }
    } else if (nPlayer == 0) {
        if (!(gPlayers[nPlayer].nC3C & 0x1000)) {
            gPlayers[nPlayer].nC3C |= 0x1000;
            EVENT_Trigger(nPlayer, 0x44, 0, 0);
        }
    }
}

void fn_800FA994(int nPlayer) {
}

void fn_800FA998(int nPlayer) {
    gPlayers[nPlayer].nC54--;
    if (gPlayers[nPlayer].nC54 <= 0 && (gPlayers[nPlayer].nC3C & 4)) {
        gPlayers[nPlayer].nC3C &= ~6;
    }
}

// States 12 and 24, enter: the shot starts; the ball is saved and the shot clock set.
void fn_800FA9E0(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    Mem_cpy(p->ballBefore, p->ball, 0xBC);
    p->nBallBeforeOwner = -1;
    fn_8006ACF8(nPlayer, 0);
    fn_8006BAA8(nPlayer);
    fn_800FA554(nPlayer);
    gPlayers[nPlayer].nC40 = gPlayers[nPlayer].nLie;
}

// An event's sound, if it has one: only the first 37 events play one.
void fn_800FAA70(int nEvent) {
    // Read before the range check, but in bounds: the table has 44 entries and the callers pass
    // at most event 42.
    u16 nSound = lbl_80192BA8[nEvent];
    if (nEvent >= 37 || nSound == 0xFFFF) {
        return;
    }
    fn_800FE164(nSound, 1);
}

// An event for a player: its flags, and its points taken from the other player. A player whose
// points run out loses the hole; the other gets 6000.
void fn_800FAAB8(int nPlayer, int nEvent) {
    int nOther;
    s32 nPoints;
    if ((!(gPlayers[nPlayer].nC3C & 0x6000) || nEvent == 0x28 || nEvent == 0x29) && nEvent <= 0x2A) {
        if (nEvent == 0x25) {
            lbl_802823CC++;
        }
        if (nEvent == 0x27) {
            fn_80062C80(gPlayers[nPlayer].nC58, 1);
        }
        nOther = nPlayer ? 0 : 1;
        nPoints = lbl_80192908[nEvent].nPoints;
        gPlayers[nPlayer].uC48 |= lbl_80192908[nEvent].uFlags;
        gPlayers[nPlayer].nC44 += nPoints;
        if (gPlayers[nPlayer].nC44 <= 0 && !(gPlayers[nPlayer].nC3C & 0x2000) &&
            !(gPlayers[nPlayer].nC3C & 0x8000)) {
            gPlayers[nPlayer].nC44 = 0;
            gPlayers[nOther].nC44 = 6000;
            gPlayers[nPlayer].nC3C |= 0xC000;
            gPlayers[nOther].nC3C |= 0x10000 | 0x4000;
            if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26) {
                GOLFERSTATE_Set(26, (u8)nOther);
            }
            if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
        } else {
            fn_800FDFC4(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 0);
            fn_800FE100(gPlayers[nPlayer].nC58, nEvent, nPoints);
            lbl_802120F8[lbl_802823D0].nEvent = nEvent;
            lbl_802120F8[lbl_802823D0].nPlayer = nPlayer;
            if (++lbl_802823D0 >= 100) {
                lbl_802823D0 = 0;
            }
            fn_800FAA70(nEvent);
            if (nPoints != 0) {
                gPlayers[nOther].nC44 -= nPoints;
                fn_800FDFC4(gPlayers[nOther].nC58, gPlayers[nOther].nC44, 0);
                if (gPlayers[nOther].nC44 <= 0) {
                    if (!(gPlayers[nOther].nC3C & 0x6000) && !(gPlayers[nOther].nC3C & 0x8000)) {
                        gPlayers[nOther].nC44 = 0;
                        gPlayers[nPlayer].nC44 = 6000;
                        gPlayers[nOther].nC3C |= 0xC000;
                        gPlayers[nPlayer].nC3C |= 0x10000 | 0x4000;
                        if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26) {
                            GOLFERSTATE_Set(26, (u8)nOther);
                        }
                        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                            GOLFERSTATE_Set(26, (u8)nPlayer);
                        }
                    }
                }
            }
        }
    }
}

// A shot has come to rest (from state 12's update): count the stroke, and after out of bounds
// drop the ball (water) or replace it. Returns 0 when the golfer goes back to state 1. In the
// two-player stroke game (mode 7) it also scores events: the penalties (4/5, or 0x1E/0x1F with
// nC3C bit 4), and the distance from fBallX/fBallZ against the other player's (events 1, 0x18
// and 0x19).
u8 fn_800FAD54(int nPlayer) {
    int nOther;
    f32 fDist;
    SurfaceType* pSurf;
    int nStrokes;               // read and never used
    int nOtherStrokes;          // read and never used
    f32 fX;
    f32 fZ;
    f32 dx;
    f32 dz;
    if (lbl_802823C8 && (gPlayers[nPlayer].nC3C & 0x100000)) {
        lbl_802823C8 = 0;
        gPlayers[nPlayer].nC3C &= ~0x100000;
        GOLFERSTATE_Switch(1, nPlayer);
        return 0;
    }
    if (Game_GetMode() != 7) {
        GM_PlayerAddStroke(nPlayer);
        if (GM_CheckForBallOOB(nPlayer)) {
            pSurf = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].pBallCourse, gPlayers[nPlayer].ball);
            if (pSurf != NULL && pSurf->nClass == 7) {
                fn_800DEB5C(nPlayer);
                gPlayers[nPlayer].nC3C &= ~1;
            } else {
                gPlayers[nPlayer].nC3C &= ~1;
                GM_ReplaceOOBBall(nPlayer);
            }
            fn_800FE0AC(gPlayers[nPlayer].nC58, 0);
            fn_800FE080(gPlayers[nPlayer].nC58, 0);
            fn_800FE054(gPlayers[nPlayer].nC58, 0);
            GOLFERSTATE_Switch(1, nPlayer);
            return 0;
        }
        return 1;
    }
    nOther = nPlayer ? 0 : 1;
    GM_PlayerAddStroke(nPlayer);
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    nOtherStrokes = gPlayers[nOther].nStrokes[Game_CurHoleIndex()];
    if (GM_CheckForBallOOB(nPlayer)) {
        pSurf = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].pBallCourse, gPlayers[nPlayer].ball);
        if (pSurf != NULL && pSurf->nClass == 7) {
            if (gPlayers[nPlayer].nC3C & 0x10) {
                fn_800FAAB8(nPlayer, 0x1F);
            } else {
                fn_800FAAB8(nPlayer, 5);
            }
            fn_800DEB5C(nPlayer);
            gPlayers[nPlayer].nC3C &= ~1;
            gPlayers[nPlayer].nC3C |= 0x800;
        } else {
            if (gPlayers[nPlayer].nC3C & 0x10) {
                fn_800FAAB8(nPlayer, 0x1E);
            } else {
                fn_800FAAB8(nPlayer, 4);
            }
            gPlayers[nPlayer].nC3C &= ~1;
            GM_ReplaceOOBBall(nPlayer);
        }
        fn_800FE0AC(gPlayers[nPlayer].nC58, 0);
        fn_800FE080(gPlayers[nPlayer].nC58, 0);
        fn_800FE054(gPlayers[nPlayer].nC58, 0);
        if (lbl_802823C8) {
            GOLFERSTATE_Switch(1, nPlayer);
            lbl_802823C8 = 0;
        } else {
            gPlayers[nPlayer].nC3C |= 0x100000;
        }
        return 0;
    }
    fX = gPlayers[nPlayer].fBallX;
    fZ = gPlayers[nPlayer].fBallZ;
    if (fX == gPlayers[nPlayer].vA44[0] && fZ == gPlayers[nPlayer].vA44[2]) {
        dx = *(f32*)(gPlayers[nPlayer].ball + 0) - fX;
        dz = *(f32*)(gPlayers[nPlayer].ball + 8) - fZ;
        fDist = fn_80009680(dx * dx + dz * dz);
        if (!(gPlayers[nPlayer].nC3C & 0x20)) {
            gPlayers[nPlayer].fC50 = fDist;
            gPlayers[nPlayer].nC3C |= 0x20;
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1) {
                if ((gPlayers[nOther].nC3C & 0x20) && fn_800D2B08() > 3) {
                    if (gPlayers[nPlayer].fC50 > gPlayers[nOther].fC50) {
                        fn_800FAAB8(nPlayer, 1);
                    } else if (gPlayers[nPlayer].fC50 < gPlayers[nOther].fC50) {
                        fn_800FAAB8(nOther, 1);
                    }
                }
            }
        } else if (gPlayers[nPlayer].nC3C & 8) {
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == gPlayers[nPlayer].nC60 + 1 &&
                fn_800D2B08() > 3) {
                if (gPlayers[nOther].uC48 & 0x03000002) {
                    if (fDist > gPlayers[nOther].fC50) {
                        gPlayers[nOther].uC48 &= ~(u64)0x03000002;
                        fn_800FAAB8(nPlayer, 0x18);
                        gPlayers[nPlayer].fC50 = fDist;
                    }
                } else if (fDist > gPlayers[nPlayer].fC50) {
                    fn_800FAAB8(nPlayer, 0x19);
                    gPlayers[nPlayer].fC50 = fDist;
                }
            }
        }
    }
    return 1;
}

// The hole is finished in nStrokes: its events. fn_800D2B08's value (above 3 for the
// drive-distance events) less the strokes picks events 8 to 11 (0 to 3 under it); a hole in one
// is event 12 or 13, and 14 when bit 9 of nC3C is already set.
void fn_800FB204(int nPlayer, int nStrokes) {
    int nPar;
    int nUnder;
    nPar = fn_800D2B08();
    nUnder = nPar - nStrokes;
    if (nUnder >= 0 && (gPlayers[nPlayer].nC3C & 0xC00)) {
        fn_800FAAB8(nPlayer, 0x17);
    }
    gPlayers[nPlayer].nC3C &= ~0xC00;
    if (nStrokes == 1) {
        if (nPar == 3) {
            fn_800FAAB8(nPlayer, 0xC);
        } else {
            fn_800FAAB8(nPlayer, 0xD);
        }
        if (gPlayers[nPlayer].nC3C & 0x200) {
            fn_800FAAB8(nPlayer, 0xE);
        } else {
            gPlayers[nPlayer].nC3C |= 0x200;
        }
    } else if (nUnder >= 0) {
        switch (nUnder) {
        case 0:
            fn_800FAAB8(nPlayer, 8);
            break;
        case 1:
            fn_800FAAB8(nPlayer, 9);
            break;
        case 2:
            fn_800FAAB8(nPlayer, 0xA);
            break;
        case 3:
            fn_800FAAB8(nPlayer, 0xB);
            break;
        }
    }
}

// A shot's length (from vPreShot to the ball): a putt of 20/3 or more is event 17; any other
// shot of 10 or more is event 18, or 19 from 60. The unit is not proven (in yards, 20/3 would be
// 20 feet). fn_800FB41C always returns 0, so none of these fire.
void fn_800FB35C(int nPlayer, int nOther) {
    f32 fDist = fn_800FB41C(gPlayers[nPlayer].vPreShot, (f32*)gPlayers[nPlayer].ball);
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER) {
        if (fDist >= 20.0f / 3.0f) {
            fn_800FAAB8(nPlayer, 0x11);
        }
    } else if (fDist >= 10.0f) {
        if (fDist >= 60.0f) {
            fn_800FAAB8(nPlayer, 0x13);
        } else {
            fn_800FAAB8(nPlayer, 0x12);
        }
    }
}

// The distance from pA to pB on the ground (x and z).
f32 fn_800FB41C(f32* pA, f32* pB) {
    f32 v[4];
    fn_800FE190(pB, pB, v);     // EA bug: pB less itself, so the distance is always 0
    return fn_80009680(v[0] * v[0] + v[2] * v[2]);
}

// The pad's sticks (beyond the 96..160 dead zone) scaled to -1..1 into the player's fA7C..fA8C;
// 0 inside the dead zone.
void fn_800FB460(int nPlayer) {
    u8* pPad = fn_800136C4(gPlayers[nPlayer].nController);
    if (pPad) {
        if (pPad[3] < 96.0f) {
            gPlayers[nPlayer].fA84 = (96.0f - pPad[3]) / 96.0f;
        } else if (pPad[3] > 160.0f) {
            gPlayers[nPlayer].fA84 = (160.0f - pPad[3]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA84 = 0.0f;
        }
        if (pPad[2] < 96.0f) {
            gPlayers[nPlayer].fA80 = (96.0f - pPad[2]) / 96.0f;
        } else if (pPad[2] > 160.0f) {
            gPlayers[nPlayer].fA80 = (160.0f - pPad[2]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA80 = 0.0f;
        }
        if (pPad[0] < 96.0f) {
            gPlayers[nPlayer].fA7C = -((96.0f - pPad[0]) / 96.0f);
        } else if (pPad[0] > 160.0f) {
            gPlayers[nPlayer].fA7C = -((160.0f - pPad[0]) / 96.0f);
        } else {
            gPlayers[nPlayer].fA7C = 0.0f;
        }
        if (pPad[1] < 96.0f) {
            gPlayers[nPlayer].fA8C = -(96.0f - pPad[1]) / 96.0f;
        } else if (pPad[1] > 160.0f) {
            gPlayers[nPlayer].fA8C = -(160.0f - pPad[1]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA8C = 0.0f;
        }
    }
}

// Every frame of the run to the ball: the sticks, four buttons that trigger events 22 to 25,
// the stick-driven countdowns in nC54 (bits 21 to 24 of nC3C), and fCB4, which a button raises
// and which otherwise falls at a rate scaled by the frame time.
void fn_800FB774(int nPlayer) {
    f32 fStep;
    if (!fn_800FA118(nPlayer, 1)) {
        fn_800FB460(nPlayer);
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1A, 1)) {
            EVENT_Trigger(nPlayer, 0x16, 0, -1);
        } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1B, 1)) {
            EVENT_Trigger(nPlayer, 0x17, 0, -1);
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1C, 1)) {
            EVENT_Trigger(nPlayer, 0x18, 0, -1);
        } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1D, 1)) {
            EVENT_Trigger(nPlayer, 0x19, 0, -1);
        }
        if (gPlayers[nPlayer].nC3C & 0x200000) {
            gPlayers[nPlayer].nC3C &= ~0x200000;
            if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
                gPlayers[nPlayer].nC54 = 179;
            } else {
                fn_800FE080(gPlayers[nPlayer].nC58, 1);
            }
        } else if (gPlayers[nPlayer].nC3C & 0xC00000) {
            if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
                if (gPlayers[nPlayer].nC3C & 0x400000) {
                    fn_800FE080(gPlayers[nPlayer].nC58, 2);
                    gPlayers[nPlayer].nC3C &= ~0x400000;
                } else {
                    fn_800FE080(gPlayers[nPlayer].nC58, 3);
                    gPlayers[nPlayer].nC3C &= ~0x800000;
                }
                gPlayers[nPlayer].nC3C |= 0x1000000;
                gPlayers[nPlayer].nC54 = 239;
            }
        } else if (gPlayers[nPlayer].nC3C & 0x1000000) {
            gPlayers[nPlayer].nC54--;
            if (gPlayers[nPlayer].nC54 == 0) {
                fn_800FE080(gPlayers[nPlayer].nC58, 0);
                gPlayers[nPlayer].nC3C &= ~0x1000000;
            }
        } else if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
            gPlayers[nPlayer].nC54 = 179;
            fn_800FE080(gPlayers[nPlayer].nC58, 0);
        } else {
            gPlayers[nPlayer].nC54--;
            if (gPlayers[nPlayer].nC54 == 0) {
                fn_800FE080(gPlayers[nPlayer].nC58, 1);
            }
        }
        fStep = 0.999f * (59.94f * gSession.fFrameTime);
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x24, 0)) {
            gPlayers[nPlayer].fCB4 += lbl_802816B0;
            gPlayers[nPlayer].nCB8 = 0;
        } else if (gPlayers[nPlayer].nCB8 > (s32)(59.94f * lbl_802816C4)) {
            gPlayers[nPlayer].fCB4 -= lbl_802816C0 * fStep;
        } else {
            gPlayers[nPlayer].fCB4 -= lbl_802816B4 * fStep;
        }
    }
}

// A CPU player's sticks for the run: turn from the heading fA88 towards vPlacement (fA80),
// forward speed from how far off the heading is (fA84), both eased off near the target; fCB4
// rises until it reaches a level set by the golfer's speed attribute.
void fn_800FBB30(Player* p) {
    f32 v[4];
    f32 fAngle;
    f32 fDistSq;
    f32 fOff;
    f32 fScale;
    f32 fLow;
    fn_800FE190((f32*)p->ball, p->vPlacement, v);
    fDistSq = v[0] * v[0] + v[2] * v[2];
    fn_800BAF04(v, v);
    fAngle = p->fA88 - fn_8000AD78(v[2], v[0]) - PI / 2.0f;
    while (fAngle < -PI) {
        fAngle += TWOPI;
    }
    while (fAngle > PI) {
        fAngle -= TWOPI;
    }
    fOff = fn_8000AD9C(fAngle);
    if (fOff > DEG(1.0f) && fOff < DEG(15.0f)) {
        fOff *= 3.0f;
        fAngle *= 3.0f;
    }
    fScale = (PI - fOff) / PI;
    p->fA84 = fScale;
    if (fOff >= PI / 4.0f) {
        p->fA84 *= 0.25f;
    }
    p->fA80 = (1.0f / PI) * fAngle;
    if (fDistSq < 31.25f) {
        p->fA84 *= fDistSq / 31.25f;
        p->fA80 *= fDistSq / 31.25f;
    }
    if (fOff > DEG(2.0f)) {
        p->fA84 *= fScale;
        p->fA80 *= fScale;
        p->fA7C = (1.0f / PI) * -fAngle;
    } else {
        p->fA7C = 0.0f;
    }
    fLow = lbl_802816B8;
    if (p->fCB4 < (lbl_802816BC - fLow) *
                      (0.5f * (0.01f * (s8)Golfer_GetAttribute(p, ATTR_SPEED, ATTR_TOTAL))) + fLow) {
        p->fCB4 += lbl_802816B0;
    }
}

// States 12 and 24, update: the ball flies, then the golfer runs to it. Once the ball stops,
// fn_800FAD54 scores the shot; a holed ball (mode 7: its events, and state 26 once both are
// down) or a ball on the green gets its events, and out of bounds replaces the ball. Then the
// run: the countdown from fn_800FA518, the sticks (fn_800FB774, or fn_800FBB30 for the CPU), and
// arriving at the ball (within 5 of it).
void fn_800FBD2C(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    int nOther = nPlayer ? 0 : 1;
    f32 vStart[4];
    f32 vDir[4];
    f32 v[4];
    int nStrokes;
    int nPar;
    int nDiff;
    int n;
    CourseInfo* pHole;
    f32 fDist;
    f32 fToPlace;
    f32 fAngle;
    f32 fLow;
    f32 fHigh;
    u8* pBall;
    f32* pTarget;
    if (Game_GetMode() == 7) {

        if (!(gPlayers[nPlayer].uC48 & 1) && !(gPlayers[nOther].uC48 & 1)) {
            fn_800FAAB8(nPlayer, 0);
        }
        if (p->vA44[0] == *(f32*)(p->ball + 0) && p->vA44[2] == *(f32*)(p->ball + 8) && (p->nC3C & 8)) {
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == gPlayers[nPlayer].nC60 &&
                !(gPlayers[nPlayer].uC48 & 0x4000000000LL)) {
                fn_800FAAB8(nPlayer, 0x26);
            }
        }
    }
    GM_SimulateBallMovement(nPlayer);
    if (gPlayers[nPlayer].nBallState == 0) {
    } else if (gPlayers[nPlayer].nBallState == 1 || gPlayers[nPlayer].nBallState == 5) {
        if (!fn_800FAD54(nPlayer)) {
            if (Game_GetMode() == 7 && (s8)GOLFERSTATE_GetCurrentState(nOther) == 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
            return;
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
        if (gPlayers[nPlayer].nLie == LIE_HOLED) {
            if (Game_GetMode() == 7) {
                fn_800FB35C(nPlayer, nOther);
                nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[nPlayer].nC3C & 8) {
                    gPlayers[nPlayer].nC3C |= 0x10;
                    if (!(gPlayers[nOther].nC3C & 8)) {
                        fn_800FAAB8(nPlayer, 0x1D);
                    }
                    fn_800FB204(nPlayer, nStrokes - gPlayers[nPlayer].nC60);
                } else {
                    gPlayers[nPlayer].nC3C |= 8;
                    gPlayers[nPlayer].nC5C = 59;
                    gPlayers[nPlayer].nC3C |= 0x100;
                    gPlayers[nPlayer].nC60 = nStrokes;
                    fn_800FB204(nPlayer, nStrokes);
                    if (gPlayers[nOther].nC3C & 8) {
                        if (nStrokes < gPlayers[nOther].nC60) {
                            if (!(gPlayers[nPlayer].uC48 & 0x2000000000LL)) {
                                fn_800FAAB8(nPlayer, 0x25);
                            }
                        } else if (nStrokes > gPlayers[nOther].nC60) {
                            if (!(gPlayers[nOther].uC48 & 0x2000000000LL)) {
                                fn_800FAAB8(nOther, 0x25);
                            }
                        }
                    } else if (nStrokes <= gPlayers[nOther].nStrokes[Game_CurHoleIndex()]) {
                        if (!(gPlayers[nPlayer].uC48 & 0x2000000000LL)) {
                            fn_800FAAB8(nPlayer, 0x25);
                        }
                    }
                }
                if ((gPlayers[nOther].nC3C & 8) || (gPlayers[nPlayer].nC3C & 0x10)) {
                    GOLFERSTATE_Set(26, (u8)nPlayer);
                    if (gPlayers[nOther].nBallState != 2 && gPlayers[nOther].nBallState != 3 &&
                        gPlayers[nOther].nBallState != 4) {
                        GOLFERSTATE_Set(26, (u8)nOther);
                    }
                    return;
                }
                if (!(gPlayers[nPlayer].uC48 & 4)) {
                    fn_800FAAB8(nPlayer, 2);
                }
                pHole = fn_8000C594();
                fn_80055AA8((Ball*)p->ball, &((HoleTees*)pHole)->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
                Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[nPlayer]].x, &p->fBallX);
                Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[nPlayer]].x, p->vA44);
                gPlayers[nPlayer].nC3C &= ~1;
                if (lbl_802823C8) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    lbl_802823C8 = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            } else {
                fn_800F80D4(0);
                fn_800ED710(nPlayer);
                GOLFERSTATE_Switch(13, nPlayer);
                if (lbl_802823C9) {
                    lbl_802823C9 = 0;
                    lbl_802823CA = 1;
                } else {
                    lbl_802823CA = 0;
                }
            }
        } else {
            if (!(gPlayers[nPlayer].nC3C & 8) && (gPlayers[nOther].nC3C & 8)) {
                if (nStrokes >= gPlayers[nOther].nC60 && !(gPlayers[nOther].uC48 & 0x2000000000LL)) {
                    fn_800FAAB8(nOther, 0x25);
                }
            }
            pBall = p->ball;
            if (!Ter_PointInOOBNetwork(pBall)) {
                gPlayers[nPlayer].nBallState = 5;
            }
            if (gPlayers[nPlayer].nBallState == 5) {
                GM_ReplaceOOBBall(nPlayer);
                if (lbl_802823C8) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    lbl_802823C8 = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            } else {
                gPlayers[nPlayer].nBallState = 0;
                Mem_cpy(p->ballBefore, pBall, 0xBC);
                nPar = fn_800D2B08();
                if (gPlayers[nPlayer].nLie == LIE_GREEN) {
                    if (Game_GetMode() == 7) {
                        fDist = fn_800FB41C(p->vPreShot, (f32*)pBall);
                        if (fn_800FB41C((f32*)pBall, gpGame->p130) <= 1.0f && fDist >= 20.0f) {
                            fn_800FAAB8(nPlayer, 0x14);
                        }
                        if (!(gPlayers[nPlayer].nC3C & 0x40)) {
                            gPlayers[nPlayer].nC3C |= 0x40;
                            gPlayers[nPlayer].nC64 = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
                            if (!(gPlayers[nOther].nC3C & 0x40)) {
                                fn_800FAAB8(nPlayer, 3);
                            }
                            fn_800FE190((f32*)p->ball, gpGame->p130, v);
                            p->fC68 = fn_80009680(v[0] * v[0] + v[2] * v[2]);
                            nDiff = nPar - 2 - gPlayers[nPlayer].nC64;
                            if (nDiff == 0) {
                                fn_800FAAB8(nPlayer, 0x15);
                            } else if (nDiff > 0) {
                                fn_800FAAB8(nPlayer, 0x16);
                            }
                            if ((gPlayers[nOther].uC48 & 0x600000) && nDiff >= 0) {
                                if (gPlayers[nOther].fC68 > gPlayers[nPlayer].fC68) {
                                    fn_800FAAB8(nPlayer, 7);
                                } else if (gPlayers[nOther].fC68 < gPlayers[nPlayer].fC68) {
                                    fn_800FAAB8(nOther, 7);
                                }
                            }
                        } else if ((gPlayers[nPlayer].nC3C & 8) && !(gPlayers[nPlayer].nC3C & 0x80)) {
                            gPlayers[nPlayer].nC3C |= 0x80;
                            fn_800FAAB8(nPlayer, 0x1C);
                            nDiff = nPar - 2 - (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] -
                                                gPlayers[nPlayer].nC60);
                            if (nDiff == 0) {
                                fn_800FAAB8(nPlayer, 0x15);
                            } else if (nDiff > 0) {
                                fn_800FAAB8(nPlayer, 0x16);
                            }
                            if ((gPlayers[nOther].uC48 & 0x600000) && nDiff >= 0) {
                                fn_800FE190((f32*)p->ball, gpGame->p130, v);
                                fDist = fn_80009680(v[0] * v[0] + v[2] * v[2]);
                                if (gPlayers[nPlayer].uC48 & 0x0C000080) {
                                    if (fDist < gPlayers[nPlayer].fC68) {
                                        fn_800FAAB8(nPlayer, 0x1B);
                                        gPlayers[nPlayer].fC68 = fDist;
                                    }
                                } else if (gPlayers[nOther].uC48 & 0x0C000080) {
                                    if (fDist < gPlayers[nOther].fC68) {
                                        fn_800FAAB8(nPlayer, 0x1A);
                                        gPlayers[nPlayer].fC68 = fDist;
                                        gPlayers[nOther].uC48 &= ~(u64)0x0C000080;
                                    }
                                } else {
                                    fn_800FAAB8(nPlayer, 7);
                                    gPlayers[nPlayer].fC68 = fDist;
                                }
                            }
                        }
                    }
                } else if (Game_GetMode() == 7 &&
                           (gPlayers[nPlayer].nLie == 6 || gPlayers[nPlayer].nLie == 7 ||
                            gPlayers[nPlayer].nLie == 8)) {

                    if (gPlayers[nPlayer].nC3C & 0x10) {
                        fn_800FAAB8(nPlayer, 0x20);
                    } else {
                        fn_800FAAB8(nPlayer, 6);
                    }
                    gPlayers[nPlayer].nC3C |= 0x400;
                }
            }
        }
        if (Game_GetMode() == 7 && (s8)GOLFERSTATE_GetCurrentState(nOther) == 26) {
            if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
            return;
        }
    } else {
        fn_80058F5C(nPlayer);
    }
    switch (fn_800FA518(nPlayer)) {
    case 1:
        break;
    case 0:
        Vec_Copy(&p->fBallX, vStart);
        fn_80069330(nPlayer, vStart);
        fn_8006A6C4(nPlayer);
        n = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(n), 9, nPlayer, n);
        gPlayers[nPlayer].nC3C |= 1;
        fn_80062C80(gPlayers[nPlayer].nC58, 0);
        gPlayers[nPlayer].fCB4 = lbl_802816B8;
        if (gPlayers[nPlayer].nC3C & 0x200000) {
            gPlayers[nPlayer].nC54 = 59;
        } else {
            gPlayers[nPlayer].nC54 = 179;
        }
        fn_800FE190((f32*)p->ball, p->vPlacement, vDir);
        Vec_Normalize(vDir, vDir);
        gPlayers[nPlayer].fA88 = PI / 2.0f + fn_8000AD78(vDir[2], vDir[0]);
        break;
    case -1:
        if (Player_IsCPU(nPlayer)) {
            fn_800FBB30(p);
        } else {
            fn_800FB774(nPlayer);
        }
        if (gPlayers[nPlayer].fCB4 < lbl_802816B8) {
            gPlayers[nPlayer].fCB4 = lbl_802816B8;
        }
        if (gPlayers[nPlayer].fCB4 > lbl_802816BC) {
            gPlayers[nPlayer].fCB4 = lbl_802816BC;
        }
        fDist = gPlayers[nPlayer].fCB4;
        fDist *= 0.01f * (s8)Golfer_GetAttribute(p, ATTR_SPEED, ATTR_TOTAL);
        PlaceBall_UpdateMomentums(nPlayer, fDist);
        pBall = p->ball;
        pTarget = p->vPlacement;
        fn_800FE190((f32*)pBall, pTarget, vDir);
        Vec_Normalize(vDir, vDir);
        fAngle = gPlayers[nPlayer].fA88 - fn_8000AD78(vDir[2], vDir[0]) - PI / 2.0f;
        fAngle *= 180.0f / PI;
        if (gPlayers[nPlayer].nC58 == 2) {
            fLow = 40.0f;
            fHigh = 330.0f;
        } else {
            fLow = 30.0f;
            fHigh = 340.0f;
        }
        while (fAngle < 0.0f) {
            fAngle += 360.0f;
        }
        while (fAngle > 360.0f) {
            fAngle -= 360.0f;
        }
        if (fAngle <= fLow || fAngle >= fHigh) {
            n = 0;
        } else if (fAngle > fLow && fAngle < 135.0f) {
            n = 1;
        } else if (fAngle < fHigh && fAngle > 225.0f) {
            n = 3;
        } else {
            n = 2;
        }
        fn_800FE0AC(gPlayers[nPlayer].nC58, n);
        fn_800FE190((f32*)pBall, pTarget, vDir);
        fToPlace = fn_80009680(vDir[0] * vDir[0] + vDir[2] * vDir[2]);
        // the distance to a position in the view's object (+0x34), if that is nearer; the second
        // square root is written twice, as a MIN() macro would expand
        fn_800FE190((f32*)pBall, (f32*)(fn_80008370(*(u8**)fn_80016CFC(gPlayers[nPlayer].nView0)) + 0x34),
                    vDir);
        fDist = (fToPlace <= (f32)fn_80009680(vDir[0] * vDir[0] + vDir[2] * vDir[2]))

                    ? fToPlace
                    : (f32)fn_80009680(vDir[0] * vDir[0] + vDir[2] * vDir[2]);
        if (gPlayers[nPlayer].nBallState == 0) {
            if (fDist < 5.0f) {
                Vec_Copy((f32*)pBall, &p->fBallX);
                gPlayers[nPlayer].nC3C &= ~1;
                fn_80062C80(gPlayers[nPlayer].nC58, 1);
                fn_800FE0AC(gPlayers[nPlayer].nC58, 0);
                fn_800FE080(gPlayers[nPlayer].nC58, 0);
                fn_800FE054(gPlayers[nPlayer].nC58, 0);
                if (lbl_802823C8 || Game_GetMode() != 7) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    lbl_802823C8 = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            }
        } else if (fDist < 5.0f) {
            if (!Player_IsCPU(nPlayer)) {
                if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x23, 0)) {
                    if (gPlayers[nPlayer].nBallState != 1) {
                        Physics_DropBall((Ball*)pBall, lbl_801D5888[nPlayer]);
                        gPlayers[nPlayer].nBallState = 1;
                    }
                } else {
                    fn_800FE054(gPlayers[nPlayer].nC58, 1);
                }
            }
        } else {
            fn_800FE054(gPlayers[nPlayer].nC58, 0);
        }
        break;
    }
}

// States 12 and 24, exit: camera 25.
void fn_800FCBDC(int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 25, nPlayer, nView);
}

// 1 when the player's pad has buttons 0x1000, 0x400 and 0x800 (Start, X and Y on a GameCube pad)
// all down, each pressed this frame or held; 0 for the CPU and for any other controller.
u8 fn_800FCC38(int nPlayer) {
    int bDown;
    int nCtrl = gPlayers[nPlayer].nController;  // fake match: only the first read goes through nCtrl
    if (nCtrl >= 8) {
        return 0;
    }
    bDown = (fn_800136DC(nCtrl) & 0x1000 || fn_800136DC(gPlayers[nPlayer].nController) & 0x10000000) &&
            (fn_800136DC(gPlayers[nPlayer].nController) & 0x400 ||
             fn_800136DC(gPlayers[nPlayer].nController) & 0x4000000) &&
            (fn_800136DC(gPlayers[nPlayer].nController) & 0x800 ||
             fn_800136DC(gPlayers[nPlayer].nController) & 0x8000000);
    return bDown;
}

// Every frame (pfn220 through fn_800FDF38): in the two-player game, a holed player takes 5 points a
// second from one still playing, which can end the hole; in both games, button 0x25 restarts the
// hole from the tee (for the cost of event 39 in mode 7). Then speed golf's lbl_802823C8 is set.
void fn_800FCCF0(void) {
    int i;
    int nOther;
    int k;
    CourseInfo* pHole;
    if (gSession.unk14 == 0) {
        if (Game_GetMode() == 7) {
            i = 0;
            nOther = 1;
            if ((gPlayers[i].nC3C & 0x6000) || (gPlayers[nOther].nC3C & 0x6000)) {
                return;
            }
            for (k = 0; k < 2; k++) {
                if (gPlayers[i].nC3C & 0x100) {
                    if (!(gPlayers[nOther].nC3C & 8)) {
                        gPlayers[i].nC5C--;
                        if (gPlayers[i].nC5C == 0) {
                            gPlayers[i].nC44 += 5;
                            fn_800FDFC4(gPlayers[i].nC58, gPlayers[i].nC44, 0);
                            fn_800FE138(gPlayers[i].nC58, 5);
                            gPlayers[nOther].nC44 -= 5;
                            fn_800FDFC4(gPlayers[nOther].nC58, gPlayers[nOther].nC44, 0);
                            gPlayers[i].nC5C = 59;
                            if (gPlayers[nOther].nC44 <= 0 && !(gPlayers[nOther].nC3C & 0x6000) &&
                                !(gPlayers[nOther].nC3C & 0x8000)) {
                                gPlayers[nOther].nC44 = 0;
                                gPlayers[i].nC44 = 6000;
                                gPlayers[nOther].nC3C |= 0xC000;
                                gPlayers[i].nC3C |= 0x10000 | 0x4000;
                                GOLFERSTATE_Set(26, (u8)nOther);
                                GOLFERSTATE_Set(26, (u8)i);
                                return;
                            }
                        }
                    }
                }
                gPlayers[i].nCB8++;
                if (!Player_IsCPU(i) && !fn_800FA118(i, 1) &&
                    lbl_80192908[0x27].nPoints + gPlayers[i].nC44 > 0) {
                    if ((fn_800136DC(gPlayers[i].nController) & fn_800142AC(0x25, 0)) && !fn_800FCC38(i) &&
                        ((s8)GOLFERSTATE_GetCurrentState(i) == 24 ||
                         (gPlayers[i].nLie != 0 && gPlayers[i].nLie != LIE_HOLED && gPlayers[i].nLie != 16 &&
                          (s8)GOLFERSTATE_GetCurrentState(i) != 2 &&
                          (s8)GOLFERSTATE_GetCurrentState(i) != 4 &&
                          (s8)GOLFERSTATE_GetCurrentState(i) != 3 &&
                          (s8)GOLFERSTATE_GetCurrentState(i) != 8 &&
                          (s8)GOLFERSTATE_GetCurrentState(i) != 10))) {
                        fn_800FAAB8(i, 0x27);
                        pHole = fn_8000C594();
                        fn_80055AA8((Ball*)gPlayers[i].ball, &((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x,
                                    i);
                        Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x, &gPlayers[i].fBallX);
                        Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x, gPlayers[i].vA44);
                        gPlayers[i].nC3C &= ~1;
                        fn_800FE0AC(gPlayers[i].nC58, 0);
                        fn_800FE080(gPlayers[i].nC58, 0);
                        fn_800FE054(gPlayers[i].nC58, 0);
                        GOLFERSTATE_Switch(1, i);
                        lbl_802823C8 = 0;
                    }
                }
                nOther = 0;
                i = 1;
            }
        } else if (Game_GetMode() == 8) {
            i = 0;
            if ((fn_800136DC(gPlayers[i].nController) & fn_800142AC(0x25, 0)) && !fn_800FCC38(i) &&
                ((s8)GOLFERSTATE_GetCurrentState(i) == 24 ||
                 (gPlayers[i].nLie != 0 && gPlayers[i].nLie != LIE_HOLED && gPlayers[i].nLie != 16))) {
                pHole = fn_8000C594();
                fn_80055AA8((Ball*)gPlayers[i].ball, &((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x, i);
                Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x, &gPlayers[i].fBallX);
                Vec_Copy(&((HoleTees*)pHole)->tee[gSession.nTeeSet[i]].x, gPlayers[i].vA44);
                gPlayers[i].nC3C &= ~1;
                fn_800FE0AC(gPlayers[i].nC58, 0);
                fn_800FE080(gPlayers[i].nC58, 0);
                fn_800FE054(gPlayers[i].nC58, 0);
                GOLFERSTATE_Switch(1, i);
            }
        }
        lbl_802823C8 = 1;
    }
}

// State 26, enter: the hole is over for this player. In the two-player game each played hole is
// scored by who gained more points on it (1 won, 0 halved, 2 lost); a run of wins up to this
// hole is event 33 (three) or 34 (more), and one win after three or more losses is event 35.
void fn_800FD1C0(int nPlayer) {
    int aResult[18];
    int nHole;
    s32 nOther;
    int h;
    int nMine;
    int nTheirs;
    int nWon;
    int nLost;
    nHole = Game_CurHoleIndex();
    nOther = nPlayer ? 0 : 1;
    if (nPlayer == 0) {
        if (lbl_802823C9) {
            lbl_802823C9 = 0;
            lbl_802823CA = 1;
        } else {
            lbl_802823CA = 0;
        }
    }
    gPlayers[nPlayer].nC54 = 134;
    gPlayers[nPlayer].nC3C &= ~1;
    fn_800FE0AC(gPlayers[nPlayer].nC58, 0);
    fn_800FE080(gPlayers[nPlayer].nC58, 0);
    fn_800FE054(gPlayers[nPlayer].nC58, 0);
    fn_800FDFC4(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 1);
    if (Game_GetMode() == 7) {
        gPlayers[nPlayer].nC3C |= 0x2000000;
        gPlayers[nPlayer].nC54 = 239;
        if (nHole != fn_800F9328()) {
            for (h = fn_800F9328(); h != -1; h = fn_800F93D8(h)) {
                if (h == fn_800F9328()) {
                    nMine = gPlayers[nPlayer].nC6C[h] - 3000;
                    nTheirs = gPlayers[nOther].nC6C[h] - 3000;
                } else if (h == nHole) {
                    nMine = gPlayers[nPlayer].nC44 - gPlayers[nPlayer].nC6C[fn_800F9414(h)];
                    nTheirs = gPlayers[nOther].nC44 - gPlayers[nOther].nC6C[fn_800F9414(h)];
                } else {
                    nMine = gPlayers[nPlayer].nC6C[h] - gPlayers[nPlayer].nC6C[fn_800F9414(h)];
                    nTheirs = gPlayers[nOther].nC6C[h] - gPlayers[nOther].nC6C[fn_800F9414(h)];
                }
                if (nMine > nTheirs) {
                    aResult[h] = 1;
                } else if (nMine == nTheirs) {
                    aResult[h] = 0;
                } else {
                    aResult[h] = 2;
                }
            }
            nWon = 0;
            for (h = nHole; h != -1; h = fn_800F9414(h)) {
                if (aResult[h] != 1) break;
                nWon++;
            }
            if (nWon > 3) {
                gPlayers[nPlayer].nC3C |= 0x40000;
            } else if (nWon == 3) {
                gPlayers[nPlayer].nC3C |= 0x20000;
            } else if (nWon == 1) {
                if (nHole != 0) {
                    nLost = 0;
                    for (h = fn_800F9414(nHole); h != -1; h = fn_800F9414(h)) {
                        if (aResult[h] != 2) break;
                        nLost++;
                    }
                }
                // EA bug: on the first hole (nHole 0) nLost is never set
                if (nLost > 2) {
                    gPlayers[nPlayer].nC3C |= 0x80000;
                }
            }
            if (gPlayers[nPlayer].nC3C & 0x20000) {
                gPlayers[nPlayer].nC3C &= ~0x20000;
                fn_800FAAB8(nPlayer, 0x21);
                gPlayers[nPlayer].nC54 += 119;
            } else if (gPlayers[nPlayer].nC3C & 0x40000) {
                gPlayers[nPlayer].nC3C &= ~0x40000;
                fn_800FAAB8(nPlayer, 0x22);
                gPlayers[nPlayer].nC54 += 119;
            } else if (gPlayers[nPlayer].nC3C & 0x80000) {
                gPlayers[nPlayer].nC3C &= ~0x80000;
                fn_800FAAB8(nPlayer, 0x23);
                gPlayers[nPlayer].nC54 += 119;
            }
        }
    }
}

// State 26, update: a player who lost the hole (bit 15) or won it on the other's points (bit 16)
// gets event 40 or 41. When the countdown ends (and fn_800A7720 is clear), the player's turn is
// over, and the other player's too if that one is not in state 26 and the ball is not in play.
void fn_800FD534(int nPlayer) {
    int nOther;
    if (gPlayers[nPlayer].nC3C & 0x8000) {
        fn_800FAAB8(nPlayer, 0x28);
        gPlayers[nPlayer].nC3C &= ~0x8000;
        gPlayers[nPlayer].nC54 += 119;
    } else if (gPlayers[nPlayer].nC3C & 0x10000) {
        fn_800FAAB8(nPlayer, 0x29);
        gPlayers[nPlayer].nC3C &= ~0x10000;
        gPlayers[nPlayer].nC54 += 119;
    }
    if (gPlayers[nPlayer].nC54-- <= 0 && !fn_800A7720()) {
        if (gPlayers[nPlayer].nC3C & 0x4000) {
            gPlayers[nPlayer].nC3C ^= 0x6000;
        }
        gPlayers[nPlayer].nC3C |= 8;
        GM_EndOfGolferTurn(nPlayer);
        nOther = nPlayer ? 0 : 1;
        if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26 && gPlayers[nOther].nBallState == 0) {
            gPlayers[nOther].nC3C |= 8;
            gPlayers[nPlayer].nC3C |= 8;
            GM_EndOfGolferTurn(nPlayer);
        }
    }
}

void fn_800FD6A0(int nPlayer) {
}

// In speed golf (modes 7 and 8), bit 0 of nC3C: the golfer is running to the ball.
s32 fn_800FD6A4(int nPlayer) {
    if (Game_GetMode() == 7 || Game_GetMode() == 8) {
        return gPlayers[nPlayer].nC3C & 1;
    }
    return 0;
}

void fn_800FDC0C(s32* p0, s32* p1, s32* p2) {
    *p0 = lbl_80192C00[gpGame->nCurCourse].n0;
    *p1 = lbl_80192C00[gpGame->nCurCourse].n4;
    *p2 = lbl_80192C00[gpGame->nCurCourse].n8;
}

void fn_800FDF38(void) {
    fn_800FCCF0();
}

u8 fn_800FDF58(int nPlayer) {
    return 0;
}

void fn_800FDFC4(s32 p0, s32 p1, s32 p2) {
    fn_800E53F0(21, p0, p1, p2);
}

void fn_800FDFFC(s32 p0, s32 p1) {
    fn_80062D38(19, p0, (p1 & 0xFF));
}

void fn_800FE02C(void) {
    fn_80062D6C(16, 1);
}

void fn_800FE054(s32 p0, s32 p1) {
    fn_80062D38(44, p0, p1);
}

void fn_800FE080(s32 p0, s32 p1) {
    fn_80062D38(41, p0, p1);
}

void fn_800FE0AC(s32 p0, s32 p1) {
    fn_80062D38(37, p0, p1);
}

void fn_800FE0D8(void) {
    fn_80062D6C(16, 2);
}

void fn_800FE100(s32 p0, s32 p1, s32 p2) {
    fn_800E53F0(23, p0, p1, p2);
}

void fn_800FE138(s32 p0, s32 p1) {
    fn_80062D38(22, p0, p1);
}

void fn_800FE164(s32 p0, s32 p1) {
    fn_800A7664(4, p0, p1);
}

// Four floats of pA less pB into pOut.
asm void fn_800FE190(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
