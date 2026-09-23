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
s32    fn_800B09C8(int nSlot, int n);
void   fn_800B0DB8(void);
void   fn_800B0DFC(void);
void   fn_800B0E40(void);
void   fn_800B0E84(void);
void   fn_800B0EC8(void);
void   fn_800B0F0C(void);
void   fn_800B0F50(void);
void   fn_800B0F94(void);
void   fn_800B0FD8(void);
void   fn_800B101C(void);
void   fn_800B1060(void);
void   fn_800B10A4(void);
u8     fn_800B1180(void);
s32    fn_800B12FC(s32* pnSlot, s32* pn);
int    fn_800B13FC(s32* pnSlot, s32* pn);
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
void   AXSetVoiceSrc(AXVPB* pVpb, f32 fRatio);          // the playback rate
void   AXSetVoiceAdpcm(AXVPB* pVpb, u32* pCoefs);
void   AXSetVoiceAdpcmLoop(AXVPB* pVpb, u16* pLoop);
void   AXSetVoiceType(AXVPB* pVpb, u32 uType);
void   MIXSetInput(AXVPB* pVpb, int nDb);
void   MIXSetAuxA(AXVPB* pVpb, int nDb);
void   MIXSetPan(AXVPB* pVpb, int nPan);
void   MIXSetSPan(AXVPB* pVpb, int nSPan);
void   MIXMute(AXVPB* pVpb);
void   MIXUnMute(AXVPB* pVpb);
void   fn_80146B18(void);               // MIX: pass the settings to the hardware
f32    fn_8000AF7C(f32 x);              // natural logarithm
void   fn_8009527C(void* p);            // frees what fn_800951A0 allocated
void   fn_800B1A88(f32* pA, f32* pB);   // swap two floats
void   fn_80110458(u8 b);

// The memory card code (MC.c, MC_Gc.c).
void   fn_8009CD10(void);
void   fn_8009CD7C(void);
void   fn_8009DCEC(s32 a, s32 b);
u8     fn_8009F7E8(int nSlot);
u8     fn_8009F850(void);
s32    fn_8009FAD0(void);               // returns the card result, 0 when it worked
s32    fn_800A0A7C(s32 a, s32 b);
s32    fn_800A2100(s32 a, s32 b);
void   fn_8009F7F4(CardInfo* pInfo, int nSlot, int n);     // copy out a card's state
s32    fn_8009D3DC(s32 nSlot, s32 n);
s32    fn_8009D50C(s32 nSlot, s32 n);
s32    fn_8009EE28(s32 nSlot, s32 n);

// The save kinds (a table of functions at lbl_8018C7D8).
void   fn_80084FF0(s32 nKind);          // pick a save kind
s32    fn_80084FB4(CardPos* pPos);      // the picked save kind's size on that card

// The two built-in sounds: where their data is, its size, its playback rate (16.16 fixed point:
// 0.5 and 0.25) and its header, with the addresses counted from the data's start (fn_800B07A0
// rebases them into ARAM).
BootSound lbl_8018FE98[2] = {
    {lbl_8018F040, 0x600, 0, 0x8000,
     {2, 0xBC9, 0x8002, 0,
      {0x024302E8, 0x0C3BF9E6, 0x0807FE3A, 0x0E9DF887, 0x00420786, 0x0CC7FA46, 0x0A2FFD3B, 0x0E53F921},
      0x40, 0, 0, 0}},
    {lbl_8018F640, 0x860, 0, 0x4000,
     {2, 0x10AA, 0x8002, 0,
      {0x0E77F80C, 0x0FAEF80D, 0x0F44F808, 0x0FD7F818, 0x0EABF858, 0x0FC7F813, 0x0F7FF80C, 0x0FDDF81B},
      0x37, 0, 0, 0}},
};

s32 lbl_80281498 = -1;
s32 lbl_8028149C = -1;
u8  lbl_802814A0 = 1;

