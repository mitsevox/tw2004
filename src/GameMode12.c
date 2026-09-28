// GameMode12.c (our name): game mode 12, stroke play with points. Each time the ball lands on a
// surface that has points in the mini-game prize table (gEarningsTable.aMini, at + 0x710), the
// shot scores them times how many times it has now landed there this shot (up to 5 times; once for
// a surface that costs points), times the shot's multiplier (the best one landed on so far this
// shot); the row's bonus-meter points fill a meter (0..100, emptied each hole). At the end of the
// shot its points go to the hole (nHolePoints); when the hole ends they are scaled by the score
// against par (32 for a hole in one down to 0 beyond 3 over), and after a full round each human
// player with an active profile is paid the round's total as money. The HUD reads the points, the
// meter, the multiplier and the list of surfaces scored. The honors, hole-finished and
// game-finished callbacks are stroke play's (mode 0, GameModeStroke).

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

s32 gGameMode12Surface;             // the surface the ball last touched, a gSurfaceTypes index (-1: none)

// The surfaces a player has scored on this shot, for the HUD (GameMode12_ListScoredSurfaces):
// gGameMode12NumScored entries.
s32 gGameMode12NumScored;
s32 gGameMode12ScoredSurfaces[20];  // the surface (a prize-table id)
s32 gGameMode12ScoredHits[20];      // how many times it scored

u8   GameMode12_GoToPlayoff(u8 bCheck);
void GameMode12_GetSurfacePrize(s32 nSurface, s32* pPoints, s32* pMeter, s32* pMult);
s32  GameMode12_CountSurfaceHits(int nPlayer, s32 nSurface);
void GameMode12_BallLanded(int nPlayer);
s32  GameMode12_BallHitSurface(int nPlayer);
void GameMode12_EndHole(void);
void GameMode12_EndGame(void);
void GameMode12_LoadHole(void);
void GameMode12_SetupNextGolfer(void);
void GameMode12_AddShotPoints(int nPlayer);
void GameMode12_ShotSetupUpdate(int nPlayer);

// Mode 12 starts (pfnInit): its callbacks (the honors, hole-finished and game-finished ones are
// stroke play's, GameModeStroke), round flags b271, b281, b288 and the stroke limit off, any number
// of mulligans, hole 1, no surface hit yet, no split screen, and the options' nWeather 0.
void GameMode12_Init(void) {
    gpGame->pfnInit = GameMode12_Init;
    gpGame->pfnGetHonors = GameModeStroke_GetHonors;
    gpGame->pfnHoleFinished = GameModeStroke_HoleFinished;
    gpGame->pfnGameFinished = GameModeStroke_GameFinished;
    gpGame->pfnGoToPlayoff = GameMode12_GoToPlayoff;
    gpGame->pfnEndGame = GameMode12_EndGame;
    gpGame->pfnBallCollision = GameMode12_BallLanded;
    gpGame->pfnTriggerSplash = GameMode12_BallHitSurface;
    gpGame->pfnEndHole = GameMode12_EndHole;
    gpGame->pfnLoadHole = GameMode12_LoadHole;
    gpGame->pfnSetupNextGolfer = GameMode12_SetupNextGolfer;
    gpGame->pfnCheckShotAwards = GameMode12_AddShotPoints;
    gpGame->pfnSwingUpdate = GameMode12_ShotSetupUpdate;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b288 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 1;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    gGameMode12Surface = -1;
    gSession.nSplitScreen = 0;
    gSession.options.nWeather = 0;
}

// Never a playoff (pfnGoToPlayoff): always 0.
u8 GameMode12_GoToPlayoff(u8 bCheck) {
    return 0;
}

// A surface is used up once it has scored 5 times this shot (1 time for one that costs points); one
// with no points (0) always counts as used up, so its meter points and multiplier never count.
static inline u8 SurfaceUsedUp(s32* pPoints, s32 nHits) {
    u8 bUsed = 1;
    s32 nPoints = *pPoints;
    if (nPoints < 0 && nHits < 1) {
        bUsed = 0;
    }
    if (nPoints > 0 && nHits < 5) {
        bUsed = 0;
    }
    return bUsed;
}

