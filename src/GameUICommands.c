// GameUICommands.c (our name): the commands the menu UI can send while a round is on (session game
// types 4 to 8; uiProcessInterface.c's fn_8008F568 routes them here). fn_80085120 fills a table of
// 214 command functions and fn_800850E4 runs one: most answer a question about the round (a
// player's state, the score, the wind, Battle mode's clubs, the PGA Tour event) or act on it.

#include "game.h"
#include "game/frontend.h"
#include "game/earnings.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/rte.h"
#include "core/memcard.h"
#include "core/startup.h"
#include "frontend/fe.h"

u8  lbl_80281ED8;               // which player the alternating question answers next (mode 26)
s32 lbl_80281EDC;               // counts -1..10, one step per fn_800894E8

// The round's commands, in address order (the table's order is fn_80085120's).
void fn_80085C78(MsgArg* pArgs, MsgArg* pResult);
void fn_80085D04(MsgArg* pArgs, MsgArg* pResult);
void fn_80085E20(MsgArg* pArgs, MsgArg* pResult);
void fn_80085F5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80085FDC(MsgArg* pArgs, MsgArg* pResult);
void fn_800860C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008618C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086208(MsgArg* pArgs, MsgArg* pResult);
void fn_80086238(MsgArg* pArgs, MsgArg* pResult);
void fn_8008628C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086300(MsgArg* pArgs, MsgArg* pResult);
void fn_8008633C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086378(MsgArg* pArgs, MsgArg* pResult);
void fn_80086384(MsgArg* pArgs, MsgArg* pResult);
void fn_800863A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800863CC(MsgArg* pArgs, MsgArg* pResult);
void fn_800863EC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008640C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008642C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086460(MsgArg* pArgs, MsgArg* pResult);
void fn_800864A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800864D8(MsgArg* pArgs, MsgArg* pResult);
void fn_800864F8(MsgArg* pArgs, MsgArg* pResult);
void fn_800865A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800865E0(MsgArg* pArgs, MsgArg* pResult);
void fn_80086610(MsgArg* pArgs, MsgArg* pResult);
void fn_80086640(MsgArg* pArgs, MsgArg* pResult);
void fn_8008666C(MsgArg* pArgs, MsgArg* pResult);
void fn_800866E8(MsgArg* pArgs, MsgArg* pResult);
void fn_800866EC(MsgArg* pArgs, MsgArg* pResult);
void fn_80086738(MsgArg* pArgs, MsgArg* pResult);
void fn_8008685C(MsgArg* pArgs, MsgArg* pResult);
void fn_800868E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80086900(MsgArg* pArgs, MsgArg* pResult);
void fn_80086920(MsgArg* pArgs, MsgArg* pResult);
void fn_80086940(MsgArg* pArgs, MsgArg* pResult);
void fn_80086970(MsgArg* pArgs, MsgArg* pResult);
void fn_800869A4(MsgArg* pArgs, MsgArg* pResult);
void fn_80086C78(MsgArg* pArgs, MsgArg* pResult);
void fn_80086D24(MsgArg* pArgs, MsgArg* pResult);
void fn_80086D58(MsgArg* pArgs, MsgArg* pResult);
void fn_80086DDC(MsgArg* pArgs, MsgArg* pResult);
void fn_80086DFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80086E2C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086E5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086F0C(MsgArg* pArgs, MsgArg* pResult);
void fn_80086FB4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087038(MsgArg* pArgs, MsgArg* pResult);
void fn_8008709C(MsgArg* pArgs, MsgArg* pResult);
void fn_800870E4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087130(MsgArg* pArgs, MsgArg* pResult);
void fn_8008719C(MsgArg* pArgs, MsgArg* pResult);
void fn_800871E4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087238(MsgArg* pArgs, MsgArg* pResult);
void fn_80087270(MsgArg* pArgs, MsgArg* pResult);
void fn_800872AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800872F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80087338(MsgArg* pArgs, MsgArg* pResult);
void fn_800873D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087420(MsgArg* pArgs, MsgArg* pResult);
void fn_80087460(MsgArg* pArgs, MsgArg* pResult);
void fn_800874C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800874F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80087510(MsgArg* pArgs, MsgArg* pResult);
void fn_80087548(MsgArg* pArgs, MsgArg* pResult);
void fn_80087574(MsgArg* pArgs, MsgArg* pResult);
void fn_8008759C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087628(MsgArg* pArgs, MsgArg* pResult);
void fn_80087658(MsgArg* pArgs, MsgArg* pResult);
void fn_80087684(MsgArg* pArgs, MsgArg* pResult);
void fn_800876CC(MsgArg* pArgs, MsgArg* pResult);
void fn_80087700(MsgArg* pArgs, MsgArg* pResult);
void fn_80087734(MsgArg* pArgs, MsgArg* pResult);
void fn_80087764(MsgArg* pArgs, MsgArg* pResult);
void fn_80087790(MsgArg* pArgs, MsgArg* pResult);
void fn_800877CC(MsgArg* pArgs, MsgArg* pResult);
void fn_800879B4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087BE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80087C1C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087C7C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087CBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80087D14(MsgArg* pArgs, MsgArg* pResult);
void fn_80087D34(MsgArg* pArgs, MsgArg* pResult);
void fn_80087D54(MsgArg* pArgs, MsgArg* pResult);
void fn_80087D8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087DD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80087E1C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087E60(MsgArg* pArgs, MsgArg* pResult);
void fn_80087E80(MsgArg* pArgs, MsgArg* pResult);
void fn_80087E9C(MsgArg* pArgs, MsgArg* pResult);
void fn_80087ED8(MsgArg* pArgs, MsgArg* pResult);
void fn_80087F48(MsgArg* pArgs, MsgArg* pResult);
void fn_80087FAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008802C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008805C(MsgArg* pArgs, MsgArg* pResult);
void fn_800880AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800880CC(MsgArg* pArgs, MsgArg* pResult);
void fn_800880D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80088160(MsgArg* pArgs, MsgArg* pResult);
void fn_800881A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800881C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800881E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80088208(MsgArg* pArgs, MsgArg* pResult);
void fn_8008823C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008828C(MsgArg* pArgs, MsgArg* pResult);
void fn_800882C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800882F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088324(MsgArg* pArgs, MsgArg* pResult);
void fn_80088354(MsgArg* pArgs, MsgArg* pResult);
void fn_80088358(MsgArg* pArgs, MsgArg* pResult);
void fn_8008835C(MsgArg* pArgs, MsgArg* pResult);
void fn_800883FC(MsgArg* pArgs, MsgArg* pResult);
void fn_80088428(MsgArg* pArgs, MsgArg* pResult);
void fn_80088474(MsgArg* pArgs, MsgArg* pResult);
void fn_800884F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80088538(MsgArg* pArgs, MsgArg* pResult);
void fn_80088570(MsgArg* pArgs, MsgArg* pResult);
void fn_800885A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800885F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80088634(MsgArg* pArgs, MsgArg* pResult);
void fn_80088654(MsgArg* pArgs, MsgArg* pResult);
void fn_80088660(MsgArg* pArgs, MsgArg* pResult);
void fn_80088730(MsgArg* pArgs, MsgArg* pResult);
void fn_8008879C(MsgArg* pArgs, MsgArg* pResult);
void fn_800887C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088804(MsgArg* pArgs, MsgArg* pResult);
void fn_80088830(MsgArg* pArgs, MsgArg* pResult);
void fn_80088834(MsgArg* pArgs, MsgArg* pResult);
void fn_8008886C(MsgArg* pArgs, MsgArg* pResult);
void fn_80088AD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088CC4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088CF0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089324(MsgArg* pArgs, MsgArg* pResult);
void fn_80089414(MsgArg* pArgs, MsgArg* pResult);
void fn_800894B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800894E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80089584(MsgArg* pArgs, MsgArg* pResult);
void fn_80089590(MsgArg* pArgs, MsgArg* pResult);
void fn_80089600(MsgArg* pArgs, MsgArg* pResult);
void fn_80089648(MsgArg* pArgs, MsgArg* pResult);
void fn_8008967C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089688(MsgArg* pArgs, MsgArg* pResult);
void fn_8008968C(MsgArg* pArgs, MsgArg* pResult);
void fn_800896A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800896B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800896D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800896F0(MsgArg* pArgs, MsgArg* pResult);
void fn_800897F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089A50(MsgArg* pArgs, MsgArg* pResult);
void fn_80089A8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089AD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089AD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089B78(MsgArg* pArgs, MsgArg* pResult);
void fn_80089B8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BE0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C00(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C20(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C84(MsgArg* pArgs, MsgArg* pResult);
void fn_80089CAC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089CCC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D04(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D28(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D48(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D68(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D98(MsgArg* pArgs, MsgArg* pResult);
void fn_80089DA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089DB0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E60(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E64(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E98(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E9C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089ED0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089F24(MsgArg* pArgs, MsgArg* pResult);
void fn_80089F6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A010(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A0CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A128(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A184(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A188(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A1C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A208(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A20C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A240(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A294(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A2E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A468(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A4A8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A690(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A758(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A788(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A7C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A7D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A800(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A804(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A838(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A86C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A870(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A8B8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A8E8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A914(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A964(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A9A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A9E8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AAAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AAB8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AB04(MsgArg* pArgs, MsgArg* pResult);

// This file's helpers.
u8   fn_80085BC0(int nController);
void fn_8008AC3C(int a, int b);
u8   fn_8008AC40(void);
void fn_8008AC48(int nPlayer, char* sz);
void fn_8008AC4C(int nMsg, int a);
void fn_8008AC78(int nMsg, int a);
void fn_8008ACA4(int nMsg, int a);
void fn_8008ACD0(int nMsg, int a);
void fn_8008ACFC(int nMsg, int a);
void fn_8008AD28(int nMsg, int a);
void fn_8008AD54(int nMsg, int a);

// Other files' functions no header declares yet.
f32   GM_GetGolferDistanceToPin(int nPlayer);                 // GameManager.c
void  GM_GolferConcede_Hole(int nPlayer);                     // GameManager.c
void  GM_RestartHole(void);                                   // GameManager.c
void  GameModeBattle_AddClub(int nPlayer, int nClub);         // GameModeBattle.c
int   GameModeBattle_CanAddClub(int nPlayer, int nClub);
s32   GameModeBattle_GetWinner(void);
u8    GameModeBattle_IsClubStealingFinished(void);
int   GameModeBattle_NumRemovableClubsLeft(int nPlayer);
u8    GameModeBattle_RemoveClub(int nPlayer, int nClub);
void  fn_8001A870(void);
void  fn_80062B84(int a);
void  fn_8006F4E0(void);
s32   fn_80084FB4(CardPos* pPos);
void  fn_8009CD80(s32 nPort, s32 nSlot);
s32   fn_800A036C(MCCardPos* pPos);
s32   fn_800A0610(s32 nPort, s32 nSlot, s32 n);
void  fn_800A6148(void);
void  fn_800A61C4(int n);
void  fn_800A6F38(void);
void  fn_800A7350(int a);
void  fn_800A73C0(u8 a, int b);
s32   fn_800A7528(void);
void  fn_800A7944(void);
u8    fn_800C6E44(View* pView);
u8    fn_800C708C(View* pView);
void  fn_800C9038(int nView, f32* pLong, f32* pSide);    // GoBreakLine.c
s32   fn_800D2B4C(int nPlayer);
s32   fn_800D2D40(int nTeeSet);
s32   fn_800D2DA0(int nTeeSet);
s32   fn_800D2E00(int nTeeSet);
s32   fn_800D2E60(void);
s32   fn_800D2EB0(void);
f32   fn_800D6EEC(void);
char* fn_800DAD1C(int nPlayer);
s32   fn_800DAD30(int nPlayer);
u8    fn_800DAD44(void);
s32   fn_800DAD4C(void);
s32   fn_800DADC0(void);
int   fn_800E16F4(void);
int   fn_800E19A4(int nPlayer, int nHoles);
void  fn_800E3E3C(void);
void  fn_800E3ECC(void);
void  fn_800E4238(int i);
void  fn_800E42F4(int i);
void  fn_800E508C(void);
void  fn_800E50FC(void);
void  fn_800E53A4(u8 b);
void  fn_800E53AC(void);
void  fn_800E53C0(void);
void  fn_800E53E8(u8 b);
void  fn_800E5DE4(int n);
int   fn_800E5E54(void);
f32   fn_800E6578(int nPlayer, u32 nStat);
s32   fn_800E8114(int nPlayer);
int   fn_800E8CA8(int nPlayer, u8 bCurrent);
u8    fn_800ECA08(void);
s32   fn_800ECA34(int k);
int   fn_800ECC14(void);
char* fn_800ED280(int nId);
char* fn_800ED2C8(int nId);
int   fn_800ED314(void);
void  fn_800ED554(void);
void  fn_800ED974(void);
Pga80205F30* fn_800EE8B8(void);
s32   fn_800F031C(char* pDst);
s32   fn_800F0E20(s32* pRound);
int   fn_800F1960(void);
void  fn_800F1DF0(void);
void  fn_800F1E1C(void);
s32   fn_800F2408(s32 nPlayer);
s32   fn_800F2494(s32 nPlayer);
s32   fn_800F24D8(s32 nPlayer);
s32   fn_800F2534(s32 nPlayer);
s32   fn_800F3490(int nPlayer);
s32   fn_800F34F0(int nPlayer);
s32   fn_800F363C(int i);
s32   fn_800F3654(s32 p0);
s32   fn_800F3668(int n);
s8    fn_800F4878(void);
s32   fn_800F4B00(void);
void  fn_800F7DA4(void);
s32   fn_800F9308(void);
s32   fn_800FA4B8(int nPlayer);
s32   fn_800FD704(int nPlayer, int nHole, s32* pWon);
s32   fn_800FD8D0(char* szName1, s32* pPoints1, char* szName2, s32* pPoints2);
void  fn_800FDADC(void);
void  fn_800FDC0C(s32* p0, s32* p1, s32* p2);
s32   fn_800FDC5C(s32* pMoney);
s32   fn_800FDE58(char* szName, s32* pSeconds, s32* pStrokes, s32* pScore);
s32   fn_800FF49C(int nPlayer);
s32   fn_800FF4B4(int nPlayer);
s32   fn_800FF4CC(int nPlayer);
s32   fn_800FF514(int nPlayer);
s32   fn_800FF604(int nPlayer);   // GameMode12.c defines it without the (unused) player
s32   fn_800FF60C(int nPlayer, int i);
s32   fn_800FF620(int nPlayer, int i);
void  fn_800FF634(int nPlayer);
void  fn_801002C0(void);
void  fn_80100B38(void);
void  fn_80101CFC(void);
void  fn_80101D24(void);
s32   fn_80126FA0(void);
s32   fn_80127098(s32 n);

// Run command nCmd.
void fn_800850E4(int nCmd, MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D83B0[nCmd](pArgs, pResult);
}

// Fill the command table.
void fn_80085120(void) {
    int i;

    for (i = 0; i < UI_NUM_ROUND_COMMANDS; i++) {
        lbl_801D83B0[i] = NULL;
    }
    lbl_801D83B0[1] = fn_80085C78;
    lbl_801D83B0[2] = fn_80085D04;
    lbl_801D83B0[3] = fn_80085FDC;
    lbl_801D83B0[4] = fn_8008618C;
    lbl_801D83B0[5] = fn_80086208;
    lbl_801D83B0[6] = fn_80086238;
    lbl_801D83B0[7] = fn_8008628C;
    lbl_801D83B0[8] = fn_80086300;
    lbl_801D83B0[9] = fn_8008633C;
    lbl_801D83B0[10] = fn_80086378;
    lbl_801D83B0[11] = fn_80086384;
    lbl_801D83B0[12] = fn_800863A8;
    lbl_801D83B0[13] = fn_800863CC;
    lbl_801D83B0[14] = fn_800863EC;
    lbl_801D83B0[15] = fn_8008640C;
    lbl_801D83B0[16] = fn_8008642C;
    lbl_801D83B0[17] = fn_80086460;
    lbl_801D83B0[18] = fn_800864A0;
    lbl_801D83B0[19] = fn_800864D8;
    lbl_801D83B0[20] = fn_800864F8;
    lbl_801D83B0[21] = fn_800865A0;
    lbl_801D83B0[22] = fn_800865E0;
    lbl_801D83B0[23] = fn_80086610;
    lbl_801D83B0[24] = fn_80086640;
    lbl_801D83B0[25] = fn_8008666C;
    lbl_801D83B0[26] = fn_800866EC;
    lbl_801D83B0[27] = fn_80086738;
    lbl_801D83B0[28] = fn_8008685C;
    lbl_801D83B0[29] = fn_800868E8;
    lbl_801D83B0[30] = fn_80086900;
    lbl_801D83B0[31] = fn_80086920;
    lbl_801D83B0[32] = fn_80086940;
    lbl_801D83B0[33] = fn_80086970;
    lbl_801D83B0[34] = fn_80086C78;
    lbl_801D83B0[35] = fn_80086D24;
    lbl_801D83B0[36] = fn_80086D58;
    lbl_801D83B0[37] = fn_80086DDC;
    lbl_801D83B0[38] = fn_80086DFC;
    lbl_801D83B0[39] = fn_80086E2C;
    lbl_801D83B0[40] = fn_80086E5C;
    lbl_801D83B0[41] = fn_80086F0C;
    lbl_801D83B0[42] = fn_80086FB4;
    lbl_801D83B0[43] = fn_80087038;
    lbl_801D83B0[44] = fn_8008709C;
    lbl_801D83B0[45] = fn_800870E4;
    lbl_801D83B0[46] = fn_80087130;
    lbl_801D83B0[47] = fn_8008719C;
    lbl_801D83B0[48] = fn_800871E4;
    lbl_801D83B0[49] = fn_80087238;
    lbl_801D83B0[50] = fn_80087270;
    lbl_801D83B0[51] = fn_800872AC;
    lbl_801D83B0[52] = fn_800872F8;
    lbl_801D83B0[53] = fn_80087338;
    lbl_801D83B0[54] = fn_800873D4;
    lbl_801D83B0[55] = fn_80087420;
    lbl_801D83B0[56] = fn_80087460;
    lbl_801D83B0[57] = fn_800874C8;
    lbl_801D83B0[58] = fn_800874F0;
    lbl_801D83B0[59] = fn_80087510;
    lbl_801D83B0[60] = fn_80087548;
    lbl_801D83B0[61] = fn_80087574;
    lbl_801D83B0[62] = fn_8008759C;
    lbl_801D83B0[63] = fn_80087628;
    lbl_801D83B0[64] = fn_80087658;
    lbl_801D83B0[65] = fn_80087684;
    lbl_801D83B0[66] = fn_800876CC;
    lbl_801D83B0[67] = fn_80087700;
    lbl_801D83B0[68] = fn_80087734;
    lbl_801D83B0[69] = fn_80087764;
    lbl_801D83B0[70] = fn_80087790;
    lbl_801D83B0[71] = fn_800877CC;
    lbl_801D83B0[72] = fn_800879B4;
    lbl_801D83B0[73] = fn_80087BE8;
    lbl_801D83B0[74] = fn_80087C1C;
    lbl_801D83B0[75] = fn_80087C7C;
    lbl_801D83B0[76] = fn_80087CBC;
    lbl_801D83B0[77] = fn_80087D14;
    lbl_801D83B0[78] = fn_80087D34;
    lbl_801D83B0[79] = fn_80087D54;
    lbl_801D83B0[80] = fn_80087D8C;
    lbl_801D83B0[81] = fn_80087DD4;
    lbl_801D83B0[82] = fn_80087E1C;
    lbl_801D83B0[83] = fn_80087E60;
    lbl_801D83B0[84] = fn_80087E80;
    lbl_801D83B0[85] = fn_80087E9C;
    lbl_801D83B0[86] = fn_80087ED8;
    lbl_801D83B0[87] = fn_80087F48;
    lbl_801D83B0[88] = fn_80087FAC;
    lbl_801D83B0[89] = fn_8008802C;
    lbl_801D83B0[90] = fn_8008805C;
    lbl_801D83B0[91] = fn_800880AC;
    lbl_801D83B0[92] = fn_800880CC;
    lbl_801D83B0[93] = fn_800880D0;
    lbl_801D83B0[94] = fn_800881A8;
    lbl_801D83B0[95] = fn_800881C8;
    lbl_801D83B0[96] = fn_800881E8;
    lbl_801D83B0[97] = fn_80088208;
    lbl_801D83B0[98] = fn_8008823C;
    lbl_801D83B0[99] = fn_8008828C;
    lbl_801D83B0[100] = fn_800882C0;
    lbl_801D83B0[101] = fn_800882F4;
    lbl_801D83B0[102] = fn_80088324;
    lbl_801D83B0[103] = fn_80088354;
    lbl_801D83B0[104] = fn_80088358;
    lbl_801D83B0[105] = fn_8008835C;
    lbl_801D83B0[106] = fn_800883FC;
    lbl_801D83B0[107] = fn_80088428;
    lbl_801D83B0[108] = fn_80088474;
    lbl_801D83B0[109] = fn_800884F0;
    lbl_801D83B0[110] = fn_80088538;
    lbl_801D83B0[111] = fn_80088570;
    lbl_801D83B0[112] = fn_800885A0;
    lbl_801D83B0[113] = fn_800885F8;
    lbl_801D83B0[114] = fn_80088634;
    lbl_801D83B0[115] = fn_80088654;
    lbl_801D83B0[116] = fn_800869A4;
    lbl_801D83B0[117] = fn_80088660;
    lbl_801D83B0[118] = fn_80088730;
    lbl_801D83B0[120] = fn_800887C4;
    lbl_801D83B0[121] = fn_80088804;
    lbl_801D83B0[122] = fn_80088830;
    lbl_801D83B0[123] = fn_80088834;
    lbl_801D83B0[124] = fn_8008886C;
    lbl_801D83B0[125] = fn_80088AD4;
    lbl_801D83B0[126] = fn_80088CC4;
    lbl_801D83B0[127] = fn_80088CF0;
    lbl_801D83B0[128] = fn_80089324;
    lbl_801D83B0[129] = fn_80089414;
    lbl_801D83B0[130] = fn_800894B4;
    lbl_801D83B0[131] = fn_800866E8;
    lbl_801D83B0[132] = fn_800894E8;
    lbl_801D83B0[133] = fn_80089584;
    lbl_801D83B0[134] = fn_80089590;
    lbl_801D83B0[135] = fn_80089600;
    lbl_801D83B0[136] = fn_80089648;
    lbl_801D83B0[137] = fn_8008967C;
    lbl_801D83B0[138] = fn_80089688;
    lbl_801D83B0[139] = fn_8008968C;
    lbl_801D83B0[140] = fn_800896A8;
    lbl_801D83B0[141] = fn_800896B4;
    lbl_801D83B0[142] = fn_800896D0;
    lbl_801D83B0[143] = fn_800896F0;
    lbl_801D83B0[144] = fn_800897F0;
    lbl_801D83B0[145] = fn_80089A50;
    lbl_801D83B0[146] = fn_80089A8C;
    lbl_801D83B0[147] = fn_80089AD0;
    lbl_801D83B0[148] = fn_80089AD4;
    lbl_801D83B0[149] = fn_80089B78;
    lbl_801D83B0[150] = fn_80089B8C;
    lbl_801D83B0[151] = fn_80089BBC;
    lbl_801D83B0[152] = fn_80085E20;
    lbl_801D83B0[153] = fn_80089BD0;
    lbl_801D83B0[154] = fn_80089BD4;
    lbl_801D83B0[155] = fn_80089BE0;
    lbl_801D83B0[156] = fn_80089C00;
    lbl_801D83B0[157] = fn_80089C20;
    lbl_801D83B0[158] = fn_80089C4C;
    lbl_801D83B0[159] = fn_80089C84;
    lbl_801D83B0[160] = fn_80089CAC;
    lbl_801D83B0[161] = fn_80089CCC;
    lbl_801D83B0[162] = fn_800834A8;
    lbl_801D83B0[163] = fn_80089D04;
    lbl_801D83B0[164] = fn_80089D28;
    lbl_801D83B0[165] = fn_80088160;
    lbl_801D83B0[166] = fn_80089D48;
    lbl_801D83B0[167] = fn_80089D68;
    lbl_801D83B0[168] = fn_80089D98;
    lbl_801D83B0[169] = fn_80089DB0;
    lbl_801D83B0[170] = fn_80089E5C;
    lbl_801D83B0[171] = fn_8008879C;
    lbl_801D83B0[172] = fn_80089E60;
    lbl_801D83B0[173] = fn_80089E64;
    lbl_801D83B0[174] = fn_80085F5C;
    lbl_801D83B0[175] = fn_80089E98;
    lbl_801D83B0[176] = fn_80089E9C;
    lbl_801D83B0[177] = fn_80089ED0;
    lbl_801D83B0[178] = fn_80089F24;
    lbl_801D83B0[180] = fn_80089F6C;
    lbl_801D83B0[179] = fn_8008A010;
    lbl_801D83B0[181] = fn_8008A0CC;
    lbl_801D83B0[197] = fn_8008A128;
    lbl_801D83B0[182] = fn_800860C8;
    lbl_801D83B0[184] = fn_8008A188;
    lbl_801D83B0[185] = fn_8008A1C8;
    lbl_801D83B0[183] = fn_8008A184;
    lbl_801D83B0[186] = fn_8008A208;
    lbl_801D83B0[187] = fn_8008A20C;
    lbl_801D83B0[188] = fn_8008A240;
    lbl_801D83B0[189] = fn_8008A294;
    lbl_801D83B0[190] = fn_8008A2E0;
    lbl_801D83B0[191] = fn_8008A468;
    lbl_801D83B0[192] = fn_8008A4A8;
    lbl_801D83B0[193] = fn_8008A690;
    lbl_801D83B0[194] = fn_8008A758;
    lbl_801D83B0[195] = fn_80089DA4;
    lbl_801D83B0[196] = fn_8008A788;
    lbl_801D83B0[198] = fn_8008A7C8;
    lbl_801D83B0[199] = fn_8008A7D4;
    lbl_801D83B0[200] = fn_8008A800;
    lbl_801D83B0[201] = fn_8008A804;
    lbl_801D83B0[202] = fn_8008A838;
    lbl_801D83B0[203] = fn_8008A86C;
    lbl_801D83B0[204] = fn_8008A870;
    lbl_801D83B0[205] = fn_8008A8B8;
    lbl_801D83B0[206] = fn_8008A8E8;
    lbl_801D83B0[207] = fn_8008A914;
    lbl_801D83B0[208] = fn_8008A964;
    lbl_801D83B0[209] = fn_8008A9A0;
    lbl_801D83B0[210] = fn_8008A9E8;
    lbl_801D83B0[211] = fn_8008AAAC;
    lbl_801D83B0[212] = fn_8008AAB8;
    lbl_801D83B0[213] = fn_8008AB04;
}

// Whether a player with this controller may use the menu now: always outside game type 6; in it,
// a player of that controller (or a CPU) who is still playing (or any, in mode 7).
u8 fn_80085BC0(int nController) {
    int i;

    if (gSession.nGameType != 6) {
        return 1;
    }
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (((s8)GOLFERSTATE_GetCurrentState(i) != GS_WAIT || Game_GetMode() == 7) &&
            (nController == PLAYER(i)->nController || Player_IsCPU(i))) {
            return 1;
        }
    }
    return 0;
}

// Which screen layout the menus use: 3 in mode 8, 2 in modes 6 and 7, 1 split screen, else 0.
void fn_80085C78(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 8) {
        pResult->i = 3;
        return;
    }
    if (Game_GetMode() == 6 || Game_GetMode() == 7) {
        pResult->i = 2;
        return;
    }
    if (gSession.nSplitScreen != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The name a player is shown by: a created golfer's last name, else the golfer's nickname when he
// has one ("NA" = none), else his last name (Cedric the Entertainer is shown as "CEDRIC").
void fn_80085D04(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szLast);
        return;
    }
    if (strcmp(gPlayers[pArgs[0].i].golfer.szNick, "NA") != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szNick);
        return;
    }
    if (stricmp(gPlayers[pArgs[0].i].golfer.szLast, "the entertainer") == 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "CEDRIC");
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szLast);
}

// The same, but a last name longer than 10 letters is cut to its first four.
void fn_80085E20(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];
    char szName[32];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szLast);
        return;
    }
    if (strcmp(gPlayers[pArgs[0].i].golfer.szNick, "NA") != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szNick);
        return;
    }
    if (strlen(gPlayers[pArgs[0].i].golfer.szLast) > 10) {
        strcpy(szName, gPlayers[pArgs[0].i].golfer.szLast);
        szName[4] = '\0';
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szLast);
}

// A player's first name.
void fn_80085F5C(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szFirst);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szFirst);
}

// A player's strokes on a hole; "hole" 18 is the front nine, 19 the back nine, 20 the round.
void fn_80085FDC(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = fn_800E1788(pArgs[0].i) - fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = fn_800E1788(pArgs[0].i);
    } else if (Game_GetMode() == 19) {
        pResult->i = fn_800E8C24((u8)pArgs[0].i, pArgs[1].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nStrokes[pArgs[1].i];
    }
}

// The same for the mode's points.
void fn_800860C8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = fn_800E1788(pArgs[0].i) - fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = fn_800E1788(pArgs[0].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nModePoints[pArgs[1].i];
    }
}

// The course: -1 for the made-up rounds (b137..b139), -2 for a custom round.
void fn_8008618C(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->b137 != 0 || gpGame->b138 != 0 || gpGame->b139 != 0) {
        pResult->i = -1;
        return;
    }
    if (gpGame->b136 != 0) {
        pResult->i = -2;
        return;
    }
    pResult->i = Game_GetCourse();
}

// The current hole, 0..17 in the round.
void fn_80086208(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_CurHoleIndex();
}

// The player whose turn it is; mode 26 alternates between 0 and 1 each time it is asked.
void fn_80086238(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 26) {
        pResult->i = lbl_80281ED8;
        lbl_80281ED8 = 1 - lbl_80281ED8;
        return;
    }
    pResult->i = lbl_80282278;
}

void fn_8008628C(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 9) {
        fn_800E3ECC();
    } else {
        lbl_801D87C0.b0 = 1;
    }
    if (Game_GetMode() == 23) {
        fn_80117DE8(0, 1);
    }
    if (fn_8008AC40()) {
        fn_80062B84(6);
        fn_8008AC3C(0, 1);
    }
}

void fn_80086300(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E3EE0();
    if (lbl_801D87C0.b0 == 0) {
        fn_80100B38();
        return;
    }
    fn_8006F4E0();
}

// Pause the game.
void fn_8008633C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281F18 = fn_800E4BF8() == 0;
    fn_800A7350(1);
    fn_800E3E3C();
}

void fn_80086378(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gNumPlayersSetUp;
}

void fn_80086384(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E42F4(pArgs[0].i);
}

void fn_800863A8(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E4238(pArgs[0].i);
}

// The player's club.
void fn_800863CC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nClub;
}

// The player's kind of shot.
void fn_800863EC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nShotKind;
}

// The player's trajectory (low, normal, high).
void fn_8008640C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nTrajectory;
}

// The player's distance to the pin.
void fn_8008642C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = GM_GetGolferDistanceToPin(pArgs[0].i);
}

// How far a club reaches for a kind of shot.
void fn_80086460(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = AI_MaxDistance(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// The ball's lie, or 99 when it is on surface 151.
void fn_800864A0(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].ball.nSurface == 151) {
        pResult->i = 99;
        return;
    }
    pResult->i = gPlayers[pArgs[0].i].ball.nLie;
}

void fn_800864D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].ball.n6C;
}

// Where the wind blows from, against the player's aim: 0..8, eighths of a turn.
void fn_800864F8(MsgArg* pArgs, MsgArg* pResult) {
    f32 fAim = gPlayers[pArgs[0].i].fAim;
    f32 vWind[3];
    f32 fAngle;

    Wind_Get(vWind);
    fAngle = fn_8000AD78(-vWind[0], vWind[2]) - fAim;
    while (fAngle < 0.0f) {
        fAngle += TWOPI;
    }
    while (fAngle > TWOPI) {
        fAngle -= TWOPI;
    }
    pResult->f = 8.0f * (fAngle / TWOPI);
}

// The wind's speed.
void fn_800865A0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Wind_Get(NULL);
}

void fn_800865E0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AB40();
}

void fn_80086610(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E27C0();
}

// The mode's points for a player on a hole.
void fn_80086640(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nModePoints[pArgs[1].i];
}

// Restart the hole.
void fn_8008666C(MsgArg* pArgs, MsgArg* pResult) {
    gSession.bReplay = 0;
    gReplayData.bF10 = 0;
    EVENT_Trigger(lbl_80282278, 2, NULL, 0);
    GM_RestartHole();
    if (fn_800EC550()) {
        fn_800ED554();
    }
    if (Game_GetMode() == 26 || Game_GetMode() == 22) {
        fn_800A7944();
    }
}

void fn_800866E8(MsgArg* pArgs, MsgArg* pResult) {
}

// The money in the player's save profile (0 without one).
void fn_800866EC(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
        pResult->i = 0;
        return;
    }
    pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].n6C;
}

void fn_80086738(MsgArg* pArgs, MsgArg* pResult) {
    u8 bView6E44;
    u8 bView708C;
    u8 bCharFlag;               // the golfer's uFlags bit 1
    u8 bOtherN20;               // the golfer's n20 is not 9, 11 or 12
    u8 bNearEnd;                // his animation has less than the camera tuning's f170 left

    if (pArgs[0].i >= 5) {
        pResult->i = 1;
        return;
    }
    bView6E44 = fn_800C6E44(fn_80017028(gPlayers[pArgs[0].i].nView[0]));
    bView708C = fn_800C708C(fn_80017028(gPlayers[pArgs[0].i].nView[0]));
    bCharFlag = fn_80062C1C(gPlayers[pArgs[0].i].pChar);
    bOtherN20 = gPlayers[pArgs[0].i].pChar->n20 != 9 && gPlayers[pArgs[0].i].pChar->n20 != 11 &&
                gPlayers[pArgs[0].i].pChar->n20 != 12;
    bNearEnd = fn_80062C28(gPlayers[pArgs[0].i].pChar) < lbl_80281F78->f170;
    if ((bView6E44 && (!bView708C || bCharFlag || bNearEnd)) || bOtherN20) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// A hole's par; "hole" 18 is the front nine, 19 the back nine, 20 the course.
void fn_8008685C(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 18) {
        pResult->i = fn_800D2E60();
    } else if (pArgs[0].i == 19) {
        pResult->i = fn_800D2EB0();
    } else if (pArgs[0].i == 20) {
        pResult->i = fn_800D2FB4(gSession.nTeeSet[pArgs[1].i]);
    } else {
        pResult->i = fn_800D2AD8(pArgs[0].i);
    }
}

// Whether the round plays a hole.
void fn_800868E8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpGame->bHoleSelected[pArgs[0].i];
}

