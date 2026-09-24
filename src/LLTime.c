// LLTime.c (EA file, TW07 legacy/ll/LLTime.c): own unit, its .sdata (0x802813B0) starts on the
// 8-aligned address after GoShaderObject_Particle_Gc.c's padding at 0x802813AC..0x802813B0

#include "engine.h"

void TI_vInitModule(void) {
    int i;

    lbl_802813B0->tStart.u = 0;
    lbl_802813B0->tStart.w.nLo = fn_8000B3E8();
    lbl_802813B0->tNow = lbl_802813B0->tStart;
    for (i = 0; i < 5; i++) {
        lbl_802813B0->aWatches[i].bRunning = 0;
        TI_vResetCounter(i);
    }
}

void TI_vCloseModule(void) {
}

// Reads the clock: the tick's wraps are counted in the high half.
u64 TI_sRead(void) {
    u32 nTick;

    nTick = fn_8000B3E8();
    if (nTick < lbl_802813B0->tNow.w.nLo) {
        lbl_802813B0->tNow.w.nHi++;
    }
    lbl_802813B0->tNow.w.nLo = nTick;
    return lbl_802813B0->tNow.u - lbl_802813B0->tStart.u;
}

void TI_vStartCounter(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    pWatch->tBase.u = pWatch->tBase.u + TI_sRead() - pWatch->tStop.u;
    pWatch->bRunning = 1;
}

u8 TI_bCounterIsRunning(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    return pWatch->bRunning;
}

u64 TI_sStopCounter(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    pWatch->tStop.u = TI_sRead();
    pWatch->bRunning = 0;
    return pWatch->tStop.u - pWatch->tBase.u;
}

u64 TI_sReadCounter(int nWatch) {
    ProfWatch* pWatch;
    u64 uNow;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    if (pWatch->bRunning) {
        uNow = TI_sRead();
    } else {
        uNow = pWatch->tStop.u;
    }
    return uNow - pWatch->tBase.u;
}

void TI_vResetCounter(int nWatch) {
    ProfWatch* pWatch;
    u64 uNow;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    uNow = TI_sRead();
    pWatch->tStop.u = uNow;
    pWatch->tBase.u = uNow;
}
