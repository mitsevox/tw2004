// GameMode22.c (our name): game mode 22, a long-drive contest (GameRound.c starts it with
// GameMode22_Init), and before it the trophy case's text (Rookie of the Year, Player of the Year, the
// money and scoring leaders; "Earned on %s"). One file: both halves share its .data, .sdata and
// .sbss blocks.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/save.h"
#include "game/modes/ladder.h"
#include "game/modes/rte.h"
#include "game/modes/mode22.h"

void GameMode22_Shutdown(void);
void GameMode22_Unused1F0(void);
void GameMode22_SetupNextGolfer(void);
void GameMode22_EndGame(void);
void GameMode22_UpdateFrame(void);
void GameMode22_StartSwing(int nPlayer);
s32 GameMode22_GoToPlayoff(void);
u8   GameMode22_HoleFinished(int nPlayer, int n);
void GameMode22_ScoreShot(int nPlayer);
u8   fn_80126FB0(s32* pn8);
u8   GameMode22_GameFinished(int n);
void GameMode22_BallOutOfBounds(int nPlayer);
void GameMode22_EndGolferTurn(int nPlayer);
void GameMode22_DecideWinner(void);
s32  GameMode22_GetLieGroup(int n);
s32  GameMode22_GetHonors(int nPlayer);
void GameMode22_StartEvent(void);
void GameMode22_ClearPlayerStats(void);
void GameMode22_HoleStart(void);
void GameMode22_RestartHole(void);
void fn_80126F7C(void);
void fn_80126F80(void);
void fn_80126F84(s32 p0);
void fn_80126F94(s32 n);
s32 fn_80126FA0(void);
s32 fn_80126FD8(void);
u8   fn_80127004(void);
void fn_80127034(int nPlayer);
s32 fn_80127098(s32 arg0);

char* lbl_8019543C[4] = {
    "Rookie of the Year",
    "Player of the Year",
    "PGA Tour\xAE Money Leader",
    "PGA Tour\xAE Scoring Leader",
};

// FE message 605: tour trophy pArgs[0] (0..3: Rookie of the Year, Player of the Year, the money
// leader and the scoring leader; the profile's a1C0[12..15]). Returns whether it is won, writes its
// title and the day it was won (empty while not).
void TrophyRoom_GetTourTrophy(MsgArg* pArgs, MsgArg* pResult) {
    s32 nTrophy = pArgs[0].i;
    char* szName = ((MsgString*)pArgs[1].p)->pStr;
    char* szDate = ((MsgString*)pArgs[2].p)->pStr;

    pResult->i = FE_GetCurrentProfile()->a1C0[nTrophy + 12].bWon;
    strcpy(szName, lbl_8019543C[nTrophy]);
    if (pResult->i) {
        CalDate_ToString(FE_GetCurrentProfile()->a1C0[nTrophy + 12].nDate, szDate);
        return;
    }
    szDate[0] = '\0';
}

// FE message 606: a tour trophy's text by column pArgs[1]: 0 its title; 1 and 2 only placeholders
// ("some year", "amount").
void TrophyRoom_GetTourTrophyText(MsgArg* pArgs, MsgArg* pResult) {
    s32 nTrophy = pArgs[0].i;
    s32 nColumn = pArgs[1].i;
    char* szOut = ((MsgString*)pArgs[2].p)->pStr;

    switch (nColumn) {
    case 0:
        strcpy(szOut, lbl_8019543C[nTrophy]);
        break;
    case 1:
        strcpy(szOut, "some year");
        break;
    case 2:
        strcpy(szOut, "amount");
        break;
    }
}

// FE message 607: how many of the 118 real-time events start in month pArgs[0] (0-based) and have
// their profile flag (a104D0) set.
void TrophyRoom_CountEventsInMonth(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;
    s32 nMonth;
    s32 i;
    s32 nCount;
    u16 nDate;
    s32 nEventMonth;
    s32 nDay;

    nCount = 0;
    nMonth = pArgs[0].i + 1;
    pProfile = FE_GetCurrentProfile();
    for (i = 0; i < 118; i++) {
        nDate = GM_RealtimeMode_GetStartDate(i);
        CalDate_GetMDY(&nDate, &nEventMonth, &nDay, &nDay);
        if (nEventMonth == nMonth && pProfile->a104D0[i]) {
            nCount++;
        }
    }
    pResult->i = nCount;
}

