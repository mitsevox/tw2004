// FE_MessageTable.c (our name): the menus' message table. The menu UI (uiProcessInterface.c) sends
// its messages here while the front end runs (game type 3): fn_80079EA8 fills a table of 770
// handlers and fn_80079E6C calls the one for a message's number with its arguments and results.
// The handlers read and set what the menus show: golfer names, the session's setup, the save
// profile's stats and records, the Create-A-Player choices. TW06 has GetGolferName in
// apt_fe_gamemessages.c. Rounds have their own table (fn_800850E4).

#include "game_types.h"
#include "platform.h"

// ---- sweep code (not yet cleaned up) ----

void Session_SetNumPlayers();
void fn_800E0B38();
s32 fn_800E2520();
void fn_800E25E0();
void fn_8007BBA0(u8* p0, u8* p1);
void fn_8007BBD8(u8* p0, u8* p1);
void fn_8007BC10(u8* p0, u8* p1);
void fn_8007BC48(u8* p0);
void fn_8007BC74(u8* p0);
void fn_80079AD4();
void fn_8007BCA0(void);
void fn_8007BCA4(void);
void fn_8007BD18(void);
s32 fn_80077A80();
void fn_800E1260();
void fn_800E1404();
void fn_800E14E0();
void fn_8007BD1C(u8* p0);
void fn_8007BD44(u8* p0);
void fn_8007BD84(u8* p0);
void GetGolferName();
void fn_8007BDFC(u8* p0);
void fn_8007BEEC(void);
s32 Game_GetMode();
s32 Session_SetGolfer(s32, s32);
extern u8* lbl_80281ED4;
void fn_8007C12C(void* arg0);
extern u8 gSession[];
u8* fn_8007C17C(s32 p0, u8* p1);
void fn_8007C218(s32 p0, u8* p1);
void fn_8007C248(s32 a, u8* p);
s32 fn_8008E354();
extern u8* lbl_80281EE0;
void fn_8007C254(s32* arg0);
void fn_8007C330(u8* p0, u8* p1);
void fn_8007C488(void);
void fn_8008F80C();
void fn_8007C48C(u8* p0);
void fn_8009CD10();
void fn_8009CD7C();
void fn_8007C4B8(void);
void fn_8007C4D8(void);
s32 fn_800A2100(s32, s32, void*);
void fn_8007C698(void* arg0, s32* arg1);
void fn_8007C784(s32 a, u8* p);
void fn_8007C790(s32 a, u8* p);
void fn_8007C79C(u8* p0, u8* p1);
void fn_8007C7AC(void);
void fn_8007C7B0(u8* p0, u8* p1);
void fn_8007C94C(void);
s32 fn_8009F7E8();
void fn_8007C950(u8* p0, u8* p1);
extern u8 lbl_801D880C[];
u8* fn_8007C988(u8* p0);
f32 fn_80012C30(s32);
extern f32 lbl_80283AE8;
void fn_8007CD58(void** arg0, f32* arg1);
void fn_8007CDF0(void);
void fn_800EAC94();
void fn_800EAE38();
void fn_8007CDF4(u8* p0);
void fn_8007CE1C(void);
void fn_8007CE20(u8* p0, u8* p1);
void fn_80102308();
void fn_8007CE58(u8* p);
void fn_8007D25C(void);
void fn_8007D260(void);
void fn_8007D264(void);
void fn_8007D268(void);
void fn_8007D26C(void);
void fn_8007D270(s32* arg0);
void fn_8007D2D0(void);
s32 fn_80077ACC();
void fn_8007D380(s32 p0, u8* p1);
void fn_80057438();
void fn_8007D3B4(void);
s32 fn_801020C0();
void fn_8007D3D8(s32 p0, u8* p1);
void fn_8007D408(void);
void fn_8007D40C(void);
void fn_8007D410(void);
void fn_8007D414(void);
void fn_8007D418(void);
void fn_8007D41C(void);
void fn_8007D420(void);
void fn_8007D424(void);
void fn_8007D6D8(void);
void fn_8007D6DC(void);
void fn_8007D6E0(void* arg0, s32* arg1);
void fn_8007D76C(u8* p0);
s32 fn_800770FC();
void fn_800A75B4();
void fn_8007D7E4(void);
u8* fn_8007D924(u8* p0);
void fn_8007D938(u8* p0);
void fn_8007D964(void);
void fn_8007D9D0(s32 p0, u8* p1);
extern s32 gpSaveData;
void fn_8007DAB0(u8* p0, u8* p1);
void fn_8007DAD0(void);
extern u8 lbl_801D87C0[];
u8* fn_8007DAD4(s32 p0, u8* p1);
void fn_8007DB28(void);
void fn_8007DB2C(void);
void fn_8007DB30(void);
void fn_8007DB34(void);
void fn_8007DB38(void);
u8* fn_8007E0BC(u8* p0);
void fn_800142A4();
void fn_8007E0D0(u8* p0);
extern void* gpGame;
void fn_8007E194(s32* arg0);
void fn_8007E200(void);
void fn_8007E288(void);
void fn_8007E354(void);
void fn_8007E458();
void fn_8007E51C(s32 p0, s32 p1);
void fn_8007E548(s32 p0, s32 p1);
void fn_8007E574(s32 p0, s32 p1);
void fn_8007E5A0(s32 p0, s32 p1);
void fn_8007E5CC(s32 p0, s32 p1);
void fn_8007E5F8(s32 p0, s32 p1);
void fn_8007E624(s32 p0, s32 p1);
void fn_8007E650(s32 p0, s32 p1);
void fn_8007E744(void);
void fn_8007E748(void);
void fn_8007E798(void);
s32 fn_800A1164(s32, s32, s32, s32);
void fn_8007E85C(void* arg0, s32* arg1);
void fn_8007E8B4(s32 a, u8* p);
void fn_8007E8C0(void);
extern u8 lbl_801D7148[];
u8* fn_8007E8C4(u8* p0, u8* p1);
u8* fn_8007E8DC(s32 p0, u8* p1);
u8* fn_8007E8F0(u8* p0);
void fn_8007E92C(u8* p0);
void fn_8007E9A0(void);
void fn_8007E9A4(void);
u8* fn_8007E9A8(s32 p0, u8* p1);
void fn_8007EE7C(void);
void fn_8007EE80(u8* p0);
void fn_8007EF9C(void);
s32 GM_vGetAllTimeRecordsHeld();
void fn_8007F088(u8* p0, u8* p1);
u8* fn_8007FCC0(s32 p0, u8* p1);
u8* fn_8007FCD4(u8* p0);
void fn_800907AC();
void fn_8007FEAC(u8* p0);
u8* fn_8007FED8(s32 p0, u8* p1);
void fn_8007FF3C(s32 p0, u8* p1);
extern u8 gReplayData[];
extern u8 lbl_80191990[];
void fn_8008017C(u8* p0);
u8* fn_800801C0(s32 p0, u8* p1);
void fn_800801D4(u8* p0);
void fn_80080300(void);
void fn_8009CD80();
void fn_80080358(u8* p0);
void fn_800804D8(s32 a, u8* p);
s32 fn_800E22E4();
void fn_800804E4(u8* p0, u8* p1);
void fn_800805C4(void);
void fn_800805F0(void);
void fn_800807D0(s32 a, u8* p);
void fn_80080AD0(u8* p0, u8* p1);
void fn_80080C60(s32 p0, u8* p1);
void fn_80080C74(u8* p0);
void fn_80080C84(s32 p0, u8* p1);
void fn_80080C98(u8* p0);
void fn_80080CA8(u8* p0);
s32 GM_SetupCustomHoleSelection();
s32 fn_800E1434();
void fn_800819FC(void* arg0);
void fn_80077780();
void fn_80077808();
void fn_80081BD4(void);
void fn_80081BF4(u8* p);
void fn_80081CF8(void);
void fn_80081F98(u8* p0);
void fn_80081FA8(s32 p0, u8* p1);
void fn_80082608(s32 p0, u8* p1);
void fn_8008266C(void);
s32 fn_8009EB44();
s32 fn_800A218C();
s32 fn_800A2194();
s32 fn_800A2604();
s32 fn_800A2614();
s32 fn_800A2628();
void fn_80082680(u8* p0, u8* p1);
void fn_800826C4(u8* p0, u8* p1);
void fn_80082708(u8* p0, u8* p1);
void fn_80082758(u8* p0, u8* p1);
void fn_80082790(u8* p0);
void fn_80082800(u8* p0);
void fn_8008281C(s32 a, u8* p);
u8* fn_80082928(void);
void fn_8008293C(u8* p0);
void fn_80082978(void);
void fn_8008297C(void);
void fn_80082980(u8* p0);
void fn_800829D4(s32 a, u8* p);
void fn_800829E0(s32 a, u8* p);
void fn_80077968();
void fn_800829EC(u8* p);
void fn_80082A10(void** arg0, s32* arg1);
void fn_80082A44(void);
void fn_80082A48(void);
void fn_80082A4C(void);
void fn_80082C74(u8* p0);
void fn_80082CA4(void);
void fn_800ED650();
void fn_80082CA8(u8* p0);
void fn_80082CDC(u8* p0, u8* p1);
void fn_80082D98(s32* arg0);
s32 fn_8009EE28(s32, s32, void*);
void fn_80082DBC(void* arg0, s32* arg1);
s32 fn_800A0A7C(s32, s32, void*);
void fn_80082E10(void* arg0, s32* arg1);
void fn_80083354(void);
void fn_80083358(s32* arg0);
void fn_800833A4(void);
void fn_800833C4(s32 a, u8* p);
s32 fn_80013050(s32);
void fn_80083430(s32* arg0, s32* arg1);
u8* fn_80083480(s32 p0, u8* p1);
u8* fn_80083494(u8* p0);
s32 fn_8009F728();
void fn_800834A8(u8* p0, u8* p1);
void fn_800834DC(s32 a, u8* p);
void fn_800835B8(void);
void fn_800835BC(s32 a, u8* p);
void fn_800835C8(s32 a, u8* p);
void fn_80083890(s32 a, u8* p);
void fn_8008389C(void);
void fn_800838A0(void);
void fn_800838A4(void);
void fn_800838A8(void);
void fn_800838AC(void);
void fn_800838B0(void);
void fn_800838B4(void);
void fn_800838B8(void);
void fn_800838BC(void);
void fn_800838C0(void);
void fn_800838C4(u8* p0, u8* p1);
void fn_80083904(void);
void fn_80083908(void);
void fn_8008390C(void);
void fn_80083910(void);
void fn_80083914(void);
void fn_80083918(void);
void fn_8008391C(void);
void fn_80083920(void);
void fn_80083924(void);
void fn_80083928(void);
void fn_8008392C(void);
void fn_80083930(void);
void fn_80083970(void);
void fn_80083974(void* arg0);
void fn_80083A44(void);
void fn_80083A48(void);
void fn_8010D334();
void fn_80083BA4(u8* p);
void fn_80123FF8();
void fn_8012408C();
void fn_80083E48(void);
void fn_80083E70(void);
void fn_80083E94(void);
void fn_80083EB8(void);
void fn_80083EBC(void);
void fn_80083F54(s32 a, u8* p);
s32 fn_8012411C(s32*);
s32 fn_80124138(s32);
void fn_8008410C(s32** arg0, s32* arg1);
void fn_80084158(void);
void fn_8008415C(void);
s32 fn_80124174();
s32 fn_801241CC();
void fn_80084160(s32 p0, u8* p1);
void fn_80084190(s32 p0, u8* p1);
void fn_80084208(void);
void fn_80125600();
void fn_80125648();
void fn_8008422C(u8* p0);
void fn_80084258(u8* p0);
void fn_8012566C();
void fn_80125680();
void fn_80084288(u8* p);
void fn_800842AC(u8* p);
s32 fn_80125354(s32, s32);
s32 fn_801253F0(s32, s32);
void fn_800842D0(void* arg0, s32* arg1);
s32 fn_80125434(s32, s32, void*);
void fn_800844E0(void* arg0, s32* arg1);
s32 EASBio_IsBioLoaded();
void fn_80084544(s32 p0, u8* p1);
s32 fn_80125528();
void fn_800845D4(u8* p0, u8* p1);
s32 fn_80125280();
void fn_80084678(u8* p0, u8* p1);
void fn_800846C8(s32 a, u8* p);
s32 fn_80125928();
void fn_800846D4(s32** arg0);
void fn_80084750(void);
f32 fn_80124BDC();
s32 fn_801254B8();
s32 fn_801254EC();
void fn_80084754(s32 p0, u8* p1);
void fn_8008478C(s32 p0, u8* p1);
void fn_800847BC(u8* p);
void fn_800848E4(void);
void fn_800EAE44();
s32 fn_800EAE6C();
s32 fn_800ED280();
s32 fn_800ED2C8();
void fn_800848E8(s32 p0, u8* p1);
void fn_80084918(u8* p0);
void fn_80084940(u8* p0);
void fn_80084984(u8* p0);
s32 fn_8010F2FC();
s32 fn_8010F3A4();
s32 fn_8011DF90();
void fn_80084B88(s32* arg0);
void fn_80084BE4(void);
void fn_80084D6C(void* arg0);
void fn_80084DF4(void* arg0);
void fn_80084E7C(s32 p0, u8* p1);
s32 fn_8009D390();
s32 fn_80125194();
void fn_80084E90(s32 p0, u8* p1);
void fn_80084EC8(u8* p0, u8* p1);
void fn_80084F04(void);
void fn_80084F08(void);
s32 fn_800A27F4();
void fn_80084F0C(s32 p0, u8* p1);
void fn_80084F3C(void);
s32 fn_801252D0();
s32 EASBio_GetCurrentRewardMessage();
void fn_80084F40(u8* p0, u8* p1);
void fn_80084F84(s32 p0, u8* p1);
extern s32 lbl_80281FFC;
void fn_80084FF0(s32 v);

