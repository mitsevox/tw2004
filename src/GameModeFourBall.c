// GameModeFourBall.c (TW06's GameModeFourBall): game mode 20, two-against-two match play where each
// team counts its better ball on every hole. Team 0 is players 0 and 1, team 1 players 2 and 3.

#include "golfer.h"
#include "ball.h"

int   Game_CurHoleIndex(void);
int   Game_CurrentHole(void);
u8    Player_IsHoled(int nPlayer);
u8    Player_IsHoledNotState23(int nPlayer);
u8    Player_OnTee(int nPlayer);
u8    Team_IsAllHuman(int nTeam);
void  GOLFERSTATE_Set(int nState, int nPlayer);
CourseInfo* fn_8000C594(void);
u8    fn_800E1BBC(void);
u8    fn_800EC550(void);
void  fn_800E2BA4();
void  fn_800E45C0();
int   fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize);
void  fn_800D3548(int nPlayer, int nMoney, int a);
void  fn_800E4364(u32 nQueue, int a, int b, int c);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern u8* gpSaveData;
extern s32 lbl_80282278;                    // the player whose turn it is
extern u8  lbl_80282240;

// The tee order before anyone has won a hole (lbl_80184DC0: 0, 1, 2, 3).
typedef struct TeeOrder {
    s32 a[4];
} TeeOrder;
extern TeeOrder lbl_80184DC0;

u8   fn_800E8E24(int nTeam);
u8   fn_800E8F20(int nTeam);
int  fn_800E8FC8(int nTeam);
int  fn_800E90AC(int nTeam);
void fn_800E90FC(void);
s32  fn_800E9178(int nPlayer);
int  fn_800E947C(int nPlayer);
u8   fn_800E948C(u32 nPlayer, int a);
u8   fn_800E96B8(u8 bCheck);
u8   fn_800E98F0(u8 bCheck);
void fn_800E9BBC(void);
void fn_800E9CF4(void);

