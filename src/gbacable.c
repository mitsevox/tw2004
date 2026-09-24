// gbacable.c (EA's name, from its asserts): the Game Boy Advance link cable: the four ports' link
// state (lbl_80260E18), the commands sent over the cable and the getters and setters the front end
// uses. Only partly decompiled.

#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "pad.h"
#include "core/gbacable.h"
#include "frontend/fe.h"
#include "core/gba.h"

s32  fn_80122AF0(s32 nChan);
s32  fn_80122BCC(s32 nChan);
s32  fn_80122E68(s32 nChan, u32* pWord);
s32  fn_80122CFC(s32 nChan, u32* pCmd);
void fn_80123398(s32 nChan, s32 nCmd, s32 nStat);
void fn_80123ABC(s32 nChan);
void fn_80123C2C(s32 nChan);
void fn_80123CBC(s32 a, s32 b);
void fn_80123E34(void);
s32  fn_80122FD8(s32 nChan);
void fn_8012311C(s32 nChan);
void fn_8012408C(s32 v);

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
        lbl_80260E18[i].n0 = 0;
        fn_8012408C(0x12);
        lbl_80260E18[i].n5C = 0x40;
        lbl_80260E18[i].n64 = 0;
        lbl_80260E18[i].n4C = 0;
        lbl_80260E18[i].n50 = 0;
        lbl_80260E18[i].uKey = (i + 0x40) << 24;
        lbl_80260E18[i].uKey |= (i % 2 ? 0xDF : 0) << 16;
        lbl_80260E18[i].uKey |= (i / 2 ? 0x8F : 0) << 8;
        lbl_80260E18[i].uKey |= fn_801228E0(((lbl_80260E18[i].uKey >> 16) & 0xFF) |
                                            (lbl_80260E18[i].uKey & 0xFF00));
    }
}

// Waits (up to 100 ms) for the GBA to offer its handshake word, and reads it into n4C.
s32 fn_80122AF0(s32 nChan) {
    u32 uWord;
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &lbl_80260E18[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (lbl_80260E18[nChan].uStatus == 0x28) {
            break;
        }
    }
    if (GBARead(nChan, (u8*)&uWord, &lbl_80260E18[nChan].uStatus) != 0) {
        return 0;
    }
    lbl_80260E18[nChan].n4C = uWord;
    return 1;
}

// Waits for the GBA to take a word, sends it the disc's game code, then waits for its answer
// status (0x30).
s32 fn_80122BCC(s32 nChan) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &lbl_80260E18[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (lbl_80260E18[nChan].uStatus == 0x20) {
            break;
        }
    }
    if (GBAWrite(nChan, (u8*)lbl_8028255C, &lbl_80260E18[nChan].uStatus) != 0) {
        return 0;
    }
    uStart = OSGetTick();
    for (;;) {
        if (GBAGetStatus(nChan, &lbl_80260E18[nChan].uStatus) != 0) {
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            return 0;
        }
        if (lbl_80260E18[nChan].uStatus == 0x30) {
            return 1;
        }
    }
}

// Sends the GBA one word once it can take one ("GbaWriteOnline").
s32 fn_80122CFC(s32 nChan, u32* pCmd) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &lbl_80260E18[nChan].uStatus) != 0) {
            OSReport("GbaWriteOnline: Failed to get status from GBA (chan=%d).\n", nChan);
            return 0;
        }
        if ((lbl_80260E18[nChan].uStatus & 0x30) != 0x30) {
            OSReport("GbaWriteOnline: Lost connection with GBA while waiting to write (chan=%d).\n", nChan);
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            OSReport("GbaWriteOnline: Timeout to wait to change GBA status (chan=%d).\n", nChan);
            return 0;
        }
        if (!(lbl_80260E18[nChan].uStatus & 2)) {
            break;
        }
    }
    if (GBAWrite(nChan, (u8*)pCmd, &lbl_80260E18[nChan].uStatus) != 0) {
        OSReport("GbaWriteOnline: Failed to write data to GBA (chan=%d).\n", nChan);
        return 0;
    }
    if ((lbl_80260E18[nChan].uStatus & 0x30) != 0x30) {
        OSReport("GbaWriteOnline: Lost connection with GBA while writing (chan=%d).\n", nChan);
        return 0;
    }
    return 1;
}