// Defined last-address-first: the compiler lays an object's uninitialised data out in reverse.
s32    lbl_80282150[NUM_CARD_SLOTS][1];
s32    lbl_80282148[NUM_CARD_SLOTS][1];
s32    lbl_80282140[NUM_CARD_SLOTS][1];
s32    lbl_80282138[NUM_CARD_SLOTS];
void*  lbl_80282134;
void*  lbl_80282130;
u32    lbl_8028212C;
u32    lbl_80282128;
s32    lbl_80282124;
u8     lbl_80282120;
// fake match: lbl_8028211C, lbl_80282114 and lbl_802820EC are never used by the game's code, but
// the original's data has a word at each of these addresses (likely the globals of functions the
// linker stripped); kept through the dead-stripping so the rest lines up. Types unknown.
KEEP_UNUSED u32 lbl_8028211C;
u16    lbl_80282118;
KEEP_UNUSED u32 lbl_80282114;
u8     lbl_80282110;
void*  lbl_8028210C;
u32    lbl_80282108;
void*  lbl_80282104;
u32    lbl_80282100;
u32    lbl_802820FC;
void*  lbl_802820F8;
s32    lbl_802820F4;
u32    lbl_802820F0;
KEEP_UNUSED u32 lbl_802820EC;
Voice* lbl_802820E8;

