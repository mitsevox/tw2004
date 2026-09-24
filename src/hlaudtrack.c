// hlaudtrack.c (TW06's name, by structure: golf/audio/engine/hl/hlaudtrack.c, the file before
// hlaudtrackseq.c and hlaudtrackstm.c): the sound engine's tracks. A pool of 32 tracks, each
// playing one channel of a sound source, either sequenced (hlaudtrackseq.c) or streamed from disc
// (hlaudtrackstm.c). The tracks of 3D sources sit in a list sorted on their priority, so the
// quietest can be stolen when the pool runs out. Its extent is its data: it is the first to use
// the .bss at 0x801F1868 and the only user of the .sdata2 block 0x80283FD0-0x80283FD8.

#include "core/audtrack.h"

// Puts a track in the sorted list, before the first one of lower priority.
void fn_800A9808(AudTrack* pTrack) {
    AudTrack* pAt;
    UList* pList;

    pAt = (AudTrack*)lbl_801F1868[1].pHead;
    pList = &lbl_801F1868[1];
    if (pAt == NULL) {
        fn_800ADEC8(pList, &pTrack->link);
    }
    while (pAt != NULL) {
        if (pAt->f48 < pTrack->f48) {
            fn_800ADF08(pList, &pTrack->link, &pAt->link);
            return;
        }
        if (pAt->link.pNext == NULL) {
            fn_800ADEC8(pList, &pTrack->link);
            return;
        }
        pAt = (AudTrack*)pAt->link.pNext;
    }
}

// Sets up the track pool and the two lists, then the sequencer and the streamer.
u8 fn_800A98B4(void) {
    u8 bOk;
    u8 i;

    bOk = 0;
    lbl_802820A0 = fn_800B5BD8(32 * sizeof(AudTrack));
    if (lbl_802820A0 != NULL) {
        fn_80005AE8(lbl_802820A0, 0, 32 * sizeof(AudTrack));
        for (i = 0; i < 32; i++) {
            lbl_802820A0[i].nIndex = i;
            lbl_802820A0[i].nState = 0;
        }
        fn_800AE0DC(&lbl_80282098, lbl_802820A0, 32, sizeof(AudTrack));
        fn_800ADE70(&lbl_801F1868[0], 32);
        fn_800ADE70(&lbl_801F1868[1], 32);
        bOk = fn_800AAD18();
        if (bOk) {
            bOk = fn_800ABBC8();
        }
    }
    return bOk;
}

// Frees every track.
u8 fn_800A9A50(u8 a, u8 b) {
    s32 i;
    UList* pList;
    AudTrack* pTrack;

    i = 0;
    pList = lbl_801F1868;
    do {
        pTrack = (AudTrack*)pList->pHead;
        while (pTrack != NULL) {
            fn_800A9D7C(pTrack);
            // the free only reuses the link's first word, so pNext is still there
            pTrack = (AudTrack*)pTrack->link.pNext;
        }
        i++;
        pList++;
    } while (i < 2);
    return 1;
}

void fn_800A9AC4(void) {
}

// Ticks every track: one that was allocated but never started is freed on its second tick, one
// whose tick says it has ended is freed, the others are rendered.
void fn_800A9AC8(void) {
    s32 i;
    UList* pList;
    AudTrack* pTrack;
    AudTrackTmpl* pTmpl;

    pList = lbl_801F1868;
    if ((lbl_8028207C & 0xD) != 0xD) return;
    i = 0;
    do {
        pTrack = (AudTrack*)pList->pHead;
        while (pTrack != NULL) {
            pTmpl = pTrack->pTmpl;
            if (pTrack->nState == 1) {
                if (pTrack->bits.b.bTicked) {
                    fn_800A9D7C(pTrack);
                } else {
                    pTrack->bits.b.bTicked = 1;
                }
            } else if (!(lbl_8028207C & 0x40) ||
                       (pTmpl->data.pPlayList->n3 == 0 && pTrack->pSource->nSound != 8)) {
                if (fn_800AA2A4(pTrack)) {
                    fn_800AA34C(pTrack);
                } else {
                    fn_800A9D7C(pTrack);
                }
            }
            pTrack = (AudTrack*)pTrack->link.pNext;
        }
        i++;
        pList++;
    } while (i < 2);
}

