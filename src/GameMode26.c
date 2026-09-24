// GameMode26.c (our name): game mode 26, a two-player long-drive contest and GameMode22.c's near
// twin: each scoring shot earns its length in points, the first player to the target score
// (fn_8010D334) wins. GameRound.c starts it with fn_8010C4A0 (its case 26), which fills gpGame's
// callbacks with this file's hooks the way GameMode22.c's fn_80125E68 does; fn_8010D364 tests
// Game_GetMode() == 26. The file ends where CharSliders.c begins (CharSlider_Free, the slider code
// char.c calls).

#include "engine.h"
#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "game/modes/mode26.h"

s32 lbl_80281760 = 10000;
s32 lbl_80281764 = 5;
s32 lbl_80281768 = 5;
s32 lbl_8028176C = 120;
s32 lbl_80281770 = 15;
s32 lbl_80281774[2] = {0, 0};
s32 lbl_8028177C[2] = {0, 0};

// .sbss in reverse address order (CodeWarrior lays it out backwards)
u8  lbl_802824A8[5];
u8  lbl_802824A0[5];
u8  lbl_80282498[5];
f32 lbl_80282494;
u8 lbl_80282491;                        // set when the session is split screen (fn_8010D3B8)
u8  lbl_80282490;

void fn_8010C714(void);
void fn_8010C73C(void);
void fn_8010C740(void);
void fn_8010C764(void);
void fn_8010C8B8(int nPlayer);
u8   fn_8010C8D8(u8 bCheck);
s32  fn_8010C8E0(int nPlayer);
u8   fn_8010C8E8(int nPlayer, u8 bCheck);
u8   fn_8010C934(u8 bCheck);
void fn_8010C958(int nPlayer);
void fn_8010C978(int nPlayer);
s32  fn_8010C9D4(int nLie);
void fn_8010CA2C(PlayerNumber_t nPlayer);
void fn_8010D230(void);
void fn_8010D250(void);
void fn_8010D278(void);
void fn_8010D32C(void);
void fn_8010D330(void);
u8   fn_8010D33C(s32* pnWinner);

// The mode's setup: its callbacks, its rules (no gimmes, no mulligans, two players) and its
// state reset.
void fn_8010C4A0(void) {
    s32 i;

    gpGame->pfnInit = fn_8010C4A0;
    gpGame->pfnShutdown = fn_8010C714;
    gpGame->pfnSetupNextGolfer = fn_8010C73C;
    gpGame->pfnGetHonors = fn_8010C8E0;
    gpGame->pfn250 = fn_8010C958;
    gpGame->pfnHoleFinished = fn_8010C8E8;
    gpGame->pfnGameFinished = fn_8010C934;
    gpGame->pfnGoToPlayoff = fn_8010C8D8;
    gpGame->pfnEndGolferTurn = fn_8010C978;
    gpGame->pfnEndGame = fn_8010C740;
    gpGame->pfn220 = fn_8010C764;
    gpGame->pfn20C = fn_8010C8B8;
    gpGame->pfn244 = (void (*)(int))fn_8010CA2C;    // port: its parameter is PlayerNumber_t, pfn244's int
    gpGame->pfn1E4 = fn_8010D230;
    gpGame->pfn224 = fn_8010D250;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 1;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b285 = 0;
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
    gpGame->n4 = 0;
    gSession.nSplitScreen = lbl_8028227C;
    lbl_80282491 = 0;
    lbl_80282494 = 0.0f;
    lbl_80281768 = 5;
    lbl_8028176C = 120;
    lbl_80282490 = 0;
    Session_SetNumPlayers(2);
    for (i = 0; i < 5; i++) {
        lbl_802824A8[i] = 0;
        lbl_802824A0[i] = 0;
        lbl_80282498[i] = 0;
    }
}

void fn_8010C714(void) {
}

// Both players on the first tee set, the option n20 off and no winner.
void fn_8010C718(void) {
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.options.n20 = 0;
    lbl_80281764 = 5;
}

void fn_8010C73C(void) {
}

void fn_8010C740(void) {
    EASBio_SetCurrentGameWon(1);
}

