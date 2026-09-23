// startUp.c (EA's name, from its asserts; also in EA's 2002 source tree): the boot-time systems.
// The sound voices (lbl_802820E8: 50 wrappers around the hardware's voices, run from the mixer
// callback fn_800AF324), the audio-RAM heap and its DMA, and the 'LEGL' stream (the legal screens).
// Most of it talks to the GameCube's audio libraries; see core/startup.h.

#include "core/startup.h"
#include "game/frontend.h"

void   fn_800AF324(void);
void   fn_800AF93C(void* pVpb);
u8     fn_800AF9BC(s16 nVoice);
void   fn_800AFA2C(s16 nVoice);
void   fn_800AFBD8(u16 nVoice, u8 bOn);
void   fn_800AFDC8(u16 nVoice, SoundHeader* pHdr);
void   fn_800AFEF4(u16 nVoice, s16 nVolume, int a, int b);
s16    fn_800AFF9C(s16 nVolume);
void   fn_800B00A4(u16 nVoice, u32 u, int a);
void   fn_800B0114(u16 nVoice, VoiceEnvelope* pEnv);
void   fn_800B01B4(u16 nVoice, u8 bA, u8 bB);
void   fn_800B04EC(void* p, u32 uLen, int nDir);
void   fn_800B051C(void* p, u32 uLen, int nDir);
void   fn_800B055C(void);
u32    fn_800B0698(u32 uSize);
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
u32    fn_800B6564(u32 uSize);          // take ARAM, returns its address
void*  fn_800B5C40(u32 uSize, u32 uAram, u32 uAlign, void* pInfo);   // ARAM heap: create
void   fn_800B65C0(void* pSrc, u32 uAram, u32 uLen, int a, int b, void (*pfnDone)(void), int n,
                   int c);                                            // ARAM DMA
f32    fn_8000AF7C(f32 x);              // natural logarithm
void   fn_8009527C(void* p);            // frees what fn_800951A0 allocated

// The memory card code (MC.c, MC_Gc.c).
void   fn_8009CD10(void);
void   fn_8009CD7C(void);
void   fn_8009DCEC(s32 a, s32 b);
u8     fn_8009F850(void);
void   fn_8009FAD0(void);
s32    fn_800A0A7C(s32 a, s32 b);
s32    fn_800A2100(s32 a, s32 b);

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
    return (p->flags.b.nState == 0 || p->flags.b.nState == 1) && !p->flags.b.bLost;
}

// Start a set-up voice (bOn), or release a playing one into its envelope's release.
void fn_800AFBD8(u16 nVoice, u8 bOn) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (bOn) {
        if (p->flags.b.nState == 1) {
            p->flags.b.nState = 2;
            p->flags.b.bStart = 1;
            p->flags.b.bRelease = 0;
            p->flags.b.bResume = 0;
        }
    } else if (p->flags.b.nState >= 2) {
        p->flags.b.nState = 6;
        p->flags.b.bStart = 0;
        p->flags.b.bRelease = 1;
        p->flags.b.bResume = 0;
    }
    OSRestoreInterrupts(bEnabled);
}

// Pause a started voice (a released one is silenced instead), or resume it.
void fn_800AFCBC(u16 nVoice, u8 bPause) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (p->flags.b.nState >= 2) {
        if (bPause) {
            if (p->flags.b.nState == 6) {
                fn_800AFEF4(nVoice, 0, 0, 0);
            } else {
                p->flags.b.n5_03 = 2;
                p->flags.b.bResume = 0;
            }
        } else {
            p->flags.b.bResume = 1;
        }
    }
    OSRestoreInterrupts(bEnabled);
}

// Where a voice is in its sound, in bytes of ARAM: from the hardware once it plays.
u32 fn_800AFD8C(u16 nVoice) {
    Voice* p = &lbl_802820E8[nVoice];
    if (p->flags.b.nState <= 2) {
        return p->u14 >> 1;
    }
    // fake match: one 32-bit load across both halves (combining them in C loads each alone).
    // port: this reads a u32 at a 2-byte boundary and assumes big-endian; a port should use
    // ((u32)n1B2 << 16 | n1B4).
    return *(u32*)&p->pVpb->n1B2 >> 1;
}

