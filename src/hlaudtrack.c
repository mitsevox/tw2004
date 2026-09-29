// hlaudtrack.c (TW06's name: golf/audio/engine/hl/hlaudtrack.c; TW07's HLAudTrack.c has the same
// functions in the same order, plus Trk_ExitModule, Trk_FreeAllPerfs and Trk_SetTempo): the sound
// engine's tracks, "perfs" to EA. A pool of 32 tracks, each playing one track of a sound source's
// sound, either sequenced (hlaudtrackseq.c) or streamed from disc (hlaudtrackstm.c): allocated,
// started, ticked and rendered once a frame (Trk_Cycle), stopped and freed. The tracks of placed
// (3D) sources sit in a list sorted on their distance attenuation, so the quietest can be stolen
// when the pool runs out. Its last three functions are header inlines of TW07's compiled out of
// line (Seq_SelectVariation, Mas_GetSubmix, Mas_IsChanMuted). Its extent is its data: it is the
// first to use the .bss at 0x801F1868 and the only user of the .sdata2 block 0x80283FD0-0x80283FD8.

#include "core/audtrack.h"

UList gTrkPerfLists[2];                // the tracks in use: [0] in start order, [1] placed ones
                                        // sorted on f48, loudest first (InsertSortWorldPerf)

AudTrack* gTrkPerfs;                    // the 32 tracks (Trk_InitModule)
UPool gTrkPerfPool;                     // the free ones

// Puts a placed (3D) track into the sorted track list, on f48, its distance attenuation (TW07's
// distAttn), loudest first: before the first quieter track, else at the end. Trk_AllocPerf steals
// from the quiet end.
void InsertSortWorldPerf(AudTrack* pTrack) {
    AudTrack* pAt;
    UList* pList;

    pAt = (AudTrack*)gTrkPerfLists[1].pHead;
    pList = &gTrkPerfLists[1];
    if (pAt == NULL) {
        UList_PushTail(pList, &pTrack->link);
    }
    while (pAt != NULL) {
        if (pAt->f48 < pTrack->f48) {
            UList_InsertAt(pList, &pTrack->link, &pAt->link);
            return;
        }
        if (pAt->link.pNext == NULL) {
            UList_PushTail(pList, &pTrack->link);
            return;
        }
        pAt = (AudTrack*)pAt->link.pNext;
    }
}

// Sets the tracks up at start-up (Aud_InitOnce): 32 tracks from the audio memory stack, numbered
// and idle, their free pool and the two track lists, then the sequencer (Seq_InitModule) and the
// streamer (Stm_InitModule). Returns 0 when a step fails.
u8 Trk_InitModule(void) {
    u8 bOk;
    u8 i;

    bOk = 0;
    gTrkPerfs = AudMem_Alloc(32 * sizeof(AudTrack));
    if (gTrkPerfs != NULL) {
        Mem_set(gTrkPerfs, 0, 32 * sizeof(AudTrack));
        for (i = 0; i < 32; i++) {
            gTrkPerfs[i].nIndex = i;
            gTrkPerfs[i].nState = 0;
        }
        UPool_Init(&gTrkPerfPool, gTrkPerfs, 32, sizeof(AudTrack));
        UList_Reset(&gTrkPerfLists[0], 32);
        UList_Reset(&gTrkPerfLists[1], 32);
        bOk = Seq_InitModule();
        if (bOk) {
            bOk = Stm_InitModule();
        }
    }
    return bOk;
}

// A new sound session (Ses_Init): frees every track in both lists and returns 1. The session
// numbers are not used. TW07 has the loop as Trk_FreeAllPerfs.
u8 Trk_InitSession(u8 a, u8 b) {
    s32 i;
    UList* pList;
    AudTrack* pTrack;

    i = 0;
    pList = gTrkPerfLists;
    do {
        pTrack = (AudTrack*)pList->pHead;
        while (pTrack != NULL) {
            Trk_FreePerf(pTrack);
            // the free only reuses the link's first word, so pNext is still there
            pTrack = (AudTrack*)pTrack->link.pNext;
        }
        i++;
        pList++;
    } while (i < 2);
    return 1;
}

// Empty (TW07's is too); called when hlaudmovie.c tears the sound session down.
void Trk_ExitSession(void) {
}

