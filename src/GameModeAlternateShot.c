// GameModeAlternateShot.c (TW06's GameModeAlternateShot): game mode 21, two-against-two match play
// where partners take turns hitting one ball. Team 0 is players 0 and 1, team 1 players 2 and 3.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

u8 gAltShotTeamHonors[2] = {0, 0};                // per team: 1 when the second partner (1 or 3) hits next

u8  GameModeAlternateShot_TeamDone(int nTeam);
int GameModeAlternateShot_GetPartner(int nPlayer);
u8  GameModeAlternateShot_PlayerHasTeamHonors(int nPlayer);
int GetGamePlayerTeam(int nPlayer);
int GameModeAlternateShot_TeamBestPossibleScore(int nTeam);
int GameModeAlternateShot_TeamMatchWins(int nTeam);
void GameModeAlternateShot_SetupNextGolfer(void);
s32  GameModeAlternateShot_GetHonors(int nPlayer);
void GameModeAlternateShot_EndGolferTurn(int nPlayer);
u8   GameModeAlternateShot_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeAlternateShot_GameFinished(u8 bCheck);
u8   GameModeAlternateShot_GoToPlayoff(u8 bCheck);
void GameModeAlternateShot_EndHole(void);
void GameModeAlternateShot_EndGame(void);

// Game mode 21 (alternate shot) starts, from GameRound's mode switch: this file's golfer order,
// honors, turn, hole, game-over, playoff and end-of-game callbacks; no gimmes, no mulligans, nC and
// n10 4 (as the other team modes), nDC 0, single view.
void GameModeAlternateShot_Init(void) {
    gpGame->pfnInit = GameModeAlternateShot_Init;
    gpGame->pfnSetupNextGolfer = GameModeAlternateShot_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeAlternateShot_GetHonors;
    gpGame->pfnEndGolferTurn = GameModeAlternateShot_EndGolferTurn;
    gpGame->pfnHoleFinished = GameModeAlternateShot_HoleFinished;
    gpGame->pfnGameFinished = GameModeAlternateShot_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeAlternateShot_GoToPlayoff;
    gpGame->pfnEndHole = GameModeAlternateShot_EndHole;
    gpGame->pfnEndGame = GameModeAlternateShot_EndGame;
    gpGame->bGimmesAllowed = 0;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// Whether team nTeam (0: players 0 and 1, 1: players 2 and 3) has holed out: either partner holed
// counts, as they share one ball.
u8 GameModeAlternateShot_TeamDone(int nTeam) {
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

// The partner of player 0..3 (0 and 1, 2 and 3); 5 (nobody) for any other number.
int GameModeAlternateShot_GetPartner(int nPlayer) {
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
u8 GameModeAlternateShot_PlayerHasTeamHonors(int nPlayer) {
    int bSecond = (nPlayer == 1 || nPlayer == 3) ? 1 : 0;
    return bSecond == gAltShotTeamHonors[GetGamePlayerTeam(nPlayer)];
}

// A player's team.
int GetGamePlayerTeam(int nPlayer) {
    return nPlayer / 2;
}

// The best score team nTeam can still make on this hole, read from its first player (0 or 2), who
// holds the team's strokes: its strokes plus one (holing the next shot), at most 9; once holed, its
// strokes (also at most 9).
int GameModeAlternateShot_TeamBestPossibleScore(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int n;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    if (gPlayers[a].nStrokes[nHole] + 1 < 9) {
        n = gPlayers[a].nStrokes[nHole] + 1;
    } else {
        n = 9;
    }
    if (Player_IsHoled(a)) {
        n = n <= gPlayers[a].nStrokes[nHole] ? n : gPlayers[a].nStrokes[nHole];
    }
    return n;
}

// The holes team nTeam has won in the match: both partners' nHolesWon
// (GameModeAlternateShot_EndHole credits the first).
int GameModeAlternateShot_TeamMatchWins(int nTeam) {
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

// Whoever plays next (the mode's honors) gets ready, the others wait.
void GameModeAlternateShot_SetupNextGolfer(void) {
    int i;
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        } else {
            GOLFERSTATE_Set(GS_WAIT, i);
        }
    }
}

// Who plays next, never nPlayer (5 = nobody). On the tee: the tee order starts 0, 1, 2, 3 and the
// teams swap places whenever a hole is won by the team not leading the order (a halved hole changes
// nothing); the first player in it who is on the tee, whose turn it is on the team
// (GameModeAlternateShot_PlayerHasTeamHonors) and whose team has not holed out. Otherwise, of the
// players whose turn it is on a team still playing, the one farthest from the pin (flat distance)
// and off the green; if all are on the green, the farthest of them.
s32 GameModeAlternateShot_GetHonors(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};  // the tee order before anyone has won a hole
    int nBest;
    CourseInfo* pCourse;
    int w;
    int t;
    int i;
    int h;
    int nPinSet;
    f32 fBest;
    int nLead;
    f32 dx;
    f32 dz;
    f32 d;
    nLead = 0;
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
                t = aOrder[0];
                aOrder[0] = aOrder[2];
                aOrder[2] = t;
                t = aOrder[1];
                aOrder[1] = aOrder[3];
                aOrder[3] = t;
            }
        }
    }
    // fake match: the tee-order loop reuses the hole counter h; a counter of its own gets another
    // register (h, t or w all match)
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) && GameModeAlternateShot_PlayerHasTeamHonors(aOrder[h]) &&
            !GameModeAlternateShot_TeamDone(GetGamePlayerTeam(aOrder[h]))) {
            return aOrder[h];
        }
    }
    pCourse = Ter_GetTGD();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && GameModeAlternateShot_PlayerHasTeamHonors(i) &&
            !GameModeAlternateShot_TeamDone(GetGamePlayerTeam(i)) && PLAYER(i)->ball.nLie != LIE_GREEN_e) {
            dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
            dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
            d = Math_Sqrt(dx * dx + dz * dz);
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
            if (i != nPlayer && !Player_IsHoled(i) && GameModeAlternateShot_PlayerHasTeamHonors(i) &&
                !GameModeAlternateShot_TeamDone(GetGamePlayerTeam(i))) {
                dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
                dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
                d = Math_Sqrt(dx * dx + dz * dz);
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

// After nPlayer's shot the partner takes over the team's ball: the Ball, the low-IQ penalty flag
// and this hole's strokes are copied to the partner, and it becomes the partner's turn.
void GameModeAlternateShot_EndGolferTurn(int nPlayer) {
    int nPartner = GameModeAlternateShot_GetPartner(nPlayer);
    int nTeam;
    Mem_cpy(&gPlayers[nPartner].ball, &gPlayers[nPlayer].ball, sizeof(Ball));
    gPlayers[nPartner].bLowIQPenalty = gPlayers[nPlayer].bLowIQPenalty;
    gPlayers[nPartner].ball.nPlayer = nPartner;
    gPlayers[nPartner].nStrokes[Game_CurHoleIndex()] = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    nTeam = GetGamePlayerTeam(nPlayer);
    gAltShotTeamHonors[nTeam] = 1 - gAltShotTeamHonors[nTeam];
}

// Whether the hole is over (bCheck is not used): both teams holed out; or one team holed out and
// the other can no longer beat its score; or can at best tie it when a tie would lose the match
// (the holed team is ahead by the holes left, this one included). While lbl_80282240 is set, a
// holed team's own players (nPlayer) do not end it this way.
u8 GameModeAlternateShot_HoleFinished(int nPlayer, u8 bCheck) {
    int nLeft;
    int h;
    if (GameModeAlternateShot_TeamDone(0) && GameModeAlternateShot_TeamDone(1)) {
        return 1;
    }
    if (GameModeAlternateShot_TeamDone(0) && (!lbl_80282240 || (u32)nPlayer > 1)) {
        if (GameModeAlternateShot_TeamBestPossibleScore(0) < GameModeAlternateShot_TeamBestPossibleScore(1)) {
            return 1;
        }
    }
    if (GameModeAlternateShot_TeamDone(1) && (!lbl_80282240 || (u32)(nPlayer - 2) > 1)) {
        if (GameModeAlternateShot_TeamBestPossibleScore(1) < GameModeAlternateShot_TeamBestPossibleScore(0)) {
            return 1;
        }
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex(); h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (GameModeAlternateShot_TeamDone(0) && (!lbl_80282240 || (u32)nPlayer > 1)) {
        if (nLeft + GameModeAlternateShot_TeamMatchWins(1) == GameModeAlternateShot_TeamMatchWins(0)) {
            if (GameModeAlternateShot_TeamBestPossibleScore(0) <=
                GameModeAlternateShot_TeamBestPossibleScore(1)) {
                return 1;
            }
        }
    }
    if (GameModeAlternateShot_TeamDone(1) && (!lbl_80282240 || (u32)(nPlayer - 2) > 1)) {
        if (nLeft + GameModeAlternateShot_TeamMatchWins(0) == GameModeAlternateShot_TeamMatchWins(1)) {
            if (GameModeAlternateShot_TeamBestPossibleScore(1) <=
                GameModeAlternateShot_TeamBestPossibleScore(0)) {
                return 1;
            }
        }
    }
    return 0;
}

// Clears every player's round (all 18 holes) for a playoff.
#define PLAYER_AT(i) (&gPlayers[i])
#define CLEAR_ROUNDS(P)                                  \
    for (i = 0; i < gNumPlayersSetUp; i++) {            \
        for (h = 0; h < 18; h++) {                      \
            P(i)->nStrokes[h] = 0;                \
            P(i)->nPutts[h] = 0;                  \
            P(i)->nModePoints[h] = 0;             \
            P(i)->n22C[h] = 0;                    \
            P(i)->n290[h] = 0;                    \
            P(i)->b2F6[h] = 0;                    \
            P(i)->b2E4[h] = 0;                    \
        }                                               \
        P(i)->n2D8 = 0;                           \
        P(i)->n2DC = 0;                           \
        P(i)->n2E0 = 0;                           \
        P(i)->n308 = 0;                           \
    }

// Whether the match is over. In a playoff (gpGame->bD4): over once one team has won more holes;
// otherwise, unless bCheck (only asking), the next playoff hole is set up (nD8 counted, hole
// picked, every player's scores cleared, the tied message). In the round: after the last hole it is
// over unless a playoff starts (GameModeAlternateShot_GoToPlayoff); before, when a team leads by
// more than the holes left.
u8 GameModeAlternateShot_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bD4) {
        if (GameModeAlternateShot_TeamMatchWins(0) != GameModeAlternateShot_TeamMatchWins(1)) {
            return 1;
        }
        if (!bCheck) {
            gpGame->nD8++;
            GM_Pick_PlayOffHole();
            CLEAR_ROUNDS(PLAYER_AT);
            GUI_GolfersTiedUIMessage();
        }
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !GameModeAlternateShot_GoToPlayoff(bCheck);
        }
        if (nLeft + GameModeAlternateShot_TeamMatchWins(0) < GameModeAlternateShot_TeamMatchWins(1) ||
            nLeft + GameModeAlternateShot_TeamMatchWins(1) < GameModeAlternateShot_TeamMatchWins(0)) {
            return 1;
        }
    }
    return 0;
}

