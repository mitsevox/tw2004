// GameMode26.c (our name; no EA name found): game mode 26, a two-player long-drive race and the
// near twin of GameMode22.c's contest. Always split screen (GM_SetSplitScreenForMode), with both
// players' shot lengths shown and nobody holding the honor (GameMode26_GetHonors). Every drive is
// scored the way GameMode22_ScoreShot scores it (a fair drive's length, 20% more on surface 0x9B,
// 100 more from 400 up; nothing in the rough; less for sand, surfaces 0x2F and 0x68 and the low-IQ
// penalty) and the points add up; the first player to the target score (GameMode26_SetTargetScore
// from the menu, 10000 until set) wins, and the hole ends when the 120-frame winner countdown runs
// out. GM_SetModeType sets it up with GameMode26_Init, the menus start it with
// GameMode26_StartEvent. There is no prize: the win goes to the EA SPORTS Bio. The file also holds
// GameMode26_StartComment (the long-drive commentary that GameMode22.c uses too) and
// GameMode26_BallBounceSound (Gaud_BallBounce, both modes). The file ends where CharSliders.c
// begins (CharSlider_Free, the slider code char.c calls).

#include "engine.h"
#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "game/modes/mode26.h"

s32 gGameMode26TargetScore = 10000;
s32 gGameMode26Winner = 5;
s32 gGameMode26LongestPlayer = 5;
s32 gGameMode26WinnerCountdown = 120;
s32 gGameMode26LengthCheckFrames = 15;
s32 gGameMode26LastLength[2] = {0, 0};
s32 gGameMode26LengthSoundOn[2] = {0, 0};

// .sbss in reverse address order (CodeWarrior lays it out backwards)
u8  gGameMode26Reached1200[5];
u8  gGameMode26Reached800[5];
u8  gGameMode26Reached400[5];
f32 gGameMode26LongestLength;
u8 gGameMode26SplitScreenShot;      // set when the session is split screen (GameMode26_NoteSplitScreenShot)
u8  gGameMode26IntroSaid;

void GameMode26_Shutdown(void);
void GameMode26_SetupNextGolfer(void);
void GameMode26_EndGame(void);
void GameMode26_UpdateFrame(void);
void GameMode26_StartSwing(int nPlayer);
u8   GameMode26_GoToPlayoff(u8 bCheck);
s32  GameMode26_GetHonors(int nPlayer);
u8   GameMode26_HoleFinished(int nPlayer, u8 bCheck);
u8   GameMode26_GameFinished(u8 bCheck);
void GameMode26_BallOutOfBounds(int nPlayer);
void GameMode26_EndGolferTurn(int nPlayer);
s32  GameMode26_GetLieGroup(int nLie);
void GameMode26_ScoreShot(PlayerNumber_t nPlayer);
void GameMode26_HoleStart(void);
void GameMode26_RestartHole(void);
void GameMode26_ClearPlayerStats(void);
void GameMode26_AfterClearStats(void);
void GameMode26_PreSwing(void);
u8   GameMode26_GetWinner(s32* pnWinner);