void fn_8007BBA0(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BBD8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BC10(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BC48(u8* p0) {
    fn_800E0B38((*(s32*)p0 & 0xFF));
    fn_800E25E0();
}

void fn_8007BC74(u8* p0) {
    Session_SetNumPlayers((*(s32*)p0 & 0xFF));
    fn_800E25E0();
}

void fn_8007BCA0(void) {
}

void fn_8007BCA4(void) {
    fn_80079AD4();
}

void fn_8007BD18(void) {
}

void fn_8007BD1C(u8* p0) {
    fn_800E14E0((*(s32*)p0 & 0xFF));
}

void fn_8007BD44(u8* p0) {
    fn_800E1260(0);
    fn_800E1404(((*(s32*)p0 & 0xFF) - 1));
}

void fn_8007BD84(u8* p0) {
    fn_800E1260((*(s32*)p0 & 0xFF));
}

void fn_8007BDFC(u8* p0) {
    GetGolferName(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

void fn_8007BEEC(void) {
}

void fn_8007C12C(void* arg0) {
    if (Game_GetMode() == 5) {
        (*(s8*)((u8*)(lbl_80281ED4) + 0x11703)) = 1;
    }
    Session_SetGolfer((*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)(arg0) + 0)));
}

u8* fn_8007C17C(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(gSession + 0x2C);
    return gSession;
}

void fn_8007C218(s32 p0, u8* p1) {
    s32 t0;
    t0 = Game_GetMode();
    *(s32*)p1 = t0;
}

void fn_8007C248(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 1;
}

void fn_8007C254(s32* arg0) {
    u8 temp_r5;

    temp_r5 = (*(u8*)((u8*)(lbl_80281EE0) + 0x86));
    (*(u8*)((u8*)(lbl_80281EE0) + 0x86)) = (u8) *arg0;
    if ((temp_r5 != (u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x86))) && ((s32) (*(s32*)((u8*)(lbl_80281EE0) + 0)) == 3)) {
        fn_8008E354();
    }
}

void fn_8007C330(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80077A80(*(s32*)p0);
    strcpy(*(char**)(*(u8**)p1 + 0x8), (char*)t0 + 34);
}

void fn_8007C488(void) {
}

void fn_8007C48C(u8* p0) {
    fn_8008F80C(*(s32*)p0, (*(s32*)(p0 + 0x4) & 0xFF));
}

void fn_8007C4B8(void) {
    fn_8009CD10();
}

void fn_8007C4D8(void) {
    fn_8009CD7C();
}

void fn_8007C698(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 var_r0;

    temp_r3 = fn_800A2100((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0);
    var_r0 = 1;
    if (temp_r3 != 0) {
        var_r0 = temp_r3;
    }
    *arg1 = var_r0;
}

void fn_8007C784(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 30;
}

void fn_8007C790(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 1;
}

void fn_8007C79C(u8* p0, u8* p1) {
    *(s32*)p1 = (*(s32*)p0 + 30);
}

void fn_8007C7AC(void) {
}

void fn_8007C7B0(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80077A80(*(s32*)p0);
    *(s32*)p1 = ((s8)*(u8*)(((u8*)t0) + 0x8E));
}

void fn_8007C94C(void) {
}

void fn_8007C950(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009F7E8(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

u8* fn_8007C988(u8* p0) {
    *(s32*)(lbl_801D880C + 0x4) = *(s32*)p0;
    *(s32*)lbl_801D880C = 0;
    return lbl_801D880C;
}

void fn_8007CD58(void** arg0, f32* arg1) {
    *arg1 = lbl_80283AE8 * fn_80012C30((*(s32*)((u8*)(*arg0) + 8)));
}

void fn_8007CDF0(void) {
}

void fn_8007CDF4(u8* p0) {
    fn_800EAC94(*(s32*)p0);
    fn_800EAE38();
}

void fn_8007CE1C(void) {
}

void fn_8007CE20(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80077A80(*(s32*)p0);
    *(s32*)p1 = *(u8*)(((u8*)t0) + 0x1);
}

void fn_8007CE58(u8* p) {
    fn_80102308(*(s32*)(p + 0x0));
}

void fn_8007D25C(void) {
}

void fn_8007D260(void) {
}

void fn_8007D264(void) {
}

void fn_8007D268(void) {
}

void fn_8007D26C(void) {
}

void fn_8007D270(s32* arg0) {
    if ((s32) *arg0 != 0) {
        (*(s8*)((u8*)(lbl_80281ED4) + 0x11702)) = 1;
        return;
    }
    (*(s8*)((u8*)(lbl_80281ED4) + 0x11702)) = 0;
}

void fn_8007D2D0(void) {
}

void fn_8007D380(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80077ACC();
    *(s32*)p1 = *(s32*)(((u8*)t0) + 0x6C);
}

void fn_8007D3B4(void) {
    fn_80077ACC();
    fn_80057438();
}

void fn_8007D3D8(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_801020C0();
    *(s32*)p1 = t0;
}

void fn_8007D408(void) {
}

void fn_8007D40C(void) {
}

void fn_8007D410(void) {
}

void fn_8007D414(void) {
}

void fn_8007D418(void) {
}

void fn_8007D41C(void) {
}

void fn_8007D420(void) {
}

void fn_8007D424(void) {
}

void fn_8007D6D8(void) {
}

void fn_8007D6DC(void) {
}

void fn_8007D6E0(void* arg0, s32* arg1) {
    if ((s32) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        *arg1 = 0;
        return;
    }
    *arg1 = (*(s32*)((u8*)(arg0) + 0)) + 0x1E;
}

void fn_8007D76C(u8* p0) {
    s32 t0;
    t0 = fn_80077ACC();
    *(s32*)(((u8*)t0) + 0x6C) = *(s32*)(p0 + 0x4);
}

void fn_8007D7E4(void) {
    s32 t0;
    t0 = fn_800770FC();
    *(s32*)((u8*)t0) = 2;
    fn_800A75B4();
}

u8* fn_8007D924(u8* p0) {
    *(u8*)(gSession + 0x10) = *(s32*)p0;
    return gSession;
}

void fn_8007D938(u8* p0) {
    fn_800E1404(((*(s32*)p0 & 0xFF) - 1));
}

void fn_8007D964(void) {
}

void fn_8007D9D0(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x2));
}

void fn_8007DAB0(u8* p0, u8* p1) {
    *(s32*)p1 = *(u8*)(((u8*)gpSaveData) + (*(s32*)p0 * 67072));
}

void fn_8007DAD0(void) {
}

u8* fn_8007DAD4(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(lbl_801D87C0 + 0x38);
    return lbl_801D87C0;
}

void fn_8007DB28(void) {
}

void fn_8007DB2C(void) {
}

void fn_8007DB30(void) {
}

void fn_8007DB34(void) {
}

void fn_8007DB38(void) {
}

u8* fn_8007E0BC(u8* p0) {
    *(u8*)(gSession + 0x5B38) = *(s32*)p0;
    return gSession;
}

void fn_8007E0D0(u8* p0) {
    fn_800142A4(((s8)*(s32*)p0));
}

void fn_8007E194(s32* arg0) {
    if (Game_GetMode() == 7) {
        (*(s32*)((u8*)(gpGame) + 8)) = 0;
        return;
    }
    if (Game_GetMode() == 9) {
        (*(s32*)((u8*)(gpGame) + 8)) = 1;
        return;
    }
    (*(s32*)((u8*)(gpGame) + 8)) = (s32) *arg0;
}

void fn_8007E200(void) {
}

void fn_8007E288(void) {
}

void fn_8007E354(void) {
}

void fn_8007E51C(s32 p0, s32 p1) {
    fn_8007E458(0, p0, p1);
}

void fn_8007E548(s32 p0, s32 p1) {
    fn_8007E458(1, p0, p1);
}

void fn_8007E574(s32 p0, s32 p1) {
    fn_8007E458(2, p0, p1);
}

void fn_8007E5A0(s32 p0, s32 p1) {
    fn_8007E458(3, p0, p1);
}

void fn_8007E5CC(s32 p0, s32 p1) {
    fn_8007E458(4, p0, p1);
}

void fn_8007E5F8(s32 p0, s32 p1) {
    fn_8007E458(5, p0, p1);
}

void fn_8007E624(s32 p0, s32 p1) {
    fn_8007E458(6, p0, p1);
}

void fn_8007E650(s32 p0, s32 p1) {
    fn_8007E458(7, p0, p1);
}

void fn_8007E744(void) {
}

void fn_8007E748(void) {
}

void fn_8007E798(void) {
}

void fn_8007E85C(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 var_r0;

    temp_r3 = fn_800A1164((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)((*(void**)((u8*)(arg0) + 8))) + 8)), (*(s32*)((u8*)(arg0) + 0xC)));
    var_r0 = 1;
    if (temp_r3 != 0) {
        var_r0 = temp_r3;
    }
    *arg1 = var_r0;
}

