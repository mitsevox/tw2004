// StateGolfer.c (EA's name: TW07's Golf/State_Engine/StateGolfer.c holds GOLFERSTATE_OpenONCE ..
// GOLFERSTATE_GetPreviousState in this order): split off Swing.c at 0x8005CCAC. Its .sbss
// (gInSwingExit, padded to 8 at 0x80281E00..0x80281E08) ends before the next object's.

#include "golfer.h"
#include "game.h"

typedef struct SwingStack {
    s8   nState[5];             // 0x00
    s8   nTop;                  // 0x05  index of the current state, -1 when empty
} SwingStack;

SwingStack    gSwingStacks[5];            // 0x801D5A90
u8            gInSwingExit;               // 0x80281E00  set while a state's enter or exit callback runs

// ---- the golfer state stack ---------------------------------------------------------------------
// Each player has a small stack of golfer states (GS_*, rows of sGolferStateEngineTable, whose
// entries carry the state's callbacks). The top is the current state; -1 is empty.

// Empties every player's golfer state stack and clears gInSwingExit; once, when the game's modules
// start.
void GOLFERSTATE_OpenONCE(void) {
    int i = 0;
    while (i < 5) {
        gSwingStacks[i++].nTop = -1;
    }
    gInSwingExit = 0;
}

// Runs the current golfer state's update for every player that has one, once a frame (slot 1 of the
// mode state table that Code8005D2E4.c runs). Nothing while the pause menu is open
// (GUI_IsPauseMenuOpen) or while GOLFERSTATE_IsFrozen is set (never, in this build).
void GOLFERSTATE_Update(void) {
    int i;
    if (GUI_IsPauseMenuOpen()) return;
    switch (GOLFERSTATE_IsFrozen()) {
    case 0:
        for (i = 0; i < 5; i++) {
            SwingStack* pStack = &gSwingStacks[(u32)i];
            void (*pfn)(int);
            if (pStack->nTop > -1) {
                pfn = sGolferStateEngineTable[pStack->nState[pStack->nTop]].pfnUpdate;
                if (pfn != NULL) {
                    pfn(i);
                }
            }
        }
        break;
    }
}

// Pops every player's golfer states, top first, running each one's exit callback (gInSwingExit set
// around it); once, when the game's modules shut down.
void GOLFERSTATE_CloseONCE(void) {
    s8*         pTop;
    SwingStack* pStack;
    int         i;
    for (i = 0; i < 5; i++) {
        pStack = &gSwingStacks[(u32)i];
        pTop   = &pStack->nTop;
        while (*pTop > -1) {
            if (sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
                gInSwingExit = 1;
                sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit(i);
                gInSwingExit = 0;
            }
            (*pTop)--;
        }
    }
    gInSwingExit = 0;
}

// Pops all of one player's golfer states, top first, running each one's exit callback; the stack is
// left empty. Players_Reset does it for every player.
void GOLFERSTATE_Kill(int nPlayer) {
    s8*         pTop;
    SwingStack* pStack = &gSwingStacks[nPlayer];
    pTop = &pStack->nTop;
    while (*pTop > -1) {
        if (sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
            gInSwingExit = 1;
            sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit(nPlayer);
            gInSwingExit = 0;
        }
        (*pTop)--;
    }
}

// Push a state and run its enter callback.
void GOLFERSTATE_Push(int nState, int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    void (*pfn)(int);
    pStack->nTop++;
    pStack->nState[pStack->nTop] = nState;
    pfn = sGolferStateEngineTable[pStack->nState[pStack->nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// Pop the current state, running its exit callback.
void GOLFERSTATE_Pop(int nPlayer) {
    void (*pfn)(int);
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
    gSwingStacks[nPlayer].nTop--;
}

// Replaces a player's whole state stack with one state: pops every state, running each one's exit
// callback, then pushes nState and runs its enter callback.
void GOLFERSTATE_Set(s8 nState, int nPlayer) {
    void (*pfn)(int);
    while (gSwingStacks[nPlayer].nTop > -1) {
        if (sGolferStateEngineTable[(s8)gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit !=
            NULL) {
            gInSwingExit = 1;
            sGolferStateEngineTable[(s8)gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit(
                nPlayer);
            gInSwingExit = 0;
        }
        gSwingStacks[nPlayer].nTop--;
    }
    gSwingStacks[nPlayer].nTop = 0;
    gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop] = nState;
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// Replace the current state: its exit, then the new state's enter.
void GOLFERSTATE_Switch(int nState, int nPlayer) {
    void (*pfn)(int);
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
    gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop] = nState;
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// The current golfer state (the top of the stack), or -1.
int GOLFERSTATE_GetCurrentState(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop == -1) return -1;
    return (u8)pStack->nState[pStack->nTop];
}

// The state under the current one on the player's stack (the one a Pop returns to), -1 when there
// is none. The STATEFUNC_* exits ask it whether their state was pushed over GS_SWING.
s8 GOLFERSTATE_GetPreviousState(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop < 1) return -1;
    return pStack->nState[pStack->nTop - 1];
}

// Whether the golfer states are frozen: always 0 in this build (a stripped switch).
// GOLFERSTATE_Update runs the states only when it is 0, and the GameEffects time step (fn_800DAF98)
// stops time while it is set.
u8 GOLFERSTATE_IsFrozen(void) {
    return 0;
}
