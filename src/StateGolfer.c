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

// Empty every player's swing stack.
void GOLFERSTATE_OpenONCE(void) {
    int i = 0;
    while (i < 5) {
        gSwingStacks[i++].nTop = -1;
    }
    gInSwingExit = 0;
}

// Run the current state's update for every player (none while fn_800E415C() is true).
void GOLFERSTATE_Update(void) {
    int i;
    if (fn_800E415C()) return;
    switch (fn_8005D2DC()) {
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

// Pop every player's states.
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

// Pop every state, running each one's exit callback.
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

// Pop everything and start again from one state.
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

s8 GOLFERSTATE_GetPreviousState(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop < 1) return -1;
    return pStack->nState[pStack->nTop - 1];
}

u8 fn_8005D2DC(void) {
    return 0;
}