// Reads one word from the GBA once it has one ("GbaReadOnline").
s32 fn_80122E68(s32 nChan, u32* pWord) {
    u32 uStart = OSGetTick();

    for (;;) {
        if (GBAGetStatus(nChan, &lbl_80260E18[nChan].uStatus) != 0) {
            OSReport("GbaReadOnline: Failed to get status from GBA (chan=%d).\n", nChan);
            return 0;
        }
        if ((lbl_80260E18[nChan].uStatus & 0x30) != 0x30) {
            OSReport("GbaReadOnline: Lost connection with GBA while waiting to read (chan=%d).\n", nChan);
            return 0;
        }
        if (OSGetTick() - uStart > GBA_TIMEOUT_TICKS) {
            OSReport("GbaReadOnline: Timeout to wait to change GBA status (chan=%d).\n", nChan);
            return 0;
        }
        if ((lbl_80260E18[nChan].uStatus & 8) == 8) {
            break;
        }
    }
    if (GBARead(nChan, (u8*)pWord, &lbl_80260E18[nChan].uStatus) != 0) {
        OSReport("GbaReadOnline: Failed to read data to GBA (chan=%d).\n", nChan);
        return 0;
    }
    if ((lbl_80260E18[nChan].uStatus & 0x30) != 0x30) {
        OSReport("GbaReadOnline: Lost connection with GBA while reading (chan=%d).\n", nChan);
        return 0;
    }
    return 1;
}

// The handshake: the GBA answers with the disc's game code or 0x42545745, is sent the game code,
// asked for its context and read it in eight words ("GbaReadContext").
s32 fn_80122FD8(s32 nChan) {
    u32* pWord;
    u32 uCmd;
    GbaChannel* pCh;
    u32 i;

    if (fn_80122AF0(nChan) == 0) {
        return 0;
    }
    pCh = &lbl_80260E18[nChan];
    if (memcmp(&pCh->n4C, lbl_8028255C, 4) != 0 && lbl_80260E18[nChan].n4C != 0x42545745) {
        return 0;
    }
    if (fn_80122BCC(nChan) == 0) {
        return 0;
    }
    uCmd = 0x60000000;
    if (fn_80122CFC(nChan, &uCmd) == 0) {
        OSReport("GbaReadContext: An error occurred to command 'FROMGC_REQUEST_CONTEXT' (chan=%d).\n", nChan);
        return 0;
    }
    for (i = 0, pWord = (u32*)&pCh->got; i < sizeof(GbaContext); i += 4) {
        if (fn_80122E68(nChan, pWord) == 0) {
            OSReport("GbaReadContext: An error occurred in reading the %d(th) part of %d (chan=%d).\n", i + 1,
                     sizeof(GbaContext), nChan);
            return 0;
        }
        pWord++;
    }
    return 1;
}

