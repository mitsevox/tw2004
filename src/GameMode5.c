// GameMode5.c (our name): game mode 5, a run of 83 challenges (lbl_80203554, loaded from the
// 'PLY ' stream object), each an entry of 0x80 bytes; probably TW2004's Tiger Challenge.

#include "golfer.h"

// One challenge (0x80 bytes).
typedef struct Challenge {
    s32 n0;
    s32 n4;
    s32 nGroup;                 // 0x08  challenges with the same group are played together
    s32 nC;
    s32 nCourse;                // 0x10
    s32 nType;                  // 0x14  0 one hole, 1 all 18, 2/3 a nine, 4/5/6 the par 5s/4s/3s
    s32 nHole;                  // 0x18  1-based
    u8  unk1C[0x24 - 0x1C];
    s32 uClubs;                 // 0x24  the bag for the challenge (bits, see fn_800EBEF0), 0 = the golfer's own
    s32 n28;                    // 0x28
    s32 n2C[3];                 // 0x2C
    u8  unk38[0x3C - 0x38];
    s32 nTargetKind;            // 0x3C  how the target is built (see fn_800ED028)
    s32 nTargetBase;            // 0x40
    s32 nHoleKind;              // 0x44  what the challenge hole adds
    s32 nHoleExtra;             // 0x48
    u8  bPlaceBall;             // 0x4C  the ball starts at the spot in lbl_80203170
    u8  b4D;                    // 0x4D  f54 goes to fn_800ED6F8
    u8  unk4E[0x54 - 0x4E];
    f32 f54;                    // 0x54
    s32 nScoring;               // 0x58  0 the round's totals, 1 this hole
    // Three medals (0 the best): the rule (0 none, see fn_800EC558), its mark, the reward.
    s32 bMedal2;                // 0x5C  medal 0's rule (named by fn_800ECA34's index: 2)
    s32 n60;                    //       its mark
    s32 n64;                    //       its reward
    s32 bMedal1;                // 0x68  medal 1
    s32 n6C;
    s32 n70;
    s32 bMedal0;                // 0x74  medal 2
    s32 n78;
    s32 n7C;
} Challenge;

void fn_800EADD8(void);
extern s32 lbl_802822F4;
extern s32 lbl_802822F8;
s32 fn_800EAC94();
void fn_800EAE38(s32 p0);
void fn_800EAE44(void);
s32 fn_800EAE6C(void);
extern Challenge lbl_80203554[83];
void UStream_UnregisterHandler();
void fn_8000E790();
void fn_800EAEB8(void);
void fn_800EAEEC(s32 p0);
extern Challenge* lbl_80281664;
extern s32 lbl_80281668;

