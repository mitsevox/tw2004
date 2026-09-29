// hlaudtrackseq.c (TW06's name: golf/audio/engine/hl/hlaudtrackseq.c; TW07's HLAudTrackSeq.c has
// its functions in the same order): the sequencer, the tracks that play events (EA's track
// commands) instead of a stream. A template holds sets of variations of events; each frame the
// track waits out the next event's delay, then runs it through the handler table Seq_InitModule
// fills (gSeqCmdHandlers, OnNoOp .. OnRvbWetAttn). A stepped template (n0 & 1) runs only the
// events it is asked for (Seq_Step). Notes take a voice per channel, stealing one when all are
// busy. ResetSequencerPerf and SetVarCmdBounds, inlines in TW07, are functions here; four small
// helpers sit at its end (Aud_RandomBelow, Ses_IsSessionZero, Ses_GetInstrumentTone,
// Mas_GetUpdateRateScale). Its extent is its data: OnKeyOn is the first to use its .sdata2 block
// (0x80283FD8-0x80283FF8), and its handlers run up to 0x800AAD14.

#include "core/audtrack.h"

AudSeqHandler gSeqCmdHandlers[13];     // the event handlers by event type (Seq_InitModule)

// Picks a sequenced track's next variation the way its template's n1 says: 4 and up the next in
// order, 2 at random but not the same one twice, 3 at random but not the one the template played
// last (n9), others at random; the pick wraps below n7. A template with a single variation (n4 0 or
// 1) always plays 0. A set forced on the template (nA, not 0xFF) becomes the track's set first.
void AutoSelectVariation(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;
    u8 nNext;

    pTmpl = pTrack->pTmpl;
    if (pTmpl->n4 <= 1) {
        pTrack->u.seq.n64 = 0;
        return;
    }
    if (pTmpl->nA != 0xFF) {
        pTrack->u.seq.n68 = pTmpl->nA;
    }
    if (pTmpl->n1 >= 4) {
        nNext = pTrack->u.seq.n64 + 1;
    } else {
        nNext = Aud_RandomBelow(pTmpl->n7);
        switch (pTmpl->n1) {
        case 2:
            if (nNext == pTrack->u.seq.n64) {
                nNext++;
            }
            break;
        case 3:
            if (nNext == pTmpl->n9) {
                nNext++;
            }
            break;
        }
    }
    if (nNext >= pTmpl->n7) {
        nNext -= pTmpl->n7;
    }
    pTrack->u.seq.n64 = nNext;
    pTmpl->n9 = nNext;
}

// The sequenced voices' end callback (OnKeyOn's): Trk_VoiceEndCB first, then the channel forgets
// its note, unless the voice was stolen (nReason 1, from Voc_Alloc) while playing a looping tone
// (bLoops, the request's loop flag): CheckForStolenLoopers plays that note again.
void VoiceEndCB(AudVoice* pVoice, int nReason) {
    AudTrack* pTrack;
    u8 nChannel;

    pTrack = pVoice->pUser;
    nChannel = pVoice->nIndex;
    Trk_VoiceEndCB(pVoice, nReason);
    if (nReason != 1 || !pVoice->flags.b.bLoops) {
        pTrack->u.seq.apEvents[nChannel] = NULL;
    }
}

// Plays again each note whose looping voice was stolen: a channel with no voice that still holds
// its note (VoiceEndCB keeps it). Seq_Tick calls it on every 16th audio frame.
void CheckForStolenLoopers(AudTrack* pTrack) {
    AudVoice** ppVoice;
    AudVoice** ppEnd;
    AudSeqEvent** ppEvent;
    AudSeqEvent* pEvent;

    ppVoice = pTrack->apVoices;
    ppEvent = pTrack->u.seq.apEvents;
    ppEnd = ppVoice + pTrack->pTmpl->n2;
    for (; ppVoice < ppEnd; ppVoice++, ppEvent++) {
        pEvent = *ppEvent;
        if (*ppVoice == NULL && pEvent != NULL) {
            *ppEvent = NULL;
            OnKeyOn(pEvent, pTrack);
        }
    }
}

