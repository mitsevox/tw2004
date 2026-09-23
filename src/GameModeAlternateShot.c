// GameModeAlternateShot.c (TW06's GameModeAlternateShot): game mode 21, two-against-two match play
// where partners take turns hitting one ball. Team 0 is players 0 and 1, team 1 players 2 and 3.

#include "golfer.h"
#include "ball.h"

int   Game_CurHoleIndex(void);
int   Game_CurrentHole(void);
u8    Player_IsHoled(int nPlayer);
u8    Player_OnTee(int nPlayer);
u8    Team_IsAllHuman(int nTeam);
void  GOLFERSTATE_Set(int nState, int nPlayer);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);   // memcpy
CourseInfo* fn_8000C594(void);
u8    fn_800E1BBC(void);
u8    fn_800EC550(void);
void  fn_800E2BA4();
void  fn_800E45C0();
int   fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize);
void  fn_800D3548(int nPlayer, int nMoney, int a);
void  fn_800E4364(u32 nQueue, int a, int b, int c);
void  fn_80125910(int a);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern u8* gpSaveData;
extern s32 lbl_80282278;                    // the player whose turn it is
extern u8  lbl_80282240;
extern u8  lbl_80281648[2];                 // per team: 1 when the second partner (1 or 3) hits next

// The tee order before anyone has won a hole (lbl_80184DA0: 0, 1, 2, 3).
typedef struct TeeOrder {
    s32 a[4];
} TeeOrder;
extern TeeOrder lbl_80184DA0;

u8  fn_800E69CC(int nTeam);
int fn_800E6A48(int nPlayer);
u8  fn_800E6A98(int nPlayer);
int fn_800E6AF8(int nPlayer);
int fn_800E6B08(int nTeam);
int fn_800E6BA4(int nTeam);
void fn_800E6C10(void);
s32  fn_800E6C8C(int nPlayer);
void fn_800E6F88(int nPlayer);
u8   fn_800E7038(int nPlayer, int a);
u8   fn_800E723C(u8 bCheck);
u8   fn_800E7474(u8 bCheck);
void fn_800E7740(void);
void fn_800E7828(void);

// TW06: GameModeAlternateShot::Init. Four players, no mulligans, no gimmes, one view.
void fn_800E68F0(void) {
    gpGame->pfn1C8 = fn_800E68F0;
    gpGame->pfn1D0 = fn_800E6C10;
    gpGame->pfn1D4 = fn_800E6C8C;
    gpGame->pfn248 = fn_800E6F88;
    gpGame->pfn1D8 = fn_800E7038;
    gpGame->pfn1DC = (u8 (*)(int))fn_800E723C;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800E7474;
    gpGame->pfn1E8 = fn_800E7740;
    gpGame->pfn1F4 = fn_800E7828;
    gpGame->bGimmesAllowed = 0;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// TW06: GameModeAlternateShot::TeamDone. The team's ball is in the hole.
u8 fn_800E69CC(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int bDone;
    int b;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    bDone = 0;
    if (Player_IsHoled(a) || Player_IsHoled(b)) {
        bDone = 1;
    }
    return bDone;
}

// TW06: GameModeAlternateShot::GetPartner.
int fn_800E6A48(int nPlayer) {
    switch (nPlayer) {
    case 0:
        return 1;
    case 1:
        return 0;
    case 2:
        return 3;
    case 3:
        return 2;
    default:
        return 5;
    }
}

// Whether it is this player's turn to hit the team's ball.
u8 fn_800E6A98(int nPlayer) {
    int bSecond = (nPlayer == 1 || nPlayer == 3) ? 1 : 0;
    return bSecond == lbl_80281648[fn_800E6AF8(nPlayer)];
}

// A player's team.
int fn_800E6AF8(int nPlayer) {
    return nPlayer / 2;
}

// TW06: GameModeAlternateShot::TeamBestPossibleScore. The team's score on this hole if it holes the
// next shot (at most 9), or its score once holed.
int fn_800E6B08(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int n;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    n = 9;
    if (gPlayers[a].nStrokes[nHole] + 1 < 9) {
        n = gPlayers[a].nStrokes[nHole] + 1;
    }
    if (Player_IsHoled(a)) {
        n = n <= gPlayers[a].nStrokes[nHole] ? n : gPlayers[a].nStrokes[nHole];
    }
    return n;
}

// TW06: GameModeAlternateShot::TeamMatchWins.
int fn_800E6BA4(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int b;
    int a;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    return gPlayers[a].nHolesWon + gPlayers[b].nHolesWon;
}

// The hole starts: the first golfer to play gets ready, the others wait.
void fn_800E6C10(void) {
    int i;
    lbl_80282278 = gpGame->pfn1D4(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        } else {
            GOLFERSTATE_Set(GS_WAIT, i);
        }
    }
}

