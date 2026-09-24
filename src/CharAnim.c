// CharAnim.c (our name; TW06's char_state.c, its functions CharacterState_*): the golfer's
// animation state - the blend data added for each clip group, the idle and emotion updates, and
// the queued state changes played out on the second animation player (Character.anim29C).

#include "golfer.h"
#include "game.h"
#include "engine.h"

f32   fn_8001F02C(ClipBlend* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)

void  fn_800732F4(void* pNode, void* pAnim, f32 fTime);   // set a blend node's time (blend or node3E0)

void  fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f);
void  fn_80095FD0(Character* pChar, void* pClip, u8 bKeep, int n, SKABlendFn pfnBlend, int c, int d, f32 f1,
                  f32 f2, f32 f3, f32 f4, f32 f5);
void  fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset);
s8    fn_80096338(void);
s32   fn_800962F8(Character* pChar);
s32   fn_80096508(void);
int   fn_80096530(Character* pChar);
f32   fn_800971B8(Character* pChar);

void fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f) {
    pAnim->nC  = n;
    pAnim->f10 = f;
}

// Start pClip on the second player (fn_80095FD0) and queue state 4; unless bKeep, its blend node
// first moves to fOffset past the player's time.
void fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset) {
    if (pClip == NULL) return;
    if (!bKeep) {
        fn_800732F4(pChar->node3E0, &pChar->anim29C, pChar->anim29C.fTime + fOffset);
    }
    fn_80095FD0(pChar, pClip, bKeep, 0, fn_80072ACC, 1, 5, -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
    pChar->n2C = 4;
    pChar->n30 = 4;
    pChar->u28 |= 1;
    if (pChar->n20 == 5) {
        pChar->anim29C.f10 -= 0.95f;
    }
}

// Start the idle wait over: a new count (fn_80096338) and the other two counters cleared.
s32 fn_800962F8(Character* pChar) {
    pChar->n24 = fn_80096338();
    pChar->n25 = 0;
    pChar->n26 = 0;
    return 2;
}

// A random wait of 8 to 10.
s8 fn_80096338(void) {
    return Rand_Next(1) % 3 + 8;
}

u8 fn_8009637C(Character* pChar) {
    return pChar->n26 != 1;
}

s32 fn_80096508(void) {
    Rand_Next(1);
    return 1;
}

// TW06: CharacterState_UpdateGameEmotionState. The animation style from how the shot turned out.
int fn_80096530(Character* pChar) {
    int aStyle[10] = {5, 6, 7, 2, 1, 0, 3, 4, 5, 2};
    int nResult    = fn_8006AA9C(pChar->nPlayer);
    fn_8001C7FC(pChar, aStyle[nResult]);
    return nResult;
}

// TW06: CharacterState_SetTapInState. Animation 11, the gimme tap-in: the style is the score the
// tap-in will give (under par 6, par 5, over par 2; the style-0 branch repeats the par test and can
// never be taken), then clip group 9 plays. Which clip that is comes from the golfer's animation
// library (AnimLib_Pick): the two standard tap-ins, plus the pool-cue tap-in for a few golfers.
void CharAnim_StartTapIn(Character* pChar) {
    int nScore;
    if (pChar == NULL) return;
    nScore = Hole_ScoreAfterTapIn(pChar->nPlayer);
    if (nScore < 0) {
        fn_8001C7FC(pChar, 6);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 5);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 0);
    } else {
        fn_8001C7FC(pChar, 2);
    }
    CharacterState_AddSKABlendData(pChar, 1, 9, fn_80072ACC, 1, 8, -10000.0f, -30000.0f, -10000.0f, 0.0f,
                                   -10000.0f);
}

// Halfway between the blend's second clip's f0C and the time of the blend's event 2.
f32 fn_800971B8(Character* pChar) {
    f32 fTime  = fn_8001F02C(pChar->pBlend, 2);
    f32 fStart = pChar->pBlend->pD8->f0C;
    fTime = (fTime - fStart) / 2.0f + fStart;
    return fTime;
}