// FE message 613: placeholder texts, "trophy name %d %d" and "date %d %d" of the two values
// pArgs[0] and pArgs[1].
void TrophyRoom_GetPlaceholderText(MsgArg* pArgs, MsgArg* pResult) {
    s32 nA = pArgs[0].i;
    s32 nB = pArgs[1].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;

    sprintf(((MsgString*)pArgs[2].p)->pStr, "trophy name %d %d", nA, nB);
    sprintf(szDate, "date %d %d", nA, nB);
}

// FE message 637: pArgs[0] modulo 4.
void TrophyRoom_GetIndexMod4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i % 4;
}

// FE message 638: the day the best medal of challenge group pArgs[0] (1-based) was earned, as text;
// empty for group 0 or while the group has no medal (aMedal 3).
void TrophyRoom_GetMedalDate(MsgArg* pArgs, MsgArg* pResult) {
    s32 nGroup = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    if (nGroup == 0) {
        szOut[0] = '\0';
        return;
    }
    if (pProfile->aMedal[nGroup - 1] != 3) {
        CalDate_ToString(pProfile->aMedalDate[nGroup - 1], szOut);
        return;
    }
    szOut[0] = '\0';
}

// FE message 640: ladder event pArgs[0]'s course name, and the day it was won (empty while not).
void TrophyRoom_GetLadderAward(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    char* szCourse = ((MsgString*)pArgs[1].p)->pStr;
    char* szDate = ((MsgString*)pArgs[2].p)->pStr;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    strcpy(szCourse, lbl_80191990[GameMode4_GetEventCourse(nEvent)]);
    if (pProfile->aLadderAward[nEvent].bWon) {
        CalDate_ToString(FE_GetCurrentProfile()->aLadderAward[nEvent].nDate, szDate);
        return;
    }
    szDate[0] = '\0';
}

// FE message 641: ladder event pArgs[0]'s course (into the int pArgs[1] points to); returns whether
// the event is won.
void TrophyRoom_GetLadderEventCourse(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    *(s32*)pArgs[1].p = GameMode4_GetEventCourse(nEvent);
    pResult->i = pProfile->aLadderAward[nEvent].bWon;
}

// FE message 670: the icon of real-time event award pArgs[0]
// (GM_RealtimeMode_GetIconIDByTrophyGroup) once it is won, else -1.
void TrophyRoom_GetRTEAwardIcon(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nEvent = pArgs[0].i;

    if (pProfile->aRTEAward[nEvent].bWon) {
        pResult->i = GM_RealtimeMode_GetIconIDByTrophyGroup(nEvent);
        return;
    }
    pResult->i = -1;
}

// FE message 674: "Earned on <day>" for award pArgs[0] (the profile's aAward) once it is won, else
// empty.
void TrophyRoom_GetAwardEarnedText(MsgArg* pArgs, MsgArg* pResult) {
    s32 nAward = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    char szDate[12];    // the size is not known (the frame leaves room for 12 bytes)

    if (FE_GetCurrentProfile()->aAward[nAward].bWon == 1) {
        CalDate_ToString(FE_GetCurrentProfile()->aAward[nAward].nDate, szDate);
        sprintf(szOut, "Earned on %s", szDate);
        return;
    }
    szOut[0] = '\0';
}

char lbl_80195488[16] = "D";
GameMode22State lbl_80195498 = {0, 5, 5, 0, {0, 0, 0}, 0.0f, 5, 120};
s32 lbl_802819A0 = 15;

// Defined here, last address first (CodeWarrior lays out .bss in reverse).
u8 lbl_8028259C[5];
u8 lbl_80282594[5];
u8 lbl_8028258C[5];
s32 lbl_80282588;
s32 lbl_80282584;
s32 lbl_80282580;