// TW06: GameModeAlternateShot::GetHonors. Who plays next after nPlayer (5 = nobody): on the tee the
// team that won the last decided hole goes first; otherwise whoever's turn it is on the team that is
// farthest from the pin and off the green, then anyone farthest.
s32 fn_800E6C8C(int nPlayer) {
    TeeOrder order;
    int nLead;
    int h;
    int w;
    int t;
    int i;
    CourseInfo* pCourse;
    f32* pPin;
    f32 fBest;
    int nBest;
    f32 dx;
    f32 dz;
    f32 d;
    nLead = 0;
    order = lbl_80184DA0;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            int a = gPlayers[0].nModePoints[h] + gPlayers[1].nModePoints[h];
            int b = gPlayers[2].nModePoints[h] + gPlayers[3].nModePoints[h];
            if (a > b) {
                w = 0;
            } else if (b > a) {
                w = 1;
            } else {
                w = nLead;
            }
            if (w != nLead) {
                nLead = w;
                t = order.a[0];
                order.a[0] = order.a[2];
                order.a[2] = t;
                t = order.a[1];
                order.a[1] = order.a[3];
                order.a[3] = t;
            }
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (nPlayer != order.a[i] && Player_OnTee(order.a[i]) && fn_800E6A98(order.a[i]) &&
            !fn_800E69CC(fn_800E6AF8(order.a[i]))) {
            return order.a[i];
        }
    }
    pCourse = fn_8000C594();
    fBest = 0.0f;
    pPin = (f32*)&pCourse->pin[Game_CurrentHole()];
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && fn_800E6A98(i) && !fn_800E69CC(fn_800E6AF8(i)) &&
            gPlayers[i].nLie != LIE_GREEN) {
            dx = ((Ball*)gPlayers[i].ball)->vPos[0] - pPin[0];
            dz = ((Ball*)gPlayers[i].ball)->vPos[2] - pPin[2];
            d = fn_80009680(dx * dx + dz * dz);
            if (d > fBest) {
                fBest = d;
                nBest = i;
            }
        }
    }
    if (nBest == 5) {
        fBest = 0.0f;
        nBest = 5;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i != nPlayer && !Player_IsHoled(i) && fn_800E6A98(i) && !fn_800E69CC(fn_800E6AF8(i))) {
                dx = ((Ball*)gPlayers[i].ball)->vPos[0] - pPin[0];
                dz = ((Ball*)gPlayers[i].ball)->vPos[2] - pPin[2];
                d = fn_80009680(dx * dx + dz * dz);
                if (d > fBest) {
                    fBest = d;
                    nBest = i;
                }
            }
        }
    }
    if (nBest == nPlayer) {
        return 5;
    }
    return nBest;
}

// TW06: GameModeAlternateShot::EndGolferTurn. The partner takes over the ball (and the stroke
// count), and it becomes the partner's turn.
void fn_800E6F88(int nPlayer) {
    int nPartner = fn_800E6A48(nPlayer);
    int nTeam;
    Mem_cpy(gPlayers[nPartner].ball, gPlayers[nPlayer].ball, 0xBC);
    gPlayers[nPartner].bLowIQPenalty = gPlayers[nPlayer].bLowIQPenalty;
    ((Ball*)gPlayers[nPartner].ball)->nPlayer = nPartner;
    gPlayers[nPartner].nStrokes[Game_CurHoleIndex()] = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    nTeam = fn_800E6AF8(nPlayer);
    lbl_80281648[nTeam] = 1 - lbl_80281648[nTeam];
}

