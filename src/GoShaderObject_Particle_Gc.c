// GoShaderObject_Particle_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): the
// particle shader's buffers and drawing, and at its end the game's main-memory heap (made from the
// OS arena by fn_80095108) and the stopwatches the main loop times its frames with. The heap and
// the stopwatches are this file's by their data: its .sdata (0x802813A8-0x802813B8) and .sbss
// (0x80281F58-0x80281F70). Where it ends between 0x8009554C and char_state.c is not proven.

#include "engine.h"

// The main-memory heap's globals (fn_80095108, fn_800951A0), defined last-address-first.
s32 lbl_80281F68;                       // the heap's size
int lbl_80281F64;                       // the heap
u8* lbl_80281F60;                       // the end of the last block allocated
u32 lbl_80281F5C;                       // the most ever allocated
u32 lbl_80281F58;                       // allocated now, headers and padding included

// The particles' four buffers, two of 90000 bytes and two of 10000.
void fn_8009414C(void) {
    lbl_802813A8->apBuffers[0] = fn_80009B34(90000, 2, 32, "GoShaderObject_Particle_Gc.c", 109);
    lbl_802813A8->apBuffers[1] = fn_80009B34(90000, 2, 32, "GoShaderObject_Particle_Gc.c", 111);
    lbl_802813A8->apBuffers[2] = fn_80009B34(10000, 2, 32, "GoShaderObject_Particle_Gc.c", 113);
    lbl_802813A8->apBuffers[3] = fn_80009B34(10000, 2, 32, "GoShaderObject_Particle_Gc.c", 115);
    lbl_802813A8->n14 = 0;
    lbl_802813A8->b10 = 0;
}

void fn_80094214(void) {
    lbl_802813A8->n14 = 0;
    lbl_802813A8->b10 = 0;
    fn_80009E70(lbl_802813A8->apBuffers[0]);
    fn_80009E70(lbl_802813A8->apBuffers[1]);
    fn_80009E70(lbl_802813A8->apBuffers[2]);
    fn_80009E70(lbl_802813A8->apBuffers[3]);
}

void fn_80094274(void) {
}

void fn_80094278(void) {
    lbl_802813A8->b10 = 1 - lbl_802813A8->b10;
}

void fn_80094B84(void);
void fn_80094E34(void);