// The holes a player has won (match play).
void fn_80086900(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nHolesWon;
}

void fn_80086920(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].n274;
}

// The game mode.
void fn_80086940(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_GetMode();
}

void fn_80086970(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800FA4B8(pArgs[0].i);
}

// A value of the modes' own for a player, picked by pArgs[1].
void fn_800869A4(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nDD8;
        return;
    case 1:
        pResult->i = fn_800FF4CC(pArgs[0].i);
        return;
    case 2:
        pResult->i = fn_800FF514(pArgs[0].i);
        return;
    case 3:
        pResult->i = fn_800FF49C(pArgs[0].i);
        return;
    case 4:
        pResult->i = fn_800FF4B4(pArgs[0].i);
        return;
    case 5:
        pResult->i = gPlayers[pArgs[0].i].nDD8;
        return;
    case 6:
        pResult->i = gPlayers[pArgs[0].i].aDC4[4];
        return;
    case 7:
        pResult->i = gPlayers[pArgs[0].i].aDC4[0];
        return;
    case 8:
        pResult->i = gPlayers[pArgs[0].i].nDC0;
        return;
    case 9:
        pResult->i = fn_800F20C0(pArgs[0].i);
        return;
    case 10:
        pResult->i = gPlayers[pArgs[0].i].nDDC;
        return;
    case 11:
        pResult->i = gPlayers[pArgs[0].i].aDC4[3];
        return;
    case 12:
        pResult->i = gPlayers[pArgs[0].i].nDE0;
        return;
    case 13:
        fn_800F7DA4();
        pResult->i = 0;
        return;
    case 14:
        pResult->i = fn_800F3490(pArgs[0].i);
        return;
    case 15:
        pResult->i = fn_800F34F0(pArgs[0].i);
        return;
    case 16:
        pResult->i = fn_800F354C(pArgs[0].i);
        return;
    case 17:
        pResult->i = fn_800F363C(pArgs[0].i);
        return;
    case 18:
        pResult->i = fn_800F3654(pArgs[0].i);
        return;
    case 19:
        pResult->i = fn_800F3668(pArgs[0].i);
        return;
    case 20:
        pResult->i = gPlayers[pArgs[0].i].nE88;
        return;
    case 21:
        pResult->i = fn_800F4878();
        return;
    case 22:
        pResult->i = gPlayers[pArgs[0].i].nE8C;
        return;
    case 23:
        pResult->i = gPlayers[pArgs[0].i].nE94;
        return;
    case 24:
        pResult->i = gPlayers[pArgs[0].i].aDC4[1];
        return;
    case 25:
        pResult->i = gPlayers[pArgs[0].i].aDC4[1] + 5;
        return;
    case 26:
        pResult->i = fn_800F4B00();
        return;
    case 27:
        pResult->i = fn_800F1960();
        return;
    }
}

