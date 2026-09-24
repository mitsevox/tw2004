// AudTable.c (our name): the sound engine's table of 256 playing sounds (AudSource, 0x7C bytes
// each), allocated from the audio memory stack (UAudMemStack.c's fn_800B5BD8) by fn_800A7AF0 and
// reached through lbl_80282058. Each entry plays a bank sound's tracks and, for a sound placed in
// the world, works out its volume, pan and doppler pitch from its distance to the listeners. Its
// data starts on its own 8-byte boundaries (.sbss 0x80282058, .sdata2 0x80283F88); where its code
// starts before fn_800A7AF0 is not proven.

#include "core/audtrack.h"

f32  fn_800B1A40(f32* pVec);                     // its length

void fn_800A7E44(AudSource* pSource);
f32  fn_800A7EA4(f32 fDist, f32 fScale);
void fn_800A7F2C(AudSource* pSource);
void fn_800A7FA8(AudSource* pSource);
f32  fn_800A809C(AudSource* pSource, f32 (*aPos)[3]);
void fn_800A8134(AudSource* pSource, u32* auStreams, u16 uMask);

AudSource* lbl_80282058;

u8 fn_800A7AF0(void) {
    u8 bOk;
    s32 i;

    bOk = 0;
    lbl_80282058 = fn_800B5BD8(256 * sizeof(AudSource));
    if (lbl_80282058 != NULL) {
        fn_80005AE8(lbl_80282058, 0, 256 * sizeof(AudSource));
        for (i = 0; i < 256; i++) {
            lbl_80282058[i].fPitch = 1.0f;
        }
        bOk = 1;
    }
    return bOk;
}

u8 fn_800A7C24(void) {
    return 1;
}

void fn_800A7C2C(void) {
}

// Clears entry nEntry and binds it to sound nSound.
AudSource* fn_800A7C30(u8 nEntry, s16 nSound) {
    AudSound* pSound;
    AudSource* pSource;

    pSound = fn_800A85CC(nSound);
    pSource = &lbl_80282058[nEntry];
    fn_80005AE8(pSource, 0, sizeof(AudSource));
    pSource->nSound = nSound;
    pSource->pSound = pSound;
    return pSource;
}

// Updates entry nEntry's tracks. A placed sound out of earshot is stopped.
void fn_800A7CA4(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, f32 (*aPos)[3], u16 uMask) {
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
        fn_800A8134(pSource, auStreams, uMask);
    }
    if (pSource->pSound->n3 & 1) {
        fDist = fn_800A809C(pSource, aPos);
        bHeard = fDist - pSource->pSound->f4 < 0.0f;
        if (bHeard) {
            pSource->fDist = fDist;
            fn_800A7FA8(pSource);
            fn_800A7F2C(pSource);
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
                fVolume = fn_800A7EA4(fDist, pTmpl->f10);
            }
            Trk_UpdatePerf(pSource, pTrack, pTmpl, i, bOn, bOff, fVolume);
        }
        return;
    }
    fn_800A7E44(pSource);
}

// Stops every track of an entry.
void fn_800A7E44(AudSource* pSource) {
    u8 i;

    for (i = 0; i < pSource->pSound->nTracks; i++) {
        Trk_FreePerf(pSource->apTracks[i]);
    }
}

// A placed track's volume by distance: full up to 2, falling to a quarter at 5, then to nothing.
f32 fn_800A7EA4(f32 fDist, f32 fScale) {
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

// The doppler pitch from how fast the sound nears the nearest listener (345: the speed of sound).
void fn_800A7F2C(AudSource* pSource) {
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

// The pan from where the sound is: with one listener, its side and its front or back; with two
// (split screen) the sound is centred.
void fn_800A7FA8(AudSource* pSource) {
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
f32 fn_800A809C(AudSource* pSource, f32 (*aPos)[3]) {
    f32 fNearest;
    u8 i;
    f32 fDist;

    fNearest = 32768.0f;
    for (i = 0; i < lbl_80282068; i++) {
        fDist = fn_800B1A40(pSource->aPos[i]);
        pSource->afDist[i] = fDist;
        if (fNearest > fDist) {
            fNearest = fDist;
        }
    }
    return fNearest;
}

// Sets the play list and stream of each streamed track in uMask: auStreams[i] holds the stream in
// its low 16 bits, the play list above it and the mode in the top byte.
void fn_800A8134(AudSource* pSource, u32* auStreams, u16 uMask) {
    AudTrack* pTrack;
    u32 uStream;
    u8 i;
    u16 uBit;

    for (uBit = 1, i = 0; i < 8; i++, uBit <<= 1) {
        if (uMask & uBit) {
            pTrack = pSource->apTracks[i];
            if (pTrack == NULL) {
                pTrack = fn_800A9BC8(pSource, &pSource->pSound->aTracks[i], i, 1.0f);
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
void fn_800A8200(u8 nEntry) {
    AudSource* pSource;

    pSource = &lbl_80282058[nEntry];
    fn_800A7E44(pSource);
    pSource->pSound = NULL;
    pSource->nSound = 0;
}

void fn_800A8248(u8 nEntry, u8 nTrack, u8 n) {
    AudSource* pSource;
    AudTrackTmpl* pTmpl;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTmpl = &pSource->pSound->aTracks[nTrack];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pSource, pTmpl, nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA30C(pTrack, n);
}

void fn_800A82CC(u8 nEntry, u8 nTrack, u8 n) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA32C(pTrack, n);
}

void fn_800A834C(s16 nSound, u8 nTrack, u8 n) {
    fn_800A85CC(nSound)->aTracks[nTrack].nA = n;
}

void fn_800A8394(u8 nEntry, u8 nTrack, u8 n, int bCheck) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA2EC(pTrack, n, bCheck);
}

// Sets a track's volume.
void fn_800A8424(u8 nEntry, u8 nTrack, f32 fVolume) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->f44 = fVolume;
}

// Sets a track's pitch.
void fn_800A84A4(u8 nEntry, u8 nTrack, f32 fPitch) {
    AudSource* pSource;
    AudTrack* pTrack;

    pSource = &lbl_80282058[nEntry];
    pTrack = pSource->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pSource, &pSource->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->f4C = fPitch;
}

void fn_800A8524(AudSound* pSound, int n) {
    u8 i;

    for (i = 0; i < pSound->nTracks; i++) {
        fn_800AA3D4(&pSound->aTracks[i]);
    }
}

void fn_800A8584(AudSource* pSource, u8 nTrack, s32 n) {
    fn_800ADDC8((u8)(pSource - lbl_80282058), nTrack, n);
}