// A callback: what *pnWhat asks for, 0 or 1, is done by fn_80094B84 or fn_80094E34.
void fn_80095088(void* p, s32* pnWhat) {
    switch (*pnWhat) {
    case 0:
        fn_80094B84();
        break;
    case 1:
        fn_80094E34();
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_800950CC(f32 farg0, f32 farg1);
void fn_800950DC(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800950F4(f32 farg0, f32 farg1, f32 farg2);
void fn_80095364(void);

void fn_800950CC(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_800950DC(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_800950F4(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

// ---- end of sweep code ----

// Makes the main-memory heap from the arena: all of it but the top 4 MB when there is more than
// 24 MB, else all but the top 16 KB (the arena then starts at the heap's end).
void fn_80095108(void) {
    u8* pLo;
    u8* pHi;
    u8* pEnd;

    pLo = OSGetArenaLo();
    pHi = OSGetArenaHi();
    lbl_80281F68 = pHi - pLo;
    if (lbl_80281F68 > 0x1800000) {
        pEnd = pHi - 0x400000;
    } else {
        pEnd = pHi - 0x4000;
    }
    lbl_80281F68 = pEnd - pLo;
    pLo = OSInitAlloc(pLo, pEnd, 1);
    OSSetArenaLo(pLo);
    // port: the heap's bounds rounded to 32 bytes
    lbl_80281F64 = OSCreateHeap((void*)(((uptr)pLo + 0x1F) & ~0x1F), (void*)((uptr)pEnd & ~0x1F));
    OSSetCurrentHeap(lbl_80281F64);
    OSSetArenaLo(pEnd);
}

// Allocates uSize bytes aligned to nAlign (0: 16). A HeapBlockHead sits just before the result,
// and the padding's size in the byte after it (for fn_8009527C).
void* fn_800951A0(u32 uSize, u16 nAlign, int n) {
    u32 uPad;
    u32 uTotal;
    u8* pBlock;
    u32 uOff;
    u8* pData;
    u32 uPeak;

    if (uSize == 0) return NULL;
    if (nAlign == 0) {
        nAlign = 16;
    }
    uPad = nAlign - 1;
    uTotal = sizeof(HeapBlockHead) + 1 + uPad + uSize;
    pBlock = OSAllocFromHeap(__OSCurrHeap, uTotal);
    if (pBlock == NULL) return NULL;
    // port: aligned by the address
    uOff = (uptr)(pBlock + sizeof(HeapBlockHead)) % nAlign;
    if (uOff != 0) {
        uOff = nAlign - uOff;
    }
    pData = pBlock + uOff;
    ((HeapBlockHead*)pData)->pBlock = pBlock;
    ((HeapBlockHead*)pData)->uSize = uSize;
    pData += sizeof(HeapBlockHead);
    *(pData + uSize) = uPad;
    uPeak = lbl_80281F5C;
    lbl_80281F58 += uTotal;
    if (lbl_80281F58 > uPeak) {
        uPeak = lbl_80281F58;
    }
    lbl_80281F5C = uPeak;
    lbl_80281F60 = pData + uTotal;
    return pData;
}

void fn_8009527C(void* p) {
    HeapBlockHead* pHead;
    u32 uSize;
    u8 uPad;

    if (p != NULL) {
        pHead = (HeapBlockHead*)p - 1;
        uSize = pHead->uSize;
        uPad = ((u8*)p)[uSize];
        OSFreeToHeap(__OSCurrHeap, pHead->pBlock);
        lbl_80281F58 = lbl_80281F58 - uSize - (sizeof(HeapBlockHead) + 1) - uPad;
    }
}

void fn_800952D8(void) {
    int i;

    lbl_802813B0->tStart.u = 0;
    lbl_802813B0->tStart.w.nLo = fn_8000B3E8();
    lbl_802813B0->tNow = lbl_802813B0->tStart;
    for (i = 0; i < 5; i++) {
        lbl_802813B0->aWatches[i].bRunning = 0;
        fn_80095504(i);
    }
}

void fn_80095364(void) {
}

// Reads the clock: the tick's wraps are counted in the high half.
u64 fn_80095368(void) {
    u32 nTick;

    nTick = fn_8000B3E8();
    if (nTick < lbl_802813B0->tNow.w.nLo) {
        lbl_802813B0->tNow.w.nHi++;
    }
    lbl_802813B0->tNow.w.nLo = nTick;
    return lbl_802813B0->tNow.u - lbl_802813B0->tStart.u;
}

void fn_800953C8(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    pWatch->tBase.u = pWatch->tBase.u + fn_80095368() - pWatch->tStop.u;
    pWatch->bRunning = 1;
}

u8 fn_80095430(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    return pWatch->bRunning;
}

u64 fn_80095444(int nWatch) {
    ProfWatch* pWatch;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    pWatch->tStop.u = fn_80095368();
    pWatch->bRunning = 0;
    return pWatch->tStop.u - pWatch->tBase.u;
}

u64 fn_800954A4(int nWatch) {
    ProfWatch* pWatch;
    u64 uNow;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    if (pWatch->bRunning) {
        uNow = fn_80095368();
    } else {
        uNow = pWatch->tStop.u;
    }
    return uNow - pWatch->tBase.u;
}

void fn_80095504(int nWatch) {
    ProfWatch* pWatch;
    u64 uNow;

    pWatch = &lbl_802813B0->aWatches[nWatch];
    uNow = fn_80095368();
    pWatch->tStop.u = uNow;
    pWatch->tBase.u = uNow;
}