// TW06: GameModeFourBall::Init. Four players; the CPU may concede.
void fn_800E8D58(void) {
    gpGame->pfn1C8 = fn_800E8D58;
    gpGame->pfn1D0 = fn_800E90FC;
    gpGame->pfn1D4 = fn_800E9178;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800E948C;
    gpGame->pfn1DC = (u8 (*)(int))fn_800E96B8;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800E98F0;
    gpGame->pfn1E8 = fn_800E9BBC;
    gpGame->pfn1F4 = fn_800E9CF4;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// TW06: GameModeFourBall::TeamDone. A partner has holed out and the other can no longer beat that
// score.
u8 fn_800E8E24(int nTeam) {
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
    if ((Player_IsHoled(a) && gPlayers[a].nStrokes[nHole] <= gPlayers[b].nStrokes[nHole] + 1) ||
        (Player_IsHoled(b) && gPlayers[b].nStrokes[nHole] <= gPlayers[a].nStrokes[nHole] + 1)) {
        bDone = 1;
    }
    return bDone;
}

// TW06: GameModeFourBall::TeamConceded. Both partners picked up.
u8 fn_800E8F20(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int bConceded;
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
    bConceded = 0;
    if (Player_IsHoled(a) && !Player_IsHoledNotState23(a) && Player_IsHoled(b) && !Player_IsHoledNotState23(b)) {
        bConceded = 1;
    }
    return bConceded;
}

// TW06: GameModeFourBall::TeamBestPossibleScore. The team's best score on this hole if a partner
// holes the next shot (at most 9), or its score once holed.
int fn_800E8FC8(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int n;
    int b;
    int nBest;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    n = 9;
    if (gPlayers[a].nStrokes[nHole] + 1 < 9) {
        n = gPlayers[a].nStrokes[nHole] + 1;
    }
    nBest = gPlayers[b].nStrokes[nHole] + 1;
    if (n <= nBest) {
        nBest = n;
    }
    if (Player_IsHoled(a)) {
        nBest = nBest <= gPlayers[a].nStrokes[nHole] ? nBest : gPlayers[a].nStrokes[nHole];
    }
    if (Player_IsHoled(b)) {
        nBest = nBest <= gPlayers[b].nStrokes[nHole] ? nBest : gPlayers[b].nStrokes[nHole];
    }
    return nBest;
}

// TW06: GameModeFourBall::TeamMatchWins (kept on the team's first player).
int fn_800E90AC(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    return gPlayers[a].nHolesWon;
}

// The hole starts: the first golfer to play gets ready, the others wait.
void fn_800E90FC(void) {
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

// TW06: GameModeFourBall::GetHonors. On the tee the team that won the last decided hole, and (on
// team 0 only) the better score of the pair; otherwise the player farthest from the pin (off the
// green first) whose team is still playing.
s32 fn_800E9178(int nPlayer) {
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
    order = lbl_80184DC0;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            if (gPlayers[0].nModePoints[h] != 0) {
                w = 0;
            } else if (gPlayers[2].nModePoints[h] != 0) {
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
            t = order.a[0];
            if (gPlayers[order.a[1]].nStrokes[h] < gPlayers[t].nStrokes[h]) {
                order.a[0] = order.a[1];
                order.a[1] = t;
            }
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (nPlayer != order.a[i] && Player_OnTee(order.a[i]) && !fn_800E8E24(fn_800E947C(order.a[i]))) {
            return order.a[i];
        }
    }
    pCourse = fn_8000C594();
    fBest = 0.0f;
    pPin = (f32*)&pCourse->pin[Game_CurrentHole()];
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && !fn_800E8E24(fn_800E947C(i)) && gPlayers[i].nLie != LIE_GREEN) {
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
            if (i != nPlayer && !Player_IsHoled(i) && !fn_800E8E24(fn_800E947C(i))) {
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

// TW06: GameModeFourBall::GetPlayerTeam.
int fn_800E947C(int nPlayer) {
    return nPlayer / 2;
}

// TW06: GameModeFourBall::HoleFinished. Both teams done or one conceded; or one team done and the
// other can no longer beat it (or only tie, when dormie).
u8 fn_800E948C(u32 nPlayer, int a) {
    int nLeft;
    int h;
    if (fn_800E8E24(0) && fn_800E8E24(1)) {
        return 1;
    }
    if (fn_800E8F20(0) || fn_800E8F20(1)) {
        return 1;
    }
    if (fn_800E8E24(0) && (!lbl_80282240 || nPlayer > 1)) {
        if (fn_800E8FC8(0) < fn_800E8FC8(1)) {
            return 1;
        }
    }
    if (fn_800E8E24(1) && (!lbl_80282240 || nPlayer - 2 > 1)) {
        if (fn_800E8FC8(1) < fn_800E8FC8(0)) {
            return 1;
        }
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex(); h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (fn_800E8E24(0) && (!lbl_80282240 || nPlayer > 1)) {
        if (nLeft + fn_800E90AC(1) == fn_800E90AC(0)) {
            if (fn_800E8FC8(0) <= fn_800E8FC8(1)) {
                return 1;
            }
        }
    }
    if (fn_800E8E24(1) && (!lbl_80282240 || nPlayer - 2 > 1)) {
        if (nLeft + fn_800E90AC(0) == fn_800E90AC(1)) {
            if (fn_800E8FC8(1) <= fn_800E8FC8(0)) {
                return 1;
            }
        }
    }
    return 0;
}

#define PLAYER_AT(i) (&gPlayers[i])
// Clears every player's round (all 18 holes) for a playoff.
#define CLEAR_ROUNDS(P)                             \
    for (i = 0; i < gNumPlayersSetUp; i++) {        \
        for (h = 0; h < 18; h++) {                  \
            P(i)->nStrokes[h] = 0;                  \
            P(i)->nPutts[h] = 0;                    \
            P(i)->nModePoints[h] = 0;               \
            P(i)->n22C[h] = 0;                      \
            P(i)->n290[h] = 0;                      \
            P(i)->b2F6[h] = 0;                      \
            P(i)->b2E4[h] = 0;                      \
        }                                           \
        P(i)->n2D8 = 0;                             \
        P(i)->n2DC = 0;                             \
        P(i)->n2E0 = 0;                             \
        P(i)->n308 = 0;                             \
    }

// TW06: GameModeFourBall::GameFinished. In a playoff: over once a team is ahead; otherwise (unless
// only checking) the next playoff hole starts. In the round: over when no holes are left and no
// playoff starts, or when a team leads by more than the holes left.
u8 fn_800E96B8(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bD4) {
        if (fn_800E90AC(0) != fn_800E90AC(1)) {
            return 1;
        }
        if (!bCheck) {
            fn_800E2BA4(gpGame->nD8++);
            CLEAR_ROUNDS(PLAYER_AT);
            fn_800E45C0();
        }
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !fn_800E98F0(bCheck);
        }
        if (nLeft + fn_800E90AC(0) < fn_800E90AC(1) || nLeft + fn_800E90AC(1) < fn_800E90AC(0)) {
            return 1;
        }
    }
    return 0;
}

// TW06: GameModeFourBall::GoToPlayoff. After the last hole with the match tied: a playoff starts
// (bD5 when the round played all 18 holes).
u8 fn_800E98F0(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (fn_800E90AC(0) == fn_800E90AC(1)) {
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
        CLEAR_ROUNDS(PLAYER);
        gpGame->bD4 = 1;
        fn_800E45C0(gpGame->nD8++);
        return 1;
    }
    return 0;
}

// TW06: GameModeFourBall::EndHole. The hole goes to the other team when a team conceded, or to a
// team that holed out and cannot be caught (the point goes on players 0 and 2).
void fn_800E9BBC(void) {
    int nWinner = -1;
    int nHole = Game_CurHoleIndex();
    if (fn_800E8F20(0)) {
        nWinner = 1;
    } else if (fn_800E8F20(1)) {
        nWinner = 0;
    } else if (fn_800E8E24(0) && fn_800E8FC8(0) < fn_800E8FC8(1)) {
        nWinner = 0;
    } else if (fn_800E8E24(1) && fn_800E8FC8(1) < fn_800E8FC8(0)) {
        nWinner = 1;
    }
    switch (nWinner) {
    case 0:
        gPlayers[0].nModePoints[nHole] = 1;
        gPlayers[0].nHolesWon++;
        return;
    case 1:
        gPlayers[2].nModePoints[nHole] = 1;
        gPlayers[2].nHolesWon++;
        return;
    }
}

// TW06: GameModeFourBall::EndGame. The winning team's human players with a profile get the prize
// money (by the margin).
void fn_800E9CF4(void) {
    int nPrize;
    Player* p;
    int nLoser;
    int nMargin;
    int nMoney;
    int nFirst;
    int nWinner;
    int i;
    int k;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        if (fn_800E90AC(0) > fn_800E90AC(1)) {
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
                p = PLAYER(i);
                if (gpSaveData[p->nIndex * 0x10600] && nMoney) {
                    fn_800E4364(0, 0x6B, nPrize, p->nIndex);
                    fn_800D3548(i, nMoney, 0);
                    p->nMoney += nMoney;
                }
            }
        }
    }
}
