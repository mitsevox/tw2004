// GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree): the terrain manager
// (TerrainMgr, TW06's Ter_TerrainGameDataMgr): loads the hole's ground from the 'ter ', 'tgd ' and
// 'tLOD' chunks, keeps its patches and the lists they are drawn from, and draws them. The file ends
// with small setters of the renderer's state (RenderState), which share its constant pool.

#include "golfer.h"
#include "ball.h"
#include "engine.h"

void* fn_800073B4(u8* pData, int n);
void  fn_80030894(void);
void  fn_80030A40(void* p, int n);
s32   fn_800318AC(const void* pA, const void* pB);
void  fn_80031938(f32* pOut, f32 f, s32 a, s32 b, s32 c, s32 d);
void  fn_80032F88(void* pList, s32 nCount, s32 n3, s32 n4);
void  fn_800341A4(UStreamObject* pObject);
void  fn_800342B4(UStreamObject* pObject);
void  fn_800342F0(UStreamObject* pObject);
void  fn_80035098(u8 b);

// ---- sweep code (not yet cleaned up) ----

s32 fn_800354BC(s32);
void* fn_800354C4();
s32 fn_800354D0(s32, s32);
s32 fn_800354E4(s32, s32);
s32 fn_800354F4(s32);

void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0) {
    s32 temp_r31;

    (*(f32*)((u8*)(arg3) + 8)) = farg0;
    (*(f32*)((u8*)(arg3) + 0xC)) = (f32) (*(f32*)((u8*)(fn_800354C4()) + 0xC));
    (*(s32*)((u8*)(arg3) + 0x10)) = arg1;
    (*(s32*)((u8*)(arg3) + 0x14)) = arg2;
    temp_r31 = fn_800354F4(arg0);
    (*(s32*)((u8*)(arg3) + 0)) = fn_800354E4(arg0, 0);
    (*(s32*)((u8*)(arg3) + 0x18)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 3);
    (*(s32*)((u8*)(arg3) + 0x1C)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 2);
    (*(s32*)((u8*)(arg3) + 0x20)) = fn_800354D0(arg0, 1);
    if (temp_r31 >= 2) {
        (*(s32*)((u8*)(arg3) + 4)) = fn_800354BC((*(s32*)((u8*)(arg3) + 0)));
        return;
    }
    (*(s32*)((u8*)(arg3) + 4)) = 0;
}

// ---- end of sweep code ----

// The terrain's chunk loaders.
void fn_800307C0(void) {
    UStream_RegisterHandler('ter ', fn_800342B4);
    UStream_RegisterHandler('tgd ', fn_800342F0);
    UStream_RegisterHandler('tLOD', fn_800341A4);
}

void fn_80030818(void) {
    UStream_UnregisterHandler('ter ');
    UStream_UnregisterHandler('tgd ');
}

void fn_8003084C(void) {
    fn_80035098(0);
    fn_80012F50(1, 6, 128);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012EF8();
}

// Sorts p18 by f10, except when gSession.b11 is set.
void fn_8003185C(void) {
    if (gSession.b11 == 0) {
        fn_8015929C(lbl_801D3CB0.p18, lbl_801D3CB0.n10D0, sizeof(TerSortItem), fn_800318AC);
    }
}

// fn_8003185C's comparison: by f10, smallest first.
s32 fn_800318AC(const void* pA, const void* pB) {
    f32 fA = ((const TerSortItem*)pA)->f10;
    f32 fB = ((const TerSortItem*)pB)->f10;

    if (fA > fB) return 1;
    if (fA < fB) return -1;
    return 0;
}

void fn_800318D8(void) {
    fn_80031938(lbl_801D3CB0.a1110, lbl_802810D8, lbl_802810D0, lbl_802810D4, lbl_802810DC, lbl_802810E0);
    lbl_801D3CB0.f11A8 = lbl_801D3CB0.f11A0 * lbl_801D3CB0.f11A4;
    lbl_801D3CB0.f11A8 = lbl_801D3CB0.f11A8 * lbl_801D3CB0.f11A8;
}

// Whether the 'tLOD' chunk has been loaded.
u8 fn_80031E40(void) {
    return lbl_802810E4 != -1;
}

void fn_8003272C(int n) {
    if (lbl_801D3CB0.b11AC) {
        fn_80012F34(n);
        lbl_802810CC = n;
    }
}

