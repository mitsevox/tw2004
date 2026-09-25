// GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts): the list of morph
// animations the shader objects play (include/morphanim.h): a count, the total of their recorded
// vertices and a slot per animation.

#include "morphanim.h"
#include "charstate.h"
#include "golfer.h"

MorphAnimMgr* lbl_80281F70;

void fn_800975B0(MorphAnim* pAnim);
void fn_800975FC(MorphAnim* pAnim);

void fn_80097208(void) {
    lbl_80281F70 = fn_80009B34(sizeof(MorphAnimMgr), 2, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c",
                               85);
    fn_80005AE8(lbl_80281F70, 0, sizeof(MorphAnimMgr));
}

void fn_80097250(void) {
    fn_80009E70(lbl_80281F70);
    lbl_80281F70 = NULL;
}

// Set up an animation's buffers, unless fn_80112C04 rules the morphs out.
void fn_8009727C(MorphAnim* pAnim) {
    if (fn_80112C04()) {
        pAnim->fC = 0.0f;
        pAnim->nFrames = 0;
        pAnim->p10 = NULL;
        pAnim->p14 = NULL;
        pAnim->p18 = NULL;
        pAnim->p1C = NULL;
        pAnim->p10 = fn_80009B34(2000, 1, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c", 127);
        pAnim->p14 = fn_80009B34(12000, 1, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c", 129);
        pAnim->p18 = fn_80009B34(3000, 1, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c", 131);
    }
}

// Record vertex nVert's morph offset pDelta (with its position from p1C), unless it is zero or the
// vertex is recorded already. nFrames counts the vertices recorded.
void fn_80097330(MorphAnim* pAnim, u16 nVert, s8* pDelta) {
    int i;
    if (!fn_80112C04()) return;
    if (pAnim == NULL) return;
    if (pDelta[0] == 0 && pDelta[1] == 0 && pDelta[2] == 0) return;
    for (i = 0; i < pAnim->nFrames; i++) {
        if (nVert == pAnim->p10[i]) return;
    }
    pAnim->p10[i] = nVert;
    pAnim->p18[i * 3] = pDelta[0];
    pAnim->p18[i * 3 + 1] = pDelta[1];
    pAnim->p18[i * 3 + 2] = pDelta[2];
    pAnim->p14[i * 3] = pAnim->p1C[nVert * 3];
    pAnim->p14[i * 3 + 1] = pAnim->p1C[nVert * 3 + 1];
    pAnim->p14[i * 3 + 2] = pAnim->p1C[nVert * 3 + 2];
    pAnim->nFrames++;
}

// Once the display list is built: move the recorded vertices into buffers of their exact size and
// add the animation to the manager (none when no vertex morphs).
void fn_80097474(MorphAnim* pAnim) {
    u16* p10 = pAnim->p10;
    f32* p14 = pAnim->p14;
    s8* p18 = pAnim->p18;
    if (!fn_80112C04()) return;
    if (pAnim->nFrames != 0) {
        pAnim->p10 = fn_80009B34(pAnim->nFrames * sizeof(u16), 2, 32,
                                 "GoShaderObjectCommon_MorphAnimManager_Gc.c", 206);
        pAnim->p14 = fn_80009B34(pAnim->nFrames * 12, 2, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c",
                                 208);
        pAnim->p18 = fn_80009B34(pAnim->nFrames * 3, 2, 32, "GoShaderObjectCommon_MorphAnimManager_Gc.c",
                                 210);
        Mem_cpy(pAnim->p10, p10, pAnim->nFrames * sizeof(u16));
        Mem_cpy(pAnim->p14, p14, pAnim->nFrames * 12);
        Mem_cpy(pAnim->p18, p18, pAnim->nFrames * 3);
        fn_800975B0(pAnim);
    } else {
        pAnim->p10 = NULL;
        pAnim->p14 = NULL;
        pAnim->p18 = NULL;
    }
    fn_80009E70(p10);
    fn_80009E70(p14);
    fn_80009E70(p18);
}

// Add an animation: it takes the next slot, which starts empty.
void fn_800975B0(MorphAnim* pAnim) {
    pAnim->nIndex = lbl_80281F70->nCount;
    lbl_80281F70->nFrames += pAnim->nFrames;
    lbl_80281F70->au8[lbl_80281F70->nCount] = 0;
    lbl_80281F70->nCount++;
}

// Take an animation off the manager: one less, and its vertices off the total.
void fn_800975FC(MorphAnim* pAnim) {
    lbl_80281F70->nCount--;
    lbl_80281F70->nFrames -= pAnim->nFrames;
}

// Free an animation's data.
void fn_80097624(MorphAnim* pAnim) {
    if (pAnim->nFrames != 0) {
        fn_80009E70(pAnim->p10);
        fn_80009E70(pAnim->p14);
        fn_80009E70(pAnim->p18);
        pAnim->p10 = NULL;
        pAnim->p14 = NULL;
        pAnim->p18 = NULL;
        fn_800975FC(pAnim);
    }
}

s32 fn_80097688(void) {
    return lbl_80281F70->nCount;
}

// The frame slot nIndex's animation was last brought up to date in.
u32 fn_80097694(u8 nIndex) {
    return lbl_80281F70->au8[nIndex];
}

// Move each recorded vertex to its position plus its morph offset scaled by fC * fWeight (the
// offsets are s8s in 127ths), and stamp the animation's slot with this frame.
void fn_800976A8(MorphAnim* pAnim, f32 fWeight) {
    int i = pAnim->nFrames - 1;
    f32 fScale = (1.0f / 127.0f) * (pAnim->fC * fWeight);
    // EA bug: zero frames and the final decrements form out-of-range pointers; neither is dereferenced.
    // port: form these pointers only inside a nonempty loop and skip the final decrements.
    u16* pVert = pAnim->p10 + i;
    f32* pX = pAnim->p1C;
    f32* pBase = pAnim->p14 + i * 3 + 2;
    s8* pDelta = pAnim->p18 + i * 3 + 2;
    f32* pY = pX + 1;
    f32* pZ = pX + 2;

    for (; i >= 0; i--) {
        u16 nVert = *pVert;
        int n = nVert * 3;
        pZ[n] = pDelta[0] * fScale + pBase[0];
        pY[n] = pDelta[-1] * fScale + pBase[-1];
        pX[n] = pDelta[-2] * fScale + pBase[-2];
        pVert--;
        pDelta -= 3;
        pBase -= 3;
    }
    lbl_80281F70->au8[pAnim->nIndex] = gSession.nFrameCount;
}
