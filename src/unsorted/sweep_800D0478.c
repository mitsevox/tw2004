// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"

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