// Event 0: does nothing.
void OnNoOp(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Event 1, the end of a variation. A looping template (n0 & 2) goes on, unless the track is
// stopping: a new variation when it picks them (n1, AutoSelectVariation), from event n3 of it,
// bNewVariation telling Seq_Tick to fetch the new events, and Emi_TrackCallback with 1 when the
// template asks (n0 & 0x80). Any other template's track stops (state 3).
void OnEnd(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    if (pTmpl->n0 & 2) {
        if (pTrack->nState != 3) {
            if (pTmpl->n1 != 0) {
                AutoSelectVariation(pTrack);
            }
            pTrack->u.seq.n66 = pEvent->n3;
            pTrack->bits.b.bNewVariation = 1;
            if (pTmpl->n0 & 0x80) {
                Emi_TrackCallback(pTrack->pSource, pTmpl->n6, 1);
            }
        }
    } else {
        pTrack->nState = 3;
    }
}

// Event 2, a note: tone n3 of the bank at velocity n4, its volume n4 << 7 through the track's
// distance attenuation (fDistAttn) and the bank's submix; nothing when that is 0. From the channel
// after the last note's, it takes the first channel that is free (no voice, and no stolen looping
// note waiting to be played again) or whose voice has a lower steal level or is ending (level 0),
// deleting that voice; none, no note. Looping tones ask for steal level 1, others 0; outside
// session 0, sound 1's track 0 asks 2. The voice starts with the track's pending settings (pitch 1
// unless one is set), which are then cleared.
void OnKeyOn(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrackTmpl* pTmpl = pTrack->pTmpl;
    f32 fAttn;
    s16 nVolume;
    AudSeqTone* pTone;
    u8 bLoops;
    AudVoiceRequest request;
    u8 nChannel;
    u8 i;
    AudVoice* pVoice;
    AudVoiceParams* pParams;
    f32 f;

    // fake match: the (u8) changes nothing; it gives EA's instruction order.
    pTone = Ses_GetInstrumentTone(pTmpl->data.pBank, (u8)pEvent->n3);
    fAttn = audfrac_Mul(pTrack->fDistAttn, Mas_GetSubmix(pTmpl->data.pBank->n3));
    nVolume = audfrac_Mul((f32)(pEvent->n4 << 7), fAttn);
    bLoops = pTone->n10 & 1;
    if (nVolume == 0) return;
    if (pTone == NULL) return;
    nChannel = pTrack->u.seq.n65;
    request.nStealLevel = bLoops != 0;
    if (!Ses_IsSessionZero() && pTrack->pSource->nSound == 1 && pTrack->nChannel == 0) {
        request.nStealLevel = 2;
    }
    for (i = 0; i < pTmpl->n2;) {
        pVoice = pTrack->apVoices[nChannel];
        if (pVoice == NULL) {
            if (pTrack->u.seq.apEvents[nChannel] == NULL) break;
        } else if (request.nStealLevel > pVoice->nStealLevel || pVoice->nStealLevel == 0) {
            Voc_Delete(pVoice);
            pTrack->u.seq.apEvents[nChannel] = NULL;
            pTrack->apVoices[nChannel] = NULL;
            pTrack->nVoices--;
            break;
        }
        nChannel++;
        if (nChannel >= pTmpl->n2) {
            nChannel = 0;
        }
        i++;
    }
    if (i >= pTmpl->n2) return;
    request.pfnCallback = VoiceEndCB;
    pParams = &pTrack->params;
    f = 1.0f;
    request.pUser = pTrack;
    request.nIndex = nChannel;
    request.flags.n = 0;
    request.flags.b.bNoReverb = (pTrack->pSource->pSound->n3 & 4) || (pTrack->pTmpl->n0 & 0x20);
    request.flags.b.bLoops = bLoops;
    request.nPriority = nVolume;
    request.n2 = 0x40;
    request.n3 = 0x7F;
    pVoice = Voc_Alloc(&request);
    if (pVoice == NULL) return;
    pVoice->pTone = pTone;
    if (pParams->flags.b.bPitch) {
        f = pParams->fPitch;
    }
    Voc_Start(pVoice, pParams, pEvent->n4, f);
    pTrack->nVoices++;
    pTrack->apVoices[nChannel] = pVoice;
    pTrack->u.seq.apEvents[nChannel] = pEvent;
    pTrack->u.seq.n65 = nChannel;
    if (++pTrack->u.seq.n65 >= pTmpl->n2) {
        pTrack->u.seq.n65 = 0;
    }
    pParams->flags.n = 0;
}

// fake match: an identity read (u8 -> s8 -> u8 gives back the same byte); it gives EA's register
// order.
static inline u8 fn_800AA9EC_Read(s8 n) { return n; }

// Event 3: lets the voices playing tone n3 end, starting from the channel of the last note.
void OnKeyOff(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudSeqTone* pTone;
    AudTrackTmpl* pTmpl;
    s8 nChannel;
    u8 i;           // fake match: declared after nChannel for EA's register order
    AudVoice* pVoice;

    pTmpl = pTrack->pTmpl;
    pTone = Ses_GetInstrumentTone(pTmpl->data.pBank, pEvent->n3);
    if (pTone == NULL) return;
    nChannel = fn_800AA9EC_Read(pTrack->u.seq.n65) - 1;
    if (nChannel < 0) {
        nChannel = 0;
    }
    for (i = 0; i < pTmpl->n2; i++, nChannel++) {
        if (nChannel >= pTmpl->n2) {
            nChannel -= (s8)pTmpl->n2;     // EA sign-extends the count here (extsb)
        }
        pVoice = pTrack->apVoices[nChannel];
        if (pVoice != NULL && pVoice->pTone == pTone) {
            Voc_Stop(pVoice);
        }
    }
}

// Event 4: empty (TW07's is too).
void OnPitchBend(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Event 5: sets the pitch ramp (fPitchRamp, added to the pitch every frame) of the source's track
// n3 (0xFF: this track) to n4 / 65536, allocating that track when the source has none.
void OnPitchRamp(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrack* pTarget;
    AudSource* pSource;
    u8 n;

    n = (u8)pEvent->n3;     // fake match: the (u8) changes nothing; it gives EA's instructions
    if (n == 0xFF) {
        pTarget = pTrack;
    } else {
        pSource = pTrack->pSource;
        pTarget = pSource->apTracks[n];
        if (pTarget == NULL) {
            pTarget = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[n], n, pTrack->fDistAttn);
        }
    }
    if (pTarget != NULL) {
        pTarget->fPitchRamp = (u32)pEvent->n4 / 65536.0f;
    }
}

// Event 6: switches the source's track n3 on (n4 non-zero: its bit in u0) or off (its bit in u1).
void OnTrackStatus(AudSeqEvent* pEvent, AudTrack* pTrack) {
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

// Event 7: gives the source's streamed track n3 play list n4 (Stm_SetPlayList), allocating that
// track when the source has none.
void OnTrackSetPlayList(AudSeqEvent* pEvent, AudTrack* pTrack) {
    int n;
    AudSource* pSource;
    AudTrack* pTarget;
    u8 nPlayList;

    n = pEvent->n3;
    pSource = pTrack->pSource;
    nPlayList = pEvent->n4;
    pTarget = pSource->apTracks[n];
    if (pTarget == NULL) {
        pTarget = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[n], n, pTrack->fDistAttn);
    }
    if (pTarget != NULL) {
        Stm_SetPlayList(pTarget, nPlayList);
    }
}

// Event 8: gives the source's streamed track n3 stream n4 (Stm_SetStream, mode 0), allocating that
// track when the source has none.
void OnTrackSetStream(AudSeqEvent* pEvent, AudTrack* pTrack) {
    int n;
    AudSource* pSource;
    AudTrack* pTarget;
    u16 nStream;

    n = pEvent->n3;
    pSource = pTrack->pSource;
    nStream = pEvent->n4;
    pTarget = pSource->apTracks[n];
    if (pTarget == NULL) {
        pTarget = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[n], n, pTrack->fDistAttn);
    }
    if (pTarget != NULL) {
        Stm_SetStream(pTarget, nStream, 0);
    }
}