// Allocates a track for channel nChannel of a source. When the pool is empty, a sorted source
// steals the lowest-priority sorted track below fPriority that is not streamed or unstarted; an
// unsorted source takes the last sorted track.
AudTrack* fn_800A9BC8(AudSource* pSource, AudTrackTmpl* pTmpl, u8 nChannel, f32 fPriority) {
    UPool* const pPool = &lbl_80282098;
    s32 bSorted;
    UList* pList;
    AudTrack* pTrack;

    bSorted = pSource->pSound->n3 & 1;
    pList = &lbl_801F1868[bSorted];
    if (pPool->nFree == 0) {
        pTrack = (AudTrack*)lbl_801F1868[1].pTail;
        if (lbl_801F1868[1].nCount == 0) return NULL;
        if (bSorted == 1) {
            while (pTrack != NULL) {
                if (pTrack->nState != 1 && !(pTrack->pTmpl->n0 & 8) && pTrack->f48 < fPriority) break;
                pTrack = (AudTrack*)pTrack->link.pPrev;
            }
        }
        if (pTrack != NULL) {
            fn_800A9D7C(pTrack);
        } else {
            return NULL;
        }
    }
    pTrack = fn_800AE1AC(pPool);
    pTrack->pTmpl = pTmpl;
    pTrack->pSource = pSource;
    pSource->apTracks[nChannel] = pTrack;
    pTrack->f40 = pTmpl->fC;
    pTrack->f44 = 1.0f;
    pTrack->f48 = fPriority;
    pTrack->f4C = 1.0f;
    pTrack->f50 = 0.0f;
    pTrack->nChannel = nChannel;
    pTrack->nState = 1;
    pTrack->n5D = 0;
    pTrack->bits.n = 0;
    pTrack->bits.b.bSorted = bSorted;
    pTrack->bits.b.b5 = pSource->nSound >= 0;
    pTrack->params.flags.n = 0;
    fn_80005AE8(pTrack->apVoices, 0, sizeof(pTrack->apVoices));
    if (bSorted == 0) {
        fn_800ADEC8(pList, &pTrack->link);
    } else {
        fn_800A9808(pTrack);
    }
    if (!(pTmpl->n0 & 8)) {
        fn_800AADE8(pTrack);
    } else {
        fn_800ABC34(pTrack);
    }
    return pTrack;
}

// Frees a track: stops its voices at once and gives it back to the pool.
s32 fn_800A9D7C(AudTrack* pTrack) {
    UPool* pPool;
    UList* pList;
    AudTrackTmpl* pTmpl;
    AudSource* pSource;

    pPool = &lbl_80282098;
    if (pTrack == NULL) return 0;
    pSource = pTrack->pSource;
    pList = &lbl_801F1868[pTrack->bits.b.bSorted];
    pTmpl = pTrack->pTmpl;
    if (!pTrack->bits.b.bDetached) {
        pTrack->pSource->apTracks[pTrack->nChannel] = NULL;
        pTrack->bits.b.bDetached = 1;
    }
    fn_800AA1B8(pTrack, 1);
    if (!(pTrack->pTmpl->n0 & 8)) {
        fn_800AAE70(pTrack);
    } else {
        Stm_Exit(pTrack);
    }
    if (pTmpl->n0 & 0x40) {
        fn_800A8584(pSource, pTmpl->n6, 0);
    }
    pTrack->pTmpl = NULL;
    pTrack->nState = 0;
    pTrack->pSource = NULL;
    fn_800ADF6C(pList, &pTrack->link);
    fn_800AE1DC(pPool, pTrack);
    return 0;
}

// Starts, stops or restarts channel nChannel of a source as its priority changes. A track at
// priority 0 or less is stopped; one that becomes audible is started. bOn and bOff are the
// caller's requests, which only count for the templates flagged 0x04 (and not 0x01).
void fn_800A9E7C(AudSource* pSource, AudTrack* pTrack, AudTrackTmpl* pTmpl, u8 nChannel, u8 bOn,
                 u8 bOff, f32 fPriority) {
    u8 bPlaying;
    u8 bSwitch;
    u8 bRetrigger;
    u8 bResort;
    u8 bAudible;
    u8 bStart;
    u8 bStop;
    u8 bKeep;
    u8 n64;
    u8 n68;

    bKeep = 0;
    if (pTrack != NULL && pTrack->nState == 2) return;
    bPlaying = pTrack != NULL && pTrack->nState > 2;
    bSwitch = (pTmpl->n0 & 4) && !(pTmpl->n0 & 1);
    bRetrigger = (pTmpl->n0 & 0x10) == 0;
    bResort = bPlaying;
    bAudible = fPriority > 0.0f;
    bStart = bAudible &&
             ((!bPlaying && ((!bRetrigger && (!bSwitch || (bSwitch && bOn && !bOff))) ||
                             (bRetrigger && (!bSwitch || (bSwitch && bOn))))) ||
              (bPlaying && bRetrigger && bSwitch && bOn));
    bStop = (!bAudible && bPlaying) ||
            (bAudible && bPlaying && bSwitch &&
             ((!bRetrigger && bOff && !bOn) || (bRetrigger && (bOn || bOff))));
    if (bStop && bStart && !(pTmpl->n0 & 8)) {
        n64 = pTrack->u.seq.n64;
        bKeep = 1;
        n68 = pTrack->u.seq.n68;
    }
    if (bStop) {
        fn_800AA118(pTrack);
        pTrack = NULL;
        bResort = 0;
    }
    if (bStart) {
        bResort = 0;
        if (pTrack == NULL) {
            pTrack = fn_800A9BC8(pSource, pTmpl, nChannel, fPriority);
        }
        if (pTrack != NULL) {
            fn_800AA0D8(pTrack);
        }
    }
    if (bKeep && pTrack != NULL) {
        pTrack->u.seq.n64 = n64;
        pTrack->u.seq.n68 = n68;
    }
    if (bResort && pTrack->bits.b.bSorted == 1) {
        pTrack->f48 = fPriority;
        fn_800ADF6C(&lbl_801F1868[1], &pTrack->link);
        fn_800A9808(pTrack);
    }
}

