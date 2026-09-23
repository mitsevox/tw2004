// uiArc.c (our name): a menu UI element drawn as an arc or circle: nSegments pieces from fStart
// to fEnd degrees (0 to 360 by default), shaded from one colour to another (fn_80102AC8), and
// the message handler that sets it up (fn_80103684).

#include "game/frontend.h"

// fe_movies.c
void fn_800913EC(s16 n2, s16 n0, s16 n8, s32 a, s32 b);
void fn_80091460(s16 n2, s16 n0, s16 n8, s32 a, s32 b);

void fn_80102AC8(UIArc* pArc, s32 a, s32 b);

// The element's messages: -1, -3 pass it on, -2 draws it, the others set its fields.
void fn_80103684(UIArc* pArc, int nMsg, s32 n, MsgArg* pArgs) {
    switch (nMsg) {
    case -1:
        fn_800913EC(pArc->n2, pArc->n0, pArc->u8, 0, 0);
        return;
    case -2:
        fn_80102AC8(pArc, 0, 0);
        return;
    case -3:
        fn_80091460(pArc->n2, pArc->n0, pArc->u8, 0, 0);
        return;
    case 1:
        pArc->v20[0] = pArgs[0].f;
        pArc->v20[1] = pArgs[1].f;
        return;
    case 2:
        pArc->v28[0] = pArgs[0].f;
        pArc->v28[1] = pArgs[1].f;
        return;
    case 3:
        pArc->fStart = pArgs[0].f;
        return;
    case 4:
        pArc->fEnd = pArgs[0].f;
        return;
    case 5:
        pArc->v18[0] = pArgs[0].f;
        pArc->v18[1] = pArgs[1].f;
        return;
    case 6:
        pArc->aColorA[0] = pArgs[0].i;
        pArc->aColorA[1] = pArgs[1].i;
        pArc->aColorA[2] = pArgs[2].i;
        pArc->aColorA[3] = pArgs[3].i;
        return;
    case 7:
        pArc->aColorB[0] = pArgs[0].i;
        pArc->aColorB[1] = pArgs[1].i;
        pArc->aColorB[2] = pArgs[2].i;
        pArc->aColorB[3] = pArgs[3].i;
        return;
    case 8:
        pArc->nSegments = pArgs[0].i;
        return;
    case 11:
        pArc->nQuarterTurns = (pArgs[0].i / 90) % 4;
        return;
    case 12:                    // taken, and ignored
    case 13:
    case 14:
        return;
    }
}