// Game mode 22's setup (GM_SetModeType): its callbacks; no gimmes, mulligans, stroke limit,
// GameBreakers (b285), yardage or bumped obstructions; split screen as chosen (lbl_8028227C); the
// per-player score sounds cleared (400 / 800 / 1200) and the intro messages to show
// (GameMode22_UpdateFrame).
void GameMode22_Init(void) {
    s32 i;

    gpGame->pfnInit = GameMode22_Init;
    gpGame->pfnShutdown = GameMode22_Shutdown;
    gpGame->pfn1F0 = GameMode22_Unused1F0;
    gpGame->pfnSetupNextGolfer = GameMode22_SetupNextGolfer;
    gpGame->pfnGetHonors = GameMode22_GetHonors;
    gpGame->pfn250 = GameMode22_BallOutOfBounds;
    gpGame->pfnHoleFinished = (u8 (*)(int, u8))GameMode22_HoleFinished;
    gpGame->pfnGameFinished = (u8 (*)(u8))GameMode22_GameFinished;
    gpGame->pfnGoToPlayoff = (u8 (*)(u8))GameMode22_GoToPlayoff;  // port: EA passes an argument GameMode22_GoToPlayoff ignores
    gpGame->pfnEndGolferTurn = GameMode22_EndGolferTurn;
    gpGame->pfnEndGame = GameMode22_EndGame;
    gpGame->pfn220 = GameMode22_UpdateFrame;
    gpGame->pfn20C = GameMode22_StartSwing;
    gpGame->pfn244 = GameMode22_ScoreShot;
    gpGame->pfn1E4 = GameMode22_HoleStart;
    gpGame->pfn224 = GameMode22_RestartHole;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 1;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b285 = 0;
    gpGame->b274 = 0;
    gpGame->b286 = 1;
    gpGame->b287 = 1;
    gpGame->b288 = 0;
    gpGame->b289 = 0;
    gpGame->b28A = 0;
    gpGame->bBumpObstructions = 0;
    gpGame->bShowYardage = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 2;
    gpGame->nC = 4;
    gpGame->nDC = 0;
    gpGame->n4 = 0;
    gSession.nSplitScreen = lbl_8028227C;
    for (i = 0; i < 5; i++) {
        lbl_8028259C[i] = 0;
        lbl_80282594[i] = 0;
        lbl_8028258C[i] = 0;
    }
    lbl_80282580 = 1;
}

// The mode's pfnShutdown: empty.
void GameMode22_Shutdown(void) {
}

// The mode's pfn1F0 slot, which nothing in the binary calls: empty.
void GameMode22_Unused1F0(void) {
}

// Starts a long-drive event from the menus (FE_MessageTable.c fn_80083BFC, as
// GameModeDriverRTE_StartEvent does for mode 24): every player on tee set 0, options.n20 off, no
// winner (n8 5, bC 0), no longest drive yet (f10 0, n14 5), and the 120-frame winner countdown
// (n18) reset.
void GameMode22_StartEvent(void) {
    s32 i;

    i = 0;
    while (i < gSession.nNumPlayers) {
        gSession.nTeeSet[i++] = 0;
    }
    gSession.options.n20 = 0;
    lbl_80195498.n8 = 5;
    lbl_80195498.bC = 0;
    lbl_80195498.f10 = 0.0f;
    lbl_80195498.n14 = 5;
    lbl_80195498.n18 = 120;
}

// The mode's pfnSetupNextGolfer: stroke play's (GameModeStroke_SetupNextGolfer).
void GameMode22_SetupNextGolfer(void) {
    GameModeStroke_SetupNextGolfer();
}

// The game is over (pfnEndGame): the winner (n8, from GameMode22_GetWinner) is paid 5000.
void GameMode22_EndGame(void) {
    s32 nPlayer;

    fn_80126FB0(&nPlayer);
    GM_Earnings_AwardMoney(nPlayer, 5000, NULL);
}

