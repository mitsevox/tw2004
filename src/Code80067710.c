// Code80067710.c (our name; a file of its own by its constants, name unknown): the queue of events
// the situation scripts (SitDevFile.c) react to. event.c's handlers queue each moment of a shot
// (fn_80067710); fn_8006795C runs every script whose event came up, then empties the queue.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "game.h"
#include "sitdev.h"

void FE_GolferAttributes(int nPlayer, u8 nEvent);
void fn_800BB0DC(void);
void fn_800BB1C0(void);
u8   fn_800BB7AC(SitDevEntry* pEntry, SitDevData* pData, int nPlayer);
void fn_800BCD68(SitDevEntry* pEntry, int nSit, int nPlayer, u8 nEvent);
void fn_80067B5C(f32* pA, f32* pB, f32* pOut);

// .sbss, defined in reverse address order
u8 lbl_80281E2A;    // event 26 has been queued since the last event 2 or 3
u8 lbl_80281E29;
u8 lbl_80281E28;

// Queue event nEvent for nPlayer (0xFF: player 0; below 0: nobody).
void fn_80067710(int nPlayer, int a, u8 nEvent) {
    int nWho;
    SitDevEvent* pEvent;
    int i;
    f32 vDiff[3];
    int nInches;

    if (nPlayer == 0xFF) {
        nWho = 0;
    } else if (nPlayer >= 0) {
        nWho = nPlayer;
    } else {
        return;
    }
    if (nEvent == 30) {
        if (gpGame->b136 || gpGame->b137 || gpGame->b138 || gpGame->b139) {
            return;
        }
    }
    switch (nEvent) {
    case 27:
        if (gPlayers[nPlayer].ball.nCollideCount > 0) {
            return;
        }
        break;
    case 26:
        if (lbl_80281E2A) {
            return;
        }
        lbl_80281E2A = 1;
        break;
    case 3:
        lbl_80281E28 = 0;
        fn_800BD74C();
        fn_800BB0DC();
        fn_800BB0C8();
        fn_800BB1C0();
        // fall through
    case 2:
        lbl_80281E2A = 0;
        // fall through
    case 25:
        fn_800BB1C0();
        for (i = 0; i < lbl_802811B8->n13C; i++) {
            if (lbl_802811B8->aEvents[i].nEvent == 8 || lbl_802811B8->aEvents[i].nEvent == 9 ||
                lbl_802811B8->aEvents[i].nEvent == 10 || lbl_802811B8->aEvents[i].nEvent == 11) {
                return;
            }
        }
        break;
    }
    pEvent = &lbl_802811B8->aEvents[lbl_802811B8->n13C];
    pEvent->nPlayer = nWho;
    pEvent->nEvent = nEvent;
    lbl_802811B8->n13C++;
    if (lbl_802811B8->n13C == 1) {
        FE_GolferAttributes(nWho, nEvent);
        if (nEvent == 20 || nEvent == 29) {
            fn_80067B5C(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
            vDiff[1] = 0.0f;
            nInches = 36.0f * (f32)fn_80009680(fn_80009744(vDiff));
            fn_80067B1C(lbl_802811B8->aValue, 18, nInches, lbl_802811B8->aSetBits);
            fn_80067B1C(lbl_802811B8->aValue, 19, nInches, lbl_802811B8->aSetBits);
        }
    }
}

// Run the scripts for this frame's events, then empty the queue.
void fn_8006795C(void) {
    int i;
    int j;
    SitDevEntry* pEntry;
    int nPlayer33;
    SitDevEvent* pEvent;
    SitDevData* pData;
    u8 bFound;
    u8 nEvent;

    nPlayer33 = 5;
    if (lbl_802811B8->n13C == 0) {
        return;
    }
    pEntry = lbl_80282208->p14;
    // fake match: a signed compare here, an unsigned one in SitDevFile.c's fn_800BB52C
    for (i = 0; i < (int)lbl_80282208->nEntries; i++, pEntry++) {
        pData = lbl_802811B8;
        bFound = 0;
        for (j = 0; j < pData->n13C; j++) {
            pEvent = &pData->aEvents[j];
            if (pEvent->nEvent == 33) {
                nPlayer33 = pEvent->nPlayer;
            }
            if ((pEntry->nEvent == pEvent->nEvent || pEntry->nEvent == 0) &&
                pEvent->nPlayer == pData->aEvents[0].nPlayer) {
                bFound = 1;
                break;
            }
        }
        if (bFound) {
            if (pEntry->b2.s.n5 != 22) {
                fn_80067B1C(lbl_802811B8->aValue, 0, Game_CurHoleIndex() + 1, pData->aSetBits);
            }
            if (fn_800BB7AC(pEntry, lbl_802811B8, pEvent->nPlayer) &&
                (pEntry->n0 == 0 || !lbl_802811B8->pD4[pEntry->n0])) {
                lbl_802811B8->pD4[pEntry->n0] = 1;
                nEvent = lbl_802811B8->aEvents[j].nEvent;
                if (nEvent == 29 && (pEntry->auTests[2] & 1)) {
                    lbl_80281E28 = 1;
                    lbl_80281E29 = 1;
                }
                fn_800BCD68(pEntry, pEntry->b2.s.n5, lbl_802811B8->aEvents[j].nPlayer, nEvent);
            }
        }
    }
    if (nPlayer33 != 5 && lbl_801FA198[nPlayer33] == 0) {
        fn_8006AAB4(nPlayer33, 5);
    }
    lbl_802811B8->n13C = 0;
}

void fn_80067B1C(u16* pValues, int nIndex, u16 uValue, u32* pSetBits) {
    pValues[nIndex] = (int)uValue;  // fake match: the no-op widening only moves the store in the schedule
    pSetBits[nIndex / 32] |= 1 << (nIndex % 32);
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_80067B5C(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80067B5C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
