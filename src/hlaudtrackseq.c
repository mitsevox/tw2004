// hlaudtrackseq.c (TW06's name, by structure: golf/audio/engine/hl/hlaudtrackseq.c, between
// hlaudtrack.c and hlaudtrackstm.c): the sequencer, the tracks that play events instead of a
// stream. fn_800AAD18 fills its table of event handlers (lbl_801F1880, 0x800AA694-0x800AAD14).
// Its extent is its data: fn_800AA744 is the first to use its .sdata2 block
// (0x80283FD8-0x80283FF8), and the handlers it lists run up to 0x800AAD14.
// Not yet decompiled: the functions below are the sweep's.

#include "core/audtrack.h"

void fn_800AAE08(AudTrack* pTrack);

void fn_800AA694(void) {
}

void fn_800AAAA0(void) {
}

// An event handler: sets a bit (the event's n3) in one of the track's source's two masks.
void fn_800AAB48(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudSource* pSource;
    u8 uBit;

    pSource = pTrack->pSource;
    uBit = 1 << (s8)pEvent->n3;
    if ((u8)pEvent->n4 != 0) {
        pSource->u0 |= uBit;
        return;
    }
    pSource->u1 |= uBit;
}

void fn_800AAD14(void) {
}

void fn_800AADE8(AudTrack* pTrack) {
    fn_800AAE08(pTrack);
}

void fn_800AAE70(AudTrack* pTrack) {
    fn_800AAE08(pTrack);
}

void fn_800AAEEC(AudTrack* pTrack) {
    pTrack->u.seq.n66 = pTrack->pTmpl->n3;
}

u32 fn_800AB32C(u32 nRange) {
    if (nRange != 0) {
        return Rand_Next(1) % nRange;
    }
    return 0;
}

u8 fn_800AB374(void) {
    return lbl_80282080 == 0;
}

f32 fn_800AB39C(void) {
    return lbl_80281460;
}
