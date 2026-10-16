// hlaudemitter.c: the sound engine's sound sources, one playing bank sound per emitter instance,
// whose tracks it starts, steers and frees. For a sound placed in the world it works out the
// volume, pan and doppler pitch from the sound's distance to the listeners.

#include "core/audtrack.h"

AudSource* gEmiSources;

static void FreeAllPerfs(AudSource* pSource);
static f32  Attenuation3D(f32 fDist, f32 fDistScale);
static void Doppler3D(AudSource* pSource);
static void Panning3D(AudSource* pSource);
static f32  Distance3D(AudSource* pSource, f32 (*aPos)[3]);
static void PreprocessControllers(AudSource* pSource, u32* auParams, u16 uChanged);

// Allocates the sources from the audio memory stack, cleared, each at pitch 1. Nonzero when it
// worked.
u8 Emi_InitModule(void) {
    u8 bOk;
    s32 i;

    bOk = 0;
    gEmiSources = AudMem_Alloc(EMI_NUM_SOURCES * sizeof(AudSource));
    if (gEmiSources != NULL) {
        Mem_set(gEmiSources, 0, EMI_NUM_SOURCES * sizeof(AudSource));
        for (i = 0; i < EMI_NUM_SOURCES; i++) {
            gEmiSources[i].fPitch = 1.0f;
        }
        bOk = 1;
    }
    return bOk;
}

// The sources' part of starting a sound session (Ses_Init): nothing to do, always succeeds (1).
u8 Emi_InitSession(void) {
    return 1;
}

// The sources' part of ending a sound session (Ses_Exit): nothing to do.
void Emi_ExitSession(void) {
}

// Aud_EmiAdd's half: clears source nEntry (the instance's own number) and binds it to bank sound
// nSound. Returns the source, in which the instance then queues its commands. The commands passed
// with it (uTracksOn to uChanged) are not read; Aud_EmiAdd passes zeros.
AudSource* Emi_AddInstance(u8 nEntry, s16 nSound, u8 uTracksOn, u8 uTracksOff, u32* auParams,
                           f32 (*aPos)[3], u16 uChanged) {
    AudSound* pSound;
    AudSource* pSource;

    pSound = Ses_GetEmitterTemplateFromID(nSound);
    pSource = &gEmiSources[nEntry];
    Mem_set(pSource, 0, sizeof(AudSource));
    pSource->nSound = nSound;
    pSource->pSound = pSound;
    return pSource;
}

// Applies source nEntry's commands of this frame (Aud_EmiCycle): uTracksOn / uTracksOff are the
// tracks switched on / off (a bit each), auParams each track's controller value and uChanged which
// of them were queued (a bit per track's controller, and the AUDSOURCE_CHANGED_ bits). A placed
// sound (AUDSOUND_3D) gets its distance, pan and doppler pitch, and each track a volume by distance
// (Attenuation3D); one farther away than its sound's fMaxDist is out of earshot and all its tracks
// are freed. Every track then starts, stops or carries on (Trk_UpdatePerf).
void Emi_UpdInstance(u8 nEntry, u8 uTracksOn, u8 uTracksOff, u32* auParams, f32 (*aPos)[3],
                     u16 uChanged) {
    AudSource* pSource;
    f32 fDist;
    u8 bHeard;
    AudTrack** ppTrack;
    AudTrackTmpl* pTmpl;
    f32 fDistAttn;
    u8 nTracks;
    u8 i;
    u8 uBit;
    AudTrack* pTrack;
    u8 bOn;
    u8 bOff;

    pSource = &gEmiSources[nEntry];
    if (uChanged & AUDSOURCE_CHANGED_PARAMS_0_2) {
        PreprocessControllers(pSource, auParams, uChanged);
    }
    if (pSource->pSound->uFlags & AUDSOUND_3D) {
        fDist = Distance3D(pSource, aPos);
        bHeard = fDist - pSource->pSound->fMaxDist < 0.0f;
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
        fDistAttn = 1.0f;
        nTracks = pSource->pSound->nTracks;
        pTmpl = pSource->pSound->aTracks;
        for (i = 0, uBit = 1; i < nTracks; uBit <<= 1, i++, pTmpl++, ppTrack++) {
            pTrack = *ppTrack;
            bOn = (uTracksOn & uBit) != 0;
            bOff = (uTracksOff & uBit) != 0;
            if (pSource->pSound->uFlags & AUDSOUND_3D) {
                fDistAttn = Attenuation3D(fDist, pTmpl->fDistScale);
            }
            Trk_UpdatePerf(pSource, pTrack, pTmpl, i, bOn, bOff, fDistAttn);
        }
        return;
    }
    FreeAllPerfs(pSource);
}

