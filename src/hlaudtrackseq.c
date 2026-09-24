// hlaudtrackseq.c (TW06's name, by structure: golf/audio/engine/hl/hlaudtrackseq.c, between
// hlaudtrack.c and hlaudtrackstm.c): the sequencer, the tracks that play events instead of a
// stream. A template holds sets of variations of events; each tick the track waits out the next
// event's delay, then runs it through the handler table fn_800AAD18 fills (lbl_801F1880). Notes
// take a voice per channel, stealing one when all are busy. Its extent is its data: fn_800AA744 is
// the first to use its .sdata2 block (0x80283FD8-0x80283FF8), and its handlers run up to
// 0x800AAD14.

#include "core/audtrack.h"

// Picks a track's next variation, the way its template's n1 says: 4 and up in order, 2 at random
// but not the same twice, 3 at random but not the one the template played last, others at random.
void fn_800AA4BC(AudTrack* pTrack) {
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
        nNext = fn_800AB32C(pTmpl->n7);
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

// A note's voice has ended: the channel is free again, and forgets its note unless the voice
// asks to keep it (nReason 1 with bA_1 set).
void fn_800AA5A0(AudVoice* pVoice, int nReason) {
    AudTrack* pTrack;
    u8 nChannel;

    pTrack = pVoice->pUser;
    nChannel = pVoice->nIndex;
    fn_800AA400(pVoice, nReason);
    if (nReason != 1 || !pVoice->bA_1) {
        pTrack->u.seq.apEvents[nChannel] = NULL;
    }
}

// Plays again every note whose voice was taken from it.
void fn_800AA618(AudTrack* pTrack) {
    AudVoice** ppVoice;
    AudSeqEvent** ppEvent;
    AudVoice** ppEnd;

    ppVoice = pTrack->apVoices;
    ppEvent = pTrack->u.seq.apEvents;
    ppEnd = ppVoice + pTrack->pTmpl->n2;
    for (; ppVoice < ppEnd; ppVoice++, ppEvent++) {
        if (*ppVoice == NULL && *ppEvent != NULL) {
            AudSeqEvent* pEvent = *ppEvent;

            *ppEvent = NULL;
            fn_800AA744(pEvent, pTrack);
        }
    }
}

// Events that do nothing.
void fn_800AA694(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Event: the end of a variation. Templates with n0 & 2 go on to the next variation (the event's
// n3 is where to resume); the others stop.
void fn_800AA698(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    if (pTmpl->n0 & 2) {
        if (pTrack->nState != 3) {
            if (pTmpl->n1 != 0) {
                fn_800AA4BC(pTrack);
            }
            pTrack->u.seq.n66 = pEvent->n3;
            pTrack->bits.b.b7 = 1;
            if (pTmpl->n0 & 0x80) {
                fn_800A8584(pTrack->pSource, pTmpl->n6, 1);
            }
        }
    } else {
        pTrack->nState = 3;
    }
}

// Event: a note. Its tone is the event's n3, its loudness the event's n4 through the track's
// volume; it takes the next free channel, stealing a voice that is easier to steal if none is.
void fn_800AA744(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudVoiceRequest request;
    AudTrackTmpl* pTmpl;
    AudSeqTone* pTone;
    s16 nVolume;
    u32 bLoops;
    u8 nChannel;
    u8 i;
    AudVoice* pVoice;
    AudVoiceParams* pParams;
    f32 f;

    pTmpl = pTrack->pTmpl;
    pTone = fn_800AB384(pTmpl->data.pBank, pEvent->n3);
    nVolume = fn_800A85FC((f32)(pEvent->n4 << 7),
                          fn_800A85FC(pTrack->f48, fn_800AA44C(pTmpl->data.pBank->n3)));
    bLoops = pTone->n10 & 1;
    if (nVolume == 0 || pTone == NULL) return;
    nChannel = pTrack->u.seq.n65;
    request.n4 = bLoops != 0;
    if (!fn_800AB374() && pTrack->pSource->nSound == 1 && pTrack->nChannel == 0) {
        request.n4 = 2;
    }
    for (i = 0; i < pTmpl->n2;) {
        pVoice = pTrack->apVoices[nChannel];
        if (pVoice == NULL) {
            if (pTrack->u.seq.apEvents[nChannel] == NULL) break;
        } else if (request.n4 > pVoice->n10 || pVoice->n10 == 0) {
            fn_800ACB28(pVoice);
            pTrack->u.seq.apEvents[nChannel] = NULL;
            pTrack->apVoices[nChannel] = NULL;
            pTrack->n5D--;
            break;
        }
        nChannel++;
        if (nChannel >= pTmpl->n2) {
            nChannel = 0;
        }
        i++;
    }
    if (i >= pTmpl->n2) return;
    request.pfnCallback = fn_800AA5A0;
    pParams = &pTrack->params;
    f = 1.0f;
    request.pUser = pTrack;
    request.nIndex = nChannel;
    request.flags.n = 0;
    request.flags.b.b14 = (pTrack->pSource->pSound->n3 & 4) || (pTrack->pTmpl->n0 & 0x20);
    request.nPriority = nVolume;
    request.flags.b.b9 = bLoops;
    request.n2 = 0x40;
    request.n3 = 0x7F;
    pVoice = fn_800AC4A0(&request);
    if (pVoice == NULL) return;
    pVoice->pTone = pTone;
    if (pParams->flags.b.bPitch) {
        f = pParams->fPitch;
    }
    fn_800AC6D0(pVoice, pParams, pEvent->n4, f);
    pTrack->n5D++;
    pTrack->apVoices[nChannel] = pVoice;
    pTrack->u.seq.apEvents[nChannel] = pEvent;
    pTrack->u.seq.n65 = nChannel;
    if (++pTrack->u.seq.n65 >= pTmpl->n2) {
        pTrack->u.seq.n65 = 0;
    }
    pParams->flags.n = 0;
}

// Event: lets the voices playing tone n3 end, starting from the channel of the last note.
void fn_800AA9EC(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;
    AudSeqTone* pTone;
    s8 nChannel;
    u8 i;
    AudVoice* pVoice;

    pTmpl = pTrack->pTmpl;
    pTone = fn_800AB384(pTmpl->data.pBank, pEvent->n3);
    if (pTone == NULL) return;
    nChannel = pTrack->u.seq.n65 - 1;
    if (nChannel < 0) {
        nChannel = 0;
    }
    for (i = 0; i < pTmpl->n2; i++, nChannel++) {
        if (nChannel >= pTmpl->n2) {
            nChannel -= pTmpl->n2;
        }
        pVoice = pTrack->apVoices[nChannel];
        if (pVoice != NULL && pVoice->pTone == pTone) {
            fn_800ACA94(pVoice);
        }
    }
}

void fn_800AAAA0(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Event: sets the rate (n4 / 65536) of the source's track n3 (0xFF: this track), starting that
// track if it is not playing.
void fn_800AAAA4(AudSeqEvent* pEvent, AudTrack* pTrack) {
    AudTrack* pTarget;
    AudSource* pSource;
    u8 n;

    n = pEvent->n3;
    if (n == 0xFF) {
        pTarget = pTrack;
    } else {
        pSource = pTrack->pSource;
        pTarget = pSource->apTracks[n];
        if (pTarget == NULL) {
            pTarget = fn_800A9BC8(pSource, &pSource->pSound->aTracks[n], n, pTrack->f48);
        }
    }
    if (pTarget != NULL) {
        pTarget->f50 = (u32)pEvent->n4 / 65536.0f;
    }
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

// Event: gives the source's streamed track n3 play list n4, starting the track if needed.
void fn_800AAB8C(AudSeqEvent* pEvent, AudTrack* pTrack) {
    int n;
    AudSource* pSource;
    AudTrack* pTarget;
    u8 nPlayList;

    n = pEvent->n3;
    pSource = pTrack->pSource;
    nPlayList = pEvent->n4;
    pTarget = pSource->apTracks[n];
    if (pTarget == NULL) {
        pTarget = fn_800A9BC8(pSource, &pSource->pSound->aTracks[n], n, pTrack->f48);
    }
    if (pTarget != NULL) {
        Stm_SetPlayList(pTarget, nPlayList);
    }
}

// Event: the same with stream n4.
void fn_800AAC00(AudSeqEvent* pEvent, AudTrack* pTrack) {
    int n;
    AudSource* pSource;
    AudTrack* pTarget;
    u16 nStream;

    n = pEvent->n3;
    pSource = pTrack->pSource;
    nStream = pEvent->n4;
    pTarget = pSource->apTracks[n];
    if (pTarget == NULL) {
        pTarget = fn_800A9BC8(pSource, &pSource->pSound->aTracks[n], n, pTrack->f48);
    }
    if (pTarget != NULL) {
        Stm_SetStream(pTarget, nStream, 0);
    }
}

// Events: set a voice setting for the next note.
void fn_800AAC78(AudSeqEvent* pEvent, AudTrack* pTrack) {
    pTrack->params.fPitch = (u32)pEvent->n4 / 65536.0f;
    pTrack->params.flags.b.bPitch = 1;
}

void fn_800AACBC(AudSeqEvent* pEvent, AudTrack* pTrack) {
    u8 n;

    n = pEvent->n3;
    pTrack->params.a8[n] = pEvent->n4;
    pTrack->params.flags.b.b5 = n == 0;
    pTrack->params.flags.b.b4 = n == 1;
}

void fn_800AACF8(AudSeqEvent* pEvent, AudTrack* pTrack) {
    pTrack->params.nC = pEvent->n4;
    pTrack->params.flags.b.bC = 1;
}

void fn_800AAD14(AudSeqEvent* pEvent, AudTrack* pTrack) {
}

// Fills the event handler table.
u8 fn_800AAD18(void) {
    lbl_801F1880[0] = fn_800AA694;
    lbl_801F1880[1] = fn_800AA698;
    lbl_801F1880[2] = fn_800AA744;
    lbl_801F1880[3] = fn_800AA9EC;
    lbl_801F1880[4] = fn_800AAAA0;
    lbl_801F1880[5] = fn_800AAAA4;
    lbl_801F1880[6] = fn_800AAB48;
    lbl_801F1880[7] = fn_800AAB8C;
    lbl_801F1880[8] = fn_800AAC00;
    lbl_801F1880[9] = fn_800AAC78;
    lbl_801F1880[10] = fn_800AACBC;
    lbl_801F1880[11] = fn_800AACF8;
    lbl_801F1880[12] = fn_800AAD14;
    return 1;
}

void fn_800AADE8(AudTrack* pTrack) {
    fn_800AAE08(pTrack);
}

// Resets a track's sequencer fields.
void fn_800AAE08(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->n62 = 0;
    pTrack->u.seq.n64 = 0;
    pTrack->u.seq.n65 = 0;
    pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
    pTrack->u.seq.n67 = 0xFF;
    pTrack->u.seq.n68 = 0;
    pTrack->u.seq.n69 = 0;
    fn_80005AE8(pTrack->u.seq.apEvents, 0, sizeof(pTrack->u.seq.apEvents));
}

void fn_800AAE70(AudTrack* pTrack) {
    fn_800AAE08(pTrack);
}

// Starts a sequenced track.
void fn_800AAE90(AudTrack* pTrack) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->n62 = 0;
    pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
    pTrack->nState = 6;
    pTrack->u.seq.n69 = 0;
    if (pTmpl->n1 != 0) {
        fn_800AA4BC(pTrack);
    }
}

void fn_800AAEEC(AudTrack* pTrack) {
    pTrack->u.seq.n66 = pTrack->pTmpl->n3;
}

// Ticks a sequenced track: runs its events that are due. Returns 0 once it has stopped.
u8 fn_800AAEFC(AudTrack* pTrack) {
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
                fn_800AA4BC(pTrack);
            }
            pTrack->u.seq.n66 = nNext;
            {
                // fake match: its own local (pEvent's address is taken below, so it lives on the stack)
                AudSeqEvent* pNext = pTmpl->pEvents + pTrack->u.seq.n64 * pTmpl->n3 + pTrack->u.seq.n66;

                lbl_801F1880[pNext->nType](pNext, pTrack);
            }
            pTrack->u.seq.n67 = 0xFF;
        }
    } else {
        fn_800AB0CC(pTrack, &pEvent, &pEnd);
        while (pEvent < pEnd) {
            if (pTrack->n62 < pEvent->n0) {
                pTrack->n62++;
                break;
            }
            lbl_801F1880[pEvent->nType](pEvent, pTrack);
            if (pTrack->bits.b.b7) {
                pTrack->bits.b.b7 = 0;
                fn_800AB0CC(pTrack, &pEvent, &pEnd);
                pTrack->n62 = pEvent->n0;
            } else {
                pTrack->n62 = 0;
                pTrack->u.seq.n66++;
                pEvent++;
            }
            if (pTrack->u.seq.n66 == pTmpl->n3) {
                pTrack->nState = 3;
            }
        }
    }
    if (!(lbl_80282018 & 0xF)) {
        fn_800AA618(pTrack);
    }
    return pTrack->nState != 2;
}

// The track's next event and the end of its variation.
void fn_800AB0CC(AudTrack* pTrack, AudSeqEvent** ppEvent, AudSeqEvent** ppEnd) {
    AudTrackTmpl* pTmpl;
    AudSeqEvent* pVariation;
    u8 nEvents;

    pTmpl = pTrack->pTmpl;
    nEvents = pTmpl->n3;
    pVariation = &pTmpl->pEvents[nEvents * (pTrack->u.seq.n64 + pTrack->u.seq.n68 * pTmpl->n7)];
    *ppEvent = &pVariation[pTrack->u.seq.n66];
    *ppEnd = &pVariation[nEvents];
}

// Asks for event n next (templates with n0 & 1); with bCheck, not if it is the current one.
void fn_800AB118(AudTrack* pTrack, u8 n, u8 bCheck) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    if (bCheck && n == pTrack->u.seq.n66) return;
    if (n < pTmpl->n3) {
        pTrack->u.seq.n67 = n;
    }
}