// Per challenge: three values from the 'PLYs'... stream (lbl_80203170, 12 bytes each).
typedef struct ChallengeVals {
    f32 f0;
    f32 f4;
    f32 f8;
} ChallengeVals;
extern ChallengeVals lbl_80203170[83];
extern s32 lbl_801925F0[][3];               // per message kind: three message ids to pick from
extern char* lbl_80282310;                  // the challenge text block
extern s32 lbl_80281660;
extern s32 lbl_802822F0;
extern void (*lbl_8028232C)(void);
extern void (*lbl_80282324)(void);
extern void (*lbl_80282318)(int nPlayer);
int   UStream_RegisterHandler();
u32   fn_8000E81C(void* pObj, void** ppData);
void* fn_800951A0(u32 nSize, int nAlign, int a);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);   // memcpy
void  fn_80009E70(void* p);                 // free
u32   Rand_Next(int nStream);
void  fn_800E4364(u32 nQueue, int a, int b, int c);
extern u8* gpSaveData;
int   fn_800ED028(int i);
void  GM_EndOfGolferTurn(int nPlayer);
void  fn_800E5714(int a);
s32   fn_800E1074();
u8    fn_800F0818(void);
void  fn_800F06DC(void);
void  fn_800EAF7C(void);
void  fn_80019648(void);
void  fn_800E4D88(void);
void  fn_800EBEF0(void);
void  fn_800EAD6C(void);
void  fn_800EBD28(void);
void  fn_800EC1E0(void);
void  fn_800ED604(int nPlayer);
void  fn_800EAF18(void* pObj);
extern u8  (*lbl_80282320)(void);
extern s32 lbl_80282300;
extern s32 lbl_80282304;
extern s32 lbl_80282308;
extern s32 lbl_8028230C;
int   fn_800D2AD8(int nHole);               // a hole's par
int   fn_800D2ABC(int nCourse, int nHole);  // a hole's par on a course
void  fn_800E1260(int nPreset);
void  fn_800E0B38(int nMode);
void  fn_800F07C8(void);
u8    fn_800E4BF8(void);
int   Game_CurHoleIndex(void);
int   fn_800D2B08(void);                    // the hole's par
u8    fn_800E39F0(void);
extern u8 gNumPlayersSetUp;                 // 0x80281D48 (Golfer.c)
extern void (*lbl_80282328)(void);
int   fn_800EC558(void);
u8    fn_801025F4(void);
s16   fn_800D2994();
int   fn_800D7220(int nReward, int a, s32* pOut);
void  fn_800D3548(int nPlayer, int nMoney, s32* p);
u8    fn_800D9998(int nPlayer, int nAward);
u8    fn_800D750C(int nPlayer, int nAward);
extern u8 lbl_80200538[];                   // prize data: bonuses at +0x9E4 and +0xA24
#define PRIZE_AT(off) (*(s32*)(lbl_80200538 + (off)))
// A profile's best medal per challenge (3 = none) and a date stamp for it, in the save.
typedef struct ChallengeSave {
    u8  unk0[0x516C];
    s32 aMedal[29];             // 0x516C
    u8  unk51E0[4];
    s16 aStamp[29];             // 0x51E4
} ChallengeSave;
#define PROFILE_MEDAL(n, i) ((ChallengeSave*)(gpSaveData + (n) * 0x10600))->aMedal[i]
#define PROFILE_STAMP(n, i) ((ChallengeSave*)(gpSaveData + (n) * 0x10600))->aStamp[i]
int   fn_800ECF9C(int i);
u8    fn_800E5110(void);
void  fn_80055AA8(u8* pBall, f32* pPos, int nPlayer);
u8    Physics_DropBall(u8* pBall, f32* pPos);
void  Vec_Copy(f32* pSrc, f32* pDst);
void fn_800EC544(Challenge* p0, s32 p1);
extern u8 lbl_802822FC;
u8 fn_800EC550(void);
s32 fn_800ECA08(void);
void fn_800ECBE4(void);
extern u8 lbl_80282314;
u8 fn_800ED540(void);
void fn_800ED548(void);
extern u8 lbl_802822FE;
extern u8 (*lbl_8028231C)(int nPlayer, int bCheck);
u8 fn_800ED5C8(int nPlayer, int bCheck);
extern u8 lbl_802822FD;
void fn_800ED6E8(u8 v);
u8 fn_800ED6F0(void);
extern s32 lbl_802811F0;
void fn_800ED6F8(f32 x0);
void fn_80062D6C();
void fn_800ED710(s32 p0);

// Game mode 5 starts: its callbacks, one player, the challenge list.
void fn_800EACD8(void) {
    gpGame->pfn1C8 = fn_800EACD8;
    gpGame->pfn1CC = fn_800EAD6C;
    gpGame->pfn1E4 = fn_800EBD28;
    gpGame->pfn1F4 = fn_800EC1E0;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800ED5C8;
    gpGame->pfn210 = fn_800ED604;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    lbl_802822FE = 0;
    lbl_80281664 = lbl_80203554;
    lbl_80281668 = 83;
}