// The mixer callback, run after every audio frame: for each voice, ask for a lost hardware voice
// back, pass changed settings on to the hardware, start, release, pause and resume it, and step
// its volume envelope.
void fn_800AF324(void) {
    s16 i;
    Voice* p;
    u8 bMoved;
    u16 nOld;
    s32 nLevel;
    u8 bStage;
    AXVPB* pVpb;
    u32* pDst;
    u32* pSrc;
    p = lbl_802820E8;
    for (i = 0; i < NUM_VOICES; i++, p++) {
        bMoved = 0;
        if (p->flags.b.bLost) {
            p->flags.b.n6_7F8--;
            if (p->flags.b.n6_7F8 != 0) continue;
            if (fn_800AF9BC(i)) {
                fn_800AFA2C(i);
                p->flags.b.bLost = 0;
            } else {
                p->flags.b.n6_7F8 = 0xFF;
            }
        }
        // port: both reads are a u32 at a 2-byte boundary (see fn_800AFD8C).
        pVpb = p->pVpb;
        if (pVpb->n146 != 0 && *(u32*)&pVpb->n1AE - *(u32*)&pVpb->n1B2 > 0x40000 &&
            *(u32*)&pVpb->n1B2 > 0x8800) {
            AXSetVoiceState(pVpb, 0);
            fn_800AFA2C(i);
        }
        if (p->flags.b.bSetPan) {
            p->flags.b.bSetPan = 0;
            MIXSetPan(p->pVpb, p->nPan);
            bMoved = 1;
        }
        if (p->flags.b.bSetSPan) {
            p->flags.b.bSetSPan = 0;
            MIXSetSPan(p->pVpb, p->nSPan);
            bMoved = 1;
        }
        if (p->flags.b.bSetSrc) {
            p->flags.b.bSetSrc = 0;
            AXSetVoiceSrc(p->pVpb, (1.0f / 65536.0f) * p->u40);
        }
        if (p->flags.b.bSetAuxA) {
            p->flags.b.bSetAuxA = 0;
            MIXSetAuxA(p->pVpb, p->nAuxA);
        }
        if (p->flags.b.bSetAdpcm) {
            p->flags.b.bSetAdpcm = 0;
            p->flags.b.b6_10 = 1;
            AXSetVoiceAdpcm(p->pVpb, p->a18);
        }
        if (p->flags.b.bSetLoop) {
            p->flags.b.bSetLoop = 0;
            AXSetVoiceAdpcmLoop(p->pVpb, &p->n50);
        }
        if ((p->flags.b.nState == 3 || p->flags.b.nState == 4 || p->flags.b.nState == 6) &&
            p->pVpb->n146 == 0) {
            fn_800AFA2C(i);
        }
        if (p->flags.b.bStart) {
            if (!bMoved && p->u40 != 0) {
                pVpb = p->pVpb;
                if (p->flags.b.b6_40 && !p->flags.b.b6_10) {
                    fn_800AFA2C(i);
                } else {
                    // port: 0x4C bytes copied as words to a 2-byte boundary.
                    pDst = (u32*)&pVpb->n1A6;
                    pSrc = (u32*)&p->n8;
                    pDst[0] = pSrc[0];
                    pDst[1] = pSrc[1];
                    pDst[2] = pSrc[2];
                    pDst[3] = pSrc[3];
                    pDst[4] = pSrc[4];
                    pDst[5] = pSrc[5];
                    pDst[6] = pSrc[6];
                    pDst[7] = pSrc[7];
                    pDst[8] = pSrc[8];
                    pDst[9] = pSrc[9];
                    pDst[10] = pSrc[10];
                    pDst[11] = pSrc[11];
                    pDst[12] = pSrc[12];
                    pDst[13] = pSrc[13];
                    pDst[14] = pSrc[14];
                    pDst[15] = pSrc[15];
                    pDst[16] = pSrc[16];
                    pDst[17] = pSrc[17];
                    pDst[18] = pSrc[18];
                    p->pVpb->u1C |= 0x161000;
                    p->flags.b.bStart = 0;
                    p->flags.b.nState = 3;
                    p->flags.b.nEnvStage = 0;
                    p->n64 = 0;
                    p->n66 = 0xFFFF;
                    AXSetVoiceType(p->pVpb, p->flags.b.b6_40 != 0);
                    AXSetVoiceState(p->pVpb, 1);
                }
            }
        } else if (p->flags.b.bRelease) {
            p->flags.b.bRelease = 0;
            p->flags.b.nEnvStage = 3;
            p->n66 = 0;
        } else if (p->flags.b.n5_03 != 0) {
            // a pause: the first pass mutes the voice, the next stops it
            switch (p->flags.b.n5_03) {
            case 2:
                p->flags.b.nState = 4;
                MIXMute(p->pVpb);
                break;
            case 1:
                AXSetVoiceState(p->pVpb, 0);
                p->flags.b.nState = 5;
                break;
            }
            p->flags.b.n5_03--;
        } else if (p->flags.b.bResume && !bMoved && p->flags.b.nState >= 3) {
            p->flags.b.bResume = 0;
            if (*(u32*)&p->pVpb->n1B2 > 0x8800) {
                if (p->flags.b.nEnvStage != 3) {
                    p->flags.b.nState = 3;
                } else {
                    p->flags.b.nState = 6;
                }
                MIXUnMute(p->pVpb);
                AXSetVoiceState(p->pVpb, 1);
            }
        }
        if (p->flags.b.nState == 3 || p->flags.b.nState == 4 || p->flags.b.nState == 6) {
            nOld = p->n64;
            nLevel = nOld;
            bStage = 0;
            if (nOld == p->n66) {
                if (p->flags.b.nEnvStage < 2) {
                    p->flags.b.nEnvStage++;
                    if (p->flags.b.nEnvStage == 1) {
                        p->n66 = (p->env.nSustain << 12) | 0xFFF;
                    }
                    bStage = 1;
                } else if (p->flags.b.nEnvStage == 3) {
                    AXSetVoiceState(p->pVpb, 0);
                }
            }
            switch (p->flags.b.nEnvStage) {
            case 0:
                nLevel += p->env.nAttack * 16;
                if (nLevel > 0xFFFF) {
                    nLevel = 0xFFFF;
                }
                break;
            case 1:
                nLevel -= p->env.nDecay * 0x1000;
                if (nLevel < p->n66) {
                    nLevel = p->n66;
                }
                break;
            case 2:
                break;
            case 3:
                nLevel -= p->env.nRelease * 16;
                if (nLevel < 0) {
                    nLevel = 0;
                }
                break;
            }
            if (bStage || nLevel != nOld || p->flags.b.bSetInput) {
                p->n64 = nLevel;
                p->flags.b.bSetInput = 0;
                // the envelope's level (0..0xFFFF) scales the voice's volume (n58, dB x 10)
                nLevel *= p->n58 + 0x400;
                MIXSetInput(p->pVpb, nLevel / 0x10000 - 0x3FF);
            }
        }
    }
    fn_80146B18();
}

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