// Each frame (pfn220). Once after the setup: intro message 89 with 1 (variant 0) or 2 (variant 1)
// and the drives-left text (GameMode22_ShowDrivesLeft). Every 16 frames: the current player's shot
// length (fn_800D0550) sent as message 0x4D; while it is nonzero and still changing, a long-drive
// UI sound plays (script 0, track 1; started once), and it stops once the length stops changing.
// After a winner is decided (n8 not 5), the winner countdown n18 runs down.
void GameMode22_UpdateFrame(void) {
    s32 nLength;

    if (lbl_80282580 != 0) {
        switch (lbl_80195498.n0) {
        case 0:
            fn_800E5D40(1);
            break;
        case 1:
            fn_800E5D40(2);
            break;
        }
        fn_80127034(lbl_80282278);
        lbl_80282580 = 0;
    }
    if (lbl_802819A0-- <= 0) {
        nLength = fn_800D0550(lbl_80282278);
        GameMsg_Send2Ints(0x4D, 0, nLength);
        if (nLength != 0 && nLength != lbl_80282584) {
            if (lbl_80282588 == 0) {
                // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
                ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 1, 0, 1, 0);
                lbl_80282588 = 1;
            }
        } else if (lbl_80282588 != 0) {
            // port: EA passes an argument Gaud_LongDriveUi_Stop ignores
            ((void (*)(s32, int, int))Gaud_LongDriveUi_Stop)(0, 1, 0);
            lbl_80282588 = 0;
        }
        lbl_80282584 = nLength;
        lbl_802819A0 = 15;
    }
    if (lbl_80195498.n8 != 5) {
        lbl_80195498.n18--;
    }
}

// As a swing begins (pfn20C): GameMode22_PreSwing (empty), the drives-left text, and message 0x42
// for scoreboard slot 0 with nPlayer's score (nEBC).
void GameMode22_StartSwing(int nPlayer) {
    fn_80126F80();
    fn_80127034(nPlayer);
    fn_800E5CA4(0, gPlayers[nPlayer].nEBC, 0, 0, 0, 0, 0, 0.0f);
}

// The mode's pfnGoToPlayoff: never a playoff (0). The slot passes an argument it does not take.
s32 GameMode22_GoToPlayoff(void) {
    return 0;
}

// Who drives next (pfnGetHonors): player 0 until someone has driven (nEA0), then the next player
// after the current one (lbl_80282278) that is not nPlayer; 5 when there is none.
s32 GameMode22_GetHonors(int nPlayer) {
    u8 bNone = 1;
    s32 nNext;
    s32 i;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (PLAYER(i)->nEA0 != 0) {
            bNone = 0;
        }
    }
    if (bNone) {
        return 0;
    }
    nNext = lbl_80282278;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        nNext++;
        if (nNext >= gSession.nNumPlayers) {
            nNext = 0;
        }
        if (nNext != nPlayer) {
            return nNext;
        }
    }
    return 5;
}

// The hole is over (pfnHoleFinished) once a winner is decided (GameMode22_GameFinished) and the
// 120-frame winner countdown (n18) has run out. nPlayer is not read.
u8 GameMode22_HoleFinished(int nPlayer, int n) {
    s32 bRet = 0;

    if (GameMode22_GameFinished(n) && lbl_80195498.n18 < 0) {
        bRet = 1;
    }
    return bRet;
}

// The game is over (pfnGameFinished) once a winner is decided (bC, GameMode22_GetWinner). n is not
// read.
u8 GameMode22_GameFinished(int n) {
    return fn_80126FB0(NULL);
}