// The ball has landed (pfnBallCollision) on gGameMode12Surface. If that surface still scores this
// shot (it has points: up to 5 times, once for one that costs points; a surface with no points
// never scores) its row of the prize table counts, each part with a message at the ball's place on
// screen: a multiplier higher than the shot's becomes the shot's (message 0x35); the points times
// the number of times the surface has now scored this shot are added to the shot's points
// (nShotPoints) times the multiplier, the surface to the shot's list and one scoring landing to the
// hole's count (nHoleHits; message 0x33, not in a replay); the bonus-meter points fill the meter
// (nD24, at most 100; message 0x34).
void GameMode12_BallLanded(int nPlayer) {
    s32 nHits;
    s32 nScore;
    s32 nPoints;
    s32 nMeter;
    s32 nMult;
    f32 x;
    f32 y;
    if (gGameMode12Surface >= 0) {
        GameMode12_GetSurfacePrize(gGameMode12Surface, &nPoints, &nMeter, &nMult);
        nHits = GameMode12_CountSurfaceHits(nPlayer, gGameMode12Surface);
        if (!SurfaceUsedUp(&nPoints, nHits)) {
            fn_8006434C(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]),
                        gPlayers[nPlayer].ball.vPrev,
                        &x, &y, 0);
            fn_8006A8D4(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]), &x, &y);
            if (nMult > gPlayers[nPlayer].nDBC) {
                gPlayers[nPlayer].nDBC = nMult;
                GameMsg_Send3Ints(0x35, nMult, 512.0f * x, 448.0f * y);
            }
            if (nPoints != 0) {
                gPlayers[nPlayer].aShotSurfaces[gPlayers[nPlayer].nShotSurfaceCount] = gGameMode12Surface;
                gPlayers[nPlayer].nShotSurfaceCount++;
                gPlayers[nPlayer].nHoleHits[Game_CurHoleIndex()]++;
                nScore = nPoints * (nHits + 1);
                gPlayers[nPlayer].nShotPoints += nScore * gPlayers[nPlayer].nDBC;
                if (!gSession.bReplay) {
                    GameMsg_Send5Ints(0x33, nScore, 512.0f * x, 448.0f * y, gGameMode12Surface, nHits + 1);
                }
            }
            if (nMeter != 0) {
                gPlayers[nPlayer].nD24 += nMeter;
                if (gPlayers[nPlayer].nD24 > 100) {
                    gPlayers[nPlayer].nD24 = 100;
                }
                GameMsg_Send3Ints(0x34, nMeter, 512.0f * x, 448.0f * y);
            }
        }
    }
}

// fake match: stands in for a function the original linker stripped. Its constants (0.0, then 0.5)
// are still in this file's pool; its body is unknown.
static f32 GameMode12_StrippedFn(f32 x) {
    if (x > 0.5f) {
        return 0.0f;
    }
    return x;
}

// A surface's row in the prize table: its points, bonus-meter points and shot multiplier (all 0
// when it has none).
void GameMode12_GetSurfacePrize(s32 nSurface, s32* pPoints, s32* pMeter, s32* pMult) {
    int i;
    *pPoints = 0;
    *pMeter = 0;
    *pMult = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].n4;
            *pMeter = gEarningsTable.aMini[i].n14;
            *pMult = gEarningsTable.aMini[i].n18;
        }
    }
}

// How many times the player has scored on the surface nSurface this shot (the list
// GameMode12_SetupNextGolfer empties each turn).
s32 GameMode12_CountSurfaceHits(int nPlayer, s32 nSurface) {
    s32 n = 0;
    int i;
    for (i = 0; i < gPlayers[nPlayer].nShotSurfaceCount; i++) {
        if (nSurface == gPlayers[nPlayer].aShotSurfaces[i]) {
            n++;
        }
    }
    return n;
}

// The ball has touched a surface (pfnTriggerSplash, from the ball effects): its index in
// gSurfaceTypes is kept for GameMode12_BallLanded (-1 for none). Returns the extra hit effect to
// play, which is always 0 (none); it still looks the surface up as if a scoring surface had one.
s32 GameMode12_BallHitSurface(int nPlayer) {
    SurfaceType* pSurface;
    s32 nHits;
    s32 nPoints;
    s32 nMeter;
    s32 nMult;
    pSurface = PLAYER(nPlayer)->ball.pHitSurface;
    gGameMode12Surface = -1;
    if (pSurface) {
        gGameMode12Surface = pSurface - gSurfaceTypes;
        nHits = GameMode12_CountSurfaceHits(nPlayer, gGameMode12Surface);
        GameMode12_GetSurfacePrize(gGameMode12Surface, &nPoints, &nMeter, &nMult);
        if (!SurfaceUsedUp(&nPoints, nHits) && nPoints != 0) {
            return 0;
        }
    }
    return 0;
}

// Hole finished (pfnEndHole): each player's points for the hole are multiplied by the score: 32 for
// a hole in one, else 16 for 3 under par, 8, 4, 2 for par, 0.66, 0.5, 0.33 for 3 over and 0 for
// anything worse; the result is queued as message 0x73 for the player's profile.
void GameMode12_EndHole(void) {
    int i;
    f32 fMult;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] == 1) {
            fMult = 32.0f;
        } else {
            switch (PLAYER(i)->nStrokes[Game_CurHoleIndex()] - Course_GetCurHolePar()) {
            case -3:
                fMult = 16.0f;
                break;
            case -2:
                fMult = 8.0f;
                break;
            case -1:
                fMult = 4.0f;
                break;
            case 0:
                fMult = 2.0f;
                break;
            case 1:
                fMult = 0.66f;
                break;
            case 2:
                fMult = 0.5f;
                break;
            case 3:
                fMult = 0.33f;
                break;
            default:
                fMult = 0.0f;
                break;
            }
        }
        PLAYER(i)->nHolePoints[Game_CurHoleIndex()] = fMult * PLAYER(i)->nHolePoints[Game_CurHoleIndex()];
        GUI_QueueMessage(0, 0x73, PLAYER(i)->nHolePoints[Game_CurHoleIndex()], PLAYER(i)->nIndex);
    }
}

