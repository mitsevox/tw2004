// GameUICommands.c (our name): the commands the menu UI can send while a round is on (session game
// types 4 to 8; uiProcessInterface.c's fn_8008F568 routes them here). fn_80085120 fills a table of
// 214 command functions and fn_800850E4 runs one: most answer a question about the round (a
// player's state, the score, the wind, Battle mode's clubs, the PGA Tour event) or act on it.

#include "game.h"
#include "game/frontend.h"
#include "game/modes/pgatoursim.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80086208(s32 p0, u8* p1);
extern u8 lbl_80281F18;
void fn_800A7350();
void fn_800E3E3C(void);
void fn_800E5240(int n);
void fn_8008633C(void);
void fn_80086378(s32 p0, u8* p1);
void fn_800E4238();
void fn_800E42F4();
void fn_80086384(u8* p);
void fn_800863A8(u8* p);
f32 GM_GetGolferDistanceToPin();
void fn_8008642C(u8* p0, u8* p1);
void fn_80086460(u8* p0, u8* p1);
void fn_800865E0(s32 p0, u8* p1);
void fn_80086610(s32 p0, u8* p1);
void fn_800866E8(void);
s32 fn_800FA4B8();
void fn_80086940(s32 p0, u8* p1);
void fn_80086970(u8* p0, u8* p1);
s32 fn_800D2B4C();
void fn_80086D24(u8* p0, u8* p1);
s32 fn_800F9254();
s32 fn_800F9308();
void fn_80086DFC(s32 p0, u8* p1);
void fn_80086E2C(s32 p0, u8* p1);
s32 fn_800E8CA8(s32, s32);
void fn_80086F0C(s32* arg0, s32* arg1);
s32 fn_80119808();
void fn_8008709C(u8* p0, u8* p1);
void fn_800870E4(u8* p0, u8* p1);
void fn_80087130(u8* p0, u8* p1);
void fn_8008719C(u8* p0, u8* p1);
void fn_800871E4(u8* p0, u8* p1);
void fn_80087238(s32 p0, u8* p1);
void fn_80087270(s32 p0, u8* p1);
s32 fn_800FD704();
void fn_800872F8(u8* p0, u8* p1);
void fn_800873D4(s32* arg0, s32* arg1);
void fn_800C9038(s32 nView, s32 a, s32 b);
void fn_80087420(u8* p0);
void fn_801002C0();
void fn_800874F0(void);
void fn_80087510(void* arg0);
void fn_80100B38();
void fn_80087574(u8* p0);
s32 fn_8008AB4C();
void fn_80087628(s32 p0, u8* p1);
s32 fn_800ECA08();
s32 fn_800ECA34();
s32 fn_800ECC14();
void fn_800876CC(s32 p0, u8* p1);
void fn_80087700(u8* p0, u8* p1);
void fn_80087734(s32 p0, u8* p1);
s32 fn_800E5DE4();
s32 fn_800E5E54();
void fn_80087764(void);
f32 fn_800E6578();
void fn_80087790(u8* p0, u8* p1);
void fn_8009CD10();
void fn_8009CD7C();
void fn_80087D14(void);
void fn_80087D34(void);
s32 fn_8009F7E8();
void fn_80087D54(u8* p0, u8* p1);
void fn_8007E9BC();
void fn_80087E60(void);
extern u8 lbl_801D880C[];
u8* fn_80087E80(u8* p0);
s32 fn_800A2100();
void fn_80087E9C(u8* p0, u8* p1);
s32 fn_8008AC00();
void fn_8008802C(s32 p0, u8* p1);
void fn_8008805C(s32* arg0, s32* arg1);
void fn_800ED974();
void fn_800880AC(void);
void fn_800880CC(void);
void fn_80101CFC();
void fn_80101D24();
void fn_800881A8(void);
void fn_800881C8(void);
s32 fn_800F1DF0();
s32 fn_800FDADC();
void fn_80088208(void);
void fn_8008823C(s32* arg0, s32* arg1);
s32 fn_800FDC5C();
void fn_8008828C(u8* p0, u8* p1);
void fn_800882C0(s32 p0, u8* p1);
void fn_800FDC0C();
void fn_800882F4(u8* p0);
s32 fn_800ED314();
void fn_80088324(s32 p0, u8* p1);
void fn_80088354(void);
void fn_80088358(void);
void fn_800A73C0();
s32 fn_800FD8D0();
void fn_800883FC(u8* p0);
void fn_80088428(u8* p0, u8* p1);
s32 fn_80085BC0();
void fn_8009CD80();
s32 fn_800FDE58();
void fn_800884F0(u8* p0, u8* p1);
void fn_80088538(u8* p0, u8* p1);
void fn_80088570(u8* p0);
void fn_800A6F38();
void fn_80088634(void);
void fn_80088654(s32 a, u8* p);
s32 fn_800F1E1C();
void fn_80088804(void);
void fn_80088830(void);
void fn_800907AC();
void fn_80088CC4(u8* p0);
void fn_800894B4(u8* p0, u8* p1);
void fn_80089584(s32 a, u8* p);
u8 fn_800DAD44();
s32 fn_800DAD4C();
void fn_80089590(s32* arg0, s32* arg1);
void fn_80089648(s32 p0, u8* p1);
void fn_8008967C(s32 a, u8* p);
void fn_80089688(void);
void fn_8008968C(u8* p0);
void fn_800896A8(s32 a, u8* p);
void fn_800896B4(u8* p0);
void fn_8008299C();
void fn_800896D0(void);
s32 fn_800E53A4(s32);
void fn_80089A50(s32* arg0);
void fn_80089AD0(void);
Session* fn_80089B78(void);
extern u8 lbl_80189900[];
s32 fn_800DADC0();
void fn_80089B8C(s32 p0, u8* p1);
void fn_80082DBC();
void fn_80082E10();
void fn_80089BD0(void);
void fn_80089BD4(s32 a, u8* p);
void fn_80089BE0(void);
void fn_80089C00(void);
void fn_8008AC4C(s32 p0, s32 p1);
void fn_80089C4C(s32* arg0);
void fn_800A6148();
void fn_80089CAC(void);
void fn_800834E8();
void fn_800E53AC();
void fn_800E53C0();
void fn_80089D04(void);
void fn_80089D28(void);
void fn_80089D48(void);
void fn_80089D68(u8* p0);
void fn_80089D98(s32 a, u8* p);
void fn_80089DA4(s32 a, u8* p);
void fn_80089E5C(void);
void fn_80089E60(void);
void fn_80089E98(void);
void fn_80089E9C(u8* p0, u8* p1);
s32 GameModeBattle_RemoveClub(s32, s32);
s32 GameModeBattle_AddClub(s32, s32);
void fn_80089F24(void* arg0);
void fn_8008A184(void);
char* fn_800ED280(int nId);
char* fn_800ED2C8(int nId);
void fn_8008A208(void);
s32 fn_800E8114();
void fn_8008A20C(u8* p0, u8* p1);
int GameModeBattle_CanAddClub(int nPlayer, int nClub);
char* GameModeDriverPGATour_GetName(s32 i);
s32 GameModeDriverPGATour_GetCurrentEventID(void);
s32 fn_800A7528();
void fn_8008A758(s32 p0, u8* p1);
f32 fn_80012C30(s32);
extern f32 lbl_80283B34;
void fn_8008A788(void** arg0, f32* arg1);
void fn_8008A7C8(s32 a, u8* p);
void fn_8008A800(void);
s32 fn_8003DCAC();
s32 GameModeBattle_NumRemovableClubsLeft();
void fn_8008A804(u8* p0, u8* p1);
void fn_8008A838(s32 p0, u8* p1);
void fn_8008A86C(void);
s32 GameModeBattle_GetWinner();
void fn_8008A8B8(s32 p0, u8* p1);
u8 fn_800F031C(s32);
void fn_8008A914(void** arg0, s32* arg1);
s32 fn_800E53E8(s32);
void fn_8008A964(s32* arg0);
void fn_8008AAAC(s32 a, u8* p);
void fn_8008AAB8(u8* p0, u8* p1);
void fn_8008AB04(u8* p0, u8* p1);
s32 fn_8008AB40(void);
void fn_8008AC3C(void);
s32 fn_8008AC40(void);
void fn_8008AC48(s32 p0, s32 p1);
void fn_8008AC78(s32 p0, s32 p1);
void fn_8008ACA4(s32 p0, s32 p1);
void fn_8008ACD0(s32 p0, s32 p1);
void fn_8008ACFC(s32 p0, s32 p1);
void fn_8008AD28(s32 p0, s32 p1);
void fn_8008AD54(s32 p0, s32 p1);

