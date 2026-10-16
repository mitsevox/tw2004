// StateGolfer.c: the golfer state engine. Each player has a stack of golfer states (GolferState:
// pre-shot, shot set-up, swing, the aiming and green cameras, ...), whose enter, update and exit
// callbacks are the rows of sGolferStateEngineTable; these functions push, pop, switch and run them.

#include "golfer.h"
#include "game.h"

GolferStack   gGolferStacks[GOLFERSTATE_NUM_STACKS];    // one per player slot
u8            gInStateCallback;     // set while a state's enter or exit callback runs; nothing reads it

// Empties every player's golfer state stack; once, before the game's outer loop starts.
void GOLFERSTATE_OpenONCE(void) {
    int PlayerID = 0;
    while (PlayerID < GOLFERSTATE_NUM_STACKS) {
        gGolferStacks[PlayerID++].nTop = -1;
    }
    gInStateCallback = 0;
}

// Runs the current golfer state's update for every player that has one, once a frame (the update of
// MAINSTATE_INGAME, which STATEMAIN_Update runs). Nothing runs while the pause menu is open, or
// while GOLFERSTATE_IsFrozen says so (never, in this build).
void GOLFERSTATE_Update(void) {
    int PlayerID;
    if (GUI_IsPauseMenuOpen()) return;
    switch (GOLFERSTATE_IsFrozen()) {
    case 0:
        for (PlayerID = 0; PlayerID < GOLFERSTATE_NUM_STACKS; PlayerID++) {
            GolferStack* pStack = &gGolferStacks[(u32)PlayerID];
            void (*pfnUpdate)(int);
            if (pStack->nTop > -1) {
                pfnUpdate = sGolferStateEngineTable[pStack->aStates[pStack->nTop]].pfnUpdate;
                if (pfnUpdate != NULL) {
                    pfnUpdate(PlayerID);
                }
            }
        }
        break;
    }
}

// Pops every player's golfer states, top first, running each one's exit callback; once, when the
// game's outer loop ends.
void GOLFERSTATE_CloseONCE(void) {
    s8*          pTop;
    GolferStack* pStack;
    int          PlayerID;
    for (PlayerID = 0; PlayerID < GOLFERSTATE_NUM_STACKS; PlayerID++) {
        pStack = &gGolferStacks[(u32)PlayerID];
        pTop   = &pStack->nTop;
        while (*pTop > -1) {
            if (sGolferStateEngineTable[(s8)pStack->aStates[*pTop]].pfnExit != NULL) {
                gInStateCallback = 1;
                sGolferStateEngineTable[(s8)pStack->aStates[*pTop]].pfnExit(PlayerID);
                gInStateCallback = 0;
            }
            (*pTop)--;
        }
    }
    gInStateCallback = 0;
}

// Pops all of one player's golfer states, top first, running each one's exit callback; the stack is
// left empty. Players_Reset does it for each of the session's players.
void GOLFERSTATE_Kill(int PlayerID) {
    s8*          pTop;
    GolferStack* pStack = &gGolferStacks[PlayerID];
    pTop = &pStack->nTop;
    while (*pTop > -1) {
        if (sGolferStateEngineTable[(s8)pStack->aStates[*pTop]].pfnExit != NULL) {
            gInStateCallback = 1;
            sGolferStateEngineTable[(s8)pStack->aStates[*pTop]].pfnExit(PlayerID);
            gInStateCallback = 0;
        }
        (*pTop)--;
    }
}

// Pushes newState over the player's current state and runs its enter callback. The current state's
// exit callback does not run; the stack's depth is not checked.
void GOLFERSTATE_Push(s8 newState, int PlayerID) {
    GolferStack* pStack = &gGolferStacks[PlayerID];
    void (*pfnEnter)(int);
    pStack->nTop++;
    pStack->aStates[pStack->nTop] = newState;
    pfnEnter = sGolferStateEngineTable[pStack->aStates[pStack->nTop]].pfnEnter;
    if (pfnEnter != NULL) {
        gInStateCallback = 1;
        pfnEnter(PlayerID);
        gInStateCallback = 0;
    }
}

// Pops the player's current state, running its exit callback; the state under it becomes current
// again without its enter callback running.
void GOLFERSTATE_Pop(int PlayerID) {
    void (*pfnExit)(int);
    pfnExit = sGolferStateEngineTable[gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]].pfnExit;
    if (pfnExit != NULL) {
        gInStateCallback = 1;
        pfnExit(PlayerID);
        gInStateCallback = 0;
    }
    gGolferStacks[PlayerID].nTop--;
}

// Replaces a player's whole state stack with one state: pops every state, running each one's exit
// callback, then makes newState the only state and runs its enter callback.
void GOLFERSTATE_Set(s8 newState, int PlayerID) {
    void (*pfnEnter)(int);
    while (gGolferStacks[PlayerID].nTop > -1) {
        if (sGolferStateEngineTable[(s8)gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]]
                .pfnExit != NULL) {
            gInStateCallback = 1;
            sGolferStateEngineTable[(s8)gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]]
                .pfnExit(PlayerID);
            gInStateCallback = 0;
        }
        gGolferStacks[PlayerID].nTop--;
    }
    gGolferStacks[PlayerID].nTop = 0;
    gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop] = newState;
    pfnEnter =
        sGolferStateEngineTable[gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]].pfnEnter;
    if (pfnEnter != NULL) {
        gInStateCallback = 1;
        pfnEnter(PlayerID);
        gInStateCallback = 0;
    }
}

// Replaces the player's current state with newState: the current state's exit callback, then
// newState's enter callback. The stack must not be empty.
void GOLFERSTATE_Switch(s8 newState, int PlayerID) {
    void (*pfnCallback)(int);
    pfnCallback =
        sGolferStateEngineTable[gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]].pfnExit;
    if (pfnCallback != NULL) {
        gInStateCallback = 1;
        pfnCallback(PlayerID);
        gInStateCallback = 0;
    }
    gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop] = newState;
    pfnCallback =
        sGolferStateEngineTable[gGolferStacks[PlayerID].aStates[gGolferStacks[PlayerID].nTop]].pfnEnter;
    if (pfnCallback != NULL) {
        gInStateCallback = 1;
        pfnCallback(PlayerID);
        gInStateCallback = 0;
    }
}

// The player's current golfer state (the top of the stack), or -1 when the stack is empty.
GolferState GOLFERSTATE_GetCurrentState(int PlayerID) {
    GolferStack* pStack = &gGolferStacks[PlayerID];
    if (pStack->nTop == -1) return -1;
    return (u8)pStack->aStates[pStack->nTop];
}

// The state under the current one on the player's stack (the one a Pop returns to), or -1 when
// there is none. The camera states' exits (zoom, elevator, green, ...) ask it whether their state
// was pushed over GS_SWING.
s8 GOLFERSTATE_GetPreviousState(int PlayerID) {
    GolferStack* pStack = &gGolferStacks[PlayerID];
    if (pStack->nTop < 1) return -1;
    return pStack->aStates[pStack->nTop - 1];
}

// Whether the golfer states are frozen: never, in this build. GOLFERSTATE_Update runs the states
// only when it is 0, and the game-effects time step (GameEffects_AdjustTimeRate) stops time while
// it is set.
u8 GOLFERSTATE_IsFrozen(void) {
    return 0;
}
