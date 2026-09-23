// AudTable.c (our name): the sound engine's table of 256 playing sounds (AudTableEntry, 0x7C bytes
// each), allocated from the audio memory stack (UAudMemStack.c's fn_800B5BD8) by fn_800A7AF0 and
// reached through lbl_80282058. Each entry plays a bank sound's tracks and, for a sound placed in
// the world, works out its volume, pan and doppler pitch from its distance to the listeners. Its
// data starts on its own 8-byte boundaries (.sbss 0x80282058, .sdata2 0x80283F88); where its code
// starts before fn_800A7AF0 is not proven.

#include "core/audtrack.h"

AudTrack* fn_800A9BC8(AudTableEntry* pEntry, AudTrackTmpl* pTmpl, u8 nTrack, f32 fVolume);
void fn_800A9D7C(AudTrack* pTrack);
void fn_800A9E7C(AudTableEntry* pEntry, AudTrack* pTrack, AudTrackTmpl* pTmpl, u8 nTrack, int bA,
                 int bB, f32 fVolume);
void fn_800AA2EC(AudTrack* pTrack, s32 n, int b);
void fn_800AA30C(AudTrack* pTrack, s32 n);
void fn_800AA32C(AudTrack* pTrack, s32 n);
void fn_800AA3D4(AudTrackTmpl* pTmpl);
void fn_800ADDC8(s32 nEntry, u8 nTrack, s32 n);
f32  fn_800B1A40(f32* pVec);                     // its length

void fn_800A7E44(AudTableEntry* pEntry);
f32  fn_800A7EA4(f32 fDist, f32 fScale);
void fn_800A7F2C(AudTableEntry* pEntry);
void fn_800A7FA8(AudTableEntry* pEntry);
f32  fn_800A809C(AudTableEntry* pEntry, s32 n);
void fn_800A8134(AudTableEntry* pEntry, u32* auStreams, u16 uMask);

AudTableEntry* lbl_80282058;

int fn_800A7AF0(void) {
    int bOk;
    s32 i;

    bOk = 0;
    lbl_80282058 = fn_800B5BD8(256 * sizeof(AudTableEntry));
    if (lbl_80282058 != NULL) {
        fn_80005AE8(lbl_80282058, 0, 256 * sizeof(AudTableEntry));
        for (i = 0; i < 256; i++) {
            lbl_80282058[i].fPitch = 1.0f;
        }
        bOk = 1;
    }
    return bOk;
}

s32 fn_800A7C24(void) {
    return 1;
}

void fn_800A7C2C(void) {
}

// Starts entry nEntry on sound nSound.
AudTableEntry* fn_800A7C30(u8 nEntry, s16 nSound) {
    AudSound* pSound;
    AudTableEntry* pEntry;

    pSound = fn_800A85CC(nSound);
    pEntry = &lbl_80282058[nEntry];
    fn_80005AE8(pEntry, 0, sizeof(AudTableEntry));
    pEntry->nSound = nSound;
    pEntry->pSound = pSound;
    return pEntry;
}

// Updates entry nEntry's tracks. A placed sound out of earshot is stopped.
void fn_800A7CA4(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, s32 n, u16 uMask) {
    AudTableEntry* pEntry;
    f32 fDist;
    u8 bHeard;
    AudTrack** ppTrack;
    AudTrackTmpl* pTmpl;
    f32 fVolume;
    u8 nTracks;
    u8 i;
    u8 uBit;
    AudTrack* pTrack;
    int bA;
    int bB;

    pEntry = &lbl_80282058[nEntry];
    if (uMask & 7) {
        fn_800A8134(pEntry, auStreams, uMask);
    }
    if (pEntry->pSound->n3 & 1) {
        fDist = fn_800A809C(pEntry, n);
        bHeard = fDist - pEntry->pSound->f4 < 0.0f;
        if (bHeard) {
            pEntry->fDist = fDist;
            fn_800A7FA8(pEntry);
            fn_800A7F2C(pEntry);
        }
    } else {
        bHeard = 1;
    }
    if (bHeard) {
        ppTrack = pEntry->apTracks;
        fVolume = 1.0f;
        nTracks = pEntry->pSound->nTracks;
        pTmpl = pEntry->pSound->aTracks;
        for (i = 0, uBit = 1; i < nTracks; i++, uBit <<= 1, pTmpl++, ppTrack++) {
            pTrack = *ppTrack;
            bA = (uMaskA & uBit) != 0;
            bB = (uMaskB & uBit) != 0;
            if (pEntry->pSound->n3 & 1) {
                fVolume = fn_800A7EA4(fDist, pTmpl->f10);
            }
            fn_800A9E7C(pEntry, pTrack, pTmpl, i, bA, bB, fVolume);
        }
        return;
    }
    fn_800A7E44(pEntry);
}

