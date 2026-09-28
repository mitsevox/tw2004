// gbacable.c (EA's name, from its asserts): the Game Boy Advance link cable: the four ports' link
// state (gGbaChannels), the commands sent over the cable and the getters and setters the front end
// uses.

#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "pad.h"
#include "core/gbacable.h"
#include "frontend/fe.h"
#include "core/gba.h"

s32  Gba_ReadHandshakeCode(s32 nChan);
s32  Gba_SendGameCode(s32 nChan);
s32  GbaReadOnline(s32 nChan, u32* pWord);
s32  GbaWriteOnline(s32 nChan, u32* pCmd);
void GbaCommunication(s32 nChan, s32 nCmd, s32 nStat);
void GbaSetport(s32 nChan);
void Gba_SendContextDiffer(s32 nChan);
void Gba_StepPorts(s32 a, s32 b);
void Gba_ReadPads(void);
s32  GbaReadContext(s32 nChan);
void GbaOpen(s32 nChan);
void Gba_SetState(s32 v);

const u32 gGbaPadResetBits[GBA_NUM_CHANNELS] = { 0x80000000, 0x40000000, 0x20000000, 0x10000000 };

s32 gGbaLinkState = -1;
s32 gGbaPortInUse = -1;

// data order: .bss and .sbss are defined in reverse address order (CodeWarrior lays them out
// last-defined-first)
PadStatus gGbaPads[GBA_NUM_CHANNELS];
GbaChannel gGbaChannels[GBA_NUM_CHANNELS];

u32 gGbaInitTick;
DVDDiskID* gGbaDiscID;
s32 gGbaSavedBestRound;
s32 gGbaSavedHolesInOne;
s32 gGbaSavedLongestDrive;
s32 gGbaSavedLongestPutt;
u32 gGbaSearchStartTick;
u32 gGbaSearchDelayFrames;
s32 gGbaResetPressed;
s32 gGbaStatsUnsaved;
s32 gGbaCashUnsaved;
s32 gGbaUndoTransfer;
s32 gGbaSaveStatsPending;
s32 gGbaSaveCashPending;
s32 gGbaReadPending;
s32 gGbaRestoreStatsPending;
s32 gGbaSendCashPending;
s32 gGbaRewardsUnlocked;
s32 gGbaUnlocksGranted;

