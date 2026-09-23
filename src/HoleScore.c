// HoleScore.c (our name): the player's score on the current hole relative to par.

#include "golfer.h"
#include "game.h"

// ---- sweep code (not yet cleaned up) ----

f32 fn_800D05A4(f32* pPos);
f32 fn_800D04AC(int nPlayer);
f32 fn_800D04E0(int nPlayer);

f32 fn_800D0478(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vPos);
}

f32 fn_800D04AC(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vStart);
}

f32 fn_800D04E0(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ballBefore.vPos);
}

// ---- end of sweep code ----

// The score the hole will finish on once the tap-in drops: strokes so far plus one, minus par.
int Hole_ScoreAfterTapIn(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}