// TW06: GameModeAlternateShot::HoleFinished. Both teams holed; or one team holed and the other can
// no longer beat it (can only tie, when the holed team is dormie); lbl_80282240 excuses the
// holed team's own players.
u8 fn_800E7038(int nPlayer, int a) {
    int nLeft;
    int h;
    if (fn_800E69CC(0) && fn_800E69CC(1)) {
        return 1;
    }
    if (fn_800E69CC(0) && (!lbl_80282240 || (nPlayer != 0 && nPlayer != 1))) {
        if (fn_800E6B08(0) < fn_800E6B08(1)) {
            return 1;
        }
    }
    if (fn_800E69CC(1) && (!lbl_80282240 || (nPlayer != 2 && nPlayer != 3))) {
        if (fn_800E6B08(1) < fn_800E6B08(0)) {
            return 1;
        }
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex(); h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (fn_800E69CC(0) && (!lbl_80282240 || (nPlayer != 0 && nPlayer != 1))) {
        if (nLeft + fn_800E6BA4(0) == fn_800E6BA4(1)) {
            if (fn_800E6B08(0) <= fn_800E6B08(1)) {
                return 1;
            }
        }
    }
    if (fn_800E69CC(1) && (!lbl_80282240 || (nPlayer != 2 && nPlayer != 3))) {
        if (nLeft + fn_800E6BA4(0) == fn_800E6BA4(1)) {
            if (fn_800E6B08(1) <= fn_800E6B08(0)) {
                return 1;
            }
        }
    }
    return 0;
}

// Clears every player's round (all 18 holes) for a playoff.
#define CLEAR_ROUNDS()                                  \
    for (i = 0; i < gNumPlayersSetUp; i++) {            \
        for (h = 0; h < 18; h++) {                      \
            gPlayers[i].nStrokes[h] = 0;                \
            gPlayers[i].nPutts[h] = 0;                  \
            gPlayers[i].nModePoints[h] = 0;             \
            gPlayers[i].n22C[h] = 0;                    \
            gPlayers[i].n290[h] = 0;                    \
            gPlayers[i].b2F6[h] = 0;                    \
            gPlayers[i].b2E4[h] = 0;                    \
        }                                               \
        gPlayers[i].n2D8 = 0;                           \
        gPlayers[i].n2DC = 0;                           \
        gPlayers[i].n2E0 = 0;                           \
        gPlayers[i].n308 = 0;                           \
    }

// TW06: GameModeAlternateShot::GameFinished. In a playoff: over once a team is ahead; otherwise
// (unless only checking) the next playoff hole starts. In the round: over when no holes are left and
// no playoff starts, or when a team leads by more than the holes left.
u8 fn_800E723C(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bD4) {
        if (fn_800E6BA4(0) != fn_800E6BA4(1)) {
            return 1;
        }
        if (!bCheck) {
            fn_800E2BA4(gpGame->nD8++);
            CLEAR_ROUNDS();
            fn_800E45C0();
        }
        return 0;
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (nLeft == 0) {
        return !fn_800E7474(bCheck);
    }
    if (nLeft + fn_800E6BA4(0) < fn_800E6BA4(1) || nLeft + fn_800E6BA4(0) > fn_800E6BA4(1)) {
        return 1;
    }
    return 0;
}

// TW06: GameModeAlternateShot::GoToPlayoff. After the last hole with the match tied: a playoff
// starts (bD5 when the round played all 18 holes).
u8 fn_800E7474(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (fn_800E6BA4(0) == fn_800E6BA4(1)) {
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
        CLEAR_ROUNDS();
        gpGame->bD4 = 1;
        fn_800E45C0(gpGame->nD8++);
        return 1;
    }
    return 0;
}

// TW06: GameModeAlternateShot::EndHole. A team that holed out and cannot be caught wins the hole
// (the point goes on players 0 and 2); on the next hole the other partner tees off.
void fn_800E7740(void) {
    int nHole = Game_CurHoleIndex();
    if (fn_800E69CC(0) && fn_800E6B08(0) < fn_800E6B08(1)) {
        gPlayers[0].nModePoints[nHole] = 1;
        gPlayers[0].nHolesWon++;
    }
    if (fn_800E69CC(1) && fn_800E6B08(1) < fn_800E6B08(0)) {
        gPlayers[2].nModePoints[nHole] = 1;
        gPlayers[2].nHolesWon++;
    }
    lbl_80281648[0] = lbl_80281648[1] = 1 - (nHole & 1);
}

// TW06: GameModeAlternateShot::EndGame. The winning team's human players with a profile get the
// prize money (by the margin).
void fn_800E7828(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nFirst;
    int k;
    int i;
    Player* p;
    if (fn_800E1BBC()) {
        if (fn_800EC550()) {
            return;
        }
        if (fn_800E6BA4(0) > fn_800E6BA4(1)) {
            nWinner = 0;
            nLoser = 1;
            nMargin = gPlayers[0].nHolesWon - gPlayers[2].nHolesWon;
        } else {
            nWinner = 1;
            nLoser = 0;
            nMargin = gPlayers[2].nHolesWon - gPlayers[0].nHolesWon;
        }
        nMoney = fn_800D37BC(nWinner, nLoser, nMargin, &nPrize);
        if (Team_IsAllHuman(nWinner)) {
            nFirst = 2;
            if (nWinner == 0) {
                nFirst = 0;
            }
            for (k = 0, i = nFirst; k < 2; k++, i++) {
                p = &gPlayers[i];
                if (gpSaveData[p->nIndex * 0x10600]) {
                    fn_80125910(1);
                    if (nMoney) {
                        fn_800E4364(0, 0x6B, nPrize, p->nIndex);
                        fn_800D3548(i, nMoney, 0);
                        p->nMoney += nMoney;
                    }
                }
            }
        }
    }
}
