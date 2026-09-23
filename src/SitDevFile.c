// SitDevFile.c (EA's name, from its asserts; TW06): the paired-single vector helpers, a watcher
// that follows the ball after a shot (an event 48 frames in, a call when it reaches surface 105),
// and the loading of the situation scripts into the block lbl_802811B8 points at (sitdev.h).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    lbl_80282200;     // 0x80282200  the watched ball has reached surface 105
Ball* lbl_802821FC;     // 0x802821FC  the watched ball, NULL for none
u32   lbl_802821F8;     // 0x802821F8  gSession.nFrameCount when it started

void fn_80067710(int nPlayer, int a, int b);   // also declared in Swing.c; belongs in a header

// ---- vector helpers ------------------------------------------------------------------------
// Hand-written paired-single assembly. The square roots are one Newton step on the hardware
// estimate: r = frsqrte(s); r = 0.5 * r * (3 - s * r * r).

#ifdef __MWERKS__
static const f32 kVecEpsilon = 1.0f / 1073741824.0f;   // 2^-30: shorter than this is not scaled
static const f32 kHalf = 0.5f;
static const f32 kThree = 3.0f;
static const f32 kZero = 0.0f;

// Normalise a four-float vector (a quaternion) from pSrc into pDst; a near-zero one is copied.
asm void Vec_Normalize(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    blr
}

// Normalise a three-float vector from pSrc into pDst; a near-zero one is copied.
asm void fn_800BAF04(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    blr
}

// Vec_Normalize that also returns the length (0 for a near-zero vector).
asm f32 fn_800BAF58(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    lfs      f1, kZero
    blr
}

// fn_800BAF04 that also returns the length (0 for a near-zero vector).
asm f32 fn_800BAFC0(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    lfs      f1, kZero
    blr
}

// The squared distance between two three-float points.
asm f32 fn_800BB028(register f32* pA, register f32* pB) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f1, f1, f1, f0
    ps_sum0  f1, f1, f0, f0
    blr
}

// The distance between two three-float points.
asm f32 Vec_Distance(register f32* pA, register f32* pB) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f3, f1, f1, f0
    ps_sum0  f3, f3, f0, f0
    lfs      f1, kZero
    fcmpo    cr0, f3, f1
    blelr
    frsqrte  f4, f3
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f4, f4
    fmuls    f1, f4, f4
    fmuls    f2, f2, f4
    fnmsubs  f0, f3, f1, f0
    fmuls    f0, f2, f0
    fmuls    f1, f3, f0
    blr
}
#else
// port: untested, the plain-C versions for compilers without paired singles. The originals use
// the hardware square-root estimate plus one refinement step, so results differ in the last bits.
static f32 Vec3_LengthSq(const f32* p) {
    return p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
}

void Vec_Normalize(f32* pSrc, f32* pDst) {
    f32 s = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + pSrc[3] * pSrc[3];
    f32 k = 1.0f;
    int i;
    if (s > 1.0f / 1073741824.0f) {
        k = 1.0f / (f32)sqrt(s);
    }
    for (i = 0; i < 4; i++) {
        pDst[i] = pSrc[i] * k;
    }
}

void fn_800BAF04(f32* pSrc, f32* pDst) {
    f32 s = Vec3_LengthSq(pSrc);
    f32 k = 1.0f;
    int i;
    if (s > 1.0f / 1073741824.0f) {
        k = 1.0f / (f32)sqrt(s);
    }
    for (i = 0; i < 3; i++) {
        pDst[i] = pSrc[i] * k;
    }
}

f32 fn_800BAF58(f32* pSrc, f32* pDst) {
    f32 s = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + pSrc[3] * pSrc[3];
    f32 fLen;
    int i;
    if (!(s > 1.0f / 1073741824.0f)) {
        for (i = 0; i < 4; i++) {
            pDst[i] = pSrc[i];
        }
        return 0.0f;
    }
    fLen = (f32)sqrt(s);
    for (i = 0; i < 4; i++) {
        pDst[i] = pSrc[i] / fLen;
    }
    return fLen;
}

f32 fn_800BAFC0(f32* pSrc, f32* pDst) {
    f32 s = Vec3_LengthSq(pSrc);
    f32 fLen;
    int i;
    if (!(s > 1.0f / 1073741824.0f)) {
        for (i = 0; i < 3; i++) {
            pDst[i] = pSrc[i];
        }
        return 0.0f;
    }
    fLen = (f32)sqrt(s);
    for (i = 0; i < 3; i++) {
        pDst[i] = pSrc[i] / fLen;
    }
    return fLen;
}

f32 fn_800BB028(f32* pA, f32* pB) {
    f32 d[3];
    d[0] = pA[0] - pB[0];
    d[1] = pA[1] - pB[1];
    d[2] = pA[2] - pB[2];
    return Vec3_LengthSq(d);
}

f32 Vec_Distance(f32* pA, f32* pB) {
    f32 s = fn_800BB028(pA, pB);
    if (!(s > 0.0f)) return 0.0f;
    return (f32)sqrt(s);
}
#endif

// ---- the watched ball ----------------------------------------------------------------------

void fn_800BB0DC(void);

void fn_800BB0A8(void) {
    fn_800BB0DC();
}