// Set a voice up to play a sound.
void fn_800AFDC8(u16 nVoice, SoundHeader* pHdr) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    p->n8 = pHdr->uC;
    p->nA = 0;
    p->uC = pHdr->u8;
    p->u10 = pHdr->u4;
    p->u14 = pHdr->u0;
    if (pHdr->u0 < 0x106800) {
        p->flags.b.b6_40 = 1;
    } else {
        p->a18[0] = pHdr->a10[0];
        p->a18[1] = pHdr->a10[1];
        p->a18[2] = pHdr->a10[2];
        p->a18[3] = pHdr->a10[3];
        p->a18[4] = pHdr->a10[4];
        p->a18[5] = pHdr->a10[5];
        p->a18[6] = pHdr->a10[6];
        p->a18[7] = pHdr->a10[7];
        p->n3A = pHdr->n30;
        p->flags.b.b6_40 = 0;
    }
    p->n50 = pHdr->n32;
    p->n52 = pHdr->n34;
    p->n54 = pHdr->n36;
    p->flags.b.nState = 1;
    OSRestoreInterrupts(bEnabled);
}

s16 fn_800AFEDC(u16 nVoice) {
    return lbl_802820E8[nVoice].n56;
}

// Set a voice's volume, clamped to 0..0x3FFF.
void fn_800AFEF4(u16 nVoice, s16 nVolume, int a, int b) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (nVolume < 0) {
        nVolume = 0;
    } else if (nVolume > 0x3FFF) {
        nVolume = 0x3FFF;
    }
    if (p->n56 != nVolume) {
        p->n56 = nVolume;
        p->n58 = fn_800AFF9C(nVolume);
        p->flags.b.bSetInput = 1;
    }
    OSRestoreInterrupts(bEnabled);
}

// A volume (0..0x3FFF) in dB x 10, for the mixer: each halving takes 6 dB off, down to VOLUME_MIN.
s16 fn_800AFF9C(s16 nVolume) {
    s16 nDb;
    nVolume >>= 1;
    if (nVolume <= 0) return VOLUME_MIN;
    if (nVolume >= 0x3FFF) return 0;
    nDb = fn_8000AF7C(16383.0f / nVolume) * -86.5617f;
    if (nDb < VOLUME_MIN) {
        nDb = VOLUME_MIN;
    }
    return nDb;
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

void fn_800B00A4(u16 nVoice, u32 u, int a) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (p->u40 != u && u != 0) {
        p->u40 = u;
        p->flags.b.bSetSrc = 1;
    }
    OSRestoreInterrupts(bEnabled);
}

// Set a voice's envelope; a zero attack, decay or release becomes the fastest.
void fn_800B0114(u16 nVoice, VoiceEnvelope* pEnv) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    p->env = *pEnv;
    // fake match: each test is on the step the mixer takes (the field times 16 or 0x1000), and
    // each field is filled with all ones of a u16 or u8 (unsigned bit-fields keep the low bits).
    if (p->env.nAttack * 16 == 0) {
        p->env.nAttack = 0xFFFF;
    }
    if (p->env.nDecay * 0x1000 == 0) {
        p->env.nDecay = 0xFF;
    }
    if (p->env.nRelease * 16 == 0) {
        p->env.nRelease = 0xFFFF;
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

// DMA nLen bytes from main memory to ARAM; pfnDone is called when it is done.
int fn_800B044C(u32 uAram, void* pSrc, int nLen, void (*pfnDone)(void), int n) {
    fn_800B051C(pSrc, nLen, 0);
    fn_800B65C0(pSrc, uAram, nLen, 0, 1, pfnDone, n, 3);
    fn_800B04EC(pSrc, nLen, 0);
    return 1;
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

// Set up the ARAM heap: a silent block at its start, then the eight blocks of fn_800B06F4.
int fn_800B0568(void) {
    lbl_802820FC = fn_800B6564(ARAM_HEAP_SIZE);
    lbl_8028210C = fn_800B5BD8(0x2A4);
    lbl_802820F8 = fn_800B5C40(ARAM_HEAP_SIZE, lbl_802820FC, 32, lbl_8028210C);
    lbl_80282100 = fn_800B5D34(lbl_802820F8, ARAM_ZERO_SIZE, 32);
    lbl_80282104 = fn_800951A0(ARAM_ZERO_SIZE, 32, 1);
    fn_80005AE8(lbl_80282104, 0, ARAM_ZERO_SIZE);
    fn_800B044C(lbl_80282100, lbl_80282104, ARAM_ZERO_SIZE, fn_800B055C, 0);
    lbl_80282108 = fn_800B5D34(lbl_802820F8, 0x7F000, 32);
    return 1;
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

// Copy the two built-in sounds to ARAM and point their headers there.
void fn_800B07A0(void) {
    u16 i;
    for (i = 0; i < 2; i++) {
        lbl_8018FE98[i].uAram = fn_800B0698(lbl_8018FE98[i].uSize);
        lbl_8018FE98[i].hdr.u0 += lbl_8018FE98[i].uAram * 2;
        lbl_8018FE98[i].hdr.u4 += lbl_8018FE98[i].uAram * 2;
        fn_800B044C(lbl_8018FE98[i].uAram, lbl_8018FE98[i].pData, lbl_8018FE98[i].uSize, NULL, 0);
    }
}

// Play built-in sound nSound on the next voice in turn, at full volume with reverb.
void fn_800B0858(u8 nSound) {
    VoiceEnvelope env;
    env.nAttack = 0x400;
    env.nSustain = 0xF;
    env.nDecay = 0;
    env.nRelease = 0x200;
    fn_800AFDC8(lbl_80282118, &lbl_8018FE98[nSound].hdr);
    fn_800AFEF4(lbl_80282118, 0x3FFF, 0, 0);
    fn_800B00A4(lbl_80282118, lbl_8018FE98[nSound].uC, 0);
    fn_800B0114(lbl_80282118, &env);
    fn_800AFBD8(lbl_80282118, 1);
    fn_800B01B4(lbl_80282118, 1, 1);
    if (++lbl_80282118 >= NUM_VOICES) {
        lbl_80282118 = 0;
    }
}

void fn_800B0954(void) {
    lbl_802814A0 = 0;
}

void fn_800B0960(void) {
    MsgArg arg;
    if (!lbl_802814A0) {
        fn_80005AE8(&arg, 0, sizeof(arg));
        fn_8016B0F8(lbl_80281F1C->pHandler, 0x86, 1, &arg);
        return;
    }
    fn_8009CD10();
    fn_8009FAD0();
    lbl_80282120 = fn_8009F850();
    fn_8009CD7C();
}

// The messages below have no values: their one value is cleared and not counted.

void fn_800B0DB8(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x87, 0, &arg);
}

void fn_800B0DFC(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x88, 0, &arg);
}

void fn_800B0E40(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x89, 0, &arg);
}

