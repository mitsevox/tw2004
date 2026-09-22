// HoleScore.c (our name): the player's score on the current hole relative to par.

#include "golfer.h"

int Game_CurHoleIndex(void);

// The score the hole will finish on once the tap-in drops: strokes so far plus one, minus par.
int Hole_ScoreAfterTapIn(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}