// After the round's last hole with the match tied, a playoff starts (with bCheck set it is only
// reported): bD5 records whether the round played all 18 holes, a playoff hole is picked, every
// player's scores are cleared, bD4 (in a playoff) is set, nD8 counted and the tied message queued.
// Returns 1 for a playoff, 0 while holes are left or a team is ahead.
u8 GameModeAlternateShot_GoToPlayoff(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (GameModeAlternateShot_TeamMatchWins(0) == GameModeAlternateShot_TeamMatchWins(1)) {
        if (bCheck) {
            return 1;
        }
        gpGame->bD5 = 1;
        for (h = 0; h < 18; h++) {
            if (!gpGame->bHoleSelected[h]) {
                gpGame->bD5 = 0;
            }
        }
        GM_Pick_PlayOffHole();
        CLEAR_ROUNDS(PLAYER);
        gpGame->bD4 = 1;
        gpGame->nD8++;
        GUI_GolfersTiedUIMessage();
        return 1;
    }
    return 0;
}

// The hole is scored: a team that holed out with a better score than the other can still make wins
// it (nModePoints and nHolesWon on its first player, 0 or 2). Then both teams' turn flags are set
// from this hole's number: after an odd-numbered hole (1, 3, ...) the second partners (1 and 3) tee
// off next, after an even-numbered one the first partners (0 and 2).
void GameModeAlternateShot_EndHole(void) {
    int nHole = Game_CurHoleIndex();
    if (GameModeAlternateShot_TeamDone(0) &&
        GameModeAlternateShot_TeamBestPossibleScore(0) < GameModeAlternateShot_TeamBestPossibleScore(1)) {
        gPlayers[0].nModePoints[nHole] = 1;
        gPlayers[0].nHolesWon++;
    }
    if (GameModeAlternateShot_TeamDone(1) &&
        GameModeAlternateShot_TeamBestPossibleScore(1) < GameModeAlternateShot_TeamBestPossibleScore(0)) {
        gPlayers[2].nModePoints[nHole] = 1;
        gPlayers[2].nHolesWon++;
    }
    gAltShotTeamHonors[0] = gAltShotTeamHonors[1] = 1 - (nHole & 1);
}