// Once a frame (Aud_EmiCycle), when the engine is set up and bank 0 and its samples are loaded (0xD
// in gSesFlags): walks both track lists. A track allocated but never started (state 1) is freed
// on its second frame; any other is ticked (Trk_Tick) and rendered (Trk_Render), or freed once its
// tick says it has stopped. While the sound is paused (0x40) only the tracks on volume curve 0
// tick, and not those of sound 8.
void Trk_Cycle(void) {
    s32 i;
    UList* pList;
    AudTrack* pTrack;
    AudTrackTmpl* pTmpl;

    pList = gTrkPerfLists;
    if ((gSesFlags & 0xD) != 0xD) return;
    i = 0;
    do {
        pTrack = (AudTrack*)pList->pHead;
        while (pTrack != NULL) {
            pTmpl = pTrack->pTmpl;
            if (pTrack->nState == 1) {
                if (pTrack->bits.b.bTicked) {
                    Trk_FreePerf(pTrack);
                } else {
                    pTrack->bits.b.bTicked = 1;
                }
            } else if (!(gSesFlags & 0x40) ||
                       (pTmpl->data.pPlayList->n3 == 0 && pTrack->pSource->nSound != 8)) {
                if (Trk_Tick(pTrack)) {
                    Trk_Render(pTrack);
                } else {
                    Trk_FreePerf(pTrack);
                }
            }
            pTrack = (AudTrack*)pTrack->link.pNext;
        }
        i++;
        pList++;
    } while (i < 2);
}

// Takes a track from the pool for channel nChannel of a source and sets it up (state 1, allocated;
// volume and pitch 1, no pitch ramp) in the plain list or, for a placed source, the sorted one;
// then Seq_Init or Stm_Init. fPriority is the distance attenuation (TW07's distAttn). When the pool
// is empty it steals the quietest sorted track: for a placed source only one quieter than fPriority
// that is past state 1 and not streamed, for any other source the last one. Returns NULL when there
// is none to take.
AudTrack* Trk_AllocPerf(AudSource* pSource, AudTrackTmpl* pTmpl, u8 nChannel, f32 fPriority) {
    UPool* const pPool = &gTrkPerfPool;
    s32 bSorted;
    UList* pList;
    AudTrack* pTrack;

    bSorted = pSource->pSound->n3 & 1;
    pList = &gTrkPerfLists[bSorted];
    if (pPool->nFree == 0) {
        pTrack = (AudTrack*)gTrkPerfLists[1].pTail;
        if (gTrkPerfLists[1].nCount == 0) return NULL;
        if (bSorted == 1) {
            while (pTrack != NULL) {
                if (pTrack->nState != 1 && !(pTrack->pTmpl->n0 & 8) && pTrack->f48 < fPriority) break;
                pTrack = (AudTrack*)pTrack->link.pPrev;
            }
        }
        if (pTrack != NULL) {
            Trk_FreePerf(pTrack);
        } else {
            return NULL;
        }
    }
    pTrack = UPool_Alloc(pPool);
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
    Mem_set(pTrack->apVoices, 0, sizeof(pTrack->apVoices));
    if (bSorted == 0) {
        UList_PushTail(pList, &pTrack->link);
    } else {
        InsertSortWorldPerf(pTrack);
    }
    if (!(pTmpl->n0 & 8)) {
        Seq_Init(pTrack);
    } else {
        Stm_Init(pTrack);
    }
    return pTrack;
}

// Frees a track: takes it out of its source, deletes its voices at once, resets its sequencer
// (Seq_Exit) or stream (Stm_Exit) state, tells the emitter (Emi_TrackCallback with 0) when the
// template asks for it (n0 & 0x40), and gives it back to the pool. NULL is ignored. Always returns
// 0 (TW07's returns a TPerf*).
s32 Trk_FreePerf(AudTrack* pTrack) {
    UPool* pPool;
    UList* pList;
    AudTrackTmpl* pTmpl;
    AudSource* pSource;

    pPool = &gTrkPerfPool;
    if (pTrack == NULL) return 0;
    pSource = pTrack->pSource;
    pList = &gTrkPerfLists[pTrack->bits.b.bSorted];
    pTmpl = pTrack->pTmpl;
    if (!pTrack->bits.b.bDetached) {
        pTrack->pSource->apTracks[pTrack->nChannel] = NULL;
        pTrack->bits.b.bDetached = 1;
    }
    Trk_StopAllVoices(pTrack, 1);
    if (!(pTrack->pTmpl->n0 & 8)) {
        Seq_Exit(pTrack);
    } else {
        Stm_Exit(pTrack);
    }
    if (pTmpl->n0 & 0x40) {
        Emi_TrackCallback(pSource, pTmpl->n6, 0);
    }
    pTrack->pTmpl = NULL;
    pTrack->nState = 0;
    pTrack->pSource = NULL;
    UList_DeleteAt(pList, &pTrack->link);
    UPool_Free(pPool, pTrack);
    return 0;
}