void fn_8007E8B4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 1;
}

void fn_8007E8C0(void) {
}

u8* fn_8007E8C4(u8* p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + *(s32*)p0);
    return lbl_801D7148;
}

u8* fn_8007E8DC(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0xF);
    return lbl_801D7148;
}

u8* fn_8007E8F0(u8* p0) {
    *(u8*)(lbl_801D7148 + 0xF) = *(s32*)p0;
    return lbl_801D7148;
}

void fn_8007E92C(u8* p0) {
    *(u8*)(((u8*)lbl_80281EE0) + 0x83) = *(s32*)p0;
}

void fn_8007E9A0(void) {
}

void fn_8007E9A4(void) {
}

u8* fn_8007E9A8(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0x11);
    return lbl_801D7148;
}

void fn_8007EE7C(void) {
}

void fn_8007EE80(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x5391) = *(s32*)(p0 + 0x4);
}

void fn_8007EF9C(void) {
}

void fn_8007F088(u8* p0, u8* p1) {
    s32 t0;
    t0 = GM_vGetAllTimeRecordsHeld((gpSaveData + (*(s32*)p0 * 67072)), p1, gpSaveData);
    *(s32*)p1 = t0;
}

u8* fn_8007FCC0(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(lbl_801D7148 + 0x14);
    return lbl_801D7148;
}