// Switches a track to set n of its variations.
void fn_800AB14C(AudTrack* pTrack, u8 n) {
    AudTrackTmpl* pTmpl;

    pTmpl = pTrack->pTmpl;
    pTrack->u.seq.n68 = n;
    if (pTmpl->n1 != 0) {
        fn_800AA4BC(pTrack);
        pTrack->n62 = 0;
        pTrack->u.seq.n66 = (pTmpl->n0 & 1) ? 0xFF : 0;
        pTrack->u.seq.n67 = 0xFF;
    }
}

// Prepares a template's events once: scales every delay by the tempo (fn_800AB39C), gives
// looping templates' zero-delay notes a delay of 60, and resolves the play list and stream events.
void fn_800AB1B8(AudTrackTmpl* pTmpl) {
    s8 k;
    s8 j;
    s8 i;
    AudPlayList* pList;
    AudSeqEvent* pEvent;
    f32 fTempo;

    pList = NULL;
    fTempo = fn_800AB39C();
    for (i = 0; i < pTmpl->n8; i++) {
        for (j = 0; j < pTmpl->n7; j++) {
            pEvent = &pTmpl->pEvents[pTmpl->n3 * (j + i * pTmpl->n7)];
            for (k = 0; k < pTmpl->n3; k++, pEvent++) {
                pEvent->n0 = fn_800A85FC(pEvent->n0, fTempo);
                switch (pEvent->nType) {
                case 6:
                    break;
                case 1:
                    if ((pTmpl->n0 & 2) && pEvent->n0 == 0) {
                        pEvent->n0 = 60;
                    }
                    break;
                case 7:
                    pList = fn_800A9564(pEvent->n4);
                    break;
                case 8:
                    fn_800A9438(pList, pEvent->n4, NULL);
                    break;
                }
            }
        }
    }
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

AudSeqTone* fn_800AB384(AudSeqBank* pBank, u8 nTone) {
    return &pBank->aTones[nTone];
}

f32 fn_800AB39C(void) {
    return lbl_80281460;
}