// The mode ends: its cleanup callback runs once, and the options it changed come back.
void fn_800EAD6C(void) {
    if (lbl_8028232C) {
        lbl_8028232C();
        lbl_8028232C = NULL;
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    SESSION_OPTIONS->unkC = lbl_80281660;
    SESSION_OPTIONS->nWind = lbl_802822F0;
    lbl_802822FC = 0;
}

void fn_800EADD8(void) {
}

// The 'PLYs' records: three values per challenge.
void fn_800EADDC(void* pObj) {
    u8* d = *(u8**)pObj;
    int i = d[0x1C] - 1;
    if (i < 83) {
        lbl_80203170[i].f0 = *(f32*)(d + 0x10);
        lbl_80203170[i].f4 = *(f32*)(d + 0x14);
        lbl_80203170[i].f8 = *(f32*)(d + 0x18);
    }
    fn_80009E70(pObj);
}

void fn_800EAE38(s32 p0) {
    lbl_802822F4 = p0;
    lbl_802822F8 = p0;
}

void fn_800EAE44(void) {
    s32 t0;
    t0 = fn_800EAC94();
    lbl_802822F4 = t0;
    lbl_802822F8 = t0;
}

s32 fn_800EAE6C(void) {
    return 29;
}

void fn_800EAE74(void) {
    UStream_RegisterHandler('PLY ', fn_800EAEEC, 'PL\0\0');
    UStream_RegisterHandler('PLYs', fn_800EAF18, 'PL\0\0');
}

void fn_800EAEB8(void) {
    UStream_UnregisterHandler(1347180832);
    UStream_UnregisterHandler(1347180915);
}

void fn_800EAEEC(s32 p0) {
    fn_8000E790(p0, 10624, lbl_80203554);
}

// The 'PLYs' object: the challenge text block is copied out.
void fn_800EAF18(void* pObj) {
    void* pData;
    u32 nSize = fn_8000E81C(pObj, &pData);
    if (nSize) {
        lbl_80282310 = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(lbl_80282310, pData, nSize);
        fn_80009E70(pObj);
    }
}

void fn_800EC544(Challenge* p0, s32 p1) {
    lbl_80281664 = p0;
    lbl_80281668 = p1;
}

u8 fn_800EC550(void) {
    return lbl_802822FC;
}

s32 fn_800ECA08(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((8 - t0)) >> 5) & 0xFF);
}

void fn_800ECBE4(void) {
    if (Game_GetMode() == 8) {
        fn_800ED710(0);
    }
}

u8 fn_800ED540(void) {
    return lbl_80282314;
}

void fn_800ED548(void) {
    lbl_80282314 = 0;
}

// Hole finished: always after a restart; otherwise the challenge's own test.
u8 fn_800ED5C8(int nPlayer, int bCheck) {
    if ((u8) lbl_802822FE != 0) {
        return 1;
    }
    return lbl_8028231C(nPlayer, bCheck);
}

void fn_800ED6E8(u8 v) {
    lbl_802822FD = v;
}

u8 fn_800ED6F0(void) {
    return lbl_802822FD;
}

void fn_800ED6F8(f32 x0) {
    *(u8*)(((u8*)lbl_802811F0) + 0x1C) = 1;
    *(f32*)(((u8*)lbl_802811F0) + 0x18) = x0;
}

void fn_800ED710(s32 p0) {
    fn_80062D6C(18, p0);
}

// Hole start (after fn_800EAF7C set the challenge up).
void fn_800EBD28(void) {
    gpGame->b275 = 1;
    lbl_80282324();
    fn_800EBEF0();
}

// A message of kind n: one of three at random.
void fn_800EC170(int n) {
    fn_800E4364(7, lbl_801925F0[n][Rand_Next(0) % 3], 0, 0);
}

// Whether profile n has done every step: its counter at +0x5000 is set and none of the 29 steps
// at +0x516C is at 3.
u8 fn_800EC4F0(int n) {
    u8* p = gpSaveData + n * 0x10600;
    int i;
    if (*(s32*)(p + 0x5000) < 1) {
        return 0;
    }
    for (i = 0; i < 29; i++) {
        if (*(s32*)(p + 0x516C + i * 4) == 3) {
            return 0;
        }
    }
    return 1;
}

