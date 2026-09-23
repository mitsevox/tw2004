// GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree): the terrain manager
// (TerrainMgr, TW06's Ter_TerrainGameDataMgr): loads the hole's ground from the 'ter ', 'tgd ' and
// 'tLOD' chunks, keeps its patches and the lists they are drawn from, and draws them. The file ends
// with small setters of the renderer's state (RenderState), which share its constant pool.

#include "golfer.h"
#include "ball.h"
#include "engine.h"

void* fn_800073B4(u8* pData, int n);
void  fn_800075CC(void* p);         // frees what fn_800073B4 made
void  fn_80019358(Character* pChar, f32* pDir, f32 f);
void  fn_8001BE88(Character* pChar, void* pClip, int n, f32 f);
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

void fn_800306B8(void) {
    fn_80009E70(lbl_801D3CB0.p10);
    fn_80009E70(lbl_801D3CB0.p14);
    fn_80009E70(lbl_801D3CB0.p18);
    fn_80009E70(lbl_801D3CB0.p1C);
    fn_80009E70(lbl_801D3CB0.p20);
    fn_80009E70(lbl_801D3CB0.p24);
    fn_80009E70(lbl_801D3CB0.p28);
    fn_80009E70(lbl_801D3CB0.p2C);
    fn_80009E70(lbl_801D3CB0.p30);
    fn_80009E70(lbl_801D3CB0.p34);
    if (lbl_801D3CB0.p0 != NULL) {
        fn_800075CC(lbl_801D3CB0.p0);
        lbl_801D3CB0.p0 = NULL;
    }
    if (lbl_801D3CB0.pCourse != NULL) {
        fn_80009E70(lbl_801D3CB0.pCourse);
        lbl_801D3CB0.pCourse = NULL;
    }
}

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

void fn_80031938(f32* pOut, f32 f, s32 a, s32 b, s32 c, s32 d) {
    s32 n = (f32)a * lbl_801D3CB0.f11A0;

    pOut[0] = 0.0f;
    pOut[5] = 0.0f;
    pOut[1] = (f32)(n + 1) * f;
    pOut[2] = pOut[1] - (f32)c * f;
    pOut[3] = pOut[2] + (f32)b * f;
    pOut[4] = pOut[3] - (f32)d * f;
}