// Starts a track.
void fn_800AA0D8(AudTrack* pTrack) {
    pTrack->params.flags.n = 0;
    if (!(pTrack->pTmpl->n0 & 8)) {
        fn_800AAE90(pTrack);
        return;
    }
    Stm_Start(pTrack);
}

// Stops a track: its voices end on their own, and it leaves its source.
void fn_800AA118(AudTrack* pTrack) {
    if (pTrack->nState > 3) {
        fn_800AA1B8(pTrack, 0);
    } else {
        pTrack->nState = 2;
    }
    if (!pTrack->bits.b.bDetached) {
        pTrack->pSource->apTracks[pTrack->nChannel] = NULL;
        pTrack->bits.b.bDetached = 1;
    }
    if (!(pTrack->pTmpl->n0 & 8)) {
        fn_800AAEEC(pTrack);
        return;
    }
    fn_800ABD7C(pTrack);
}

// Stops a track's voices: at once (bNow == 1), or by letting them end, in which case the track
// stays in state 3 until the last one calls back (fn_800AA400).
void fn_800AA1B8(AudTrack* pTrack, int bNow) {
    AudVoice** ppVoice;
    AudVoice** ppEnd;
    u8 bNone;

    ppVoice = pTrack->apVoices;
    ppEnd = &pTrack->apVoices[pTrack->pTmpl->n2];
    if (bNow == 1) {
        for (; ppVoice < ppEnd; ppVoice++) {
            if (*ppVoice != NULL) {
                Voc_Delete(*ppVoice);
                *ppVoice = NULL;
            }
        }
        pTrack->nState = 2;
        pTrack->n5D = 0;
        return;
    }
    if (pTrack->nState != 3) {
        bNone = 1;
        for (; ppVoice < ppEnd; ppVoice++) {
            if (*ppVoice != NULL) {
                Voc_Stop(*ppVoice);
                bNone = 0;
            }
        }
        if (bNone) {
            pTrack->nState = 2;
        } else {
            pTrack->nState = 3;
        }
    }
}

// Advances a track by one tick; returns 0 once it has ended.
u8 fn_800AA2A4(AudTrack* pTrack) {
    pTrack->f4C += pTrack->f50;
    return !(pTrack->pTmpl->n0 & 8) ? fn_800AAEFC(pTrack) : Stm_Tick(pTrack);
}

void fn_800AA2EC(AudTrack* pTrack, u8 n, u8 bCheck) {
    fn_800AB118(pTrack, n, bCheck);
}

void fn_800AA30C(AudTrack* pTrack, u8 n) {
    fn_800AA444(pTrack, n);
}

void fn_800AA32C(AudTrack* pTrack, u8 n) {
    fn_800AB14C(pTrack, n);
}

// Renders a track: its volume through its curve, then its pan and volume per voice.
void fn_800AA34C(AudTrack* pTrack) {
    AudSource* pSource;
    AudPlayList* pList;
    f32 fCurve;
    f32 fVolume;

    pSource = pTrack->pSource;
    pList = pTrack->pTmpl->data.pPlayList;
    if (pList == NULL) return;
    fCurve = fn_800AA44C(pList->n3);
    fVolume = fn_800A85FC(pTrack->f44, fCurve);
    if (pTrack->bits.b.bSorted == 1) {
        fn_800A9590(pSource, pTrack, fVolume);
        return;
    }
    fn_800A96DC(pSource, pTrack, fVolume);
}

void fn_800AA3D4(AudTrackTmpl* pTmpl) {
    if (!(pTmpl->n0 & 8)) {
        fn_800AB1B8(pTmpl);
    }
}

// A voice's end callback: it leaves its track, and a stopping track with no voices left is stopped.
void fn_800AA400(AudVoice* pVoice, int nReason) {
    AudTrack* pTrack;

    pTrack = pVoice->pUser;
    pTrack->apVoices[(u8)pVoice->nIndex] = NULL;
    if (--pTrack->n5D == 0 && pTrack->nState == 3) {
        pTrack->nState = 2;
    }
}

void fn_800AA444(AudTrack* pTrack, u8 n) {
    pTrack->u.seq.n64 = n;
}

// The volume curve nCurve's value: 0 for a flat curve.
f32 fn_800AA44C(u8 nCurve) {
    if (fn_800AA498(nCurve)) {
        return 0.0f;
    }
    return lbl_801F17D0[nCurve];
}

u8 fn_800AA498(u8 nCurve) {
    return (lbl_80282060 & (1 << nCurve)) != 0;
}