// The total of fn_800ED028 over the current challenge's group.
int fn_800ECF9C(int iUnused) {
    int n = 0;
    int i;
    for (i = 0; i < lbl_80281668; i++) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
            n += fn_800ED028(i);
        }
    }
    return n;
}

// Challenge text lines (offsets into the text block).
char* fn_800ED280(int nId) {
    int i = fn_800EAC94(nId);
    if (i == -1) {
        return 0;
    }
    return lbl_80282310 + lbl_80203554[i].n0;
}

char* fn_800ED2C8(int nId) {
    int i = fn_800EAC94(nId);
    if (i == -1) {
        return 0;
    }
    return lbl_80282310 + lbl_80203554[i].n4;
}

// How many challenges are in a group.
int fn_800ED508(int nGroup) {
    int n = 0;
    int i;
    for (i = 0; i < lbl_80281668; i++) {
        if (nGroup == lbl_80281664[i].nGroup) {
            n++;
        }
    }
    return n;
}

// Restart the challenge from the pause menu.
void fn_800ED554(void) {
    lbl_802822FE = 1;
    gpGame->b134 = 1;
    lbl_802822F4 = lbl_802822F8;
    GM_EndOfGolferTurn(0);
    gpGame->pfn224();
    fn_800E5714(2);
    fn_800E1074();
    if (fn_800F0818()) {
        fn_800F06DC();
    } else {
        fn_800EAF7C();
    }
    fn_80019648();
}

// The hole is over, the game is not: after a restart the pending screen closes; otherwise the
// challenge's own callback runs.
void fn_800ED604(int nPlayer) {
    if (lbl_802822FE) {
        fn_800E4D88();
        lbl_802822FE = 0;
        gpGame->b275 = 0;
        return;
    }
    lbl_80282318(nPlayer);
}

void fn_800ED650(int i, s32* pA, s32* pB, s32* pC) {
    *pA = lbl_80281664[i].n7C;
    *pB = lbl_80281664[i].n70;
    *pC = lbl_80281664[i].n64;
}

s32 fn_800ED688(int i) {
    return lbl_80281664[i].n28;
}

s32 fn_800ED69C(int i, int k) {
    if (k == 0) {
        return lbl_80281664[i].n2C[0];
    }
    if (k == 1) {
        return lbl_80281664[i].n2C[1];
    }
    return lbl_80281664[i].n2C[2];
}

// The round ends: the round's totals are added up; then the next challenge in the group starts
// (unless only checking), or the group is over (1).
u8 fn_800EBD60(u8 bCheck) {
    int h;
    int nStrokes;
    int i;
    if (lbl_802822FE) {
        return 0;
    }
    if (lbl_80282320()) {
        nStrokes = 0;
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nStrokes += gPlayers[0].nStrokes[h];
                lbl_80282308 += fn_800D2AD8(h);
                lbl_80282304++;
                lbl_80282300 += gPlayers[0].n290[h];
            }
        }
        lbl_8028230C += nStrokes;
        fn_800E1260(0);
        for (i = lbl_802822F4 + 1; i < lbl_80281668; i++) {
            if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
                if (bCheck) {
                    return 0;
                }
                lbl_802822F4 = i;
                if (fn_800F0818()) {
                    fn_800E0B38(24);
                    fn_800F07C8();
                } else {
                    fn_800E0B38(5);
                    fn_800EAF7C();
                }
                gpGame->b134 = 1;
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// A challenge's target score (in mode 0 only): a fixed number (1); par for the holes before the
// challenge hole plus a number (2); birdie (3), par (4) or bogey (5) on each of them; or the
// opponent's strokes so far (7); then the challenge hole adds a number (1), par plus a number (2),
// birdie (3), par (4) or bogey (5).
int fn_800ED028(int i) {
    int nTarget = 0;
    int nHole = lbl_80281664[i].nHole - 1;
    int k;               // a loop counter, and in case 7 the current hole once it is over
    int h;
    if (Game_GetMode() == 0) {
        switch (lbl_80281664[i].nTargetKind) {
        case 1:
            nTarget = lbl_80281664[i].nTargetBase;
            break;
        case 2:
            for (k = 0; k < nHole; k++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, k);
            }
            nTarget += lbl_80281664[i].nTargetBase;
            break;
        case 3:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h) - 1;
            }
            break;
        case 4:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h);
            }
            break;
        case 5:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h) + 1;
            }
            break;
        case 7:
            k = 0;
            if (fn_800E4BF8()) {
                k = 1;
            }
            for (h = 0; h < k + Game_CurHoleIndex(); h++) {
                if (gpGame->bHoleSelected[h]) {
                    nTarget += gPlayers[1].nStrokes[h];
                }
            }
            break;
        }
        switch (lbl_80281664[i].nHoleKind) {
        case 1:
            nTarget += lbl_80281664[i].nHoleExtra;
            break;
        case 2:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) + lbl_80281664[i].nHoleExtra;
            break;
        case 3:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) - 1;
            break;
        case 4:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole);
            break;
        case 5:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) + 1;
            break;
        }
    }
    return nTarget;
}

