// GameMode22.c (our name): game mode 22 (GameRound.c starts it with fn_80125E68), and before it the
// trophy case's text (Rookie of the Year, Player of the Year, the money and scoring leaders; "Earned
// on %s"). One file: both halves share its .data, .sdata and .sbss blocks. Partly decompiled; the
// trophy case's handlers and the mode's small hooks are in C.

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

void fn_801260B8(void);
void fn_801260BC(void);
void fn_80126130(void);
void fn_80126150(void);
void fn_80126184(void);
void fn_801262C4(int nPlayer);
s32 fn_8012632C(void);
u8   fn_801263C4(int nPlayer, int n);
void fn_80126698(int nPlayer);
u8   fn_80126FB0(s32* pn8);
u8   fn_80126418(int n);
void fn_8012643C(int nPlayer);
void fn_8012645C(int nPlayer);
void fn_801264B8(void);
s32  fn_80126640(int n);
s32  fn_80126334(int nPlayer);
void fn_801260C0(void);
void fn_80126EC0(void);
void fn_80126E68(void);
void fn_80126E88(void);
void fn_80126F7C(void);
void fn_80126F80(void);
void fn_80126F84(s32 p0);
void fn_80126F94(s32 n);
s32 fn_80126FA0(void);
s32 fn_80126FD8(void);
u8   fn_80127004(void);
void fn_80127034(int nPlayer);
s32 fn_80127098(s32 arg0);

// Message handler (FE_MessageTable.c): one of the four trophies: whether it is won, its title,
// and the day it was won (empty while not).
void fn_8012597C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nTrophy = pArgs[0].i;
    char* szName = ((MsgString*)pArgs[1].p)->pStr;
    char* szDate = ((MsgString*)pArgs[2].p)->pStr;

    pResult->i = fn_80077ACC()->a1C0[nTrophy + 12].bWon;
    strcpy(szName, lbl_8019543C[nTrophy]);
    if (pResult->i) {
        fn_800D28DC(fn_80077ACC()->a1C0[nTrophy + 12].nDate, szDate);
        return;
    }
    szDate[0] = '\0';
}

// Message handler (FE_MessageTable.c): a trophy's text, by column: 0 its title, 1 and 2
// placeholders.
void fn_80125A24(MsgArg* pArgs, MsgArg* pResult) {
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

// Message handler (FE_MessageTable.c): how many real-time events of a month (0-based) have their
// profile flag set.
void fn_80125AA4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;
    s32 nMonth;
    s32 i;
    s32 nCount;
    u16 nDate;
    s32 nEventMonth;
    s32 nDay;

    nCount = 0;
    nMonth = pArgs[0].i + 1;
    pProfile = fn_80077ACC();
    for (i = 0; i < 118; i++) {
        nDate = fn_800F0FBC(i);
        fn_800D2714(&nDate, &nEventMonth, &nDay, &nDay);
        if (nEventMonth == nMonth && pProfile->a104D0[i]) {
            nCount++;
        }
    }
    pResult->i = nCount;
}

// Message handler (FE_MessageTable.c): placeholder texts for a trophy's name and date.
void fn_80125B38(MsgArg* pArgs, MsgArg* pResult) {
    s32 nA = pArgs[0].i;
    s32 nB = pArgs[1].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;

    sprintf(((MsgString*)pArgs[2].p)->pStr, "trophy name %d %d", nA, nB);
    sprintf(szDate, "date %d %d", nA, nB);
}

// Message handler (FE_MessageTable.c): the value mod 4.
void fn_80125BB8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i % 4;
}

// Message handler (FE_MessageTable.c): the day the medal of challenge group n (1-based) was
// earned, as text; empty for group 0 or no medal.
void fn_80125BD8(MsgArg* pArgs, MsgArg* pResult) {
    s32 nGroup = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    SaveProfile* pProfile = fn_80077ACC();

    if (nGroup == 0) {
        szOut[0] = '\0';
        return;
    }
    if (pProfile->aMedal[nGroup - 1] != 3) {
        fn_800D28DC(pProfile->aMedalDate[nGroup - 1], szOut);
        return;
    }
    szOut[0] = '\0';
}