u8* fn_8007FCD4(u8* p0) {
    *(s32*)(lbl_801D7148 + 0x14) = *(s32*)p0;
    return lbl_801D7148;
}

void fn_8007FEAC(u8* p0) {
    fn_800907AC(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

u8* fn_8007FED8(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0x18);
    return lbl_801D7148;
}

void fn_8007FF3C(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(((u8*)lbl_80281EE0) + 0x86);
}

void fn_8008017C(u8* p0) {
    strcpy(*(char**)(*(u8**)p0 + 0x8), *(char**)(lbl_80191990 + (*(s32*)(gReplayData + 0xF00) << 2)));
}

u8* fn_800801C0(s32 p0, u8* p1) {
    *(s32*)p1 = *(s16*)(gReplayData + 0xF04);
    return gReplayData;
}

void fn_800801D4(u8* p0) {
    strcpy(*(char**)(*(u8**)p0 + 0x8), (char*)(gReplayData + 0x32));
}

void fn_80080300(void) {
}

void fn_80080358(u8* p0) {
    fn_8009CD80(*(s32*)p0, *(s32*)(p0 + 0x4));
    fn_8009CD7C();
}

void fn_800804D8(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_800804E4(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E22E4(*(s32*)p0, *(s32*)(p0 + 0x4), (*(s32*)(p0 + 0x8) - 1), p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_800805C4(void) {
    s32 t0;
    t0 = fn_800770FC();
    *(s32*)((u8*)t0) = 1;
    fn_800A75B4();
}

void fn_800805F0(void) {
}

void fn_800807D0(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80080AD0(u8* p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)((u8*)*(s32*)(((u8*)*(s32*)p0) + 0x8)));
}

void fn_80080C60(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x4));
}

void fn_80080C74(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x4) = *(s32*)p0;
}