// The check byte of the two bytes in the low 16 bits of uValue: a CRC-8 with the polynomial 0xCD
// (0x1CD with its top bit), the low byte first, each byte from its top bit, then eight zero bits.
// It guards the port keys (Gba_InitChannels) and the GBA's pad words (Gba_ReadPads).
u32 Gba_CalcCheckByte(u32 uValue) {
    u32 uSum = 0;
    u32 uBit;
    int i;

    for (uBit = 0x80; uBit != 0; uBit >>= 1) {
        uSum <<= 1;
        if (uValue & uBit) {
            if (uSum & 0x100) {
                uSum ^= 0xCC;
            } else {
                uSum += 1;
            }
        } else if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    uValue >>= 8;
    for (uBit = 0x80; uBit != 0; uBit >>= 1) {
        uSum <<= 1;
        if (uValue & uBit) {
            if (uSum & 0x100) {
                uSum ^= 0xCC;
            } else {
                uSum += 1;
            }
        } else if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    for (i = 0; i < 8; i++) {
        uSum <<= 1;
        if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    return uSum & 0xFF;
}

// Unlinks every port (step 0) and makes its key (uKey): 0x40 + the port in the top byte, then 0xDF
// for ports 1 and 3, then 0x8F for ports 2 and 3, then the check byte of those two
// (Gba_CalcCheckByte). Also clears n4C, n50 and the new-pad-word flag n64, marks the probe result
// u5C as SI_ERROR_UNKNOWN (0x40), and sets link state 0x12 once per port. Called by Gba_Init.
void Gba_InitChannels(void) {
    int i;

    for (i = 0; i < GBA_NUM_CHANNELS; i++) {
        gGbaChannels[i].n0 = 0;
        Gba_SetState(0x12);
        gGbaChannels[i].u5C = 0x40;
        gGbaChannels[i].n64 = 0;
        gGbaChannels[i].n4C = 0;
        gGbaChannels[i].n50 = 0;
        gGbaChannels[i].uKey = (i + 0x40) << 24;
        gGbaChannels[i].uKey |= (i % 2 ? 0xDF : 0) << 16;
        gGbaChannels[i].uKey |= (i / 2 ? 0x8F : 0) << 8;
        gGbaChannels[i].uKey |= Gba_CalcCheckByte(((gGbaChannels[i].uKey >> 16) & 0xFF) |
                                            (gGbaChannels[i].uKey & 0xFF00));
    }
}

// The first step of the handshake (GbaReadContext): waits up to 100 ms for the GBA's status to be
// exactly 0x28 (GBA_JSTAT_PSF1 | GBA_JSTAT_SEND: a word waiting), then reads that word, the code
// the GBA answers with, into n4C. 1: read; 0: a status call or the read failed, or the wait ran
// out.
s32 Gba_ReadHandshakeCode(s32 nChan) {
    u32 uWord;
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &gGbaChannels[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (gGbaChannels[nChan].uStatus == 0x28) {
            break;
        }
    }
    if (GBARead(nChan, (u8*)&uWord, &gGbaChannels[nChan].uStatus) != 0) {
        return 0;
    }
    gGbaChannels[nChan].n4C = uWord;
    return 1;
}

// The second step of the handshake (GbaReadContext): waits up to 100 ms for the GBA's status to be
// exactly 0x20 (GBA_JSTAT_PSF1), sends it the first word of the disc's ID (gGbaDiscID: its game
// code), then waits up to 100 ms for the status 0x30 (GBA_JSTAT_PSF1 | GBA_JSTAT_PSF0). 1: done; 0:
// a call failed or a wait ran out.
s32 Gba_SendGameCode(s32 nChan) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &gGbaChannels[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (gGbaChannels[nChan].uStatus == 0x20) {
            break;
        }
    }
    if (GBAWrite(nChan, (u8*)gGbaDiscID, &gGbaChannels[nChan].uStatus) != 0) {
        return 0;
    }
    uStart = OSGetTick();
    for (;;) {
        if (GBAGetStatus(nChan, &gGbaChannels[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (gGbaChannels[nChan].uStatus == 0x30) {
            return 1;
        }
    }
}

// Sends a GBA that is online one word, *pCmd ("GbaWriteOnline"). It waits up to 100 ms for the GBA
// to have taken the last word (GBA_JSTAT_RECV clear) while both general flags hold (0x30:
// GBA_JSTAT_PSF1 | GBA_JSTAT_PSF0, the GBA program is running the link), writes the word and checks
// the flags again. 1: sent; 0: a call failed, the flags dropped or the wait ran out (each reported
// with OSReport).
s32 GbaWriteOnline(s32 nChan, u32* pCmd) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &gGbaChannels[nChan].uStatus) != 0) {
            OSReport("GbaWriteOnline: Failed to get status from GBA (chan=%d).\n", nChan);
            return 0;
        }
        if ((gGbaChannels[nChan].uStatus & 0x30) != 0x30) {
            OSReport("GbaWriteOnline: Lost connection with GBA while waiting to write (chan=%d).\n", nChan);
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            OSReport("GbaWriteOnline: Timeout to wait to change GBA status (chan=%d).\n", nChan);
            return 0;
        }
        if (!(gGbaChannels[nChan].uStatus & 2)) {
            break;
        }
    }
    if (GBAWrite(nChan, (u8*)pCmd, &gGbaChannels[nChan].uStatus) != 0) {
        OSReport("GbaWriteOnline: Failed to write data to GBA (chan=%d).\n", nChan);
        return 0;
    }
    if ((gGbaChannels[nChan].uStatus & 0x30) != 0x30) {
        OSReport("GbaWriteOnline: Lost connection with GBA while writing (chan=%d).\n", nChan);
        return 0;
    }
    return 1;
}

// Reads one word from a GBA that is online into *pWord ("GbaReadOnline"). It waits up to 100 ms for
// the GBA to have a word waiting (GBA_JSTAT_SEND) while both general flags hold (0x30:
// GBA_JSTAT_PSF1 | GBA_JSTAT_PSF0), reads it and checks the flags again. 1: read; 0: a call failed,
// the flags dropped or the wait ran out (each reported with OSReport).
s32 GbaReadOnline(s32 nChan, u32* pWord) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &gGbaChannels[nChan].uStatus) != 0) {
            OSReport("GbaReadOnline: Failed to get status from GBA (chan=%d).\n", nChan);
            return 0;
        }
        if ((gGbaChannels[nChan].uStatus & 0x30) != 0x30) {
            OSReport("GbaReadOnline: Lost connection with GBA while waiting to read (chan=%d).\n", nChan);
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            OSReport("GbaReadOnline: Timeout to wait to change GBA status (chan=%d).\n", nChan);
            return 0;
        }
        if ((gGbaChannels[nChan].uStatus & 8) == 8) {
            break;
        }
    }
    if (GBARead(nChan, (u8*)pWord, &gGbaChannels[nChan].uStatus) != 0) {
        OSReport("GbaReadOnline: Failed to read data to GBA (chan=%d).\n", nChan);
        return 0;
    }
    if ((gGbaChannels[nChan].uStatus & 0x30) != 0x30) {
        OSReport("GbaReadOnline: Lost connection with GBA while reading (chan=%d).\n", nChan);
        return 0;
    }
    return 1;
}

// The handshake with a GBA that answered ("GbaReadContext"): the GBA must offer the disc's game
// code or the four letters BTWE (0x42545745) (Gba_ReadHandshakeCode); it is sent the game code
// (Gba_SendGameCode) and the context request (0x60000000, FROMGC_REQUEST_CONTEXT), and its
// 0x20-byte context is read in eight words into got. 1: read; 0: a step failed.
s32 GbaReadContext(s32 nChan) {
    u32* pWord;
    u32 uCmd;
    GbaChannel* pCh;
    u32 i;

    if (Gba_ReadHandshakeCode(nChan) == 0) {
        return 0;
    }
    pCh = &gGbaChannels[nChan];
    if (memcmp(&pCh->n4C, gGbaDiscID, 4) != 0 && gGbaChannels[nChan].n4C != 0x42545745) {
        return 0;
    }
    if (Gba_SendGameCode(nChan) == 0) {
        return 0;
    }
    uCmd = 0x60000000;
    if (GbaWriteOnline(nChan, &uCmd) == 0) {
        OSReport("GbaReadContext: An error occurred to command 'FROMGC_REQUEST_CONTEXT' (chan=%d).\n", nChan);
        return 0;
    }
    for (i = 0, pWord = (u32*)&pCh->got; i < sizeof(GbaContext); i += 4) {
        if (GbaReadOnline(nChan, pWord) == 0) {
            OSReport("GbaReadContext: An error occurred in reading the %d(th) part of %d (chan=%d).\n", i + 1,
                     sizeof(GbaContext), nChan);
            return 0;
        }
        pWord++;
    }
    return 1;
}

