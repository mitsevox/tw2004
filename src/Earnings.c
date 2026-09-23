// Earnings.c (TW06's earnings.c): the money and goals kept in the save profiles. It loads the
// prize table (stream 'ERN '), rounds payouts to $25, pays the tournament and match prizes, scales
// a payout by the bonus, course, tee, pin and TOUR card multipliers, checks the goals that unlock
// things, and keeps the saved replays. The GM_Earnings_ functions of TW06 come in the same order.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// The prize table (stream 'ERN ', 0x22F0 bytes); multipliers are percentages (100 = x1).
typedef struct EarningsTable {
    u8   unk0[0x99C];
    s32  aTourPct[6];           // 0x99C  the TOUR card multiplier per level 1..6 (level 0 pays as 1)
    u8   unk9B4[0x22F0 - 0x9B4];
} EarningsTable;
extern EarningsTable lbl_80200538;

// How a payout was made up (TW06: CourseMoneyTracking_t, at least 0x48 bytes); only the fields
// this file writes. game.h still types it as an s32 array, which is what GameMode5.c passes.
typedef struct CourseMoneyTracking {
    s32  n0;                    // 0x00  the payout
    u8   unk4[0x24 - 0x4];
    s32  n24;                   // 0x24  the payout
    u8   unk28[0x34 - 0x28];
    s32  n34;                   // 0x34  what the TOUR card level added
    u8   unk38[0x48 - 0x38];
} CourseMoneyTracking;

// A save profile (0x10600 bytes each); only what this file reads.
typedef struct Profile {
    u8   b0;                    // 0x00000  payouts are scaled (and awards given, at 1) only when set
    u8   unk1[0x5000 - 0x1];
    s32  n5000;                 // 0x05000  the TOUR card level, 0..6
    u8   unk5004[0x10600 - 0x5004];
} Profile;
extern Profile* gpSaveData;

// The working tables and their saved copies, ten entries each.
extern s32 lbl_80200010[10];
extern s32 lbl_80200038[10];
extern s32 lbl_80200060[10];
extern s32 lbl_80200088[10];
extern s32 lbl_802000B0[10];
extern s32 lbl_802000D8[10];
extern s32 lbl_80200100[10];
extern s32 lbl_80200128[10];
extern s32 lbl_80200150[10];
extern s32 lbl_80200178[10];
extern s32 lbl_802001A0[10];
extern s32 lbl_802001C8[10];
extern s32 lbl_802001F0[10];
extern s32 lbl_80200218[10];
extern s32 lbl_80200240[10];
extern s32 lbl_80200268[10];
extern s32 lbl_80200290[10];
extern s32 lbl_802002B8[10];
extern s32 lbl_80200358[10];
extern s32 lbl_80200380[10];
extern s32 lbl_802003A8[10];
extern s32 lbl_802003D0[10];
extern s32 lbl_802003F8[10];
extern s32 lbl_80200420[10];
extern u8  lbl_801FFAE8[0x280];
extern u8  lbl_801FFD90[0x280];
extern s32 lbl_80282248;
extern s32 lbl_8028224C;
extern s32 lbl_80282250;
extern s32 lbl_80282254;
extern s32 lbl_80191A08[39];

void* memcpy(void* pDst, const void* pSrc, u32 uLen);
int   fn_800E177C(void);

void  fn_800D344C(UStreamObject* pObject);
s32   fn_800D477C(int nPlayer, u8* pBall, u8 b);
void  fn_800D4F14(int nPlayer, u8 b);
s32   fn_800D9954(void);

// Put the working tables back to their saved copies.
void fn_800D3244(void) {
    lbl_80282248 = lbl_80282250;
    lbl_8028224C = lbl_80282254;
    memcpy(lbl_802001C8, lbl_80200420, sizeof(lbl_802001C8));
    memcpy(lbl_802001A0, lbl_802003F8, sizeof(lbl_802001A0));
    memcpy(lbl_80200178, lbl_802003D0, sizeof(lbl_80200178));
    memcpy(lbl_80200150, lbl_802003A8, sizeof(lbl_80200150));
    memcpy(lbl_80200128, lbl_80200380, sizeof(lbl_80200128));
    memcpy(lbl_80200100, lbl_80200358, sizeof(lbl_80200100));
    memcpy(lbl_802000D8, lbl_802002B8, sizeof(lbl_802000D8));
    memcpy(lbl_802000B0, lbl_80200290, sizeof(lbl_802000B0));
    memcpy(lbl_80200088, lbl_80200268, sizeof(lbl_80200088));
    memcpy(lbl_80200060, lbl_80200240, sizeof(lbl_80200060));
    memcpy(lbl_80200038, lbl_80200218, sizeof(lbl_80200038));
    memcpy(lbl_80200010, lbl_802001F0, sizeof(lbl_80200010));
    memcpy(lbl_801FFD90, lbl_801FFAE8, sizeof(lbl_801FFD90));
}

