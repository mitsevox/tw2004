// CharAnim.c (our name): starting the golfer's animations by id.

#include "golfer.h"

int  Hole_ScoreAfterTapIn(int nPlayer);
void fn_8001C7FC(u8* pChar, int nStyle);   // the character's animation style (+0x16E0)
void CharacterState_AddSKABlendData(u8* pChar, int a, int nGroup, void* pfn, int c, int d, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5);
void fn_80072ACC(void);

// Animation 11, the gimme tap-in: the style is the score the tap-in will give (under par 6, par
// 5, over par 2; the style-0 branch repeats the par test and can never be taken), then clip group
// 9 plays. Which clip that is comes from the golfer's animation library (AnimLib_Pick): the two
// standard tap-ins, plus the pool-cue tap-in for a few golfers.
void CharAnim_StartTapIn(u8* pChar) {
    int nScore;
    if (pChar == NULL) return;
    nScore = Hole_ScoreAfterTapIn(*(s32*)(pChar + 4));
    if (nScore < 0) {
        fn_8001C7FC(pChar, 6);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 5);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 0);
    } else {
        fn_8001C7FC(pChar, 2);
    }
    CharacterState_AddSKABlendData(pChar, 1, 9, fn_80072ACC, 1, 8, -10000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
}