// Game mode 26's setup (GM_SetModeType): its callbacks; no gimmes, mulligans, stroke limit,
// GameBreakers (b285), flight-camera toggles or in-flight replays (b286, b287), yardage or bumped
// obstructions; two players (Session_SetNumPlayers), split screen as chosen (lbl_8028227C;
// GM_SetSplitScreenForMode turns it on for this mode). No longest drive yet, the 120-frame winner
// countdown and the intro comment reset, and the per-player score sounds (400 / 800 / 1200)
// cleared. The winner is not reset here (GameMode26_StartEvent, GameMode26_RestartHole).
void GameMode26_Init(void) {
    s32 i;

    gpGame->pfnInit = GameMode26_Init;
    gpGame->pfnShutdown = GameMode26_Shutdown;
    gpGame->pfnSetupNextGolfer = GameMode26_SetupNextGolfer;
    gpGame->pfnGetHonors = GameMode26_GetHonors;
    gpGame->pfnBallOOB = GameMode26_BallOutOfBounds;
    gpGame->pfnHoleFinished = GameMode26_HoleFinished;
    gpGame->pfnGameFinished = GameMode26_GameFinished;
    gpGame->pfnGoToPlayoff = GameMode26_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameMode26_EndGolferTurn;
    gpGame->pfnEndGame = GameMode26_EndGame;
    gpGame->pfnUpdate = GameMode26_UpdateFrame;
    gpGame->pfnPreShotInit = GameMode26_StartSwing;
    // port: its parameter is PlayerNumber_t, pfnCheckShotAwards's int
    gpGame->pfnCheckShotAwards = (void (*)(int))GameMode26_ScoreShot;
    gpGame->pfnLoadHole = GameMode26_HoleStart;
    gpGame->pfnRestartHole = GameMode26_RestartHole;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 1;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->bAllowGameBreakers = 0;
    gpGame->b274 = 0;
    gpGame->b286 = 0;
    gpGame->b287 = 0;
    gpGame->b288 = 0;
    gpGame->b289 = 0;
    gpGame->b28A = 0;
    gpGame->bBumpObstructions = 0;
    gpGame->bShowYardage = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->nDC = 0;
    gpGame->nScoringType = 0;
    gSession.nSplitScreen = lbl_8028227C;
    gGameMode26SplitScreenShot = 0;
    gGameMode26LongestLength = 0.0f;
    gGameMode26LongestPlayer = 5;
    gGameMode26WinnerCountdown = 120;
    gGameMode26IntroSaid = 0;
    Session_SetNumPlayers(2);
    for (i = 0; i < 5; i++) {
        gGameMode26Reached1200[i] = 0;
        gGameMode26Reached800[i] = 0;
        gGameMode26Reached400[i] = 0;
    }
}

// The mode's pfnShutdown: empty.
void GameMode26_Shutdown(void) {
}

// Starts a mode-26 event: from the menus (FE_MessageTable.c GM_vStartEventCheckDisc, as GameMode22_StartEvent
// for mode 22) and, with session flag 0x4000, from GoEntry.c as the front end starts again. Both
// players on tee set 0, options.nFairwaySpeed off, and no winner (gGameMode26Winner 5).
void GameMode26_StartEvent(void) {
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.options.nFairwaySpeed = 0;
    gGameMode26Winner = 5;
}

// The mode's pfnSetupNextGolfer: empty in this build (GameMode22's calls stroke play's).
void GameMode26_SetupNextGolfer(void) {
}

// The game is over (pfnEndGame): the EA SPORTS Bio counts a won game (EASBio_SetCurrentGameWon). No
// money is paid.
void GameMode26_EndGame(void) {
    EASBio_SetCurrentGameWon(1);
}

// Each frame (pfnUpdate). Every 16 frames, for both players: the shot length (fn_800D0550) sent as
// message 0x4D with the player; while it is nonzero and still changing a long-drive UI sound plays
// (script 0, track 1; started once per player; EA also passes a loop flag and the player's side,
// which Gaud_LongDriveUi_Play ignores), and it stops once the length stops changing. Once somebody
// has won (gGameMode26Winner not 5) the winner countdown runs down. On the first frame the mode's
// intro comment (line 0) is said.
void GameMode26_UpdateFrame(void) {
    s32 i;
    s32 nLength;

    if (gGameMode26LengthCheckFrames-- <= 0) {
        for (i = 0; i <= 1; i++) {
            nLength = fn_800D0550(i);
            GameMsg_Send2Ints(0x4D, i, nLength);
            if (nLength != 0 && nLength != *(s32*)((u8*)gGameMode26LastLength + i * sizeof(s32))) {
                if (*(s32*)((u8*)gGameMode26LengthSoundOn + i * sizeof(s32)) == 0) {
                    // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
                    ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 1, 0, 1, (i != 0) ? 1 : -1);
                    *(s32*)((u8*)gGameMode26LengthSoundOn + i * sizeof(s32)) = 1;
                }
            } else if (*(s32*)((u8*)gGameMode26LengthSoundOn + i * sizeof(s32)) != 0) {
                // port: EA passes an argument Gaud_LongDriveUi_Stop ignores
                ((void (*)(s32, int, int))Gaud_LongDriveUi_Stop)(0, 1, 0);
                *(s32*)((u8*)gGameMode26LengthSoundOn + i * sizeof(s32)) = 0;
            }
            *(s32*)((u8*)gGameMode26LastLength + i * sizeof(s32)) = nLength;
        }
        gGameMode26LengthCheckFrames = 15;
    }
    if (gGameMode26Winner != 5) {
        gGameMode26WinnerCountdown--;
    }
    if (!gGameMode26IntroSaid) {
        GameMode26_StartComment(0, 0);
        gGameMode26IntroSaid = 1;
    }
}