// TW06: roundToNearest25. Every payout is a multiple of $25.
s32 fn_800D33A8(s32 nMoney) {
    return (s32)((12.5f + (f32)nMoney) / 25.0f) * 25;
}

// TW06: GM_Earnings_FreeStreamMemory.
void fn_800D33F0(void) {
}

// TW06: GM_Earnings_RegisterStreamClients. The prize table comes from the stream.
void fn_800D33F4(void) {
    UStream_RegisterHandler('ERN ', fn_800D344C);
}

// TW06: GM_Earnings_UnRegisterStreamClients.
void fn_800D3424(void) {
    UStream_UnregisterHandler('ERN ');
}

void fn_800D344C(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_80200538), &lbl_80200538);
}

// Whether an id is one of 23..38.
u8 fn_800D4010(int nId) {
    int b;

    b = 0;
    if (nId >= 23 && nId <= 38) {
        b = 1;
    }
    return b;
}

// Whether bit nBit of uMask is set.
u8 fn_800D4EF8(u32 uMask, int nBit) {
    return (uMask & (1 << nBit)) != 0;
}

// TW06: GM_Earnings_ComputeTOURCardModifiers. The TOUR card level raises the payout; the extra
// goes in the breakdown. Nothing is paid when fn_800E177C says so.
int fn_800D7220(int nReward, int nPlayer, s32* pOut) {
    CourseMoneyTracking* pMoney;
    f32 fMult;
    s32 nTotal;

    if (fn_800E177C() != 0) return 0;
    fMult = 1.0f;
    if (gpSaveData[gPlayers[nPlayer].nIndex].b0 != 0) {
        switch (gpSaveData[gPlayers[nPlayer].nIndex].n5000) {
        case 0:
        case 1:
            fMult = (f32)lbl_80200538.aTourPct[0] / 100.0f;
            break;
        case 2:
            fMult = (f32)lbl_80200538.aTourPct[1] / 100.0f;
            break;
        case 3:
            fMult = (f32)lbl_80200538.aTourPct[2] / 100.0f;
            break;
        case 4:
            fMult = (f32)lbl_80200538.aTourPct[3] / 100.0f;
            break;
        case 5:
            fMult = (f32)lbl_80200538.aTourPct[4] / 100.0f;
            break;
        case 6:
            fMult = (f32)lbl_80200538.aTourPct[5] / 100.0f;
            break;
        }
    }
    nTotal = (s32)((f32)nReward * fMult);
    nTotal = (s32)((12.5f + (f32)nTotal) / 25.0f) * 25;
    pMoney = (CourseMoneyTracking*)pOut;
    if (pMoney != NULL) {
        pMoney->n24 = nTotal;
        pMoney->n0 = nTotal;
        pMoney->n34 = nTotal - nReward;
    }
    return nTotal;
}

s32 fn_800D7660(int nPlayer, u8* pBall, u8 b) {
    fn_800D477C(nPlayer, pBall, b);
    return fn_800D9954();
}

s32 fn_800D7684(int nPlayer, int a, u8 b) {
    fn_800D4F14(nPlayer, b);
    return fn_800D9954();
}

s32 fn_800D86DC(s32 n) {
    switch (n) {
    case 16:
        return 0;
    case 17:
        return 1;
    case 13:
        return 2;
    default:
        return 3;
    }
}

s32 fn_800D8720(s32 n) {
    switch (n) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

s32 fn_800D9954(void) {
    return lbl_80282250;
}

s32 fn_800D995C(s32 i) {
    return lbl_802002B8[i];
}

s32 fn_800D9970(s32 i) {
    return lbl_80200290[i];
}

s32 fn_800D9984(s32 i) {
    return lbl_80200268[i];
}

s32 fn_800D9E00(s32 i) {
    return lbl_80191A08[i];
}