// Stop watching the ball.
void fn_800BB0C8(void) {
    lbl_802821FC = NULL;
}

// Whether the ball has reached surface 105 (see fn_800BB0E8).
u8 fn_800BB0D4(void) {
    return lbl_80282200;
}

void fn_800BB0DC(void) {
    lbl_80282200 = 0;
}

// Per frame while a ball is watched: 48 frames after it was hit, while it is flying or rolling,
// trigger event 0x4B for its player; the first time it lands on surface 105, call
// fn_80067710(player, 0, 0x22).
void fn_800BB0E8(void) {
    if (lbl_802821FC == NULL) return;
    switch (lbl_802821FC->nState) {
    case 2:
    case 3:
    case 4:
        if (48.0f == (f32)(gSession.nFrameCount - lbl_802821F8)) {
            EVENT_Trigger(lbl_802821FC->nPlayer, 0x4B, NULL, -1);
        }
        break;
    }
    if (lbl_802821FC->nSurface == 105 && !lbl_80282200) {
        fn_80067710(lbl_802821FC->nPlayer, 0, 0x22);
        lbl_80282200 = 1;
    }
}

// Start watching a ball: remember it and the frame it was hit on.
void fn_800BB1A8(Ball* pBall) {
    lbl_802821FC = pBall;
    lbl_802821F8 = gSession.nFrameCount;
}

void fn_800BB1C0(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_801FA198[i] = 0;
        lbl_801FA1AC[i] = 0;
    }
}

u8 fn_800BB1F8(int nPlayer) {
    return lbl_801FA1AC[nPlayer] == 1;
}

// ---- scripts -------------------------------------------------------------------------------

void fn_800BB4B0(void) {
}

// Set value 5 of the shared block.
void fn_800BBADC(int nValue) {
    fn_80067B1C(lbl_802811B8->aValue, 5, (u16)nValue, lbl_802811B8->aSetBits);
}

// ---- the values the scripts test -----------------------------------------------------------

void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer);
void fn_800BCB74(s32* pClass, int nSurface);
s32  fn_800BCCA0(int nPlayer);
s32  fn_800BCCCC(int nPlayer);
s32  fn_800BCCF8(int nPlayer);
u8   fn_800BCD24(int nPlayer);
u8   fn_800BCD50(void);
s32  fn_800BCD5C(void);
void fn_800BD77C(u16 uSound);
void fn_800BD7D0(u8 nMusic);
void fn_800BD7E8(u16 uSound);
void fn_800BD868(int nSound, int a);

// Correct the surface class of where a ball lies (SurfaceType.nClass) for the scripts: outside the
// course outline, or on ground a ball may not stay on, is 19 (not playable) unless it is water; a
// ball that must be dropped counts as water (7); surface 151 is 21; class 18 (green) reads as 12.
void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer) {
    u8 bWater;
    u8 bNoLie;
    bWater = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES &&
             (gSurfaceTypes[nSurface].nClass == 7 || gSurfaceTypes[nSurface].nClass == 16);
    bNoLie = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES && !(gSurfaceTypes[nSurface].u34 & 1) &&
             (gSurfaceTypes[nSurface].u34 & 2);
    if ((!Ter_PointInOOBNetwork(pBall->vPos) && !bWater) || (bNoLie && !bWater)) {
        *pClass = 19;
    }
    if (pPlayer->b30E) {
        *pClass = 7;
    }
    if (nSurface == 151) {
        *pClass = 21;
    }
    if (*pClass == 18) {
        *pClass = 12;
    }
}

// Surface 151 is class 21 (the same test as in fn_800BCA60).
void fn_800BCB74(s32* pClass, int nSurface) {
    if (nSurface == 151) {
        *pClass = 21;
    }
}

// The game mode's answers for the scripts (GameState's callbacks).
s32 fn_800BCCA0(int nPlayer) {
    return gpGame->pfn204(nPlayer);
}

s32 fn_800BCCCC(int nPlayer) {
    return gpGame->pfn208(nPlayer);
}

s32 fn_800BCCF8(int nPlayer) {
    return gpGame->pfn200(nPlayer);
}

u8 fn_800BCD24(int nPlayer) {
    return gpGame->pfn1FC(nPlayer);
}

u8 fn_800BCD50(void) {
    return gpGame->bD4;
}

s32 fn_800BCD5C(void) {
    return gpGame->nDC;
}

// ---- sounds and music ----------------------------------------------------------------------

// Hand GameEffects a sound to stop later (u48), unless one is waiting already; not in mode 11.
void fn_800BD77C(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b47) {
        lbl_80202898.u48 = uSound;
        lbl_80202898.b47 = 1;
    }
}

// Tell GameEffects which music to go back to.
void fn_800BD7D0(u8 nMusic) {
    lbl_80202898.b4E = 1;
    lbl_80202898.n4F = nMusic;
}

// The same as fn_800BD77C with GameEffects' second slot (u4C).
void fn_800BD7E8(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b4A) {
        lbl_80202898.u4C = uSound;
        lbl_80202898.b4A = 1;
    }
}

void fn_800BD83C(int nSound, int a) {
    fn_800A7664(0, nSound, a);
}

void fn_800BD868(int nSound, int a) {
    fn_800A7664(2, nSound, a);
}