// Message handler (FE_MessageTable.c): a ladder event's course name, and the day it was won
// (empty while not).
void fn_80125C5C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    char* szCourse = ((MsgString*)pArgs[1].p)->pStr;
    char* szDate = ((MsgString*)pArgs[2].p)->pStr;
    SaveProfile* pProfile = fn_80077ACC();

    strcpy(szCourse, lbl_80191990[fn_80102104(nEvent)]);
    if (pProfile->aLadderAward[nEvent].bWon) {
        fn_800D28DC(fn_80077ACC()->aLadderAward[nEvent].nDate, szDate);
        return;
    }
    szDate[0] = '\0';
}

// Message handler (FE_MessageTable.c): a ladder event's course, and whether it is won.
void fn_80125D08(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    SaveProfile* pProfile = fn_80077ACC();

    *(s32*)pArgs[1].p = fn_80102104(nEvent);
    pResult->i = pProfile->aLadderAward[nEvent].bWon;
}

// Message handler (FE_MessageTable.c): fn_800F1154 of a won real-time event, else -1.
void fn_80125D78(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 nEvent = pArgs[0].i;

    if (pProfile->aRTEAward[nEvent].bWon) {
        pResult->i = fn_800F1154(nEvent);
        return;
    }
    pResult->i = -1;
}

// Message handler (FE_MessageTable.c): "Earned on <day>" for a won award, else empty.
void fn_80125DE0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nAward = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    char szDate[12];    // the size is not known (the frame leaves room for 12 bytes)

    if (fn_80077ACC()->aAward[nAward].bWon == 1) {
        fn_800D28DC(fn_80077ACC()->aAward[nAward].nDate, szDate);
        sprintf(szOut, "Earned on %s", szDate);
        return;
    }
    szOut[0] = '\0';
}

// The mode's setup: its callbacks and options, split screen as chosen, the per-player values
// cleared, and its messages to show.
void fn_80125E68(void) {
    s32 i;

    gpGame->pfnInit = fn_80125E68;
    gpGame->pfnShutdown = fn_801260B8;
    gpGame->pfn1F0 = fn_801260BC;
    gpGame->pfnSetupNextGolfer = fn_80126130;
    gpGame->pfnGetHonors = fn_80126334;
    gpGame->pfn250 = fn_8012643C;
    gpGame->pfnHoleFinished = (u8 (*)(int, u8))fn_801263C4;
    gpGame->pfnGameFinished = (u8 (*)(u8))fn_80126418;
    gpGame->pfnGoToPlayoff = (u8 (*)(u8))fn_8012632C;  // port: EA passes an argument fn_8012632C ignores
    gpGame->pfnEndGolferTurn = fn_8012645C;
    gpGame->pfnEndGame = fn_80126150;
    gpGame->pfn220 = fn_80126184;
    gpGame->pfn20C = fn_801262C4;
    gpGame->pfn244 = fn_80126698;
    gpGame->pfn1E4 = fn_80126E68;
    gpGame->pfn224 = fn_80126E88;
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

void fn_801260B8(void) {
}

void fn_801260BC(void) {
}

// Every player on the first tee set, the option n20 off and the mode's state reset.
void fn_801260C0(void) {
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

void fn_80126130(void) {
    GameModeStroke_SetupNextGolfer();
}

// Pays the player in fn_80126FB0's n8 5000.
void fn_80126150(void) {
    s32 nPlayer;

    fn_80126FB0(&nPlayer);
    fn_800D3548(nPlayer, 5000, NULL);
}

// The mode's frame: its message once when asked, every 16 frames the current shot's length sent
// as message 0x4D (a track plays while it is a new nonzero length), and the n18 countdown.
void fn_80126184(void) {
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
        fn_80062D38(0x4D, 0, nLength);
        if (nLength != 0 && nLength != lbl_80282584) {
            if (lbl_80282588 == 0) {
                // port: EA passes two arguments fn_800A746C ignores
                ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 1, 0, 1, 0);
                lbl_80282588 = 1;
            }
        } else if (lbl_80282588 != 0) {
            // port: EA passes an argument fn_800A74E4 ignores
            ((void (*)(s32, int, int))fn_800A74E4)(0, 1, 0);
            lbl_80282588 = 0;
        }
        lbl_80282584 = nLength;
        lbl_802819A0 = 15;
    }
    if (lbl_80195498.n8 != 5) {
        lbl_80195498.n18--;
    }
}

void fn_801262C4(int nPlayer) {
    fn_80126F80();
    fn_80127034(nPlayer);
    fn_800E5CA4(0, gPlayers[nPlayer].nEBC, 0, 0, 0, 0, 0, 0.0f);
}

