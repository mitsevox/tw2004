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

s32  fn_80122AF0(s32 nChan);
s32  fn_80122BCC(s32 nChan);
s32  GbaReadOnline(s32 nChan, u32* pWord);
s32  GbaWriteOnline(s32 nChan, u32* pCmd);
void GbaCommunication(s32 nChan, s32 nCmd, s32 nStat);
void GbaSetport(s32 nChan);
void fn_80123C2C(s32 nChan);
void fn_80123CBC(s32 a, s32 b);
void fn_80123E34(void);
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

// The check byte of a port's key: a CRC-style sum over its two bytes, with the polynomial 0xCD.
u32 fn_801228E0(u32 uValue) {
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

// Every port starts unlinked, with its key: 0x40 + the port, then 0xDF for an odd port and 0x8F
// for ports 2 and 3, then the check byte of those two.
void fn_801229F8(void) {
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
        gGbaChannels[i].uKey |= fn_801228E0(((gGbaChannels[i].uKey >> 16) & 0xFF) |
                                            (gGbaChannels[i].uKey & 0xFF00));
    }
}

// Waits (up to 100 ms) for the GBA to offer its handshake word, and reads it into n4C.
s32 fn_80122AF0(s32 nChan) {
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

// Waits for the GBA to take a word, sends it the disc's game code, then waits for its answer
// status (0x30).
s32 fn_80122BCC(s32 nChan) {
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

// Sends the GBA one word once it can take one ("GbaWriteOnline").
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

// Reads one word from the GBA once it has one ("GbaReadOnline").
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

// The handshake: the GBA answers with the disc's game code or 0x42545745, is sent the game code,
// asked for its context and read it in eight words ("GbaReadContext").
s32 GbaReadContext(s32 nChan) {
    u32* pWord;
    u32 uCmd;
    GbaChannel* pCh;
    u32 i;

    if (fn_80122AF0(nChan) == 0) {
        return 0;
    }
    pCh = &gGbaChannels[nChan];
    if (memcmp(&pCh->n4C, gGbaDiscID, 4) != 0 && gGbaChannels[nChan].n4C != 0x42545745) {
        return 0;
    }
    if (fn_80122BCC(nChan) == 0) {
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

// Opens the link on a port from the GBA's context: a GBA with none gets a new one (sent in state 3,
// GbaSetport); one whose context is ours is sent a new tick and is linked once it echoes it; any
// other is sent its own tick back and the port goes to state 4 (the contexts differ).
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

void fn_8012332C(s32 nChan) {
    if (GBAReset(nChan, &gGbaChannels[nChan].uStatus) == 0) {
        if (GbaReadContext(nChan)) {
            GbaOpen(nChan);
        } else {
            Gba_SetState(3);
        }
    }
}

// Talks to a linked GBA ("GbaCommunication"): reads its d-pad word (u58), sends it every port's key,
// then runs one request: 0x70 reads the cash the GBA holds, 0x90 also moves n6C of it to the
// GameCube, 0xD0 sends n6C of cash to the GBA, 0xB0 sends stat nStat (0-3), 0x71 and 0xD3 ask the
// GBA to save its cash and stats, and 0xD1 reads its unlock mask. Any failed command unlinks the port.
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

// Sends the GBA the "set port" command and our context in eight words, then reads its context
// back: the port is linked ("GbaSetport").
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

// Sends the port the "context differs" command and unlinks it.
void fn_80123C2C(s32 nChan) {
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

// Moves every port's link on by one step. A port without a GBA (SIProbe type 0x40000), or other
// than the port already being worked on, is unlinked. An unlinked port waits up to 800 ms for the
// GBA to answer, then the port is opened (fn_8012332C), run (GbaCommunication), given our context
// (GbaSetport) or told the contexts differ (fn_80123C2C).
void fn_80123CBC(s32 a, s32 b) {
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
                fn_8012332C(nChan);
                break;
            case 2:
                GbaCommunication(nChan, a, b);
                break;
            case 3:
                GbaSetport(nChan);
                break;
            case 4:
                fn_80123C2C(nChan);
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

// Reads the pads. A linked GBA's d-pad (u58, when new and its check byte holds) replaces its port's
// buttons. An unlinked port is probed for what is plugged in (waiting up to 800 ms for a GBA while
// no port is being worked on); ports whose probe gave 8 or 0x40 are reset.
void fn_80123E34(void) {
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
                if ((u8)pCh->u58 == fn_801228E0(((pCh->u58 >> 16) & 0xFF) | (pCh->u58 & 0xFF00))) {
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

void fn_801229F8();
void fn_80123FF8(void);
s32 OSGetResetButtonState();
s32 OSResetSystem(s32, s32, s32);
void fn_8012402C(void);
void Gba_SetState(s32 v);
s32 Gba_GetState(void);
void fn_8012409C(void);
void fn_801240A8(void);
void fn_801241AC(s32 v);
s32 fn_801241B4(void);
void fn_801241BC(s32 v);
s32 fn_801241C4(void);
s32 fn_801241CC(void);
void fn_801241D4(s32 v);
void fn_801241DC(s32 v);
s32 fn_801241E4(void);
void fn_801241EC(s32 v);
s32 fn_801241F4(void);
void fn_801241FC(s32 v);
s32 fn_80124204(void);
void fn_8012420C(s32 v);
s32 fn_80124214(void);
void fn_8012421C(s32 v);
s32 fn_80124224(void);
void fn_8012422C(void);
void fn_80124238(s32 arg0, s32 arg1);
s32 fn_80124280(s32 arg0);

void fn_80123FF8(void) {
    gGbaDiscID = DVDGetCurrentDiskID();
    gGbaInitTick = OSGetTick();
    fn_801229F8();
    GBAInit();
}

void fn_8012402C(void) {
    fn_80123E34();
    fn_80123CBC(0, 0);
    if (OSGetResetButtonState() != 0) {
        gGbaResetPressed = 1;
        return;
    }
    if ((s32) gGbaResetPressed != 0) {
        OSResetSystem(0, 1, 0);
    }
}

// Sets the GBA link state (the link code uses 3, 4 and 0x12; -1 at start).
void Gba_SetState(s32 v) {
    gGbaLinkState = v;
}

// The GBA link state set by Gba_SetState.
s32 Gba_GetState(void) {
    return gGbaLinkState;
}

void fn_8012409C(void) {
    gGbaUnlocksGranted = 1;
}

void fn_801240A8(void) {
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

s32 fn_8012411C(void) {
    return gGbaChannels[gGbaPortInUse].n6C;
}

void fn_80124138(s32 n) {
    gGbaChannels[gGbaPortInUse].n6C = n;
}

void fn_80124154(void) {
    gGbaChannels[gGbaPortInUse].n6C = 0;
}

s32 fn_80124174(void) {
    return gGbaChannels[gGbaPortInUse].u68;
}

s32 fn_80124190(void) {
    return gGbaChannels[gGbaPortInUse].n70;
}

void fn_801241AC(s32 v) {
    gGbaSendCashPending = v;
}

s32 fn_801241B4(void) {
    return gGbaSendCashPending;
}

void fn_801241BC(s32 v) {
    gGbaRestoreStatsPending = v;
}

s32 fn_801241C4(void) {
    return gGbaRestoreStatsPending;
}

s32 fn_801241CC(void) {
    return gGbaReadPending;
}

void fn_801241D4(s32 v) {
    gGbaReadPending = v;
}

void fn_801241DC(s32 v) {
    gGbaSaveCashPending = v;
}

s32 fn_801241E4(void) {
    return gGbaSaveCashPending;
}

void fn_801241EC(s32 v) {
    gGbaSaveStatsPending = v;
}

s32 fn_801241F4(void) {
    return gGbaSaveStatsPending;
}

void fn_801241FC(s32 v) {
    gGbaCashUnsaved = v;
}

s32 fn_80124204(void) {
    return gGbaCashUnsaved;
}

void fn_8012420C(s32 v) {
    gGbaStatsUnsaved = v;
}

s32 fn_80124214(void) {
    return gGbaStatsUnsaved;
}

void fn_8012421C(s32 v) {
    gGbaUndoTransfer = v;
}

s32 fn_80124224(void) {
    return gGbaUndoTransfer;
}

void fn_8012422C(void) {
    gGbaPortInUse = -1;
}

void fn_80124238(s32 arg0, s32 arg1) {
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

s32 fn_80124280(s32 arg0) {
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

// Runs the link for the front end once a frame, by the state Gba_SetState sets. 0 starts it (15
// frames' grace, the port free); 1 polls until a GBA links, giving up (0x11) after 4 seconds; 2, 4,
// 5, 7 and 9 poll the linked GBA (4 clears the amount and goes to 5), and in 5 a pending request is
// sent (cash to the GBA, the stats copied into the profile, the save-cash and save-stats requests);
// 0x12 (a failed command) undoes what the pending request did to the profile; 6 takes cash from the
// GBA and has it save its cash (7); 8 swaps stats with it (below) and, when none failed, asks it to
// save them (9). 0xC to 0xF raise a request and go back to 5.
void fn_801242D0(void) {
    SaveProfile* pProfile;
    s32 bFailed;
    s32 nOld;
    s32 nGot;

    if (Gba_GetState() == 0) {
        gGbaSearchStartTick = OSGetTick();
        fn_8012422C();
        gGbaSearchDelayFrames = 15;
        Gba_SetState(1);
    } else if (Gba_GetState() == 1) {
        if (gGbaSearchDelayFrames == 0) {
            fn_8012402C();
        } else {
            gGbaSearchDelayFrames--;
        }
        if (OSGetTick() - gGbaSearchStartTick > GBA_TICKS_PER_MS * 4000) {
            Gba_SetState(0x11);
        }
    } else if (Gba_GetState() == 2) {
        fn_8012402C();
    } else if (Gba_GetState() == 4) {
        fn_8012402C();
        fn_80124138(0);
        Gba_SetState(5);
    } else if (Gba_GetState() == 5) {
        fn_8012402C();
        if (fn_801241B4()) {
            fn_80123CBC(0xD0, 0);
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash -= fn_8012411C();
            fn_80124154();
            fn_801241AC(0);
        } else if (fn_801241C4()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nBestRound = fn_80124280(0);
            pProfile->nHolesInOne = fn_80124280(1);
            pProfile->nLongestDrive = fn_80124280(2);
            pProfile->nLongestPutt = fn_80124280(3);
            fn_801241BC(0);
        } else if (fn_801241E4()) {
            fn_80123CBC(0x71, 0);
            fn_801241FC(0);
            fn_801241DC(0);
        } else if (fn_801241F4()) {
            fn_80123CBC(0xD3, 0);
            fn_8012420C(0);
            fn_801241EC(0);
        }
    } else if (Gba_GetState() == 0x12) {
        if (fn_80124204()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash -= fn_8012411C();
            fn_80124154();
            fn_801241FC(0);
        }
        if (fn_80124214()) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nBestRound = fn_80124280(0);
            pProfile->nHolesInOne = fn_80124280(1);
            pProfile->nLongestDrive = fn_80124280(2);
            pProfile->nLongestPutt = fn_80124280(3);
            fn_8012420C(0);
        }
    } else if (Gba_GetState() == 6) {
        fn_80123CBC(0x90, 0);
        if (Gba_GetState() != 0x12) {
            pProfile = FE_GetCurrentProfile();
            pProfile->nCurrentCash += fn_8012411C();
            fn_80123CBC(0x71, 0);
            fn_80124138(0);
            Gba_SetState(7);
        }
    } else if (Gba_GetState() == 8) {
        bFailed = 0;
        pProfile = FE_GetCurrentProfile();
        fn_80124238(0, pProfile->nBestRound);
        fn_80124238(1, pProfile->nHolesInOne);
        fn_80124238(2, pProfile->nLongestDrive);
        fn_80124238(3, pProfile->nLongestPutt);

        // the best round: when ours is unset (<= 0), the GBA's if it is set (not 0 or 0xFF);
        // else the lower of the two
        fn_80123CBC(0xB0, 0);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        nOld = fn_80124280(0);
        nGot = fn_80124190();
        if (nOld <= 0) {
            if (nGot != 0 && nGot != 0xFF) {
                pProfile->nBestRound = nGot;
            }
        } else if (nOld > nGot) {
            pProfile->nBestRound = nGot;
        }

        // the GBA's count is added
        fn_80123CBC(0xB0, 1);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        pProfile->nHolesInOne += fn_80124190();

        // the longest drive and the longest putt: the higher
        fn_80123CBC(0xB0, 2);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        if (fn_80124280(2) < fn_80124190()) {
            pProfile->nLongestDrive = fn_80124190();
        }
        fn_80123CBC(0xB0, 3);
        if (Gba_GetState() == 0x12) {
            bFailed = 1;
        }
        if (fn_80124280(3) < fn_80124190()) {
            pProfile->nLongestPutt = fn_80124190();
        }
        if (bFailed == 0) {
            fn_80123CBC(0xD3, 0);
            Gba_SetState(9);
        }
    } else if (Gba_GetState() == 7) {
        fn_8012402C();
    } else if (Gba_GetState() == 9) {
        fn_8012402C();
    } else if (Gba_GetState() == 0xC) {
        fn_801241AC(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xD) {
        fn_801241BC(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xE) {
        fn_801241DC(1);
        Gba_SetState(5);
    } else if (Gba_GetState() == 0xF) {
        fn_801241EC(1);
        Gba_SetState(5);
    } else {
        // the state is read once more with nothing done (an empty test in the original)
        Gba_GetState();
    }
}
