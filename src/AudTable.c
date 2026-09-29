// AudTable.c (our name; EA's file is HLAudEmitter.c: TW07's golf/audio/engine/hl/HLAudEmitter.c
// has these Emi_ functions and, inline, its statics FreeAllPerfs to PreprocessControllers; TW06
// lists hl/hlaudemitter.c): the sound engine's table of 256 playing sounds (AudSource, 0x7C bytes
// each), allocated from the audio memory stack (UAudMemStack.c's fn_800B5BD8) by Emi_InitModule and
// reached through lbl_80282058. Each entry plays a bank sound's tracks and, for a sound placed in
// the world, works out its volume, pan and doppler pitch from its distance to the listeners. Its
// data starts on its own 8-byte boundaries (.sbss 0x80282058, .sdata2 0x80283F88); where its code
// starts before Emi_InitModule is not proven.

#include "core/audtrack.h"

f32  audvec3_ApproxLength(f32* pVec);                     // its length

void FreeAllPerfs(AudSource* pSource);
f32  Attenuation3D(f32 fDist, f32 fScale);
void Doppler3D(AudSource* pSource);
void Panning3D(AudSource* pSource);
f32  Distance3D(AudSource* pSource, f32 (*aPos)[3]);
void PreprocessControllers(AudSource* pSource, u32* auStreams, u16 uMask);

AudSource* lbl_80282058;

// Allocates the 256 sound entries from the audio memory stack, cleared, each at pitch 1. Nonzero
// when it worked.
u8 Emi_InitModule(void) {
    u8 bOk;
    s32 i;

    bOk = 0;
    lbl_80282058 = fn_800B5BD8(256 * sizeof(AudSource));
    if (lbl_80282058 != NULL) {
        Mem_set(lbl_80282058, 0, 256 * sizeof(AudSource));
        for (i = 0; i < 256; i++) {
            lbl_80282058[i].fPitch = 1.0f;
        }
        bOk = 1;
    }
    return bOk;
}

// The source table's part of starting a sound session, one of Ses_Init's steps: nothing to do,
// always succeeds (1).
u8 Emi_InitSession(void) {
    return 1;
}

// The source table's part of ending a sound session (fn_800A8D88): nothing to do.
void Emi_ExitSession(void) {
}

// Aud_EmiAdd's half: clears entry nEntry (the instance's own number) and binds it to bank sound
// nSound. Returns the entry, which the instance keeps as its command block (AudInstance.pCmd).
AudSource* Emi_AddInstance(u8 nEntry, s16 nSound) {
    AudSound* pSound;
    AudSource* pSource;

    pSound = fn_800A85CC(nSound);
    pSource = &lbl_80282058[nEntry];
    Mem_set(pSource, 0, sizeof(AudSource));
    pSource->nSound = nSound;
    pSource->pSound = pSound;
    return pSource;
}

// Applies instance nEntry's commands of this frame (Aud_EmiCycle): uMaskA / uMaskB are the tracks
// switched on / off (a bit each), auStreams the controller value of each track, uMask what changed
// (AudSource.uChanged; bits 0-2 hand the streams on through PreprocessControllers). A sound placed
// in the world (AudSound.n3 bit 0) gets its distance, pan and doppler pitch, and each track its
// volume by distance (Attenuation3D); one farther away than its sound's f4 is out of earshot and
// all its tracks are freed.
void Emi_UpdInstance(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, f32 (*aPos)[3], u16 uMask) {
    AudSource* pSource;
    f32 fDist;
    u8 bHeard;
    AudTrack** ppTrack;
    AudTrackTmpl* pTmpl;
    f32 fVolume;
    u8 nTracks;
    u8 i;
    u8 uBit;
    AudTrack* pTrack;
    u8 bOn;
    u8 bOff;

    pSource = &lbl_80282058[nEntry];
    if (uMask & 7) {
        PreprocessControllers(pSource, auStreams, uMask);
    }
    if (pSource->pSound->n3 & 1) {
        fDist = Distance3D(pSource, aPos);
        bHeard = fDist - pSource->pSound->f4 < 0.0f;
        if (bHeard) {
            pSource->fDist = fDist;
            Panning3D(pSource);
            Doppler3D(pSource);
        }
    } else {
        bHeard = 1;
    }
    if (bHeard) {
        ppTrack = pSource->apTracks;
        fVolume = 1.0f;
        nTracks = pSource->pSound->nTracks;
        pTmpl = pSource->pSound->aTracks;
        for (i = 0, uBit = 1; i < nTracks; uBit <<= 1, i++, pTmpl++, ppTrack++) {
            pTrack = *ppTrack;
            bOn = (uMaskA & uBit) != 0;
            bOff = (uMaskB & uBit) != 0;
            if (pSource->pSound->n3 & 1) {
                fVolume = Attenuation3D(fDist, pTmpl->f10);
            }
            Trk_UpdatePerf(pSource, pTrack, pTmpl, i, bOn, bOff, fVolume);
        }
        return;
    }
    FreeAllPerfs(pSource);
}