// Frees every track of a source (Trk_FreePerf; a track it does not have is skipped).
static void FreeAllPerfs(AudSource* pSource) {
    u8 i;

    for (i = 0; i < pSource->pSound->nTracks; i++) {
        Trk_FreePerf(pSource->apTracks[i]);
    }
}

// A placed track's volume (0 to 1) at distance fDist, scaled first by its template's fDistScale:
// full up to 2, falling in a straight line to a quarter at 5 and on to silence at 10.
static f32 Attenuation3D(f32 fDist, f32 fDistScale) {
    f32 fScaled;
    f32 fDistAttn;

    fScaled = audfrac_Mul(fDist, fDistScale);
    if (fScaled > 5.0f) {
        fDistAttn = 0.25f - audfrac_Mul(fScaled - 5.0f, 3276.0f / 65536.0f);
        if (fDistAttn < 0.0f) {
            fDistAttn = 0.0f;
        }
    } else {
        fDistAttn = 1.0f;
        if (fScaled > 2.0f) {
            fDistAttn -= audfrac_Mul(fScaled - 2.0f, 0.25f);
        }
    }
    return fDistAttn;
}

// The doppler pitch of a placed sound (1 with AUDSOUND_NO_DOPPLER; its test for a sound not placed
// never passes, as only placed sounds get here): 345 / (345 - fSpeed), 345 being the speed of
// sound and fSpeed 32 times how much nearer than fDist the nearest listener now is (at most 1).
// Then keeps that nearest distance in fDist.
static void Doppler3D(AudSource* pSource) {
    f32 fDist;
    u8 uFlags;
    f32 fSpeed;
    f32 fPitch;

    fDist = pSource->afDist[0];
    uFlags = pSource->pSound->uFlags;
    if ((uFlags & AUDSOUND_NO_DOPPLER) || !(uFlags & AUDSOUND_3D)) {
        fPitch = 1.0f;
    } else {
        if (gMicCount >= 2 && pSource->afDist[1] < pSource->afDist[0]) {
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

// The pan of a placed sound from where the one listener hears it: fPan -1 left to 1 right from
// its x, fPanDepth -1 to 1 from its z, each over its distance. With two listeners (split screen)
// it is centred (0, 1); its test for a sound not placed never passes, as only placed sounds get
// here.
static void Panning3D(AudSource* pSource) {
    f32 fInv;
    f32 fPan;
    f32 fDepth;

    if (gMicCount < 2 && (pSource->pSound->uFlags & AUDSOUND_3D)) {
        fInv = pSource->afDist[0] > 0.0f ? 1.0f / pSource->afDist[0] : 0.0f;
        fPan = audfrac_Mul(-pSource->aPos[0][0], fInv);
        fDepth = audfrac_Mul(pSource->aPos[0][2], fInv);
        fPan = fPan < -1.0f ? -1.0f : fPan > 1.0f ? 1.0f : fPan;
        fDepth = fDepth < -1.0f ? -1.0f : fDepth > 1.0f ? 1.0f : fDepth;
        pSource->fPan = fPan;
        pSource->fPanDepth = fDepth;
        return;
    }
    pSource->fPan = 0.0f;
    pSource->fPanDepth = 1.0f;
}

// Measures the sound's distance from each listener into afDist; returns the nearest (32768 with
// no listener). aPos is not read: the distances come from pSource->aPos, the array Aud_EmiCycle
// passes in it.
static f32 Distance3D(AudSource* pSource, f32 (*aPos)[3]) {
    f32 fNearest;
    u8 i;
    f32 fDist;

    fNearest = 32768.0f;
    for (i = 0; i < gMicCount; i++) {
        fDist = audvec3_ApproxLength(pSource->aPos[i]);
        pSource->afDist[i] = fDist;
        if (fNearest > fDist) {
            fNearest = fDist;
        }
    }
    return fNearest;
}

// For each track whose bit is set in uChanged: takes a track from the pool for it if it has none
// (and stops there if the pool has none to give), and for a streamed track sets the play list and
// stream its controller value auParams[i] holds: the stream in the low 16 bits, the play list
// above it and the play mode in the top byte.
static void PreprocessControllers(AudSource* pSource, u32* auParams, u16 uChanged) {
    AudTrack* pTrack;
    u32 uParam;
    u8 i;
    u16 uBit;

    for (uBit = 1, i = 0; i < AUDSOURCE_NUM_TRACKS; i++, uBit <<= 1) {
        if (uChanged & uBit) {
            pTrack = pSource->apTracks[i];
            if (pTrack == NULL) {
                pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[i], i, 1.0f);
                if (pTrack == NULL) return;
            }
            if (pTrack->pTmpl->uFlags & AUDTRACKTMPL_STREAMED) {
                uParam = auParams[i];
                Stm_SetPlayList(pTrack, uParam >> 16);
                Stm_SetStream(pTrack, uParam & 0xFFFF, (s32)uParam >> 24);
            }
        }
    }
}

// Frees source nEntry's tracks and unbinds it from its sound.
void Emi_DelInstance(u8 nEntry) {
    AudSource* pSource;

    pSource = &gEmiSources[nEntry];
    FreeAllPerfs(pSource);
    pSource->pSound = NULL;
    pSource->nSound = 0;
}

// Sets the variation of source nEntry's track nTrack, taking a track from the pool for it if it
// has none.
void Emi_SetTrackVariation(u8 nEntry, u8 nTrack, u8 nVariation) {
    AudSource* pSource;
    AudTrackTmpl* pTmpl;
    AudTrack* pTrack;

    pSource = &gEmiSources[nEntry];
    pTmpl = &pSource->pSound->aTracks[nTrack];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, pTmpl, nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_SelectVariation(pTrack, nVariation);
}

// Switches source nEntry's track nTrack to set nVarSet of its variations, taking a track from the
// pool for it if it has none.
void Emi_SetTrackVarRange(u8 nEntry, u8 nTrack, u8 nVarSet) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &gEmiSources[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_SetVariationRange(pTrack, nVarSet);
}

// Forces set nVarSet of its variations on track nTrack of sound nSound: on every source that plays
// the sound, that track switches to the set when it next picks a variation (0xFF: no set forced).
void Emi_SetTrackVarRangeTmpl(s16 nSound, u8 nTrack, u8 nVarSet) {
    Ses_GetEmitterTemplateFromID(nSound)->aTracks[nTrack].nVarSet = nVarSet;
}

// Asks source nEntry's track nTrack to run event nEvent of its variation next (with bCheck, not
// when that is already its next event), taking a track from the pool for it if it has none.
void Emi_SetTrackStep(u8 nEntry, u8 nTrack, u8 nEvent, int bCheck) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &gEmiSources[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    Trk_Step(pTrack, nEvent, bCheck);
}

// Sets the volume of source nEntry's track nTrack, taking a track from the pool
// for it if it has none.
void Emi_SetTrackAttenuation(u8 nEntry, u8 nTrack, f32 fVolume) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &gEmiSources[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->fVolume = fVolume;
}

// Sets the pitch factor of source nEntry's track nTrack, taking a track from the
// pool for it if it has none.
void Emi_SetTrackPitchFactor(u8 nEntry, u8 nTrack, f32 fPitch) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &gEmiSources[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = Trk_AllocPerf(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->fPitch = fPitch;
}

// Prepares each track template of a sound just loaded from a bank (Trk_Check: a sequenced one's
// events). nSound, the sound's number in the bank, is not used.
void Emi_CheckTemplate(AudSound* pSound, u16 nSound) {
    u8 i;

    for (i = 0; i < pSound->nTracks; i++) {
        Trk_Check(&pSound->aTracks[i]);
    }
}

// Passes a track's report on to the emitter instance of this source (the same number) through
// Aud_EmiTrkCB: nReason AUD_TRKCB_FREED or AUD_TRKCB_LOOPED.
void Emi_TrackCallback(AudSource* pSource, u8 nTrack, s32 nReason) {
    Aud_EmiTrkCB((u8)(pSource - gEmiSources), nTrack, nReason);
}