void fn_80080C84(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x5));
}

void fn_80080C98(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x5) = *(s32*)p0;
}

void fn_80080CA8(u8* p0) {
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = 0;
    *(s32*)((u8*)*(s32*)(p0 + 0xC)) = 0;
    *(s32*)((u8*)*(s32*)(p0 + 0x10)) = 0;
}

void fn_800819FC(void* arg0) {
    (*(u8*)((u8*)(gpGame) + 0x136)) = (u8) (*(s32*)((u8*)(arg0) + 0));
    (*(s32*)((u8*)(gpGame) + 0x13C)) = (s32) (*(s32*)((u8*)(arg0) + 4));
    (*(s32*)((u8*)(gpGame) + 0x140)) = (s32) (*(s32*)((u8*)(arg0) + 8));
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x136)) != 0) {
        GM_SetupCustomHoleSelection();
        fn_800E1434();
    }
}

void fn_80081BD4(void) {
    fn_80077780();
}

void fn_80081BF4(u8* p) {
    fn_80077808(*(s32*)(p + 0x0));
}

void fn_80081CF8(void) {
}

void fn_80081F98(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x1) = *(s32*)p0;
}

void fn_80081FA8(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x1));
}

void fn_80082608(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x620));
}

void fn_8008266C(void) {
    *(u8*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x620) = 0;
}

