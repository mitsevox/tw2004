// Replay.c (EA's name, from its asserts; TW06): the shot replay. Records a shot while it plays
// (into the buffer at lbl_80281E48) and keeps the saved shot (gReplayData, game.h): the player
// as he was, and the spin he put on the ball. Only part of the file is decompiled so far.

#include "game.h"

void fn_8006BED4(void) {
    lbl_80281E48 = fn_80009B34(sizeof(ReplayBuffer), 2, 0, "Replay.c", 77);
    lbl_80281E48->b1525C = 0;
}

void fn_8006BF20(void) {
    fn_80009E70(lbl_80281E48);
    lbl_80281E48 = NULL;
}

void fn_8006BF4C(void) {
    gReplayData.bF10 = 0;
}

// While in-flight replays are on, the saved player uses this controller.
void fn_8006C28C(int nPlayer, int nController) {
    if (gReplayData.bF10) {
        gReplayData.player.nController = nController;
    }
}

// While in-flight replays are on, keep the spin put on the ball.
void fn_8006C2A8(int nPlayer, f32 fForwardSpin, f32 fSideSpin) {
    if (gReplayData.bF10) {
        gReplayData.fF08 = fForwardSpin;
        gReplayData.fF0C = fSideSpin;
    }
}

// The saved spin, or none when in-flight replays are off.
void fn_8006C2C8(int nPlayer, f32* pForwardSpin, f32* pSideSpin) {
    if (gReplayData.bF10) {
        *pForwardSpin = gReplayData.fF08;
        *pSideSpin = gReplayData.fF0C;
    } else {
        *pForwardSpin = 0.0f;
        *pSideSpin = 0.0f;
    }
}

// Ends a saved replay's playback.
void fn_8006C4A0(void) {
    if (gSession.bReplay) {
        gSession.bReplay = 0;
    }
}

void fn_8006C5E0(void) {
    if (!gSession.bReplay) {
        lbl_80281E48->b1525C = 1;
    }
}

void fn_8006C608(void) {
    if (!gSession.bReplay) {
        lbl_80281E48->b1525C = 0;
    }
}