// The score for medal k (0..2) over the current group: the last challenge's mark minus the target
// so far (-1 when the challenge gives no such medal).
s32 fn_800ECA34(int k) {
    int i;
    int nLast = lbl_802822F4;
    int nTarget;
    for (i = 0; i < lbl_80281668; i++) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup && i > nLast) {
            nLast = i;
        }
    }
    nTarget = fn_800ECF9C(lbl_802822F4);
    if ((lbl_80281664[lbl_802822F4].nTargetKind == 7 && Game_GetMode() == 0) ||
        lbl_80281664[lbl_802822F4].nTargetKind == 1) {
        nTarget = 0;
    }
    switch (k) {
    case 0:
        if (lbl_80281664[lbl_802822F4].bMedal0 == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].n78 - nTarget;
        }
    case 1:
        if (lbl_80281664[lbl_802822F4].bMedal1 == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].n6C - nTarget;
        }
    case 2:
        if (lbl_80281664[lbl_802822F4].bMedal2 == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].n60 - nTarget;
        }
    default:
        return 0;
    }
}

// The holes left in the group: the rest of this round, plus each later challenge's holes (one, 18,
// a nine, or the par 5s/4s/3s of its course).
int fn_800ED314(void) {
    int h;
    int i;
    int n = 0;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            n++;
        }
    }
    i = lbl_802822F4 + (fn_800ED5C8(0, 1) == 0);
    if (fn_800ED508(lbl_80281664[lbl_802822F4].nGroup) > 1) {
        for (; i < lbl_80281668; i++) {
            if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
                switch (lbl_80281664[i].nType) {
                case 0:
                    n += 1;
                    break;
                case 1:
                    n += 18;
                    break;
                case 2:
                    n += 9;
                    break;
                case 3:
                    n += 9;
                    break;
                case 4:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 5) {
                            n++;
                        }
                    }
                    break;
                case 5:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 4) {
                            n++;
                        }
                    }
                    break;
                case 6:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 3) {
                            n++;
                        }
                    }
                    break;
                case 7:
                    break;
                }
            }
        }
    }
    if (fn_800E5110()) {
        return 0;
    }
    fn_800E4BF8();
    return n;
}

