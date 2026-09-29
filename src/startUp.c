// startUp.c (EA's name, from its asserts; in Golf\Entry\ in EA's 2002 source tree, in
// Golf/UI Core/Startup/ in TW2005's): the boot-time systems.
// - The sound's GameCube layer, under hlaudvoice.c's voices: the hardware voices (HwVoice_:
//   gpHwVoices, NUM_VOICES wrappers around the AX library's voices, run from the mixer callback
//   HwVoice_MixerCallback), the DMA into audio RAM (AudDma_), the sound's ARAM heap with its eight
//   stream buffers (AudAram_) and the two built-in sounds (BootSound_). TW06 and TW07 have no
//   counterpart: it is platform code.
// - The boot-time memory-card checks (Startup_: a status per card, and the hint each sends the
//   front end) and the start-up message handlers (gStartupMessageHandlers).
// - The 'LEGL' stream: two legal-screen pictures (uiProcessPolygon.c shows the first at boot).
//   TW07's startUp.c has only two functions, startup_RegisterStreamClients and
//   startup_UnregisterStreamClients (empty there).
// - A length estimate without a square root (for AudTable.c) and the ball-against-object test (for
//   Ball.c).
// The sound code talks to the GameCube's audio libraries; see core/startup.h.

#include "core/startup.h"
#include "core/goaram.h"
#include "core/memcard.h"
#include "core/gameaudio.h"
#include "game/frontend.h"
#include "frontend/uistudio.h"
#include "frontend/fe.h"
#include "ball.h"
#include "dynobj.h"