// Event 9: the next note's pitch, n4 / 65536.
void OnModPitch(AudSeqEvent* pEvent, AudTrack* pTrack) {
    pTrack->params.fPitch = (u32)pEvent->n4 / 65536.0f;
    pTrack->params.flags.b.bPitch = 1;
}

// Event 10: the next note's envelope value anAdsr[n3] (n3 0 attack, 1 decay) = n4; bAttack or
// bDecay says which was set.
void OnModADSRVol(AudSeqEvent* pEvent, AudTrack* pTrack) {
    u8 n;

    n = pEvent->n3;
    pTrack->params.anAdsr[n] = pEvent->n4;
    pTrack->params.flags.b.bAttack = n == 0;
    pTrack->params.flags.b.bDecay = n == 1;
}

// Event 11: the next note's start offset (nStartOffset) = n4.
void OnModStartOffset(AudSeqEvent* pEvent, AudTrack* pTrack) {
    pTrack->params.nStartOffset = pEvent->n4;
    pTrack->params.flags.b.bStartOffset = 1;
}

// Event 12: empty here (TW07's OnRvbWetAttn has a body).
void OnRvbWetAttn(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Fills the sequencer's event handler table, by event type (0 OnNoOp .. 12 OnRvbWetAttn). Always
// returns 1. Trk_InitModule and Aud_InitOnce both call it.
u8 Seq_InitModule(void) {
    gSeqCmdHandlers[0] = OnNoOp;
    gSeqCmdHandlers[1] = OnEnd;
    gSeqCmdHandlers[2] = OnKeyOn;
    gSeqCmdHandlers[3] = OnKeyOff;
    gSeqCmdHandlers[4] = OnPitchBend;
    gSeqCmdHandlers[5] = OnPitchRamp;
    gSeqCmdHandlers[6] = OnTrackStatus;
    gSeqCmdHandlers[7] = OnTrackSetPlayList;
    gSeqCmdHandlers[8] = OnTrackSetStream;
    gSeqCmdHandlers[9] = OnModPitch;
    gSeqCmdHandlers[10] = OnModADSRVol;
    gSeqCmdHandlers[11] = OnModStartOffset;
    gSeqCmdHandlers[12] = OnRvbWetAttn;
    return 1;
}

// A new sequenced track (Trk_AllocPerf): resets its sequencer fields (ResetSequencerPerf).
void Seq_Init(AudTrack* pTrack) {
    ResetSequencerPerf(pTrack);
}

// Clears a track's sequencer fields: no wait, variation 0 of set 0, the first channel, the first
// event (0xFF for a stepped template, n0 & 1: it waits for Seq_Step), no step asked for, no notes
// held.
void ResetSequencerPerf(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->nWait = 0;
    pTrack->u.seq.n64 = 0;
    pTrack->u.seq.n65 = 0;
    pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
    pTrack->u.seq.n67 = 0xFF;
    pTrack->u.seq.n68 = 0;
    pTrack->u.seq.n69 = 0;
    Mem_set(pTrack->u.seq.apEvents, 0, sizeof(pTrack->u.seq.apEvents));
}

// A freed sequenced track (Trk_FreePerf): resets its sequencer fields (ResetSequencerPerf).
void Seq_Exit(AudTrack* pTrack) {
    ResetSequencerPerf(pTrack);
}

// Starts a sequenced track (Trk_Start): from its first event (none for a stepped template, n0 & 1,
// which waits for Seq_Step), playing (state 6), with a new variation when the template picks them
// (n1).
void Seq_Start(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->nWait = 0;
    pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
    pTrack->nState = 6;
    pTrack->u.seq.n69 = 0;
    if (pTmpl->n1 != 0) {
        AutoSelectVariation(pTrack);
    }
}

// Stops a sequenced track's events (Trk_Stop): it moves past its last one.
void Seq_Stop(AudTrack* pTrack) {
    pTrack->u.seq.n66 = pTrack->pTmpl->n3;
}

// Ticks a sequenced track once a frame (Trk_Tick). A stopping track (state 3) runs nothing and
// returns 1, a stopped one returns 0. A stepped template (n0 & 1) runs only the event Seq_Step
// asked for, at once, after picking a new variation when it picks them (n1). Any other waits out
// each event's delay (nWait counts the frames) and runs the ones that are due; after a looping End
// event (bNewVariation) it fetches the new variation's events, and past the variation's last event
// the track is stopping. On every 16th audio frame it plays stolen looping notes again
// (CheckForStolenLoopers). Returns 0 once the track has stopped.
u8 Seq_Tick(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;
    u8 nNext;
    AudSeqEvent* pEvent;
    AudSeqEvent* pEnd;

    pTmpl = pTrack->pTmpl;
    if (pTrack->nState == 3) return 1;
    if (pTrack->nState == 2) return 0;
    if (pTmpl->n0 & 1) {
        nNext = pTrack->u.seq.n67;
        if (nNext != 0xFF) {
            if (pTmpl->n1 != 0) {
                AutoSelectVariation(pTrack);
            }
            pTrack->u.seq.n66 = nNext;
            {
                // fake match: its own local (pEvent's address is taken below, so it lives on the stack)
                AudSeqEvent* pNext = pTmpl->pEvents + pTrack->u.seq.n64 * pTmpl->n3 + pTrack->u.seq.n66;

                gSeqCmdHandlers[pNext->nType](pNext, pTrack);
            }
            pTrack->u.seq.n67 = 0xFF;
        }
    } else {
        SetVarCmdBounds(pTrack, &pEvent, &pEnd);
        while (pEvent < pEnd) {
            if (pTrack->nWait < pEvent->n0) {
                pTrack->nWait++;
                break;
            }
            gSeqCmdHandlers[pEvent->nType](pEvent, pTrack);
            if (pTrack->bits.b.bNewVariation) {
                pTrack->bits.b.bNewVariation = 0;
                SetVarCmdBounds(pTrack, &pEvent, &pEnd);
                pTrack->nWait = pEvent->n0;
            } else {
                pTrack->nWait = 0;
                pTrack->u.seq.n66++;
                pEvent++;
            }
            if (pTrack->u.seq.n66 == pTmpl->n3) {
                pTrack->nState = 3;
            }
        }
    }
    if (!(lbl_80282018 & 0xF)) {
        CheckForStolenLoopers(pTrack);
    }
    return pTrack->nState != 2;
}

// *ppEvent: the track's next event (n66) in its variation (n64 of set n68); *ppEnd: the end of that
// variation.
void SetVarCmdBounds(AudTrack* pTrack, AudSeqEvent** ppEvent, AudSeqEvent** ppEnd) {
    AudSeqEvent* pVariation;
    AudSeqEvent* pEnd;
    u8 nEvents;
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    nEvents = pTmpl->n3;
    pVariation = &pTmpl->pEvents[nEvents * (pTrack->u.seq.n64 + pTrack->u.seq.n68 * pTmpl->n7)];
    pEnd = pVariation + nEvents;
    pVariation += pTrack->u.seq.n66;
    *ppEvent = pVariation;
    *ppEnd = pEnd;
}

// Asks a stepped template's track (n0 & 1) to run event n of its variation next (n below n3); with
// bCheck (TW07's debounce), not when it is on that event already.
void Seq_Step(AudTrack* pTrack, u8 n, u8 bCheck) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    if (bCheck && n == pTrack->u.seq.n66) return;
    if (n < pTmpl->n3) {
        pTrack->u.seq.n67 = n;
    }
}

