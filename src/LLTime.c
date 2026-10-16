// LLTime.c: the game's clock, a 64-bit count of ticks since the current mode started, and the five
// counters the game times against it like stopwatches (real time, a round's time that stops while
// paused, the front end's).

#include "engine.h"

TI_STimeManager gTimeManager;
TI_STimeManager* gpTimeManager = &gTimeManager;

void TI_vInitModule(void) {
    int i;

    gpTimeManager->tStart.u = 0;
    gpTimeManager->tStart.w.nLo = TI_uiReadCP0Count();
    gpTimeManager->tNow = gpTimeManager->tStart;
    for (i = 0; i < TI_NUM_COUNTERS; i++) {
        gpTimeManager->aCounters[i].bRunning = 0;
        TI_vResetCounter(i);
    }
}

void TI_vCloseModule(void) {
}

// Reads the clock: a 32-bit tick lower than at the last reading has wrapped, and the high half
// counts one more.
u64 TI_sRead(void) {
    u32 nTick;

    nTick = TI_uiReadCP0Count();
    if (nTick < gpTimeManager->tNow.w.nLo) {
        gpTimeManager->tNow.w.nHi++;
    }
    gpTimeManager->tNow.w.nLo = nTick;
    return gpTimeManager->tNow.u - gpTimeManager->tStart.u;
}

void TI_vStartCounter(int nCounter) {
    TI_SCounter* pCounter;

    pCounter = &gpTimeManager->aCounters[nCounter];
    pCounter->tBase.u = pCounter->tBase.u + TI_sRead() - pCounter->tStop.u;
    pCounter->bRunning = 1;
}

u8 TI_bCounterIsRunning(int nCounter) {
    TI_SCounter* pCounter;

    pCounter = &gpTimeManager->aCounters[nCounter];
    return pCounter->bRunning;
}

u64 TI_sStopCounter(int nCounter) {
    TI_SCounter* pCounter;

    pCounter = &gpTimeManager->aCounters[nCounter];
    pCounter->tStop.u = TI_sRead();
    pCounter->bRunning = 0;
    return pCounter->tStop.u - pCounter->tBase.u;
}

u64 TI_sReadCounter(int nCounter) {
    TI_SCounter* pCounter;
    u64 tNow;

    pCounter = &gpTimeManager->aCounters[nCounter];
    if (pCounter->bRunning) {
        tNow = TI_sRead();
    } else {
        tNow = pCounter->tStop.u;
    }
    return tNow - pCounter->tBase.u;
}

void TI_vResetCounter(int nCounter) {
    TI_SCounter* pCounter;
    u64 tNow;

    pCounter = &gpTimeManager->aCounters[nCounter];
    tNow = TI_sRead();
    pCounter->tStop.u = tNow;
    pCounter->tBase.u = tNow;
}