// The mode's frame: every 16 frames each player's current shot length sent as message 0x4D (a
// track plays, panned to the player's side, while it is a new nonzero length), the countdown once
// there is a winner, and the mode's first message once.
void fn_8010C764(void) {
    s32 i;
    s32 nLength;

    if (lbl_80281770-- <= 0) {
        for (i = 0; i <= 1; i++) {
            nLength = fn_800D0550(i);
            fn_80062D38(0x4D, i, nLength);
            if (nLength != 0 && nLength != *(s32*)((u8*)lbl_80281774 + i * sizeof(s32))) {
                if (*(s32*)((u8*)lbl_8028177C + i * sizeof(s32)) == 0) {
                    // port: EA passes two arguments fn_800A746C ignores
                    ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 1, 0, 1, (i != 0) ? 1 : -1);
                    *(s32*)((u8*)lbl_8028177C + i * sizeof(s32)) = 1;
                }
            } else if (*(s32*)((u8*)lbl_8028177C + i * sizeof(s32)) != 0) {
                // port: EA passes an argument fn_800A74E4 ignores
                ((void (*)(s32, int, int))fn_800A74E4)(0, 1, 0);
                *(s32*)((u8*)lbl_8028177C + i * sizeof(s32)) = 0;
            }
            *(s32*)((u8*)lbl_80281774 + i * sizeof(s32)) = nLength;
        }
        lbl_80281770 = 15;
    }
    if (lbl_80281764 != 5) {
        lbl_8028176C--;
    }
    if (!lbl_80282490) {
        fn_8010D428(0, 0);
        lbl_80282490 = 1;
    }
}

void fn_8010C8B8(int nPlayer) {
    fn_8010D330();
}

u8 fn_8010C8D8(u8 bCheck) {
    return 0;
}

// Always 5: no player has the honor.
s32 fn_8010C8E0(int nPlayer) {
    return 5;
}

// The hole is over once the game is and the countdown has run out.
u8 fn_8010C8E8(int nPlayer, u8 bCheck) {
    s32 bRet = 0;

    if (fn_8010C934(bCheck) && lbl_8028176C < 0) {
        bRet = 1;
    }
    return bRet;
}

// The game is over once somebody has won.
u8 fn_8010C934(u8 bCheck) {
    return fn_8010D33C(NULL);
}

// The ball went out of bounds: scored as a shot.
void fn_8010C958(int nPlayer) {
    fn_8010CA2C(nPlayer);
}