void fn_80086208(s32 p0, u8* p1) {
    s32 t0;
    t0 = Game_CurHoleIndex();
    *(s32*)p1 = t0;
}

void fn_8008633C(void) {
    s32 t0;
    t0 = fn_800E4BF8();
    lbl_80281F18 = (((u32)__cntlzw((t0 & 0xFF)) >> 5) & 0xFF);
    fn_800A7350(1);
    fn_800E3E3C();
}

void fn_80086378(s32 p0, u8* p1) {
    *(s32*)p1 = gNumPlayersSetUp;
}

void fn_80086384(u8* p) {
    fn_800E42F4(*(s32*)(p + 0x0));
}

void fn_800863A8(u8* p) {
    fn_800E4238(*(s32*)(p + 0x0));
}

// ---- end of sweep code ----

// The player's club.
void fn_800863CC(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nClub;
}

// The player's kind of shot.
void fn_800863EC(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nShotKind;
}

// The player's trajectory (low, normal, high).
void fn_8008640C(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nTrajectory;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8008642C(u8* p0, u8* p1) {
    f32 t0;
    t0 = GM_GetGolferDistanceToPin(*(s32*)p0);
    *(f32*)p1 = t0;
}

void fn_80086460(u8* p0, u8* p1) {
    f32 t0;
    t0 = AI_MaxDistance(*(s32*)p0, *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
    *(f32*)p1 = t0;
}

// ---- end of sweep code ----

// The ball's lie, or 99 when it is on surface 151.
void fn_800864A0(s32* pArgs, s32* pResult) {
    if (gPlayers[pArgs[0]].ball.nSurface == 151) {
        *pResult = 99;
        return;
    }
    *pResult = gPlayers[pArgs[0]].ball.nLie;
}

void fn_800864D8(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].ball.n6C;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800865E0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AB40();
    *(s32*)p1 = t0;
}

void fn_80086610(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800E27C0();
    *(s32*)p1 = t0;
}

// ---- end of sweep code ----

// The mode's points for a player on a hole.
void fn_80086640(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nModePoints[pArgs[1]];
}

// ---- sweep code (not yet cleaned up) ----

void fn_800866E8(void) {
}

// ---- end of sweep code ----

// Whether the round plays a hole.
void fn_800868E8(s32* pArgs, s32* pResult) {
    *pResult = gpGame->bHoleSelected[pArgs[0]];
}

// The holes a player has won (match play).
void fn_80086900(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nHolesWon;
}

void fn_80086920(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].n274;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80086940(s32 p0, u8* p1) {
    s32 t0;
    t0 = Game_GetMode();
    *(s32*)p1 = t0;
}

void fn_80086970(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FA4B8(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80086D24(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800D2B4C(*(s32*)p0);
    *(s32*)p1 = t0;
}

// ---- end of sweep code ----

// The tee set a player plays from.
void fn_80086DDC(s32* pArgs, s32* pResult) {
    *pResult = gSession.nTeeSet[pArgs[0]];
}

// ---- sweep code (not yet cleaned up) ----

void fn_80086DFC(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800F9254();
    *(s32*)p1 = t0;
}

void fn_80086E2C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800F9308();
    *(s32*)p1 = t0;
}

void fn_80086F0C(s32* arg0, s32* arg1) {
    s32 temp_r3;

    temp_r3 = Game_GetMode();
    switch (temp_r3) {
    case 18:
        *arg1 = fn_800E1788(*arg0);
        return;
    case 23:
        *arg1 = fn_8011937C(*arg0, 0, 0);
        return;
    case 19:
        *arg1 = fn_800E8CA8(*arg0, 1);
        return;
    default:
        *arg1 = fn_800E1904(*arg0, 1);
        return;
    }
}

void fn_80087038(MsgArg* pArgs) {
    s32 t0;
    s32 t1;
    char* t2;
    t0 = fn_801197CC(0, pArgs[0].i);
    t1 = fn_80119118(0, t0);
    t2 = fn_80118E30(0, t1);
    strcpy(((MsgString*)pArgs[1].p)->pStr, t2);
}

void fn_8008709C(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_801190D8(0, t0);
    *(s32*)p1 = t1;
}

void fn_800870E4(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_80119808(0, t0);
    *(s32*)p1 = (t1 & 0xFF);
}

void fn_80087130(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    s32 t2;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_8011908C(0, t0);
    t2 = fn_8011937C(0, t0, (((u32)__cntlzw((t1 & 0xFF)) >> 5) & 0xFF));
    *(s32*)p1 = t2;
}

void fn_8008719C(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_80119A04(0, t0);
    *(s32*)p1 = t1;
}

void fn_800871E4(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_80119638(0, t0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t1;
}

void fn_80087238(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_801190D8(0, 0);
    *(s32*)p1 = t0;
}

void fn_80087270(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80119808(0, 0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_800872F8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FD704(*(s32*)p0, *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
    *(s32*)p1 = t0;
}

void fn_800873D4(s32* arg0, s32* arg1) {
    if (Player_IsCPU(*arg0) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_80087420(u8* p0) {
    fn_800C9038(gPlayers[*(s32*)p0].nView[0], *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
}

void fn_800874F0(void) {
    fn_801002C0();
}

void fn_80087510(void* arg0) {
    gpGame->pfn25C(((s32*)arg0)[0], ((s32*)arg0)[1]);
}

// ---- end of sweep code ----

// Whether GameBreaker is off.
void fn_80087548(s32* pArgs, s32* pResult) {
    if (gSession.a8[0] != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80087574(u8* p0) {
    fn_800E5240(*(s32*)p0);
    fn_80100B38();
}

void fn_80087628(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AB4C();
    *(s32*)p1 = t0;
}

// ---- end of sweep code ----

// The same question as fn_80087548, under a second command.
void fn_80087658(s32* pArgs, s32* pResult) {
    if (gSession.a8[0] != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800876CC(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECA08();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80087700(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECA34(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80087734(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECC14();
    *(s32*)p1 = t0;
}

void fn_80087764(void) {
    if (fn_800E5E54() != 0xE) {
        fn_800E5DE4();
    }
}

void fn_80087790(u8* p0, u8* p1) {
    f32 t0;
    t0 = fn_800E6578(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(f32*)p1 = t0;
}

void fn_80087D14(void) {
    fn_8009CD10();
}

void fn_80087D34(void) {
    fn_8009CD7C();
}

void fn_80087D54(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009F7E8(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80087E60(void) {
    fn_8007E9BC();
}

u8* fn_80087E80(u8* p0) {
    *(s32*)(lbl_801D880C + 0x4) = *(s32*)p0;
    *(s32*)lbl_801D880C = 0;
    return lbl_801D880C;
}

void fn_80087E9C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A2100(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = t0;
}

void fn_8008802C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AC00();
    *(s32*)p1 = t0;
}

void fn_8008805C(s32* arg0, s32* arg1) {
    if ((s8)GOLFERSTATE_GetCurrentState(*arg0) == 3) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_800880AC(void) {
    fn_800ED974();
}

void fn_800880CC(void) {
}

void fn_800881A8(void) {
    fn_80101CFC();
}

void fn_800881C8(void) {
    fn_80101D24();
}

// ---- end of sweep code ----

// Who controls the player (CONTROLLER_CPU for the AI).
void fn_800881E8(s32* pArgs, s32* pResult) {
    *pResult = gPlayers[pArgs[0]].nController;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80088208(void) {
    if (fn_800E39F0() != 0) {
        fn_800F1DF0();
        return;
    }
    fn_800FDADC();
}

void fn_8008823C(s32* arg0, s32* arg1) {
    if ((s8)GOLFERSTATE_GetCurrentState(*arg0) == 4) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_8008828C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FDC5C(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_800882C0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800E1BBC();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_800882F4(u8* p0) {
    fn_800FDC0C(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
}

void fn_80088324(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ED314();
    *(s32*)p1 = t0;
}

void fn_80088354(void) {
}

void fn_80088358(void) {
}

void fn_800883FC(u8* p0) {
    fn_800A73C0((*(s32*)p0 & 0xFF), *(s32*)(p0 + 0x4));
}

void fn_80088428(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FD8D0(*(s32*)(((u8*)*(s32*)p0) + 0x8), *(s32*)(p0 + 0x4), *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + 0x8), *(s32*)(p0 + 0xC));
    *(s32*)p1 = t0;
}

void fn_800884F0(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FDE58(*(s32*)(((u8*)*(s32*)p0) + 0x8), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC));
    *(s32*)p1 = t0;
}

void fn_80088538(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80085BC0(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80088570(u8* p0) {
    fn_8009CD80(*(s32*)p0, *(s32*)(p0 + 0x4));
    fn_8009CD7C();
}

void fn_80088634(void) {
    fn_800A6F38();
}

void fn_80088654(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

// ---- end of sweep code ----

void fn_8008879C(s32* pArgs, s32* pResult) {
    if (gSession.uFlags & 0x4000) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

void fn_800887C4(s32* pArgs, s32* pResult) {
    if (gSession.nGameType == 6 && (gSession.nPaused == 2 || gSession.nPaused == 3)) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80088804(void) {
    if (fn_800E39F0() != 0) {
        fn_800F1E1C();
    }
}

void fn_80088830(void) {
}

void fn_80088834(MsgArg* pArgs, s32* pResult) {
    *pResult = strlen(((MsgString*)pArgs[0].p)->pStr);
}

void fn_80088CC4(u8* p0) {
    fn_800907AC(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

void fn_800894B4(u8* p0, u8* p1) {
    f32 t0;
    t0 = fn_800D0478(*(s32*)p0);
    *(f32*)p1 = t0;
}

void fn_80089584(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 7;
}

void fn_80089590(s32* arg0, s32* arg1) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        if (fn_800DAD44() != 0) {
            *arg1 = 1;
            return;
        }
        *arg1 = 0;
        return;
    case 1:
        *arg1 = fn_800DAD4C();
        return;
    }
}

void fn_80089648(s32 p0, u8* p1) {
    s32 t0;
    t0 = Rand_Next(1);
    *(s32*)p1 = t0;
}

void fn_8008967C(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80089688(void) {
}

void fn_8008968C(u8* p0) {
    *(s32*)((u8*)*(s32*)p0) = 2;
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = 1;
}

void fn_800896A8(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 2;
}

void fn_800896B4(u8* p0) {
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = *(s32*)p0;
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = 0;
}

void fn_800896D0(void) {
    fn_8008299C();
}

void fn_80089A50(s32* arg0) {
    if ((s32) *arg0 != 0) {
        fn_800E53A4(1);
        return;
    }
    fn_800E53A4(0);
}

void fn_80089AD0(void) {
}

Session* fn_80089B78(void) {
    gSession.nC = 2;
    return &gSession;
}

void fn_80089B8C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800DADC0();
    *(s32*)p1 = t0;
}

void fn_80089BD0(void) {
}

void fn_80089BD4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80089BE0(void) {
    fn_80082DBC();
}

void fn_80089C00(void) {
    fn_80082E10();
}

void fn_80089C4C(s32* arg0) {
    s32 temp_r31;

    temp_r31 = *arg0;
    fn_800A6F38();
    fn_8008AC4C((u16) temp_r31, 0);
}

// ---- end of sweep code ----

void fn_80089C84(s32* pArgs, s32* pResult) {
    if (gpGame->bD4 != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80089CAC(void) {
    fn_800A6148();
}

// ---- end of sweep code ----

// Whether the player missed the cut.
void fn_80089CCC(s32* pArgs, s32* pResult) {
    if (gPlayers[pArgs[0]].bPlayerCut != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80089D04(void) {
    fn_800E4204();
    fn_800E53AC();
}

void fn_80089D28(void) {
    fn_800E53C0();
}

void fn_80089D48(void) {
    fn_800834E8();
}

void fn_80089D68(u8* p0) {
    fn_8008AC48(lbl_80282278, *(s32*)(((u8*)*(s32*)p0) + 0x8));
}

void fn_80089D98(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80089DA4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80089E5C(void) {
}

void fn_80089E60(void) {
}

// ---- end of sweep code ----

void fn_80089E64(s32* pArgs, s32* pResult) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80089E98(void) {
}

void fn_80089E9C(u8* p0, u8* p1) {
    s32 t0;
    t0 = Bag_CountClubs(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80089ED0(void* arg0, s32* arg1) {
    if (Bag_HasClub(((s32*)arg0)[0], ((s32*)arg0)[1]) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_80089F24(void* arg0) {
    if ((s32) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        GameModeBattle_AddClub((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
        return;
    }
    GameModeBattle_RemoveClub((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
}

void fn_8008A184(void) {
}

void fn_8008A188(MsgArg* pArgs) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED280(pArgs[1].i));
}

void fn_8008A1C8(MsgArg* pArgs) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED2C8(pArgs[1].i));
}

void fn_8008A208(void) {
}

void fn_8008A20C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E8114(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_8008A240(void* arg0, s32* arg1) {
    // the caller tests only the low byte of the result
    if ((u8)GameModeBattle_CanAddClub(((s32*)arg0)[0], ((s32*)arg0)[1]) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_8008A468(MsgArg* pArgs) {
    strcpy(((MsgString*)pArgs[0].p)->pStr,
           GameModeDriverPGATour_GetName(GameModeDriverPGATour_GetCurrentEventID()));
}

void fn_8008A758(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800A7528();
    *(s32*)p1 = t0;
}

void fn_8008A788(void** arg0, f32* arg1) {
    *arg1 = lbl_80283B34 * fn_80012C30((*(s32*)((u8*)(*arg0) + 8)));
}

void fn_8008A7C8(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

// ---- end of sweep code ----

void fn_8008A7D4(s32* pArgs, s32* pResult) {
    if (gSession.options.a24[1] != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8008A800(void) {
}

void fn_8008A804(u8* p0, u8* p1) {
    s32 t0;
    t0 = GameModeBattle_NumRemovableClubsLeft(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_8008A838(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8003DCAC();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_8008A86C(void) {
}

void fn_8008A8B8(s32 p0, u8* p1) {
    s32 t0;
    t0 = GameModeBattle_GetWinner();
    *(s32*)p1 = t0;
}

// ---- end of sweep code ----

// Whether the game is paused.
void fn_8008A8E8(s32* pArgs, s32* pResult) {
    if (gSession.nPaused != 0) {
        *pResult = 1;
        return;
    }
    *pResult = 0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8008A914(void** arg0, s32* arg1) {
    if (fn_800F031C((*(s32*)((u8*)(*arg0) + 8))) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_8008A964(s32* arg0) {
    if ((s32) *arg0 == 0) {
        fn_800E53E8(0);
        return;
    }
    fn_800E53E8(1);
}

void fn_8008AAAC(s32 a, u8* p) {
    *(s32*)(p + 0x0) = -1;
}

void fn_8008AAB8(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_801197A4(0, t0);
    *(s32*)p1 = (t1 & 0xFF);
}

void fn_8008AB04(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_801197A4(*(s32*)p0, 0);
    *(s32*)p1 = (t0 & 0xFF);
}

s32 fn_8008AB40(void) {
    return gpGame->n4;
}

void fn_8008AC3C(void) {
}

s32 fn_8008AC40(void) {
    return 0;
}

void fn_8008AC48(s32 p0, s32 p1) {
}

void fn_8008AC4C(s32 p0, s32 p1) {
    fn_800A7664(14, p0, p1);
}

void fn_8008AC78(s32 p0, s32 p1) {
    fn_800A7664(19, p0, p1);
}

void fn_8008ACA4(s32 p0, s32 p1) {
    fn_800A7664(17, p0, p1);
}

void fn_8008ACD0(s32 p0, s32 p1) {
    fn_800A7664(16, p0, p1);
}

void fn_8008ACFC(s32 p0, s32 p1) {
    fn_800A7664(15, p0, p1);
}

void fn_8008AD28(s32 p0, s32 p1) {
    fn_800A7664(20, p0, p1);
}

void fn_8008AD54(s32 p0, s32 p1) {
    fn_800A7664(18, p0, p1);
}

// ---- end of sweep code ----