void fn_80082680(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A218C(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_800826C4(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A2194(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_80082708(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_8009EB44(*(s32*)p0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t0;
    t1 = fn_800A2628();
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = t1;
}

void fn_80082758(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A2604(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80082790(u8* p0) {
    s32 t0;
    t0 = fn_800A2614(*(s32*)p0);
    strcpy(*(char**)(*(u8**)(p0 + 0x4) + 0x8), (char*)t0);
}

void fn_80082800(u8* p0) {
    *(s32*)((u8*)*(s32*)p0) = 2;
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = 1;
}

void fn_8008281C(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 7;
}

u8* fn_80082928(void) {
    *(s32*)(gSession + 0xC) = 2;
    return gSession;
}

void fn_8008293C(u8* p0) {
    strcpy(*(char**)(*(u8**)(p0 + 0x4) + 0x8), *(char**)(lbl_80191990 + (*(s32*)p0 << 2)));
}

void fn_80082978(void) {
}

void fn_8008297C(void) {
}

void fn_80082980(u8* p0) {
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = *(s32*)p0;
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = 0;
}

void fn_800829D4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 2;
}

void fn_800829E0(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 4;
}

void fn_800829EC(u8* p) {
    fn_80077968(*(s32*)(p + 0x0));
}

void fn_80082A10(void** arg0, s32* arg1) {
    *arg1 = 0;
    strcpy(*(char**)((u8*)(*arg0) + 8), "");
}

void fn_80082A44(void) {
}

void fn_80082A48(void) {
}

void fn_80082A4C(void) {
}

void fn_80082C74(u8* p0) {
    *(u8*)(((u8*)gpSaveData) + (*(s32*)p0 * 67072)) = 0;
    *(u8*)(lbl_801D7148 + *(s32*)p0) = 0;
}

void fn_80082CA4(void) {
}

void fn_80082CA8(u8* p0) {
    fn_800ED650(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC));
}

void fn_80082CDC(u8* p0, u8* p1) {
    s32 t0;
    t0 = strlen(*(char**)(*(u8**)p0 + 0x8));
    *(s32*)p1 = t0;
}

void fn_80082D98(s32* arg0) {
    (*(s8*)((u8*)(lbl_80281ED4) + 3)) = (s8) *arg0;
}

void fn_80082DBC(void* arg0, s32* arg1) {
    if (fn_8009EE28((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0) == -0x12) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

void fn_80082E10(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 var_r0;

    temp_r3 = fn_800A0A7C((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0);
    var_r0 = 1;
    if (temp_r3 != 0) {
        var_r0 = temp_r3;
    }
    *arg1 = var_r0;
}

void fn_80083354(void) {
}

void fn_80083358(s32* arg0) {
    s32* temp_r3;

    temp_r3 = (s32*)fn_800770FC();
    *temp_r3 = 3;
    if ((s32) *arg0 == -1) {
        *temp_r3 = 1;
    }
    fn_800A75B4();
}

void fn_800833A4(void) {
    fn_800A75B4();
}

void fn_800833C4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80083430(s32* arg0, s32* arg1) {
    if ((u32) (fn_80013050(*arg0) + 0x74F00000) == 0U) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}

u8* fn_80083480(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0x10);
    return lbl_801D7148;
}

u8* fn_80083494(u8* p0) {
    *(u8*)(lbl_801D7148 + 0x10) = *(s32*)p0;
    return lbl_801D7148;
}

void fn_800834A8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009F728(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_800834DC(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_800835B8(void) {
}

void fn_800835BC(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_800835C8(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80083890(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 150;
}

void fn_8008389C(void) {
}

void fn_800838A0(void) {
}

void fn_800838A4(void) {
}

void fn_800838A8(void) {
}

void fn_800838AC(void) {
}

void fn_800838B0(void) {
}

void fn_800838B4(void) {
}

void fn_800838B8(void) {
}

void fn_800838BC(void) {
}

void fn_800838C0(void) {
}

void fn_800838C4(u8* p0, u8* p1) {
    s32 t0;
    t0 = strcmp(*(char**)(*(u8**)p0 + 0x8), *(char**)(*(u8**)(p0 + 0x4) + 0x8));
    *(s32*)p1 = t0;
}

void fn_80083904(void) {
}

void fn_80083908(void) {
}

void fn_8008390C(void) {
}

void fn_80083910(void) {
}

void fn_80083914(void) {
}

void fn_80083918(void) {
}

void fn_8008391C(void) {
}

void fn_80083920(void) {
}

void fn_80083924(void) {
}

void fn_80083928(void) {
}

void fn_8008392C(void) {
}

void fn_80083930(void) {
}

void fn_80083970(void) {
}

void fn_80083974(void* arg0) {
    s32 temp_r0;
    s32 temp_r5;

    temp_r0 = (*(s32*)((u8*)(arg0) + 8)) % 3;
    temp_r5 = (((*(s32*)((u8*)(arg0) + 0xC)) % 10) + 1) * 0x19;
    switch (temp_r0) {
    case 0:
        *(*(s32**)((u8*)(arg0) + 0x10)) = temp_r5;
        *(*(s32**)((u8*)(arg0) + 0x14)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x18)) = 0x32;
        return;
    case 1:
        *(*(s32**)((u8*)(arg0) + 0x10)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x14)) = temp_r5;
        *(*(s32**)((u8*)(arg0) + 0x18)) = 0x32;
        return;
    case 2:
        *(*(s32**)((u8*)(arg0) + 0x10)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x14)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x18)) = temp_r5;
        return;
    }
}

void fn_80083A44(void) {
}

void fn_80083A48(void) {
}

void fn_80083BA4(u8* p) {
    fn_8010D334(*(s32*)(p + 0x0));
}

void fn_80083E48(void) {
    fn_80123FF8();
    fn_8012408C(0);
}

void fn_80083E70(void) {
    fn_8012408C(6);
}

void fn_80083E94(void) {
    fn_8012408C(8);
}

void fn_80083EB8(void) {
}

void fn_80083EBC(void) {
    fn_8012408C(18);
}

void fn_80083F54(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_8008410C(s32** arg0, s32* arg1) {
    s32 temp_r31;
    s32* temp_r3;

    temp_r3 = *arg0;
    temp_r31 = *temp_r3;
    *arg1 = temp_r31 + fn_8012411C(temp_r3);
    fn_80124138(*arg1);
}

void fn_80084158(void) {
}

void fn_8008415C(void) {
}

void fn_80084160(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_801241CC();
    *(s32*)p1 = t0;
}

void fn_80084190(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80124174();
    *(s32*)p1 = t0;
}

void fn_80084208(void) {
    fn_8012408C(5);
}

void fn_8008422C(u8* p0) {
    fn_80125600(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4));
}

void fn_80084258(u8* p0) {
    fn_80125648(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
}

void fn_80084288(u8* p) {
    fn_8012566C(*(s32*)(p + 0x0));
}

void fn_800842AC(u8* p) {
    fn_80125680(*(s32*)(p + 0x0));
}

void fn_800842D0(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 temp_r3_2;

    temp_r3 = fn_80125354((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)));
    (*(s32*)((u8*)(lbl_80281ED4) + 0x11704)) = temp_r3;
    if (temp_r3 != 0) {
        *arg1 = 0;
        return;
    }
    temp_r3_2 = fn_801253F0((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)));
    (*(s32*)((u8*)(lbl_80281ED4) + 0x11704)) = temp_r3_2;
    *arg1 = temp_r3_2 == 0;
}

void fn_800844E0(void* arg0, s32* arg1) {
    s32 temp_r3;

    temp_r3 = fn_80125434((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0);
    (*(s32*)((u8*)(lbl_80281ED4) + 0x11704)) = temp_r3;
    if (temp_r3 != 0) {
        *arg1 = 0;
        return;
    }
    *arg1 = temp_r3 == 0;
}

void fn_80084544(s32 p0, u8* p1) {
    s32 t0;
    t0 = EASBio_IsBioLoaded();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_800845D4(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80125528(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80084678(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80125280(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x1704) = t0;
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_800846C8(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_800846D4(s32** arg0) {
    s32* temp_r31;

    temp_r31 = *arg0;
    *temp_r31 = fn_80125928();
}

void fn_80084750(void) {
}

void fn_80084754(s32 p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801254EC();
    *(s32*)p1 = t0;
    t1 = fn_801254B8();
    *(s32*)p1 = t1;
}

void fn_8008478C(s32 p0, u8* p1) {
    f32 t0;
    t0 = fn_80124BDC();
    *(f32*)p1 = t0;
}

void fn_800847BC(u8* p) {
    fn_80084FF0(*(s32*)(p + 0x0));
}

void fn_800848E4(void) {
}

void fn_800848E8(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800EAE6C();
    *(s32*)p1 = t0;
}

void fn_80084918(u8* p0) {
    fn_800EAE44((*(s32*)p0 - 1));
}

void fn_80084940(u8* p0) {
    s32 t0;
    t0 = fn_800ED280((*(s32*)(p0 + 0x4) - 1));
    strcpy(*(char**)(*(u8**)p0 + 0x8), (char*)t0);
}

void fn_80084984(u8* p0) {
    s32 t0;
    t0 = fn_800ED2C8((*(s32*)(p0 + 0x4) - 1));
    strcpy(*(char**)(*(u8**)p0 + 0x8), (char*)t0);
}

void fn_80084B88(s32* arg0) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        fn_8010F2FC();
        return;
    case 1:
        fn_8010F3A4();
        return;
    case 3:
        fn_8011DF90();
        /* fallthrough */
    case 2:
        return;
    }
}

void fn_80084BE4(void) {
}

void fn_80084D6C(void* arg0) {
    strcpy((*(char**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8)), (*(char**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8)));
    if (strlen((*(char**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8))) > 0xCU) {
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0xB)) = 0;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0xA)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 9)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 8)) = 0x2E;
    }
}

void fn_80084DF4(void* arg0) {
    strcpy((*(char**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8)), (*(char**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8)));
    if (strlen((*(char**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8))) > 0x20U) {
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0x1F)) = 0;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0x1E)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0x1D)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0x1C)) = 0x2E;
    }
}

void fn_80084E7C(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x1704);
}

void fn_80084E90(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80125194(0, 0);
    *(s32*)p1 = t0;
}

void fn_80084EC8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009D390(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = t0;
}

void fn_80084F04(void) {
}

void fn_80084F08(void) {
}

void fn_80084F0C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800A27F4();
    *(s32*)p1 = t0;
}

void fn_80084F3C(void) {
}

void fn_80084F40(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_801252D0(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_80084F84(s32 p0, u8* p1) {
    s32 t0;
    t0 = EASBio_GetCurrentRewardMessage();
    *(s32*)p1 = t0;
}

void fn_80084FF0(s32 v) {
    lbl_80281FFC = v;
}

// ---- end of sweep code ----
