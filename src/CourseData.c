#include "game_types.h"
#include "game.h"

void fn_800D29E8(void);
void fn_800D2A30(void);
void fn_800D2A64(UStreamObject* pObject);
void fn_800D2A90(UStreamObject* pObject);
s32 fn_800D2B80(int nCourse, int nHole, int nTee);
s32 fn_800D2C30(int nHole, int nTee);
s32 fn_800D2C68(int nTee);
int fn_800D30B4(int nRound);
int fn_800D3208(void);

void fn_800D29E8(void) {
}

void fn_800D2A30(void) {
    UStream_UnregisterHandler('CRI ');
    UStream_UnregisterHandler('CMPS');
}

void fn_800D2A64(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA2F4), lbl_801FA2F4);
}

void fn_800D2A90(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA1F8), lbl_801FA1F8);
}

int fn_800D2ABC(int nCourse, int nHole) {
    return lbl_801FA2F4[nCourse].aHoles[nHole].nPar;
}

int fn_800D2AD8(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].nPar;
}

s32 fn_800D2C30(int nHole, int nTee) {
    return fn_800D2B80(gpGame->nHoleCourse[nHole], gpGame->nHoleNum[nHole], nTee);
}

s32 fn_800D2C68(int nTee) {
    return fn_800D2B80(Game_GetCourse(), fn_80015464(), nTee);
}

int fn_800D3118(int nRound, int nHole) {
    return lbl_801FA1F8[fn_800D30B4(nRound)].aHoles[nHole].nCourse;
}

int fn_800D3208(void) {
    return fn_800D31A4(4) + fn_800D31A4(5);
}