void fn_80032954(void) {
    fn_80032F88(lbl_801D3CB0.p1C, lbl_801D3CB0.n10D4, lbl_801D3CB0.n11B8, lbl_801D3CB0.n11BC);
    if (lbl_801D3CB0.n10D8 != 0) {
        fn_80032F88(lbl_801D3CB0.p20, lbl_801D3CB0.n10D8, lbl_801D3CB0.n11B8, lbl_801D3CB0.n11BC);
    }
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

void fn_80032AEC(void) {
    fn_80030894();
    fn_80032F88(lbl_801D3CB0.p30, lbl_801D3CB0.n10E4, lbl_801D3CB0.n11B8, lbl_801D3CB0.n11BC);
    fn_8003272C(0);
    fn_80032F88(lbl_801D3CB0.p24, lbl_801D3CB0.n10DC, lbl_801D3CB0.n11B8, lbl_801D3CB0.n11BC);
    fn_8003272C(1);
    fn_80035098(0);
    fn_8003084C();
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

void fn_800332F4(void) {
    lbl_802810CC = -1;
    lbl_802810C8 = -1.0f;
}

f32 fn_800336E4(void) {
    return lbl_801D3CB0.f1178;
}

f32 fn_800336F4(void) {
    return lbl_801D3CB0.f1170;
}

// Sets the patch's n1C to 1 when bit 0 of its a20[3] is set.
void fn_80033704(u16 nRow, u16 nCol) {
    TerPatch* pPatch = &lbl_801D3CB0.p34[lbl_801D3CB0.aRowStart[nRow] + nCol];

    if (pPatch->a20[3] & 1) {
        pPatch->n1C = 1;
    }
}

// The 'ter ' chunk arrived.
void fn_800342B4(UStreamObject* pObject) {
    lbl_801D3CB0.p4 = pObject;
    lbl_801D3CB0.p0 = fn_800073B4(pObject->pData, 0);
}

void fn_80034648(int n) {
    lbl_801D3CB0.v1140[0] = lbl_801876D8[n][0];
    lbl_801D3CB0.v1140[1] = lbl_801876D8[n][1];
    lbl_801D3CB0.v1140[2] = lbl_801876D8[n][2];
}

void fn_800349CC(int n) {
    if (lbl_801D3CB0.p0 != NULL) {
        fn_80030894();
        fn_80030A40(lbl_801D3CB0.p0, n);
        fn_8003084C();
    }
}

// Sets b11AC and returns what it was.
u8 fn_8003505C(u8 b) {
    u8 bOld = lbl_801D3CB0.b11AC;

    lbl_801D3CB0.b11AC = b;
    return bOld;
}

f32 fn_80035074(f32 x) {
    return floor(x);
}

// ---- the renderer's state ----

void fn_80035098(u8 b) {
    lbl_801B8980.b1D = b;
    lbl_801B8980.u110 |= 0x80;
}

void fn_800350B4(f32 f) {
    lbl_801B8980.f2C = f;
    lbl_801B8980.u110 |= 0x8;
}

void fn_800350D0(f32 f) {
    lbl_801B8980.f28 = f;
    lbl_801B8980.u110 |= 0x8;
}

void fn_800350EC(u8 r, u8 g, u8 b) {
    lbl_801B8980.a30[0] = r;
    lbl_801B8980.a30[1] = g;
    lbl_801B8980.a30[2] = b;
    lbl_801B8980.a30[3] = 0x80;
    lbl_801B8980.u110 |= 0x8;
}

void fn_80035118(int a, int b) {
    lbl_801B8980.n10 = a;
    lbl_801B8980.n14 = b;
    lbl_801B8980.u110 |= 0x10;
}

void fn_80035138(int a) {
    lbl_801B8980.nFC = a;
    lbl_801B8980.u110 |= 0x400;
}

void fn_80035154(u8 b) {
    lbl_801B8980.b1C = b;
    lbl_801B8980.u110 |= 0x80;
}

void fn_80035170(u32 uClear, u32 uSet) {
    lbl_801B8980.u20 &= ~uClear;
    lbl_801B8980.u20 |= uSet;
    lbl_801B8980.u110 |= 0x20;
}

// Where n frames falls in a cycle of fPeriod seconds, in seconds.
f32 fn_800351D8(u32 n, f32 fPeriod) {
    return FRAME_TIME * (f32)(n % (u32)(FRAME_RATE * (0.5f / FRAME_RATE + fPeriod)));
}

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80280DF0;
void fn_80013D68();
void fn_80013D9C();
void fn_80035240(s32 p0);
void fn_8003526C(void);
void fn_80035294(void);
void fn_800352BC(void);
extern u8* lbl_80281380;
void fn_80035398(void);
void fn_8003541C();
void fn_80035440();
void fn_800352E4(void);
void fn_80035308(void);
s32 fn_8003532C(void);
void fn_80035338(s32 p0);
void fn_8006F334();
void fn_8003534C(void);
void fn_8006EDC0();
void fn_80035370(void);

void fn_80035240(s32 p0) {
    fn_80013D9C(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

void fn_8003526C(void) {
    fn_80012EF0(*(void**)lbl_80280DF0);
}

void fn_80035294(void) {
    fn_80013CCC(*(void**)lbl_80280DF0);
}

void fn_800352BC(void) {
    fn_80013D68(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800352E4(void) {
    fn_8003541C();
    fn_80035398();
}

void fn_80035308(void) {
    fn_8003532C();
    fn_80035440();
}

s32 fn_8003532C(void) {
    return *(s32*)(lbl_80281380 + 0x230);
}

void fn_80035338(s32 p0) {
    *(s32*)(((u8*)lbl_80281380) + 0x230) = (s32)(lbl_80281380 + (p0 * 140));
}

void fn_8003534C(void) {
    fn_8003532C();
    fn_8006F334();
}

void fn_80035370(void) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8006EDC0((t0 + 84));
}

// ---- end of sweep code ----

// Hands the renderer the colour and the two distances made from the current settings.
void fn_80035398(void) {
    fn_800350EC(lbl_802811E0->f44, lbl_802811E0->f48, lbl_802811E0->f4C);
    fn_800350D0(0.375f * lbl_802811E0->f50);
    fn_800350B4(4.15f * (10.0f + lbl_802811E0->f50));
}
