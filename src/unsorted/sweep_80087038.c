// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80118E30();
s32 fn_8011908C();
s32 fn_801190D8();
s32 fn_80119118();
s32 fn_8011937C();
s32 fn_80119638();
s32 fn_801197CC();
s32 fn_80119808();
s32 fn_80119A04();
void strcpy();

void fn_80087038(u8* p0);
void fn_8008709C(u8* p0, u8* p1);
void fn_800870E4(u8* p0, u8* p1);
void fn_80087130(u8* p0, u8* p1);
void fn_8008719C(u8* p0, u8* p1);
void fn_800871E4(u8* p0, u8* p1);
void fn_80087238(s32 p0, u8* p1);
void fn_80087270(s32 p0, u8* p1);
void fn_80087038(u8* p0) {
    s32 t0;
    s32 t1;
    s32 t2;
    t0 = fn_801197CC(0, *(s32*)p0);
    t1 = fn_80119118(0, t0);
    t2 = fn_80118E30(0, t1);
    strcpy(*(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), t2, *(s32*)(p0 + 0x4));
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
