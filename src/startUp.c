// startUp.c (EA's name, from its asserts; also in EA's 2002 source tree): the boot-time systems.
// The sound voices (lbl_802820E8: 50 wrappers around the hardware's voices, run from the mixer
// callback fn_800AF324), the audio-RAM heap and its DMA, and the 'LEGL' stream (the legal screens).
// Most of it talks to the GameCube's audio libraries; see core/startup.h.

#include "core/startup.h"

void   fn_800AF324(void);
void   fn_800AF93C(void* pVpb);
u8     fn_800AF9BC(s16 nVoice);
void   fn_800AFA2C(s16 nVoice);
void   fn_800AFBD8(u16 nVoice, u8 bOn);
int    fn_800B13FC(s32* pnA, s32* pnB);
void   fn_800B166C(UStreamObject* pObject);

// port: the GameCube's audio and ARAM libraries and their set-up.
void   AIInit(u8* pStack);
void   ARQInit(void);
void   AXInit(void);
void   MIXInit(void);
void   AXSetMode(u32 uMode);
u32    OSGetSoundMode(void);
void   fn_80145B1C(u32 uMode);          // MIX: the output mode (mono, stereo, surround)
void   fn_80136DF4(void (*pfn)(void));  // AX: the callback run after each audio frame
void*  fn_800B5BD8(u32 uSize);
u32    fn_800B5D34(void* pHeap, u32 uSize, u32 uAlign);   // ARAM heap: allocate, returns the address
void   fn_800B5E88(void* pHeap, u32 uAddr);               // ARAM heap: free
void   fn_800B6728(u32 u);
void   fn_8009527C(void* p);            // frees what fn_800951A0 allocated

// The hardware dropped a voice (to play one of higher priority): mark ours lost and stop it. The
// mixer callback asks for it back after 255 passes.
void fn_800AF93C(void* pVpb) {
    u16 i;
    Voice* p;
    for (i = 0; i < NUM_VOICES; i++) {
        p = &lbl_802820E8[i];
        if (p->pVpb == pVpb) {
            p->flags.b.bLost = 1;
            p->flags.b.n6_7F8 = 0xFF;
            AXSetVoiceState(p->pVpb, 0);
            return;
        }
    }
}

// Take a hardware voice for voice nVoice; returns whether one was free.
u8 fn_800AF9BC(s16 nVoice) {
    AXVPB* pVpb = AXAcquireVoice(1, fn_800AF93C, nVoice);
    Voice* p = &lbl_802820E8[nVoice];
    if (pVpb) {
        p->pVpb = pVpb;
        AXSetVoiceSrcType(pVpb, 1);
    }
    return pVpb != NULL;
}

// Reset a voice: silent, centred, all flags clear.
void fn_800AFA2C(s16 nVoice) {
    Voice* p = &lbl_802820E8[nVoice];
    p->flags.u = 0;
    p->n56 = 0;
    p->n58 = VOLUME_MIN;
    p->nAuxA = VOLUME_MIN;
    p->u40 = 0;
    p->nPan = 64;
    p->nSPan = 127;
    fn_80145B24(p->pVpb, 0, VOLUME_MIN, VOLUME_MIN, VOLUME_MIN, 64, 127, 0);
}

// Start the audio hardware and the voice table.
int fn_800AFAB0(void) {
    s16 i;
    AIInit(NULL);
    ARQInit();
    AXInit();
    MIXInit();
    AXSetMode(0);
    fn_80145B1C(OSGetSoundMode());
    lbl_802820E8 = fn_800B5BD8(NUM_VOICES * sizeof(Voice));
    fn_80005AE8(lbl_802820E8, 0, NUM_VOICES * sizeof(Voice));
    for (i = 0; i < NUM_VOICES; i++) {
        fn_800AF9BC(i);
        fn_800AFA2C(i);
    }
    fn_80136DF4(fn_800AF324);
    return 1;
}

int fn_800AFB48(void) {
    return 1;
}

void fn_800AFB50(void) {
    u16 i;
    for (i = 0; i < NUM_VOICES; i++) {
        fn_800AFBD8(i, 0);
    }
}

// Whether a voice is free: idle, and not waiting to get its hardware voice back.
u8 fn_800AFB98(u16 nVoice) {
    Voice* p = &lbl_802820E8[nVoice];
    return (p->flags.b.n4_E0 == 0 || p->flags.b.n4_E0 == 1) && !p->flags.b.bLost;
}