// The ball's lie as a percentage: the surface's share of speed kept, plus RECOVERY's part.
void fn_80086C78(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 100.0f *
                 (0.01f * (gPlayers[pArgs[0].i].ball.f70 *
                           (s8)Golfer_GetAttribute(&gPlayers[pArgs[0].i], ATTR_RECOVERY, ATTR_TOTAL)) +
                  gSurfaceTypes[gPlayers[pArgs[0].i].ball.nSurface].f00);
}

void fn_80086D24(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800D2B4C(pArgs[0].i);
}

// A hole's length from a tee set; "hole" 18 is the front nine, 19 the back nine, 20 the course.
void fn_80086D58(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 18) {
        pResult->i = fn_800D2DA0(pArgs[1].i);
    } else if (pArgs[0].i == 19) {
        pResult->i = fn_800D2E00(pArgs[1].i);
    } else if (pArgs[0].i == 20) {
        pResult->i = fn_800D2D40(pArgs[1].i);
    } else {
        pResult->i = fn_800D2C30(pArgs[0].i, pArgs[1].i);
    }
}

// The tee set a player plays from.
void fn_80086DDC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nTeeSet[pArgs[0].i];
}

void fn_80086DFC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800F9254();
}

void fn_80086E2C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800F9308();
}

// How much the lie can vary: the surface's range, less RECOVERY.
void fn_80086E5C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = gSurfaceTypes[gPlayers[pArgs[0].i].ball.nSurface].f04 *
                 (100.0f - (s8)Golfer_GetAttribute(&gPlayers[pArgs[0].i], ATTR_RECOVERY, ATTR_TOTAL));
}

