// GameMode8.c (our name): speed golf. Each hole scores the time taken (n290, in seconds) plus 30
// seconds per stroke. The golfer runs to the ball between shots (custom golfer states 12 and
// 24..26 replace the normal ones). Mode 8 is solo; modes 7 and 6 (GameMode7.c, GameMode6.c) are
// the two-player stroke and match versions.

#include "golfer.h"

typedef struct SwingStateDef {
    void (*pfnEnter)(int nPlayer);  // 0x00
    void (*pfnUpdate)(int nPlayer); // 0x04
    void (*pfnExit)(int nPlayer);   // 0x08
} SwingStateDef;
extern SwingStateDef sGolferStateEngineTable[];        // 0x801883D8

void  STATEFUNC_SimulateInit(int nPlayer);
void  STATEFUNC_SimulateUpdate(int nPlayer);
void  STATEFUNC_SimulateExit(int nPlayer);
int   Game_CurHoleIndex(void);
u8    Player_IsHoled(int nPlayer);
int   GOLFERSTATE_GetCurrentState(int nPlayer);
void  GOLFERSTATE_Set(int nState, int nPlayer);
void  Session_SetNumPlayers(int n);
void  GM_MovePlayerToBall(int nPlayer);
void  Shot_Plan(int nPlayer, int a);
void  Emotion_UpdatePlayerEmotion(int nPlayer);
void  EVENT_Trigger(int nPlayer, int nEvent, int a, int b);
void  View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);
void* fn_80017028(int nView);
void  fn_8001704C(int nView, int nPlayer);
void  fn_8001C804(int nPlayer, int a, int b);
void  fn_8001D8DC(int nPlayer);
void  fn_80062C80(int a, int b);
void  fn_80062CB0(int a, int b);
void  fn_80062D38();
void  fn_80062D6C();
void  fn_80062F1C(void);
void  fn_80095744(int nHandle, int nAnim);
void  fn_800957D8(int nHandle);
void  fn_800A7664();
u8    fn_800E1BBC(void);
void  fn_800E2BA4();
s32   fn_800E27C0(void);
void  fn_800E3C70(int a);
void  fn_800E3CD4(int a);
u8    fn_800E4254(int nPlayer);
void  fn_800E4364(u32 nQueue, int a, int b, int c);
void  fn_800E45C0();
void  fn_800E53F0();
int   fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize);
void  fn_800D3548(int nPlayer, int nMoney, int a);
void  fn_800F80D4(s32 p0);
void  fn_80125910(int a);
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
void  GOLFERSTATE_Switch(int nState, int nPlayer);
void  GM_PlayerAddStroke(int nPlayer);
u8    GM_CheckForBallOOB(int nPlayer);
void  GM_ReplaceOOBBall(int nPlayer);
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, u8* pBall);
void  fn_800DEB5C(int nPlayer);
void  fn_800FE190(f32* pA, f32* pB, f32* pOut);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);
void  fn_8006ACF8(int nPlayer, int a);
void  fn_8006BAA8(int nPlayer);
void  fn_800FE100(s32 p0, s32 p1, s32 p2);
void  fn_800FA554(int nPlayer);

void  fn_800F9824(void);
void  fn_800F9844(void);
void  fn_800F9A58(void);
void  fn_800F9AB0(void);
void  fn_800F9B34(void);
s32   fn_800F9BF8(int nPlayer);
u8    fn_800F9C00(int nPlayer, int a);
void  fn_800F9C48(void);
u8    fn_800F9D00(u8 bCheck);
void  fn_800F9E00(void);
u8    fn_800F9F04(u8 bCheck);
u8    fn_800FA118(int nPlayer, int a);
u8    fn_800FA148(int a);
u8    fn_800FA1CC(int nPlayer, int a);
u8    fn_800FA26C(int a);
s32   fn_800FA2C8(void);
void  fn_800FA2D0(void);
void  fn_800FA3AC(void);
void  fn_800FA410(void);
s32   fn_800FA48C(int nPlayer, int nHole);
s32   fn_800FA4B8(int nPlayer);
void  fn_800FA570(void);
void  fn_800FA608(int nPlayer);
void  fn_800FA844(int nPlayer);
void  fn_800FA994(int nPlayer);
void  fn_800FA998(int nPlayer);
void  fn_800FA9E0(int nPlayer);
f32   fn_800FB41C(f32* pA, f32* pB);
void  fn_800FBD2C(int nPlayer);
void  fn_800FCBDC(int nPlayer);
void  fn_800FCCF0(void);
void  fn_800FD1C0(int nPlayer);
void  fn_800FD534(int nPlayer);
void  fn_800FD6A0(int nPlayer);
void  fn_800FDA30(int nPlayer, int a);
void  fn_800FDF38(void);
u8    fn_800FDF58(int nPlayer);
u8    fn_800FDF60(void);
void  fn_800FDFC4(s32 p0, s32 p1, s32 p2);
void  fn_800FDFFC(s32 p0, s32 p1);
void  fn_800FE02C();
void  fn_800FE054(s32 p0, s32 p1);
void  fn_800FE080(s32 p0, s32 p1);
void  fn_800FE0AC(s32 p0, s32 p1);
void  fn_800FE0D8(void);
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
                fn_800D3548(nWinner, nMoney, 0);
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

// A holed shot's length (from vPreShot to the ball): a putt of 20 feet or more is event 17; any
// other shot of 10 yards or more is event 18, or 19 from 60 yards.
void fn_800FB35C(int nPlayer) {
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

void fn_800FD6A0(int nPlayer) {
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
    fn_800E53F0(21, p0, p1, p2, p0);
}

void fn_800FDFFC(s32 p0, s32 p1) {
    fn_80062D38(19, p0, (p1 & 0xFF));
}

void fn_800FE02C() {
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
    fn_800E53F0(23, p0, p1, p2, p0);
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
