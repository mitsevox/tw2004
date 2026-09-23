// FE_CrAPMessages.c (our name, after EA's FE_PGATourMessages.c): the Create-A-Player screens'
// message handlers, registered in the front end's message table (FE_MessageTable.c). They read
// and set the golfer being created through the Create-A-Player database (FE_CrAPDB.c), the logo
// editor (FE_LogoDesign.c) and the menu golfer (FEgolferanim.c).

#include "engine.h"
#include "camera.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "charstate.h"
#include "game.h"

// FE_Manager.c
void fn_8007873C(SaveProfile* pProfile);
void fn_80078E34(SaveProfile* pProfile);
void fn_80079664(SaveProfile* pProfile);
void fn_8007975C(SaveProfile* pProfile, s16 nPart, int nChance);
int  fn_800797E0(SaveProfile* pProfile, s16 nPart, int b, int nChance);

void fn_80103B74(u8 b);                 // FE_CrAPDB.c: set the database's b14
void fn_801073DC(s16 nSlot);            // FE_CrAPDB.c
void fn_800A73F0(s32 n);

// ---- sweep code (not yet cleaned up) ----

void fn_801077EC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801048EC(pArgs[0].i, pArgs[1].i);
}

void fn_80107994(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80107998(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80107BA4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80107BF4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80105C44(pArgs[0].i, pArgs[1].i);
}

void fn_80107C30(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80108140(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801049C8(pArgs[0].i);
}

void fn_80108300(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->bCopy = pArgs[0].i;
}

void fn_80108398(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = fn_8001E9CC(pProfile->aB1CC, nAsset);
}

void fn_80108494(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077ACC();
    if (fn_80106244(pArgs[0].i) >= 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_801088A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7C0(pArgs[0].i);
}

void fn_801088C8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        fn_8010F7E4(LOGO_SQUARE);
        return;
    }
    fn_8010F7E4(LOGO_RECT);
}

void fn_80108904(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

void fn_801089BC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F880();
}

void fn_801089DC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F90C(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

void fn_80108B10(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = fn_8001E9CC(pProfile->aB344, nAsset);
}

void fn_80108B84(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    if (fn_8001E9CC(pProfile->aB344, nAsset)) {
        fn_8001EA34(pProfile->aB4BC, nAsset);
    }
}

void fn_80108E4C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = fn_8001E9CC(pProfile->a10548, pArgs[0].i);
}

// ---- end of sweep code ----

// The logo's name.
void fn_801091B8(MsgArg* pArgs, MsgArg* pResult) {
    LogoRecord* pLogo = fn_8010FB70();
    strcpy(((MsgString*)pArgs[1].p)->pStr, pLogo->szName);
    pResult->i = pLogo->b1020;
}

// Name the logo.
void fn_8010920C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(fn_8010FB70()->szName, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_80109354(MsgArg* pArgs, MsgArg* pResult) {
    fn_80104DB8(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr);
}

void fn_80109388(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010645C(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_80109430(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109434(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E364(pArgs[0].i);
}

void fn_80109458(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_8008E420();
}

void fn_8010948C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_8008E44C();
}

void fn_801094C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008B990() == 1;
}

void fn_801094FC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109500(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->bCopy;
}

// A palette colour's components.
void fn_80109618(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

// A pixel's colour index and components.
void fn_80109650(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FBCC(pArgs[0].i, pArgs[1].i, pArgs[2].p, pArgs[3].p, pArgs[4].p, pArgs[5].p);
}

// Set the logo's shape.
void fn_8010969C(MsgArg* pArgs, MsgArg* pResult) {
    LogoRecord* pLogo = fn_8010FB70();
    s32 nShape = pArgs[1].i;
    switch (nShape) {
    case LOGO_SQUARE:
        pLogo->nShape = LOGO_SQUARE;
        break;
    case LOGO_RECT:
        pLogo->nShape = LOGO_RECT;
        break;
    }
    fn_8010F7E4(nShape);
}

// The logo's shape.
void fn_80109700(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FB70()->nShape;
}

void fn_80109734(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109738(MsgArg* pArgs, MsgArg* pResult) {
    fn_80103B74(pArgs[0].i);
}

void fn_80109760(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E824();
}

void fn_801097FC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        fn_8008E6D4(0);
        return;
    case 1:
        fn_8008E6D4(1);
        return;
    case 2:
        fn_8008E6D4(4);
        return;
    case 3:
        fn_8008E6D4(5);
        return;
    case 4:
        fn_8008E6D4(2);
        return;
    default:
        fn_8008E6D4(0);
        return;
    }
}

void fn_8010988C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008DAEC();
}

void fn_801098AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109BA4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    fn_80103B74(0);
    fn_801073DC(2);
    fn_801073DC(5);
    fn_801073DC(6);
    fn_801073DC(7);
    fn_801073DC(8);
    fn_801073DC(11);
    fn_801073DC(12);
    fn_801073DC(13);
    fn_801073DC(14);
    fn_8007975C(pProfile, 0, 80);
    fn_8007975C(pProfile, 1, 0);
    fn_8007975C(pProfile, 2, 0);
    fn_8007975C(pProfile, 7, 0);
    fn_8007975C(pProfile, 19, 70);
    fn_8007975C(pProfile, 20, 70);
    fn_8007975C(pProfile, 8, 70);
    fn_800797E0(pProfile, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Idle" shot.
void fn_80109CBC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    View* pView = fn_80017028(fn_80016D10());
    fn_800A73F0((Rand_Next(0) & 7) + 11);
    fn_8008E244();
    fn_8008E364(0);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Idle", lbl_80281EE0->n4, 0, 0, 0);
    fn_80103B74(0);
    fn_80079664(pProfile);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Face" shot.
void fn_80109D5C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    View* pView = fn_80017028(fn_80016D10());
    fn_8008E244();
    fn_8008E364(1);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Face", lbl_80281EE0->n4, 0, 0, 0);
    fn_80103B74(0);
    fn_80078E34(pProfile);
}

void fn_80109DDC(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_801072CC(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

void fn_8010A208(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E724(0, 0, 0, 0);
}

void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80104F68(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

void fn_8010A3A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80104804();
}

void fn_8010A3C4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800D256C(pArgs[0].i);
}

// ---- end of sweep code ----