// A fresh ball for the player on the tee set the session gives it.
void fn_8010C978(int nPlayer) {
    Physics_InitBall(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

// The group a lie falls in for scoring (fn_8010CA2C).
s32 fn_8010C9D4(int nLie) {
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

// A shot is over: score it by where the ball ended up, keep the players' shot statistics, play
// the tracks for new records and point totals, and say one of the collected messages. The first
// player to lbl_80281760 points wins.
void fn_8010CA2C(PlayerNumber_t nPlayer) {
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

    if (fn_8010D33C(NULL)) {
        return;
    }
    nLead = gPlayers[nPlayer].nEBC - gPlayers[nPlayer == 0].nEBC;
    gPlayers[nPlayer].nEA0++;
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
        switch (fn_8010C9D4(pPlayer->ball.nLie)) {
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
        if (fn_80015464() == 4) {
            // port: EA passes two arguments fn_800A746C ignores
            ((void (*)(s32, int, int, int, int))fn_800A746C)(1, 0, 2, 0, 0);
        } else {
            // port: EA passes two arguments fn_800A746C ignores
            ((void (*)(s32, int, int, int, int))fn_800A746C)(1, 0, 1, 0, 0);
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

    // A scoring shot longer than the longest so far is the new longest.
    if (nPoints > 0) {
        if (nLength > lbl_80282494) {
            if (lbl_80282494 != 0.0f) {
                if (nPlayer != lbl_80281768 && lbl_80281768 != 5) {
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
            lbl_80281768 = nPlayer;
            lbl_80282494 = nLength;
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

    pPlayer->nEBC += nPoints;
    nTotal = pPlayer->nEBC;
    pPlayer->nEBC = (nTotal <= 0) ? 0 : nTotal;     // never below zero
    fn_800E5CA4(nPlayer, gPlayers[nPlayer].nEBC, nLength, nKind, 0, 0, nPoints, 0.0f);

    // A track the first time the score reaches 1200, 800 and 400 (EA also passes the player's
    // side, 1 or -1, which fn_800A746C ignores).
    if (!lbl_802824A8[nPlayer] && gPlayers[nPlayer].nEBC >= 1200) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 2, 0, (nPlayer != 0) ? 1 : -1);
        lbl_802824A8[nPlayer] = 1;
    }
    if (!lbl_802824A0[nPlayer] && gPlayers[nPlayer].nEBC >= 800) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 3, 0, (nPlayer != 0) ? 1 : -1);
        lbl_802824A0[nPlayer] = 1;
    }
    if (!lbl_80282498[nPlayer] && gPlayers[nPlayer].nEBC >= 400) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 4, 0, (nPlayer != 0) ? 1 : -1);
        lbl_80282498[nPlayer] = 1;
    }

    // The shot took the lead.
    if (nLead < 0 && gPlayers[nPlayer].nEBC - gPlayers[nPlayer == 0].nEBC > 0) {
        ADD_MSG(0x18);
    }

    // The player's longest counted shot, and where the ball lay.
    if (bCounts && nLength > pPlayer->nEA8) {
        pPlayer->nEA8 = nLength;
        Vec_Copy(pPlayer->ball.vPos, pPlayer->vEAC);
        if (gPlayers[nPlayer].nEA8 > gPlayers[1 - nPlayer].nEA8) {
            fn_80062D38(0x4C, nLength, nPlayer);
            // port: EA passes two arguments fn_800A746C ignores
            ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 0, 0, 0);
        }
    }

    // The target score reached: the player wins, with one of four winning messages.
    if (pPlayer->nEBC >= lbl_80281760) {
        lbl_80281764 = nPlayer;
        nMsgs = 0;
        nPick = Misc_RandFunc(1) % 3;
        if (nPick == 0) {
            fn_8010D428(1, 0);
        } else if (nPick == 1) {
            fn_8010D428(2, 0);
        } else if (nPlayer == 0) {
            fn_8010D428(3, 0);
        } else {
            fn_8010D428(4, 0);
        }
    }
    if (nMsgs > 0 && lbl_80281764 == 5) {
        fn_8010D428(aMsgs[Misc_RandFunc(1) % nMsgs], 0);
    }
}

void fn_8010D230(void) {
    fn_8010D278();
}

void fn_8010D250(void) {
    fn_8010D278();
    lbl_80281764 = 5;
}

// Every player's mode values cleared (and message 0x42 sent for each).
void fn_8010D278(void) {
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
    fn_8010D32C();
}

void fn_8010D32C(void) {
}

void fn_8010D330(void) {
}

void fn_8010D334(s32 v) {
    lbl_80281760 = v;
}

// Whether somebody has won, and who (5 = nobody).
u8 fn_8010D33C(s32* pnWinner) {
    if (pnWinner != NULL) {
        *pnWinner = lbl_80281764;
    }
    return lbl_80281764 != 5;
}

u8 fn_8010D364(void) {
    return Game_GetMode() == 26;
}

// The countdown is running: somebody has won and it has not run out.
u8 fn_8010D390(void) {
    if (lbl_80281764 != 5 && lbl_8028176C > 0) {
        return 1;
    }
    return 0;
}

void fn_8010D3B8(void) {
    if (gSession.nSplitScreen) {
        lbl_80282491 = 1;
    }
}

// A track (fn_800A746C kind 1, track 0) when the player's ball is on surface 155 (GameAudio.c
// calls it when the ball hits a surface).
void fn_8010D3D8(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];

    if (pPlayer->ball.nSurface == 155) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(1, 0, 0, 0, 0);
    }
}

void fn_8010D428(s32 p0, s32 p1) {
    fn_800A7664(8, p0, p1);
}