// Switches a sequenced track to set n of its variations. A template that picks its variations (n1)
// picks one in the new set and starts it over from its first event, with no step waiting.
void Seq_SetVariationRange(AudTrack* pTrack, u8 n) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->u.seq.n68 = n;
    if (pTmpl->n1 != 0) {
        AutoSelectVariation(pTrack);
        pTrack->nWait = 0;
        pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
        pTrack->u.seq.n67 = 0xFF;
    }
}

// Prepares a loaded template's events once (Trk_Check): every event's delay is scaled by the update
// rate (Mas_GetUpdateRateScale), a looping template's (n0 & 2) End events without a delay get 60,
// and each stream event's stream is looked up in the play list of the play list event before it
// (the result is not kept).
void Seq_Check(AudTrackTmpl* pTmpl) {
    s8 k;
    s8 j;
    s8 i;
    AudPlayList* pList;
    AudSeqEvent* pEvent;
    f32 fRateScale;

    pList = NULL;
    fRateScale = Mas_GetUpdateRateScale();
    for (i = 0; i < pTmpl->n8; i++) {
        for (j = 0; j < pTmpl->n7; j++) {
            pEvent = &pTmpl->pEvents[pTmpl->n3 * (j + i * pTmpl->n7)];
            for (k = 0; k < pTmpl->n3; k++, pEvent++) {
                pEvent->n0 = audfrac_Mul(pEvent->n0, fRateScale);
                switch (pEvent->nType) {
                case 6:
                    break;
                case 1:
                    if ((pTmpl->n0 & 2) && pEvent->n0 == 0) {
                        pEvent->n0 = 60;
                    }
                    break;
                case 7:
                    pList = Ses_GetStreamPlayList(pEvent->n4);
                    break;
                case 8:
                    Ses_GetStreamFromPlayList(pList, pEvent->n4, NULL);
                    break;
                }
            }
        }
    }
}

// A random number from 0 to nRange - 1 (Misc_RandFunc(1)); 0 when nRange is 0.
u32 Aud_RandomBelow(u32 nRange) {
    if (nRange != 0) {
        return Misc_RandFunc(1) % nRange;
    }
    return 0;
}

// Whether the sound session Ses_Init set up is session 0 (OnKeyOn).
u8 Ses_IsSessionZero(void) {
    return gSesSession == 0;
}

// Tone nTone of a sequencer bank (the instrument a sequenced template plays).
AudSeqTone* Ses_GetInstrumentTone(AudSeqBank* pBank, u8 nTone) {
    return &pBank->aTones[nTone];
}

// The audio update rate given at start-up over 60 (Aud_InitOnce is passed 60, so 1): Seq_Check
// scales event delays by it.
f32 Mas_GetUpdateRateScale(void) {
    return gMasTickRateScale;
}