// A streamed sound's first chunk (nBuffer 0, the first half of its ARAM buffer) sets the voice's
// decoder: the chunk's coefficients and its first frame's header, also used when it loops.
void fn_800B0268(u16 nVoice, StreamChunk* pChunk, u32 uSize, int nBuffer) {
    Voice* p = &lbl_802820E8[nVoice];
    if (nBuffer == 0) {
        u8 nHeader = pChunk->aData[0];
        int bEnabled = OSDisableInterrupts();
        p->a18[0] = pChunk->a0[0];
        p->a18[1] = pChunk->a0[1];
        p->a18[2] = pChunk->a0[2];
        p->a18[3] = pChunk->a0[3];
        p->a18[4] = pChunk->a0[4];
        p->a18[5] = pChunk->a0[5];
        p->a18[6] = pChunk->a0[6];
        p->a18[7] = pChunk->a0[7];
        p->n3A = nHeader;
        p->n50 = nHeader;
        if (!p->flags.b.b6_10) {
            p->flags.b.bSetAdpcm = 1;
        }
        p->flags.b.bSetLoop = 1;
        OSRestoreInterrupts(bEnabled);
    }
}

// Set a movie voice's decoder from a block of the movie's sound: channel 0 (left) or 1 (right).
// nMode 0 also sets the header used when it loops.
void fn_800B0338(u16 nVoice, MovieSoundBlock* pBlock, int nChannel, int nMode) {
    Voice* p = &lbl_802820E8[nVoice];
    int bEnabled = OSDisableInterrupts();
    u32* pCoefs;
    u16 nHeader;
    // port: the coefficients sit at a 2-byte boundary and EA copies them as words, which the
    // GameCube allows; a port should copy the 32 bytes with memcpy.
    if (nChannel == 0) {
        pCoefs = (u32*)pBlock->a1A;
        nHeader = pBlock->aDataL[0];
    } else {
        pCoefs = (u32*)pBlock->a3C;
        nHeader = pBlock->aDataR[0];
    }
    p->a18[0] = pCoefs[0];
    p->a18[1] = pCoefs[1];
    p->a18[2] = pCoefs[2];
    p->a18[3] = pCoefs[3];
    p->a18[4] = pCoefs[4];
    p->a18[5] = pCoefs[5];
    p->a18[6] = pCoefs[6];
    p->a18[7] = pCoefs[7];
    if (!p->flags.b.b6_10) {
        p->flags.b.bSetAdpcm = 1;
    }
    p->n3A = nHeader;
    if (nMode == 0) {
        p->n50 = nHeader;
        p->flags.b.bSetLoop = 1;
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

// The status of card slot nSlot's entry n, for the status table (fn_800B10A4): 0 while flag 0x02
// is clear, then 7, 6 (a sector size other than 0x2000), 8, 9 or 1 from the card's flags, 10 when
// fn_8009EE28 fails with -18, 3 when the card's two counts cover what the save needs (save kinds 0
// and 3, and fn_8009D3DC + fn_8009D50C), and 2 when not.
s32 fn_800B09C8(int nSlot, int n) {
    CardInfo info;
    CardPos pos;
    s32 nStatus = 0;
    s32 nNeed;
    s32 nNeed3;
    s32 nNeed2;
    lbl_802814A0 = 1;
    fn_8009F7F4(&info, nSlot, n);
    if (info.uFlags & 0x2) {
        if (info.uFlags & 0x10) {
            nStatus = 7;
        } else if (info.n88 != 0x2000) {
            nStatus = 6;
        } else if (info.uFlags & 0x20) {
            nStatus = 8;
        } else if (info.uFlags & 0x40) {
            nStatus = 9;
        } else if (info.uFlags & 0x8) {
            if (info.uFlags & 0x80) {
                nStatus = 1;
            } else {
                pos.nSlot = nSlot;
                pos.n = n;
                fn_80084FF0(0);
                nStatus = 2;
                nNeed = fn_80084FB4(&pos);
                fn_80084FF0(3);
                nNeed3 = fn_80084FB4(&pos);
                fn_80084FF0(0);
                nNeed += nNeed3;
                nNeed2 = fn_8009D50C(nSlot, n) + fn_8009D3DC(nSlot, n);
                if (fn_8009EE28(nSlot, n) == -18) {
                    nStatus = 10;
                } else if (info.n4 >= nNeed && info.n84 >= nNeed2) {
                    nStatus = 3;
                }
            }
        } else {
            nStatus = 1;
        }
    }
    return nStatus;
}

// Build the memory-card status table from scratch, every status reported, and send the message
// for the first status that has one; message 0x80 when a status is 3 or out of range first, 0x81
// when no status sends one.
void fn_800B0B1C(void) {
    int i;
    int j;
    int n;
    u8 bFound;
    fn_8009CD10();
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        lbl_80282140[i][0] = 0;
        if (fn_8009F7E8(i)) {
            lbl_80282138[i] = n = 1;
        } else {
            lbl_80282138[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            lbl_80282150[i][j] = 0;
        }
        for (j = 0; j < n; j++) {
            lbl_80282150[i][j] = fn_800B09C8(i, j);
            lbl_80282148[i][j] = lbl_80282150[i][j];
        }
    }
    bFound = 0;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        for (j = 0; j < 1; j++) {
            switch (lbl_80282150[i][j]) {
            case 0:
                // slot 1's status 0 is reported only when slot 0's is not 0
                if (i == 1 && lbl_80282150[0][0] != 0) {
                    lbl_80281498 = j;
                    lbl_8028149C = i;
                    fn_800B0F94();
                    fn_8009CD7C();
                    return;
                }
                break;
            case 1:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0F0C();
                fn_8009CD7C();
                return;
            case 10:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B1060();
                fn_8009CD7C();
                return;
            case 6:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0DB8();
                fn_8009CD7C();
                return;
            case 7:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0DFC();
                fn_8009CD7C();
                return;
            case 8:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0E40();
                fn_8009CD7C();
                return;
            case 9:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0E84();
                fn_8009CD7C();
                return;
            case 11:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B0EC8();
                fn_8009CD7C();
                return;
            case 2:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_800B101C();
                fn_8009CD7C();
                return;
            case 3:
            case 4:
            case 5:
            default:
                bFound = 1;
                // fake match: leaves both loops in one jump (a flag test after each loop adds code)
                goto done;
            }
        }
    }
done:
    fn_8009CD7C();
    lbl_80281498 = j;
    lbl_8028149C = i;
    if (bFound) {
        fn_800B0FD8();
    } else {
        lbl_80281498 = -1;
        lbl_8028149C = -1;
        fn_800B0F50();
    }
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

// Read every card slot's status; a changed status is marked not yet reported. Here and in
// fn_800B1180 and fn_800B13FC EA tests the slot (fn_8009F7E8) but gives it one entry either way.
void fn_800B10A4(void) {
    int j;
    int i;
    s32 n;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        if (fn_8009F7E8(i)) {
            n = 1;
        } else {
            n = 1;
        }
        if (lbl_80282138[i] != n) {
            lbl_80282138[i] = n;
            lbl_80282150[i][0] = 0;
            lbl_80282148[i][0] = 0;
            lbl_80282140[i][0] = 0;
        }
        for (j = 0; j < n; j++) {
            lbl_80282150[i][j] = fn_800B09C8(i, j);
            if (lbl_80282148[i][j] != lbl_80282150[i][j]) {
                lbl_80282148[i][j] = lbl_80282150[i][j];
                lbl_80282140[i][j] = 0;
            }
        }
    }
}

// Whether every status is 0.
u8 fn_800B1180(void) {
    int i;
    int j;
    s32 n;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        if (fn_8009F7E8(i)) {
            n = 1;
        } else {
            n = 1;
        }
        for (j = 0; j < n; j++) {
            if (lbl_80282150[i][j] != 0) return 0;
        }
    }
    return 1;
}