// A player's score for the round, as the mode counts it.
void fn_80086F0C(MsgArg* pArgs, MsgArg* pResult) {
    switch (Game_GetMode()) {
    case 18:
        pResult->i = fn_800E1788(pArgs[0].i);
        return;
    case 23:
        pResult->i = fn_8011937C(pArgs[0].i, 0, 0);
        return;
    case 19:
        pResult->i = fn_800E8CA8(pArgs[0].i, 1);
        return;
    default:
        pResult->i = fn_800E1904(pArgs[0].i, 1);
        return;
    }
}

// A player's score in one round of the tournament (PGA TOUR mode: from the simulation).
void fn_80086FB4(MsgArg* pArgs, MsgArg* pResult) {
    switch (Game_GetMode()) {
    case 23:
        pResult->i = fn_80119638(pArgs[0].i, 0, pArgs[1].i);
        return;
    }
    pResult->i = gPlayers[pArgs[0].i].nRoundScore[pArgs[1].i];
}

// The PGA TOUR leaderboard: the name of the golfer on row pArgs[0].
void fn_80087038(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = fn_801197CC(0, pArgs[0].i);
    s32 nGolfer = fn_80119118(0, nEntrant);

    strcpy(((MsgString*)pArgs[1].p)->pStr, fn_80118E30(0, nGolfer));
}

