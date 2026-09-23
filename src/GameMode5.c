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
    u8  unk18[0x28 - 0x18];
    s32 n28;                    // 0x28
    s32 n2C[3];                 // 0x2C
    u8  unk38[0x64 - 0x38];
    s32 n64;                    // 0x64
    u8  unk68[0x70 - 0x68];
    s32 n70;                    // 0x70
    u8  unk74[0x7C - 0x74];
    s32 n7C;                    // 0x7C
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
void fn_800EC544(Challenge* p0, s32 p1);
extern u8 lbl_802822FC;
u8 fn_800EC550(void);
s32 fn_800ECA08(void);
void fn_800ECBE4(void);
extern u8 lbl_80282314;
u8 fn_800ED540(void);
void fn_800ED548(void);
extern u8 lbl_802822FE;
extern s32 (*lbl_8028231C)();
s32 fn_800ED5C8(void);
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

s32 fn_800ED5C8(void) {
    if ((u8) lbl_802822FE != 0) {
        return 1;
    }
    return lbl_8028231C();
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
int fn_800ECF9C(void) {
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
