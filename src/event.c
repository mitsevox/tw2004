// event.c (TW06's golf/eventmanager/event.c): the game's event handlers. EVENT_Trigger calls the
// handler for an event number from the file's table (0x80188628): stepping through the clubs, the
// end of a shot and the other things that happen in play. Not decompiled yet beyond the functions
// below.

#include "game.h"
#include "terrain.h"
#include "sitdev.h"

void fn_80069A84();
void fn_80069AFC();
void fn_80069B74();
void fn_80069BEC();
void fn_800664D8(void);
void fn_800664F8(void);
void fn_80066518(void);
void fn_80066538(void);
void fn_800670A8();
void fn_80066994(void);
void fn_800669B4(void);
void fn_800669D4(void);
void fn_800669F4(void);
void fn_80067710();
void fn_80066A14(s32 p0);
void fn_800DC498();
void fn_80066BE8(void);
void fn_80066C08(void);
void fn_800A67E8();
void fn_800A6854();
void fn_80066D0C(s32 p0);
void fn_80066D30(s32 p0);
void fn_80066D54(void);
void fn_80066D58(void);
void fn_80066D5C(void);
void fn_80066D60(void);
void fn_80066D64(void);
void fn_80066D68(void);
void fn_80066D6C(void);
void fn_80066D70(void);
void fn_80066D74(void);
void fn_800A64A8();
void fn_800A6660();
void fn_80066E6C(s32 p0);
void fn_80066E90(s32 p0);
void fn_80066EB8(s32 p0);
void fn_80066F30(s32 p0);
void fn_80066F58(s32 p0);
void fn_80066F80(s32 p0);
void fn_80066FA8(void);
void fn_80066FAC(void);
void fn_80066FB0(void);
void fn_80066FB4(void);
void fn_80067004(s32 p0);
void fn_8006752C(void);
void fn_80067550(void);
void fn_80067554(void);
void fn_80067558(void);
void fn_8006755C(void);
void fn_8006765C(void);
void fn_800676AC(void);
void fn_800676E8(void);

void fn_800664D8(void) {
    fn_80069BEC();
}

void fn_800664F8(void) {
    fn_80069B74();
}

void fn_80066518(void) {
    fn_80069A84();
}

void fn_80066538(void) {
    fn_80069AFC();
}

void fn_80066994(void) {
    fn_800670A8();
}

void fn_800669B4(void) {
    fn_800670A8();
}

void fn_800669D4(void) {
    fn_800670A8();
}

void fn_800669F4(void) {
    fn_800670A8();
}

void fn_80066A14(s32 p0) {
    fn_80067710(p0, 2, 29);
}

void fn_80066BE8(void) {
    fn_800DC498();
}

void fn_80066C08(void) {
    fn_800335F8(0);
}

void fn_80066D0C(s32 p0) {
    fn_800A67E8((p0 & 0xFF));
}

void fn_80066D30(s32 p0) {
    fn_800A6854((p0 & 0xFF));
}

void fn_80066D54(void) {
}

void fn_80066D58(void) {
}

void fn_80066D5C(void) {
}

void fn_80066D60(void) {
}

void fn_80066D64(void) {
}

void fn_80066D68(void) {
}

void fn_80066D6C(void) {
}

void fn_80066D70(void) {
}

void fn_80066D74(void) {
}

void fn_80066E6C(s32 p0) {
    fn_800A6660((p0 & 0xFF));
}

void fn_80066E90(s32 p0) {
    fn_800A64A8((p0 & 0xFF), 1);
}

void fn_80066EB8(s32 p0) {
    fn_800A6660((p0 & 0xFF));
}

void fn_80066F30(s32 p0) {
    fn_80067710(p0, 4, 22);
}

void fn_80066F58(s32 p0) {
    fn_80067710(p0, 8, 23);
}

void fn_80066F80(s32 p0) {
    fn_80067710(p0, 8, 24);
}

void fn_80066FA8(void) {
}

void fn_80066FAC(void) {
}

void fn_80066FB0(void) {
}

void fn_80066FB4(void) {
    fn_80035574();
}

void fn_80067004(s32 p0) {
    fn_80067710(p0, 9, 30);
}

void fn_8006752C(void) {
    fn_800E58B4(58);
}

void fn_80067550(void) {
}

void fn_80067554(void) {
}

void fn_80067558(void) {
}

void fn_8006755C(void) {
}

void fn_8006765C(void) {
    if (lbl_802811B8->pD0 != NULL) {
        fn_80009E70(lbl_802811B8->pD0);
    }
    fn_80009E70(lbl_802811B8->pCC);
    fn_80009E70(lbl_802811B8->pD4);
    lbl_80282208 = NULL;
}

void fn_800676AC(void) {
    lbl_80282210 = 0;
}

void fn_800676E8(void) {
    UStream_UnregisterHandler(1936941938);
}