// The match's end, for a full round outside a mode 5 challenge: the team with more holes won (team
// 1 on a tie) wins by the difference in holes won. If both its players are human, each of them with
// an active profile counts the game as won (EASBio) and, when the team's winnings
// (GM_Earnings_GetStrokeWinningsTeam) are not 0, gets the prize message (queue 0, 0x6B), the money
// (GM_Earnings_AwardMoney) and the money added to money.n14.
void GameModeAlternateShot_EndGame(void) {
    int nPrize;
    Player* p;
    int nLoser;
    int nMargin;
    int nMoney;
    int nFirst;
    int nWinner;
    int nProfile;
    int i;
    int k;
    if (GM_FullRoundOfGolf()) {
        switch (GM5_IsChallengeRunning()) {
        case 0:
            break;
        default:
            return;
        }
        if (GameModeAlternateShot_TeamMatchWins(0) > GameModeAlternateShot_TeamMatchWins(1)) {
            nWinner = 0;
            nLoser = 1;
            nMargin = gPlayers[0].nHolesWon - gPlayers[2].nHolesWon;
        } else {
            nWinner = 1;
            nLoser = 0;
            nMargin = gPlayers[2].nHolesWon - gPlayers[0].nHolesWon;
        }
        nMoney = GM_Earnings_GetStrokeWinningsTeam(nWinner, nLoser, nMargin, &nPrize);
        if (Team_IsAllHuman(nWinner)) {
            nFirst = 2;
            if (nWinner == 0) {
                nFirst = 0;
            }
            for (k = 0, i = nFirst; k < 2; k++, i++) {
                p = PLAYER(i);
                nProfile = p->nIndex;
                if (gpSaveData[nProfile].bActive) {
                    EASBio_SetCurrentGameWon(1);
                    if (nMoney) {
                        GUI_QueueMessage(0, 0x6B, nPrize, nProfile);
                        GM_Earnings_AwardMoney(i, nMoney, 0);
                        p->money.n14 += nMoney;
                    }
                }
            }
        }
    }
}