// Game finished (pfnEndGame), after a full round only: each human player's points for the round are
// paid as money (GM_Earnings_AwardMoney, with message 0x6A when not 0) when the player's profile is
// active; a CPU's profile is paid 0.
void GameMode12_EndGame(void) {
    int i;
    int h;
    s32 nMoney;
    if (GM_FullRoundOfGolf()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            nMoney = 0;
            if (!Player_IsCPU(i)) {
                for (h = 0; h < 18; h++) {
                    nMoney += PLAYER(i)->nHolePoints[h];
                }
            }
            if (gpSaveData[PLAYER(i)->nIndex].bActive) {
                if (nMoney) {
                    GUI_QueueMessage(0, 0x6A, nMoney, PLAYER(i)->nIndex);
                }
                GM_Earnings_AwardMoney(i, nMoney, 0);
            }
        }
    }
}

// Hole start (pfnLoadHole): every player's bonus meter (nD24) empties.
void GameMode12_LoadHole(void) {
    int i;
    i = 0;
    while (i < 5) {
        gPlayers[i++].nD24 = 0;
    }
}

// Next turn (pfnSetupNextGolfer): every player's list of surfaces scored this shot is emptied, the
// shot's points go to 0 and its multiplier to 1; then stroke play picks who plays
// (GameModeStroke_SetupNextGolfer).
void GameMode12_SetupNextGolfer(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        PLAYER(i)->nShotSurfaceCount = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nShotPoints = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aShotSurfaces[j] = 0;
        }
    }
    GameModeStroke_SetupNextGolfer();
}

s32 GameMode12_GetShotMultiplier(int nPlayer) {
    return gPlayers[nPlayer].nDBC;
}

// The player's bonus meter, 0..100.
s32 GameMode12_GetBonusMeter(int nPlayer) {
    return gPlayers[nPlayer].nD24;
}

// The player's points on this hole.
s32 GameMode12_GetHolePoints(int nPlayer) {
    return gPlayers[nPlayer].nHolePoints[Game_CurHoleIndex()];
}

// The player's points for the round.
s32 GameMode12_GetRoundPoints(int nPlayer) {
    s32 n = 0;
    int h;
    for (h = 0; h < 18; h++) {
        n += gPlayers[nPlayer].nHolePoints[h];
    }
    return n;
}

// End of a shot (pfnCheckShotAwards, from GM_Earnings_PayShotGoals): the shot's points go to the
// hole.
void GameMode12_AddShotPoints(int nPlayer) {
    gPlayers[nPlayer].nHolePoints[Game_CurHoleIndex()] += gPlayers[nPlayer].nShotPoints;
}

// The length of the list GameMode12_ListScoredSurfaces made (the HUD passes a player it does not
// take).
s32 GameMode12_GetNumScoredSurfaces(void) {
    return gGameMode12NumScored;
}

// Entry i of that list: a prize-table surface id. nPlayer is unused.
s32 GameMode12_GetScoredSurface(int nPlayer, int i) {
    return gGameMode12ScoredSurfaces[i];
}

// How many times entry i of that list scored. nPlayer is unused.
s32 GameMode12_GetScoredSurfaceHits(int nPlayer, int i) {
    return gGameMode12ScoredHits[i];
}

// The HUD's list of the surfaces the player has scored on this shot, in prize-table order, and how
// many times each (gGameMode12ScoredSurfaces, gGameMode12ScoredHits, gGameMode12NumScored).
void GameMode12_ListScoredSurfaces(int nPlayer) {
    int i;
    s32 n;
    gGameMode12NumScored = 0;
    for (i = 0; i < 20; i++) {
        n = GameMode12_CountSurfaceHits(nPlayer, gEarningsTable.aMini[i].nId);
        if (n != 0) {
            gGameMode12ScoredSurfaces[gGameMode12NumScored] = gEarningsTable.aMini[i].nId;
            gGameMode12ScoredHits[gGameMode12NumScored] = n;
            gGameMode12NumScored++;
        }
    }
}

// Every frame over the ball (pfnSwingUpdate): once the swing has started (swing state not idle),
// front-end message 0x36 is sent.
void GameMode12_ShotSetupUpdate(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}