// Stops every track of an entry.
void FreeAllPerfs(AudSource* pSource) {
    u8 i;

    for (i = 0; i < pSource->pSound->nTracks; i++) {
        Trk_FreePerf(pSource->apTracks[i]);
    }
}

// A placed track's volume (0 to 1) at distance fDist, scaled by its template's fScale
// (AudTrackTmpl.f10): full up to 2, falling in a straight line to a quarter at 5 and to silence at
// 10 (3276/65536 = 0.05 a unit).
f32 Attenuation3D(f32 fDist, f32 fScale) {
    f32 f;
    f32 fVolume;

    f = fn_800A85FC(fDist, fScale);
    if (f > 5.0f) {
        fVolume = 0.25f - fn_800A85FC(f - 5.0f, 3276.0f / 65536.0f);
        if (fVolume < 0.0f) {
            fVolume = 0.0f;
        }
    } else {
        fVolume = 1.0f;
        if (f > 2.0f) {
            fVolume -= fn_800A85FC(f - 2.0f, 0.25f);
        }
    }
    return fVolume;
}

// The doppler pitch of a placed sound (1 when its sound has AudSound.n3 bit 1 or is not placed):
// 345 / (345 - speed), 345 being the speed of sound and speed 32 times how much nearer to the
// nearest listener it came since the last frame (at most 1); then keeps that nearest distance in
// fDist.
// EA bug: Emi_UpdInstance stores the new nearest distance in fDist before it calls this, so the
// change is always 0 and the pitch stays 1.
void Doppler3D(AudSource* pSource) {
    f32 fDist;
    u8 uFlags;
    f32 fSpeed;
    f32 fPitch;

    fDist = pSource->afDist[0];
    uFlags = pSource->pSound->n3;
    if ((uFlags & 2) || !(uFlags & 1)) {
        fPitch = 1.0f;
    } else {
        if (lbl_80282068 >= 2 && pSource->afDist[1] < pSource->afDist[0]) {
            fDist = pSource->afDist[1];
        }
        fSpeed = 32.0f * (pSource->fDist - fDist);
        if (fSpeed > 1.0f) {
            fSpeed = 1.0f;
        }
        fPitch = 345.0f / (345.0f - fSpeed);
    }
    pSource->fPitch = fPitch;
    pSource->fDist = fDist;
}

// The pan of a sound from where the one listener hears it: fPan -1 left to 1 right, f68 -1 to 1 by
// its depth in the listener's view (AudVoiceParams.n7). With two listeners (split screen), or for a
// sound not placed in the world, it is centred (0, 1).
void Panning3D(AudSource* pSource) {
    f32 fInv;
    f32 fPan;
    f32 f68;

    if (lbl_80282068 < 2 && (pSource->pSound->n3 & 1)) {
        fInv = pSource->afDist[0] > 0.0f ? 1.0f / pSource->afDist[0] : 0.0f;
        fPan = fn_800A85FC(-pSource->aPos[0][0], fInv);
        f68 = fn_800A85FC(pSource->aPos[0][2], fInv);
        fPan = fPan < -1.0f ? -1.0f : fPan > 1.0f ? 1.0f : fPan;
        f68 = f68 < -1.0f ? -1.0f : f68 > 1.0f ? 1.0f : f68;
        pSource->fPan = fPan;
        pSource->f68 = f68;
        return;
    }
    pSource->fPan = 0.0f;
    pSource->f68 = 1.0f;
}