// Report the entry the reports reached again, re-reading the statuses first; 5 when every status
// is 0.
s32 fn_800B120C(s32* pnSlot, s32* pn) {
    fn_800B10A4();
    if (fn_800B1180()) return 5;
    if (lbl_8028149C == -1 || lbl_80281498 == -1) {
        return fn_800B12FC(pnSlot, pn);
    }
    lbl_80282140[lbl_8028149C][lbl_80281498] = 1;
    lbl_80282148[lbl_8028149C][lbl_80281498] = lbl_80282150[lbl_8028149C][lbl_80281498];
    *pnSlot = lbl_8028149C;
    *pn = lbl_80281498;
    // EA bug: the slot is used for both indexes; for slot 1 this reads past the table (the word
    // after it, lbl_80282158).
    if (lbl_80282150[lbl_8028149C][lbl_8028149C] == 0 && fn_800B1180()) return 5;
    return lbl_80282150[lbl_8028149C][lbl_80281498];
}

// Report the first status not yet reported (4 when there is none, 5 when every status is 0).
s32 fn_800B12FC(s32* pnSlot, s32* pn) {
    s32 n;
    int i;
    int j;
    fn_800B10A4();
    if (fn_800B1180()) return 5;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        n = lbl_80282138[i];
        for (j = 0; j < n; j++) {
            if (lbl_80282140[i][j] == 0) {
                lbl_8028149C = i;
                lbl_80281498 = j;
                lbl_80282140[i][j] = 1;
                lbl_80282148[i][j] = lbl_80282150[i][j];
                *pnSlot = i;
                *pn = j;
                if (n > 1) {
                    (*pn)++;
                }
                return lbl_80282150[i][j];
            }
        }
    }
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    return 4;
}