// The ball went out of bounds (pfn250): the drive is scored like any other (GameMode22_ScoreShot).
void GameMode22_BallOutOfBounds(int nPlayer) {
    GameMode22_ScoreShot(nPlayer);
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (his tee set)
// for the next drive.
void GameMode22_EndGolferTurn(int nPlayer) {
    Physics_InitBall(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

// Once every player's nEA0 has reached n4 and they are all level, the player with the highest nEBC
// wins (n8 = the player, bC set). A tie for the highest leaves no winner, and with n0 1 clears
// every nEBC.
// fake match: no loop unrolling in the frontend, so the second loop keeps the count in a register
// and gets the backend's unroll (srwi by 8 + remainder) as EA's does.
#pragma push
#pragma opt_unroll_loops off
void GameMode22_DecideWinner(void) {
    s32 nMin;
    s32 nFirst;
    s32* pScore;
    Player* pPlayer;
    s32 n;
    s32 i;
    s32 nWinner = 5;
    s32 nBest = -0x7FFFFFFF - 1;
    u8 bTie = 0;

    nMin = lbl_80195498.n4;
    nFirst = gPlayers[0].nEA0;
    n = gSession.nNumPlayers;
    for (i = 0; i < n; i++) {
        pPlayer = PLAYER(i);
        if (pPlayer->nEA0 < nMin) {
            return;
        }
        if (nFirst != pPlayer->nEA0) {
            return;
        }
        pScore = &pPlayer->nEBC;
        if (*pScore >= nBest) {
            bTie = 0;
            if (nBest == *pScore && nWinner != 5) {
                bTie = 1;
            }
            nWinner = i;
            // fake match: the (s32*) cast (same type) keeps this reload apart from the compare's
            // load above, so the original's lwz through pScore stays.
            nBest = *(s32*)pScore;
        }
    }
    // fake match: pPlayer goes into the high word of a 64-bit OR whose low word is nWinner, so
    // nWinner is unchanged. The dead OR keeps pPlayer live to the end of the loop at register
    // allocation, which puts pPlayer and pScore in different registers (the original's r8 / r7:
    // the peephole then loads through pPlayer + 0xEBC), and it is deleted after allocation.
    // port: reads pPlayer uninitialised when there are no players (the value is discarded);
    //       truncates the pointer to 32 bits; a port leaves this line out.
    nWinner = (s32)((u64)(s64)nWinner | ((u64)(u32)pPlayer << 32));
    lbl_80195498.n8 = nWinner;
    lbl_80195498.bC = 1;
    if (bTie) {
        lbl_80195498.n8 = 5;
        lbl_80195498.bC = 0;
        if (lbl_80195498.n0 == 1) {
            for (i = 0; i < n; i++) {
                PLAYER(i)->nEBC = 0;
            }
        }
    }
}
#pragma pop

// The contest's scoring group of lie nLie: 0 the tee; 1 the fairways and the fringe (and any lie
// not listed: cart path, water, out of bounds); 2 the roughs, ice, snow and misc; 3 the sands; 4
// the green; 5 in the cup.
s32 GameMode22_GetLieGroup(int n) {
    switch (n) {
    case 0:
        return 0;
    case 1:
    case 2:
    case 10:
        return 1;
    case 3:
    case 4:
    case 5:
    case 14:
    case 15:
    case 17:
        return 2;
    case 6:
    case 7:
    case 8:
        return 3;
    case 9:
        return 4;
    case 12:
        return 5;
    default:
        return 1;
    }
}

// Adds message n to the list the shot's comment is picked from (20 at most). Our name.
#define ADD_MSG(n)                          \
    if (nMsgs < 20) {                       \
        aMsgs[nMsgs] = (n);                 \
        nMsgs++;                            \
    }

// A drive is over (pfn244; GameMode22_BallOutOfBounds for one out of bounds). It counts as a drive
// (nEA0) and scores by where the ball ended: surface 0x9B kind 1, the length plus 20%; surface 0x2F
// or 0x68 kind 4, -100 (with a sound); bLowIQPenalty set kind 5, -100; else by
// GameMode22_GetLieGroup: the tee or the rough kind 2, 0 points; the fairway, green or cup kind 0,
// the length; sand kind 3, -50. Kinds 0 and 1 are fair drives: counted (nEA4), their total (nEC4)
// and average (nEC0) kept, and 400 or more earns 100 more; each kind has its own count (nEC8, nECC,
// nED0, nED4, nED8, nEDC). In variant 1 a drive scores only what it adds to the player's best fair
// drive. The score (nEBC) never drops below 0 and goes to the scoreboard (message 0x42); sounds the
// first time it reaches 400, 800 and 1200. The player's longest fair drive (nEA8) and where it lay
// (vEAC) are kept, with message 0x4C when it beats the other player's. Then
// GameMode22_DecideWinner: a winner gets his commentary line, otherwise one line is picked at
// random from those the drive earned (a new longest drive, a long one, a bad one).
void GameMode22_ScoreShot(int nPlayer) {
    s32 nPoints;
    s32 nKind;
    Player* pPlayer = &gPlayers[nPlayer];
    s32 nLength;
    u8 bCounts = 0;
    s32 nMsgs;
    u16 aMsgs[20];
    s32 nScore;

    nMsgs = 0;
    pPlayer->nEA0++;
    switch (pPlayer->ball.nSurface) {
    case 0x9B:
        nKind = 1;
        bCounts = 1;
        break;
    case 0x2F:
    case 0x68:
        nKind = 4;
        break;
    default:
        if (pPlayer->bLowIQPenalty) {
            nKind = 5;
            break;
        }
        switch (GameMode22_GetLieGroup(pPlayer->ball.nLie)) {
        case 0:
        case 2:
            nKind = 2;
            break;
        case 1:
        case 4:
        case 5:
            nKind = 0;
            bCounts = 1;
            break;
        case 3:
            nKind = 3;
            break;
        default:
            nKind = 0;
            break;
        }
        break;
    }

    nLength = fn_800D0550(nPlayer);
    if (bCounts) {
        gPlayers[nPlayer].nEA4++;
        gPlayers[nPlayer].nEC4 += nLength;
        gPlayers[nPlayer].nEC0 = (f32)gPlayers[nPlayer].nEC4 / (f32)gPlayers[nPlayer].nEA4;
    }

    nPoints = 0;
    switch (nKind) {
    case 1:
        ADD_MSG(0x17);
        gPlayers[nPlayer].nECC++;
        nPoints = nLength + (s32)(0.2f * nLength);
        break;
    case 0:
        nPoints = nLength;
        gPlayers[nPlayer].nEC8++;
        break;
    case 2:
        ADD_MSG(0x1C);
        nPoints = 0;
        gPlayers[nPlayer].nED0++;
        break;
    case 3:
        ADD_MSG(0x1D);
        nPoints = -50;
        gPlayers[nPlayer].nED4++;
        break;
    case 4:
        if (Game_GetCurHoleNum() == 4) {
            // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
            ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(1, 0, 2, 0, 0);
        } else {
            // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
            ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(1, 0, 1, 0, 0);
        }
        ADD_MSG(0x1E);
        nPoints = -100;
        gPlayers[nPlayer].nED8++;
        break;
    case 5:
        ADD_MSG(0x1B);
        nPoints = -100;
        gPlayers[nPlayer].nEDC++;
        break;
    }
    if (nLength >= 400 && (nKind == 1 || nKind == 0)) {
        nPoints += 100;
    }

    // A scoring shot longer than the longest so far (f10, by n14) is the new longest.
    if (nPoints > 0) {
        if (nLength > lbl_80195498.f10) {
            if (lbl_80195498.f10 != 0.0f) {
                if (nPlayer != lbl_80195498.n14 && lbl_80195498.n14 != 5) {
                    GameMsg_Send2Ints(0x4C, nLength, nPlayer);
                    ADD_MSG(0xB);
                    if (nPlayer == 0) {
                        ADD_MSG(5);
                        ADD_MSG(9);
                    }
                    if (nPlayer == 1) {
                        ADD_MSG(6);
                        ADD_MSG(0xA);
                    }
                }
                if (nPlayer == 0) {
                    ADD_MSG(7);
                }
                if (nPlayer == 1) {
                    ADD_MSG(8);
                }
                ADD_MSG(0xC);
            }
            lbl_80195498.n14 = nPlayer;
            lbl_80195498.f10 = nLength;
        } else if (nLength > 400) {
            ADD_MSG(0x22);
        }
    }
    if (nPoints < 0) {
        ADD_MSG(0x1A);
    }
    if (nPoints == 0) {
        ADD_MSG(0x19);
    }

    // With n0 1 a shot only scores what it adds to the player's best.
    if (lbl_80195498.n0 == 1) {
        nPoints = 0;
        if (bCounts && pPlayer->nEBC < nLength) {
            nPoints = nLength - pPlayer->nEBC;
        }
    }
    pPlayer->nEBC += nPoints;
    // The score never drops below 0.
    nScore = pPlayer->nEBC;
    nScore = (nScore <= 0) ? 0 : nScore;
    pPlayer->nEBC = nScore;
    fn_800E5CA4(0, gPlayers[nPlayer].nEBC, nLength, nKind, 0, 0, nPoints, 0.0f);

    // A track the first time the score reaches 1200, 800 and 400.
    if (!lbl_8028259C[nPlayer] && gPlayers[nPlayer].nEBC >= 1200) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 2, 0, 0);
        lbl_8028259C[nPlayer] = 1;
    }
    if (!lbl_80282594[nPlayer] && gPlayers[nPlayer].nEBC >= 800) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 3, 0, 0);
        lbl_80282594[nPlayer] = 1;
    }
    if (!lbl_8028258C[nPlayer] && gPlayers[nPlayer].nEBC >= 400) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 4, 0, 0);
        lbl_8028258C[nPlayer] = 1;
    }

    // The player's longest counted shot, and where the ball lay.
    if (bCounts && nLength > pPlayer->nEA8) {
        pPlayer->nEA8 = nLength;
        LLMath_CopyVec(pPlayer->ball.vPos, pPlayer->vEAC);
        if (gPlayers[nPlayer].nEA8 > gPlayers[1 - nPlayer].nEA8) {
            GameMsg_Send2Ints(0x4C, nLength, nPlayer);
            // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
            ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 0, 0, 0);
        }
    }

    GameMode22_DecideWinner();
    if (lbl_80195498.n8 != 5) {
        nMsgs = 0;
        if (nPlayer > 1) {
            if (nPlayer == lbl_80195498.n8) {
                fn_8010D428(2, 0);
            } else {
                fn_8010D428(1, 0);
            }
        } else if (lbl_80195498.n8 == 0) {
            fn_8010D428(3, 0);
        } else if (lbl_80195498.n8 == 1) {
            fn_8010D428(4, 0);
        } else {
            fn_8010D428(1, 0);
        }
    }
    if (nMsgs > 0 && lbl_80195498.n8 == 5) {
        fn_8010D428(aMsgs[Misc_RandFunc(1) % nMsgs], 0);
    }
}

