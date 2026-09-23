// FE_MessageTable.c (our name): the menus' message table. The menu UI (uiProcessInterface.c) sends
// its messages here while the front end runs (game type 3): fn_80079EA8 fills a table of 770
// handlers and fn_80079E6C calls the one for a message's number with its arguments and results.
// The handlers read and set what the menus show: golfer names, the session's setup, the save
// profile's stats and records, the Create-A-Player choices. TW06 has GetGolferName in
// apt_fe_gamemessages.c. Rounds have their own table (fn_800850E4).

#include "game.h"
#include "camera.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "core/memcard.h"
#include "core/easb.h"
#include "game/earnings.h"

// Outside this file.
void fn_800142A4(s8 n);                 // sets lbl_80281C98
void fn_80057438(SaveProfile* pProfile);
void fn_8008E354(void);                 // FEgolferanim.c
void fn_8008F80C(s32 p0, s32 p1);       // uiProcessInterface.c
void fn_8008DAEC(void);                 // FE_Manager.c
void fn_8008E358(s32 p0);               // FEgolferanim.c
s32  fn_800A0C6C(MCCardPosStr* pPos);   // MC.c
void fn_800A78F0(f32 f);
void fn_8009CD80(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_8009D390(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_8009EB44(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_800A1164(s32 nPort, s32 nSlot, char* pName, s32 n);     // MC.c
u8   fn_800E22E4(int nSlot, int a, int b);      // GameRound.c
int  fn_800E2520(int nMode);            // GameRound.c
void fn_800E25E0(void);                 // GameRound.c
void GM_SetupCustomHoleSelection(void); // GameManager.c
int  GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);  // GameManager.c
void fn_800EAE44(int nId);              // GameMode5.c
s32  fn_800EAE6C(void);                 // GameMode5.c
char* fn_800ED280(int nId);             // GameMode5.c
char* fn_800ED2C8(int nId);             // GameMode5.c
void fn_800ED650(int i, s32* pA, s32* pB, s32* pC);     // GameMode5.c
int  fn_801020C0(void);                 // GameMode4.c
void fn_80102308(s32 n);                // GameMode4.c
void fn_8010D334(s32 v);                // CharSliders.c
void fn_8008DD34(int nSlot, int n);
void fn_801102AC(void);
s32  fn_8009FCFC(MCCardPos* pPos);      // MC.c: load the save from the card
s32  fn_8009FE90(MCCardPos* pPos);      // } MC.c, in lbl_8018C7D8's set 0
s32  fn_800A09EC(MCCardPos* pPos);      // }
s32  fn_800A0E6C(MCCardPos* pPos);      // }
s32  fn_800A1964(MCCardPos* pPos);      // }
void fn_800A4FD8(void);
void fn_800A73F0(int n);
void fn_8010F2FC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F3A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8011DF90(MsgArg* pArgs, MsgArg* pResult);
void fn_80123FF8(void);
void fn_8012408C(s32 v);
s32  fn_8012411C(void);
void fn_80124138(s32 n);
s32  fn_80124174(void);
s32  fn_801241CC(void);

// The other files' message handlers in the table (the Create-A-Player screens, the logo editor,
// the PGA TOUR screens, the stats screen, the EA Sports Bio...).
void fn_801077EC(MsgArg* pArgs, MsgArg* pResult);
void fn_80107828(MsgArg* pArgs, MsgArg* pResult);
void fn_801078B8(MsgArg* pArgs, MsgArg* pResult);
void fn_80107994(MsgArg* pArgs, MsgArg* pResult);
void fn_80107998(MsgArg* pArgs, MsgArg* pResult);
void fn_8010799C(MsgArg* pArgs, MsgArg* pResult);
void fn_80107A2C(MsgArg* pArgs, MsgArg* pResult);
void fn_80107BA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80107BA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80107BF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80107C30(MsgArg* pArgs, MsgArg* pResult);
void fn_80107C3C(MsgArg* pArgs, MsgArg* pResult);
void fn_80107DBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80107EB0(MsgArg* pArgs, MsgArg* pResult);
void fn_80107FAC(MsgArg* pArgs, MsgArg* pResult);
void fn_80108070(MsgArg* pArgs, MsgArg* pResult);
void fn_80108140(MsgArg* pArgs, MsgArg* pResult);
void fn_80108178(MsgArg* pArgs, MsgArg* pResult);
void fn_80108244(MsgArg* pArgs, MsgArg* pResult);
void fn_80108300(MsgArg* pArgs, MsgArg* pResult);
void fn_80108314(MsgArg* pArgs, MsgArg* pResult);
void fn_80108398(MsgArg* pArgs, MsgArg* pResult);
void fn_8010840C(MsgArg* pArgs, MsgArg* pResult);
void fn_80108494(MsgArg* pArgs, MsgArg* pResult);
void fn_801084F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80108594(MsgArg* pArgs, MsgArg* pResult);
void fn_80108690(MsgArg* pArgs, MsgArg* pResult);
void fn_80108768(MsgArg* pArgs, MsgArg* pResult);
void fn_801088A4(MsgArg* pArgs, MsgArg* pResult);
void fn_801088C8(MsgArg* pArgs, MsgArg* pResult);
void fn_80108904(MsgArg* pArgs, MsgArg* pResult);
void fn_8010893C(MsgArg* pArgs, MsgArg* pResult);
void fn_801089BC(MsgArg* pArgs, MsgArg* pResult);
void fn_801089DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80108A0C(MsgArg* pArgs, MsgArg* pResult);
void fn_80108B10(MsgArg* pArgs, MsgArg* pResult);
void fn_80108B84(MsgArg* pArgs, MsgArg* pResult);
void fn_80108C00(MsgArg* pArgs, MsgArg* pResult);
void fn_80108CA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80108D1C(MsgArg* pArgs, MsgArg* pResult);
void fn_80108DF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80108E4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80108E9C(MsgArg* pArgs, MsgArg* pResult);
void fn_801090B4(MsgArg* pArgs, MsgArg* pResult);
void fn_80109120(MsgArg* pArgs, MsgArg* pResult);
void fn_801091B8(MsgArg* pArgs, MsgArg* pResult);
void fn_8010920C(MsgArg* pArgs, MsgArg* pResult);
void fn_80109248(MsgArg* pArgs, MsgArg* pResult);
void fn_80109294(MsgArg* pArgs, MsgArg* pResult);
void fn_80109304(MsgArg* pArgs, MsgArg* pResult);
void fn_80109354(MsgArg* pArgs, MsgArg* pResult);
void fn_80109388(MsgArg* pArgs, MsgArg* pResult);
void fn_801093B4(MsgArg* pArgs, MsgArg* pResult);
void fn_80109430(MsgArg* pArgs, MsgArg* pResult);
void fn_80109434(MsgArg* pArgs, MsgArg* pResult);
void fn_80109458(MsgArg* pArgs, MsgArg* pResult);
void fn_8010948C(MsgArg* pArgs, MsgArg* pResult);
void fn_801094C0(MsgArg* pArgs, MsgArg* pResult);
void fn_801094FC(MsgArg* pArgs, MsgArg* pResult);
void fn_80109500(MsgArg* pArgs, MsgArg* pResult);
void fn_80109514(MsgArg* pArgs, MsgArg* pResult);
void fn_80109618(MsgArg* pArgs, MsgArg* pResult);
void fn_80109650(MsgArg* pArgs, MsgArg* pResult);
void fn_8010969C(MsgArg* pArgs, MsgArg* pResult);
void fn_80109700(MsgArg* pArgs, MsgArg* pResult);
void fn_80109734(MsgArg* pArgs, MsgArg* pResult);
void fn_80109738(MsgArg* pArgs, MsgArg* pResult);
void fn_80109760(MsgArg* pArgs, MsgArg* pResult);
void fn_80109780(MsgArg* pArgs, MsgArg* pResult);
void fn_801097FC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010988C(MsgArg* pArgs, MsgArg* pResult);
void fn_801098AC(MsgArg* pArgs, MsgArg* pResult);
void fn_801098B0(MsgArg* pArgs, MsgArg* pResult);
void fn_80109BA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80109CBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80109D5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80109DDC(MsgArg* pArgs, MsgArg* pResult);
void fn_80109DE0(MsgArg* pArgs, MsgArg* pResult);
void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult);
void fn_80109EE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80109FB4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A208(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A238(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A2C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A3A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A3C4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8010A400(MsgArg* pArgs, MsgArg* pResult);
void fn_8010E748(MsgArg* pArgs, MsgArg* pResult);
void fn_8010E85C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010E890(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EA24(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EAC4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EB9C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EBDC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EEA8(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EEE4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EF80(MsgArg* pArgs, MsgArg* pResult);
void fn_8010EF8C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F10C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F1B4(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F248(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F278(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F2CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F440(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F4EC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F52C(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F550(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F574(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F5AC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F63C(MsgArg* pArgs, MsgArg* pResult);
void fn_80111F58(MsgArg* pArgs, MsgArg* pResult);
void fn_80112020(MsgArg* pArgs, MsgArg* pResult);
void fn_8011206C(MsgArg* pArgs, MsgArg* pResult);
void fn_80112130(MsgArg* pArgs, MsgArg* pResult);
void fn_80112154(MsgArg* pArgs, MsgArg* pResult);
void fn_801121D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80112254(MsgArg* pArgs, MsgArg* pResult);
void fn_80112290(MsgArg* pArgs, MsgArg* pResult);
void fn_801122D8(MsgArg* pArgs, MsgArg* pResult);
void fn_80112384(MsgArg* pArgs, MsgArg* pResult);
void fn_80112438(MsgArg* pArgs, MsgArg* pResult);
void fn_8011243C(MsgArg* pArgs, MsgArg* pResult);
void fn_8011247C(MsgArg* pArgs, MsgArg* pResult);
void fn_801124D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80112548(MsgArg* pArgs, MsgArg* pResult);
void fn_80112580(MsgArg* pArgs, MsgArg* pResult);
void fn_801125B8(MsgArg* pArgs, MsgArg* pResult);
void fn_801125E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8011CF64(MsgArg* pArgs, MsgArg* pResult);
void fn_8011D05C(MsgArg* pArgs, MsgArg* pResult);
void fn_8011D09C(MsgArg* pArgs, MsgArg* pResult);
void fn_8011D0A8(MsgArg* pArgs, MsgArg* pResult);
void fn_8011D268(MsgArg* pArgs, MsgArg* pResult);
void fn_8011D274(MsgArg* pArgs, MsgArg* pResult);
void fn_8011DDFC(MsgArg* pArgs, MsgArg* pResult);
void fn_8011DEF0(MsgArg* pArgs, MsgArg* pResult);
void fn_80121430(MsgArg* pArgs, MsgArg* pResult);
void fn_80121458(MsgArg* pArgs, MsgArg* pResult);
void fn_8012153C(MsgArg* pArgs, MsgArg* pResult);
void fn_8012168C(MsgArg* pArgs, MsgArg* pResult);
void fn_8012172C(MsgArg* pArgs, MsgArg* pResult);
void fn_80121770(MsgArg* pArgs, MsgArg* pResult);
void fn_801217C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80121808(MsgArg* pArgs, MsgArg* pResult);
void fn_8012185C(MsgArg* pArgs, MsgArg* pResult);
void fn_80121890(MsgArg* pArgs, MsgArg* pResult);
void fn_8012597C(MsgArg* pArgs, MsgArg* pResult);
void fn_80125A24(MsgArg* pArgs, MsgArg* pResult);
void fn_80125AA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80125B38(MsgArg* pArgs, MsgArg* pResult);
void fn_80125BB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80125BD8(MsgArg* pArgs, MsgArg* pResult);
void fn_80125C5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80125D08(MsgArg* pArgs, MsgArg* pResult);
void fn_80125D78(MsgArg* pArgs, MsgArg* pResult);
void fn_80125DE0(MsgArg* pArgs, MsgArg* pResult);

// This file.
void GetGolferName(int nGolfer, char* szName);
void fn_8007E458(int n, MsgArg* pArgs, MsgArg* pResult);
s32  fn_80084FB4(MCCardPos* pPos);

// This file's message handlers, in address order.
void fn_8007BBA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BBD8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BC10(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BC48(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BC74(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BCA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BCA4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BCC4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BD18(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BD1C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BD44(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BD84(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BDAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BDFC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BEEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007BEF0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C118(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C12C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C17C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C190(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C1F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C218(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C248(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C254(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C2A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C330(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C370(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C3C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C440(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C488(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C48C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4B8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C594(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C5F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C634(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C698(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C6E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C748(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C784(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C790(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C79C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7AC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7B0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7EC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C81C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C864(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C8AC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C8F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C94C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C950(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C988(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C9A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C9F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CA4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CACC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CBCC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CC0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CC90(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD1C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD98(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CDF0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CDF4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE1C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE20(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CF4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D028(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D0E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D160(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D1A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D25C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D260(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D264(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D268(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D26C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D270(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D380(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D3B4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D3D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D408(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D40C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D410(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D414(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D418(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D41C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D420(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D424(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D428(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D598(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6DC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D708(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D76C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D7A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D7E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D810(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D924(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D938(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D964(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D968(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D9D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D9E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DA6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAB0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAD0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB04(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB28(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB2C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB30(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB34(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB38(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB3C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DBD8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DC50(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DCD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DD60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DDEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE10(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE34(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEC4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DF0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DF30(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0BC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E128(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E174(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E194(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E200(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E204(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E288(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E28C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E354(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E358(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E3D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E51C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E548(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E574(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E624(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E650(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E67C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E744(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E748(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E74C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E798(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E79C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E818(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E85C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8B4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8C0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8C4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8DC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E904(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E92C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E93C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EA14(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EA70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EB70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EC70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007ECFC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007ED88(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EDDC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE40(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE80(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE90(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EF54(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EF9C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EFA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EFEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F088(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F0D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F2C0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F5CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F640(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F724(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F784(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F7D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F81C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F87C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F8A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FA60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCC0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FD0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FEAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FED8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FEEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF3C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80080054(MsgArg* pArgs, MsgArg* pResult);
void fn_8008017C(MsgArg* pArgs, MsgArg* pResult);
void fn_800801C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800801D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80080208(MsgArg* pArgs, MsgArg* pResult);
void fn_80080300(MsgArg* pArgs, MsgArg* pResult);
void fn_80080304(MsgArg* pArgs, MsgArg* pResult);
void fn_80080334(MsgArg* pArgs, MsgArg* pResult);
void fn_80080358(MsgArg* pArgs, MsgArg* pResult);
void fn_80080388(MsgArg* pArgs, MsgArg* pResult);
void fn_800804D8(MsgArg* pArgs, MsgArg* pResult);
void fn_800804E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8008052C(MsgArg* pArgs, MsgArg* pResult);
void fn_800805C4(MsgArg* pArgs, MsgArg* pResult);
void fn_800805F0(MsgArg* pArgs, MsgArg* pResult);
void fn_800805F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80080654(MsgArg* pArgs, MsgArg* pResult);
void fn_800807D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800807DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80080828(MsgArg* pArgs, MsgArg* pResult);
void fn_80080878(MsgArg* pArgs, MsgArg* pResult);
void fn_800809F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AA0(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080BB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C2C(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C60(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C74(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C84(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C98(MsgArg* pArgs, MsgArg* pResult);
void fn_80080CA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080CC8(MsgArg* pArgs, MsgArg* pResult);
void fn_800810BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800810D8(MsgArg* pArgs, MsgArg* pResult);
void fn_800810F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081158(MsgArg* pArgs, MsgArg* pResult);
void fn_800811E4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081270(MsgArg* pArgs, MsgArg* pResult);
void fn_800812B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800812F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081330(MsgArg* pArgs, MsgArg* pResult);
void fn_80081370(MsgArg* pArgs, MsgArg* pResult);
void fn_800813B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800813F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081430(MsgArg* pArgs, MsgArg* pResult);
void fn_80081470(MsgArg* pArgs, MsgArg* pResult);
void fn_800814B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800814F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081530(MsgArg* pArgs, MsgArg* pResult);
void fn_800815E0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081634(MsgArg* pArgs, MsgArg* pResult);
void fn_80081688(MsgArg* pArgs, MsgArg* pResult);
void fn_800816DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081718(MsgArg* pArgs, MsgArg* pResult);
void fn_80081754(MsgArg* pArgs, MsgArg* pResult);
void fn_80081790(MsgArg* pArgs, MsgArg* pResult);
void fn_800817CC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081808(MsgArg* pArgs, MsgArg* pResult);
void fn_80081844(MsgArg* pArgs, MsgArg* pResult);
void fn_80081880(MsgArg* pArgs, MsgArg* pResult);
void fn_800818BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800818F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081934(MsgArg* pArgs, MsgArg* pResult);
void fn_80081970(MsgArg* pArgs, MsgArg* pResult);
void fn_800819FC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081A54(MsgArg* pArgs, MsgArg* pResult);
void fn_80081B04(MsgArg* pArgs, MsgArg* pResult);
void fn_80081B50(MsgArg* pArgs, MsgArg* pResult);
void fn_80081BD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081BF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081C18(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CF8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081D50(MsgArg* pArgs, MsgArg* pResult);
void fn_80081DB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081F98(MsgArg* pArgs, MsgArg* pResult);
void fn_80081FA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081FBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082608(MsgArg* pArgs, MsgArg* pResult);
void fn_80082620(MsgArg* pArgs, MsgArg* pResult);
void fn_8008266C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082680(MsgArg* pArgs, MsgArg* pResult);
void fn_800826C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80082708(MsgArg* pArgs, MsgArg* pResult);
void fn_80082758(MsgArg* pArgs, MsgArg* pResult);
void fn_80082790(MsgArg* pArgs, MsgArg* pResult);
void fn_800827D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80082800(MsgArg* pArgs, MsgArg* pResult);
void fn_8008281C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082828(MsgArg* pArgs, MsgArg* pResult);
void fn_80082928(MsgArg* pArgs, MsgArg* pResult);
void fn_8008293C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082978(MsgArg* pArgs, MsgArg* pResult);
void fn_8008297C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082980(MsgArg* pArgs, MsgArg* pResult);
void fn_800829D4(MsgArg* pArgs, MsgArg* pResult);
void fn_800829E0(MsgArg* pArgs, MsgArg* pResult);
void fn_800829EC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A10(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A44(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A48(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A50(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A94(MsgArg* pArgs, MsgArg* pResult);
void fn_80082C74(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CDC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D14(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D3C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D98(MsgArg* pArgs, MsgArg* pResult);
void fn_80082DA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80082E5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082F68(MsgArg* pArgs, MsgArg* pResult);
void fn_80083068(MsgArg* pArgs, MsgArg* pResult);
void fn_8008311C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083354(MsgArg* pArgs, MsgArg* pResult);
void fn_80083358(MsgArg* pArgs, MsgArg* pResult);
void fn_800833A4(MsgArg* pArgs, MsgArg* pResult);
void fn_800833C4(MsgArg* pArgs, MsgArg* pResult);
void fn_800833D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800833F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083414(MsgArg* pArgs, MsgArg* pResult);
void fn_80083430(MsgArg* pArgs, MsgArg* pResult);
void fn_80083480(MsgArg* pArgs, MsgArg* pResult);
void fn_80083494(MsgArg* pArgs, MsgArg* pResult);
void fn_800834DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083550(MsgArg* pArgs, MsgArg* pResult);
void fn_800835B8(MsgArg* pArgs, MsgArg* pResult);
void fn_800835BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800835C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800835D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083658(MsgArg* pArgs, MsgArg* pResult);
void fn_80083860(MsgArg* pArgs, MsgArg* pResult);
void fn_80083890(MsgArg* pArgs, MsgArg* pResult);
void fn_8008389C(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A4(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800838AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B8(MsgArg* pArgs, MsgArg* pResult);
void fn_800838BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800838C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083904(MsgArg* pArgs, MsgArg* pResult);
void fn_80083908(MsgArg* pArgs, MsgArg* pResult);
void fn_8008390C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083910(MsgArg* pArgs, MsgArg* pResult);
void fn_80083914(MsgArg* pArgs, MsgArg* pResult);
void fn_80083918(MsgArg* pArgs, MsgArg* pResult);
void fn_8008391C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083920(MsgArg* pArgs, MsgArg* pResult);
void fn_80083924(MsgArg* pArgs, MsgArg* pResult);
void fn_80083928(MsgArg* pArgs, MsgArg* pResult);
void fn_8008392C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083930(MsgArg* pArgs, MsgArg* pResult);
void fn_80083934(MsgArg* pArgs, MsgArg* pResult);
void fn_80083964(MsgArg* pArgs, MsgArg* pResult);
void fn_80083970(MsgArg* pArgs, MsgArg* pResult);
void fn_80083974(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A44(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A48(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BC8(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083D88(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E48(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E70(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E94(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EE0(MsgArg* pArgs, MsgArg* pResult);
void fn_80083F54(MsgArg* pArgs, MsgArg* pResult);
void fn_80083F60(MsgArg* pArgs, MsgArg* pResult);
void fn_80084008(MsgArg* pArgs, MsgArg* pResult);
void fn_8008410C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084158(MsgArg* pArgs, MsgArg* pResult);
void fn_8008415C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084160(MsgArg* pArgs, MsgArg* pResult);
void fn_80084190(MsgArg* pArgs, MsgArg* pResult);
void fn_800841C0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084208(MsgArg* pArgs, MsgArg* pResult);
void fn_8008422C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084258(MsgArg* pArgs, MsgArg* pResult);
void fn_80084288(MsgArg* pArgs, MsgArg* pResult);
void fn_800842AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800842D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084354(MsgArg* pArgs, MsgArg* pResult);
void fn_80084458(MsgArg* pArgs, MsgArg* pResult);
void fn_8008449C(MsgArg* pArgs, MsgArg* pResult);
void fn_800844E0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084544(MsgArg* pArgs, MsgArg* pResult);
void fn_80084578(MsgArg* pArgs, MsgArg* pResult);
void fn_800845D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084614(MsgArg* pArgs, MsgArg* pResult);
void fn_80084678(MsgArg* pArgs, MsgArg* pResult);
void fn_800846C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800846D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084704(MsgArg* pArgs, MsgArg* pResult);
void fn_80084750(MsgArg* pArgs, MsgArg* pResult);
void fn_80084754(MsgArg* pArgs, MsgArg* pResult);
void fn_8008478C(MsgArg* pArgs, MsgArg* pResult);
void fn_800847BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800847E0(MsgArg* pArgs, MsgArg* pResult);
void fn_800848E4(MsgArg* pArgs, MsgArg* pResult);
void fn_800848E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084918(MsgArg* pArgs, MsgArg* pResult);
void fn_80084940(MsgArg* pArgs, MsgArg* pResult);
void fn_80084984(MsgArg* pArgs, MsgArg* pResult);
void fn_800849C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800849F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084AA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084B88(MsgArg* pArgs, MsgArg* pResult);
void fn_80084BE4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084BE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084C88(MsgArg* pArgs, MsgArg* pResult);
void fn_80084CFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80084D6C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084DF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084E7C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084E90(MsgArg* pArgs, MsgArg* pResult);
void fn_80084EC8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F04(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F08(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F0C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F3C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F40(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F84(MsgArg* pArgs, MsgArg* pResult);

// The handlers, by message number (fn_80079EA8 fills it).
#define FE_NUM_MESSAGES 770
MsgHandler lbl_801D77A8[FE_NUM_MESSAGES];

// Run message nMsg's handler.
void fn_80079E6C(int nMsg, MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D77A8[nMsg](pArgs, pResult);
}

// Fill the table. Messages with no handler stay NULL.
void fn_80079EA8(void) {
    memset(lbl_801D77A8, 0, sizeof(lbl_801D77A8));
    lbl_801D77A8[1] = NULL;
    lbl_801D77A8[2] = fn_8007BBA0;
    lbl_801D77A8[3] = fn_8007BCA4;
    lbl_801D77A8[4] = fn_8007BBD8;
    lbl_801D77A8[5] = fn_8007BC48;
    lbl_801D77A8[6] = fn_8007BC10;
    lbl_801D77A8[7] = fn_8007BC74;
    lbl_801D77A8[8] = fn_8007BCA0;
    lbl_801D77A8[9] = fn_8007BCC4;
    lbl_801D77A8[10] = fn_8007BD18;
    lbl_801D77A8[12] = fn_8007BD1C;
    lbl_801D77A8[13] = fn_8007BD44;
    lbl_801D77A8[14] = fn_8007BD84;
    lbl_801D77A8[15] = fn_8007BDAC;
    lbl_801D77A8[16] = fn_8007BDFC;
    lbl_801D77A8[17] = fn_8007BEEC;
    lbl_801D77A8[18] = fn_8007BEF0;
    lbl_801D77A8[19] = fn_8007C118;
    lbl_801D77A8[20] = fn_8007C12C;
    lbl_801D77A8[21] = fn_8007C17C;
    lbl_801D77A8[22] = fn_8007C190;
    lbl_801D77A8[23] = fn_8007C1F8;
    lbl_801D77A8[24] = fn_8007C218;
    lbl_801D77A8[25] = fn_8007C248;
    lbl_801D77A8[26] = fn_8007C254;
    lbl_801D77A8[27] = fn_8007C2A0;
    lbl_801D77A8[28] = fn_8007C330;
    lbl_801D77A8[29] = fn_8007C370;
    lbl_801D77A8[30] = fn_8007C440;
    lbl_801D77A8[31] = fn_8007C488;
    lbl_801D77A8[32] = fn_8007C48C;
    lbl_801D77A8[33] = fn_8007C4B8;
    lbl_801D77A8[34] = fn_8007C4D8;
    lbl_801D77A8[35] = fn_8007C4F8;
    lbl_801D77A8[36] = fn_8007C594;
    lbl_801D77A8[37] = fn_8007C5F0;
    lbl_801D77A8[38] = fn_8007C634;
    lbl_801D77A8[39] = fn_8007C784;
    lbl_801D77A8[40] = fn_8007C698;
    lbl_801D77A8[41] = fn_8007C6E4;
    lbl_801D77A8[42] = fn_8007C748;
    lbl_801D77A8[43] = fn_8007C790;
    lbl_801D77A8[44] = fn_8007C79C;
    lbl_801D77A8[45] = fn_8007C7AC;
    lbl_801D77A8[46] = fn_801217C4;
    lbl_801D77A8[47] = fn_8007C7B0;
    lbl_801D77A8[48] = fn_8007C7EC;
    lbl_801D77A8[49] = fn_8007C81C;
    lbl_801D77A8[50] = fn_8007C8AC;
    lbl_801D77A8[51] = fn_8007C8F0;
    lbl_801D77A8[52] = fn_8007C950;
    lbl_801D77A8[53] = fn_8007C988;
    lbl_801D77A8[54] = fn_8007C9A4;
    lbl_801D77A8[537] = fn_8007C9F8;
    lbl_801D77A8[55] = fn_8007CA4C;
    lbl_801D77A8[56] = fn_8007CACC;
    lbl_801D77A8[57] = fn_8007CBCC;
    lbl_801D77A8[58] = fn_8007CC0C;
    lbl_801D77A8[59] = fn_8007CC90;
    lbl_801D77A8[60] = fn_8007CD1C;
    lbl_801D77A8[61] = fn_8007CD58;
    lbl_801D77A8[62] = fn_8007CD98;
    lbl_801D77A8[63] = fn_8007CDF4;
    lbl_801D77A8[64] = fn_8007CE1C;
    lbl_801D77A8[65] = fn_8007CE20;
    lbl_801D77A8[66] = fn_8007CE58;
    lbl_801D77A8[67] = fn_8007CE7C;
    lbl_801D77A8[68] = fn_8007CF4C;
    lbl_801D77A8[69] = fn_8007D0E0;
    lbl_801D77A8[70] = fn_8007D160;
    lbl_801D77A8[71] = fn_8007D1A4;
    lbl_801D77A8[72] = fn_8007D380;
    lbl_801D77A8[73] = fn_8007D3B4;
    lbl_801D77A8[74] = fn_8007D3D8;
    lbl_801D77A8[75] = fn_8007D41C;
    lbl_801D77A8[76] = fn_8007D420;
    lbl_801D77A8[77] = fn_8007D424;
    lbl_801D77A8[78] = fn_8007D428;
    lbl_801D77A8[79] = fn_8007D598;
    lbl_801D77A8[80] = fn_8007D6D8;
    lbl_801D77A8[81] = fn_8007D6E0;
    lbl_801D77A8[82] = fn_8007D708;
    lbl_801D77A8[83] = fn_8007D76C;
    lbl_801D77A8[84] = fn_8007D7A0;
    lbl_801D77A8[85] = fn_8007D7E4;
    lbl_801D77A8[86] = fn_8007D810;
    lbl_801D77A8[87] = fn_8007D924;
    lbl_801D77A8[88] = fn_8007D938;
    lbl_801D77A8[89] = fn_8007D964;
    lbl_801D77A8[90] = fn_8007D968;
    lbl_801D77A8[91] = fn_8007D9D0;
    lbl_801D77A8[92] = fn_8007DA6C;
    lbl_801D77A8[94] = fn_8007DAD0;
    lbl_801D77A8[95] = fn_8007DAD4;
    lbl_801D77A8[96] = fn_8007DAE8;
    lbl_801D77A8[97] = fn_8007DB04;
    lbl_801D77A8[98] = fn_8007DB28;
    lbl_801D77A8[99] = fn_8007DB2C;
    lbl_801D77A8[100] = fn_8007DB30;
    lbl_801D77A8[101] = fn_8007DB34;
    lbl_801D77A8[102] = fn_8007DB38;
    lbl_801D77A8[103] = fn_8007DB3C;
    lbl_801D77A8[104] = fn_8007DB60;
    lbl_801D77A8[105] = fn_8007DBD8;
    lbl_801D77A8[106] = fn_8007DC50;
    lbl_801D77A8[107] = fn_8007DCD4;
    lbl_801D77A8[108] = fn_8007DD60;
    lbl_801D77A8[109] = fn_8007DDEC;
    lbl_801D77A8[110] = fn_8007DE10;
    lbl_801D77A8[111] = fn_8007DE34;
    lbl_801D77A8[112] = fn_8007DE58;
    lbl_801D77A8[113] = fn_8007DE7C;
    lbl_801D77A8[114] = fn_8007DEA0;
    lbl_801D77A8[115] = fn_8007DEC4;
    lbl_801D77A8[116] = fn_8007DEE8;
    lbl_801D77A8[117] = fn_8007DF0C;
    lbl_801D77A8[118] = fn_8007DF30;
    lbl_801D77A8[119] = fn_8007E0BC;
    lbl_801D77A8[120] = fn_8007E0D0;
    lbl_801D77A8[121] = fn_8007E0F8;
    lbl_801D77A8[122] = fn_8007E174;
    lbl_801D77A8[123] = fn_8007E194;
    lbl_801D77A8[124] = fn_8007E200;
    lbl_801D77A8[125] = fn_8007E204;
    lbl_801D77A8[126] = fn_8007E288;
    lbl_801D77A8[127] = fn_8007E28C;
    lbl_801D77A8[128] = fn_8007E358;
    lbl_801D77A8[129] = fn_8007E3D4;
    lbl_801D77A8[130] = fn_8007E51C;
    lbl_801D77A8[131] = fn_8007E548;
    lbl_801D77A8[132] = fn_8007E574;
    lbl_801D77A8[133] = fn_8007E5A0;
    lbl_801D77A8[134] = fn_8007E5CC;
    lbl_801D77A8[135] = fn_8007E5F8;
    lbl_801D77A8[136] = fn_8007E624;
    lbl_801D77A8[137] = fn_8007E650;
    lbl_801D77A8[138] = fn_8007E67C;
    lbl_801D77A8[139] = fn_8007E744;
    lbl_801D77A8[140] = fn_8007E748;
    lbl_801D77A8[141] = fn_8007E74C;
    lbl_801D77A8[142] = fn_8007E798;
    lbl_801D77A8[143] = fn_8007E79C;
    lbl_801D77A8[144] = fn_8007E818;
    lbl_801D77A8[145] = fn_8007E85C;
    lbl_801D77A8[146] = fn_8007E8B4;
    lbl_801D77A8[147] = fn_8007E8C0;
    lbl_801D77A8[148] = fn_8007E8C4;
    lbl_801D77A8[149] = fn_8007E8DC;
    lbl_801D77A8[150] = fn_8007E8F0;
    lbl_801D77A8[152] = fn_8007E92C;
    lbl_801D77A8[153] = fn_8007E93C;
    lbl_801D77A8[154] = fn_8007E9A0;
    lbl_801D77A8[155] = fn_8007E9A4;
    lbl_801D77A8[156] = fn_8007E9A8;
    lbl_801D77A8[157] = fn_8007E9BC;
    lbl_801D77A8[158] = fn_8007EA14;
    lbl_801D77A8[159] = fn_8007EA70;
    lbl_801D77A8[160] = fn_8007EB70;
    lbl_801D77A8[161] = fn_8007EC70;
    lbl_801D77A8[162] = fn_8007ECFC;
    lbl_801D77A8[163] = fn_8007ED88;
    lbl_801D77A8[164] = fn_8007EDDC;
    lbl_801D77A8[165] = fn_8007EE40;
    lbl_801D77A8[166] = fn_8007EE7C;
    lbl_801D77A8[167] = fn_8007EE90;
    lbl_801D77A8[168] = fn_8007EF54;
    lbl_801D77A8[169] = fn_8007EF9C;
    lbl_801D77A8[170] = fn_8007EFA0;
    lbl_801D77A8[171] = fn_8007F088;
    lbl_801D77A8[172] = fn_8007F2C0;
    lbl_801D77A8[173] = fn_8007F5CC;
    lbl_801D77A8[174] = fn_8007F640;
    lbl_801D77A8[175] = fn_8007F724;
    lbl_801D77A8[176] = fn_8007F784;
    lbl_801D77A8[177] = fn_8007F7D0;
    lbl_801D77A8[178] = fn_8007F81C;
    lbl_801D77A8[179] = fn_8007F87C;
    lbl_801D77A8[180] = fn_8007F8A0;
    lbl_801D77A8[181] = fn_8007FA60;
    lbl_801D77A8[182] = fn_8007FCC0;
    lbl_801D77A8[183] = fn_8007FCD4;
    lbl_801D77A8[184] = fn_8007FCE8;
    lbl_801D77A8[185] = fn_8007FD0C;
    lbl_801D77A8[186] = fn_8007FEAC;
    lbl_801D77A8[187] = fn_8007FED8;
    lbl_801D77A8[188] = fn_8007FEEC;
    lbl_801D77A8[189] = fn_8007FF3C;
    lbl_801D77A8[190] = fn_8007FF4C;
    lbl_801D77A8[191] = fn_8007FF6C;
    lbl_801D77A8[192] = fn_8007FF8C;
    lbl_801D77A8[193] = fn_80080054;
    lbl_801D77A8[194] = fn_8008017C;
    lbl_801D77A8[195] = fn_800801C0;
    lbl_801D77A8[196] = fn_800801D4;
    lbl_801D77A8[197] = fn_80080208;
    lbl_801D77A8[198] = fn_80080300;
    lbl_801D77A8[199] = fn_80080304;
    lbl_801D77A8[200] = fn_80080334;
    lbl_801D77A8[201] = fn_80080358;
    lbl_801D77A8[202] = fn_80080388;
    lbl_801D77A8[203] = fn_800804D8;
    lbl_801D77A8[204] = fn_800804E4;
    lbl_801D77A8[205] = fn_800805C4;
    lbl_801D77A8[206] = fn_800805F0;
    lbl_801D77A8[207] = fn_800805F4;
    lbl_801D77A8[208] = fn_80080654;
    lbl_801D77A8[209] = fn_800807D0;
    lbl_801D77A8[151] = fn_8007E904;
    lbl_801D77A8[93] = fn_8007DAB0;
    lbl_801D77A8[210] = fn_800807DC;
    lbl_801D77A8[211] = fn_80080828;
    lbl_801D77A8[212] = fn_80080878;
    lbl_801D77A8[213] = fn_800809F8;
    lbl_801D77A8[214] = fn_80080AA0;
    lbl_801D77A8[215] = fn_80080AD0;
    lbl_801D77A8[216] = fn_8007D410;
    lbl_801D77A8[217] = fn_8007D40C;
    lbl_801D77A8[218] = fn_8007D408;
    lbl_801D77A8[219] = fn_80080AE8;
    lbl_801D77A8[220] = fn_80080BB8;
    lbl_801D77A8[221] = fn_80080C2C;
    lbl_801D77A8[222] = fn_8007D414;
    lbl_801D77A8[223] = fn_80080C60;
    lbl_801D77A8[224] = fn_80080C74;
    lbl_801D77A8[225] = fn_80080C84;
    lbl_801D77A8[226] = fn_80080C98;
    lbl_801D77A8[227] = fn_80080CA8;
    lbl_801D77A8[228] = fn_80080CC8;
    lbl_801D77A8[229] = fn_800810BC;
    lbl_801D77A8[230] = fn_800810D8;
    lbl_801D77A8[231] = fn_800810F4;
    lbl_801D77A8[232] = fn_80081158;
    lbl_801D77A8[233] = fn_800812B0;
    lbl_801D77A8[234] = fn_80081330;
    lbl_801D77A8[235] = fn_80081370;
    lbl_801D77A8[236] = fn_800813B0;
    lbl_801D77A8[237] = fn_800813F0;
    lbl_801D77A8[238] = fn_80081430;
    lbl_801D77A8[239] = fn_80081470;
    lbl_801D77A8[240] = fn_800814B0;
    lbl_801D77A8[241] = fn_800814F0;
    lbl_801D77A8[242] = fn_80081530;
    lbl_801D77A8[243] = fn_800815E0;
    lbl_801D77A8[244] = fn_80081634;
    lbl_801D77A8[245] = fn_80081718;
    lbl_801D77A8[246] = fn_80081790;
    lbl_801D77A8[247] = fn_800817CC;
    lbl_801D77A8[248] = fn_80081808;
    lbl_801D77A8[249] = fn_80081844;
    lbl_801D77A8[250] = fn_80081880;
    lbl_801D77A8[251] = fn_800818BC;
    lbl_801D77A8[252] = fn_800818F8;
    lbl_801D77A8[253] = fn_80081934;
    lbl_801D77A8[254] = fn_80081970;
    lbl_801D77A8[255] = fn_800819FC;
    lbl_801D77A8[256] = fn_80081A54;
    lbl_801D77A8[257] = fn_80081B04;
    lbl_801D77A8[258] = fn_80081B50;
    lbl_801D77A8[259] = fn_80081BD4;
    lbl_801D77A8[260] = fn_80081BF4;
    lbl_801D77A8[261] = fn_80081C18;
    lbl_801D77A8[262] = fn_80081CA4;
    lbl_801D77A8[263] = fn_80081CF8;
    lbl_801D77A8[264] = fn_80081CFC;
    lbl_801D77A8[265] = fn_80081D50;
    lbl_801D77A8[266] = fn_80081DB8;
    lbl_801D77A8[267] = fn_80081F98;
    lbl_801D77A8[268] = fn_80081FA8;
    lbl_801D77A8[269] = fn_80081FBC;
    lbl_801D77A8[270] = fn_80082608;
    lbl_801D77A8[271] = fn_80082620;
    lbl_801D77A8[272] = fn_8008266C;
    lbl_801D77A8[273] = fn_80082680;
    lbl_801D77A8[274] = fn_80082708;
    lbl_801D77A8[275] = fn_80082758;
    lbl_801D77A8[276] = fn_80082790;
    lbl_801D77A8[277] = fn_800827D0;
    lbl_801D77A8[278] = fn_80082800;
    lbl_801D77A8[279] = fn_8008281C;
    lbl_801D77A8[280] = fn_80082828;
    lbl_801D77A8[281] = fn_80082928;
    lbl_801D77A8[282] = fn_8008293C;
    lbl_801D77A8[283] = fn_8007D2D4;
    lbl_801D77A8[284] = fn_80082978;
    lbl_801D77A8[285] = fn_8008297C;
    lbl_801D77A8[286] = fn_8007EE80;
    lbl_801D77A8[287] = fn_8007D25C;
    lbl_801D77A8[288] = fn_80082980;
    lbl_801D77A8[289] = fn_8008299C;
    lbl_801D77A8[290] = fn_800829D4;
    lbl_801D77A8[291] = fn_800829E0;
    lbl_801D77A8[292] = fn_8007D268;
    lbl_801D77A8[293] = fn_8007D028;
    lbl_801D77A8[294] = fn_800829EC;
    lbl_801D77A8[295] = fn_80082A10;
    lbl_801D77A8[296] = fn_80082A44;
    lbl_801D77A8[297] = fn_80082A48;
    lbl_801D77A8[298] = fn_80082A4C;
    lbl_801D77A8[299] = fn_8007D260;
    lbl_801D77A8[300] = fn_80082A50;
    lbl_801D77A8[301] = fn_8007D418;
    lbl_801D77A8[302] = fn_8007D264;
    lbl_801D77A8[303] = fn_8007D270;
    lbl_801D77A8[304] = fn_80082A94;
    lbl_801D77A8[305] = fn_8007E354;
    lbl_801D77A8[306] = fn_80082C74;
    lbl_801D77A8[307] = fn_80082CA4;
    lbl_801D77A8[308] = fn_80082CA8;
    lbl_801D77A8[309] = fn_80082CDC;
    lbl_801D77A8[310] = fn_80082D14;
    lbl_801D77A8[311] = fn_80082D3C;
    lbl_801D77A8[312] = fn_8007D26C;
    lbl_801D77A8[313] = fn_8007D2A4;
    lbl_801D77A8[314] = fn_8007D2D0;
    lbl_801D77A8[315] = fn_80082D98;
    lbl_801D77A8[316] = fn_80082DA8;
    lbl_801D77A8[317] = fn_80082DBC;
    lbl_801D77A8[318] = fn_80082E10;
    lbl_801D77A8[319] = fn_80082E5C;
    lbl_801D77A8[320] = fn_80082F68;
    lbl_801D77A8[321] = fn_80083068;
    lbl_801D77A8[322] = fn_8007D6DC;
    lbl_801D77A8[323] = fn_8008311C;
    lbl_801D77A8[324] = fn_80083354;
    lbl_801D77A8[325] = fn_8007CDF0;
    lbl_801D77A8[326] = fn_8007C94C;
    lbl_801D77A8[327] = fn_80083358;
    lbl_801D77A8[328] = fn_800833C4;
    lbl_801D77A8[329] = fn_800833D0;
    lbl_801D77A8[330] = fn_800833F4;
    lbl_801D77A8[331] = fn_80083414;
    lbl_801D77A8[332] = fn_80083430;
    lbl_801D77A8[333] = fn_80083480;
    lbl_801D77A8[334] = fn_80083494;
    lbl_801D77A8[335] = fn_800834A8;
    lbl_801D77A8[336] = fn_800834DC;
    lbl_801D77A8[337] = fn_800834E8;
    lbl_801D77A8[766] = fn_80083550;
    lbl_801D77A8[338] = fn_800835B8;
    lbl_801D77A8[339] = fn_800835BC;
    lbl_801D77A8[340] = fn_800835C8;
    lbl_801D77A8[341] = fn_800835D4;
    lbl_801D77A8[342] = fn_80083658;
    lbl_801D77A8[343] = fn_80083860;
    lbl_801D77A8[344] = fn_80083890;
    lbl_801D77A8[345] = fn_8008389C;
    lbl_801D77A8[346] = fn_800838A0;
    lbl_801D77A8[347] = fn_800838A4;
    lbl_801D77A8[348] = fn_800838A8;
    lbl_801D77A8[349] = fn_800838AC;
    lbl_801D77A8[350] = fn_800838B0;
    lbl_801D77A8[351] = fn_800838B4;
    lbl_801D77A8[352] = fn_800838B8;
    lbl_801D77A8[353] = fn_800838BC;
    lbl_801D77A8[354] = fn_800838C0;
    lbl_801D77A8[355] = fn_800838C4;
    lbl_801D77A8[356] = fn_80083904;
    lbl_801D77A8[357] = fn_80083908;
    lbl_801D77A8[358] = fn_8008390C;
    lbl_801D77A8[359] = fn_80083910;
    lbl_801D77A8[360] = fn_80083914;
    lbl_801D77A8[361] = fn_80083918;
    lbl_801D77A8[362] = fn_8008391C;
    lbl_801D77A8[363] = fn_80083920;
    lbl_801D77A8[364] = fn_80083924;
    lbl_801D77A8[365] = fn_80083928;
    lbl_801D77A8[366] = fn_8008392C;
    lbl_801D77A8[367] = fn_80083930;
    lbl_801D77A8[368] = fn_80083934;
    lbl_801D77A8[369] = fn_80083964;
    lbl_801D77A8[370] = fn_80083970;
    lbl_801D77A8[371] = fn_80083974;
    lbl_801D77A8[372] = fn_80083A44;
    lbl_801D77A8[374] = fn_80081688;
    lbl_801D77A8[375] = fn_800811E4;
    lbl_801D77A8[380] = fn_80083A48;
    lbl_801D77A8[381] = fn_801077EC;
    lbl_801D77A8[382] = fn_80107828;
    lbl_801D77A8[383] = fn_801078B8;
    lbl_801D77A8[399] = fn_80083A4C;
    lbl_801D77A8[404] = fn_80107994;
    lbl_801D77A8[405] = fn_80107998;
    lbl_801D77A8[406] = fn_8010799C;
    lbl_801D77A8[407] = fn_80107A2C;
    lbl_801D77A8[408] = fn_80107BA4;
    lbl_801D77A8[409] = fn_80107BA8;
    lbl_801D77A8[410] = fn_80107BF4;
    lbl_801D77A8[411] = fn_80107C30;
    lbl_801D77A8[412] = fn_80107C3C;
    lbl_801D77A8[451] = fn_80107DBC;
    lbl_801D77A8[452] = fn_80107EB0;
    lbl_801D77A8[413] = fn_80107FAC;
    lbl_801D77A8[441] = fn_80108140;
    lbl_801D77A8[453] = fn_80108178;
    lbl_801D77A8[454] = fn_80108244;
    lbl_801D77A8[457] = fn_80108300;
    lbl_801D77A8[460] = fn_80108314;
    lbl_801D77A8[461] = fn_80108398;
    lbl_801D77A8[462] = fn_8010840C;
    lbl_801D77A8[470] = fn_80108494;
    lbl_801D77A8[474] = fn_801084F4;
    lbl_801D77A8[475] = fn_80108594;
    lbl_801D77A8[476] = fn_80108690;
    lbl_801D77A8[477] = fn_80108768;
    lbl_801D77A8[498] = fn_80108A0C;
    lbl_801D77A8[499] = fn_80108B10;
    lbl_801D77A8[500] = fn_80108B84;
    lbl_801D77A8[501] = fn_80108C00;
    lbl_801D77A8[503] = fn_80108DF4;
    lbl_801D77A8[504] = fn_80108E4C;
    lbl_801D77A8[507] = fn_80108E9C;
    lbl_801D77A8[509] = fn_801090B4;
    lbl_801D77A8[512] = fn_80109120;
    lbl_801D77A8[515] = fn_80109304;
    lbl_801D77A8[527] = fn_80109354;
    lbl_801D77A8[528] = fn_80109388;
    lbl_801D77A8[529] = fn_801093B4;
    lbl_801D77A8[534] = fn_80109430;
    lbl_801D77A8[535] = fn_80109434;
    lbl_801D77A8[538] = fn_80109458;
    lbl_801D77A8[539] = fn_801094FC;
    lbl_801D77A8[542] = fn_80109500;
    lbl_801D77A8[547] = fn_80109CBC;
    lbl_801D77A8[554] = fn_80109D5C;
    lbl_801D77A8[557] = fn_80108D1C;
    lbl_801D77A8[562] = fn_8010948C;
    lbl_801D77A8[687] = fn_801094C0;
    lbl_801D77A8[566] = fn_80109DDC;
    lbl_801D77A8[458] = fn_8010E748;
    lbl_801D77A8[459] = fn_8010E85C;
    lbl_801D77A8[466] = fn_8010E890;
    lbl_801D77A8[467] = fn_8010EA24;
    lbl_801D77A8[520] = fn_8010EAC4;
    lbl_801D77A8[551] = fn_8010EB9C;
    lbl_801D77A8[552] = fn_8010EBDC;
    lbl_801D77A8[553] = fn_8010EEA8;
    lbl_801D77A8[559] = fn_8010EEE4;
    lbl_801D77A8[560] = fn_8010EF80;
    lbl_801D77A8[563] = fn_8010EF8C;
    lbl_801D77A8[742] = fn_8010F52C;
    lbl_801D77A8[743] = fn_8010F550;
    lbl_801D77A8[472] = fn_80083BFC;
    lbl_801D77A8[465] = fn_80083BC8;
    lbl_801D77A8[473] = fn_80083D88;
    lbl_801D77A8[468] = fn_80111F58;
    lbl_801D77A8[469] = fn_80112020;
    lbl_801D77A8[483] = fn_8011206C;
    lbl_801D77A8[484] = fn_80112130;
    lbl_801D77A8[485] = fn_80112154;
    lbl_801D77A8[486] = fn_801121D4;
    lbl_801D77A8[487] = fn_80112254;
    lbl_801D77A8[488] = fn_80112290;
    lbl_801D77A8[496] = fn_801122D8;
    lbl_801D77A8[497] = fn_80112384;
    lbl_801D77A8[511] = fn_80112438;
    lbl_801D77A8[519] = fn_8011243C;
    lbl_801D77A8[549] = fn_8011247C;
    lbl_801D77A8[521] = fn_8011CF64;
    lbl_801D77A8[522] = fn_8011D05C;
    lbl_801D77A8[523] = fn_8011D09C;
    lbl_801D77A8[524] = fn_8011D0A8;
    lbl_801D77A8[525] = fn_8011D268;
    lbl_801D77A8[526] = fn_8011D274;
    lbl_801D77A8[489] = fn_801088A4;
    lbl_801D77A8[490] = fn_801088C8;
    lbl_801D77A8[491] = fn_80108904;
    lbl_801D77A8[492] = fn_8010893C;
    lbl_801D77A8[493] = fn_801089BC;
    lbl_801D77A8[494] = fn_801089DC;
    lbl_801D77A8[502] = fn_80108CA8;
    lbl_801D77A8[513] = fn_801091B8;
    lbl_801D77A8[514] = fn_8010920C;
    lbl_801D77A8[583] = fn_80109618;
    lbl_801D77A8[584] = fn_80109650;
    lbl_801D77A8[518] = fn_80083BA4;
    lbl_801D77A8[536] = fn_8011DDFC;
    lbl_801D77A8[544] = fn_8011DEF0;
    lbl_801D77A8[543] = fn_80109514;
    lbl_801D77A8[587] = fn_8010969C;
    lbl_801D77A8[588] = fn_80109700;
    lbl_801D77A8[598] = fn_801098AC;
    lbl_801D77A8[565] = fn_8007D9E4;
    lbl_801D77A8[567] = fn_8007C3C8;
    lbl_801D77A8[568] = fn_80112548;
    lbl_801D77A8[569] = fn_80121430;
    lbl_801D77A8[573] = fn_80121458;
    lbl_801D77A8[575] = fn_8012153C;
    lbl_801D77A8[576] = fn_8012168C;
    lbl_801D77A8[579] = fn_8012172C;
    lbl_801D77A8[580] = fn_80121770;
    lbl_801D77A8[592] = fn_80083E48;
    lbl_801D77A8[593] = fn_80083E70;
    lbl_801D77A8[594] = fn_80083E94;
    lbl_801D77A8[595] = fn_80083EB8;
    lbl_801D77A8[596] = fn_80121808;
    lbl_801D77A8[599] = fn_800816DC;
    lbl_801D77A8[600] = fn_80081270;
    lbl_801D77A8[605] = fn_8012597C;
    lbl_801D77A8[606] = fn_80125A24;
    lbl_801D77A8[608] = fn_8007F0D0;
    lbl_801D77A8[609] = fn_80109734;
    lbl_801D77A8[610] = fn_8007E128;
    lbl_801D77A8[611] = fn_80083EBC;
    lbl_801D77A8[612] = fn_80083F60;
    lbl_801D77A8[623] = fn_80084008;
    lbl_801D77A8[624] = fn_8008410C;
    lbl_801D77A8[630] = fn_80083F54;
    lbl_801D77A8[629] = fn_80083EE0;
    lbl_801D77A8[633] = fn_80084160;
    lbl_801D77A8[653] = fn_80084190;
    lbl_801D77A8[654] = fn_800841C0;
    lbl_801D77A8[748] = fn_80084208;
    lbl_801D77A8[618] = fn_80084158;
    lbl_801D77A8[619] = fn_8008415C;
    lbl_801D77A8[607] = fn_80125AA4;
    lbl_801D77A8[613] = fn_80125B38;
    lbl_801D77A8[614] = fn_8008422C;
    lbl_801D77A8[615] = fn_80084258;
    lbl_801D77A8[616] = fn_80084288;
    lbl_801D77A8[617] = fn_800842AC;
    lbl_801D77A8[620] = fn_8008052C;
    lbl_801D77A8[621] = fn_80109248;
    lbl_801D77A8[622] = fn_80109294;
    lbl_801D77A8[627] = fn_800842D0;
    lbl_801D77A8[628] = fn_80084354;
    lbl_801D77A8[632] = fn_800826C4;
    lbl_801D77A8[637] = fn_80125BB8;
    lbl_801D77A8[638] = fn_80125BD8;
    lbl_801D77A8[640] = fn_80125C5C;
    lbl_801D77A8[641] = fn_80125D08;
    lbl_801D77A8[643] = fn_80084458;
    lbl_801D77A8[644] = fn_8008449C;
    lbl_801D77A8[645] = fn_800844E0;
    lbl_801D77A8[646] = fn_80084544;
    lbl_801D77A8[647] = fn_80084578;
    lbl_801D77A8[648] = fn_800845D4;
    lbl_801D77A8[649] = fn_80084614;
    lbl_801D77A8[650] = fn_80084678;
    lbl_801D77A8[655] = fn_800846C8;
    lbl_801D77A8[657] = fn_80109738;
    lbl_801D77A8[658] = fn_80084704;
    lbl_801D77A8[663] = fn_80109760;
    lbl_801D77A8[664] = fn_80109780;
    lbl_801D77A8[665] = fn_801097FC;
    lbl_801D77A8[666] = fn_8008478C;
    lbl_801D77A8[667] = fn_80084750;
    lbl_801D77A8[668] = fn_80084754;
    lbl_801D77A8[671] = fn_800847BC;
    lbl_801D77A8[672] = fn_800847E0;
    lbl_801D77A8[670] = fn_80125D78;
    lbl_801D77A8[674] = fn_80125DE0;
    lbl_801D77A8[675] = fn_800848E4;
    lbl_801D77A8[678] = fn_800848E8;
    lbl_801D77A8[679] = fn_80084918;
    lbl_801D77A8[680] = fn_80084940;
    lbl_801D77A8[681] = fn_80084984;
    lbl_801D77A8[682] = fn_800849C8;
    lbl_801D77A8[683] = fn_8010F10C;
    lbl_801D77A8[684] = fn_800849F8;
    lbl_801D77A8[685] = fn_8010988C;
    lbl_801D77A8[688] = fn_800846D4;
    lbl_801D77A8[689] = fn_801098B0;
    lbl_801D77A8[690] = fn_80109BA4;
    lbl_801D77A8[686] = fn_8007C864;
    lbl_801D77A8[691] = fn_8012185C;
    lbl_801D77A8[692] = fn_80084AA8;
    lbl_801D77A8[693] = fn_80112580;
    lbl_801D77A8[694] = fn_801125B8;
    lbl_801D77A8[695] = fn_801125E0;
    lbl_801D77A8[696] = fn_80084B88;
    lbl_801D77A8[697] = fn_8010F1B4;
    lbl_801D77A8[698] = fn_8010F248;
    lbl_801D77A8[699] = fn_8010F278;
    lbl_801D77A8[700] = fn_8010F2CC;
    lbl_801D77A8[701] = fn_8010F440;
    lbl_801D77A8[704] = fn_800833A4;
    lbl_801D77A8[708] = fn_801124D0;
    lbl_801D77A8[711] = fn_80109DE0;
    lbl_801D77A8[712] = fn_80109EAC;
    lbl_801D77A8[713] = fn_80109EE8;
    lbl_801D77A8[714] = fn_80109FB4;
    lbl_801D77A8[715] = fn_80084BE4;
    lbl_801D77A8[718] = fn_8010F4EC;
    lbl_801D77A8[720] = fn_80121890;
    lbl_801D77A8[721] = fn_80081754;
    lbl_801D77A8[722] = fn_800812F0;
    lbl_801D77A8[723] = fn_8007EFEC;
    lbl_801D77A8[727] = fn_80084BE8;
    lbl_801D77A8[725] = fn_8010A208;
    lbl_801D77A8[726] = fn_80084C88;
    lbl_801D77A8[728] = fn_8010A238;
    lbl_801D77A8[730] = fn_8010A2C8;
    lbl_801D77A8[736] = fn_80084CFC;
    lbl_801D77A8[737] = fn_80084D6C;
    lbl_801D77A8[739] = fn_8010A35C;
    lbl_801D77A8[740] = fn_8010A3A4;
    lbl_801D77A8[741] = fn_8010A3C4;
    lbl_801D77A8[746] = fn_80084DF4;
    lbl_801D77A8[749] = fn_8010F574;
    lbl_801D77A8[751] = fn_80084E7C;
    lbl_801D77A8[754] = fn_8010F5AC;
    lbl_801D77A8[755] = fn_8010F63C;
    lbl_801D77A8[756] = fn_8010A3C8;
    lbl_801D77A8[757] = fn_8010A400;
    lbl_801D77A8[760] = fn_80108070;
    lbl_801D77A8[759] = fn_80084E90;
    lbl_801D77A8[761] = fn_80084EC8;
    lbl_801D77A8[762] = fn_80084F04;
    lbl_801D77A8[763] = fn_80084F08;
    lbl_801D77A8[764] = fn_80084F0C;
    lbl_801D77A8[765] = fn_80084F3C;
    lbl_801D77A8[768] = fn_80084F40;
    lbl_801D77A8[769] = fn_80084F84;
}

void fn_8007BBA0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BBD8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BC10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BC48(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E0B38((u8)pArgs[0].i);
    fn_800E25E0();
}

void fn_8007BC74(MsgArg* pArgs, MsgArg* pResult) {
    Session_SetNumPlayers((u8)pArgs[0].i);
    fn_800E25E0();
}

void fn_8007BCA0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007BCA4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80079AD4();
}

// Set up a game mode from the menus.
void fn_8007BCC4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b11 = 1;
    fn_800E0B38((u8)pArgs[0].i);
    lbl_801D87C0.b0 = 1;
    gSession.a8[0] = 1;
    fn_800A4FD8();
}

void fn_8007BD18(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007BD1C(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E14E0((u8)pArgs[0].i);
}

void fn_8007BD44(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1260(0);
    fn_800E1404((u8)pArgs[0].i - 1);
}

void fn_8007BD84(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1260((u8)pArgs[0].i);
}

// A golfer's attribute.
void fn_8007BDAC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077A80(pArgs[0].i)->attr[pArgs[1].i];
}

void fn_8007BDFC(MsgArg* pArgs, MsgArg* pResult) {
    GetGolferName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// A golfer's full name: "First "Nick" Last" when the golfer has a nickname ("NA" is none), else
// "First Last". Golfer 18 never shows a nickname.
void GetGolferName(int nGolfer, char* szName) {
    int bNick;
    GolferRecord* pRecord;

    pRecord = fn_80077A80(nGolfer);
    bNick = 0;
    if (strcmp(pRecord->szNick, "NA") != 0 && strlen(pRecord->szNick) > 1 && nGolfer != 18) {
        bNick = 1;
    }
    if (bNick != 0) {
        sprintf(szName, "%s \"%s\" %s", pRecord->szFirst, pRecord->szNick, pRecord->szLast);
    } else {
        sprintf(szName, "%s %s", pRecord->szFirst, pRecord->szLast);
    }
}

void fn_8007BEEC(MsgArg* pArgs, MsgArg* pResult) {
}

// The custom round's hole list: four holes to start with, or, with both session flags 0x4000
// and 0x8000 set, all 18 holes of course 0.
void fn_8007BEF0(MsgArg* pArgs, MsgArg* pResult) {
    s32 i;

    gpGame->nHoleCourse[0] = 8;
    gpGame->nHoleNum[0] = 16;
    gpGame->nHoleCourse[1] = 5;
    gpGame->nHoleNum[1] = 3;
    gpGame->nHoleCourse[2] = 17;
    gpGame->nHoleNum[2] = 1;
    gpGame->nHoleCourse[3] = 18;
    gpGame->nHoleNum[3] = 8;
    for (i = 0; i < 18; i++) {
        gpGame->bHoleSelected[i] = 0;
    }
    gpGame->bHoleSelected[0] = 1;
    gpGame->bHoleSelected[1] = 1;
    gpGame->bHoleSelected[2] = 1;
    gpGame->bHoleSelected[3] = 1;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        gSession.options.a0[4] = 0;
        for (i = 0; i < 18; i++) {
            gpGame->nHoleCourse[i] = 0;
            gpGame->nHoleNum[i] = 17;
            gpGame->bHoleSelected[i] = 1;
        }
    }
    gpGame->b136 = 1;
}

void fn_8007C118(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (gSession.uFlags >> 14) & 1;
}

void fn_8007C12C(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 5) {
        lbl_80281ED4->b11703 = 1;
    }
    Session_SetGolfer(pArgs[1].i, pArgs[0].i);
}

void fn_8007C17C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nNumPlayers;
}

// A player's controller: -1 and 9 mean none (9); a real one is marked in use.
void fn_8007C190(MsgArg* pArgs, MsgArg* pResult) {
    s32 nController;

    nController = pArgs[1].i;
    if (nController == -1 || nController == 9) {
        gSession.nController[pArgs[0].i] = 9;
        return;
    }
    gSession.nController[pArgs[0].i] = nController;
    lbl_801D87C0.a2C[pArgs[1].i] = 1;
}

void fn_8007C1F8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nGolfer[pArgs[0].i];
}

void fn_8007C218(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_GetMode();
}

void fn_8007C248(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007C254(MsgArg* pArgs, MsgArg* pResult) {
    u8 bOld;

    bOld = lbl_80281EE0->b86;
    lbl_80281EE0->b86 = pArgs[0].i;
    if (bOld != lbl_80281EE0->b86 && lbl_80281EE0->n0 == 3) {
        fn_8008E354();
    }
}

// The CrAP screen's state: leaving state 0, or entering state 3, calls fn_8008DAEC; fn_8008E358
// is told whether it is now 0.
void fn_8007C2A0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nOld;

    nOld = lbl_80281EE0->n0;
    lbl_80281EE0->n0 = pArgs[0].i;
    if (nOld != 0 && lbl_80281EE0->n0 == 0) {
        fn_8008DAEC();
    }
    if (lbl_80281EE0->n0 == 3 && nOld != 3) {
        fn_8008DAEC();
    }
    if (lbl_80281EE0->n0 == 0) {
        fn_8008E358(1);
    } else {
        fn_8008E358(0);
    }
}

void fn_8007C330(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pResult->p)->pStr, fn_80077A80(pArgs[0].i)->szLast);
}

// One of the four names the card's state holds.
void fn_8007C370(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[1].i, pArgs[2].i);
    strcpy(((MsgString*)pArgs[0].p)->pStr, state.aszName[pArgs[3].i]);
}

// All four of them.
void fn_8007C3C8(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[0].i, pArgs[1].i);
    strcpy(((MsgString*)pArgs[2].p)->pStr, state.aszName[0]);
    strcpy(((MsgString*)pArgs[3].p)->pStr, state.aszName[1]);
    strcpy(((MsgString*)pArgs[4].p)->pStr, state.aszName[2]);
    strcpy(((MsgString*)pArgs[5].p)->pStr, state.aszName[3]);
}

// The card's state: a flag of it, or its free space.
void fn_8007C440(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags >> 1) & 1;
}

void fn_8007C488(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C48C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008F80C(pArgs[0].i, (u8)pArgs[1].i);
}

void fn_8007C4B8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
}

void fn_8007C4D8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD7C();
}

// fn_800A0C6C with a card, a profile slot and a string, then the slot's profile is marked loaded
// (the test never fails: the result is 1 or an error, never 0).
void fn_8007C4F8(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPosStr pos;
    s32 nError;
    s32 n;

    pos.pos.nPort = pArgs[0].i;
    pos.pos.nSlot = pArgs[1].i;
    pos.pos.n8 = pArgs[2].i;
    pos.szC = ((MsgString*)pArgs[3].p)->pStr;
    nError = fn_800A0C6C(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
    if (pResult->i != 0) {
        fn_80077808(pArgs[2].i);
        lbl_801D7148.aLoaded[pArgs[2].i] = 1;
    }
}

void fn_8007C594(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    nError = fn_800A0E6C(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007C5F0(MsgArg* pArgs, MsgArg* pResult) {
    int n;

    n = pArgs[0].i;
    if (n == 11) {
        fn_800A73F0((Rand_Next(0) & 7) + 11);
    } else {
        fn_800A73F0(n);
    }
}

void fn_8007C634(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.nC = 2;
        return;
    case 2:
        gSession.options.nC = 3;
        return;
    case 3:
        gSession.options.nC = 0;
        return;
    }
}

void fn_8007C698(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A2100(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// The slot's profile has a created golfer.
// Every player's tee set: 1, 2 or 3 picks tee set 2, 1 or 0.
void fn_8007C6E4(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 5; i++) {
        switch (pArgs[1].i) {
        case 1:
            gSession.nTeeSet[i] = 2;
            break;
        case 2:
            gSession.nTeeSet[i] = 1;
            break;
        case 3:
            gSession.nTeeSet[i] = 0;
            break;
        }
    }
}

void fn_8007C748(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)gpSaveData[pArgs[0].i].createdGolfer.bAvailable != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8007C784(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 30;
}

void fn_8007C790(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007C79C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i + 30;
}

void fn_8007C7AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C7B0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (s8)fn_80077A80(pArgs[0].i)->bAvailable;
}

void fn_8007C7EC(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n == 9) {
        pResult->i = 1;
        return;
    }
    pResult->i = lbl_801D87C0.a1[n];
}

void fn_8007C81C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags >> 3) & 1;
}

void fn_8007C864(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags >> 4) & 1;
}

void fn_8007C8AC(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    fn_8009F7F4(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.nFreeBlocks;
}

// The game save's size on the card.
void fn_8007C8F0(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    fn_80084FF0(0);
    fn_8009CD80(pos.nPort, pos.nSlot);
    pResult->i = fn_80084FB4(&pos);
    fn_8009CD7C();
}

void fn_8007C94C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C950(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F7E8(pArgs[0].i);
}

void fn_8007C988(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D880C.n4 = pArgs[0].i;
    lbl_801D880C.n0 = 0;
}

void fn_8007C9A4(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    nError = fn_8009FE90(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007C9F8(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    nError = fn_8009FCFC(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// The on/off options: the menus send and read 1 for on and 2 for off.
void fn_8007CA4C(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a7[1] = 1;
        break;
    case 2:
        gSession.options.a7[1] = 0;
        break;
    }
    switch (pArgs[1].i) {
    case 1:
        gSession.options.a7[2] = 1;
        return;
    case 2:
        gSession.options.a7[2] = 0;
        return;
    }
}

// Option a0[4]: the menus' choices 1..6 are the values 5, 0, 1, 2, 3, 4; it is passed on times 0.2.
void fn_8007CACC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
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
}

void fn_8007CBCC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bGimmes = 1;
        return;
    case 2:
        gSession.options.bGimmes = 0;
        return;
    }
}

void fn_8007CC0C(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a7[1]) {
    case 1:
        *(s32*)pArgs[0].p = 1;
        break;
    case 0:
        *(s32*)pArgs[0].p = 2;
        break;
    }
    switch (gSession.options.a7[2]) {
    case 1:
        *(s32*)pArgs[1].p = 1;
        return;
    case 0:
        *(s32*)pArgs[1].p = 2;
        return;
    }
}

void fn_8007CC90(MsgArg* pArgs, MsgArg* pResult) {
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
}

void fn_8007CD1C(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bGimmes) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// A string's width, scaled.
void fn_8007CD58(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

// Save kind 1's size on the card.
void fn_8007CD98(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    fn_8009CD80(pos.nPort, pos.nSlot);
    fn_80084FF0(1);
    pResult->i = fn_80084FB4(&pos);
    fn_8009CD7C();
}

void fn_8007CDF0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CDF4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800EAE38(fn_800EAC94(pArgs[0].i));
}

void fn_8007CE1C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CE20(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077A80(pArgs[0].i)->nModelID;
}

void fn_8007CE58(MsgArg* pArgs, MsgArg* pResult) {
    fn_80102308(pArgs[0].i);
}

void fn_8007D25C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D260(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D264(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D268(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D26C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D270(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        lbl_80281ED4->b11702 = 1;
    } else {
        lbl_80281ED4->b11702 = 0;
    }
}

void fn_8007D2A4(MsgArg* pArgs, MsgArg* pResult) {
    if (lbl_80281ED4->b11702 != 0) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_8007D2D0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D380(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077ACC()->n6C;
}

void fn_8007D3B4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80057438(fn_80077ACC());
}

void fn_8007D3D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801020C0();
}

void fn_8007D408(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D40C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D410(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D414(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D418(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D41C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D420(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D424(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6D8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6DC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6E0(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        pResult->i = 0;
    } else {
        pResult->i = pArgs[0].i + 30;
    }
}

// Look a value up in the prize table's ranges (-1: in none).
void fn_8007D708(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int n;
    int nValue;

    nValue = pArgs[0].i;
    n = -1;
    for (i = 0; i < NUM_EARNINGS_RANGES; i++) {
        if (nValue >= lbl_80200538.aRange[i].n0 - 1 && nValue <= lbl_80200538.aRange[i].n4) {
            n = lbl_80200538.aRange[i].n8;
            break;
        }
    }
    pResult->i = n;
}

void fn_8007D76C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;

    pProfile = fn_80077ACC();
    pProfile->n6C = pArgs[1].i;
}

// Set an attribute of the created golfer being worked on.
void fn_8007D7A0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;
    int nAttr;

    nAttr = pArgs[1].i;
    pProfile = fn_80077ACC();
    pProfile->createdGolfer.attr[nAttr] = pArgs[2].i;
}

void fn_8007D7E4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = 2;
    fn_800A75B4();
}

void fn_8007D924(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nSplitScreen = pArgs[0].i;
}

void fn_8007D938(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1404((u8)pArgs[0].i - 1);
}

void fn_8007D964(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D9D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->nSlot;
}

void fn_8007DAB0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bActive;
}

void fn_8007DAD0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DAD4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D87C0.n38;
}

void fn_8007DAE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D87C0.a2C[pArgs[0].i];
}

void fn_8007DB04(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n < 4) {
        lbl_801D87C0.a2C[n] = 0;
    }
}

void fn_8007DB28(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB2C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB30(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB34(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB38(MsgArg* pArgs, MsgArg* pResult) {
}

// A slot's profile's numbers.
void fn_8007DB3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].n7C;
}

void fn_8007DDEC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nA0;
}

void fn_8007DE10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nA4;
}

void fn_8007DE34(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nAC;
}

void fn_8007DE58(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nB0;
}

void fn_8007DE7C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nB4;
}

void fn_8007DEA0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nB8;
}

void fn_8007DEC4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nBC;
}

void fn_8007DEE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nC0;
}

void fn_8007DF0C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nC4;
}

void fn_8007E0BC(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nPinSet = pArgs[0].i;
}

void fn_8007E0D0(MsgArg* pArgs, MsgArg* pResult) {
    fn_800142A4(pArgs[0].i);
}

void fn_8007E0F8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aAward[pArgs[1].i].bWon;
}

void fn_8007E128(MsgArg* pArgs, MsgArg* pResult) {
    Award* pAward;

    pAward = &fn_80077ACC()->aAward[pArgs[0].i];
    pResult->i = pAward[23].bWon;
}

void fn_8007E174(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80200538.a9B4[pArgs[0].i];
}

// The mulligan rule: none in game mode 7, any number in mode 9, else the one picked.
void fn_8007E194(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 7) {
        gpGame->nMulligans = 0;
        return;
    }
    if (Game_GetMode() == 9) {
        gpGame->nMulligans = 1;
        return;
    }
    gpGame->nMulligans = pArgs[0].i;
}

void fn_8007E200(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E288(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E354(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E51C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(0, pArgs, pResult);
}

void fn_8007E548(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(1, pArgs, pResult);
}

void fn_8007E574(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(2, pArgs, pResult);
}

void fn_8007E5A0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(3, pArgs, pResult);
}

void fn_8007E5CC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(4, pArgs, pResult);
}

void fn_8007E5F8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(5, pArgs, pResult);
}

void fn_8007E624(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(6, pArgs, pResult);
}

void fn_8007E650(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(7, pArgs, pResult);
}

void fn_8007E744(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E748(MsgArg* pArgs, MsgArg* pResult) {
}

// How many challenge groups in a row, from the first, have a medal.
void fn_8007E74C(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 29; i++, n++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 3) break;
    }
    pResult->i = n;
}

void fn_8007E798(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E818(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pResult->i = fn_800A1964(&pos);
}

void fn_8007E85C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A1164(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr, pArgs[3].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007E8B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007E8C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E8C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.aLoaded[pArgs[0].i];
}

void fn_8007E8DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b0F;
}

void fn_8007E8F0(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b0F = pArgs[0].i;
}

void fn_8007E904(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 60;
        return;
    }
    pResult->i = 30;
}

void fn_8007E92C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281EE0->b83 = pArgs[0].i;
}

void fn_8007E9A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b11;
}

void fn_8007E9BC(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    n = fn_800A09EC(&pos);
    if (n >= 0) {
        pResult->i = n;
        return;
    }
    pResult->i = 0;
}

void fn_8007ED88(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.nC) {
    case 2:
        pResult->i = 1;
        return;
    case 3:
        pResult->i = 2;
        return;
    case 0:
        pResult->i = 3;
        return;
    }
}

// The wind option: calm (0) to gusty (3), shown as 1 to 4.
void fn_8007EDDC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.nWind) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    case 3:
        pResult->i = 4;
        return;
    }
}

void fn_8007EE40(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a7[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_8007EE7C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007EE80(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->profile.createdGolfer.nModelID = pArgs[1].i;
}

void fn_8007EF9C(MsgArg* pArgs, MsgArg* pResult) {
}

// How many of the first 23 awards the slot's profile has won.
void fn_8007EFA0(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 23; i++) {
        if (gpSaveData[pArgs[0].i].aAward[i].bWon == 1) {
            n++;
        }
    }
    pResult->i = n;
}

void fn_8007F088(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_vGetAllTimeRecordsHeld(&gpSaveData[pArgs[0].i]);
}

// One for a TOUR card, plus one per challenge group with a medal.
void fn_8007F724(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    if (gpSaveData[pArgs[0].i].nTourCardLevel >= 1) {
        n = 1;
    }
    for (i = 0; i < 29; i++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] != 3) {
            n++;
        }
    }
    pResult->i = n;
}

void fn_8007F87C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nTourCardLevel;
}

void fn_8007FCC0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.nMode;
}

void fn_8007FCD4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.nMode = pArgs[0].i;
}

void fn_8007FCE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].b70;
}

void fn_8007FEAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800907AC(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_8007FED8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b18;
}

void fn_8007FF3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281EE0->b86;
}

void fn_8007FF4C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nController[pArgs[0].i];
}

// What unlocks a course.
void fn_8007FF6C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80200538.aCoursePrice[pArgs[0].i].nPrice;
}

// The saved replay's course, hole and golfer.
void fn_8008017C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, lbl_80191990[gReplayData.nCourse]);
}

void fn_800801C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gReplayData.nHole;
}

void fn_800801D4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, gReplayData.player.golfer.szLast);
}

void fn_80080300(MsgArg* pArgs, MsgArg* pResult) {
}

// A challenge group's best medal (0 best, 3 none).
void fn_80080304(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i];
}

void fn_80080334(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].createdGolfer.nModelID;
}

void fn_80080358(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD80(pArgs[0].i, pArgs[1].i);
    fn_8009CD7C();
}

void fn_800804D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800804E4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E22E4(pArgs[0].i, pArgs[1].i, pArgs[2].i - 1);
}

void fn_800805C4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_CREDITS;
    fn_800A75B4();
}

void fn_800805F0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800807D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80080AA0(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15 = pArgs[2].i;
}

// A string's first character.
void fn_80080AD0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = ((MsgString*)pArgs[0].p)->pStr[0];
}

void fn_80080C2C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15;
}

void fn_80080C60(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n4;
}

void fn_80080C74(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n4 = pArgs[0].i;
}

void fn_80080C84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n5;
}

void fn_80080C98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n5 = pArgs[0].i;
}

void fn_80080CA8(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = 0;
}

void fn_800810BC(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aCPU[pArgs[0].i] = pArgs[1].i;
}

void fn_800810D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.aCPU[pArgs[0].i];
}

void fn_80081270(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.b84 = 1;
        return;
    case 2:
        gSession.options.b84 = 0;
        return;
    }
}

void fn_800812B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[0] = 1;
        return;
    case 2:
        gSession.options.a24[0] = 0;
        return;
    }
}

void fn_800812F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[1] = 1;
        return;
    case 2:
        gSession.options.a24[1] = 0;
        return;
    }
}

void fn_80081330(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[2] = 1;
        return;
    case 2:
        gSession.options.a24[2] = 0;
        return;
    }
}

void fn_80081370(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[3] = 1;
        return;
    case 2:
        gSession.options.a24[3] = 0;
        return;
    }
}

void fn_800813B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[4] = 1;
        return;
    case 2:
        gSession.options.a24[4] = 0;
        return;
    }
}

void fn_800813F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[5] = 1;
        return;
    case 2:
        gSession.options.a24[5] = 0;
        return;
    }
}

void fn_80081430(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[6] = 1;
        return;
    case 2:
        gSession.options.a24[6] = 0;
        return;
    }
}

void fn_80081470(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[7] = 1;
        return;
    case 2:
        gSession.options.a24[7] = 0;
        return;
    }
}

void fn_800814B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bBoostEnabled = 1;
        return;
    case 2:
        gSession.options.bBoostEnabled = 0;
        return;
    }
}

void fn_800814F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bSpinEnabled = 1;
        return;
    case 2:
        gSession.options.bSpinEnabled = 0;
        return;
    }
}

void fn_800816DC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.b84) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081718(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081754(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[1]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081790(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[2]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800817CC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[3]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081808(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[4]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081844(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[5]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081880(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[6]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800818BC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[7]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800818F8(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bBoostEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081934(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bSpinEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Pick the saved custom round the holes come from.
void fn_800819FC(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->b136 = pArgs[0].i;
    gpGame->nSaveSlot = pArgs[1].i;
    gpGame->nSaveCourse = pArgs[2].i;
    if (gpGame->b136 != 0) {
        GM_SetupCustomHoleSelection();
        fn_800E1434();
    }
}

void fn_80081BD4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077780();
}

void fn_80081BF4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077808(pArgs[0].i);
}

void fn_80081CF8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80081F98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n1 = pArgs[0].i;
}

void fn_80081FA8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n1;
}

void fn_80082608(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n10620;
}

void fn_8008266C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n10620 = 0;
}

void fn_80082680(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A218C(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_800826C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2194(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80082708(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009EB44(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_800A2628();
}

void fn_80082758(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2604(pArgs[0].i);
}

void fn_80082790(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, fn_800A2614(pArgs[0].i));
}

// Add a payout to player slot 0's money (when pArgs[1] is set); the front end keeps the amount.
void fn_800827D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nAmount;

    nAmount = pArgs[0].i;
    lbl_801D7148.n1C = nAmount;
    if (pArgs[1].i != 0) {
        gpSaveData->n6C += nAmount;
    }
}

void fn_80082800(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

void fn_8008281C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

void fn_80082928(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

void fn_8008293C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[pArgs[0].i]);
}

void fn_80082978(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008297C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082980(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

// The letter for a number: 0 is "A".
void fn_8008299C(MsgArg* pArgs, MsgArg* pResult) {
    sprintf(((MsgString*)pArgs[2].p)->pStr, "%c", pArgs[0].i + 'A');
}

void fn_800829D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

void fn_800829E0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 4;
}

void fn_800829EC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077968(pArgs[0].i);
}

void fn_80082A10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    strcpy(((MsgString*)pArgs[0].p)->pStr, "");
}

void fn_80082A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A48(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A4C(MsgArg* pArgs, MsgArg* pResult) {
}

// Empty a player slot: no profile in it, none loaded.
void fn_80082C74(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].bActive = 0;
    lbl_801D7148.aLoaded[pArgs[0].i] = 0;
}

void fn_80082CA4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082CA8(MsgArg* pArgs, MsgArg* pResult) {
    fn_800ED650(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p);
}

void fn_80082CDC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

void fn_80082D14(MsgArg* pArgs, MsgArg* pResult) {
    gSession.options.rows[pArgs[0].i][pArgs[1].i] = pArgs[2].i;
}

void fn_80082D98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n3 = pArgs[0].i;
}

// The game's title.
void fn_80082DA8(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

void fn_80082DBC(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_8009EE28(pArgs[0].i, pArgs[1].i) == MC_ERR_BADDATA) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80082E10(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A0A7C(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_80083354(MsgArg* pArgs, MsgArg* pResult) {
}

// Play a golfer's bio movie, or the credits for -1.
void fn_80083358(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_BIO;
    if (pArgs[0].i == -1) {
        pMovie->nKind = FE_MOVIE_CREDITS;
    }
    fn_800A75B4();
}

void fn_800833A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A75B4();
}

void fn_800833C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800833D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    gSession.nGolfer[n] = (u8)(n + 30);
}

// No player slot has a backup row.
void fn_800833F4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aBackup[0] = -1;
    lbl_801D7148.aBackup[1] = -1;
    lbl_801D7148.aBackup[2] = -1;
    lbl_801D7148.aBackup[3] = -1;
}

void fn_80083414(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aBackup[pArgs[0].i] = -1;
}

// The pad in port pArgs[0] is a WaveBird (its SI device type).
void fn_80083430(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_80013050(pArgs[0].i) == 0x8B100000) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80083480(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b10;
}

void fn_80083494(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b10 = pArgs[0].i;
}

void fn_800834A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F728(pArgs[0].i);
}

void fn_800834DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800835B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800835BC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800835C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80083860(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = pArgs[2].i;
}

void fn_80083890(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 150;
}

void fn_8008389C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838BC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strcmp(((MsgString*)pArgs[0].p)->pStr, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_80083904(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083908(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008390C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083910(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083914(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083918(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008391C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083920(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083924(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083928(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008392C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083930(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083934(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008DD34(lbl_80281ED4->nSlot, pArgs[0].i);
}

void fn_80083964(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 0.0f;
}

void fn_80083970(MsgArg* pArgs, MsgArg* pResult) {
}

// Three values out: 50, 50 and a level of 25..250 (the tenth of pArgs[3] plus one, times 25), the
// level going to the one of the three pArgs[2] picks.
void fn_80083974(MsgArg* pArgs, MsgArg* pResult) {
    s32 nWhich;
    s32 nLevel;

    nWhich = pArgs[2].i % 3;
    nLevel = (pArgs[3].i % 10 + 1) * 25;
    switch (nWhich) {
    case 0:
        *(s32*)pArgs[4].p = nLevel;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = 50;
        return;
    case 1:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = nLevel;
        *(s32*)pArgs[6].p = 50;
        return;
    case 2:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = nLevel;
        return;
    }
}

void fn_80083A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083A48(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083BA4(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010D334(pArgs[0].i);
}

void fn_80083BC8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    if (pResult->i == 0) {
        fn_801102AC();
    }
}

void fn_80083E48(MsgArg* pArgs, MsgArg* pResult) {
    fn_80123FF8();
    fn_8012408C(0);
}

void fn_80083E70(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(6);
}

void fn_80083E94(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(8);
}

void fn_80083EB8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083EBC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(18);
}

void fn_80083F54(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_8008410C(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = *(s32*)pArgs[0].p;
    pResult->i = n + fn_8012411C();
    fn_80124138(pResult->i);
}

void fn_80084158(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008415C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084160(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801241CC();
}

void fn_80084190(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80124174();
}

void fn_80084208(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(5);
}

// ---- the EA Sports Bio screens (EASportsBio.c does the work) ----

void fn_8008422C(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125600(pArgs[0].i, pArgs[1].i);
}

void fn_80084258(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125648(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

void fn_80084288(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012566C(pArgs[0].i);
}

void fn_800842AC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125680(pArgs[0].i);
}

void fn_800842D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125354(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    nError = fn_801253F0(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

void fn_800844E0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125434(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    pResult->i = nError == 0;
}

void fn_80084544(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_IsBioLoaded();
}

void fn_800845D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125528(pArgs[0].i, pArgs[1].i);
}

void fn_80084678(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125280(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

void fn_800846C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800846D4(MsgArg* pArgs, MsgArg* pResult) {
    s32* pN;

    pN = pArgs[0].p;
    *pN = fn_80125928();
}

void fn_80084750(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084754(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801254EC();
    pResult->i = fn_801254B8();
}

void fn_8008478C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_80124BDC();
}

void fn_800847BC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80084FF0(pArgs[0].i);
}

void fn_800848E4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800848E8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800EAE6C();
}

void fn_80084918(MsgArg* pArgs, MsgArg* pResult) {
    fn_800EAE44(pArgs[0].i - 1);
}

void fn_80084940(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED280(pArgs[1].i - 1));
}

void fn_80084984(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED2C8(pArgs[1].i - 1));
}

// A challenge group's best medal, the group counted from 1.
void fn_800849C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i - 1];
}

// Passes the message on to one of three handlers, by pArgs[0].
void fn_80084B88(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        fn_8010F2FC(pArgs, pResult);
        return;
    case 1:
        fn_8010F3A4(pArgs, pResult);
        return;
    case 3:
        fn_8011DF90(pArgs, pResult);
        return;
    case 2:
        return;
    }
}

void fn_80084BE4(MsgArg* pArgs, MsgArg* pResult) {
}

// Copy a string, cut to eight characters and "..." when it is longer than 12.
void fn_80084D6C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 12) {
        ((MsgString*)pArgs[1].p)->pStr[11] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[10] = '.';
        ((MsgString*)pArgs[1].p)->pStr[9] = '.';
        ((MsgString*)pArgs[1].p)->pStr[8] = '.';
    }
}

// The same, to 28 characters and "..." when it is longer than 32.
void fn_80084DF4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 32) {
        ((MsgString*)pArgs[1].p)->pStr[31] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[30] = '.';
        ((MsgString*)pArgs[1].p)->pStr[29] = '.';
        ((MsgString*)pArgs[1].p)->pStr[28] = '.';
    }
}

void fn_80084E7C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n11704;
}

void fn_80084E90(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125194(0, 0);
}

void fn_80084EC8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009D390(pArgs[0].i, pArgs[1].i);
}

void fn_80084F04(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F08(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F0C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A27F4();
}

void fn_80084F3C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F40(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801252D0(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80084F84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_GetCurrentRewardMessage();
}

// The memory-card operations of the set picked (lbl_80281FFC), run on the card at pPos.
s32 fn_80084FB4(MCCardPos* pPos) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[4](pPos);
}

void fn_80084FF0(int n) {
    lbl_80281FFC = n;
}

s32 fn_80084FF8(MCCardPos* pPos) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[2](pPos);
}

s32 fn_80085034(MCCardPos* pPos) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[3](pPos);
}

s32 fn_80085070(MCCardPos* pPos) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[1](pPos);
}

s32 fn_800850AC(MCCardPos* pPos) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[0](pPos);
}
