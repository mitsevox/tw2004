// statemain.c: the main state machine, one current state for the whole game, front end or in-game
// (MAINSTATE_*, the rows of gMainStateTable); the in-game state's update runs the golfer states.

#include "golfer.h"
#include "game.h"

s8            gnMainState;              // MAINSTATE_*, -1 when there is no current state
// STATEMAIN_Set sets it around a main state's enter or exit callback, but no row of gMainStateTable
// has either, so it is never seen set; the Open/Close functions clear it. Nothing reads it.
u8            gInMainStateCallback;

// Empties the main state machine (no current state); once, when the game's modules start.
void STATEMAIN_OpenONCE(void) {
    gnMainState = -1;
    gInMainStateCallback = 0;
}

// Runs the current main state's update callback, if it has one: in-game, the golfer states
// (GOLFERSTATE_Update). The frame loop calls it each frame, except during start-up (game type 1).
void STATEMAIN_Update(void) {
    if (gnMainState > -1 && gMainStateTable[gnMainState].pfnUpdate != NULL) {
        gMainStateTable[gnMainState].pfnUpdate();
    }
}

// Runs the current main state's exit callback, if it has one (no main state has one), and empties
// the machine; once, when the game's outer loop ends.
void STATEMAIN_CloseONCE(void) {
    if (gnMainState > -1 && gMainStateTable[gnMainState].pfnExit != NULL) {
        gMainStateTable[gnMainState].pfnExit();
    }
    gnMainState = -1;
    gInMainStateCallback = 0;
}

// Makes nState the current main state, running the old state's exit callback and then nState's
// enter callback with gInMainStateCallback set. No main state has either callback, so this only
// sets gnMainState.
void STATEMAIN_Set(s8 nState) {
    void (*pfn)(void);
    if (gnMainState > -1 && (pfn = gMainStateTable[gnMainState].pfnExit) != NULL) {
        gInMainStateCallback = 1;
        pfn();
        gInMainStateCallback = 0;
    }
    gnMainState = nState;
    pfn = gMainStateTable[gnMainState].pfnEnter;
    if (pfn != NULL) {
        gInMainStateCallback = 1;
        pfn();
        gInMainStateCallback = 0;
    }
}