// The challenge's setup: the ball placed at its spot, its bag (bit n of uClubs gives the n-th club
// of the list below; club 25 is always in), and fn_800ED6F8's value.
void fn_800EBEF0(void) {
    f32 v[4];
    if (lbl_80281664[lbl_802822F4].bPlaceBall) {
        v[0] = lbl_80203170[lbl_802822F4].f0;
        v[1] = lbl_80203170[lbl_802822F4].f4;
        v[2] = lbl_80203170[lbl_802822F4].f8;
        v[3] = 1.0f;
        fn_80055AA8(gPlayers[0].ball, v, 0);
        Physics_DropBall(gPlayers[0].ball, v);
        Vec_Copy(v, &gPlayers[0].fBallX);
    }
    if (lbl_80281664[lbl_802822F4].uClubs) {
        gPlayers[0].golfer.uBagMask = 0x2000000;
        if (lbl_80281664[lbl_802822F4].uClubs & 1) {
            gPlayers[0].golfer.uBagMask |= 0x1;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 2) {
            gPlayers[0].golfer.uBagMask |= 0x40;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 4) {
            gPlayers[0].golfer.uBagMask |= 0x80;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 8) {
            gPlayers[0].golfer.uBagMask |= 0x100;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x10) {
            gPlayers[0].golfer.uBagMask |= 0x200;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x20) {
            gPlayers[0].golfer.uBagMask |= 0x400;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x40) {
            gPlayers[0].golfer.uBagMask |= 0x800;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x80) {
            gPlayers[0].golfer.uBagMask |= 0x1000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x100) {
            gPlayers[0].golfer.uBagMask |= 0x2000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x200) {
            gPlayers[0].golfer.uBagMask |= 0x4000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x400) {
            gPlayers[0].golfer.uBagMask |= 0x8000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x800) {
            gPlayers[0].golfer.uBagMask |= 0x10000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x1000) {
            gPlayers[0].golfer.uBagMask |= 0x20000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x2000) {
            gPlayers[0].golfer.uBagMask |= 0x40000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x4000) {
            gPlayers[0].golfer.uBagMask |= 0x80000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x8000) {
            gPlayers[0].golfer.uBagMask |= 0x200000;
        }
        if (lbl_80281664[lbl_802822F4].uClubs & 0x10000) {
            gPlayers[0].golfer.uBagMask |= 0x800000;
        }
    }
    if (lbl_80281664[lbl_802822F4].b4D) {
        fn_800ED6F8(lbl_80281664[lbl_802822F4].f54);
    }
}

// The challenge is over: a better medal than the profile's best is saved (with a stamp); the medal's
// message; for a profile, the medal's reward, then two bonuses (0x1C, and 0xC once every challenge
// has a medal).
void fn_800EC1E0(void) {
    s32 aOut[18];
    int nMedal;
    int nReward;
    int nProfile;
    int nMoney;
    lbl_80282328();
    if (!fn_800F0818()) {
        nMedal = fn_800EC558();
        if (!fn_801025F4() && nMedal != 3) {
            if (nMedal < PROFILE_MEDAL(gPlayers[0].nIndex, fn_800EAC7C())) {
                PROFILE_MEDAL(gPlayers[0].nIndex, fn_800EAC7C()) = nMedal;
                PROFILE_STAMP(gPlayers[0].nIndex, fn_800EAC7C()) = fn_800D2994();
            }
            switch (nMedal) {
            case 0:
                nReward = lbl_80281664[lbl_802822F4].n64;
                break;
            case 1:
                nReward = lbl_80281664[lbl_802822F4].n70;
                break;
            case 2:
                nReward = lbl_80281664[lbl_802822F4].n7C;
                break;
            }
            fn_800EC170(nMedal);
            nProfile = gPlayers[0].nIndex;
            if (gpSaveData[nProfile * 0x10600]) {
                nMoney = fn_800D7220(nReward, 0, aOut);
                if (nMoney) {
                    switch (nMedal) {
                    case 0:
                        fn_800E4364(0, 0x6F, nMoney, nProfile);
                        break;
                    case 1:
                        fn_800E4364(0, 0x70, nMoney, nProfile);
                        break;
                    case 2:
                        fn_800E4364(0, 0x71, nMoney, nProfile);
                        break;
                    }
                }
                fn_800D3548(0, nMoney, aOut);
                if (fn_800ED6F0() && fn_800D9998(0, 0x1C) && fn_800D750C(0, 0x1C)) {
                    fn_800E4364(6, 0x1C, PRIZE_AT(0xA24), nProfile);
                    fn_800D3548(0, PRIZE_AT(0xA24), 0);
                    gPlayers[0].n31C += PRIZE_AT(0xA24);
                }
                if (fn_800EC4F0(nProfile) && fn_800D750C(0, 0xC)) {
                    fn_800E4364(2, 0xC, PRIZE_AT(0x9E4), nProfile);
                    fn_800D3548(0, PRIZE_AT(0x9E4), 0);
                    gPlayers[0].n31C += PRIZE_AT(0x9E4);
                }
            }
        }
    }
}