void   HwVoice_MixerCallback(void);
void   HwVoice_DroppedCallback(void* pVpb);
u8     HwVoice_Acquire(s16 nVoice);
void   HwVoice_Reset(s16 nVoice);
void   HwVoice_StartOrRelease(u16 nVoice, u8 bOn);
void   HwVoice_SetSound(u16 nVoice, SoundHeader* pHdr);
void   HwVoice_SetVolume(u16 nVoice, s16 nVolume, int a, int b);
s16    HwVoice_VolumeToDb(s16 nVolume);
void   HwVoice_SetRate(u16 nVoice, u32 u, int a);
void   HwVoice_SetEnvelope(u16 nVoice, VoiceEnvelope* pEnv);
void   HwVoice_SetReverb(u16 nVoice, u8 bA, u8 bB);
void   AudAram_ZeroBlockDone(u32 n);
s32    Startup_ReadCardStatus(int nPort, int nSlot);
void   Startup_HintCardWrongSectorSize(void);
void   Startup_HintCardNotMemoryCard(void);
void   Startup_HintCardIoError(void);
void   Startup_HintCardBroken(void);
void   Startup_HintCardStatus11(void);
void   Startup_HintCardUnformatted(void);
void   Startup_SendNoCardsMsg(void);
void   Startup_SendSlotBEmptyMsg(void);
void   Startup_SendCardReadyMsg(void);
void   Startup_SendCardFullMsg(void);
void   Startup_SendSaveDamagedMsg(void);
void   Startup_UpdateCardStatuses(void);
u8     Startup_AreAllSlotsEmpty(void);
s32    Startup_GetNextCardStatus(s32* pnPort, s32* pnSlot);
int    Startup_FindNextCardWithStatus(s32* pnPort, s32* pnSlot);
void   Startup_LoadLegalPicture(UStreamObject* pObject);
void   Startup_Vec3Add(f32* pA, f32* pB, f32* pOut);
void   Startup_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void   GM_vStartupGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupCheckCards(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFindFirstCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFindNextCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFadeToBlack(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupLoadFromCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupSkipCardLoad(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFormatCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetNextCardStatus(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetCurrentCardStatus(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupPlaySound(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFormatHadIOError(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage11_Return0(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage12_Empty(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage13_Empty(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage14_Return1(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetFilesNeeded(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupEndGameLoop(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupDeleteSaveGame(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupLoadOptionsCheckDisc(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupChangeDisc(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetDiscChangeStatus(MsgArg* pArgs, MsgArg* pResult);

// port: the GameCube's audio and ARAM libraries and their set-up.
void   AIInit(u8* pStack);
void   ARQInit(void);
void   AXInit(void);
void   MIXInit(void);
void   AXSetMode(u32 uMode);
u32    OSGetSoundMode(void);
void   MIXSetSoundMode(u32 uMode);      // the output mode (mono, stereo, surround)
void   AXRegisterCallback(void (*pfn)(void));  // the callback run after each audio frame
void*  fn_800B5BD8(u32 uSize);
void   AXSetVoiceSrcRatio(AXVPB* pVpb, f32 fRatio);     // the playback rate
void   AXSetVoiceAdpcm(AXVPB* pVpb, u32* pCoefs);
void   AXSetVoiceAdpcmLoop(AXVPB* pVpb, u16* pLoop);
void   AXSetVoiceType(AXVPB* pVpb, u32 uType);
void   MIXSetInput(AXVPB* pVpb, int nDb);
void   MIXSetAuxA(AXVPB* pVpb, int nDb);
void   MIXSetPan(AXVPB* pVpb, int nPan);
void   MIXSetSPan(AXVPB* pVpb, int nSPan);
void   MIXMute(AXVPB* pVpb);
void   MIXUnMute(AXVPB* pVpb);
void   MIXUpdateSettings(void);         // MIX: pass the settings to the hardware
void   audfrac_Swap(f32* pV1, f32* pV2);
void   audvec2_Set(f32* dest, f32 x, f32 y);

// The save kinds (a table of functions at lbl_8018C7D8).
s32    MC_CallActionFnMemoryRequired(CardPos* pPos);      // the picked save kind's size on that card

// The sounds' ADPCM data is game data: tools/build/gendata.py writes the initializers from main.dol.
u8 lbl_8018F040[0x600] = {
#include "startUp_sound0.inc"
};
u8 lbl_8018F640[0x858] = {
#include "startUp_sound1.inc"
};

// The two built-in sounds: where their data is, its size, its playback rate (16.16 fixed point:
// 0.5 and 0.25) and its header, with the addresses counted from the data's start
// (BootSound_CopyToAram rebases them into ARAM).
BootSound gBootSounds[2] = {
    {lbl_8018F040, 0x600, 0, 0x8000,
     {2, 0xBC9, 0x8002, 0,
      {0x024302E8, 0x0C3BF9E6, 0x0807FE3A, 0x0E9DF887, 0x00420786, 0x0CC7FA46, 0x0A2FFD3B, 0x0E53F921},
      0x40, 0, 0, 0}},
    {lbl_8018F640, 0x860, 0, 0x4000,
     {2, 0x10AA, 0x8002, 0,
      {0x0E77F80C, 0x0FAEF80D, 0x0F44F808, 0x0FD7F818, 0x0EABF858, 0x0FC7F813, 0x0F7FF80C, 0x0FDDF81B},
      0x37, 0, 0, 0}},
};

s32 gStartupCardSlot = -1;      // } the slot and port the card checks and reports reached; -1 to
s32 gStartupCardPort = -1;      // } start again
u8  gbStartupCardLoad = 1;      // the boot-time load goes ahead: set by Startup_ReadCardStatus,
                                // cleared by Startup_SkipCardLoad, tested by Startup_LoadFromCard

// The rest is defined last-address-first: the compiler lays an object's uninitialised data out in
// reverse.

// The memory-card status table: for each port, gStartupCardSlotsPerPort[port] slots (always 1),
// each with the status Startup_ReadCardStatus read (gStartupCardStatus), the status last reported
// (gStartupCardReportedStatus) and whether it has been reported (gbStartupCardReported).
s32    gStartupCardStatus[MC_NUM_PORTS][MC_NUM_SLOTS];
s32    gStartupCardReportedStatus[MC_NUM_PORTS][MC_NUM_SLOTS];
s32    gbStartupCardReported[MC_NUM_PORTS][MC_NUM_SLOTS];
s32    gStartupCardSlotsPerPort[MC_NUM_PORTS];

void*  gpLegalPicture;          // the first 'LEGL' object's copy (Startup_LoadLegalPicture keeps two)
void*  gpLegalPicture2;         // the second one's (nothing reads it)
u32    gLegalPictureSize;       // the first one's size
u32    gLegalPicture2Size;      // the second one's
s32    gnLegalPictures;         // how many it has kept, 0..2
u8     gbStartupCardUserLoaded; // MC_LoadInitialUser's answer (Startup_LoadFromCard); never read
// fake match: lbl_8028211C, lbl_80282114 and lbl_802820EC are never used by the game's code, but
// the original's data has a word at each of these addresses (likely the globals of functions the
// linker stripped); kept through the dead-stripping so the rest lines up. Types unknown.
KEEP_UNUSED u32 lbl_8028211C;
u16    gBootSoundVoice;         // the voice BootSound_Play uses next
KEEP_UNUSED u32 lbl_80282114;

// The sound's ARAM heap (AudAram_InitModule).
u8     gbAudAramZeroDone;       // set when the silent block's DMA is done
void*  gpAudAramHeapRecords;    // the heap's bookkeeping: its ARAMHeap and 32 ARAMBlock records
u32    gAudAramStreamBuffers;   // the ARAM address of the eight 0xFE00-byte stream buffers
void*  gpAudAramZeroBuffer;     // the zeroes DMA'd into the silent block, freed once it is done
u32    gAudAramZeroBlock;       // the silent block's ARAM address
u32    gAudAramBase;            // the heap's ARAM address
ARAMHeap* gpAudAramHeap;        // the heap
s32    gnAudAramStreamBuffersUsed;  // how many of the eight stream buffers are taken
u32    gAudAramStreamBufferMask;    // which of them are taken (bit n: buffer n)
KEEP_UNUSED u32 lbl_802820EC;

Voice* gpHwVoices;              // the voices, NUM_VOICES of them (HwVoice_InitModule)

// The start-up message handlers (Startup_InitGameMessages fills 0..22, all but 4; the last 7 are never set).
MsgHandler gStartupMessageHandlers[30];

// The mixer callback (registered by HwVoice_InitModule), run after every audio frame. For each
// voice: ask for a lost hardware voice back after its wait; stop and reset a playing one whose
// position is more than 0x40000 nibbles before its end address (or past it: the difference is
// unsigned) while above nibble 0x8800; pass changed settings on to the hardware; reset one whose
// hardware voice has stopped (states 3, 4 and 6); start, release, pause and resume it; step its
// volume envelope.
void HwVoice_MixerCallback(void) {
    s16 i;
    Voice* p;
    u8 bMoved;
    u16 nOld;
    s32 nLevel;
    u8 bStage;
    AXVPB* pVpb;
    u32* pDst;
    u32* pSrc;
    p = gpHwVoices;
    for (i = 0; i < NUM_VOICES; i++, p++) {
        bMoved = 0;
        if (p->flags.b.bLost) {
            p->flags.b.n6_7F8--;
            if (p->flags.b.n6_7F8 != 0) continue;
            if (HwVoice_Acquire(i)) {
                HwVoice_Reset(i);
                p->flags.b.bLost = 0;
            } else {
                p->flags.b.n6_7F8 = 0xFF;
            }
        }
        // port: both reads are a u32 at a 2-byte boundary (see HwVoice_GetPlayPos).
        pVpb = p->pVpb;
        if (pVpb->n146 != 0 && *(u32*)&pVpb->n1AE - *(u32*)&pVpb->n1B2 > 0x40000 &&
            *(u32*)&pVpb->n1B2 > 0x8800) {
            AXSetVoiceState(pVpb, 0);
            HwVoice_Reset(i);
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
            AXSetVoiceSrcRatio(p->pVpb, (1.0f / 65536.0f) * p->u40);
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
            HwVoice_Reset(i);
        }
        if (p->flags.b.bStart) {
            if (!bMoved && p->u40 != 0) {
                pVpb = p->pVpb;
                if (p->flags.b.b6_40 && !p->flags.b.b6_10) {
                    HwVoice_Reset(i);
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
    MIXUpdateSettings();
}

// The hardware dropped a voice (to play one of higher priority): mark ours lost and stop it. The
// mixer callback asks for it back after 255 passes.
void HwVoice_DroppedCallback(void* pVpb) {
    u16 i;
    Voice* p;
    for (i = 0; i < NUM_VOICES; i++) {
        p = &gpHwVoices[i];
        if (p->pVpb == pVpb) {
            p->flags.b.bLost = 1;
            p->flags.b.n6_7F8 = 0xFF;
            AXSetVoiceState(p->pVpb, 0);
            return;
        }
    }
}

// Take a hardware voice for voice nVoice; returns whether one was free.
u8 HwVoice_Acquire(s16 nVoice) {
    AXVPB* pVpb = AXAcquireVoice(1, HwVoice_DroppedCallback, nVoice);
    Voice* p = &gpHwVoices[nVoice];
    if (pVpb) {
        p->pVpb = pVpb;
        AXSetVoiceSrcType(pVpb, 1);
    }
    return pVpb != NULL;
}

// Reset a voice: silent, centred, all flags clear.
void HwVoice_Reset(s16 nVoice) {
    Voice* p = &gpHwVoices[nVoice];
    p->flags.u = 0;
    p->n56 = 0;
    p->n58 = VOLUME_MIN;
    p->nAuxA = VOLUME_MIN;
    p->u40 = 0;
    p->nPan = 64;
    p->nSPan = 127;
    MIXInitChannel(p->pVpb, 0, VOLUME_MIN, VOLUME_MIN, VOLUME_MIN, 64, 127, 0);
}

// Aud_InitOnce's first step: start the audio libraries (AI, ARQ, AX, MIX; the mixer's sound mode
// from the console's setting) and the voice table, NUM_VOICES voices, each given a hardware voice
// and reset, then register HwVoice_MixerCallback. Returns 1.
u8 HwVoice_InitModule(void) {
    s16 i;
    AIInit(NULL);
    ARQInit();
    AXInit();
    MIXInit();
    AXSetMode(0);
    MIXSetSoundMode(OSGetSoundMode());
    gpHwVoices = fn_800B5BD8(NUM_VOICES * sizeof(Voice));
    Mem_set(gpHwVoices, 0, NUM_VOICES * sizeof(Voice));
    for (i = 0; i < NUM_VOICES; i++) {
        HwVoice_Acquire(i);
        HwVoice_Reset(i);
    }
    AXRegisterCallback(HwVoice_MixerCallback);
    return 1;
}

// Ses_Init's step for the hardware voices: nothing to set up; returns 1.
u8 HwVoice_InitSession(void) {
    return 1;
}

// The sound session's exit step (hlaudmovie.c) for the hardware voices: release every voice.
void HwVoice_ExitSession(void) {
    u16 i;
    for (i = 0; i < NUM_VOICES; i++) {
        HwVoice_StartOrRelease(i, 0);
    }
}

// Whether a voice is free: idle, and not waiting to get its hardware voice back.
u8 HwVoice_IsFree(u16 nVoice) {
    Voice* p = &gpHwVoices[nVoice];
    return (p->flags.b.nState == 0 || p->flags.b.nState == 1) && !p->flags.b.bLost;
}

// Start a set-up voice (bOn), or release a playing one into its envelope's release.
void HwVoice_StartOrRelease(u16 nVoice, u8 bOn) {
    Voice* p = &gpHwVoices[nVoice];
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
void HwVoice_Pause(u16 nVoice, u8 bPause) {
    Voice* p = &gpHwVoices[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (p->flags.b.nState >= 2) {
        if (bPause) {
            if (p->flags.b.nState == 6) {
                HwVoice_SetVolume(nVoice, 0, 0, 0);
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
u32 HwVoice_GetPlayPos(u16 nVoice) {
    Voice* p = &gpHwVoices[nVoice];
    if (p->flags.b.nState <= 2) {
        return p->u14 >> 1;
    }
    // fake match: one 32-bit load across both halves (combining them in C loads each alone).
    // port: this reads a u32 at a 2-byte boundary and assumes big-endian; a port should use
    //       ((u32)n1B2 << 16 | n1B4).
    return *(u32*)&p->pVpb->n1B2 >> 1;
}

// Set a voice up to play the sound pHdr describes (state 1, for HwVoice_StartOrRelease to start):
// its ARAM addresses and loop settings and, for a sound whose start is at nibble 0x106800 (ARAM
// byte 0x83400) or above, its ADPCM coefficients and first frame header. One that starts below is
// flagged b6_40 instead: it plays as AX voice type 1, and the mixer resets it rather than start it
// until HwVoice_SetStreamDecoder or HwVoice_SetMovieDecoder has passed its coefficients on.
void HwVoice_SetSound(u16 nVoice, SoundHeader* pHdr) {
    Voice* p = &gpHwVoices[nVoice];
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

// A voice's volume as last set (0..0x3FFF, HwVoice_SetVolume).
s16 HwVoice_GetVolume(u16 nVoice) {
    return gpHwVoices[nVoice].n56;
}

// Set a voice's volume, clamped to 0..0x3FFF; its dB value (HwVoice_VolumeToDb) goes to the mixer
// on the next pass. a and b are not used (callers pass 0, and hlaudvoice.c Voc_Render its
// bPlaying).
void HwVoice_SetVolume(u16 nVoice, s16 nVolume, int a, int b) {
    Voice* p = &gpHwVoices[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (nVolume < 0) {
        nVolume = 0;
    } else if (nVolume > 0x3FFF) {
        nVolume = 0x3FFF;
    }
    if (p->n56 != nVolume) {
        p->n56 = nVolume;
        p->n58 = HwVoice_VolumeToDb(nVolume);
        p->flags.b.bSetInput = 1;
    }
    OSRestoreInterrupts(bEnabled);
}

// A volume (0..0x3FFF) in dB x 10, for the mixer: each halving takes 6 dB off, down to VOLUME_MIN.
s16 HwVoice_VolumeToDb(s16 nVolume) {
    s16 nDb;
    f32 fDb;
    nVolume >>= 1;
    if (nVolume <= 0) return VOLUME_MIN;
    if (nVolume >= 0x3FFF) return 0;
    fDb = 16383.0f;
    fDb /= nVolume;
    fDb = logf(fDb);
    fDb *= -86.5617f;
    nDb = fDb;
    if (nDb < VOLUME_MIN) {
        nDb = VOLUME_MIN;
    }
    return nDb;
}

// Set a voice's pan (mode 2; hlaudvoice.c also sends mode 3, which does nothing here). bPlaying is
// not used: the caller passes it to every voice setter.
void HwVoice_SetPan(u16 nVoice, u8 nPan, int nMode, int bPlaying) {
    Voice* p = &gpHwVoices[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (nMode == 2) {
        p->flags.b.bSetPan = 1;
        p->nPan = nPan;
    }
    OSRestoreInterrupts(bEnabled);
}

// Set a voice's playback rate, 16.16 fixed point (0x10000 plays the sound at its own rate); 0 or
// the rate it has already is ignored. a is not used (hlaudvoice.c Voc_Render passes its bPlaying).
void HwVoice_SetRate(u16 nVoice, u32 u, int a) {
    Voice* p = &gpHwVoices[nVoice];
    int bEnabled = OSDisableInterrupts();
    if (p->u40 != u && u != 0) {
        p->u40 = u;
        p->flags.b.bSetSrc = 1;
    }
    OSRestoreInterrupts(bEnabled);
}

// Set a voice's envelope; a zero attack, decay or release becomes the fastest.
void HwVoice_SetEnvelope(u16 nVoice, VoiceEnvelope* pEnv) {
    Voice* p = &gpHwVoices[nVoice];
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

// Turn a voice's reverb send (aux A) on at -15 dB when either flag is set, else off. The callers
// pass one flag twice.
void HwVoice_SetReverb(u16 nVoice, u8 bA, u8 bB) {
    Voice* p = &gpHwVoices[nVoice];
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
void HwVoice_SetStreamDecoder(u16 nVoice, StreamChunk* pChunk, u32 uSize, int nBuffer) {
    Voice* p = &gpHwVoices[nVoice];
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
void HwVoice_SetMovieDecoder(u16 nVoice, MovieSoundBlock* pBlock, int nChannel, int nMode) {
    Voice* p = &gpHwVoices[nVoice];
    int bEnabled = OSDisableInterrupts();
    u32* pCoefs;
    u16 nHeader;
    // port: the coefficients sit at a 2-byte boundary and EA copies them as words, which the
    //       GameCube allows; a port should copy the 32 bytes with memcpy.
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

// Called by Voc_Cycle for each voice it frees (with the hardware voice and a flag, which this build
// does not take); empty.
void HwVoice_OnVoiceFreed(void) {
}

// Once a frame, from Aud_EmiCycle after Voc_Cycle; empty in this build.
void HwVoice_Cycle(void) {
}

// Aud_InitOnce's step between the voices and the ARAM heap, next to the DMA functions: nothing to
// set up in this build; returns 1.
u8 AudDma_InitModule(void) {
    return 1;
}

// Ses_Init's step between the voices and the ARAM heap: nothing to set up in this build; returns 1.
u8 AudDma_InitSession(void) {
    return 1;
}

// The sound session exit's step between the ARAM heap and the voices; empty in this build.
void AudDma_ExitSession(void) {
}

// DMA nLen bytes at pSrc into ARAM at uAram, the CPU cache written back first; pfnDone(n) is called
// when it is done (NULL: no call), and n is also the transfer's owner for AudDma_CancelOwner.
// Returns 1.
int AudDma_ToAram(u32 uAram, void* pSrc, int nLen, void (*pfnDone)(u32 n), int n) {
    AudDma_CacheBeforeTransfer(pSrc, nLen, 0);
    GoARAM_QueueTransfer((u32)pSrc, uAram, nLen, 0, 1, pfnDone, n, 3);  // port: the ARQ library takes addresses as u32
    AudDma_CacheAfterTransfer(pSrc, nLen, 0);
    return 1;
}

// Cancel the queued ARAM transfers whose owner is pOwner (AudDma_ToAram's n).
void AudDma_CancelOwner(void* pOwner) {
    GoARAM_CancelTransfers((u32)pOwner);  // port: the ARQ library keeps owners as u32
}

// After a DMA between main memory and ARAM: when the data came into main memory (nDir 1), drop
// the CPU cache over it so the CPU reads the new bytes.
void AudDma_CacheAfterTransfer(void* p, u32 uLen, int nDir) {
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
void AudDma_CacheBeforeTransfer(void* p, u32 uLen, int nDir) {
    switch (nDir) {
    case 0:
        DCStoreRange(p, uLen);
        break;
    case 1:
        DCInvalidateRange(p, uLen);
        break;
    }
}

// AudAram_InitModule's DMA callback: the silent block is filled, its zero buffer can go.
void AudAram_ZeroBlockDone(u32 n) {
    gbAudAramZeroDone = 1;
}

// Aud_InitOnce's step for the sound's ARAM: take ARAM_HEAP_SIZE bytes for its heap, put a silent
// block of ARAM_ZERO_SIZE bytes at the start (DMA'd from a zeroed buffer; AudAram_ZeroBlockDone),
// then the eight 0xFE00-byte stream buffers (AudAram_AllocStreamBuffer). Returns 1.
u8 AudAram_InitModule(void) {
    gAudAramBase = GoARAM_Alloc(ARAM_HEAP_SIZE);
    gpAudAramHeapRecords = fn_800B5BD8(sizeof(ARAMHeap) + 32 * sizeof(ARAMBlock));
    gpAudAramHeap = GoARAM_HeapInit(ARAM_HEAP_SIZE, gAudAramBase, 32, gpAudAramHeapRecords);
    gAudAramZeroBlock = GoARAM_HeapAlloc(gpAudAramHeap, ARAM_ZERO_SIZE, 32);
    gpAudAramZeroBuffer = fn_800951A0(ARAM_ZERO_SIZE, 32, 1);
    Mem_set(gpAudAramZeroBuffer, 0, ARAM_ZERO_SIZE);
    AudDma_ToAram(gAudAramZeroBlock, gpAudAramZeroBuffer, ARAM_ZERO_SIZE, AudAram_ZeroBlockDone, 0);
    gAudAramStreamBuffers = GoARAM_HeapAlloc(gpAudAramHeap, 0x7F000, 32);
    return 1;
}

// Ses_Init's step for the ARAM heap: free the silent block's zero buffer if its DMA is done.
// Returns 1.
u8 AudAram_InitSession(void) {
    if (gbAudAramZeroDone) {
        gbAudAramZeroDone = 0;
        fn_8009527C(gpAudAramZeroBuffer);
    }
    return 1;
}

// The sound session exit's step for the ARAM heap: the same as AudAram_InitSession.
void AudAram_ExitSession(void) {
    if (gbAudAramZeroDone) {
        gbAudAramZeroDone = 0;
        fn_8009527C(gpAudAramZeroBuffer);
    }
}

// Take uSize bytes (rounded up to 32) from the sound's ARAM heap, 32-byte aligned; returns the ARAM
// address.
u32 AudAram_Alloc(u32 uSize) {
    return GoARAM_HeapAlloc(gpAudAramHeap, (uSize + 31) & ~31, 32);
}

// Give an AudAram_Alloc block back.
void AudAram_Free(u32 uAddr) {
    GoARAM_HeapFree(gpAudAramHeap, uAddr);
}

// Take the first free one of the eight 0xFE00-byte stream buffers (Voc_Alloc, for a streamed
// voice); returns its ARAM address.
u32 AudAram_AllocStreamBuffer(void) {
    u32 uAddr;
    u32 uBit = 1;
    for (uAddr = gAudAramStreamBuffers; uAddr < gAudAramStreamBuffers + 0x7F000; uAddr += 0xFE00) {
        if (!(gAudAramStreamBufferMask & uBit)) {
            gAudAramStreamBufferMask |= uBit;
            gnAudAramStreamBuffersUsed++;
            return uAddr;
        }
        uBit <<= 1;
    }
    // EA bug: the original has no return here, so when all eight blocks are taken the caller
    // (which tests for 0) gets the loop's address, the end of the blocks. This keeps that value.
    return uAddr;
}

// Give a stream buffer (AudAram_AllocStreamBuffer) back.
// fake match: a per-function pragma (not EA's build setting): without propagation the shift keeps
// its own register and the flags load is numbered after the divide, as in EA's code (Gemini).
#pragma opt_propagation off
void AudAram_FreeStreamBuffer(u32 uAddr) {
    u32 mask;
    u32 uBit = 1;
    uAddr -= gAudAramStreamBuffers;
    mask = uBit << (uAddr / 0xFE00);
    gAudAramStreamBufferMask &= ~mask;
    gnAudAramStreamBuffersUsed--;
}
#pragma opt_propagation reset

// The ARAM address of the movie sound's buffers, a fixed 0x4400 (not from the heap): Mov_Init puts
// the left channel there and the right after it.
int AudAram_GetMovieBuffer(void) {
    return 0x4400;
}

// Aud_InitOnce's step next to the built-in sounds' code: nothing to set up in this build; returns 1
// (BootSound_CopyToAram copies the sounds once every step has succeeded).
u8 BootSound_InitModule(void) {
    return 1;
}

// Copy the two built-in sounds to ARAM and point their headers there.
void BootSound_CopyToAram(void) {
    u16 i;
    for (i = 0; i < 2; i++) {
        gBootSounds[i].uAram = AudAram_Alloc(gBootSounds[i].uSize);
        gBootSounds[i].hdr.u0 += gBootSounds[i].uAram * 2;
        gBootSounds[i].hdr.u4 += gBootSounds[i].uAram * 2;
        AudDma_ToAram(gBootSounds[i].uAram, gBootSounds[i].pData, gBootSounds[i].uSize, NULL, 0);
    }
}

// Play built-in sound nSound on the next voice in turn, at full volume with reverb.
void BootSound_Play(u8 nSound) {
    VoiceEnvelope env;
    env.nAttack = 0x400;
    env.nSustain = 0xF;
    env.nDecay = 0;
    env.nRelease = 0x200;
    HwVoice_SetSound(gBootSoundVoice, &gBootSounds[nSound].hdr);
    HwVoice_SetVolume(gBootSoundVoice, 0x3FFF, 0, 0);
    HwVoice_SetRate(gBootSoundVoice, gBootSounds[nSound].uC, 0);
    HwVoice_SetEnvelope(gBootSoundVoice, &env);
    HwVoice_StartOrRelease(gBootSoundVoice, 1);
    HwVoice_SetReverb(gBootSoundVoice, 1, 1);
    if (++gBootSoundVoice >= NUM_VOICES) {
        gBootSoundVoice = 0;
    }
}

// Start-up message 19's work: skip the boot-time load (Startup_LoadFromCard then only sends hint
// 0x86) until the next card check (Startup_ReadCardStatus) sets gbStartupCardLoad again.
void Startup_SkipCardLoad(void) {
    gbStartupCardLoad = 0;
}

// Start-up message 6's work, the boot-time load: when Startup_SkipCardLoad has cleared
// gbStartupCardLoad, only send hint 0x86 (the one MC_LoadOptionsFromFirstCardFound sends when it finds
// no save); else load the options from the first card with a good save, then the last user
// (MC_LoadInitialUser; its answer goes to gbStartupCardUserLoaded, which nothing reads).
void Startup_LoadFromCard(void) {
    MsgArg arg;
    if (!gbStartupCardLoad) {
        Mem_set(&arg, 0, sizeof(arg));
        UISDoHint(gpFrontEnd->pHandler, 0x86, 1, (s32*)&arg);
        return;
    }
    MC_Connect();
    MC_LoadOptionsFromFirstCardFound();
    gbStartupCardUserLoaded = MC_LoadInitialUser();
    MC_Disconnect();
}

// The status of the card in nPort, nSlot for the status table (gStartupCardStatus): 0 no card, 7 not a
// memory card, 6 a sector size other than 0x2000, 8 an I/O error, 9 broken, 1 unformatted or with
// an encoding error, 10 its save file is damaged (fn_8009EE28 answers MC_ERR_BADDATA), 3 when it
// has the free blocks and directory entries a save needs (the game's save and the EA Sports Bio:
// save kinds 0 and 3, and the new files fn_8009D3DC and fn_8009D50C count), else 2. Also sets
// gbStartupCardLoad (the boot-time load goes ahead).
s32 Startup_ReadCardStatus(int nPort, int nSlot) {
    MCCardState card;
    CardPos pos;
    s32 nStatus = 0;
    s32 nBlocks;
    s32 nBlocks3;
    s32 nFiles;
    gbStartupCardLoad = 1;
    MC_GetMC(&card, nPort, nSlot);
    if (card.uFlags & MC_CARD_PRESENT) {
        if (card.uFlags & MC_CARD_WRONGDEVICE) {
            nStatus = 7;
        } else if (card.nSectorSize != 0x2000) {
            nStatus = 6;
        } else if (card.uFlags & MC_CARD_IOERROR) {
            nStatus = 8;
        } else if (card.uFlags & MC_CARD_BROKEN) {
            nStatus = 9;
        } else if (card.uFlags & MC_CARD_FORMATTED) {
            if (card.uFlags & MC_CARD_ENCODING) {
                nStatus = 1;
            } else {
                pos.nPort = nPort;
                pos.nSlot = nSlot;
                MC_SetCurrentFileType(0);
                nStatus = 2;
                nBlocks = MC_CallActionFnMemoryRequired(&pos);
                MC_SetCurrentFileType(3);
                nBlocks3 = MC_CallActionFnMemoryRequired(&pos);
                MC_SetCurrentFileType(0);
                nBlocks += nBlocks3;
                nFiles = fn_8009D50C(nPort, nSlot) + fn_8009D3DC(nPort, nSlot);
                if (fn_8009EE28(nPort, nSlot) == MC_ERR_BADDATA) {
                    nStatus = 10;
                } else if (card.nFreeBlocks >= nBlocks && card.nFreeFiles >= nFiles) {
                    nStatus = 3;
                }
            }
        } else {
            nStatus = 1;
        }
    }
    return nStatus;
}

// Start-up message 1's work: check both cards from scratch, every status marked not yet reported
// (the multitap test gives a port one slot either way), and send the front end the hint for the
// first card that needs one, keeping its port and slot in gStartupCardPort and gStartupCardSlot: status 1
// hint 0x83, 2 0x82, 6 0x87, 7 0x88, 8 0x89, 9 0x8A, 10 and 11 0x8C. Status 0 is passed over (its
// hint 0x8B is never sent: see the EA bug below). Any other status (3: a card ready for the save)
// ends the search with hint 0x80; when no card stops it, hint 0x81, and the port and slot go back
// to -1.
void Startup_CheckCards(void) {
    int i;
    int j;
    int n;
    u8 bFound;
    MC_Connect();
    gStartupCardSlot = -1;
    gStartupCardPort = -1;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        gbStartupCardReported[i][0] = 0;
        if (MC_IsMultitapPluggedIn(i)) {
            gStartupCardSlotsPerPort[i] = n = 1;
        } else {
            gStartupCardSlotsPerPort[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            gStartupCardStatus[i][j] = 0;
        }
        for (j = 0; j < n; j++) {
            gStartupCardStatus[i][j] = Startup_ReadCardStatus(i, j);
            gStartupCardReportedStatus[i][j] = gStartupCardStatus[i][j];
        }
    }
    bFound = 0;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        for (j = 0; j < MC_NUM_SLOTS; j++) {
            switch (gStartupCardStatus[i][j]) {
            case 0:
                // port 1's status 0 is reported only when port 0's is not 0
                // EA bug: never true: the search gets to port 1 only when port 0's status is 0
                // (every other port-0 status returns or jumps out first), so hint 0x8B is never sent.
                if (i == 1 && gStartupCardStatus[0][0] != 0) {
                    gStartupCardSlot = j;
                    gStartupCardPort = i;
                    Startup_SendSlotBEmptyMsg();
                    MC_Disconnect();
                    return;
                }
                break;
            case 1:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardUnformatted();
                MC_Disconnect();
                return;
            case 10:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_SendSaveDamagedMsg();
                MC_Disconnect();
                return;
            case 6:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardWrongSectorSize();
                MC_Disconnect();
                return;
            case 7:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardNotMemoryCard();
                MC_Disconnect();
                return;
            case 8:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardIoError();
                MC_Disconnect();
                return;
            case 9:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardBroken();
                MC_Disconnect();
                return;
            case 11:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_HintCardStatus11();
                MC_Disconnect();
                return;
            case 2:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                Startup_SendCardFullMsg();
                MC_Disconnect();
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
    MC_Disconnect();
    gStartupCardSlot = j;
    gStartupCardPort = i;
    if (bFound) {
        Startup_SendCardReadyMsg();
    } else {
        gStartupCardSlot = -1;
        gStartupCardPort = -1;
        Startup_SendNoCardsMsg();
    }
}

// The messages below have no values: their one value is cleared and not counted.

// Hint 0x87 to the front end: the card's sector size is not 0x2000 (status 6).
void Startup_HintCardWrongSectorSize(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x87, 0, (s32*)&arg);
}

// Hint 0x88 to the front end: the device in the slot is not a memory card (status 7).
void Startup_HintCardNotMemoryCard(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x88, 0, (s32*)&arg);
}

// Hint 0x89 to the front end: the card has an I/O error (status 8).
void Startup_HintCardIoError(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x89, 0, (s32*)&arg);
}

// Hint 0x8A to the front end: the card is broken (status 9).
void Startup_HintCardBroken(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x8A, 0, (s32*)&arg);
}

// Hint 0x8C to the front end for status 11, the hint status 10 (a damaged save) also sends.
// Startup_ReadCardStatus never answers 11, so nothing reaches this in this build.
void Startup_HintCardStatus11(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x8C, 0, (s32*)&arg);
}

// Hint 0x83 to the front end: the card is unformatted or has an encoding error (status 1).
void Startup_HintCardUnformatted(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x83, 0, (s32*)&arg);
}

// The start-up card check's message 0x81 to the front end: no card in either slot (Startup_CheckCards
// sends it when every card status is 0).
void Startup_SendNoCardsMsg(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x81, 0, (s32*)&arg);
}

// The start-up card check's message 0x8B to the front end, meant for port 1 (slot B) empty while
// port 0 has a card. Never sent: Startup_CheckCards tests for it only after port 0's status was found to
// be 0 (every other status of port 0 returns or leaves the loops first).
void Startup_SendSlotBEmptyMsg(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x8B, 0, (s32*)&arg);
}

// The start-up card check's message 0x80 to the front end: a card is ready for the save
// (Startup_CheckCards: status 3, formatted with room for the game's save and the EA Sports Bio; also 4, 5
// or out of range, which Startup_ReadCardStatus never gives).
void Startup_SendCardReadyMsg(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x80, 0, (s32*)&arg);
}

// The start-up card check's message 0x82 to the front end: a formatted card without the free blocks
// or directory entries the game's save and the EA Sports Bio need (status 2).
void Startup_SendCardFullMsg(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x82, 0, (s32*)&arg);
}

// The start-up card check's message 0x8C to the front end: the card's save file is damaged (status
// 10: neither the save file nor its backup is a good save, MC_ERR_BADDATA).
// Startup_HintCardStatus11 sends the same message for status 11.
void Startup_SendSaveDamagedMsg(void) {
    MsgArg arg;
    Mem_set(&arg, 0, sizeof(arg));
    UISDoHint(gpFrontEnd->pHandler, 0x8C, 0, (s32*)&arg);
}

// Re-read every card's status (Startup_ReadCardStatus) into the status table; a status that changed is marked
// not yet reported, and a port whose slot count changed has its entries cleared first. Here and in
// Startup_AreAllSlotsEmpty and Startup_FindNextCardWithStatus EA tests for a multitap
// (MC_IsMultitapPluggedIn) but gives the port one slot either way.
void Startup_UpdateCardStatuses(void) {
    int j;
    int i;
    s32 n;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        if (MC_IsMultitapPluggedIn(i)) {
            n = 1;
        } else {
            n = 1;
        }
        if (gStartupCardSlotsPerPort[i] != n) {
            gStartupCardSlotsPerPort[i] = n;
            gStartupCardStatus[i][0] = 0;
            gStartupCardReportedStatus[i][0] = 0;
            gbStartupCardReported[i][0] = 0;
        }
        for (j = 0; j < n; j++) {
            gStartupCardStatus[i][j] = Startup_ReadCardStatus(i, j);
            if (gStartupCardReportedStatus[i][j] != gStartupCardStatus[i][j]) {
                gStartupCardReportedStatus[i][j] = gStartupCardStatus[i][j];
                gbStartupCardReported[i][j] = 0;
            }
        }
    }
}

// Whether every card status is 0: no card in any port.
u8 Startup_AreAllSlotsEmpty(void) {
    int i;
    int j;
    s32 n;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        if (MC_IsMultitapPluggedIn(i)) {
            n = 1;
        } else {
            n = 1;
        }
        for (j = 0; j < n; j++) {
            if (gStartupCardStatus[i][j] != 0) return 0;
        }
    }
    return 1;
}

// Re-read the statuses, then report again the card the reports reached (port gStartupCardPort, slot
// gStartupCardSlot): mark it reported, answer its port and slot (the slot from 0) and return its
// status. 5 when every status is 0; when no card has been reported yet, Startup_GetNextCardStatus's
// answer. Start-up command 9. (The EA bug in its last test is labelled where it happens.)
s32 Startup_GetCurrentCardStatus(s32* pnPort, s32* pnSlot) {
    Startup_UpdateCardStatuses();
    if (Startup_AreAllSlotsEmpty()) return 5;
    if (gStartupCardPort == -1 || gStartupCardSlot == -1) {
        return Startup_GetNextCardStatus(pnPort, pnSlot);
    }
    gbStartupCardReported[gStartupCardPort][gStartupCardSlot] = 1;
    gStartupCardReportedStatus[gStartupCardPort][gStartupCardSlot] =
        gStartupCardStatus[gStartupCardPort][gStartupCardSlot];
    *pnPort = gStartupCardPort;
    *pnSlot = gStartupCardSlot;
    // EA bug: the port is used for both indexes; for port 1 this reads past the table (the word
    // after it, lbl_80282158).
    if (gStartupCardStatus[gStartupCardPort][gStartupCardPort] == 0 && Startup_AreAllSlotsEmpty()) return 5;
    return gStartupCardStatus[gStartupCardPort][gStartupCardSlot];
}

// Re-read the statuses, then report the first card not yet reported: mark it reported, note it as
// the card the reports reached, answer its port and slot (the slot from 0; from 1 only on a port
// with more than one slot, which never happens here) and return its status
// (Startup_ReadCardStatus's numbers). 4 when every card has been reported (the reports start again
// from the top), 5 when every status is 0. Start-up command 8.
s32 Startup_GetNextCardStatus(s32* pnPort, s32* pnSlot) {
    s32 n;
    int i;
    int j;
    Startup_UpdateCardStatuses();
    if (Startup_AreAllSlotsEmpty()) return 5;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        n = gStartupCardSlotsPerPort[i];
        for (j = 0; j < n; j++) {
            if (gbStartupCardReported[i][j] == 0) {
                gStartupCardPort = i;
                gStartupCardSlot = j;
                gbStartupCardReported[i][j] = 1;
                gStartupCardReportedStatus[i][j] = gStartupCardStatus[i][j];
                *pnPort = i;
                *pnSlot = j;
                if (n > 1) {
                    (*pnSlot)++;
                }
                return gStartupCardStatus[i][j];
            }
        }
    }
    gStartupCardSlot = -1;
    gStartupCardPort = -1;
    return 4;
}

// Continue the card search: find the next port and slot after the one found last (gStartupCardPort,
// gStartupCardSlot) whose status is not 0, note it, and answer its port and slot (the slot counts from
// 1 here). Returns whether there is one. It also sets every port's slot count to 1 again. Start-up
// command 3.
int Startup_FindNextCardWithStatus(s32* pnPort, s32* pnSlot) {
    int i;
    int j;
    s32 n;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        if (MC_IsMultitapPluggedIn(i)) {
            gStartupCardSlotsPerPort[i] = n = 1;
        } else {
            gStartupCardSlotsPerPort[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            if (gStartupCardStatus[i][j] != 0 &&
                (i > gStartupCardPort || (i == gStartupCardPort && j > gStartupCardSlot))) {
                *pnPort = i;
                *pnSlot = j;
                if (n == 1) {
                    (*pnSlot)++;
                }
                gStartupCardPort = i;
                gStartupCardSlot = j;
                return 1;
            }
        }
    }
    return 0;
}

// Start the card search from the top: the first port and slot whose status is not 0
// (Startup_FindNextCardWithStatus, which continues it). Returns whether there is one. Start-up
// command 2.
int Startup_FindFirstCardWithStatus(s32* pnPort, s32* pnSlot) {
    gStartupCardSlot = -1;
    gStartupCardPort = -1;
    return Startup_FindNextCardWithStatus(pnPort, pnSlot);
}

// Format the card at a port and slot (MC_FormatCard), look at it again (fn_8009DCEC) and send the
// result to the start-up UI as message 0x84 (0 when it worked, else a negative error code).
// Start-up command 7.
void Startup_FormatCard(s32 nPort, s32 nSlot) {
    MsgArg arg;
    s32 nResult = MC_FormatCard(nPort, nSlot);
    fn_8009DCEC(nPort, nSlot);
    Mem_set(&arg, 0, sizeof(arg));
    arg.i = nResult;
    UISDoHint(gpFrontEnd->pHandler, 0x84, 1, (s32*)&arg);
}

// Delete the game's save from the card at a port and slot (MC_DeleteSaveGame), look at the card
// again (fn_8009DCEC) and send the result to the start-up UI as message 0x8D (0 when it worked,
// else a negative error code). Start-up command 17.
void Startup_DeleteSaveGame(s32 nPort, s32 nSlot) {
    MsgArg arg;
    s32 nResult = MC_DeleteSaveGame(nPort, nSlot);
    fn_8009DCEC(nPort, nSlot);
    Mem_set(&arg, 0, sizeof(arg));
    arg.i = nResult;
    UISDoHint(gpFrontEnd->pHandler, 0x8D, 1, (s32*)&arg);
}

// Start-up's per-frame update: the main loop (gomainloop.c fn_8006D8E8) calls it each frame of game
// type 1, in place of fn_8005D2F8. Empty in this build.
void Startup_Update(void) {
}

// Register the stream handler for the 'LEGL' (legal screen) pictures, Startup_LoadLegalPicture, and
// start its count again.
void startup_RegisterStreamClients(void) {
    gnLegalPictures = 0;
    Stream_RegisterLoadChunkCallback('LEGL', Startup_LoadLegalPicture);
}

void startup_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('LEGL');
}

// The 'LEGL' stream handler: keep a copy of the first two objects (the legal screen pictures;
// uiProcessPolygon.c UI_PlayStartUpMovies shows the first after the start-up movies and frees it)
// and free each object. A copy's size is rounded up to 128 bytes (a size already a multiple of 128
// gets 128 more).
void Startup_LoadLegalPicture(UStreamObject* pObject) {
    s32 nPad = 128 - (s32)pObject->uSize % 128;
    if (gnLegalPictures == 0) {
        gLegalPictureSize = pObject->uSize + nPad;
        gpLegalPicture = StaticMem_Alloc(gLegalPictureSize, 2, 16, "startUp.c", 882);
        Mem_cpy(gpLegalPicture, pObject->pData, gLegalPictureSize);
        gnLegalPictures++;
    } else if (gnLegalPictures == 1) {
        gLegalPicture2Size = pObject->uSize + nPad;
        gpLegalPicture2 = StaticMem_Alloc(gLegalPicture2Size, 2, 16, "startUp.c", 891);
        Mem_cpy(gpLegalPicture2, pObject->pData, gLegalPicture2Size);
        gnLegalPictures++;
    }
    StaticMem_Free(pObject);
}

// Build the card status table from scratch (as Startup_CheckCards does, without its messages), then look
// for a card with status 3 (formatted, room for the saves) or out of range. Found: load the options
// from the first card with a good save (MC_LoadOptionsFromFirstCardFound) and note in DiscCheck.c
// (fn_80110458) whether that worked, 1 or 0. Each card passed on the way notes 0 first. Not found:
// the reports start again from the top. Start-up command 20.
void Startup_LoadOptionsFromCard(void) {
    int i;
    int j;
    int n;
    u8 bFound;
    MC_Connect();
    gStartupCardSlot = -1;
    gStartupCardPort = -1;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        gbStartupCardReported[i][0] = 0;
        if (MC_IsMultitapPluggedIn(i)) {
            gStartupCardSlotsPerPort[i] = n = 1;
        } else {
            gStartupCardSlotsPerPort[i] = n = 1;
        }
        for (j = 0; j < n; j++) {
            gStartupCardStatus[i][j] = 0;
        }
        for (j = 0; j < n; j++) {
            gStartupCardStatus[i][j] = Startup_ReadCardStatus(i, j);
            gStartupCardReportedStatus[i][j] = gStartupCardStatus[i][j];
        }
    }
    bFound = 0;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        for (j = 0; j < MC_NUM_SLOTS; j++) {
            switch (gStartupCardStatus[i][j]) {
            case 0:
            case 1:
            case 2:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                gStartupCardSlot = j;
                gStartupCardPort = i;
                fn_80110458(0);
                MC_Disconnect();
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
    MC_Disconnect();
    gStartupCardSlot = j;
    gStartupCardPort = i;
    if (bFound) {
        MC_Connect();
        if (MC_LoadOptionsFromFirstCardFound() == 0) {
            fn_80110458(1);
        } else {
            fn_80110458(0);
        }
        MC_Disconnect();
    } else {
        gStartupCardSlot = -1;
        gStartupCardPort = -1;
    }
}

// Estimate the length of the 2D vector v without a square root. With x the longer side and y the
// shorter (both made positive): x + 31/128 y when y is at most half of x, else (106 x + 75 y) /
// 128.
f32 audvec2_ApproxLength(f32* v) {
    f32 x = v[0];
    f32 y = v[1];
    f32 fSum;
    f32 fDiff;
    if (x < 0.0f) {
        x = -x;
    }
    if (y < 0.0f) {
        y = -y;
    }
    if (y > x) {
        audfrac_Swap(&x, &y);
    }
    if (y > 0.5f * x) {
        fSum = x + y;
        fDiff = x - y;
        fSum = 32.0f * y + (x + (2.0f * fSum + (64.0f * fSum + 8.0f * fSum)));
        y = fDiff;
        x = (1.0f / 128.0f) * fSum;
    }
    return x + 0.25f * y - 0.0078125f * y;
}

// The same estimate for the 3D vector v: the length of (v[2], the length of (v[0], v[1])).
// AudTable.c's Distance3D measures a sound's distance with it.
f32 audvec3_ApproxLength(f32* v) {
    f32 tmpv[2];
    f32 tmp;

    tmp = audvec2_ApproxLength(v);
    audvec2_Set(tmpv, v[2], tmp);
    return audvec2_ApproxLength(tmpv);
}

// Swap the floats *pV1 and *pV2.
void audfrac_Swap(f32* pV1, f32* pV2) {
    f32 temp;

    temp = *pV2;
    *pV2 = *pV1;
    *pV1 = temp;
}

void audvec2_Set(f32* dest, f32 x, f32 y) {
    dest[0] = x;
    dest[1] = y;
}

// Whether the ball can hit the object; every object can.
u32 DynObj_CanBallHit(UObject* pObj, f32* pPos) {
    return 1;
}

// An object's bounding sphere: its centre (the object's position plus its first LOD mesh's sphere
// centre) into pCenter and its radius into pRadius; either may be NULL.
void DynObj_GetBoundingSphere(DynObj* pObj, f32* pCenter, f32* pRadius) {
    UObjMesh* pMesh = pObj->obj.pModel->apLod[0];
    if (pCenter != NULL) {
        Startup_Vec3Add(pObj->obj.m80[3], pMesh->pInfo->v58, pCenter);
    }
    if (pRadius != NULL) {
        *pRadius = pMesh->pInfo->f64;
    }
}

// Did the ball, moving from pFrom to pTo, hit an object? Of the dynamic objects with flag 8 whose
// bounding sphere, grown by the ball's radius (in yards), holds pTo, take the one nearest pFrom:
// tell it (its handler, message 12, with the player number), and give the hit point on its sphere,
// the sphere's normal there and the object (each output may be NULL). Returns whether there was
// one.
u8 DynObj_FindBallHit(int nPlayer, f32* pTo, f32* pFrom, f32* pHit, f32* pNormal, HitObject** ppWhat) {
    f32 vNormal[3];
    f32 vCenter[4];     // fake match: three floats are used; the frame has room for four
    f32 fRadius;
    f32 fBest;
    f32 fDX;
    f32 fDY;
    f32 fDZ;
    f32 fFlat;
    f32 fReach;
    f32 fDist;
    DynObj* pObj;
    DynObj* pBest = NULL;

    for (pObj = fn_80048E44(); pObj != NULL; pObj = pObj->pNext) {
        if (pObj->uFlags & 8) {
            DynObj_GetBoundingSphere(pObj, vCenter, &fRadius);
            fDZ = pTo[2] - vCenter[2];
            fDX = pTo[0] - vCenter[0];
            fFlat = fDX * fDX + fDZ * fDZ;
            fDist = Math_Sqrt(fFlat);
            fReach = gRealBallRadiusIn / 36.0f + fRadius;
            if (fDist < fReach) {
                fDY = pTo[1] - vCenter[1];
                if ((f32)Math_Sqrt(fDY * fDY + fFlat) < fReach && DynObj_CanBallHit(&pObj->obj, pTo)) {
                    fDist = LLMath_DistanceBetween3(pFrom, vCenter);
                    if (pBest == NULL || fDist < fBest) {
                        fBest = fDist;
                        pBest = pObj;
                    }
                }
            }
        }
    }
    if (pBest != NULL) {
        // port: the player number goes through the handler's pointer argument
        pBest->pfnHandler(12, pBest, (void*)nPlayer, NULL);
        DynObj_GetBoundingSphere(pBest, vCenter, &fRadius);
        Startup_Vec3Sub(pTo, vCenter, vNormal);
        LLMath_Normalize3(vNormal, vNormal);
        if (pHit != NULL) {
            fn_8000C5D4(vCenter, vNormal, fRadius, pHit);
        }
        if (pNormal != NULL) {
            Vec3Copy(vNormal, pNormal);
        }
        if (ppWhat != NULL) {
            *ppWhat = (HitObject*)pBest;    // HitObject is Ball.c's view of a DynObj
        }
        return 1;
    }
    return 0;
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void Startup_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Startup_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void Startup_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Startup_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// The UI commands while the session's game type is 1 (start-up): run command nCmd's handler.
void Startup_RunGameMessage(int nCmd, MsgArg* pArgs, MsgArg* pResult) {
    gStartupMessageHandlers[nCmd](pArgs, pResult);
}

// Fill in the start-up UI command table (gStartupMessageHandlers): commands 0..22, command 4 left empty
// (NULL). Called once when start-up begins (gomainloop.c fn_8006CEFC).
void Startup_InitGameMessages(void) {
    int i;
    for (i = 0; i < 23; i++) {
        gStartupMessageHandlers[i] = NULL;
    }
    gStartupMessageHandlers[0] = GM_vStartupGetBlocksNeeded;
    gStartupMessageHandlers[1] = GM_vStartupCheckCards;
    gStartupMessageHandlers[2] = GM_vStartupFindFirstCard;
    gStartupMessageHandlers[3] = GM_vStartupFindNextCard;
    gStartupMessageHandlers[5] = GM_vStartupFadeToBlack;
    gStartupMessageHandlers[6] = GM_vStartupLoadFromCard;
    gStartupMessageHandlers[7] = GM_vStartupFormatCard;
    gStartupMessageHandlers[8] = GM_vStartupGetNextCardStatus;
    gStartupMessageHandlers[9] = GM_vStartupGetCurrentCardStatus;
    gStartupMessageHandlers[10] = GM_vStartupPlaySound;
    gStartupMessageHandlers[11] = GM_vStartupMessage11_Return0;
    gStartupMessageHandlers[12] = GM_vStartupMessage12_Empty;
    gStartupMessageHandlers[13] = GM_vStartupMessage13_Empty;
    gStartupMessageHandlers[14] = GM_vStartupMessage14_Return1;
    gStartupMessageHandlers[15] = GM_vStartupGetFilesNeeded;
    gStartupMessageHandlers[16] = GM_vStartupEndGameLoop;
    gStartupMessageHandlers[17] = GM_vStartupDeleteSaveGame;
    gStartupMessageHandlers[18] = GM_vStartupFormatHadIOError;
    gStartupMessageHandlers[19] = GM_vStartupSkipCardLoad;
    gStartupMessageHandlers[20] = GM_vStartupLoadOptionsCheckDisc;
    gStartupMessageHandlers[21] = GM_vStartupChangeDisc;
    gStartupMessageHandlers[22] = GM_vStartupGetDiscChangeStatus;
}

// Command 0: the card space the game's save and the EA Sports Bio need (fn_8009D390) on the card at
// port pArgs[0], slot pArgs[1]. The slot counts from 1 here; both are kept at 0 or above.
void GM_vStartupGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult) {
    s32 nPort = pArgs[0].i;
    s32 nSlot = pArgs[1].i;
    if (nSlot > 0) {
        nSlot--;
    }
    if (nPort < 0) {
        nPort = 0;
    }
    if (nSlot < 0) {
        nSlot = 0;
    }
    MC_Connect();
    pResult->i = fn_8009D390(nPort, nSlot);
    MC_Disconnect();
}

// Command 1: the start-up card check (Startup_CheckCards): build the card status table and send the
// start-up UI the message for the first card that needs one.
void GM_vStartupCheckCards(MsgArg* pArgs, MsgArg* pResult) {
    Startup_CheckCards();
}

// Command 2: start the card search (Startup_FindFirstCardWithStatus). The port and slot found go to
// the addresses in pArgs[0] and pArgs[1]; answers whether one was found.
void GM_vStartupFindFirstCard(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)Startup_FindFirstCardWithStatus(pArgs[0].p, pArgs[1].p);
}

// Command 3: continue the card search (Startup_FindNextCardWithStatus), as command 2.
void GM_vStartupFindNextCard(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)Startup_FindNextCardWithStatus(pArgs[0].p, pArgs[1].p);
}

// Command 5: start the fade to black (gUIState.bFadeToBlack).
void GM_vStartupFadeToBlack(MsgArg* pArgs, MsgArg* pResult) {
    gUIState.bFadeToBlack = 1;
}

// Command 6 (Startup_LoadFromCard): load the options from the first card with a good save, then the last
// user (MC_LoadInitialUser). When no card status has been read since command 19, it only sends the
// start-up UI message 0x86 (no save found).
void GM_vStartupLoadFromCard(MsgArg* pArgs, MsgArg* pResult) {
    Startup_LoadFromCard();
}

// Command 19 (Startup_SkipCardLoad): make command 6 skip the card and only report no save found (message
// 0x86), until a card's status is read again.
void GM_vStartupSkipCardLoad(MsgArg* pArgs, MsgArg* pResult) {
    Startup_SkipCardLoad();
}

// Command 7: format the card at port pArgs[0], slot pArgs[1] (Startup_FormatCard; the result goes
// to the UI as message 0x84).
void GM_vStartupFormatCard(MsgArg* pArgs, MsgArg* pResult) {
    Startup_FormatCard(pArgs[0].i, pArgs[1].i);
}

// Command 8: report the next card status not yet reported (Startup_GetNextCardStatus). Its port and
// slot go to the addresses in pArgs[0] and pArgs[1].
void GM_vStartupGetNextCardStatus(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = Startup_GetNextCardStatus(pArgs[0].p, pArgs[1].p);
    MC_Disconnect();
}

// Command 9: report again the card the reports reached (Startup_GetCurrentCardStatus). Its port and
// slot go to the addresses in pArgs[0] and pArgs[1].
void GM_vStartupGetCurrentCardStatus(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = Startup_GetCurrentCardStatus(pArgs[0].p, pArgs[1].p);
    MC_Disconnect();
}

// Command 10: play built-in sound 0 when pArgs[0] is 2, else built-in sound 1
// (Aud_PlayBuiltInSound).
void GM_vStartupPlaySound(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 2) {
        Aud_PlayBuiltInSound(0);
        return;
    }
    Aud_PlayBuiltInSound(1);
}

// Command 18: whether a format of the card at port pArgs[0], slot pArgs[1] failed with an I/O error
// (MCCardState.b94).
void GM_vStartupFormatHadIOError(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;
    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.b94;
}

// Command 11 of the start-up table (Startup_InitGameMessages): answers 0.
void GM_vStartupMessage11_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 12 of the start-up table (Startup_InitGameMessages): empty.
void GM_vStartupMessage12_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 13 of the start-up table (Startup_InitGameMessages): empty.
void GM_vStartupMessage13_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 14 of the start-up table (Startup_InitGameMessages): answers 1.
void GM_vStartupMessage14_Return1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

// Command 15: how many new files a save of the game needs on the card at port pArgs[0], slot
// pArgs[1] (fn_8009D3DC: 1 when the save file or its backup is not on it yet, else 0).
void GM_vStartupGetFilesNeeded(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = fn_8009D3DC(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// Command 16: end start-up's main loop (gSession.nC 2, which gomainloop.c fn_8006D01C checks each
// frame), as the menus' GM_vEndGameLoop.
void GM_vStartupEndGameLoop(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

// Command 17: delete the game's save from the card at port pArgs[0], slot pArgs[1]
// (Startup_DeleteSaveGame; the result goes to the UI as message 0x8D).
void GM_vStartupDeleteSaveGame(MsgArg* pArgs, MsgArg* pResult) {
    Startup_DeleteSaveGame(pArgs[0].i, pArgs[1].i);
}

// Command 20: load the options from a card (Startup_LoadOptionsFromCard), then answer 1 when the
// disc in the drive is not disc 1 (fn_8011027C) and no options were loaded (fn_80110460 answers the
// flag Startup_LoadOptionsFromCard left), else 0.
void GM_vStartupLoadOptionsCheckDisc(MsgArg* pArgs, MsgArg* pResult) {
    Startup_LoadOptionsFromCard();
    if (fn_8011027C() && !fn_80110460()) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

// Command 21: ask for the other disc and wait for it.
void GM_vStartupChangeDisc(MsgArg* pArgs, MsgArg* pResult) {
    fn_801102AC();
}

// Command 22: the disc change's progress, as the menus' GM_vGetDiscChangeStatus answers it.
void GM_vStartupGetDiscChangeStatus(MsgArg* pArgs, MsgArg* pResult) {
    GM_vGetDiscChangeStatus(pArgs, pResult);
}