s32 fn_8012632C(void) {
    return 0;
}

// The next player after the current one other than nPlayer (5: none); 0 while no player has an
// nEA0.
s32 fn_80126334(int nPlayer) {
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

u8 fn_801263C4(int nPlayer, int n) {
    s32 bRet = 0;

    if (fn_80126418(n) && lbl_80195498.n18 < 0) {
        bRet = 1;
    }
    return bRet;
}

// n is not read (fn_801263C4 passes one).
u8 fn_80126418(int n) {
    return fn_80126FB0(NULL);
}

// The ball went out of bounds: scored as a shot.
void fn_8012643C(int nPlayer) {
    fn_80126698(nPlayer);
}

// The ball back on the player's tee.
void fn_8012645C(int nPlayer) {
    fn_80055AA8(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

// Once every player's nEA0 has reached n4 and they are all level, the player with the highest nEBC
// wins (n8 = the player, bC set). A tie for the highest leaves no winner, and with n0 1 clears
// every nEBC.
void fn_801264B8(void) {
    s32 nWinner = 5;
    s32 nBest = -0x7FFFFFFF - 1;
    u8 bTie = 0;
    s32 i;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (PLAYER(i)->nEA0 < lbl_80195498.n4 || gPlayers[0].nEA0 != PLAYER(i)->nEA0) {
            return;
        }
        if (PLAYER(i)->nEBC >= nBest) {
            bTie = 0;
            if (nBest == PLAYER(i)->nEBC && nWinner != 5) {
                bTie = 1;
            }
            nWinner = i;
            nBest = PLAYER(i)->nEBC;
        }
    }
    lbl_80195498.n8 = nWinner;
    lbl_80195498.bC = 1;
    if (bTie) {
        lbl_80195498.n8 = 5;
        lbl_80195498.bC = 0;
        if (lbl_80195498.n0 == 1) {
            for (i = 0; i < gSession.nNumPlayers; i++) {
                PLAYER(i)->nEBC = 0;
            }
        }
    }
}

s32 fn_80126640(int n) {
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

// A shot is over (gpGame->pfn244): the player's points for it from where the ball ended up (and
// how far it went), the longest shot kept, the score's tracks, the winner's message, or else a
// comment picked at random from the ones the shot earned.
void fn_80126698(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    u8 bCounts = 0;
    s32 nMsgs;
    s32 nKind;
    s32 nLength;
    s32 nPoints;
    u16 aMsgs[20];

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
        switch (fn_80126640(pPlayer->ball.nLie)) {
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

    // A scoring shot longer than the longest so far (f10, by n14) is the new longest.
    if (nPoints > 0) {
        if (nLength > lbl_80195498.f10) {
            if (lbl_80195498.f10 != 0.0f) {
                if (nPlayer != lbl_80195498.n14 && lbl_80195498.n14 != 5) {
                    fn_80062D38(0x4C, nLength, nPlayer);
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
    if (pPlayer->nEBC < 0) {
        pPlayer->nEBC = 0;
    }
    fn_800E5CA4(0, gPlayers[nPlayer].nEBC, nLength, nKind, 0, 0, nPoints, 0.0f);

    // A track the first time the score reaches 1200, 800 and 400.
    if (!lbl_8028259C[nPlayer] && gPlayers[nPlayer].nEBC >= 1200) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 2, 0, 0);
        lbl_8028259C[nPlayer] = 1;
    }
    if (!lbl_80282594[nPlayer] && gPlayers[nPlayer].nEBC >= 800) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 3, 0, 0);
        lbl_80282594[nPlayer] = 1;
    }
    if (!lbl_8028258C[nPlayer] && gPlayers[nPlayer].nEBC >= 400) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(0, 0, 4, 0, 0);
        lbl_8028258C[nPlayer] = 1;
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

    fn_801264B8();
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
        fn_8010D428(aMsgs[Rand_Next(1) % nMsgs], 0);
    }
}

void fn_80126E68(void) {
    fn_80126EC0();
}

void fn_80126E88(void) {
    fn_80126EC0();
    lbl_80195498.n8 = 5;
    lbl_80195498.bC = 0;
}

// Every player's mode values cleared (and message 0x42 sent for each); no current player.
void fn_80126EC0(void) {
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