// The medal earned (0 best, 3 none): for each medal its rule against the round's totals (strokes
// plus a number, against par, birdie or par or bogey golf, penalties below a mark, a match margin,
// the best of the n274 counts, n DD8) or against this hole.
int fn_800EC558(void) {
    int m;
    int nRule;
    int nMark;
    int nStrokes;
    int nPen;
    int nSum;
    int nPar;
    int nHoles;
    u8 bBest;
    int i;
    int nPlayoff;
    for (m = 0; m < 3; m++) {
        switch (m) {
        case 0:
            nRule = lbl_80281664[lbl_802822F4].bMedal2;
            nMark = lbl_80281664[lbl_802822F4].n60;
            break;
        case 1:
            nRule = lbl_80281664[lbl_802822F4].bMedal1;
            nMark = lbl_80281664[lbl_802822F4].n6C;
            break;
        case 2:
            nRule = lbl_80281664[lbl_802822F4].bMedal0;
            nMark = lbl_80281664[lbl_802822F4].n78;
            break;
        }
        switch (lbl_80281664[lbl_802822F4].nScoring) {
        case 0:
            nStrokes = lbl_8028230C;
            nPar = lbl_80282308;
            nHoles = lbl_80282304;
            nPen = lbl_80282300;
            switch (nRule) {
            case 1:
                if (nStrokes + lbl_80281664[lbl_802822F4].nTargetBase <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - nHoles) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + nHoles) {
                    return m;
                }
                break;
            case 6:
                if (nPen < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->n4 == 1) {
                    if (fn_800ED6F0()) {
                        if (gpGame->bD4) {
                            nPlayoff = 2;
                            if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                                nPlayoff = 1;
                            }
                            return nPlayoff;
                        }
                        if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                            return m;
                        }
                    } else if (gpGame->bD4) {
                        if (nMark == 0 && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                            return m;
                        }
                    } else if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                        return m;
                    }
                }
                if (gpGame->n4 == 0) {
                    if (gSession.nNumPlayers > 1) {
                        nSum = 0;
                        for (i = 0; i < 18; i++) {
                            if (gpGame->bHoleSelected[i]) {
                                nSum += gPlayers[1].nStrokes[i];
                            }
                        }
                    }
                    if (nSum - nStrokes <= nMark) {
                        return m;
                    }
                }
                break;
            case 8:
                bBest = 1;
                for (i = 1; i < gNumPlayersSetUp; i++) {
                    if (gPlayers[0].n274 <= PLAYER(i)->n274) {
                        bBest = 0;
                    }
                }
                if (bBest && gpGame->nD8 <= nMark) {
                    return m;
                }
                break;
            case 9:
                if (fn_800E39F0() && gPlayers[0].nDD8 >= nMark) {
                    return m;
                }
                break;
            }
            break;
        case 1:
            nStrokes = gPlayers[0].nStrokes[Game_CurHoleIndex()];
            nPar = fn_800D2B08();
            switch (nRule) {
            case 1:
                if (nStrokes <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - 1) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + 1) {
                    return m;
                }
                break;
            case 6:
                if (nPen < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->bD4 && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                    return 2;
                }
                if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                    return m;
                }
                break;
            case 9:
                if (fn_800E39F0() && gPlayers[0].nDD8 >= nMark) {
                    return m;
                }
                break;
            }
            break;
        }
    }
    return 3;
}