// The hole starts (pfn1E4): every player's contest values cleared (GameMode22_ClearPlayerStats).
void GameMode22_HoleStart(void) {
    GameMode22_ClearPlayerStats();
}

// The hole restarts (pfn224): every player's contest values cleared, and no winner (n8 5, bC 0).
void GameMode22_RestartHole(void) {
    GameMode22_ClearPlayerStats();
    lbl_80195498.n8 = 5;
    lbl_80195498.bC = 0;
}

// Every player's contest values cleared: drives, fair drives, longest drive, score (nEBC), average,
// total and the counts per kind (0xEA0..0xEDC; vEAC stays), each score sent to the scoreboard
// (message 0x42); no current player (5); then GameMode22_AfterClearStats (empty).
void GameMode22_ClearPlayerStats(void) {
    s32 i;

    for (i = 0; i < 5; i++) {
        PLAYER(i)->nEA0 = 0;
        PLAYER(i)->nEA4 = 0;
        PLAYER(i)->nEA8 = 0;
        PLAYER(i)->nEBC = 0;
        PLAYER(i)->nEC0 = 0;
        PLAYER(i)->nEC4 = 0;
        PLAYER(i)->nEC8 = 0;
        PLAYER(i)->nECC = 0;
        PLAYER(i)->nED0 = 0;
        PLAYER(i)->nED4 = 0;
        PLAYER(i)->nED8 = 0;
        PLAYER(i)->nEDC = 0;
        fn_800E5CA4(i, PLAYER(i)->nEBC, 0, 0, 0, 0, 0, 0.0f);
    }
    lbl_80282278 = 5;
    fn_80126F7C();
}