void fn_800B0E84(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x8A, 0, &arg);
}

void fn_800B0EC8(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x8C, 0, &arg);
}

void fn_800B0F0C(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x83, 0, &arg);
}

void fn_800B0F50(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x81, 0, &arg);
}

void fn_800B0F94(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x8B, 0, &arg);
}

void fn_800B0FD8(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x80, 0, &arg);
}

void fn_800B101C(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x82, 0, &arg);
}

void fn_800B1060(void) {
    MsgArg arg;
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x8C, 0, &arg);
}

// Start a search from the beginning (fn_800B13FC continues it).
int fn_800B14E4(s32* pnA, s32* pnB) {
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    return fn_800B13FC(pnA, pnB);
}

void fn_800B1510(s32 a, s32 b) {
    MsgArg arg;
    s32 n = fn_800A2100(a, b);
    fn_8009DCEC(a, b);
    fn_80005AE8(&arg, 0, sizeof(arg));
    arg.i = n;
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x84, 1, &arg);
}

void fn_800B158C(s32 a, s32 b) {
    MsgArg arg;
    s32 n = fn_800A0A7C(a, b);
    fn_8009DCEC(a, b);
    fn_80005AE8(&arg, 0, sizeof(arg));
    arg.i = n;
    fn_8016B0F8(lbl_80281F1C->pHandler, 0x8D, 1, &arg);
}

void fn_800B1608(void) {
}

void fn_800B160C(void) {
    lbl_80282124 = 0;
    UStream_RegisterHandler('LEGL', fn_800B166C);
}

void fn_800B1644(void) {
    UStream_UnregisterHandler('LEGL');
}

// The 'LEGL' handler: keep a copy of the first two objects, free each. A copy's size is rounded up
// to 128 bytes (a size already a multiple of 128 gets 128 more).
void fn_800B166C(UStreamObject* pObject) {
    s32 nPad = 128 - (s32)pObject->uSize % 128;
    if (lbl_80282124 == 0) {
        lbl_8028212C = pObject->uSize + nPad;
        lbl_80282134 = fn_80009B34(lbl_8028212C, 2, 16, "startUp.c", 882);
        Mem_cpy(lbl_80282134, pObject->pData, lbl_8028212C);
        lbl_80282124++;
    } else if (lbl_80282124 == 1) {
        lbl_80282128 = pObject->uSize + nPad;
        lbl_80282130 = fn_80009B34(lbl_80282128, 2, 16, "startUp.c", 891);
        Mem_cpy(lbl_80282130, pObject->pData, lbl_80282128);
        lbl_80282124++;
    }
    fn_80009E70(pObject);
}
