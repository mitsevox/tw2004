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
void fn_80126698();
u8   fn_80126FB0(s32* pn8);
u8   fn_80126418(int n);
void fn_8012643C(void);
void fn_8012645C(int nPlayer);
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

void fn_801260B8(void) {
}

void fn_801260BC(void) {
}

// Every player on the first tee set, the option n20 off and the mode's state reset.
void fn_801260C0(void) {
    s32 i;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        gSession.nTeeSet[i] = 0;
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
        if (gPlayers[i].nEA0 != 0) {
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

void fn_8012643C(void) {
    fn_80126698();
}

// The ball back on the player's tee.
void fn_8012645C(int nPlayer) {
    fn_80055AA8(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
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
        gPlayers[i].nEA0 = 0;
        gPlayers[i].nEA4 = 0;
        gPlayers[i].nEA8 = 0;
        gPlayers[i].nEBC = 0;
        gPlayers[i].nEC0 = 0;
        gPlayers[i].nEC4 = 0;
        gPlayers[i].nEC8 = 0;
        gPlayers[i].nECC = 0;
        gPlayers[i].nED0 = 0;
        gPlayers[i].nED4 = 0;
        gPlayers[i].nED8 = 0;
        gPlayers[i].nEDC = 0;
        fn_800E5CA4(i, gPlayers[i].nEBC, 0, 0, 0, 0, 0, 0.0f);
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