// The rank of the golfer on a row.
void fn_8008709C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = fn_801197CC(0, pArgs[0].i);

    pResult->i = fn_801190D8(0, nEntrant);
}

void fn_800870E4(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = fn_801197CC(0, pArgs[0].i);

    pResult->i = fn_80119808(0, nEntrant);
}

// The score of the golfer on a row (the player's own counts the current round).
void fn_80087130(MsgArg* pArgs, MsgArg* pResult) {
    int nEntrant = fn_801197CC(0, pArgs[0].i);

    pResult->i = fn_8011937C(0, nEntrant, !fn_8011908C(0, nEntrant));
}

// The hole the golfer on a row is on.
void fn_8008719C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = fn_801197CC(0, pArgs[0].i);

    pResult->i = fn_80119A04(0, nEntrant);
}

// The golfer on a row's score in round pArgs[1].
void fn_800871E4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80119638(0, fn_801197CC(0, pArgs[0].i), pArgs[1].i);
}

void fn_80087238(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801190D8(0, 0);
}

void fn_80087270(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80119808(0, 0);
}

void fn_800872AC(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800EE470()) {
        pResult->i = fn_80118664(0);
        return;
    }
    pResult->i = 0;
}

void fn_800872F8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800FD704(pArgs[0].i, pArgs[1].i, (s32*)pArgs[2].p);
}

// The name of a player's save profile, or "User <n>" when none is loaded.
void fn_80087338(MsgArg* pArgs, MsgArg* pResult) {
    int nSlot = gPlayers[pArgs[0].i].nIndex;
    char szName[32];

    if (lbl_801D7148.aLoaded[nSlot] == 0) {
        sprintf(szName, "User %d", nSlot + 1);
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[nSlot].szName);
}

// Whether a player is a CPU.
void fn_800873D4(MsgArg* pArgs, MsgArg* pResult) {
    if (Player_IsCPU(pArgs[0].i)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80087420(MsgArg* pArgs, MsgArg* pResult) {
    // port: the studio passes the addresses of the two answers as 32-bit words
    fn_800C9038(gPlayers[pArgs[0].i].nView[0], (f32*)pArgs[1].i, (f32*)pArgs[2].i);
}

void fn_80087460(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData->tour.field.aEntrant[fn_801197CC(0, pArgs[0].i)].n18;
}

void fn_800874C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].tour.field.aEntrant[0].n18;
}

void fn_800874F0(MsgArg* pArgs, MsgArg* pResult) {
    fn_801002C0();
}

// Set the time a player has left (GameMode13).
void fn_80087510(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->pfn25C(pArgs[0].i, pArgs[1].i);
}

// Whether GameBreaker is off.
void fn_80087548(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.a8[0] != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80087574(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E5240(pArgs[0].i);
    fn_80100B38();
}

// The name of a save profile slot, or "User <n>" when none is loaded.
void fn_8008759C(MsgArg* pArgs, MsgArg* pResult) {
    int nSlot = pArgs[0].i;
    char szName[32];

    if (lbl_801D7148.aLoaded[nSlot] == 0) {
        sprintf(szName, "User %d", nSlot + 1);
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[nSlot].szName);
}

// How many holes the round plays.
void fn_80087628(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AB4C();
}

// The same question as fn_80087548, under a second command.
void fn_80087658(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.a8[0] != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80087684(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800EC550()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_800876CC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800ECA08();
}

void fn_80087700(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800ECA34(pArgs[0].i);
}

void fn_80087734(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800ECC14();
}

// Show the tip fn_800E5E54 picks (14: none).
void fn_80087764(MsgArg* pArgs, MsgArg* pResult) {
    int nTip = fn_800E5E54();

    if (nTip != 14) {
        fn_800E5DE4(nTip);
    }
}

void fn_80087790(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_800E6578(pArgs[0].i, pArgs[1].i);
}

// One of the game options, picked by pArgs[0].
void fn_800877CC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (gSession.options.a7[0] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 1:
        if (gSession.options.bGimmes != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 2:
        pResult->i = (s8)gSession.options.a0[0];
        return;
    case 3:
        switch ((s8)gSession.options.a0[4]) {
        case 0:
            pResult->i = 2;
            return;
        case 1:
            pResult->i = 3;
            return;
        case 2:
            pResult->i = 4;
            return;
        case 3:
            pResult->i = 5;
            return;
        case 4:
            pResult->i = 6;
            return;
        case 5:
            pResult->i = 1;
            return;
        }
        break;
    case 4:
        if (gSession.options.a7[1] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 5:
        if (gSession.options.a24[6] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 6:
        if (gSession.options.a24[4] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 7:
        pResult->i = (s8)gSession.options.a0[1];
        return;
    case 8:
        if (gSession.options.a24[1] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    }
}

// Set one of the game options: pArgs[0] picks it, pArgs[1] is the value.
void fn_800879B4(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (pArgs[1].i != 0) {
            fn_8002EBA4((u8*)&gSession.options, 1);
            return;
        }
        fn_8002EBA4((u8*)&gSession.options, 0);
        return;
    case 1:
        if (pArgs[1].i != 0) {
            gSession.options.bGimmes = 1;
            return;
        }
        gSession.options.bGimmes = 0;
        return;
    case 2:
        gSession.options.a0[0] = pArgs[1].i;
        fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
        return;
    case 3:
        switch (pArgs[1].i) {
        case 1:
            gSession.options.a0[4] = 5;
            break;
        case 2:
            gSession.options.a0[4] = 0;
            break;
        case 3:
            gSession.options.a0[4] = 1;
            break;
        case 4:
            gSession.options.a0[4] = 2;
            break;
        case 5:
            gSession.options.a0[4] = 3;
            break;
        case 6:
            gSession.options.a0[4] = 4;
            break;
        }
        fn_800A78F0(0.2f * (s8)gSession.options.a0[4]);
        return;
    case 4:
        if (pArgs[1].i != 0) {
            gSession.options.a7[1] = 1;
            return;
        }
        gSession.options.a7[1] = 0;
        return;
    case 5:
        gSession.options.a0[1] = pArgs[1].i;
        return;
    }
}

// The player whose turn it is concedes the hole.
void fn_80087BE8(MsgArg* pArgs, MsgArg* pResult) {
    gSession.bReplay = 0;
    GM_GolferConcede_Hole(lbl_80282278);
}

void fn_80087C1C(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 1) {
        fn_800E50FC();
        return;
    }
    fn_800E3D38(lbl_80282278, 0);
    fn_80062C80(gPlayers[lbl_80282278].nC58, 0);
    fn_800E508C();
}

void fn_80087C7C(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[lbl_80282278].nIndex].bActive != 0) {
        gpSaveData[gPlayers[lbl_80282278].nIndex].b522F = 1;
    }
}

// The size of the save kind picked (fn_80084FF0(2)) on the card in port pArgs[0], slot pArgs[1].
void fn_80087CBC(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    fn_8009CD80(pos.nPort, pos.nSlot);
    fn_80084FF0(2);
    pResult->i = fn_80084FB4(&pos);
    fn_8009CD7C();
}

void fn_80087D14(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
}

void fn_80087D34(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD7C();
}

void fn_80087D54(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F7E8(pArgs[0].i);
}

// Whether there is a card in the port and slot.
void fn_80087D8C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    fn_8009F7F4(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = (card.uFlags & MC_CARD_PRESENT) != 0;
}

void fn_80087DD4(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    fn_8009F7F4(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = (card.uFlags & 0x08) != 0;
}

// The card's free blocks.
void fn_80087E1C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    fn_8009F7F4(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeBlocks;
}

void fn_80087E60(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E9BC(pArgs, pResult);
}

// Leave a request for the menu UI's update.
void fn_80087E80(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D880C.n4 = pArgs[0].i;
    lbl_801D880C.n0 = 0;
}

void fn_80087E9C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2100(pArgs[0].i, pArgs[1].i);
}

void fn_80087ED8(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    if (lbl_80282278 < 5) {
        gPlayers[lbl_80282278].swing.bCanSpin = 0;
    }
    pResult->i = fn_800A036C(&pos);
}

void fn_80087F48(MsgArg* pArgs, MsgArg* pResult) {
    if (lbl_80282278 < 5) {
        gPlayers[lbl_80282278].swing.bCanSpin = 0;
    }
    pResult->i = fn_800A0610(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// Whether the saved replay is on this hole of this course.
void fn_80087FAC(MsgArg* pArgs, MsgArg* pResult) {
    if (gReplayData.bF10 != 0 && gReplayData.nHole == fn_80015464() &&
        gReplayData.nCourse == Game_GetCourse()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008802C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AC00();
}

// Whether the player is in the zoom-to-aim camera.
void fn_8008805C(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)GOLFERSTATE_GetCurrentState(pArgs[0].i) == GS_ZOOM) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_800880AC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800ED974();
}

void fn_800880CC(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether the player is lining up a shot (states 2 to 4, 8 or 10) and has not started the swing.
void fn_800880D0(MsgArg* pArgs, MsgArg* pResult) {
    int nState = (s8)GOLFERSTATE_GetCurrentState(pArgs[0].i);

    // fake match: states 2 to 4 tested as one unsigned compare
    if (((u32)(nState - GS_SHOT_SETUP) <= GS_ELEVATOR - GS_SHOT_SETUP || nState == GS_KNEE_CAM ||
         nState == GS_SWING) &&
        gPlayers[pArgs[0].i].swing.nState == SW_IDLE_SWING) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Whether the player's swing is in states 1 to 3.
void fn_80088160(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].swing.nState == 2 || gPlayers[pArgs[0].i].swing.nState == 3 ||
        gPlayers[pArgs[0].i].swing.nState == 1) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_800881A8(MsgArg* pArgs, MsgArg* pResult) {
    fn_80101CFC();
}

void fn_800881C8(MsgArg* pArgs, MsgArg* pResult) {
    fn_80101D24();
}

// Who controls the player (CONTROLLER_CPU for the AI).
void fn_800881E8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nController;
}

void fn_80088208(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800E39F0()) {
        fn_800F1DF0();
        return;
    }
    fn_800FDADC();
}

// Whether the player is in the elevator camera.
void fn_8008823C(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)GOLFERSTATE_GetCurrentState(pArgs[0].i) == GS_ELEVATOR) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008828C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800FDC5C((s32*)pArgs[0].p);
}