s16 fn_800AFEDC(u16 nVoice) {
    return lbl_802820E8[nVoice].n56;
}

void fn_800B0034(u16 nVoice, u8 nPan, int nMode) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (nMode == 2) {
        p->flags.b.bSetPan = 1;
        p->nPan = nPan;
    }
    OSRestoreInterrupts(bEnabled);
}

void fn_800B00A4(u16 nVoice, u32 u) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (p->u40 != u && u != 0) {
        p->u40 = u;
        p->flags.b.bSetSrc = 1;
    }
    OSRestoreInterrupts(bEnabled);
}

// Aux A (the reverb send) on at -15 dB, or off.
void fn_800B01B4(u16 nVoice, u8 bA, u8 bB) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (bA || bB) {
        p->nAuxA = -150;
        p->flags.b.bSetAuxA = 1;
        p->flags.b.b6_20 = 1;
    } else {
        p->nAuxA = VOLUME_MIN;
        p->flags.b.bSetAuxA = 1;
        p->flags.b.b6_20 = 0;
    }
    OSRestoreInterrupts(bEnabled);
}

void fn_800B0430(void) {
}

void fn_800B0434(void) {
}

int fn_800B0438(void) {
    return 1;
}

int fn_800B0440(void) {
    return 1;
}

void fn_800B0448(void) {
}

void fn_800B04CC(u32 u) {
    fn_800B6728(u);
}

// After a DMA between main memory and ARAM: when the data came into main memory (nDir 1), drop
// the CPU cache over it so the CPU reads the new bytes.
void fn_800B04EC(void* p, u32 uLen, int nDir) {
    switch (nDir) {
    case 0:
        break;
    case 1:
        DCInvalidateRange(p, uLen);
        break;
    }
}

// Before a DMA: going out (nDir 0), write the cache back so the DMA sees the data; coming in,
// drop the cache over the destination.
void fn_800B051C(void* p, u32 uLen, int nDir) {
    switch (nDir) {
    case 0:
        DCStoreRange(p, uLen);
        break;
    case 1:
        DCInvalidateRange(p, uLen);
        break;
    }
}

// The DMA callback of fn_800B0568.
void fn_800B055C(void) {
    lbl_80282110 = 1;
}

// Free the DMA buffer once the DMA is done.
int fn_800B0624(void) {
    if (lbl_80282110) {
        lbl_80282110 = 0;
        fn_8009527C(lbl_80282104);
    }
    return 1;
}

void fn_800B0660(void) {
    if (lbl_80282110) {
        lbl_80282110 = 0;
        fn_8009527C(lbl_80282104);
    }
}

// Allocate ARAM, 32-byte aligned; returns the address.
u32 fn_800B0698(u32 uSize) {
    return fn_800B5D34(lbl_802820F8, (uSize + 31) & ~31, 32);
}

void fn_800B06CC(u32 uAddr) {
    fn_800B5E88(lbl_802820F8, uAddr);
}

// Take the first free 0xFE00-byte ARAM block; returns its address.
u32 fn_800B06F4(void) {
    u32 uAddr;
    u32 uBit = 1;
    for (uAddr = lbl_80282108; uAddr < lbl_80282108 + 0x7F000; uAddr += 0xFE00) {
        if (!(lbl_802820F0 & uBit)) {
            lbl_802820F0 |= uBit;
            lbl_802820F4++;
            return uAddr;
        }
        uBit <<= 1;
    }
    // EA bug: the original has no return here, so when all eight blocks are taken the caller
    // (which tests for 0) gets the loop's address, the end of the blocks. This keeps that value.
    return uAddr;
}

void fn_800B0748(u32 uAddr) {
    u32 uBit = 1;
    lbl_802820F4--;
    lbl_802820F0 &= ~(uBit << ((uAddr - lbl_80282108) / 0xFE00));
}

int fn_800B0790(void) {
    return 0x4400;
}

int fn_800B0798(void) {
    return 1;
}

void fn_800B0954(void) {
    lbl_802814A0 = 0;
}

// Start a search from the beginning (fn_800B13FC continues it).
int fn_800B14E4(s32* pnA, s32* pnB) {
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    return fn_800B13FC(pnA, pnB);
}

void fn_800B1608(void) {
}

void fn_800B1644(void) {
    UStream_UnregisterHandler('LEGL');
}