// As a swing begins (pfnPreShotInit): GameMode26_PreSwing (empty). nPlayer is not read.
void GameMode26_StartSwing(int nPlayer) {
    GameMode26_PreSwing();
}

// The mode's pfnGoToPlayoff: never a playoff (0).
u8 GameMode26_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): always 5, nobody; the mode never hands the turn to a player.
// nPlayer is not read.
s32 GameMode26_GetHonors(int nPlayer) {
    return 5;
}

// The hole is over (pfnHoleFinished) once somebody has won (GameMode26_GameFinished) and the
// 120-frame winner countdown (gGameMode26WinnerCountdown) has run out. nPlayer is not read.
u8 GameMode26_HoleFinished(int nPlayer, u8 bCheck) {
    s32 bRet = 0;

    if (GameMode26_GameFinished(bCheck) && gGameMode26WinnerCountdown < 0) {
        bRet = 1;
    }
    return bRet;
}

// The game is over (pfnGameFinished) once somebody has reached the target score
// (GameMode26_GetWinner). bCheck is not read.
u8 GameMode26_GameFinished(u8 bCheck) {
    return GameMode26_GetWinner(NULL);
}

// The ball went out of bounds (pfnBallOOB): the drive is scored like any other
// (GameMode26_ScoreShot).
void GameMode26_BallOutOfBounds(int nPlayer) {
    GameMode26_ScoreShot(nPlayer);
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (his tee set)
// for the next drive.
void GameMode26_EndGolferTurn(int nPlayer) {
    Physics_InitBall(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

// The contest's scoring group of lie nLie, as GameMode22_GetLieGroup: 0 the tee; 1 the fairways and
// the fringe (and any lie not listed: cart path, water, out of bounds); 2 the roughs, ice, snow and
// misc; 3 the sands; 4 the green; 5 in the cup.
s32 GameMode26_GetLieGroup(int nLie) {
    switch (nLie) {
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

#define ADD_MSG(n)                          \
    if (nMsgs < 20) {                       \
        aMsgs[nMsgs] = (n);                 \
        nMsgs++;                            \
    }

// A drive is over (pfnCheckShotAwards; GameMode26_BallOutOfBounds for one out of bounds); nothing
// once somebody has won. Scored as in GameMode22_ScoreShot: surface 0x9B kind 1, the length plus
// 20%; surface 0x2F or 0x68 kind 4, -100 (with a sound); bLowIQPenalty set kind 5, -100; else by
// GameMode26_GetLieGroup: the tee or the rough kind 2, 0 points; the fairway, green or cup kind 0,
// the length; sand kind 3, -50. Kinds 0 and 1 are fair drives: counted (nFairDrives), their total
// (nFairDriveTotal) and average (nAverageDrive) kept, and 400 or more earns 100 more; every drive
// counts (nDrivesTaken) and each kind has its own count. The score (nDriveScore) never drops below
// 0 and goes to the scoreboard (message 0x42); sounds the first time it reaches 400, 800 and 1200.
// The player's longest fair drive (nBestDrive) and where it lay (vBestDrivePos) are kept, with
// message 0x4C when it beats the other player's. The first to gGameMode26TargetScore points wins
// (gGameMode26Winner) with one of four winning lines; otherwise one line is said at random from
// those the drive earned (a new longest drive, a long one, a bad one, taking the lead).
void GameMode26_ScoreShot(PlayerNumber_t nPlayer) {
    s32 nKind;
    Player* pPlayer = &gPlayers[nPlayer];
    u8 bCounts = 0;
    s32 nMsgs = 0;
    s32 nLength;
    s32 nLead;
    s32 nPoints;
    s32 nTotal;
    s32 nPick;
    u16 aMsgs[20];

    if (GameMode26_GetWinner(NULL)) {
        return;
    }
    nLead = gPlayers[nPlayer].nDriveScore - gPlayers[nPlayer == 0].nDriveScore;
    gPlayers[nPlayer].nDrivesTaken++;
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
        if (pPlayer->bPenaltyShot) {
            nKind = 5;
            break;
        }
        switch (GameMode26_GetLieGroup(pPlayer->ball.nLie)) {
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
        gPlayers[nPlayer].nFairDrives++;
        gPlayers[nPlayer].nFairDriveTotal += nLength;
        gPlayers[nPlayer].nAverageDrive = (f32)gPlayers[nPlayer].nFairDriveTotal
                / (f32)gPlayers[nPlayer].nFairDrives;
    }

    nPoints = 0;
    switch (nKind) {
    case 1:
        ADD_MSG(0x17);
        gPlayers[nPlayer].nBonusDrives++;
        nPoints = nLength + (s32)(0.2f * nLength);
        break;
    case 0:
        nPoints = nLength;
        gPlayers[nPlayer].nFairwayDrives++;
        break;
    case 2:
        ADD_MSG(0x1C);
        nPoints = 0;
        gPlayers[nPlayer].nRoughDrives++;
        break;
    case 3:
        ADD_MSG(0x1D);
        nPoints = -50;
        gPlayers[nPlayer].nSandDrives++;
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
        gPlayers[nPlayer].nSurfacePenaltyDrives++;
        break;
    case 5:
        ADD_MSG(0x1B);
        nPoints = -100;
        gPlayers[nPlayer].nPenaltyDrives++;
        break;
    }
    if (nLength >= 400 && (nKind == 1 || nKind == 0)) {
        nPoints += 100;
    }

    // A scoring shot longer than the longest so far is the new longest.
    if (nPoints > 0) {
        if (nLength > gGameMode26LongestLength) {
            if (gGameMode26LongestLength != 0.0f) {
                if (nPlayer != gGameMode26LongestPlayer && gGameMode26LongestPlayer != 5) {
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
            gGameMode26LongestPlayer = nPlayer;
            gGameMode26LongestLength = nLength;
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

    pPlayer->nDriveScore += nPoints;
    nTotal = pPlayer->nDriveScore;
    pPlayer->nDriveScore = (nTotal <= 0) ? 0 : nTotal;     // never below zero
    GUI_UpdateLongDriveScore(nPlayer, gPlayers[nPlayer].nDriveScore, nLength, nKind, 0, 0, nPoints, 0.0f);

    // A track the first time the score reaches 1200, 800 and 400 (EA also passes the player's
    // side, 1 or -1, which Gaud_LongDriveUi_Play ignores).
    if (!gGameMode26Reached1200[nPlayer] && gPlayers[nPlayer].nDriveScore >= 1200) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 2, 0, (nPlayer != 0) ? 1 : -1);
        gGameMode26Reached1200[nPlayer] = 1;
    }
    if (!gGameMode26Reached800[nPlayer] && gPlayers[nPlayer].nDriveScore >= 800) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 3, 0, (nPlayer != 0) ? 1 : -1);
        gGameMode26Reached800[nPlayer] = 1;
    }
    if (!gGameMode26Reached400[nPlayer] && gPlayers[nPlayer].nDriveScore >= 400) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 4, 0, (nPlayer != 0) ? 1 : -1);
        gGameMode26Reached400[nPlayer] = 1;
    }

    // The shot took the lead.
    if (nLead < 0 && gPlayers[nPlayer].nDriveScore - gPlayers[nPlayer == 0].nDriveScore > 0) {
        ADD_MSG(0x18);
    }

    // The player's longest counted shot, and where the ball lay.
    if (bCounts && nLength > pPlayer->nBestDrive) {
        pPlayer->nBestDrive = nLength;
        LLMath_CopyVec(pPlayer->ball.vPos, pPlayer->vBestDrivePos);
        if (gPlayers[nPlayer].nBestDrive > gPlayers[1 - nPlayer].nBestDrive) {
            GameMsg_Send2Ints(0x4C, nLength, nPlayer);
            // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
            ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(0, 0, 0, 0, 0);
        }
    }

    // The target score reached: the player wins, with one of four winning messages.
    if (pPlayer->nDriveScore >= gGameMode26TargetScore) {
        gGameMode26Winner = nPlayer;
        nMsgs = 0;
        nPick = Misc_RandFunc(1) % 3;
        if (nPick == 0) {
            GameMode26_StartComment(1, 0);
        } else if (nPick == 1) {
            GameMode26_StartComment(2, 0);
        } else if (nPlayer == 0) {
            GameMode26_StartComment(3, 0);
        } else {
            GameMode26_StartComment(4, 0);
        }
    }
    if (nMsgs > 0 && gGameMode26Winner == 5) {
        GameMode26_StartComment(aMsgs[Misc_RandFunc(1) % nMsgs], 0);
    }
}

// The hole starts (pfnLoadHole): every player's contest values cleared
// (GameMode26_ClearPlayerStats).
void GameMode26_HoleStart(void) {
    GameMode26_ClearPlayerStats();
}

// The hole restarts (pfnRestartHole): every player's contest values cleared, and no winner
// (gGameMode26Winner 5).
void GameMode26_RestartHole(void) {
    GameMode26_ClearPlayerStats();
    gGameMode26Winner = 5;
}

// Every player's contest values cleared: drives, fair drives, longest drive, score (nDriveScore),
// average, total and the counts per kind (0xEA0..0xEDC; vBestDrivePos stays), each score sent to
// the scoreboard (message 0x42); then GameMode26_AfterClearStats (empty). Unlike
// GameMode22_ClearPlayerStats it leaves the current player (lbl_80282278) as it is.
void GameMode26_ClearPlayerStats(void) {
    s32 i;

    for (i = 0; i < 5; i++) {
        PLAYER(i)->nDrivesTaken = 0;
        PLAYER(i)->nFairDrives = 0;
        PLAYER(i)->nBestDrive = 0;
        PLAYER(i)->nDriveScore = 0;
        PLAYER(i)->nAverageDrive = 0;
        PLAYER(i)->nFairDriveTotal = 0;
        PLAYER(i)->nFairwayDrives = 0;
        PLAYER(i)->nBonusDrives = 0;
        PLAYER(i)->nRoughDrives = 0;
        PLAYER(i)->nSandDrives = 0;
        PLAYER(i)->nSurfacePenaltyDrives = 0;
        PLAYER(i)->nPenaltyDrives = 0;
        GUI_UpdateLongDriveScore(i, PLAYER(i)->nDriveScore, 0, 0, 0, 0, 0, 0.0f);
    }
    GameMode26_AfterClearStats();
}

// Empty in this build; GameMode26_ClearPlayerStats calls it last.
void GameMode26_AfterClearStats(void) {
}

// Empty in this build; GameMode26_StartSwing calls it.
void GameMode26_PreSwing(void) {
}

// The score that wins (gGameMode26TargetScore, 10000 until set), from the menu (FE_MessageTable.c
// GM_vSetLongDriveTargetScore).
void GameMode26_SetTargetScore(s32 nScore) {
    gGameMode26TargetScore = nScore;
}

// Whether somebody has won (gGameMode26Winner not 5); the winner (5 none) into *pnWinner when
// pnWinner is not NULL.
u8 GameMode26_GetWinner(s32* pnWinner) {
    if (pnWinner != NULL) {
        *pnWinner = gGameMode26Winner;
    }
    return gGameMode26Winner != 5;
}

// Whether game mode 26 is being played (GUI_IsPostShotUIAnimating asks).
u8 GameMode26_IsActive(void) {
    return Game_GetMode() == 26;
}

// Whether somebody has won and the 120-frame winner countdown (gGameMode26WinnerCountdown) is still
// running; the post-shot UI keeps animating meanwhile (GUI_IsPostShotUIAnimating).
u8 GameMode26_IsShowingWinner(void) {
    if (gGameMode26Winner != 5 && gGameMode26WinnerCountdown > 0) {
        return 1;
    }
    return 0;
}

// Called by event.c's handler of event 10 (EVENT_HitBall, nArg 1) in every mode: in split screen it
// sets gGameMode26SplitScreenShot, which nothing in the binary reads. nPlayer is not read.
void GameMode26_NoteSplitScreenShot(int nPlayer) {
    if (gSession.nSplitScreen) {
        gGameMode26SplitScreenShot = 1;
    }
}

// A ball bounce in the long-drive modes (Gaud_BallBounce, modes 22 and 26): on surface 155 (0x9B,
// where a drive scores 20% more) a long-drive sound plays (script 1, stepped to 0).
void GameMode26_BallBounceSound(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];

    if (pPlayer->ball.nSurface == 155) {
        // port: EA passes two arguments Gaud_LongDriveUi_Play ignores
        ((void (*)(s32, int, int, int, int))Gaud_LongDriveUi_Play)(1, 0, 0, 0, 0);
    }
}

// Plays line nLine of commentary playlist 8, the long-drive contests' lines (Gaud_StartComment; a
// is passed on, 0 from every caller). GameMode22_ScoreShot uses it too.
void GameMode26_StartComment(s32 nLine, s32 a) {
    Gaud_StartComment(8, nLine, a);
}