// Whether the round plays every hole.
void fn_800882C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E1BBC();
}

void fn_800882F4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800FDC0C((s32*)pArgs[0].p, (s32*)pArgs[1].p, (s32*)pArgs[2].p);
}

void fn_80088324(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800ED314();
}

void fn_80088354(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80088358(MsgArg* pArgs, MsgArg* pResult) {
}

// The event's name and description: a real-time event's, else the challenge's.
void fn_8008835C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRound;

    if (fn_800F0818()) {
        strcpy(((MsgString*)pArgs[0].p)->pStr, GameModeDriverRTE_GetName(fn_800F0E20(&nRound)));
        strcpy(((MsgString*)pArgs[1].p)->pStr, GameModeDriverRTE_GetDescription(fn_800F0E20(&nRound)));
        return;
    }
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED280(fn_800EAC7C()));
    strcpy(((MsgString*)pArgs[1].p)->pStr, fn_800ED2C8(fn_800EAC7C()));
}

void fn_800883FC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A73C0(pArgs[0].i, pArgs[1].i);
}

void fn_80088428(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800FD8D0(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p,
                             ((MsgString*)pArgs[2].p)->pStr, (s32*)pArgs[3].p);
}

// The contest hole: its number (-1: none), and into pArgs its par and length.
void fn_80088474(MsgArg* pArgs, MsgArg* pResult) {
    int nHole;
    s32 nPar = 0;
    s32 nLength = 0;

    nHole = fn_800E16F4();

    if (nHole != -1) {
        nPar = fn_800D2AD8(nHole);
        nLength = fn_800D2C30(nHole, gSession.nTeeSet[0]);
    }
    pResult->i = nHole;
    *(s32*)pArgs[0].p = nPar;
    *(s32*)pArgs[1].p = nLength;
}

void fn_800884F0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800FDE58(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p, (s32*)pArgs[2].p,
                             (s32*)pArgs[3].p);
}

void fn_80088538(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80085BC0(pArgs[0].i);
}

void fn_80088570(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD80(pArgs[0].i, pArgs[1].i);
    fn_8009CD7C();
}

// The same as fn_80087CBC.
void fn_800885A0(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    fn_8009CD80(pos.nPort, pos.nSlot);
    fn_80084FF0(2);
    pResult->i = fn_80084FB4(&pos);
    fn_8009CD7C();
}

void fn_800885F8(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A7350(0);
    fn_800E3EE0();
    if (lbl_801D87C0.b0 == 0) {
        fn_80100B38();
    }
}

void fn_80088634(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A6F38();
}

void fn_80088654(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// The modes' questions, picked by pArgs[2].
void fn_80088660(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[2].i) {
    case 0:
        fn_800FF634(pArgs[0].i);
        return;
    case 1:
        pResult->i = fn_800FF604(pArgs[0].i);
        return;
    case 2:
        pResult->i = fn_800FF60C(pArgs[0].i, pArgs[1].i);
        return;
    case 3:
        pResult->i = fn_800FF620(pArgs[0].i, pArgs[1].i);
        return;
    case 4:
        pResult->i = fn_800F2408(pArgs[0].i);
        return;
    case 5:
        pResult->i = fn_800F2494(pArgs[0].i);
        return;
    case 6:
        pResult->i = fn_800F2534(pArgs[0].i);
        return;
    case 7:
        pResult->i = fn_800F24D8(pArgs[0].i);
        return;
    }
}

void fn_80088730(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nD70[Game_CurHoleIndex()];
        return;
    }
}

void fn_8008879C(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_800887C4(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nGameType == 6 && (gSession.nPaused == 2 || gSession.nPaused == 3)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80088804(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800E39F0()) {
        fn_800F1E1C();
    }
}

void fn_80088830(MsgArg* pArgs, MsgArg* pResult) {
}

// A string's length.
void fn_80088834(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

// A record holder's name: pArgs[0] 3 is the contest's, 0 the all-time records' (kind pArgs[1]),
// else the mode's records or the course's; pArgs[2] is the place.
void fn_8008886C(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 3) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", fn_800DAD1C(pArgs[2].i));
        return;
    }
    if (pArgs[0].i == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recA[pArgs[1].i][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 16) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[fn_80015464()][0][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 17) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[fn_80015464()][1][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 13) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[fn_80015464()][2][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 22 && fn_80126FA0() == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.recC[fn_80127098(fn_80015464())][0][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 22 && fn_80126FA0() == 1) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.recC[fn_80127098(fn_80015464())][1][pArgs[2].i].szName);
        return;
    }
    sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
            gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].szName);
}

// The same records' values.
void fn_80088AD4(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 3) {
        pResult->i = fn_800DAD30(pArgs[2].i);
        return;
    }
    if (pArgs[0].i == 0) {
        pResult->i = gSession.recA[pArgs[1].i][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 16) {
        pResult->i = gSession.recB[fn_80015464()][0][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 17) {
        pResult->i = gSession.recB[fn_80015464()][1][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 13) {
        pResult->i = gSession.recB[fn_80015464()][2][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 22 && fn_80126FA0() == 0) {
        pResult->i = gSession.recC[fn_80127098(fn_80015464())][0][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 22 && fn_80126FA0() == 1) {
        pResult->i = gSession.recC[fn_80127098(fn_80015464())][1][pArgs[2].i].nValue;
        return;
    }
    pResult->i = gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].nValue;
}

// Print a number with commas.
void fn_80088CC4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800907AC(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// A player's money for the round by kind, and the multipliers that made it, picked by pArgs[1].
void fn_80088CF0(MsgArg* pArgs, MsgArg* pResult) {
    int nMult;

    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].money.nBase;
        return;
    case 2:
        pResult->i = gPlayers[pArgs[0].i].money.nCourse;
        return;
    case 4:
        pResult->i = gPlayers[pArgs[0].i].money.nTee;
        return;
    case 3:
        pResult->i = gPlayers[pArgs[0].i].money.n2C;
        return;
    case 6:
        pResult->i = gPlayers[pArgs[0].i].money.n38;
        return;
    case 5:
        pResult->i = gPlayers[pArgs[0].i].money.nTourCard;
        return;
    case 1:
        pResult->i = gPlayers[pArgs[0].i].money.n24;
        return;
    case 7:
        pResult->i = gPlayers[pArgs[0].i].money.n3C;
        return;
    case 8:
        pResult->i = gPlayers[pArgs[0].i].money.n0;
        return;
    case 9:
        pResult->i = gPlayers[pArgs[0].i].money.n4;
        return;
    case 10:
        pResult->i = gPlayers[pArgs[0].i].money.n8;
        return;
    case 11:
        pResult->i = gPlayers[pArgs[0].i].money.nC;
        return;
    case 12:
        pResult->i = gPlayers[pArgs[0].i].money.n10;
        return;
    case 13:
        pResult->i = gPlayers[pArgs[0].i].money.n14;
        return;
    case 14:
        pResult->i = gPlayers[pArgs[0].i].money.n18;
        return;
    case 15:
        pResult->i = gPlayers[pArgs[0].i].money.n1C;
        return;
    case 16:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].n6C - gPlayers[pArgs[0].i].money.n24;
        return;
    case 100:
        pResult->i = (s32)fn_800D6EEC() - 1;
        return;
    case 102:
        switch (gSession.nTeeSet[pArgs[0].i]) {
        case 0:
            pResult->i = 2;
            return;
        case 1:
            pResult->i = 1;
            return;
        case 2:
            pResult->i = 0;
            return;
        case 3:
            pResult->i = 1;
            return;
        }
        break;
    case 101:
        switch (gpGame->nPinSet[Game_CurHoleIndex()]) {
        case 0:
            pResult->i = 0;
            return;
        case 1:
            pResult->i = 1;
            return;
        case 2:
            pResult->i = 2;
            return;
        case 3:
            pResult->i = 3;
            return;
        }
        break;
    case 103:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        // EA bug: a level outside 0..6 leaves nMult unset (here and in case 104). nMult is the
        // index into the whole multiplier table (23..28: the TOUR card group).
        switch (gpSaveData[gPlayers[pArgs[0].i].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            nMult = EARN_MULT_TOUR;
            break;
        case 2:
            nMult = EARN_MULT_TOUR + 1;
            break;
        case 3:
            nMult = EARN_MULT_TOUR + 2;
            break;
        case 4:
            nMult = EARN_MULT_TOUR + 3;
            break;
        case 5:
            nMult = EARN_MULT_TOUR + 4;
            break;
        case 6:
            nMult = EARN_MULT_TOUR + 5;
            break;
        }
        fn_801025F4();
        pResult->i = lbl_80200538.aMult[nMult];
        return;
    case 104:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        switch (gpSaveData[gPlayers[pArgs[0].i].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            nMult = 1;
            break;
        case 2:
            nMult = 2;
            break;
        case 3:
            nMult = 3;
            break;
        case 4:
            nMult = 4;
            break;
        case 5:
            nMult = 5;
            break;
        case 6:
            nMult = 6;
            break;
        }
        fn_801025F4();
        pResult->i = nMult;
        return;
    case 200:
        if (gPlayers[pArgs[0].i].money.n4 != 0 || gPlayers[pArgs[0].i].money.n8 != 0 ||
            gPlayers[pArgs[0].i].money.nC != 0 || gPlayers[pArgs[0].i].money.n10 != 0 ||
            gPlayers[pArgs[0].i].money.n14 != 0 || gPlayers[pArgs[0].i].money.n18 != 0 ||
            gPlayers[pArgs[0].i].money.n1C != 0) {
            pResult->i = 2;
            return;
        }
        pResult->i = 1;
        break;
    }
}

// A record holder's name, with its string's length: pArgs[0] 0 is the all-time records, else the
// course's.
void fn_80089324(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recA[pArgs[1].i][pArgs[2].i].szName);
    } else {
        // EA bug: passes the whole record to "%s", not its name
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i]);
    }
    ((MsgString*)pArgs[3].p)->nLen = strlen(((MsgString*)pArgs[3].p)->pStr);
}