void fn_80126F7C(void) {
}

void fn_80126F80(void) {
}

void fn_80126F84(s32 p0) {
    lbl_80195498.n4 = p0;
}

void fn_80126F94(s32 n) {
    lbl_80195498.n0 = n;
}

s32 fn_80126FA0(void) {
    return lbl_80195498.n0;
}

// n8 (when pn8 is not NULL) and bC.
u8 fn_80126FB0(s32* pn8) {
    if (pn8 != NULL) {
        *pn8 = lbl_80195498.n8;
    }
    return lbl_80195498.bC;
}

s32 fn_80126FD8(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((22 - t0)) >> 5) & 0xFF);
}

u8 fn_80127004(void) {
    if (lbl_80195498.n8 != 5 && lbl_80195498.n18 > 0) {
        return 1;
    }
    return 0;
}

// Shows n4 less the current player's nEA0 as a message (nPlayer is not read; every caller passes
// one).
void fn_80127034(int nPlayer) {
    sprintf(lbl_80195488, "%d", lbl_80195498.n4 - gPlayers[lbl_80282278].nEA0);
    fn_800E5D68(lbl_80195488);
}

s32 fn_80127098(s32 arg0) {
    switch (arg0) {
    case 6:
        return 0;
    case 7:
        return 1;
    case 5:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}