void fn_80031A08(s32* pA, s32* pB, s32 a, s32 b) {
    // fake match: the (s32) and (int) casts of lbl_802810D8 are two conversions (the original
    // stores the one fctiwz result twice); the same cast everywhere shares one.
    s32 n = a + lbl_802810DC * (s32)lbl_802810D8;

    *pA = (f32)(n / (int)lbl_802810D8 - 1) / lbl_801D3CB0.f11A0;
    *pB = lbl_802810DC + (b + lbl_802810E0 * (s32)lbl_802810D8 - n) / (int)lbl_802810D8;
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

// Puts every patch with bits 0 and 1 of a20[0] back: n1C to 0, and f14 to f10, or with bReset
// f14 and n18 to 0.
void fn_800335F8(u8 bReset) {
    int i;

    lbl_801D3CB0.f116C = -1.0f;
    lbl_801D3CB0.f1178 = 0.0f;
    for (i = 0; i < TER_NUM_PATCHES; i++) {
        s32 nFlags = lbl_801D3CB0.p34[i].a20[0];
        s32 nFlags3 = lbl_801D3CB0.p34[i].a20[3];

        if (nFlags & 1) {
            if ((nFlags & 2) && !(nFlags3 & 0x40)) {
                lbl_801D3CB0.p34[i].n1C = 0;
                if (bReset) {
                    lbl_801D3CB0.p34[i].f14 = 0.0f;
                    lbl_801D3CB0.p34[i].n18 = 0;
                } else {
                    lbl_801D3CB0.p34[i].f14 = lbl_801D3CB0.p34[i].f10;
                }
            } else if ((nFlags & 2) && (nFlags3 & 0x40)) {
                lbl_801D3CB0.p34[i].n1C = 0;
                if (bReset) {
                    lbl_801D3CB0.p34[i].f14 = 0.0f;
                    lbl_801D3CB0.p34[i].n18 = 0;
                } else {
                    lbl_801D3CB0.p34[i].f14 = lbl_801D3CB0.p34[i].f10;
                }
            }
        }
    }
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

// The 'tLOD' chunk arrived: two values, made whole numbers and, when the second is the larger and
// both are at least lbl_802810DC x lbl_802810D8, turned into lbl_802810D0 and lbl_802810D4.
void fn_800341A4(UStreamObject* pObject) {
    TerLODData* pData = (TerLODData*)pObject->pData;
    s32 nRem;

    lbl_802810E4 = pData->fC;
    lbl_802810E8 = pData->f10;
    fn_80009E70(pObject);
    // A value that is not a multiple of lbl_802810D8 gets its remainder added (a multiple only
    // when the remainder was half of it); EA's rounding as it is.
    nRem = lbl_802810E4 % (s32)lbl_802810D8;
    if (nRem != 0) {
        lbl_802810E4 += nRem;
    }
    nRem = lbl_802810E8 % (s32)lbl_802810D8;
    if (nRem != 0) {
        lbl_802810E8 += nRem;
    }
    if (lbl_802810E8 > lbl_802810E4) {
        f32 fLimit = lbl_802810DC * lbl_802810D8;

        if (lbl_802810E4 < fLimit || lbl_802810E8 < fLimit) {
            return;
        }
        fn_80031A08(&lbl_802810D0, &lbl_802810D4, lbl_802810E4, lbl_802810E8);
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

// Unloads the terrain.
void fn_8003467C(void) {
    fn_800335F8(1);
    if (lbl_801D3CB0.p0 != NULL) {
        fn_800075CC(lbl_801D3CB0.p0);
        lbl_801D3CB0.p0 = NULL;
        fn_80009E70(lbl_801D3CB0.p4);
    }
    if (lbl_801D3CB0.pCourse != NULL) {
        fn_80009E70(lbl_801D3CB0.pC);
        lbl_801D3CB0.pCourse = NULL;
    }
    lbl_801D3CB0.n1154 = -1;
    lbl_802810E4 = -1;
    lbl_802810E8 = -1;
    lbl_802810D0 = 26;
    lbl_802810D4 = 16;
}

// A tee's position arrived: into its row of the course's tees, if the course is loaded.
void fn_80034720(UStreamObject* pObject) {
    TerTeeData* pTee = (TerTeeData*)pObject->pData;

    if (lbl_801D3CB0.pCourse != NULL) {
        lbl_801D3CB0.pCourse->tee[pTee->nTeeSet].x = pTee->vPos[0];
        lbl_801D3CB0.pCourse->tee[pTee->nTeeSet].y = pTee->vPos[1];
        lbl_801D3CB0.pCourse->tee[pTee->nTeeSet].z = pTee->vPos[2];
        lbl_801D3CB0.pCourse->tee[pTee->nTeeSet].w = 1.0f;
    }
    fn_80009E70(pObject);
}

// The flag follows the wind: it turns to face it and plays "flagcalm" below 5, "flagbrzy" below
// 13, else "flagwind".
void fn_800348DC(void) {
    f32 vWind[3];
    f32 fSpeed = Wind_Get(vWind);
    Character* pFlag = fn_8001D324(100);

    if (pFlag != NULL) {
        const char* aClips[3] = {"flagcalm", "flagbrzy", "flagwind"};
        int n;

        fn_80019358(pFlag, vWind, 0.0f);
        if (fSpeed < 5.0f) {
            n = 0;
        } else if (fSpeed < 13.0f) {
            n = 1;
        } else {
            n = 2;
        }
        if (n >= 3) {
            n = 2;
        }
        if (n < 0) {
            n = 0;
        }
        fn_8001BE88(pFlag, Char_SetClip(pFlag, 3, 0, aClips[n]), 1, 0.0f);
    }
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