// Called by Emi_UpdInstance for each track of a source: starts, stops or restarts channel nChannel
// as its distance attenuation fPriority changes. Nothing while the track is stopped and waiting
// (state 2). At 0 or below a playing track stops; above 0 an idle one starts (allocated first when
// there is none). Templates with n0 & 4 (and not 1) are switched by hand: bOn and bOff are the
// caller's start and stop requests, and bOn on a playing one restarts it unless n0 & 0x10 is set. A
// sequenced track stopped and started in one go keeps its variation and set. A playing placed track
// that neither starts nor stops is re-sorted on the new fPriority.
void Trk_UpdatePerf(AudSource* pSource, AudTrack* pTrack, AudTrackTmpl* pTmpl, u8 nChannel, u8 bOn,
                 u8 bOff, f32 fPriority) {
    u8 bPlaying;
    u8 bManual;
    u8 bRetrigger;
    u8 bUpdate;
    u8 bAudible;
    u8 bStart;
    u8 bStop;
    u8 bCarryOver;
    u8 nCarryVar;
    u8 nCarryRange;

    bCarryOver = 0;
    if (pTrack != NULL && pTrack->nState == 2) return;
    bPlaying = pTrack != NULL && pTrack->nState > 2;
    bManual = (pTmpl->n0 & 4) && !(pTmpl->n0 & 1);
    bRetrigger = (pTmpl->n0 & 0x10) == 0;
    bUpdate = bPlaying;
    bAudible = fPriority > 0.0f;
    bStart = bAudible &&
             ((!bPlaying && ((!bRetrigger && (!bManual || (bManual && bOn && !bOff))) ||
                             (bRetrigger && (!bManual || (bManual && bOn))))) ||
              (bPlaying && bRetrigger && bManual && bOn));
    bStop = (!bAudible && bPlaying) ||
            (bAudible && bPlaying && bManual &&
             ((!bRetrigger && bOff && !bOn) || (bRetrigger && (bOn || bOff))));
    if (bStop && bStart && !(pTmpl->n0 & 8)) {
        nCarryVar = pTrack->u.seq.n64;
        bCarryOver = 1;
        nCarryRange = pTrack->u.seq.n68;
    }
    if (bStop) {
        Trk_Stop(pTrack);
        pTrack = NULL;
        bUpdate = 0;
    }
    if (bStart) {
        bUpdate = 0;
        if (pTrack == NULL) {
            pTrack = Trk_AllocPerf(pSource, pTmpl, nChannel, fPriority);
        }
        if (pTrack != NULL) {
            Trk_Start(pTrack);
        }
    }
    if (bCarryOver && pTrack != NULL) {
        pTrack->u.seq.n64 = nCarryVar;
        pTrack->u.seq.n68 = nCarryRange;
    }
    if (bUpdate && pTrack->bits.b.bSorted == 1) {
        pTrack->f48 = fPriority;
        UList_DeleteAt(&gTrkPerfLists[1], &pTrack->link);
        InsertSortWorldPerf(pTrack);
    }
}

// Starts a track: clears its pending voice settings, then starts it as a sequenced (Seq_Start) or a
// streamed (Stm_Start) track.
void Trk_Start(AudTrack* pTrack) {
    pTrack->params.flags.n = 0;
    if (!(pTrack->pTmpl->n0 & 8)) {
        Seq_Start(pTrack);
        return;
    }
    Stm_Start(pTrack);
}

// Stops a track: a filling or playing one (state 4 and up) lets its voices end, any other is marked
// stopped (state 2); it leaves its source, and its sequencer (Seq_Stop) or stream (Stm_Stop) is
// stopped. Trk_Cycle frees it once its tick reports it stopped.
void Trk_Stop(AudTrack* pTrack) {
    if (pTrack->nState > 3) {
        Trk_StopAllVoices(pTrack, 0);
    } else {
        pTrack->nState = 2;
    }
    if (!pTrack->bits.b.bDetached) {
        pTrack->pSource->apTracks[pTrack->nChannel] = NULL;
        pTrack->bits.b.bDetached = 1;
    }
    if (!(pTrack->pTmpl->n0 & 8)) {
        Seq_Stop(pTrack);
        return;
    }
    Stm_Stop(pTrack);
}