// Opens the link on a port from the context the GBA sent (got) ("GbaOpen"); got's tick is first
// replaced with ours. With got.b0 clear: a GBA with no context (got.b3 0) gets a new one (the port,
// gGbaInitTick, got's b2 and nC, a fresh tick) and the port goes to step 3 (GbaSetport sends it); a
// GBA whose context equals ours, with the tick we sent or last used, is sent a new tick and is
// linked once it echoes it (step 2, link state 4); any other is sent its own tick back and the port
// goes to step 4 (Gba_SendContextDiffer). With got.b0 set nothing happens and the port stays in
// step 1.
void GbaOpen(s32 nChan) {
    GbaChannel* pCh;
    u32* pSentTick;
    u32 uOld;
    int bSame = 0;
    int bOther = 0;
    u32 uCmd;
    u32 uReply;

    // fake match: set in two steps, so the "connected" store at the end is not folded into pCh
    pCh = gGbaChannels;
    pCh += nChan;
    pSentTick = &pCh->sent.uTick;
    uOld = pCh->got.uTick;
    pCh->got.uTick = *pSentTick;
    if (pCh->got.b0 == 0) {
        if (pCh->got.b3 == 0) {
            memset(&pCh->sent, 0, sizeof(GbaContext));
            gGbaChannels[nChan].sent.nChan = nChan;
            gGbaChannels[nChan].sent.b3 = 1;
            gGbaChannels[nChan].sent.uStart = gGbaInitTick;
            gGbaChannels[nChan].sent.b2 = gGbaChannels[nChan].got.b2;
            gGbaChannels[nChan].sent.nC = gGbaChannels[nChan].got.nC;
            gGbaChannels[nChan].u48 = OSGetTick();
            *pSentTick = gGbaChannels[nChan].u48;
            pCh->n0 = 3;
        } else {
            if (memcmp(&pCh->got, &pCh->sent, sizeof(GbaContext)) == 0 &&
                (uOld == *pSentTick || uOld == gGbaChannels[nChan].u48)) {
                *pSentTick = uOld;
                bSame = 1;
                gGbaChannels[nChan].u48 = uOld;
            }
            if (!bSame) {
                bOther = 1;
                pCh->n0 = 4;
            }
        }
    }
    if (bSame || bOther) {
        if (bOther) {
            uCmd = uOld;
        } else {
            uCmd = OSGetTick();
            *pSentTick = uCmd;
        }
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaOpen: An error occurred in writing (chan=%d).\n", nChan);
            return;
        }
        if (GbaReadOnline(nChan, &uReply) == 0 || bOther || uCmd != uReply) {
            if (!bOther) {
                OSReport("GbaOpen: An error occurred in reading (chan=%d).\n", nChan);
            }
        } else {
            gGbaChannels[nChan].u48 = uCmd;
            gGbaChannels[nChan].n0 = 2;
            OSReport("GbaOpen: Channel %d is connected!\n", nChan);
            Gba_SetState(4);
        }
    }
}

// Port step 1 (a GBA answered): resets the GBA (GBAReset); when that works, runs the handshake
// (GbaReadContext) and opens the link (GbaOpen), or sets link state 3 when the context could not be
// read. A failed reset or handshake leaves the port in step 1, tried again on the next poll.
void Gba_ResetAndOpen(s32 nChan) {
    if (GBAReset(nChan, &gGbaChannels[nChan].uStatus) == 0) {
        if (GbaReadContext(nChan)) {
            GbaOpen(nChan);
        } else {
            Gba_SetState(3);
        }
    }
}

