// Gimme.c (our name): whether a ball that has stopped is close enough to be tapped in for the
// player. Called from swing state 14 once the ball is at rest; yes leads to state 15 (plan the
// tap-in) and state 16 (the tap-in itself, played for the player).

#include "golfer.h"

f32 fn_800D0478(int nPlayer);           // the ball's distance from the pin (yards)

// A gimme: the Gimmes option is on, it is not split screen or a replay, the session is not in the
// mode with both flag bits 0x4000 and 0x8000, the game mode allows gimmes and its rules callback
// does not object, and the ball is within half a yard (18 inches) of the pin - on the putter, or
// in any shot of a one-player game.
u8 Gimme_Allowed(int nPlayer) {
    if (!SESSION_OPTIONS->bGimmes) return 0;
    if (gSession.nSplitScreen) return 0;
    if (gSession.bReplay) return 0;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) return 0;
    if (!gpGame->bGimmesAllowed) return 0;
    if (gpGame->pfn1D8(nPlayer, 1)) return 0;
    if (fn_800D0478(nPlayer) > 0.5f) return 0;
    if (gPlayers[nPlayer].nClub != CLUB_PUTTER && gSession.nNumPlayers > 1) return 0;
    return 1;
}