// Stops a track's voices: at once (bNow == 1), or by letting them end, in which case the track
// stays in state 3 until the last one calls back (Trk_VoiceEndCB).
void Trk_StopAllVoices(AudTrack* pTrack, int bNow) {
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

// Advances a track by one frame: its pitch (f4C) by its pitch ramp (f50), then its sequencer
// (Seq_Tick) or stream (Stm_Tick). Returns 0 once it has stopped, for Trk_Cycle to free it.
u8 Trk_Tick(AudTrack* pTrack) {
    pTrack->f4C += pTrack->f50;
    return !(pTrack->pTmpl->n0 & 8) ? Seq_Tick(pTrack) : Stm_Tick(pTrack);
}

// Asks a sequenced track to run event n next (Seq_Step); with bCheck (TW07's debounce), not when it
// is on that event already.
void Trk_Step(AudTrack* pTrack, u8 n, u8 bCheck) {
    Seq_Step(pTrack, n, bCheck);
}

// Sets a sequenced track's variation (Seq_SelectVariation).
void Trk_SelectVariation(AudTrack* pTrack, u8 n) {
    Seq_SelectVariation(pTrack, n);
}

// Switches a sequenced track to set n of its variations (Seq_SetVariationRange).
void Trk_SetVariationRange(AudTrack* pTrack, u8 n) {
    Seq_SetVariationRange(pTrack, n);
}

// Once a frame after its tick: the track's volume (f44) times its submix's (Mas_GetSubmix of the
// play list's or bank's curve) goes with the track to hlaudmovie.c's render for placed (sorted)
// tracks or its stereo render for the others, which set each voice (TW07 inlines TrkRender3D and
// TrkRenderStereo here). Nothing for a template without data.
void Trk_Render(AudTrack* pTrack) {
    AudSource* pSource;
    AudPlayList* pList;
    f32 fCurve;
    f32 fVolume;

    pSource = pTrack->pSource;
    pList = pTrack->pTmpl->data.pPlayList;
    if (pList == NULL) return;
    fCurve = Mas_GetSubmix(pList->n3);
    fVolume = audfrac_Mul(pTrack->f44, fCurve);
    if (pTrack->bits.b.bSorted == 1) {
        TrkRender3D(pSource, pTrack, fVolume);
        return;
    }
    TrkRenderStereo(pSource, pTrack, fVolume);
}

// Prepares a track template of a sound just loaded (Emi_CheckTemplate): a sequenced one's events
// through Seq_Check; a streamed one needs nothing.
void Trk_Check(AudTrackTmpl* pTmpl) {
    if (!(pTmpl->n0 & 8)) {
        Seq_Check(pTmpl);
    }
}

// A voice's end callback (streamed voices take it directly, sequenced ones through VoiceEndCB): the
// voice leaves its track, and a stopping track (state 3) whose last voice has ended is stopped
// (state 2).
void Trk_VoiceEndCB(AudVoice* pVoice, int nReason) {
    AudTrack* pTrack;

    pTrack = pVoice->pUser;
    pTrack->apVoices[(u8)pVoice->nIndex] = NULL;
    if (--pTrack->n5D == 0 && pTrack->nState == 3) {
        pTrack->nState = 2;
    }
}

// Sets a sequenced track's variation (u.seq.n64). TW07's inline from HLAudTrackSeq.h, compiled out
// of line here.
void Seq_SelectVariation(AudTrack* pTrack, u8 n) {
    pTrack->u.seq.n64 = n;
}

// Submix nCurve's volume (the volume curve a play list or bank names), 0 while it is muted. TW07's
// inline from HLAudMaster.h, compiled out of line here.
f32 Mas_GetSubmix(u8 nCurve) {
    if (Mas_IsChanMuted(nCurve)) {
        return 0.0f;
    }
    return gMasSubmixVolumes[nCurve];
}

// Whether submix nCurve is muted (its bit in HLAudMaster.c's mute mask).
u8 Mas_IsChanMuted(u8 nCurve) {
    return (gMasMuteMask & (1 << nCurve)) != 0;
}