// Stops every track of an entry.
void fn_800A7E44(AudTableEntry* pEntry) {
    u8 i;

    for (i = 0; i < pEntry->pSound->nTracks; i++) {
        fn_800A9D7C(pEntry->apTracks[i]);
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
void fn_800A7F2C(AudTableEntry* pEntry) {
    f32 fDist;
    u8 uFlags;
    f32 fSpeed;
    f32 fPitch;

    fDist = pEntry->afDist[0];
    uFlags = pEntry->pSound->n3;
    if ((uFlags & 2) || !(uFlags & 1)) {
        fPitch = 1.0f;
    } else {
        if (lbl_80282068 >= 2 && pEntry->afDist[1] < fDist) {
            fDist = pEntry->afDist[1];
        }
        fSpeed = 32.0f * (pEntry->fDist - fDist);
        if (fSpeed > 1.0f) {
            fSpeed = 1.0f;
        }
        fPitch = 345.0f / (345.0f - fSpeed);
    }
    pEntry->fPitch = fPitch;
    pEntry->fDist = fDist;
}

// The pan from where the sound is: with one listener, its side and its front or back; with two
// (split screen) the sound is centred.
void fn_800A7FA8(AudTableEntry* pEntry) {
    f32 fInv;
    f32 fPan;
    f32 f68;

    if (lbl_80282068 < 2 && (pEntry->pSound->n3 & 1)) {
        fInv = 0.0f;
        if (pEntry->afDist[0] > fInv) {
            fInv = 1.0f / pEntry->afDist[0];
        }
        fPan = fn_800A85FC(-pEntry->aPos[0][0], fInv);
        f68 = fn_800A85FC(pEntry->aPos[0][2], fInv);
        fPan = fPan < -1.0f ? -1.0f : fPan > 1.0f ? 1.0f : fPan;
        f68 = f68 < -1.0f ? -1.0f : f68 > 1.0f ? 1.0f : f68;
        pEntry->fPan = fPan;
        pEntry->f68 = f68;
        return;
    }
    pEntry->fPan = 0.0f;
    pEntry->f68 = 1.0f;
}

// Measures the sound's distance from each listener; returns the nearest.
f32 fn_800A809C(AudTableEntry* pEntry, s32 n) {
    f32 fNearest;
    u8 i;
    f32 fDist;

    fNearest = 32768.0f;
    for (i = 0; i < lbl_80282068; i++) {
        fDist = fn_800B1A40(pEntry->aPos[i]);
        pEntry->afDist[i] = fDist;
        if (fNearest > fDist) {
            fNearest = fDist;
        }
    }
    return fNearest;
}

// Sets the play list and stream of each streamed track in uMask: auStreams[i] holds the stream in
// its low 16 bits, the play list above it and the mode in the top byte.
void fn_800A8134(AudTableEntry* pEntry, u32* auStreams, u16 uMask) {
    u8 i;
    u16 uBit;
    AudTrack* pTrack;
    u32 uStream;

    for (i = 0, uBit = 1; i < 8; i++, uBit <<= 1) {
        if (uMask & uBit) {
            pTrack = pEntry->apTracks[i];
            if (pTrack == NULL) {
                pTrack = fn_800A9BC8(pEntry, &pEntry->pSound->aTracks[i], i, 1.0f);
                if (pTrack == NULL) return;
            }
            if (pTrack->pTmpl->n0 & 8) {
                uStream = auStreams[i];
                Stm_SetPlayList(pTrack, uStream >> 16);
                Stm_SetStream(pTrack, uStream, (s32)uStream >> 24);
            }
        }
    }
}

// Stops entry nEntry and frees it.
void fn_800A8200(u8 nEntry) {
    AudTableEntry* pEntry;

    pEntry = &lbl_80282058[nEntry];
    fn_800A7E44(pEntry);
    pEntry->pSound = NULL;
    pEntry->nSound = 0;
}

void fn_800A8248(u8 nEntry, u8 nTrack, s32 n) {
    AudTableEntry* pEntry;
    AudTrackTmpl* pTmpl;
    AudTrack* pTrack;

    pEntry = &lbl_80282058[nEntry];
    pTmpl = &pEntry->pSound->aTracks[nTrack];
    pTrack = pEntry->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pEntry, pTmpl, nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA30C(pTrack, n);
}

void fn_800A82CC(u8 nEntry, u8 nTrack, s32 n) {
    AudTableEntry* pEntry;
    AudTrack* pTrack;

    pEntry = &lbl_80282058[nEntry];
    pTrack = pEntry->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pEntry, &pEntry->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA32C(pTrack, n);
}

void fn_800A834C(s16 nSound, u8 nTrack, s8 n) {
    fn_800A85CC(nSound)->aTracks[nTrack].nA = n;
}

void fn_800A8394(u8 nEntry, u8 nTrack, s32 n, u8 b) {
    AudTableEntry* pEntry;
    AudTrack* pTrack;

    pEntry = &lbl_80282058[nEntry];
    pTrack = pEntry->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pEntry, &pEntry->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    fn_800AA2EC(pTrack, n, b);
}

// Sets a track's volume.
void fn_800A8424(u8 nEntry, u8 nTrack, f32 fVolume) {
    AudTableEntry* pEntry;
    AudTrack* pTrack;

    pEntry = &lbl_80282058[nEntry];
    pTrack = pEntry->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pEntry, &pEntry->pSound->aTracks[nTrack], nTrack, 1.0f);
        if (pTrack == NULL) return;
    }
    pTrack->f44 = fVolume;
}

// Sets a track's pitch.
void fn_800A84A4(u8 nEntry, u8 nTrack, f32 fPitch) {
    AudTableEntry* pEntry;
    AudTrack* pTrack;

    pEntry = &lbl_80282058[nEntry];
    pTrack = pEntry->apTracks[nTrack];
    if (pTrack == NULL) {
        pTrack = fn_800A9BC8(pEntry, &pEntry->pSound->aTracks[nTrack], nTrack, 1.0f);
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

void fn_800A8584(AudTableEntry* pEntry, u8 nTrack, s32 n) {
    fn_800ADDC8((u8)(pEntry - lbl_80282058), nTrack, n);
}