// A record's value: pArgs[0] 0 is the all-time records, else the course's.
void fn_80089414(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        pResult->i = gSession.recA[pArgs[1].i][pArgs[2].i].nValue;
        return;
    }
    pResult->i = gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].nValue;
}

// The ball's distance from the pin.
void fn_800894B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_800D0478(pArgs[0].i);
}

// Entry pArgs[0] of the speed-golf event log, into pArgs[1] (the player) and pArgs[2] (the event).
void fn_800894E8(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281EDC++;
    if (lbl_80281EDC > 10) {
        lbl_80281EDC = -1;
    }
    *(s32*)pArgs[1].p = 0;
    *(s32*)pArgs[2].p = lbl_80281EDC;
    if (pArgs[0].i < 0 || pArgs[0].i >= lbl_802823D0) {
        *(s32*)pArgs[1].p = 0;
        *(s32*)pArgs[2].p = -1;
        return;
    }
    *(s32*)pArgs[1].p = lbl_802120F8[pArgs[0].i].nPlayer;
    *(s32*)pArgs[2].p = lbl_802120F8[pArgs[0].i].nEvent;
}

void fn_80089584(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

void fn_80089590(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (fn_800DAD44()) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 1:
        pResult->i = fn_800DAD4C();
        return;
    }
}

void fn_80089600(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_801025F4()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089648(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Rand_Next(1);
}

void fn_8008967C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089688(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008968C(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

void fn_800896A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

void fn_800896B4(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

void fn_800896D0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008299C(pArgs, pResult);
}

// The card in port pArgs[0], slot pArgs[1]: into pArgs[2..6] whether its sectors are not 8 KB, and
// its encoding, wrong-device, I/O-error and broken flags.
void fn_800896F0(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    fn_8009F7F4(&card, pArgs[0].i, pArgs[1].i);
    if (card.nSectorSize != 0x2000) {
        *(s32*)pArgs[2].p = 1;
    } else {
        *(s32*)pArgs[2].p = 0;
    }
    if (card.uFlags & MC_CARD_ENCODING) {
        *(s32*)pArgs[3].p = 1;
    } else {
        *(s32*)pArgs[3].p = 0;
    }
    if (card.uFlags & MC_CARD_WRONGDEVICE) {
        *(s32*)pArgs[4].p = 1;
    } else {
        *(s32*)pArgs[4].p = 0;
    }
    if (card.uFlags & MC_CARD_IOERROR) {
        *(s32*)pArgs[5].p = 1;
    } else {
        *(s32*)pArgs[5].p = 0;
    }
    if (card.uFlags & MC_CARD_BROKEN) {
        *(s32*)pArgs[6].p = 1;
        return;
    }
    *(s32*)pArgs[6].p = 0;
}

// The course's name, or the round's: a custom round's saved name, "Random 18", "Dream 18" or a
// region's courses.
void fn_800897F0(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && gpGame->b136 != 0) {
        if (gpGame->nCurCourse == 7 || Game_CurHoleIndex() >= 15) {
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Sherwood CC");
            return;
        }
        strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[gpGame->nCurCourse]);
        return;
    }
    if (gpGame->b136 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr,
               gpSaveData[gpGame->nSaveSlot].aSavedRound[gpGame->nSaveCourse].szName);
        return;
    }
    if (gpGame->b137 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Random 18");
        return;
    }
    if (gpGame->b138 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Dream 18");
        return;
    }
    if (gpGame->b139 != 0) {
        // the regions follow the courses: region 1 is 24
        switch (gpGame->b139 + 23) {
        case 24:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US Northwest");
            return;
        case 25:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US Southwest");
            return;
        case 26:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US East");
            return;
        case 27:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Europe");
            return;
        case 28:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Pacific");
            return;
        case 29:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "S. Hemisphere");
            return;
        default:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "");
            return;
        }
    }
    if (gpGame->nCurCourse == 7 || Game_CurHoleIndex() >= 15) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Sherwood CC");
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[gpGame->nCurCourse]);
}

void fn_80089A50(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        fn_800E53A4(1);
        return;
    }
    fn_800E53A4(0);
}

// The card's free directory entries.
void fn_80089A8C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    fn_8009F7F4(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeFiles;
}

void fn_80089AD0(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether the player whose turn it is may concede: nothing holds him, the hole is not over and
// his ball is not in the cup.
void fn_80089AD4(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800E4254(lbl_80282278)) {
        pResult->i = 0;
        return;
    }
    if (gpGame->pfnHoleFinished(lbl_80282278, 1)) {
        pResult->i = 0;
        return;
    }
    if (gPlayers[lbl_80282278].ball.nLie == 12) {
        pResult->i = 0;
        return;
    }
    pResult->i = 1;
}

void fn_80089B78(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

void fn_80089B8C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800DADC0();
}

// The game's title.
void fn_80089BBC(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

void fn_80089BD0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089BD4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089BE0(MsgArg* pArgs, MsgArg* pResult) {
    fn_80082DBC(pArgs, pResult);
}

void fn_80089C00(MsgArg* pArgs, MsgArg* pResult) {
    fn_80082E10(pArgs, pResult);
}

void fn_80089C20(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].f < 0.0f) {
        pArgs[0].f = 1.0f;
    }
    lbl_80202898.f54 = pArgs[0].f;
}

void fn_80089C4C(MsgArg* pArgs, MsgArg* pResult) {
    int nMsg = pArgs[0].i;

    fn_800A6F38();
    fn_8008AC4C((u16)nMsg, 0);
}

void fn_80089C84(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->bD4 != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089CAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A6148();
}

// Whether the player missed the cut.
void fn_80089CCC(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].bPlayerCut != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089D04(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E4204();
    fn_800E53AC();
}

void fn_80089D28(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E53C0();
}

void fn_80089D48(MsgArg* pArgs, MsgArg* pResult) {
    fn_800834E8(pArgs, pResult);
}

void fn_80089D68(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008AC48(lbl_80282278, ((MsgString*)pArgs[0].p)->pStr);
}