// Measures the sound's distance from each listener; returns the nearest. aPos (the position as
// each view hears it) is not used: the distances come from pSource->aPos.
f32 Distance3D(AudSource* pSource, f32 (*aPos)[3]) {
    f32 fNearest;
    u8 i;
    f32 fDist;

    fNearest = 32768.0f;
    for (i = 0; i < lbl_80282068; i++) {
        fDist = audvec3_ApproxLength(pSource->aPos[i]);
        pSource->afDist[i] = fDist;
        if (fNearest > fDist) {
            fNearest = fDist;
        }
    }
    return fNearest;
}

// Sets the play list and stream of each streamed track in uMask: auStreams[i] holds the stream in
// its low 16 bits, the play list above it and the mode in the top byte.
void PreprocessControllers(AudSource* pSource, u32* auStreams, u16 uMask) {
    AudTrack* pTrack;
    u32 uStream;
    u8 i;
    u16 uBit;

    for (uBit = 1, i = 0; i < 8; i++, uBit <<= 1) {
        if (uMask & uBit) {
            pTrack = pSource->apTracks[i];
            if (pTrack == NULL) {
                pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[i], i, 1.0f);
                if (pTrack == NULL) return;
            }
            if (pTrack->pTmpl->n0 & 8) {
                uStream = auStreams[i];
                Stm_SetPlayList(pTrack, uStream >> 16);
                Stm_SetStream(pTrack, uStream & 0xFFFF, (s32)uStream >> 24);
            }
        }
    }
}

// Stops entry nEntry and frees it.
void Emi_DelInstance(u8 nEntry) {
    AudSource* pSource;

    pSource = &lbl_80282058[nEntry];
    FreeAllPerfs(pSource);
    pSource->pSound = NULL;
    pSource->nSound = 0;
}

// Picks variation n of entry nEntry's track nTrack, starting the track if it has not started.
void Emi_SetTrackVariation(u8 nEntry, u8 nTrack, u8 n) {
    AudSource* pSource;
    AudTrackTmpl* pTmpl;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTmpl = &pSource->pSound->aTracks[nTrack];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, pTmpl, nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_SelectVariation(pTrack, n);
}

// Picks variation range n of entry nEntry's track nTrack, starting the track if it has not started.
void Emi_SetTrackVarRange(u8 nEntry, u8 nTrack, u8 n) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_SetVariationRange(pTrack, n);
}

// Sets the variation range that sound nSound's track nTrack uses (its template's nA), for every
// entry that plays it.
void Emi_SetTrackVarRangeTmpl(s16 nSound, u8 nTrack, u8 n) {
    fn_800A85CC(nSound)->aTracks[nTrack].nA = n;
}

// Asks entry nEntry's track nTrack to play step n next (with bCheck, not when it is the current
// one), starting the track if it has not started.
void Emi_SetTrackStep(u8 nEntry, u8 nTrack, u8 n, int bCheck) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_Step(pTrack, n, bCheck);
}

// Sets the volume (AudTrack.f44) of entry nEntry's track nTrack, starting the track if it has not
// started.
void Emi_SetTrackAttenuation(u8 nEntry, u8 nTrack, f32 fVolume) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->f44 = fVolume;
}

// Sets the pitch (AudTrack.f4C) of entry nEntry's track nTrack, starting the track if it has not
// started.
void Emi_SetTrackPitchFactor(u8 nEntry, u8 nTrack, f32 fPitch) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->f4C = fPitch;
}

// Prepares each sequenced track template of a sound just loaded from a bank (Trk_Check). n, the
// sound's number in the bank, is not used.
void Emi_CheckTemplate(AudSound* pSound, u16 n) {
    u8 i;

    for (i = 0; i < pSound->nTracks; i++) {
        Trk_Check(&pSound->aTracks[i]);
    }
}

// Passes a track's report on to the emitter instance of this entry (the same number), through
// Aud_EmiTrkCB: n 0 when the track was freed (Trk_FreePerf), 1 from a sequencer event
// (OnEnd).
void Emi_TrackCallback(AudSource* pSource, u8 nTrack, s32 n) {
    Aud_EmiTrkCB((u8)(pSource - lbl_80282058), nTrack, n);
}