// Opens the link on a port from the GBA's context: a GBA with none gets a new one; one whose
// context is ours keeps it; any other is sent ours back. The GBA must echo the tick sent.
void fn_8012311C(s32 nChan) {
    GbaChannel* pCh;
    u32* pSentTick;
    u32 uOld;
    int bSame = 0;
    int bOther = 0;
    u32 uCmd;
    u32 uReply;

    pCh = &lbl_80260E18[nChan];
    pSentTick = &pCh->sent.uTick;
    uOld = pCh->got.uTick;
    pCh->got.uTick = *pSentTick;
    if (pCh->got.b0 == 0) {
        if (pCh->got.b3 == 0) {
            memset(&pCh->sent, 0, sizeof(GbaContext));
            lbl_80260E18[nChan].sent.nChan = nChan;
            lbl_80260E18[nChan].sent.b3 = 1;
            lbl_80260E18[nChan].sent.uStart = lbl_80282560;
            lbl_80260E18[nChan].sent.b2 = lbl_80260E18[nChan].got.b2;
            lbl_80260E18[nChan].sent.nC = lbl_80260E18[nChan].got.nC;
            lbl_80260E18[nChan].u48 = OSGetTick();
            *pSentTick = lbl_80260E18[nChan].u48;
            pCh->n0 = 3;
        } else {
            if (memcmp(&pCh->got, &pCh->sent, sizeof(GbaContext)) == 0 &&
                (uOld == *pSentTick || uOld == lbl_80260E18[nChan].u48)) {
                *pSentTick = uOld;
                bSame = 1;
                lbl_80260E18[nChan].u48 = uOld;
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
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaOpen: An error occurred in writing (chan=%d).\n", nChan);
            return;
        }
        if (fn_80122E68(nChan, &uReply) == 0 || bOther || uCmd != uReply) {
            if (!bOther) {
                OSReport("GbaOpen: An error occurred in reading (chan=%d).\n", nChan);
            }
        } else {
            lbl_80260E18[nChan].u48 = uCmd;
            lbl_80260E18[nChan].n0 = 2;
            OSReport("GbaOpen: Channel %d is connected!\n", nChan);
            fn_8012408C(4);
        }
    }
}

void fn_8012332C(s32 nChan) {
    if (GBAReset(nChan, &lbl_80260E18[nChan].uStatus) == 0) {
        if (fn_80122FD8(nChan)) {
            fn_8012311C(nChan);
        } else {
            fn_8012408C(3);
        }
    }
}

// Talks to a linked GBA ("GbaCommunication"): reads its d-pad word (u58), sends it every port's key,
// then runs one request: 0x70 reads the cash the GBA holds, 0x90 also moves n6C of it to the
// GameCube, 0xD0 sends n6C of cash to the GBA, 0xB0 sends stat nStat (0-3), 0x71 and 0xD3 ask the
// GBA to save its cash and stats, and 0xD1 reads its unlock mask. Any failed command unlinks the port.
void fn_80123398(s32 nChan, s32 nCmd, s32 nStat) {
    u32 uCmd = 0x10000000;
    u32 uWord;
    s32 nValue;
    s32 i;

    if (fn_80122CFC(nChan, &uCmd) == 0) {
        OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_PADDATA' (chan=%d).\n",
                 nChan);
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x12);
        return;
    }
    if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0x20) {
        OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_PADDATA' (chan=%d).\n", nChan);
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x12);
        return;
    }
    lbl_80260E18[nChan].n64 = 0;
    lbl_80260E18[nChan].u58 = uWord;
    lbl_80260E18[nChan].n64 = 1;
    for (i = 0; i < GBA_NUM_CHANNELS; i++) {
        if (fn_80122CFC(nChan, &lbl_80260E18[i].uKey) == 0) {
            OSReport("GbaCommunication: POSITION DATA: An error occurred in writing the %d(th) part of %d "
                     "(chan=%d).\n",
                     i + 1, GBA_NUM_CHANNELS, nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
    }

    switch (nCmd) {
    case 0:
        break;
    case 0x70:
    case 0x90:
        uCmd = 0x70000000;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASHDATA' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0x80) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASHDATA' (chan=%d).\n", nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        lbl_80260E18[nChan].u68 = uWord & 0xFFFFFF;
        if (nCmd == 0x70) {
            break;
        }
        if (lbl_80260E18[nChan].u68 == 0) {
            OSReport("No cash available for transfer from GBA.\n");
            break;
        }
        uCmd = lbl_80260E18[nChan].n6C | 0x90000000;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASHXFER' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0xA0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASHXFER_CONFIRM' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        lbl_80260E18[nChan].n6C = uWord & 0xFFFFFF;
        break;
    case 0xD0:
        uCmd = lbl_80260E18[nChan].n6C | 0xD0000000;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_CASH2GBA' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0xE0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASH2GBA_CONFIRM' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        lbl_80260E18[nChan].n6C = uWord & 0xFFFFFF;
        break;
    case 0xB0:
        switch (nStat) {
        case 0:
            nValue = fn_80077ACC()->nA8;
            break;
        case 1:
            nValue = fn_80077ACC()->nAC;
            break;
        case 2:
            nValue = fn_80077ACC()->nA0;
            break;
        case 3:
            nValue = fn_80077ACC()->nA4;
            break;
        default:
            nValue = 0;
            break;
        }
        uCmd = ((nStat + 0xB0) << 24) | nValue;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_STATS' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != nStat + 0xC0) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_STAT_TRANSFER' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        lbl_80260E18[nChan].n70 = uWord & 0xFFFFFF;
        break;
    case 0x71:
        uCmd = 0x71000000;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_SAVE_CASH' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0x81) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_CASH_SAVED' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        break;
    case 0xD3:
        uCmd = 0xD3000000;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport("GbaCommunication: An error occurred to command 'FROMGC_REQUEST_SAVE_STAT' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0xD4) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_STAT_SAVED' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        break;
    case 0xD1:
        uCmd = 0xD1ABCDEF;
        if (fn_80122CFC(nChan, &uCmd) == 0) {
            OSReport(
                "GbaCommunication: An error occurred to command 'FROMGC_REQUEST_UNLOCKMASK' (chan=%d).\n",
                nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        if (fn_80122E68(nChan, &uWord) == 0 || uWord >> 24 != 0xD2) {
            OSReport("GbaCommunication: An error occurred in reading 'FROMGBA_UNLOCKMASK' (chan=%d).\n",
                     nChan);
            lbl_80260E18[nChan].n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        lbl_80260E18[nChan].n74 = uWord & 0xFFFFFF;
        break;
    }
}

// Sends the GBA the "set port" command and our context in eight words, then reads its context
// back: the port is linked ("GbaSetport").
void fn_80123ABC(s32 nChan) {
    u32 uCmd = 0x30000000;
    u32 i;
    GbaChannel* pCh;
    u32* pWord;

    if (fn_80122CFC(nChan, &uCmd) == 0) {
        OSReport("GbaSetport: An error occurred to command 'FROMGC_SETPORT' (chan=%d).\n", nChan);
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x12);
        return;
    }
    pCh = &lbl_80260E18[nChan];
    pWord = (u32*)&pCh->sent;
    for (i = 0; i < sizeof(GbaContext); i += 4) {
        if (fn_80122CFC(nChan, pWord) == 0) {
            OSReport("GbaSetport: An error occurred in writing  the %d(th) part of %d (chan=%d).\n", i + 1,
                     sizeof(GbaContext), nChan);
            pCh->n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        pWord++;
    }
    for (i = 0, pWord = (u32*)&pCh->got; i < sizeof(GbaContext); i += 4) {
        if (fn_80122E68(nChan, pWord) == 0) {
            OSReport("GbaSetport: An error occurred in reading (chan=%d).\n", nChan);
            pCh->n0 = 0;
            fn_8012408C(0x12);
            return;
        }
        pWord++;
    }
    pCh->n0 = 2;
    OSReport("GbaSetPort: Channel %d is connected!\n", nChan);
    fn_8012408C(4);
}

// Sends the port the "context differs" command and unlinks it.
void fn_80123C2C(s32 nChan) {
    u32 uCmd = 0x50000000;

    if (fn_80122CFC(nChan, &uCmd) == 0) {
        OSReport("GbaSetport: An error occurred to command 'FROMGC_CONTEXT_DIFFER' (chan=%d).\n", nChan);
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x10);
    } else {
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x10);
    }
}

// Moves every port's link on by one step. A port without a GBA (SIProbe type 0x40000), or other
// than the port already being worked on, is unlinked. An unlinked port waits up to 800 ms for the
// GBA to answer, then the port is opened (fn_8012332C), run (fn_80123398), given our context
// (fn_80123ABC) or told the contexts differ (fn_80123C2C).
void fn_80123CBC(s32 a, s32 b) {
    GbaChannel* pCh = lbl_80260E18;
    s32 nChan = 0;
    u32 uStart;
    s32 nErr;

    do {
        if (pCh->n5C != 0x40000 || (lbl_80281984 != -1 && lbl_80281984 != nChan)) {
            pCh->n4C = 0;
            pCh->n0 = 0;
        } else {
            switch (pCh->n0) {
            case 0:
                uStart = OSGetTick();
                do {
                    fn_800A4BDC();
                    fn_800B7490();
                    nErr = GBAGetStatus(nChan, &pCh->uStatus);
                } while (nErr != 0 && OSGetTick() - uStart < GBA_TICKS_PER_MS * 800);
                if (nErr == 0) {
                    pCh->n0 = 1;
                    fn_8012408C(2);
                    lbl_80281984 = nChan;
                }
                break;
            case 1:
                fn_8012332C(nChan);
                break;
            case 2:
                fn_80123398(nChan, a, b);
                break;
            case 3:
                fn_80123ABC(nChan);
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
        pCh++;
    } while (nChan < GBA_NUM_CHANNELS);
}

// Reads the pads. A linked GBA's d-pad (u58, when new and its check byte holds) replaces its port's
// buttons. An unlinked port is probed for what is plugged in (waiting up to 800 ms for a GBA while
// no port is being worked on); ports with a pad (types 8 and 0x40) are reset.
void fn_80123E34(void) {
    u32 uReset = 0;
    s32 nChan = 0;
    GbaChannel* pCh = lbl_80260E18;
    PadStatus* pPad;
    const u32* pMask;
    u32 uKey;
    u32 uStart;
    u8 uProc;

    PADRead(lbl_80260FF8);
    PADClamp(lbl_80260FF8);
    pPad = lbl_80260FF8;
    pMask = lbl_80184E30;
    do {
        if (pCh->n0 == 2) {
            if (pCh->n64 != 0) {
                if ((u8)pCh->u58 == fn_801228E0((pCh->u58 & 0xFF00) | ((pCh->u58 >> 16) & 0xFF))) {
                    uKey = pCh->u58;
                    pPad->uButtons = (((uKey >> 23) & 1) ? 4 : 0) |
                                     ((((uKey >> 22) & 1) ? 8 : 0) |
                                      ((((uKey >> 20) & 1) ? 2 : 0) | ((uKey >> 21) & 1)));
                }
            }
            pCh->n64 = 0;
        } else {
            if (pCh->n0 == 0 && GBAGetProcessStatus(nChan, &uProc) != 2) {
                if (lbl_80281984 == -1) {
                    uStart = OSGetTick();
                    do {
                        fn_800A4BDC();
                        fn_800B7490();
                        pCh->n5C = SIProbe(nChan);
                    } while (pCh->n5C != 0x40000 && OSGetTick() - uStart < GBA_TICKS_PER_MS * 800);
                } else if (nChan != lbl_80281984) {
                    pCh->n5C = SIProbe(nChan);
                }
            }
            if (pCh->n5C == 8 || pCh->n5C == 0x40) {
                uReset |= *pMask;
            }
        }
        nChan++;
        pPad++;
        pMask++;
        pCh++;
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
extern s32 lbl_80282540;
void fn_8012402C(void);
extern s32 lbl_80281980;
extern s32 lbl_80282518;
void fn_8012408C(s32 v);
s32 fn_80124094(void);
void fn_8012409C(void);
extern s32 lbl_8028251C;
void fn_801240A8(void);
extern s32 lbl_80282520;
extern s32 lbl_80282524;
extern s32 lbl_80282528;
extern s32 lbl_8028252C;
extern s32 lbl_80282530;
extern s32 lbl_80282534;
extern s32 lbl_80282538;
extern s32 lbl_8028253C;
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
extern s32 lbl_8028254C;
extern s32 lbl_80282550;
extern s32 lbl_80282554;
extern s32 lbl_80282558;
void fn_80124238(s32 arg0, s32 arg1);
s32 fn_80124280(s32 arg0);

void fn_80123FF8(void) {
    lbl_8028255C = DVDGetCurrentDiskID();
    lbl_80282560 = OSGetTick();
    fn_801229F8();
    GBAInit();
}

void fn_8012402C(void) {
    fn_80123E34();
    fn_80123CBC(0, 0);
    if (OSGetResetButtonState() != 0) {
        lbl_80282540 = 1;
        return;
    }
    if ((s32) lbl_80282540 != 0) {
        OSResetSystem(0, 1, 0);
    }
}

void fn_8012408C(s32 v) {
    lbl_80281980 = v;
}

s32 fn_80124094(void) {
    return lbl_80281980;
}

void fn_8012409C(void) {
    lbl_80282518 = 1;
}

void fn_801240A8(void) {
    SaveProfile* pProfile = fn_80077ACC();

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
    lbl_8028251C = 1;
}

s32 fn_8012411C(void) {
    return lbl_80260E18[lbl_80281984].n6C;
}

void fn_80124138(s32 n) {
    lbl_80260E18[lbl_80281984].n6C = n;
}

void fn_80124154(void) {
    lbl_80260E18[lbl_80281984].n6C = 0;
}

s32 fn_80124174(void) {
    return lbl_80260E18[lbl_80281984].u68;
}

s32 fn_80124190(void) {
    return lbl_80260E18[lbl_80281984].n70;
}

void fn_801241AC(s32 v) {
    lbl_80282520 = v;
}

s32 fn_801241B4(void) {
    return lbl_80282520;
}

void fn_801241BC(s32 v) {
    lbl_80282524 = v;
}

s32 fn_801241C4(void) {
    return lbl_80282524;
}

s32 fn_801241CC(void) {
    return lbl_80282528;
}

void fn_801241D4(s32 v) {
    lbl_80282528 = v;
}

void fn_801241DC(s32 v) {
    lbl_8028252C = v;
}

s32 fn_801241E4(void) {
    return lbl_8028252C;
}

void fn_801241EC(s32 v) {
    lbl_80282530 = v;
}

s32 fn_801241F4(void) {
    return lbl_80282530;
}

void fn_801241FC(s32 v) {
    lbl_80282538 = v;
}

s32 fn_80124204(void) {
    return lbl_80282538;
}

void fn_8012420C(s32 v) {
    lbl_8028253C = v;
}

s32 fn_80124214(void) {
    return lbl_8028253C;
}

void fn_8012421C(s32 v) {
    lbl_80282534 = v;
}

s32 fn_80124224(void) {
    return lbl_80282534;
}

void fn_8012422C(void) {
    lbl_80281984 = -1;
}

void fn_80124238(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        lbl_80282558 = arg1;
        return;
    case 1:
        lbl_80282554 = arg1;
        return;
    case 2:
        lbl_80282550 = arg1;
        return;
    case 3:
        lbl_8028254C = arg1;
        return;
    }
}

s32 fn_80124280(s32 arg0) {
    switch (arg0) {
    case 0:
        return lbl_80282558;
    case 1:
        return lbl_80282554;
    case 2:
        return lbl_80282550;
    case 3:
        return lbl_8028254C;
    default:
        return 0;
    }
}

// ---- end of sweep code ----

// Runs the link for the front end once a frame, by the state fn_8012408C sets. 0 starts it (15
// frames' grace, the port free); 1 polls until a GBA links, giving up (0x11) after 4 seconds; 4 and
// 5 poll the linked GBA, and in 5 a pending request is sent (cash to the GBA, the stats copied into
// the profile, the save-cash and save-stats requests); 0x12 (a failed command) undoes what the
// pending request did to the profile; 6 takes cash from the GBA; 8 swaps stats with it, keeping the
// best of each, and asks it to save them. 0xC to 0xF raise a request and go back to 5.
void fn_801242D0(void) {
    SaveProfile* pProfile;
    s32 bFailed;
    s32 nOld;
    s32 nGot;

    if (fn_80124094() == 0) {
        lbl_80282548 = OSGetTick();
        fn_8012422C();
        lbl_80282544 = 15;
        fn_8012408C(1);
    } else if (fn_80124094() == 1) {
        if (lbl_80282544 == 0) {
            fn_8012402C();
        } else {
            lbl_80282544--;
        }
        if (OSGetTick() - lbl_80282548 > GBA_TICKS_PER_MS * 4000) {
            fn_8012408C(0x11);
        }
    } else if (fn_80124094() == 2) {
        fn_8012402C();
    } else if (fn_80124094() == 4) {
        fn_8012402C();
        fn_80124138(0);
        fn_8012408C(5);
    } else if (fn_80124094() == 5) {
        fn_8012402C();
        if (fn_801241B4()) {
            fn_80123CBC(0xD0, 0);
            pProfile = fn_80077ACC();
            pProfile->n6C -= fn_8012411C();
            fn_80124154();
            fn_801241AC(0);
        } else if (fn_801241C4()) {
            pProfile = fn_80077ACC();
            pProfile->nA8 = fn_80124280(0);
            pProfile->nAC = fn_80124280(1);
            pProfile->nA0 = fn_80124280(2);
            pProfile->nA4 = fn_80124280(3);
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
    } else if (fn_80124094() == 0x12) {
        if (fn_80124204()) {
            pProfile = fn_80077ACC();
            pProfile->n6C -= fn_8012411C();
            fn_80124154();
            fn_801241FC(0);
        }
        if (fn_80124214()) {
            pProfile = fn_80077ACC();
            pProfile->nA8 = fn_80124280(0);
            pProfile->nAC = fn_80124280(1);
            pProfile->nA0 = fn_80124280(2);
            pProfile->nA4 = fn_80124280(3);
            fn_8012420C(0);
        }
    } else if (fn_80124094() == 6) {
        fn_80123CBC(0x90, 0);
        if (fn_80124094() != 0x12) {
            pProfile = fn_80077ACC();
            pProfile->n6C += fn_8012411C();
            fn_80123CBC(0x71, 0);
            fn_80124138(0);
            fn_8012408C(7);
        }
    } else if (fn_80124094() == 8) {
        bFailed = 0;
        pProfile = fn_80077ACC();
        fn_80124238(0, pProfile->nA8);
        fn_80124238(1, pProfile->nAC);
        fn_80124238(2, pProfile->nA0);
        fn_80124238(3, pProfile->nA4);

        // the best round: the lower, where the GBA's is set (0 and 0xFF: none)
        fn_80123CBC(0xB0, 0);
        if (fn_80124094() == 0x12) {
            bFailed = 1;
        }
        nOld = fn_80124280(0);
        nGot = fn_80124190();
        if (nOld <= 0) {
            if (nGot != 0 && nGot != 0xFF) {
                pProfile->nA8 = nGot;
            }
        } else if (nOld > nGot) {
            pProfile->nA8 = nGot;
        }

        // the GBA's count is added
        fn_80123CBC(0xB0, 1);
        if (fn_80124094() == 0x12) {
            bFailed = 1;
        }
        pProfile->nAC += fn_80124190();

        // the longest drive and the longest putt: the higher
        fn_80123CBC(0xB0, 2);
        if (fn_80124094() == 0x12) {
            bFailed = 1;
        }
        if (fn_80124280(2) < fn_80124190()) {
            pProfile->nA0 = fn_80124190();
        }
        fn_80123CBC(0xB0, 3);
        if (fn_80124094() == 0x12) {
            bFailed = 1;
        }
        if (fn_80124280(3) < fn_80124190()) {
            pProfile->nA4 = fn_80124190();
        }
        if (bFailed == 0) {
            fn_80123CBC(0xD3, 0);
            fn_8012408C(9);
        }
    } else if (fn_80124094() == 7) {
        fn_8012402C();
    } else if (fn_80124094() == 9) {
        fn_8012402C();
    } else if (fn_80124094() == 0xC) {
        fn_801241AC(1);
        fn_8012408C(5);
    } else if (fn_80124094() == 0xD) {
        fn_801241BC(1);
        fn_8012408C(5);
    } else if (fn_80124094() == 0xE) {
        fn_801241DC(1);
        fn_8012408C(5);
    } else if (fn_80124094() == 0xF) {
        fn_801241EC(1);
        fn_8012408C(5);
    } else {
        // the state is read once more with nothing done (an empty test in the original)
        fn_80124094();
    }
}