void fn_80089D98(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089DA4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// A player's value of the modes' own, picked by pArgs[1] (8: the number of players).
void fn_80089DB0(MsgArg* pArgs, MsgArg* pResult) {
    Player* pPlayer = &gPlayers[pArgs[0].i];

    switch (pArgs[1].i) {
    case 0:
        pResult->i = pPlayer->nEA0;
        return;
    case 1:
        pResult->i = pPlayer->nEC0;
        return;
    case 2:
        pResult->i = pPlayer->nEA8;
        return;
    case 3:
        pResult->i = pPlayer->nECC;
        return;
    case 4:
        pResult->i = pPlayer->nED4;
        return;
    case 5:
        pResult->i = pPlayer->nED8;
        return;
    case 6:
        pResult->i = pPlayer->nEDC;
        return;
    case 7:
        pResult->i = pPlayer->nEBC;
        return;
    case 8:
        pResult->i = gSession.nNumPlayers;
        return;
    }
}

void fn_80089E5C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089E60(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089E64(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089E98(MsgArg* pArgs, MsgArg* pResult) {
}

// The clubs in a player's bag.
void fn_80089E9C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Bag_CountClubs(pArgs[0].i);
}

// Whether a club is in a player's bag.
void fn_80089ED0(MsgArg* pArgs, MsgArg* pResult) {
    if (Bag_HasClub(pArgs[0].i, pArgs[1].i)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Battle mode: add a club to a player's bag (pArgs[1] set) or take it out.
void fn_80089F24(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        GameModeBattle_AddClub(pArgs[0].i, pArgs[2].i);
        return;
    }
    GameModeBattle_RemoveClub(pArgs[0].i, pArgs[2].i);
}

void fn_80089F6C(MsgArg* pArgs, MsgArg* pResult) {
    View* pView = fn_80017028(gPlayers[0].nView[0]);
    f32 vZero[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    pResult->i = 0;
    if (pResult->i == 0) {
        fn_80063CBC(pView, vZero);
        GOLFERSTATE_Set(GS_WAIT, 0);
        fn_801102AC();
        fn_8001A870();
        fn_8006F4B4();
        pView->script.nCamera = 0;
    }
}

// The disc drive's state for the menus (100: fn_80110450 says so).
void fn_8008A010(MsgArg* pArgs, MsgArg* pResult) {
    switch (DVDGetDriveStatus()) {
    case 7:
        if (fn_8011027C() != 0) {
            pResult->i = 0;
        } else {
            pResult->i = 1;
        }
        break;
    case 6:
        if (fn_8011027C() != 0) {
            pResult->i = 2;
        } else {
            pResult->i = 3;
        }
        break;
    case 1:
        pResult->i = 4;
        break;
    default:
        pResult->i = 5;
        break;
    }
    if (fn_80110450() != 0) {
        pResult->i = 100;
    }
}

void fn_8008A0CC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80086300(NULL, NULL);
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
    if (pResult->i == 0) {
        fn_8006F4E0();
    }
}

void fn_8008A128(MsgArg* pArgs, MsgArg* pResult) {
    fn_800885F8(NULL, NULL);
    if (!fn_800EC550()) {
        fn_8006F4E0();
    }
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
}

void fn_8008A184(MsgArg* pArgs, MsgArg* pResult) {
}

// A challenge's first line of text.
void fn_8008A188(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED280(pArgs[1].i));
}

// Its second line.
void fn_8008A1C8(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED2C8(pArgs[1].i));
}

void fn_8008A208(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008A20C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E8114(pArgs[0].i);
}

// Battle mode: whether a player may take a club.
void fn_8008A240(MsgArg* pArgs, MsgArg* pResult) {
    // the caller tests only the low byte of the result
    if ((u8)GameModeBattle_CanAddClub(pArgs[0].i, pArgs[1].i) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A294(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800EE8B8()->b0 != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The PGA TOUR result screen: the player's name, the money won ("$1,234"), the tournament's name
// and end date, and the place ("1st place").
void fn_8008A2E0(MsgArg* pArgs, MsgArg* pResult) {
    Pga80205F30* pTour = fn_800EE8B8();
    int nPlace;

    strcpy(((MsgString*)pArgs[0].p)->pStr, gpSaveData->szName);
    ((MsgString*)pArgs[1].p)->pStr[0] = '$';
    fn_800907AC(pTour->n8, ((MsgString*)pArgs[1].p)->pStr + 1);
    strcpy(((MsgString*)pArgs[2].p)->pStr, GameModeDriverPGATour_GetName(gpSaveData->tour.nEvent));
    fn_800D28DC(GameModeDriverPGATour_GetEndDate(gpSaveData->tour.nEvent), ((MsgString*)pArgs[3].p)->pStr);
    nPlace = pTour->n4;
    if (nPlace > 100) {
        nPlace = pTour->n4 % 100;
    }
    if (nPlace > 20) {
        nPlace %= 10;
    }
    if (nPlace == 1) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dst", pTour->n4);
    } else if (nPlace == 2) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dnd", pTour->n4);
    } else if (nPlace == 3) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%drd", pTour->n4);
    } else {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dth", pTour->n4);
    }
    strcat(((MsgString*)pArgs[4].p)->pStr, " place");
}

// The current PGA TOUR event's name.
void fn_8008A468(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr,
           GameModeDriverPGATour_GetName(GameModeDriverPGATour_GetCurrentEventID()));
}

// Pass on to fn_800A61C4 a number for pArgs[0] and pArgs[1] (0 or 2); nothing in modes 22 and 26.
void fn_8008A4A8(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 26 || Game_GetMode() == 22) return;
    switch (pArgs[1].i) {
    case 0:
        switch (pArgs[0].i) {
        case 0:
            fn_800A61C4(2);
            return;
        case 2:
            fn_800A61C4(8);
            return;
        case 3:
            fn_800A61C4(12);
            return;
        case 4:
            fn_800A61C4(6);
            return;
        case 5:
            fn_800A61C4(2);
            return;
        case 6:
            fn_800A61C4(2);
            return;
        case 7:
            fn_800A61C4(2);
            return;
        case 8:
            fn_800A61C4(0);
            return;
        case 9:
            fn_800A61C4(4);
            return;
        case 10:
            fn_800A61C4(4);
            return;
        case 11:
            fn_800A61C4(4);
            return;
        case 12:
            fn_800A61C4(4);
            return;
        case 13:
            fn_800A61C4(10);
            return;
        }
        break;
    case 1:
        break;
    case 2:
        switch (pArgs[0].i) {
        case 0:
            fn_800A61C4(3);
            return;
        case 2:
            fn_800A61C4(9);
            return;
        case 3:
            fn_800A61C4(13);
            return;
        case 4:
            fn_800A61C4(7);
            return;
        case 5:
            fn_800A61C4(3);
            return;
        case 6:
            fn_800A61C4(3);
            return;
        case 7:
            fn_800A61C4(3);
            return;
        case 8:
            fn_800A61C4(1);
            return;
        case 9:
            fn_800A61C4(5);
            return;
        case 10:
            fn_800A61C4(5);
            return;
        case 11:
            fn_800A61C4(5);
            return;
        case 12:
            fn_800A61C4(5);
            return;
        case 13:
            fn_800A61C4(11);
            break;
        }
        break;
    }
}

// Play sound pArgs[1] of kind pArgs[0]. The sound numbers are 16-bit.
void fn_8008A690(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
    case 2:
        fn_8008AC4C((u16)pArgs[1].i, 0);
        return;
    case 6:
        fn_8008AD54((u16)pArgs[1].i, 0);
        return;
    case 7:
        fn_8008AD28((u16)pArgs[1].i, 0);
        return;
    case 8:
        fn_8008ACFC((u16)pArgs[1].i, 0);
        return;
    case 9:
        fn_8008ACD0((u16)pArgs[1].i, 0);
        return;
    case 10:
        fn_8008ACA4((u16)pArgs[1].i, 0);
        return;
    case 11:
        fn_8008AC78((u16)pArgs[1].i, 0);
        return;
    }
}

void fn_8008A758(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A7528();
}

void fn_8008A788(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

void fn_8008A7C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_8008A7D4(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.options.a24[1] != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A800(MsgArg* pArgs, MsgArg* pResult) {
}

// Battle mode: how many clubs a player may still take out.
void fn_8008A804(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_NumRemovableClubsLeft(pArgs[0].i);
}

void fn_8008A838(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8003DCAC();
}

void fn_8008A86C(MsgArg* pArgs, MsgArg* pResult) {
}

// Battle mode: whether the club stealing is over.
void fn_8008A870(MsgArg* pArgs, MsgArg* pResult) {
    if (GameModeBattle_IsClubStealingFinished() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Battle mode: the winner.
void fn_8008A8B8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_GetWinner();
}

// Whether the game is paused.
void fn_8008A8E8(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nPaused != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A914(MsgArg* pArgs, MsgArg* pResult) {
    if ((u8)fn_800F031C(((MsgString*)pArgs[0].p)->pStr) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A964(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        fn_800E53E8(0);
        return;
    }
    fn_800E53E8(1);
}

// Whether a real-time event is being played.
void fn_8008A9A0(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_800F0818() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The same as fn_80085FDC, without mode 19's count.
void fn_8008A9E8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = fn_800E1788(pArgs[0].i) - fn_800E19A4(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = fn_800E1788(pArgs[0].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nStrokes[pArgs[1].i];
    }
}

void fn_8008AAAC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = -1;
}

// Whether the golfer on a leaderboard row missed the cut.
void fn_8008AAB8(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = fn_801197CC(0, pArgs[0].i);

    pResult->i = fn_801197A4(0, nEntrant);
}

void fn_8008AB04(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801197A4(pArgs[0].i, 0);
}

s32 fn_8008AB40(void) {
    return gpGame->n4;
}

// How many holes the round plays.
s32 fn_8008AB4C(void) {
    s32 n = 0;
    int i;

    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            n++;
        }
    }
    return n;
}

// How many of the round's holes are left from the current one on.
s32 fn_8008AC00(void) {
    s32 n = 0;
    int i;

    for (i = gpGame->nCurHole; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            n++;
        }
    }
    return n;
}

void fn_8008AC3C(int a, int b) {
}

u8 fn_8008AC40(void) {
    return 0;
}

void fn_8008AC48(int nPlayer, char* sz) {
}

void fn_8008AC4C(int nMsg, int a) {
    fn_800A7664(14, nMsg, a);
}

void fn_8008AC78(int nMsg, int a) {
    fn_800A7664(19, nMsg, a);
}

void fn_8008ACA4(int nMsg, int a) {
    fn_800A7664(17, nMsg, a);
}

void fn_8008ACD0(int nMsg, int a) {
    fn_800A7664(16, nMsg, a);
}

void fn_8008ACFC(int nMsg, int a) {
    fn_800A7664(15, nMsg, a);
}

void fn_8008AD28(int nMsg, int a) {
    fn_800A7664(20, nMsg, a);
}

void fn_8008AD54(int nMsg, int a) {
    fn_800A7664(18, nMsg, a);
}