// Port step 2 (linked), once a poll ("GbaCommunication"): asks for the GBA's pad word (0x10000000,
// FROMGC_REQUEST_PADDATA; the answer 0x20xxxxxx goes into u58 with n64 set), sends it every port's
// key (uKey, "POSITION DATA"), then runs request nCmd, each answer a 24-bit value under a reply
// byte: 0x70 reads the cash on the GBA into u68; 0x90 does that and, if there is some, asks the GBA
// to hand over n6C of it (FROMGC_REQUEST_CASHXFER), n6C becoming the amount it confirms; 0xD0 sends
// n6C of cash to the GBA (FROMGC_REQUEST_CASH2GBA), n6C becoming the amount it confirms; 0xB0 sends
// the current profile's stat nStat (0 best round, 1 holes in one, 2 longest drive, 3 longest putt,
// else 0) and puts the GBA's value of it in n70; 0x71 and 0xD3 ask the GBA to save its cash and its
// stats; 0xD1 reads its unlock mask into n74 (nothing sends 0xD1 in this build); 0 sends nothing
// more. A failed command unlinks the port (step 0) and sets link state 0x12.
void GbaCommunication(s32 nChan, s32 nCmd, s32 nStat) {
    u32 uCmd = 0x10000000;
    u32 uWord;
    s32 i;
    s32 nWhich;

    if (GbaWriteOnline(nChan, &uCmd) == 0) {
        OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_PADDATA' (chan=%d).\n",
                 nChan);
        gGbaChannels[nChan].n0 = 0;
        Gba_SetState(0x12);
        return;
    }
    if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0x20) {
        OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_PADDATA' (chan=%d).\n", nChan);
        gGbaChannels[nChan].n0 = 0;
        Gba_SetState(0x12);
        return;
    }
    gGbaChannels[nChan].n64 = 0;
    gGbaChannels[nChan].u58 = uWord;
    gGbaChannels[nChan].n64 = 1;
    for (i = 0; i < GBA_NUM_CHANNELS; i++) {
        if (GbaWriteOnline(nChan, &gGbaChannels[i].uKey) == 0) {
            OSReport("GbaCommunication: POSITION DATA: An error occurred in writing the %d(th) part of %d "
                     "(chan=%d).\n",
                     i + 1, GBA_NUM_CHANNELS, nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
    }

    switch (nCmd) {
    case 0:
        break;
    case 0x70:
    case 0x90:
        uCmd = 0x70000000;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASHDATA' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0x80) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASHDATA' (chan=%d).\n", nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        gGbaChannels[nChan].u68 = uWord & 0xFFFFFF;
        if (nCmd == 0x70U) {    // EA compared unsigned here (cmplwi), unlike the switch
            break;
        }
        if (gGbaChannels[nChan].u68 == 0) {
            OSReport("No cash available for transfer from GBA.\n");
            break;
        }
        uCmd = gGbaChannels[nChan].n6C | 0x90000000;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASHXFER' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0xA0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASHXFER_CONFIRM' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        gGbaChannels[nChan].n6C = uWord & 0xFFFFFF;
        break;
    case 0xD0:
        uCmd = gGbaChannels[nChan].n6C | 0xD0000000;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASH2GBA' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0xE0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASH2GBA_CONFIRM' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        gGbaChannels[nChan].n6C = uWord & 0xFFFFFF;
        break;
    case 0xB0:
        // EA keeps the stat number in nWhich and reuses nStat for the stat's value.
        nWhich = nStat;
        switch (nStat) {
        case 0:
            nStat = FE_GetCurrentProfile()->nBestRound;
            break;
        case 1:
            nStat = FE_GetCurrentProfile()->nHolesInOne;
            break;
        case 2:
            nStat = FE_GetCurrentProfile()->nLongestDrive;
            break;
        case 3:
            nStat = FE_GetCurrentProfile()->nLongestPutt;
            break;
        default:
            nStat = 0;
            break;
        }
        uCmd = ((nWhich + 0xB0) << 24) | nStat;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_STATS' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != nWhich + 0xC0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_STAT_TRANSFER' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        gGbaChannels[nChan].n70 = uWord & 0xFFFFFF;
        break;
    case 0x71:
        uCmd = 0x71000000;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_SAVE_CASH' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0x81) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASH_SAVED' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        break;
    case 0xD3:
        uCmd = 0xD3000000;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_SAVE_STAT' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0xD4) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_STAT_SAVED' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        break;
    case 0xD1:
        uCmd = 0xD1ABCDEF;
        if (GbaWriteOnline(nChan, &uCmd) == 0) {
            OSReport(
                "GbaCommunication: An error occurred to command 'FROMGC_REQUEST_UNLOCKMASK' (chan=%d).\n",
                nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        if (GbaReadOnline(nChan, &uWord) == 0 || uWord >> 24 != 0xD2) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_UNLOCKMASK' (chan=%d).\n",
                     nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
        gGbaChannels[nChan].n74 = uWord & 0xFFFFFF;
        break;
    }
}

// Port step 3 ("GbaSetport"): sends the GBA the set-port command (0x30000000, FROMGC_SETPORT) and
// our new context (sent) in eight words, then reads its context back into got; the port is linked
// (step 2, link state 4). A failed command unlinks it (step 0, link state 0x12).
void GbaSetport(s32 nChan) {
    u32 uCmd = 0x30000000;
    u32 i;

    if (GbaWriteOnline(nChan, &uCmd) == 0) {
        OSReport("GbaSetport: An error occurred to command 'FROMGC_SETPORT' (chan=%d).\n", nChan);
        gGbaChannels[nChan].n0 = 0;
        Gba_SetState(0x12);
        return;
    }
    for (i = 0; i < sizeof(GbaContext); i += 4) {
        if (GbaWriteOnline(nChan, (u32*)((u8*)&gGbaChannels[nChan].sent + i)) == 0) {
            OSReport("GbaSetport: An error occurred in writing  the %d(th) part of %d (chan=%d).\n", i + 1,
                     sizeof(GbaContext), nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
    }
    for (i = 0; i < sizeof(GbaContext); i += 4) {
        if (GbaReadOnline(nChan, (u32*)((u8*)&gGbaChannels[nChan].got + i)) == 0) {
            OSReport("GbaSetport: An error occurred in reading (chan=%d).\n", nChan);
            gGbaChannels[nChan].n0 = 0;
            Gba_SetState(0x12);
            return;
        }
    }
    gGbaChannels[nChan].n0 = 2;
    OSReport("GbaSetPort: Channel %d is connected!\n", nChan);
    Gba_SetState(4);
}

// Port step 4: tells the GBA its context is not ours (0x50000000, FROMGC_CONTEXT_DIFFER) and
// unlinks the port (step 0) with link state 0x10, whether the command went through or not. Its
// error text says "GbaSetport", copied from there.
void Gba_SendContextDiffer(s32 nChan) {
    u32 uCmd = 0x50000000;

    if (GbaWriteOnline(nChan, &uCmd) == 0) {
        OSReport("GbaSetport: An error occurred to command 'FROMGC_CONTEXT_DIFFER' (chan=%d).\n", nChan);
        gGbaChannels[nChan].n0 = 0;
        Gba_SetState(0x10);
    } else {
        gGbaChannels[nChan].n0 = 0;
        Gba_SetState(0x10);
    }
}

// Moves each port's link on one step (GbaChannel.n0); a and b are the request a linked port runs
// (GbaCommunication's nCmd and nStat; 0: none). A port whose last probe (u5C, Gba_ReadPads) found
// no GBA (SI_GBA, 0x40000), or any port but gGbaPortInUse while that is set, is unlinked. Steps: 0
// waits up to 800 ms (keeping the sound and the disc-error screen going) for the GBA to answer a
// status call, then goes to step 1 with link state 2 and makes the port gGbaPortInUse; 1
// Gba_ResetAndOpen; 2 GbaCommunication; 3 GbaSetport; 4 Gba_SendContextDiffer.
void Gba_StepPorts(s32 a, s32 b) {
    GbaChannel* pCh;
    s32 nChan = 0;
    u32 uStart;
    s32 nErr;
    u8* pStatus;

    do {
        pCh = &gGbaChannels[nChan];
        if (pCh->u5C != 0x40000 || (gGbaPortInUse != -1 && gGbaPortInUse != nChan)) {
            pCh->n4C = 0;
            pCh->n0 = 0;
        } else {
            switch (pCh->n0) {
            case 0:
                uStart = OSGetTick();
                pStatus = &pCh->uStatus;
                do {
                    Gaud_Cycle();
                    fn_800B7490();
                    nErr = GBAGetStatus(nChan, pStatus);
                } while (nErr != 0 && OSGetTick() - uStart < GBA_TICKS_PER_MS * 800);
                if (nErr == 0) {
                    pCh->n0 = 1;
                    Gba_SetState(2);
                    gGbaPortInUse = nChan;
                }
                break;
            case 1:
                Gba_ResetAndOpen(nChan);
                break;
            case 2:
                GbaCommunication(nChan, a, b);
                break;
            case 3:
                GbaSetport(nChan);
                break;
            case 4:
                Gba_SendContextDiffer(nChan);
                break;
            default:
                OSPanic("gbacable.c", 903, "Unkonwn status.\n");
                break;
            }
        }
        nChan++;
    } while (nChan < GBA_NUM_CHANNELS);
}

// fake match: EA takes the probe's slot through an inline; &pCh->u5C in place allocates pType r25, not r30
static inline u32* fn_80123E34_Read(GbaChannel* pCh) {
    return &pCh->u5C;
}

// Reads the pads into gGbaPads (nothing else reads them). On a linked port, a new pad word from the
// GBA (u58, n64 set) whose check byte holds replaces the port's buttons with the GBA's d-pad (bits
// 20-23: right, left, up, down, as the pad's right, left, up and down bits). A port in step 0 with
// no GBA transfer running (GBAGetProcessStatus not GBA_BUSY) is probed (SIProbe into u5C): with no
// port in use the probe is repeated for up to 800 ms until it finds a GBA (SI_GBA), else ports
// other than the one in use are probed once. Unlinked ports whose last probe gave
// SI_ERROR_NO_RESPONSE (8) or SI_ERROR_UNKNOWN (0x40) are reset (PADReset).
void Gba_ReadPads(void) {
    int nChan;
    u32 uReset = 0;
    GbaChannel* pCh;
    PadStatus* pPad;
    const u32* pMask;
    u32 uKey;
    u32 uStart;
    u8 uProc;
    u32* pType;

    PADRead(gGbaPads);
    PADClamp(gGbaPads);
    nChan = 0;
    do {
        pCh = &gGbaChannels[nChan];
        pPad = &gGbaPads[nChan];
        pMask = &gGbaPadResetBits[nChan];
        if (pCh->n0 == 2) {
            if (pCh->n64 != 0) {
                if ((u8)pCh->u58 == Gba_CalcCheckByte(((pCh->u58 >> 16) & 0xFF) | (pCh->u58 & 0xFF00))) {
                    uKey = pCh->u58;
                    pPad->uButtons = (((uKey >> 23) & 1) ? 4 : 0) |
                                     ((((uKey >> 22) & 1) ? 8 : 0) |
                                      ((((uKey >> 20) & 1) ? 2 : 0) | ((uKey >> 21) & 1)));
                }
            }
            pCh->n64 = 0;
        } else {
            if (pCh->n0 == 0 && GBAGetProcessStatus(nChan, &uProc) != 2) {
                if (gGbaPortInUse == -1) {
                    uStart = OSGetTick();
                    pType = fn_80123E34_Read(pCh);
                    do {
                        Gaud_Cycle();
                        fn_800B7490();
                        *pType = SIProbe(nChan);
                    } while (*pType != 0x40000 && OSGetTick() - uStart < GBA_TICKS_PER_MS * 800);
                } else if (nChan != gGbaPortInUse) {
                    pCh->u5C = SIProbe(nChan);
                }
            }
            if (pCh->u5C == 8 || pCh->u5C == 0x40) {
                uReset |= *pMask;
            }
        }
        nChan++;
    } while (nChan < GBA_NUM_CHANNELS);
    if (uReset != 0) {
        PADReset(uReset);
    }
}

// ---- sweep code (not yet cleaned up) ----

void Gba_InitChannels();
void Gba_Init(void);
s32 OSGetResetButtonState();
s32 OSResetSystem(s32, s32, s32);
void Gba_PollLink(void);
void Gba_SetState(s32 v);
s32 Gba_GetState(void);
void Gba_MarkUnlocksGranted(void);
void Gba_UnlockProfileRewards(void);
void Gba_SetSendCashPending(s32 v);
s32 Gba_IsSendCashPending(void);
void Gba_SetRestoreStatsPending(s32 v);
s32 Gba_IsRestoreStatsPending(void);
s32 Gba_IsReadPending(void);
void Gba_SetReadPending(s32 v);
void Gba_SetSaveCashPending(s32 v);
s32 Gba_IsSaveCashPending(void);
void Gba_SetSaveStatsPending(s32 v);
s32 Gba_IsSaveStatsPending(void);
void Gba_SetCashUnsaved(s32 v);
s32 Gba_IsCashUnsaved(void);
void Gba_SetStatsUnsaved(s32 v);
s32 Gba_IsStatsUnsaved(void);
void Gba_SetUndoTransfer(s32 v);
s32 Gba_IsUndoTransfer(void);
void Gba_ClearPortInUse(void);
void Gba_SaveProfileStat(s32 arg0, s32 arg1);
s32 Gba_GetSavedProfileStat(s32 arg0);

// Sets the link code up (GM_vGbaStartLink): keeps the disc's ID (gGbaDiscID) and the start tick
// (gGbaInitTick), unlinks every port and makes its key (Gba_InitChannels), and starts the GBA
// library (GBAInit).
void Gba_Init(void) {
    gGbaDiscID = DVDGetCurrentDiskID();
    gGbaInitTick = OSGetTick();
    Gba_InitChannels();
    GBAInit();
}

// One poll of the link with no request: reads the pads (Gba_ReadPads) and steps the ports
// (Gba_StepPorts(0, 0)). It also watches the reset button: while it is held gGbaResetPressed is
// set, and the first poll after it is let go resets the console (OSResetSystem(0, 1, 0)). Called by
// Gba_UpdateLinkState and, inside wait loops, gomainloop.c's fn_8006C63C.
void Gba_PollLink(void) {
    Gba_ReadPads();
    Gba_StepPorts(0, 0);
    if (OSGetResetButtonState() != 0) {
        gGbaResetPressed = 1;
        return;
    }
    if ((s32) gGbaResetPressed != 0) {
        OSResetSystem(0, 1, 0);
    }
}

// Sets the GBA link state (gGbaLinkState), which Gba_UpdateLinkState runs once a frame and the menus read
// (GM_vGbaGetLinkState): -1 never started; 0 start (GM_vGbaStartLink); 1 looking for a GBA; 2 a GBA
// answered; 3 its context could not be read; 4 linked; 5 linked, idle; 6 take its cash, then 7; 8
// swap stats, then 9; 0xC-0xF raise a pending request; 0x10 the contexts differ; 0x11 no GBA
// answered in 4 s; 0x12 a command failed, or the menus cancelled.
void Gba_SetState(s32 v) {
    gGbaLinkState = v;
}

// The GBA link state set by Gba_SetState.
s32 Gba_GetState(void) {
    return gGbaLinkState;
}

// Sets gGbaUnlocksGranted (GM_vGbaGrantUnlocks); nothing in this build reads it.
void Gba_MarkUnlocksGranted(void) {
    gGbaUnlocksGranted = 1;
}

// The Game Boy Advance link's unlocks in the current profile: the last course (aCourseUnlocked[22])
// and rewards 0-17. Sets gGbaRewardsUnlocked, which nothing reads. GM_vGbaGrantUnlocks calls it
// once per profile.
void Gba_UnlockProfileRewards(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    // the last course and the first 18 rewards
    pProfile->aCourseUnlocked[22] = 1;
    pProfile->aRewardUnlocked[0] = 1;
    pProfile->aRewardUnlocked[1] = 1;
    pProfile->aRewardUnlocked[2] = 1;
    pProfile->aRewardUnlocked[3] = 1;
    pProfile->aRewardUnlocked[4] = 1;
    pProfile->aRewardUnlocked[5] = 1;
    pProfile->aRewardUnlocked[6] = 1;
    pProfile->aRewardUnlocked[7] = 1;
    pProfile->aRewardUnlocked[8] = 1;
    pProfile->aRewardUnlocked[9] = 1;
    pProfile->aRewardUnlocked[10] = 1;
    pProfile->aRewardUnlocked[11] = 1;
    pProfile->aRewardUnlocked[12] = 1;
    pProfile->aRewardUnlocked[13] = 1;
    pProfile->aRewardUnlocked[14] = 1;
    pProfile->aRewardUnlocked[15] = 1;
    pProfile->aRewardUnlocked[16] = 1;
    pProfile->aRewardUnlocked[17] = 1;
    gGbaRewardsUnlocked = 1;
}

// The cash to move between the GBA and the profile: n6C of the port in use. With no port in use
// (gGbaPortInUse -1) it indexes gGbaChannels[-1]: the bytes before the array (TibExt.c's card
// state, its file name at 0x38).
s32 Gba_GetCashToMove(void) {
    return gGbaChannels[gGbaPortInUse].n6C;
}

// Sets the cash to move (n6C of the port in use). With no port in use (gGbaPortInUse -1) it indexes
// gGbaChannels[-1]: it writes into TibExt.c's card state (its file name at 0x38).
void Gba_SetCashToMove(s32 n) {
    gGbaChannels[gGbaPortInUse].n6C = n;
}

// Zeroes the cash to move (n6C of the port in use). With no port in use (gGbaPortInUse -1) it
// indexes gGbaChannels[-1]: it writes into TibExt.c's card state.
void Gba_ClearCashToMove(void) {
    gGbaChannels[gGbaPortInUse].n6C = 0;
}

// The cash the GBA holds, as last read (u68 of the port in use, request 0x70 or 0x90). With no port
// in use (gGbaPortInUse -1) it indexes gGbaChannels[-1]: TibExt.c's card state.
s32 Gba_GetCashOnGba(void) {
    return gGbaChannels[gGbaPortInUse].u68;
}

// The GBA's value of the stat last sent (n70 of the port in use, request 0xB0). With no port in use
// (gGbaPortInUse -1) it indexes gGbaChannels[-1]: TibExt.c's card state.
s32 Gba_GetGbaStat(void) {
    return gGbaChannels[gGbaPortInUse].n70;
}

// Raises (1, link state 0xC) or clears the request that Gba_UpdateLinkState's state 5 sends: the
// cash to move goes to the GBA (0xD0) and off the profile.
void Gba_SetSendCashPending(s32 v) {
    gGbaSendCashPending = v;
}

s32 Gba_IsSendCashPending(void) {
    return gGbaSendCashPending;
}

// Raises (1, link state 0xD) or clears the request that Gba_UpdateLinkState's state 5 carries out:
// the four stats saved before the last swap (Gba_SaveProfileStat) are copied back into the profile.
void Gba_SetRestoreStatsPending(s32 v) {
    gGbaRestoreStatsPending = v;
}

s32 Gba_IsRestoreStatsPending(void) {
    return gGbaRestoreStatsPending;
}

// gGbaReadPending, which the menus ask for (GM_vGbaIsReadPending). Only ever 0: nothing sets it
// non-zero in this build.
s32 Gba_IsReadPending(void) {
    return gGbaReadPending;
}

// Sets gGbaReadPending; the only call (GM_vGbaReadCashAndStats) clears it.
void Gba_SetReadPending(s32 v) {
    gGbaReadPending = v;
}

// Raises (1, link state 0xE) or clears the request that Gba_UpdateLinkState's state 5 sends: the
// GBA saves its cash (0x71).
void Gba_SetSaveCashPending(s32 v) {
    gGbaSaveCashPending = v;
}

s32 Gba_IsSaveCashPending(void) {
    return gGbaSaveCashPending;
}

// Raises (1, link state 0xF) or clears the request that Gba_UpdateLinkState's state 5 sends: the
// GBA saves its stats (0xD3).
void Gba_SetSaveStatsPending(s32 v) {
    gGbaSaveStatsPending = v;
}

s32 Gba_IsSaveStatsPending(void) {
    return gGbaSaveStatsPending;
}

// Sets gGbaCashUnsaved: while it is set, link state 0x12 takes the cash to move back off the
// profile. Every call clears it, so that undo never runs in this build.
void Gba_SetCashUnsaved(s32 v) {
    gGbaCashUnsaved = v;
}

s32 Gba_IsCashUnsaved(void) {
    return gGbaCashUnsaved;
}

// Sets gGbaStatsUnsaved: while it is set, link state 0x12 copies the stats saved before the swap
// back into the profile. Every call clears it, so that undo never runs in this build.
void Gba_SetStatsUnsaved(s32 v) {
    gGbaStatsUnsaved = v;
}

s32 Gba_IsStatsUnsaved(void) {
    return gGbaStatsUnsaved;
}

// Sets gGbaUndoTransfer: while it is set, GM_vGbaReadCashAndStats turns link state 7 into 0xC (send
// the cash back) and 9 into 0xD (restore the stats). The only call clears it.
void Gba_SetUndoTransfer(s32 v) {
    gGbaUndoTransfer = v;
}

// gGbaUndoTransfer (GM_vGbaReadCashAndStats); only ever 0 in this build.
s32 Gba_IsUndoTransfer(void) {
    return gGbaUndoTransfer;
}

// No port in use (gGbaPortInUse -1): every port may be searched for a GBA again. Link state 0 calls
// it.
void Gba_ClearPortInUse(void) {
    gGbaPortInUse = -1;
}

// Keeps nValue as the profile's stat nStat (0 best round, 1 holes in one, 2 longest drive, 3
// longest putt) as it was before a stat swap with the GBA; other numbers are ignored.
void Gba_SaveProfileStat(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        gGbaSavedBestRound = arg1;
        return;
    case 1:
        gGbaSavedHolesInOne = arg1;
        return;
    case 2:
        gGbaSavedLongestDrive = arg1;
        return;
    case 3:
        gGbaSavedLongestPutt = arg1;
        return;
    }
}

// The profile's stat nStat (0 best round, 1 holes in one, 2 longest drive, 3 longest putt) as
// Gba_SaveProfileStat kept it; 0 for other numbers.
s32 Gba_GetSavedProfileStat(s32 arg0) {
    switch (arg0) {
    case 0:
        return gGbaSavedBestRound;
    case 1:
        return gGbaSavedHolesInOne;
    case 2:
        return gGbaSavedLongestDrive;
    case 3:
        return gGbaSavedLongestPutt;
    default:
        return 0;
    }
}

// ---- end of sweep code ----

// The GBA link's state machine, run once a frame (gomainloop.c's fn_8006D838) by the state
// Gba_SetState sets. 0 frees the port in use, waits 15 frames and goes to 1; 1 polls (Gba_PollLink)
// and gives up (0x11) after 4 s; 2, 7 and 9 poll; 4 (just linked) polls, zeroes the cash to move
// and goes to 5; 5 polls and carries out one pending request: cash to the GBA (0xD0, taken off the
// profile), the stats saved before a swap copied back into the profile, or the save-cash /
// save-stats requests (0x71 / 0xD3). 6 takes the cash to move (the menus set it,
// GM_vGbaAddCashToMove) from the GBA (0x90) into the profile, has the GBA save its cash (0x71) and
// goes to 7. 8 keeps a copy of the profile's four stats and swaps each with the GBA (0xB0), keeping
// for the best round the lower one that is set (0 and 0xFF from the GBA mean unset only while the
// profile has none), adding the GBA's holes in one, and keeping the longer drive and putt; when no
// command failed it has the GBA save them (0xD3) and goes to 9. 0x12 undoes what gGbaCashUnsaved
// and gGbaStatsUnsaved mark (neither is set in this build). 0xC-0xF raise the send-cash,
// restore-stats, save-cash and save-stats requests and go back to 5; only 0xC and 0xD are ever set
// (GM_vGbaReadCashAndStats), and only while gGbaUndoTransfer is set, which it never is.
void Gba_UpdateLinkState(void) {
    SaveProfile* pProfile;
    s32 bFailed;
    s32 nOld;
    s32 nGot;

    if (Gba_GetState() == 0) {
        gGbaSearchStartTick = OSGetTick();
        Gba_ClearPortInUse();
        gGbaSearchDelayFrames = 15;
        Gba_SetState(1);
    } else if (Gba_GetState() == 1) {
        if (gGbaSearchDelayFrames == 0) {
            Gba_PollLink();
        } else {
            gGbaSearchDelayFrames--;
        }
        if (OSGetTick() - gGbaSearchStartTick > GBA_TICKS_PER_MS * 4000) {
            Gba_SetState(0x11);
        }
    } else if (Gba_GetState() == 2) {
        Gba_PollLink();
    } else if (Gba_GetState() == 4) {
        Gba_PollLink();
        Gba_SetCashToMove(0);
        Gba_SetState(5);
    } else if (Gba_GetState() == 5) {
        Gba_PollLink();
        if (Gba_IsSendCashPending()) {
            Gba_StepPorts(0xD0, 0);
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash -= Gba_GetCashToMove();
            Gba_ClearCashToMove();
            Gba_SetSendCashPending(0);
        } else if (Gba_IsRestoreStatsPending()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nBestRound = Gba_GetSavedProfileStat(0);
            pProfile->nHolesInOne = Gba_GetSavedProfileStat(1);
            pProfile->nLongestDrive = Gba_GetSavedProfileStat(2);
            pProfile->nLongestPutt = Gba_GetSavedProfileStat(3);
            Gba_SetRestoreStatsPending(0);
        } else if (Gba_IsSaveCashPending()) {
            Gba_StepPorts(0x71, 0);
            Gba_SetCashUnsaved(0);
            Gba_SetSaveCashPending(0);
        } else if (Gba_IsSaveStatsPending()) {
            Gba_StepPorts(0xD3, 0);
            Gba_SetStatsUnsaved(0);
            Gba_SetSaveStatsPending(0);
        }
    } else if (Gba_GetState() == 0x12) {
        if (Gba_IsCashUnsaved()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash -= Gba_GetCashToMove();
            Gba_ClearCashToMove();
            Gba_SetCashUnsaved(0);
        }
        if (Gba_IsStatsUnsaved()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nBestRound = Gba_GetSavedProfileStat(0);
            pProfile->nHolesInOne = Gba_GetSavedProfileStat(1);
            pProfile->nLongestDrive = Gba_GetSavedProfileStat(2);
            pProfile->nLongestPutt = Gba_GetSavedProfileStat(3);
            Gba_SetStatsUnsaved(0);
        }
    } else if (Gba_GetState() == 6) {
        Gba_StepPorts(0x90, 0);
        if (Gba_GetState() != 0x12) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash += Gba_GetCashToMove();
            Gba_StepPorts(0x71, 0);
            Gba_SetCashToMove(0);
            Gba_SetState(7);
        }
    } else if (Gba_GetState() == 8) {
        bFailed = 0;
        pProfile = FE_GetCurrentProfile();
        Gba_SaveProfileStat(0, pProfile->nBestRound);
        Gba_SaveProfileStat(1, pProfile->nHolesInOne);
        Gba_SaveProfileStat(2, pProfile->nLongestDrive);
        Gba_SaveProfileStat(3, pProfile->nLongestPutt);

        // the best round: when ours is unset (<= 0), the GBA's if it is set (not 0 or 0xFF);
        // else the lower of the two
        Gba_StepPorts(0xB0, 0);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        nOld = Gba_GetSavedProfileStat(0);
        nGot = Gba_GetGbaStat();
        if (nOld <= 0) {
            if (nGot != 0 && nGot != 0xFF) {
                pProfile->nBestRound = nGot;
            }
        } else if (nOld > nGot) {
            pProfile->nBestRound = nGot;
        }

        // the GBA's count is added
        Gba_StepPorts(0xB0, 1);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        pProfile->nHolesInOne += Gba_GetGbaStat();

        // the longest drive and the longest putt: the higher
        Gba_StepPorts(0xB0, 2);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        if (Gba_GetSavedProfileStat(2) < Gba_GetGbaStat()) {
            pProfile->nLongestDrive = Gba_GetGbaStat();
        }
        Gba_StepPorts(0xB0, 3);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        if (Gba_GetSavedProfileStat(3) < Gba_GetGbaStat()) {
            pProfile->nLongestPutt = Gba_GetGbaStat();
        }
        if (bFailed == 0) {
            Gba_StepPorts(0xD3, 0);
            Gba_SetState(9);
        }
    } else if (Gba_GetState() == 7) {
        Gba_PollLink();
    } else if (Gba_GetState() == 9) {
        Gba_PollLink();
    } else if (Gba_GetState() == 0xC) {
        Gba_SetSendCashPending(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xD) {
        Gba_SetRestoreStatsPending(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xE) {
        Gba_SetSaveCashPending(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xF) {
        Gba_SetSaveStatsPending(1);
        Gba_SetState(5);
    } else {
        // the state is read once more with nothing done (an empty test in the original)
        Gba_GetState();
    }
}