// Find the next slot and entry after the last one found that has a status; returns whether there
// is one.
int fn_800B13FC(s32* pnSlot, s32* pn) {
    int i;
    int j;
    s32 n;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        if (fn_8009F7E8(i)) {
            lbl_80282138[i] = n = 1;
        } else {
            lbl_80282138[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            if (lbl_80282150[i][j] != 0 &&
                (i > lbl_8028149C || (i == lbl_8028149C && j > lbl_80281498))) {
                *pnSlot = i;
                *pn = j;
                if (n == 1) {
                    (*pn)++;
                }
                lbl_8028149C = i;
                lbl_80281498 = j;
                return 1;
            }
        }
    }
    return 0;
}

// Start a search from the beginning (fn_800B13FC continues it).
int fn_800B14E4(s32* pnSlot, s32* pn) {
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    return fn_800B13FC(pnSlot, pn);
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

// fn_800B0B1C without the messages: build the status table from scratch, then look for the first
// status that is 3 or out of range and, when there is one, load from the card (fn_8009FAD0).
void fn_800B1748(void) {
    int i;
    int j;
    int n;
    u8 bFound;
    fn_8009CD10();
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        lbl_80282140[i][0] = 0;
        if (fn_8009F7E8(i)) {
            lbl_80282138[i] = n = 1;
        } else {
            lbl_80282138[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            lbl_80282150[i][j] = 0;
        }
        for (j = 0; j < n; j++) {
            lbl_80282150[i][j] = fn_800B09C8(i, j);
            lbl_80282148[i][j] = lbl_80282150[i][j];
        }
    }
    bFound = 0;
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        for (j = 0; j < 1; j++) {
            switch (lbl_80282150[i][j]) {
            case 0:
            case 1:
            case 2:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                lbl_80281498 = j;
                lbl_8028149C = i;
                fn_80110458(0);
                fn_8009CD7C();
                break;
            case 3:     // fake match: listed although default covers it; it sets the compare order
            default:
                bFound = 1;
                // fake match: leaves both loops in one jump (a flag test after each loop adds code)
                goto done;
            }
        }
    }
done:
    fn_8009CD7C();
    lbl_80281498 = j;
    lbl_8028149C = i;
    if (bFound) {
        fn_8009CD10();
        if (fn_8009FAD0() == 0) {
            fn_80110458(1);
        } else {
            fn_80110458(0);
        }
        fn_8009CD7C();
    } else {
        lbl_80281498 = -1;
        lbl_8028149C = -1;
    }
}

// Estimate the length of the vector (v[0], v[1]) without a square root, from its longer side a
// and its shorter side b.
f32 fn_800B1960(f32* v) {
    f32 a = v[0];
    f32 b = v[1];
    f32 s;
    f32 d;
    if (a < 0.0f) {
        a = -a;
    }
    if (b < 0.0f) {
        b = -b;
    }
    if (b > a) {
        fn_800B1A88(&a, &b);
    }
    if (b > 0.5f * a) {
        s = a + b;
        d = a - b;
        a = (32.0f * b + (a + (2.0f * s + (64.0f * s + 8.0f * s)))) / 128.0f;
        b = d;
    }
    return a + 0.25f * b - 0.0078125f * b;
}
