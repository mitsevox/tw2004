// GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts): the list of morph
// animations the shader objects play (include/morphanim.h): a count, the total of their frames and
// a slot per animation.

#include "morphanim.h"

void fn_800975FC(MorphAnim* pAnim);
int  fn_80112C04(void);     // hwsRender_Gc.c: 0 on course 14's hole 11 with four players

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

// Add an animation: it takes the next slot, which starts empty.
void fn_800975B0(MorphAnim* pAnim) {
    pAnim->nIndex = lbl_80281F70->nCount;
    lbl_80281F70->nFrames += pAnim->nFrames;
    lbl_80281F70->au8[lbl_80281F70->nCount] = 0;
    lbl_80281F70->nCount++;
}

// Take an animation's frames off the totals.
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
