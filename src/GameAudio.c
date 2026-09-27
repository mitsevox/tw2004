// GameAudio.c (our name): the game's side of the sound engine. Aud_InitOnce starts the engine
// (memory stack, ARAM, sound table, movie sound, banks) one step after another; the rest drives
// the sound emitters (hlaudemitter.c's fn_800AD*) from the game: the course, the game mode and the
// pin set. Its extent is proven by its data: every section starts and ends on 8-byte boundaries
// shared with no other file (.rodata 0x80183AD8-0x80183B08, .data 0x8018E988-0x8018EB30,
// .bss 0x801F1790-0x801F17D0, .sdata 0x80281418-0x80281460, .sbss 0x80282020-0x80282058,
// .sdata2 0x80283F48-0x80283F88), and all its functions share those globals.

#include "core/gameaudio.h"
#include "core/audtrack.h"

// hlaudmovie.c
void fn_800A874C(s32 n);
void fn_800A87B4(u8 a, u8 n);
void fn_800A8F68(u8 b);
void fn_800A8700(u8 n);
void Mas_SetSubmixChan(u8 nCurve, f32 fVolume);
void Mas_SetSubmixAll(u8 nCurves, f32* pVolumes);

// hlaudemitter.c
void Aud_EmiSetTrackStatus(u8 nId, u8 nTrack, u8 bOn);
u8   Aud_EmiGetTrackStatus(u8 nId, u8 nTrack);
void Aud_EmiSetControllerInt(u8 nId, u8 nTrack, u32 uParams);
void Aud_EmiSetTrackVarRange(u8 nId, u8 nTrack, u8 n);
void Aud_EmiSet3DPos(u8 nId, f32* pPos, f32* pLast, u8 b);
void Aud_EmiSetTrackPitchFactor(u8 nId, u8 nTrack, f32 fPitch);
void Aud_EmiAliasSet3DPos(s16 nKind, f32* pPos, f32* pLast, u8 b);  // types unproven

void fn_8010D3D8(int nPlayer);
u8   fn_8006BEA4(void);                    // emotion.c: a scripted GameBreaker's letterbox is up
void Aud_EmiSetTrackAttenuation(u8 nId, u8 nTrack, f32 fVolume);

void fn_800DC6E8(int nPlayer);
u8   fn_8006BAD8(int nPlayer, s32* pOut);
f32  fn_8006C630(void);
// hlaudemitter.c: makes an emitter for sound nSound; the callback is told when a track stops
// (Aud_EmiTrkCB). The other types are unproven.
u8   Aud_EmiAdd(s16 nSound, s16 nKind, int a, int b, void (*pfnCallback)(u8 nId, u8 nTrack, s32 n));
void Aud_EmiSetTrackVarRangeTmpl(s16 nSound, u8 nTrack, u8 n);
void Aud_EmiSetAllTrackStatus(u8 nId, int n);
void Character_GetBonePos(Character* pChar, int nBone, f32* pPos);
void Gaud_ExitSpecialShot(u8 nPlayer);
void Gaud_ExitCamZoom(u8 nPlayer);
void fn_800A6D48(u8 nPlayer);
void fn_800A714C(void);
void fn_800A71E4(void);
void fn_800A7294(void);
void fn_800BA734(int n, s8 nTrack);

void Aud_Pause(u8 b, u8 b2);
void Aud_SetSubmixAttn(u8 nCurve, f32 fVolume);
void Aud_SetSubmixAll(u8 nCurves, f32* pVolumes);
void FirstFrameInit(void);
void UpdateCommentVolDucking(void);
void UpdateCrowdBuildup(void);
void StartBackgroundMusic(void);
void StartAmbientStreamer(void);
void UpdateStreaming(void);
void InitCrowdBuildup(u8 n);
void fn_800A7220(f32 fAmount);
void Aud_Mute(u8 bLow, u8 bHigh);
void Aud_SetOutputmode(u8 n);
void Aud_MicSetRvbPreset(u8 nIndex, u8 nValue);
void Aud_SesTmplOvrTrackRvbMode(s16 nSound, u8 nTrack, u8 bOn);
void HeartBeatLoopCallback(u8 nId, u8 nTrack, s32 n);
void fn_800A5980(u8 nPlayer);
void Gaud_InitSlowMo(u8 nPlayer, u8 n);
void Gaud_ExitGameBreaker(u8 nPlayer);
void Aud_EmiSetTrackVariation(u8 nId, u8 nTrack, u8 n);
u8   GetAmbientStreamRange(s32 nCourse, int n);
u8   Gaud_ReInit(void);
void Aud_EmiSetTrackStream(u8 nId, u8 nTrack, u8 a, u16 b, s32 c);
u8   Gaud_GetCommentStatus(void);
u8   fn_800A7748(void);
void fn_800A70E4(int n);
void fn_800A7198(int n);

// Each volume curve's volume (Gaud_ReInit hands them to hlaudmovie.c).
f32 lbl_8018E988[32] = {
    0.3f, 0.7f, 0.8f, 1.0f, 0.7f, 0.4f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.4f,
    1.5f, 1.5f, 1.5f, 1.4f, 1.0f, 1.0f, 0.4f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

GameAudioCourseSound lbl_8018EA08[9] = {
    { 12, 3, 0 },
    { 12, 4, 0 },
    { 12, 9, 0 },
    { 12, 10, 0 },
    { 12, 11, 0 },
    { 15, 1, 1 },
    { 15, 18, 1 },
    { 18, 2, 2 },
    { 18, 18, 2 },
};

// fn_800A5980: the swing sound per club (Player.nClub) and per lie (Ball.nLie).
const u8 lbl_80183AD8[28] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0,
};
const u8 lbl_80183AF4[20] = {
    0, 0, 0, 1, 2, 3, 4, 4, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GameAudioView lbl_801F1790[2];

u8 lbl_80281418 = 0xFF;
u8 lbl_80281419 = 0xFF;
u8 lbl_8028141A = 0xFF;
u8 lbl_8028141B = 0xFF;
u8 lbl_8028141C = 0xFF;
u8 lbl_8028141D = 0xFF;
u8 lbl_8028141E = 0xFF;
u8 lbl_8028141F = 0xFF;
u8 lbl_80281420 = 0xFF;
s32 lbl_80281424 = -1;
s32 lbl_80281428 = -1;
u8 lbl_8028142C = 0xFF;
u8 lbl_8028142D = 0xFF;
f32 lbl_80281430 = 1.0f;
f32 lbl_80281434 = 0.6f;
f32 lbl_80281438 = 4.0f;
f32 lbl_8028143C = 4.0f;
f32 lbl_80281440 = 0.8f;
f32 lbl_80281444 = 0.75f;
f32 lbl_80281448 = 1.5f;
f32 lbl_8028144C = 0.03f;
f32 lbl_80281450 = 25.0f;
f32 lbl_80281454 = 2.0f;
f32 lbl_80281458 = 21.0f;

u32 lbl_80282054;                       // frames left before the queued sound starts (Gaud_Cycle)
u8 lbl_80282052;                        // } the queued sound: Aud_EmiSetTrackStream's arguments
u16 lbl_80282050;                       // }
s32 lbl_8028204C;                       // }
u32 lbl_80282048;                       // Gaud_CameraShake: the frame it last played
f32 lbl_80282044;
u8 lbl_80282042;
u8 lbl_80282041;
u8 lbl_80282040;
s32 lbl_8028203C;                       // what Gaud_SetStreamingContext plays: 0 nothing, 1 music, 2 ambience
u8 lbl_80282038;                        // a sound is queued
s32 lbl_80282034;
u8 lbl_80282033;
u8 lbl_80282032;
u8 lbl_80282031;
u8 lbl_80282030;
u8 lbl_8028202F;
u8 lbl_8028202E;
u8 lbl_8028202D;
u8 lbl_8028202C;
u8 lbl_8028202B;
u8 lbl_8028202A;
u8 lbl_80282029;
u8 lbl_80282028;
u8 lbl_80282024[4];                     // 4 bytes in the original; Aud_MicInitOnce clears only two
u8 lbl_80282020;

// startUp.c: the sound engine's start-up steps, each nonzero when it worked
u8   fn_800AFAB0(void);
u8   fn_800B0438(void);
u8   fn_800B0568(void);
u8   fn_800B0798(void);
void fn_800B07A0(void);
void fn_800B0858(u8 nSound);

// hlaudmovie.c
u8   fn_800A8604(void);
void fn_800A86BC(u8 nRate);
u8   fn_800A8754(void);
u8   fn_800A8824(void);
u8   fn_800A8D2C(void);
void fn_800A8D88(void);
u8   Ses_Init(u8 a, u8 b, u8 nListeners);
void Mov_Init(void);
void Mov_Exit(void);
void Mov_Start(void);
void Mov_Tick(void);

u8   Voc_InitModule(void);                   // hlaudvoice.c
u8   Aud_EmiInitOnce(void);                  // hlaudemitter.c; always 1
u8   fn_800AF224(void);                      // AudReverb.c; always 1
void Aud_EmiCycle(void);                     // hlaudemitter.c
void Aud_EmiExitSession(void);                   // hlaudemitter.c
void fn_800B5B80(void);                   // UAudMemStack.c
void Aud_EmiSetTrackStep(u8 nId, u8 nTrack, u8 n, int bCheck);

u8   Aud_MicInitOnce(void);
void Gaud_StopMusic(void);
void Gaud_InitSpecialShot(u8 nPlayer);
void fn_800A6EC8(void);
void fn_800A73C0(u8 a, int n);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283F48), before the 0.2f and the int-to-float double UpdateCommentVolDucking uses first; its
// body is unknown.
static f32 GameAudio_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Starts the sound engine one step after another; stops at the first step that fails and
// returns 0, else 1.
u8 Aud_InitOnce(u8 nRate) {
    u8 bOk;

    fn_800B5B80();
    if ((bOk = fn_800AFAB0()) && (bOk = fn_800B0438()) && (bOk = fn_800B0568())
        && (bOk = fn_800AF224()) && (bOk = fn_800B0798()) && (bOk = fn_800A8604())
        && (bOk = fn_800A8D2C()) && (bOk = Emi_InitModule()) && (bOk = Trk_InitModule())
        && (bOk = fn_800AAD18()) && (bOk = fn_800ABBC8()) && (bOk = fn_800A8754())
        && (bOk = Voc_InitModule()) && (bOk = fn_800A8824()) && (bOk = Aud_EmiInitOnce())
        && (bOk = Aud_MicInitOnce())) {
        fn_800B07A0();
        fn_800A86BC(nRate);
        bOk = 1;
    }
    return bOk;
}

// Pauses (b 1) or resumes (b 0) all sound. b2 is TW07's spinupDelay: the disc-error check
// (Code800B7210.c's fn_800B7490) passes 1, this file 0; nothing here reads it.
void Aud_Pause(u8 b, u8 b2) {
    fn_800A8F68(b);
}

// Sets the master mute mask: bLow mutes the low 16 channels, bHigh the high 16 (TW07 calls them
// global and local); a flag of 0 unmutes its half.
void Aud_Mute(u8 bLow, u8 bHigh) {
    s32 nMask;

    nMask = 0;
    if (bLow) {
        nMask |= 0xFFFF;
    }
    if (bHigh) {
        nMask |= 0xFFFF0000;
    }
    fn_800A874C(nMask);
}

void Aud_SetOutputmode(u8 n) {
    fn_800A8700(n);
}

void Aud_SetSubmixAttn(u8 nCurve, f32 fVolume) {
    Mas_SetSubmixChan(nCurve, fVolume);
}

void Aud_SetSubmixAll(u8 nCurves, f32* pVolumes) {
    Mas_SetSubmixAll(nCurves, pVolumes);
}

// The listeners' (EA: microphones') set-up at boot, the last step of Aud_InitOnce: no listeners and
// reverb preset 0 for both. Always 1.
u8 Aud_MicInitOnce(void) {
    Mem_set(lbl_80282024, 0, 2);
    lbl_80282020 = 0;
    return 1;
}

// A session (the front end or a hole) has n listeners, one per view (Aud_InitSession). Always 1.
s32 Aud_MicInitSession(u8 n) {
    lbl_80282020 = n;
    return 1;
}

void Aud_MicExitSession(void) {
    lbl_80282020 = 0;
}

// Gives listener nIndex reverb preset nValue, passing it on to hlaudmovie.c (fn_800A87B4) only when
// it changes. Gaud_InitHole uses preset 17 on course 7's hole index 2.
void Aud_MicSetRvbPreset(u8 nIndex, u8 nValue) {
    if (lbl_80282024[nIndex] != nValue) {
        lbl_80282024[nIndex] = nValue;
        fn_800A87B4(nIndex, nValue);
    }
}

void Aud_MicCycle(void) {
}

// On a hole's first frame (Gaud_Monitor): outside start-up and the front end (game types 0, 1, 3)
// starts the course's ambient sounds: the crowd's idle murmur (crowd reaction 0), the wind and the
// trees at the wind option's strength, the rain when fn_80035574 (at fn_8006C630's strength), and
// the flag flapping at the pin (sound 9, lbl_80281420, by the wind). Then lets Gaud_Monitor run the
// rest (lbl_80282029).
void FirstFrameInit(void) {
    u8 nWind;

    if (gSession.nGameType != 3 && gSession.nGameType != 1 && gSession.nGameType != 0) {
        Game_GetCurHoleNum();
        nWind = gSession.options.nWind;
        Gaud_InitCrowdReactionSound(0, 0);
        fn_800A70E4(nWind);
        fn_800A7198(nWind);
        if (fn_80035574()) {
            fn_800A7220(fn_8006C630());
        }
        lbl_80281420 = Aud_EmiAdd(9, -1, 1, 0, NULL);
        Aud_EmiSet3DPos(lbl_80281420, &gPlayers[0].ball.pCourse->pin[Game_CurrentPinSet()].x, NULL, 0);
        Aud_EmiSetTrackVarRangeTmpl(9, 0, nWind);
    }
    lbl_80282029 = 1;
}

// The GameBreaker heartbeat's emitter callback (Gaud_InitHole hands it to Aud_EmiAdd): each report
// of track 2, the heartbeat, rumbles the controller of view 0's player once (GameEffects.c
// fn_800DC6E8, up to 40 beats). Gaud_InitGameBreaker calls it for the first beat.
void HeartBeatLoopCallback(u8 nId, u8 nTrack, s32 n) {
    if (nTrack == 2) {
        fn_800DC6E8(ViewController_GetPlayer(0));
    }
}

// Once a frame (Gaud_Monitor): while commentary plays, the music (curve 15) drops to lbl_80281434
// (0.6) of its option volume, and goes back when it ends. The volume only changes while music is
// what streams (lbl_8028203C 1).
void UpdateCommentVolDucking(void) {
    if (Gaud_GetCommentStatus()) {
        if (lbl_80282031 == 0) {
            if (lbl_8028203C == 1) {
                Aud_SetSubmixAttn(15, lbl_8018E988[15] * (0.2f * (s8)gSession.options.a0[1] * lbl_80281434));
            }
            lbl_80282031 = 1;
        }
    } else if (lbl_80282031 != 0) {
        if (lbl_8028203C == 1) {
            Aud_SetSubmixAttn(15, 0.2f * (s8)gSession.options.a0[1] * lbl_8018E988[15]);
        }
        lbl_80282031 = 0;
    }
}

// The crowd's build-up as the ball nears the hole: variation range n on tracks 2 and 3 of both
// crowd emitters (lbl_8028141C / lbl_8028141D). Not during the GameBreaker (lbl_8028202F) or in a
// mode without a crowd (lbl_80282040 0).
void InitCrowdBuildup(u8 n) {
    if (lbl_8028202F || !lbl_80282040) return;
    Aud_EmiSetTrackVarRange(lbl_8028141C, 2, n);
    Aud_EmiSetTrackVarRange(lbl_8028141D, 2, n);
    Aud_EmiSetTrackVarRange(lbl_8028141C, 3, n);
    Aud_EmiSetTrackVarRange(lbl_8028141D, 3, n);
    Aud_EmiSetTrackStatus(lbl_8028141C, 2, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 2, 1);
    Aud_EmiSetTrackStatus(lbl_8028141C, 3, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 3, 1);
}

// Stops the crowd's build-up (tracks 2 and 3 of both crowd emitters), in a mode with a crowd.
void ExitCrowdBuildup(void) {
    if (lbl_80282040) {
        Aud_EmiSetTrackStatus(lbl_8028141C, 2, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 2, 0);
        Aud_EmiSetTrackStatus(lbl_8028141C, 3, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 3, 0);
    }
}

// Once a frame (Gaud_Monitor), after a swing (lbl_80282033), in a mode with a crowd and while no
// crowd reaction is due or held: asks emotion.c (fn_8006BAD8) how the current player's ball is
// doing near the hole. When it has the result, plays crowd reaction n + 4 (once a shot,
// lbl_80282030); else, when the closeness n changed, raises the build-up to n - 1 (at least 0).
void UpdateCrowdBuildup(void) {
    s32 n;

    if (lbl_80282040 && lbl_80282033 && !lbl_80282030 && !lbl_8028202F && !lbl_80282032
        && lbl_80281428 == -1) {
        if (fn_8006BAD8(lbl_80282278, &n)) {
            Gaud_InitCrowdReactionSound((u8)(n + 4), 1);
            lbl_80282030 = 1;
        } else if (n >= 0) {
            if (--n < 0) {
                n = 0;
            }
            InitCrowdBuildup(n);
        }
    }
}

// Picks what streams, and records it in lbl_8028203C (0 nothing, 1 music, 2 ambience). Sets the
// music volume (curve 15, option a0[1]) and the SFX level first. In the front end (game types 3 and
// 10) the music is sound 2 on the options' row 0; in play, sound 6 on the game mode's row (1 to 3),
// and only with two players at most and outside replays. The music plays when its volume is above 0
// and its row is on (the row is not asked when gSession.a8[0] is set without flag 0x4000). When
// in-game music does not play, the course ambience (sound 8, placed at (0, 0, 12)) streams instead
// if gpGame->b288 and the SFX volume is above 0; UpdateStreaming starts it.
void Gaud_SetStreamingContext(void) {
    int nMode;
    f32 fVolume;
    u8 bFixed;
    u8 bNoBreaker;
    s32 nState;
    s16 nSound;
    u8 bOn;
    u8 bMusic;
    u8 nRow;
    f32 vPos[3];

    nMode = Game_GetMode();
    fVolume = 0.2f * (s8)gSession.options.a0[1];
    bFixed = gSession.nGameType == 3 || gSession.nGameType == 10;
    bNoBreaker = !bFixed && gSession.a8[0];
    nState = 0;
    bOn = fVolume > 0.0f;
    Aud_SetSubmixAttn(15, fVolume * lbl_8018E988[15]);
    fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
    if (bFixed) {
        nSound = 2;
        bMusic = 1;
        nRow = 0;
    } else {
        nSound = 6;
        bMusic = gSession.nNumPlayers <= 2 && !gSession.bReplay;
        bOn = bOn && bMusic;
        switch (nMode) {
        case 0:
        case 1:
        case 2:
        case 9:
        case 18:
        case 19:
        case 20:
        case 21:
            nRow = 1;
            break;
        case 6:
        case 7:
        case 8:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 22:
        case 25:
        case 26:
            nRow = 2;
            break;
        default:
            nRow = 3;
            break;
        }
    }
    if (!bNoBreaker || (gSession.uFlags & 0x4000)) {
        bOn = bOn && gSession.options.abRowOn[nRow];
    }
    if (bOn) {
        if (lbl_8028203C == 2) {
            Aud_EmiDel(lbl_8028141A);
            lbl_8028141A = 0xFF;
        }
        if (bMusic) {
            nState = 1;
            if (lbl_80281418 == 0xFF) {
                lbl_80281418 = Aud_EmiAdd(nSound, -1, 1, 1, NULL);
            }
            lbl_8028142C = nRow;
        }
    } else {
        if (lbl_8028203C == 1) {
            Aud_EmiDel(lbl_80281418);
            lbl_80281418 = 0xFF;
        }
        if (bMusic && !bFixed && gpGame->b288 && (s8)gSession.options.a0[0] > 0) {
            nState = 2;
            if (lbl_8028141A == 0xFF) {
                vPos[0] = 0.0f;
                vPos[1] = 0.0f;
                vPos[2] = 12.0f;
                lbl_8028141A = Aud_EmiAdd(8, -1, 1, 1, NULL);
                Aud_EmiSet3DPos(lbl_8028141A, vPos, NULL, 0);
            }
            if (lbl_8028203C != 2) {
                lbl_80282041 = 1;
            }
        }
    }
    lbl_8028203C = nState;
}

// Steps to the next track switched on in the options' row lbl_8028142C (19 tracks) and plays it.
void StartBackgroundMusic(void) {
    u8 i;
    int nCount = 0;

    if (lbl_8028203C == 1) {
        // fake match: the original keeps an empty 19-step loop here; a count nothing reads
        // reproduces it (what EA's loop did is lost)
        for (i = 0; i < 19; i++) {
            if (gSession.options.rows[lbl_8028142C][i]) {
                nCount++;
            }
        }
        for (i = 0; i < 19; i++) {
            if (++lbl_8028142D >= 19) {
                lbl_8028142D = 0;
            }
            if (gSession.options.rows[lbl_8028142C][lbl_8028142D]) {
                Gaud_StartMusic(13, lbl_8028142D);
                return;
            }
        }
    }
}

// When ambience is what streams (lbl_8028203C 2): starts the ambience emitter's tracks 0 and 1,
// track 2 when fn_80035574 (the course flag that also starts the rain sound), track 3 by course and
// track 4 by course and hole (GetAmbientStreamRange).
void StartAmbientStreamer(void) {
    int nCourse;
    u8 n;
    u8 nSound;

    if (lbl_8028203C == 2) {
        nCourse = Game_GetCourse();
        n = Game_GetCurHoleNum();
        Aud_EmiSetTrackAttenuation(lbl_8028141A, 0, 1.0f);
        Aud_EmiSetTrackStatus(lbl_8028141A, 1, 1);
        if (fn_80035574()) {
            Aud_EmiSetTrackStatus(lbl_8028141A, 2, 1);
        }
        Aud_EmiSetTrackVarRange(lbl_8028141A, 3, nCourse);
        Aud_EmiSetTrackStatus(lbl_8028141A, 3, 1);
        nSound = GetAmbientStreamRange(nCourse, n);
        if (nSound != 0xFF) {
            Aud_EmiSetTrackVarRange(lbl_8028141A, 4, nSound);
            Aud_EmiSetTrackStatus(lbl_8028141A, 4, 1);
        }
    }
}

// Once a frame: for ambience, starts its tracks when asked (lbl_80282041) and no ambient stream is
// playing; for music, starts the next track when the current one has ended.
void UpdateStreaming(void) {
    switch (lbl_8028203C) {
    case 2:
        if (lbl_80282041 != 0 && lbl_8028202D == 0 && !fn_800A7748()) {
            lbl_80282041 = 0;
            StartAmbientStreamer();
        }
        break;
    case 1:
        if (!fn_800A75F4()) {
            StartBackgroundMusic();
        }
        break;
    }
}

// Stops the ambience's tracks; track 0 too unless bKeepFirst.
void StopAmbientStreamer(u8 bKeepFirst) {
    if (lbl_8028203C == 2) {
        if (!bKeepFirst) {
            Aud_EmiSetTrackStatus(lbl_8028141A, 0, 0);
        }
        Aud_EmiSetTrackStatus(lbl_8028141A, 1, 0);
        Aud_EmiSetTrackStatus(lbl_8028141A, 2, 0);
        Aud_EmiSetTrackStatus(lbl_8028141A, 3, 0);
        Aud_EmiSetTrackStatus(lbl_8028141A, 4, 0);
    }
}

// The variation range the ambience's track 4 plays on course nCourse, hole index n
// (StartAmbientStreamer passes Game_GetCurHoleNum(); the table lbl_8018EA08 lists hole numbers, n +
// 1), or 0xFF when the hole has none.
u8 GetAmbientStreamRange(s32 nCourse, int n) {
    u8 nSound;
    int i;

    nSound = 0xFF;
    for (i = 0; i < 9; i++) {
        if (nCourse == lbl_8018EA08[i].nCourse && (u8)(n + 1) == lbl_8018EA08[i].n4) {
            nSound = lbl_8018EA08[i].nSound;
            break;
        }
    }
    return nSound;
}

// Sets the game's sound back to nothing (no emitters, both views' emitters cleared, every flag off,
// crowd volume 1, music ducking 0.6, crowd pair 21 either side), then starts the sound engine
// (Aud_InitOnce, 60 frames a second) and gives every volume curve its level (lbl_8018E988). Always
// 1.
u8 Gaud_ReInit(void) {
    int i;

    for (i = 0; i < 2; i++) {
        lbl_801F1790[i].n0 = 0xFF;
        lbl_801F1790[i].n1 = 0xFF;
        lbl_801F1790[i].n2 = 0xFF;
        lbl_801F1790[i].n3 = 0xFF;
        lbl_801F1790[i].n4 = 0;
        lbl_801F1790[i].n5 = 0;
        lbl_801F1790[i].f8 = 0.0f;
        lbl_801F1790[i].fC = 0.0f;
        lbl_801F1790[i].tLast = 0;
    }
    lbl_80281418 = 0xFF;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_8028141B = 0xFF;
    lbl_8028141C = 0xFF;
    lbl_8028141D = 0xFF;
    lbl_8028141E = 0xFF;
    lbl_8028141F = 0xFF;
    lbl_80281420 = 0xFF;
    lbl_80282028 = 0;
    lbl_8028202A = 1;
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202D = 0;
    lbl_8028202E = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    lbl_8028203C = 0;
    lbl_80282040 = 0;
    lbl_80282041 = 0;
    lbl_80281430 = 1.0f;
    lbl_80281434 = 0.6f;
    lbl_80281458 = 21.0f;
    Aud_InitOnce(60);
    Aud_SetSubmixAll(32, lbl_8018E988);
    return 1;
}

int Gaud_InitOnce(void) {
    return Gaud_ReInit() != 0;
}

// Once a frame, also from the loading, movie and memory-card loops: runs the sound engine
// (Aud_EmiCycle) and, when the 15 frames after Gaud_StopComment run out, plays the commentary line
// Gaud_StartComment queued meanwhile.
void Gaud_Cycle(void) {
    Aud_EmiCycle();
    Aud_MicCycle();
    if (lbl_80282054 != 0) {
        if (--lbl_80282054 == 0 && lbl_80282038) {
            Aud_EmiSetTrackStream(lbl_80281419, 0, lbl_80282052, lbl_80282050, lbl_8028204C);
            Aud_EmiSetTrackStatus(lbl_80281419, 0, 1);
            lbl_80282038 = 0;
        }
    }
}

// Once a frame in play (gomainloop): on a hole's first frame FirstFrameInit; after it, re-picks the
// stream when asked (lbl_8028202C: Gaud_Pause on resuming), and updates the crowd build-up, the
// music ducking under commentary and the streams (UpdateStreaming).
void Gaud_Monitor(void) {
    if (lbl_8028202A) {
        lbl_8028202A = 0;
        FirstFrameInit();
    }
    if (lbl_80282029) {
        if (lbl_8028202C) {
            Gaud_SetStreamingContext();
            lbl_8028202C = 0;
        }
        UpdateCrowdBuildup();
        UpdateCommentVolDucking();
        UpdateStreaming();
    }
}

// Starts a world object's sound. Kinds 0, 3 and 5 play as a pair of emitters lbl_80281450 either
// side of the listener (kind 0 only once, lbl_80282042); the others play at the object. Kinds
// below 3 need gpGame->b288.
void fn_800A4CB8(GameAudioSource** ppSource, int n) {
    GameAudioSource* pSource;
    u32 nKind;
    u8 bPlay;
    u8 nParam;
    u8 nId;
    f32 vPos[3];

    pSource = *ppSource;
    nKind = pSource->nKind;
    if ((nKind >= 3 || gpGame->b288) && pSource->nSound != 0) {
        nParam = n;
        switch (nKind) {
        case 0:
        case 3:
        case 5:
            bPlay = 1;
            if (nKind == 0) {
                if (lbl_80282042) {
                    bPlay = 0;
                } else {
                    lbl_80282042 = 1;
                }
            }
            if (bPlay) {
                nId = Aud_EmiAdd(pSource->nSound, pSource->nKind, nParam, 1, NULL);
                vPos[0] = -lbl_80281450;
                vPos[1] = 0.0f;
                vPos[2] = 0.0f;
                Aud_EmiSet3DPos(nId, vPos, NULL, 0);
                vPos[0] = lbl_80281450;
                nId = Aud_EmiAdd(pSource->nSound, pSource->nKind, nParam, 1, NULL);
                Aud_EmiSet3DPos(nId, vPos, NULL, gSession.nSplitScreen != 0);
            }
            break;
        default:
            nId = Aud_EmiAdd(pSource->nSound, nKind, nParam, 0, NULL);
            vPos[0] = pSource->vPos[0];
            vPos[1] = pSource->vPos[1];
            vPos[2] = pSource->vPos[2];
            Aud_EmiSet3DPos(nId, vPos, NULL, 0);
            break;
        }
    }
}

void fn_800A4E34(void) {
    f32 vPos[3];

    lbl_8028203C = 0;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_80281418 = 0xFF;
    if (lbl_80282028 == 0) {
        // the options' volumes, 0.2 per step
        fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
        fn_800A78F0(0.2f * (s8)gSession.options.a0[4]);
        Aud_SetSubmixAttn(15, 0.2f * (s8)gSession.options.a0[1] * lbl_8018E988[15]);
        Aud_SetOutputmode(2);
        lbl_80282028 = 1;
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    lbl_8028141B = Aud_EmiAdd(1, -1, 1, 1, NULL);
    Aud_EmiSet3DPos(lbl_8028141B, vPos, NULL, 0);
    Aud_EmiSetTrackStatus(lbl_8028141B, 0, 1);
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202E = 0;
    lbl_8028202D = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    lbl_8028202A = 1;
    lbl_80282041 = 0;
    lbl_80282040 = 0;
}

void fn_800A4FD8(void) {
    Gaud_StopMusic();
    lbl_8028141B = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_80282029 = 0;
}

// Sets up the round's sounds: each view's emitters (0 the swing, 1 the ball, 2 and 3 a pair
// either side), the music and the ambience.
void fn_800A500C(void) {
    GameAudioView* pView;
    Player* pPlayer;
    int nViews;
    int nCourse;
    int nMode;
    u8 n;
    int i;
    f32 vPos[3];

    nViews = (gSession.nSplitScreen != 0) + 1;
    nCourse = Game_GetCourse();
    nMode = Game_GetMode();
    n = Game_GetCurHoleNum();
    pView = lbl_801F1790;
    pPlayer = gPlayers;
    if (lbl_80282028 == 0) {
        fn_800A78F0(0.2f * (s8)gSession.options.a0[4]);
        Aud_SetOutputmode(2);
        lbl_80282028 = 1;
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    for (i = 0; i < nViews; i++, pPlayer++, pView++) {
        pView->n0 = Aud_EmiAdd(1, -1, 0, 0, NULL);
        Aud_EmiSet3DPos(pView->n0, pPlayer->vBall, NULL, 0);
        pView->n1 = Aud_EmiAdd(2, -1, 0, 0, NULL);
        Aud_EmiSet3DPos(pView->n1, pPlayer->ball.vPos, NULL, 0);
        Aud_EmiSetTrackStatus(pView->n1, 0, 1);
        pView->n2 = Aud_EmiAdd(4, -1, 1, 1, HeartBeatLoopCallback);
        pView->n3 = Aud_EmiAdd(4, -1, 1, 1, NULL);
        vPos[0] = -lbl_80281454;
        vPos[1] = 0.0f;
        vPos[2] = 0.0f;
        Aud_EmiSet3DPos(pView->n2, vPos, NULL, 0);
        vPos[0] = lbl_80281454;
        Aud_EmiSet3DPos(pView->n3, vPos, NULL, 0);
    }
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 1.0f;
    lbl_8028141B = Aud_EmiAdd(10, -1, 1, 1, NULL);
    Aud_EmiSet3DPos(lbl_8028141B, vPos, NULL, 0);
    lbl_8028203C = 0;
    lbl_80281419 = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_80282041 = 0;
    vPos[0] = 0.0f;
    vPos[1] = 0.0f;
    vPos[2] = 10.0f;
    lbl_80281419 = Aud_EmiAdd(5, -1, 1, 1, NULL);
    Aud_EmiSet3DPos(lbl_80281419, vPos, NULL, 0);
    fn_800A77E0(0.2f * (s8)gSession.options.a0[0]);
    Gaud_SetStreamingContext();
    lbl_8028141C = Aud_EmiAdd(7, -1, 1, 1, NULL);
    vPos[0] = lbl_80281458;
    vPos[1] = 0.0f;
    vPos[2] = 0.0f;
    Aud_EmiSet3DPos(lbl_8028141C, vPos, NULL, 0);
    lbl_8028141D = Aud_EmiAdd(7, -1, 1, 1, NULL);
    vPos[0] = -lbl_80281458;
    Aud_EmiSet3DPos(lbl_8028141D, vPos, NULL, 0);
    if (nCourse == 7 && n == 2) {
        Aud_MicSetRvbPreset(0, 17);
        Aud_SesTmplOvrTrackRvbMode(2, 0, 1);
        Aud_SesTmplOvrTrackRvbMode(1, 0, 1);
        Aud_SesTmplOvrTrackRvbMode(1, 2, 1);
        Aud_SesTmplOvrTrackRvbMode(1, 1, 1);
    }
    Aud_Pause(0, 0);
    lbl_8028202A = 1;
    lbl_8028202B = 0;
    lbl_8028202C = 0;
    lbl_8028202E = 0;
    lbl_8028202D = 0;
    lbl_8028202F = 0;
    lbl_80282032 = 0;
    lbl_80281424 = -1;
    lbl_80281428 = -1;
    lbl_80282030 = 0;
    lbl_80282031 = 0;
    lbl_80282033 = 0;
    lbl_80282034 = 0;
    // the modes 0-2, 4, 5, 10, 18-21, 23-25 (bit mask 0x03BC0437)
    lbl_80282040 = ((1 << nMode) & 0x03BC0437) != 0;
}

// Stops the game's sounds and clears every view's emitters.
void fn_800A5428(void) {
    int nViews;
    int i;

    nViews = gSession.nSplitScreen ? 2 : 1;
    lbl_80282042 = 0;
    if (lbl_8028203C == 2) {
        Aud_EmiDel(lbl_8028141A);
        lbl_8028141A = 0xFF;
    } else {
        Gaud_StopMusic();
    }
    Aud_EmiDel(lbl_80281419);
    lbl_80281419 = 0xFF;
    fn_800A6EC8();
    Gaud_ExitCamZoom(0);
    Gaud_ExitGameBreaker(0);
    fn_800A6D48(0);
    Gaud_ExitSpecialShot(0);
    fn_800A707C();
    fn_800A714C();
    fn_800A7294();
    fn_800A71E4();
    for (i = 0; i < nViews; i++) {
        lbl_801F1790[i].n0 = 0xFF;
        lbl_801F1790[i].n1 = 0xFF;
        lbl_801F1790[i].n2 = 0xFF;
        lbl_801F1790[i].n3 = 0xFF;
        lbl_801F1790[i].n4 = 0;
        lbl_801F1790[i].n5 = 0;
        lbl_801F1790[i].f8 = 0.0f;
        lbl_801F1790[i].fC = 0.0f;
        lbl_801F1790[i].tLast = 0;
    }
    lbl_80281419 = 0xFF;
    lbl_80281418 = 0xFF;
    lbl_8028141C = 0xFF;
    lbl_8028141D = 0xFF;
    lbl_8028141E = 0xFF;
    lbl_8028141F = 0xFF;
    lbl_8028141A = 0xFF;
    lbl_8028141B = 0xFF;
    lbl_80281420 = 0xFF;
    lbl_80282029 = 0;
    Aud_Mute(0, 0);
}

void fn_800A5620(void) {
    lbl_8028202D = 1;
}

// Resets the swing sound state and parks the view's swish emitter (emitter 0) at the club head
// (bone 0x53), silent, before the swing meter starts.
void fn_800A562C(u8 nPlayer) {
    GameAudioView* pView;
    Player* pPlayer;
    u8 nId;
    f32 vPos[3];

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    pPlayer = &gPlayers[nPlayer];
    lbl_80282030 = 0;
    lbl_80282032 = 0;
    nId = pView->n0;
    lbl_80282034 = 0;
    lbl_8028202F = 0;
    lbl_80281424 = -1;
    fn_800A6EC8();
    Gaud_ExitSpecialShot(nPlayer);
    Aud_EmiSetAllTrackStatus(pView->n2, 0);
    Aud_EmiSetAllTrackStatus(pView->n3, 0);
    if (fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut) {
        Aud_EmiDel(lbl_80281420);
        lbl_80281420 = 0xFF;
    }
    Character_GetBonePos(pPlayer->pChar, 0x53, vPos);
    Aud_EmiSet3DPos(nId, vPos, NULL, 0);
    Aud_EmiSetTrackStep(nId, 0, 1, 1);
    pView->f8 = 0.0f;
    pView->fC = 1.0f / FRAME_RATE;
    pView->n18 = 0;
}

// The swish of the club in the backswing and downswing (swing states 1 and 3): its pitch and
// volume follow how fast the club head (bone 0x53) moves.
void fn_800A573C(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    u8 nId;
    int nState;
    s32 nLastState;
    f32 fSpeed;
    f32 fPitch;
    f32 fVolume;
    f32 vPos[4];
    f32 vLast[4];

    pPlayer = &gPlayers[nPlayer];
    pView = &lbl_801F1790[pPlayer->nView[0]];
    nId = pView->n0;
    if (nId != 0xFF) {
        Character_GetBonePos(pPlayer->pChar, 0x53, vPos);
        Aud_EmiSet3DPos(nId, vPos, vLast, 0);
        nState = pPlayer->swing.nState;
        if (nState == 1 || nState == 3) {
            if (pView->fC > 0.0f) {
                nLastState = pView->n18;
                if (nLastState == 1 && nLastState != nState) {
                    Aud_EmiSetTrackStep(nId, 0, 1, 1);
                } else {
                    fSpeed = Vec_Distance(vPos, vLast) / (FRAME_RATE * pView->fC);
                    fPitch = fSpeed * lbl_80281438;
                    fVolume = fSpeed * lbl_8028143C;
                    fPitch = (fPitch <= lbl_80281444) ? lbl_80281444 : fPitch;
                    fPitch = (fPitch <= lbl_80281448) ? fPitch : lbl_80281448;
                    fVolume = (fVolume <= lbl_80282044) ? lbl_80282044 : fVolume;
                    fVolume = (fVolume <= lbl_80281440) ? fVolume : lbl_80281440;
                    if (pPlayer->nClub > 17) {
                        fVolume *= 0.35f;
                    }
                    if (pPlayer->swing.nState == 1) {
                        fVolume *= 0.35f;
                    }
                    Aud_EmiSetTrackStep(nId, 0, 0, 1);
                    Aud_EmiSetTrackAttenuation(nId, 0, fVolume);
                    Aud_EmiSetTrackPitchFactor(nId, 0, fPitch);
                }
                pView->n18 = pPlayer->swing.nState;
            }
        } else {
            Aud_EmiSetTrackStep(nId, 0, 1, 1);
        }
        if (lbl_80202898.bSlowMo) {
            pView->fC = gSession.fFrameTime / lbl_80202898.fSlowMo;
        } else {
            pView->fC = gSession.fFrameTime;
        }
    }
}

// The swing: its sound by club and lie on the view's emitter 0. For a few calls after Gaud_InitSpecialShot
// set lbl_80282034 it plays track 4 of the view's emitters 2 and 3 instead.
void fn_800A5980(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    Clip* pClip;
    u64 uName;
    int nKind;
    int nMode;
    u8 nIdSwing;
    u8 nIdA;
    u8 nIdB;
    u8 bRestore;
    u8 n;
    u8 nCrowd;

    pPlayer = &gPlayers[nPlayer];
    pView = &lbl_801F1790[pPlayer->nView[0]];
    nIdSwing = pView->n0;
    nIdA = pView->n2;
    nIdB = pView->n3;
    nKind = fn_800C7138(ViewController_GetCameraController(pPlayer->nView[0]));
    bRestore = 1;
    nMode = Game_GetMode();
    if (fn_8006BEA4()) {
        Gaud_InitSlowMo(nPlayer, 0);
        bRestore = 0;
    }
    if (lbl_80282034 > 0) {
        n = 1;
        if (lbl_80282034 == 1) {
            switch (nKind) {
            case 2:
                n = 2;
                break;
            case 5:
            case 8:
                n = 3;
                break;
            case 11:
                nCrowd = 1;
                if (gPlayers[nPlayer].pChar != NULL) {
                    pClip = gPlayers[nPlayer].pChar->pCurClip;
                    if (pClip != NULL) {
                        // port: the clip name's first 8 characters read as one big-endian u64
                        uName = *(u64*)pClip->name;
                        if (uName == 0x67646C66756C3332ULL || uName == 0x66646C66756C3332ULL) {
                            nCrowd = 2;
                        }
                    }
                }
                Gaud_InitSlowMo(nPlayer, nCrowd);
                break;
            default:
                n = 0;
                break;
            }
        }
        if (n) {
            Aud_EmiSetTrackVarRange(nIdA, 4, n);
            Aud_EmiSetTrackVarRange(nIdB, 4, n);
            Aud_EmiSetTrackStatus(nIdA, 4, 1);
            Aud_EmiSetTrackStatus(nIdB, 4, 1);
        }
        lbl_80282034--;
        return;
    }
    if (lbl_80282032) {
        if (nKind == 11) {
            Aud_EmiSetTrackVarRange(nIdA, 5, 2);
            Aud_EmiSetTrackVarRange(nIdB, 5, 2);
        } else {
            Aud_EmiSetTrackVarRange(nIdA, 5, 0);
            Aud_EmiSetTrackVarRange(nIdB, 5, 0);
        }
        Aud_EmiSetTrackAttenuation(nIdSwing, 1, 2.0f);
        Aud_EmiSetTrackStatus(nIdA, 5, 1);
        Aud_EmiSetTrackStatus(nIdB, 5, 1);
        lbl_80282032 = 0;
    } else {
        Aud_EmiSetTrackAttenuation(nIdSwing, 1, 1.0f);
    }
    Aud_EmiSet3DPos(nIdSwing, pPlayer->ball.vPos, NULL, 0);
    Aud_EmiSetTrackVarRange(nIdSwing, 1, lbl_80183AD8[pPlayer->nClub]);
    if ((nMode == 22 || nMode == 26) && fn_8005C280(nPlayer) > 1.0f) {
        Aud_EmiSetTrackVariation(nIdSwing, 1, 5);
    } else {
        Aud_EmiSetTrackVariation(nIdSwing, 1, lbl_80183AF4[pPlayer->ball.nLie]);
    }
    Aud_EmiSetTrackStatus(nIdSwing, 1, 1);
    if (bRestore && lbl_80281428 != -1) {
        Gaud_InitCrowdReactionSound(lbl_80281428, 1);
        lbl_80281428 = -1;
        lbl_8028202F = 0;
    }
    lbl_80282033 = 1;
    pView->tLast = TI_sReadCounter(1);
}

// The ball's impact sound, by the surface it hit (its nSoundId) and scaled by its speed; on
// course 7's hole 2 a surface with a swing sound plays that instead.
void fn_800A5CA4(u8 nPlayer) {
    Player* pPlayer;
    GameAudioView* pView;
    SurfaceType* pSurface;
    u64 tNow;
    f32 fElapsed;
    f32 fSpeed;
    f32 fVolume;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    pSurface = pPlayer->ball.pHitSurface;
    pView = &lbl_801F1790[pPlayer->nView[0]];
    tNow = TI_sReadCounter(1);
    fElapsed = fn_8006E118(tNow, pView->tLast);
    pView->tLast = tNow;
    if ((pPlayer->ball.nCollideCount == 0
         || (pPlayer->ball.nCollideCount > 0 && fElapsed >= 0.2f))
        && pSurface != NULL) {
        fSpeed = pPlayer->ball.fSpeed;
        if (fSpeed < 0.0f) {
            fSpeed = -fSpeed;
        }
        fVolume = lbl_8028144C * fSpeed;
        fVolume = fVolume * fVolume;
        if (fVolume > 0.1f) {
            if (Game_GetCourse() == 7 && Game_GetCurHoleNum() == 2 && pSurface->nSwingSoundId != 0) {
                fVolume *= 2.0f;
                if (fVolume > 2.0f) {
                    fVolume = 2.0f;
                }
                Aud_EmiAliasSet3DPos(4, pPlayer->ball.vPos, NULL, 0);
                Aud_EmiAliasSetTrackStep(4, 0, pSurface->nSwingSoundId - 1, 0);
                Aud_EmiAliasSetTrackAttenuation(4, 0, fVolume);
            } else {
                nId = lbl_801F1790[pPlayer->nView[0]].n1;
                if (pSurface->nSoundId == 4) {
                    fVolume = 1.0f;
                }
                if (fVolume > 1.0f) {
                    fVolume = 1.0f;
                }
                Aud_EmiSet3DPos(nId, pPlayer->ball.vPos, NULL, 0);
                Aud_EmiSetTrackStep(nId, 0, pSurface->nSoundId, 0);
                Aud_EmiSetTrackAttenuation(nId, 0, fVolume);
            }
            if (Game_GetMode() == 26 || Game_GetMode() == 22) {
                fn_8010D3D8(nPlayer);
            }
        }
    }
    fn_800A707C();
}

void fn_800A5E94(u8 nPlayer) {
    fn_8006BAA8(nPlayer);
    ExitCrowdBuildup();
    fn_800A707C();
}

void fn_800A5EC0(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;
    u8 nRand;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    Aud_EmiSet3DPos(nId, pPlayer->ball.vPos, NULL, 0);
    Aud_EmiSetTrackAttenuation(nId, 0, 2.0f);
    nRand = Misc_RandFunc(2) & 1;   // one of two sounds at random
    Aud_EmiSetTrackStep(nId, 0, nRand == 0 ? 0x1A : 0x1C, 0);
}

void fn_800A5F60(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    Aud_EmiSet3DPos(nId, pPlayer->ball.vPos, NULL, 0);
    Aud_EmiSetTrackAttenuation(nId, 0, 1.0f);
    Aud_EmiSetTrackStep(nId, 0, 0x17, 0);
}

void fn_800A5FE8(u8 nPlayer) {
    Player* pPlayer;
    u8 nId;

    pPlayer = &gPlayers[nPlayer];
    nId = lbl_801F1790[pPlayer->nView[0]].n1;
    Aud_EmiSet3DPos(nId, pPlayer->ball.vPos, NULL, 0);
    Aud_EmiSetTrackAttenuation(nId, 0, 1.0f);
    Aud_EmiSetTrackStep(nId, 0, 0x17, 0);
}

// Plays variant 3 on track 1 of the view's emitters 2 and 3, at most every 300 frames with bLimit.
void Gaud_CameraShake(u8 nPlayer, u8 bLimit) {
    Player* pPlayer;
    u8 nIdA;
    u8 nIdB;

    pPlayer = &gPlayers[nPlayer];
    nIdA = lbl_801F1790[pPlayer->nView[0]].n2;
    nIdB = lbl_801F1790[pPlayer->nView[0]].n3;
    if (!bLimit || lbl_80282048 == 0 || lbl_80282048 + 300 < gSession.nFrameCount) {
        lbl_80282048 = gSession.nFrameCount;
        Aud_EmiSetTrackVarRange(nIdA, 1, 3);
        Aud_EmiSetTrackVarRange(nIdB, 1, 3);
        Aud_EmiSetTrackStatus(nIdA, 1, 1);
        Aud_EmiSetTrackStatus(nIdB, 1, 1);
    }
}

void fn_800A6148(void) {
    u8 nIdA;
    u8 nIdB;

    nIdA = lbl_801F1790[0].n2;
    nIdB = lbl_801F1790[0].n3;
    Aud_EmiSetTrackVarRange(nIdA, 1, 3);
    Aud_EmiSetTrackVarRange(nIdB, 1, 3);
    Aud_EmiSetTrackStatus(nIdA, 1, 1);
    Aud_EmiSetTrackStatus(nIdB, 1, 1);
}

// Plays text ditty n (variation range n of track 7 on view 0's two emitters); a UI script command
// picks n.
void Gaud_PlayTextDitty(int n) {
    u8 nIdA;
    u8 nIdB;

    nIdA = lbl_801F1790[0].n2;
    nIdB = lbl_801F1790[0].n3;
    Aud_EmiSetTrackVarRange(nIdA, 7, n);
    Aud_EmiSetTrackVarRange(nIdB, 7, n);
    Aud_EmiSetTrackStatus(nIdA, 7, 1);
    Aud_EmiSetTrackStatus(nIdB, 7, 1);
}

// Starts the shot clock's ticking (track 1 of emitter 3); the timed target game starts it for the
// last 10 seconds.
void Gaud_StartShotClock(void) {
    Aud_EmiAliasSetTrackStatus(3, 1, 1);
}

void Gaud_StopShotClock(void) {
    Aud_EmiAliasSetTrackStatus(3, 1, 0);
}

void fn_800A62A4(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 0);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A62E0(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 1);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A631C(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 2);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A6358(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 3);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A6394(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 4);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A63D0(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 6);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

void fn_800A640C(void) {
    Aud_EmiAliasSetTrackVarRange(3, 0, 7);
    Aud_EmiAliasSetTrackStatus(3, 0, 1);
}

// Empty; nPlayer is unused (event.c passes it: TW07's Gaud_Tappa, empty too, takes the player).
void fn_800A6448(u8 nPlayer) {
}

// Empty; nPlayer is unused (event.c passes it: TW07's Gaud_Spina, empty too, takes the player).
void fn_800A644C(u8 nPlayer) {
}

void fn_800A6450(u8 nPlayer) {
    u8 nId;

    nId = lbl_801F1790[gPlayers[nPlayer].nView[0]].n0;
    if (nId != 0xFF) {
        Aud_EmiSetTrackStatus(nId, 2, 1);
    }
}

// The GameBreaker starts: the heartbeat (track 2 of the view's emitters 2 and 3) with its
// controller vibration, the high half of the channels muted, and the pin's, crowd and ambience
// emitters silenced. b also starts the slow-motion sound. Nothing while it is already on
// (lbl_8028202F) or lbl_8028202B is set.
void Gaud_InitGameBreaker(u8 nPlayer, u8 b) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (lbl_8028202B || lbl_8028202F) return;
    HeartBeatLoopCallback(pView->n2, 2, 1);
    Aud_EmiSetTrackStatus(pView->n2, 2, 1);
    Aud_EmiSetTrackStatus(pView->n3, 2, 1);
    Aud_Mute(0, 1);
    Aud_EmiSetTrackAttenuation(lbl_80281420, 0, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 0, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 1, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 2, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 3, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 4, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141C, 5, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 0, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 1, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 2, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 3, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 4, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141D, 5, 0.0f);
    Aud_EmiSetTrackAttenuation(lbl_8028141A, 0, 0.0f);
    if (b) {
        Gaud_InitSlowMo(nPlayer, 0);
    }
    lbl_8028202F = 1;
}

// The GameBreaker ends: the heartbeat stops, the channels are unmuted, the pin's and ambience
// emitters go back to full and the crowd to lbl_80281430, and a crowd reaction held back meanwhile
// (lbl_80281428) plays. Nothing while lbl_8028202B is set.
void Gaud_ExitGameBreaker(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (lbl_8028202B == 0) {
        Aud_EmiSetTrackStatus(pView->n2, 2, 0);
        Aud_EmiSetTrackStatus(pView->n3, 2, 0);
        Aud_Mute(0, 0);
        Aud_EmiSetTrackAttenuation(lbl_80281420, 0, 1.0f);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 0, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 1, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 2, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 3, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 4, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141C, 5, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 0, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 1, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 2, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 3, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 4, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141D, 5, lbl_80281430);
        Aud_EmiSetTrackAttenuation(lbl_8028141A, 0, 1.0f);
        lbl_8028202F = 0;
        if (lbl_80281428 != -1) {
            Gaud_InitCrowdReactionSound(lbl_80281428, 1);
            lbl_80281428 = -1;
        }
    }
}

// The zoom camera's sound: track 0 of the player's view's emitters 2 and 3, until Gaud_ExitCamZoom.
void Gaud_InitCamZoom(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    Aud_EmiSetTrackStatus(pView->n2, 0, 1);
    Aud_EmiSetTrackStatus(pView->n3, 0, 1);
}

void Gaud_ExitCamZoom(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    Aud_EmiSetTrackStatus(pView->n2, 0, 0);
    Aud_EmiSetTrackStatus(pView->n3, 0, 0);
}

// A special swing camera starts (camera 0's kind, View.n260). Outside speed golf (modes 6 to 8) it
// mutes the high channels and, by kind, plays a swoosh (a variation range of track 1 on view 0's
// emitters 2 and 3), the slow-motion sound or kind 7's sounds; lbl_80282034 tells fn_800A5980 what
// to play on the next swing sounds.
void Gaud_InitSpecialShot(u8 nPlayer) {
    GameAudioView* pView;
    int nKind;
    u8 bPlay;
    u8 n;

    pView = &lbl_801F1790[0];
    nKind = fn_800C7138(ViewController_GetCameraController(0));
    bPlay = 1;
    n = 0;
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        Aud_Mute(0, 1);
        switch (nKind) {
        case 1:
        case 6:
            lbl_80282034 = 1;
            break;
        case 13:
        case 14:
            n = 2;
            lbl_80282034 = 1;
            break;
        case 15:
        case 16:
            Gaud_InitSlowMo(nPlayer, 2);
            lbl_80282034 = 1;
            break;
        case 4:
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 7:
            Aud_EmiSetTrackStatus(pView->n2, 2, 1);
            Aud_EmiSetTrackStatus(pView->n3, 2, 1);
            Aud_EmiSetTrackVarRange(pView->n2, 6, 5);
            Aud_EmiSetTrackVarRange(pView->n3, 6, 5);
            Aud_EmiSetTrackVariation(pView->n2, 6, 0);
            Aud_EmiSetTrackVariation(pView->n3, 6, 1);
            Aud_EmiSetTrackStatus(pView->n2, 6, 1);
            Aud_EmiSetTrackStatus(pView->n3, 6, 1);
            Gaud_InitSlowMo(nPlayer, 2);
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 9:
            bPlay = 0;
            lbl_80282034 = 1;
            break;
        case 10:
            n = 1;
            lbl_80282034 = 1;
            break;
        case 2:
        case 8:
        case 11:
            bPlay = 0;
            lbl_80282034 = 3;
            break;
        case 5:
            bPlay = 0;
            lbl_80282034 = 2;
            break;
        default:
            bPlay = 0;
            lbl_80282034 = 0;
            break;
        }
        if (bPlay) {
            Aud_EmiSetTrackVarRange(pView->n2, 1, n);
            Aud_EmiSetTrackVarRange(pView->n3, 1, n);
            Aud_EmiSetTrackStatus(pView->n2, 1, 1);
            Aud_EmiSetTrackStatus(pView->n3, 1, 1);
        }
        lbl_80282032 = 1;
    }
}

// For the comic-book camera (kinds 4 and 9), the sound of panel n: variation range n (n + 4 for
// kind 4) of track 6 on the view's emitters 2 and 3.
void Gaud_UpdtSpecialShot(u8 nPlayer, u8 n) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    switch (fn_800C7138(ViewController_GetCameraController(0))) {
    case 4:
        n += 4;
        // fall through
    case 9:
        Aud_EmiSetTrackVarRange(pView->n2, 6, n);
        Aud_EmiSetTrackVarRange(pView->n3, 6, n);
        Aud_EmiSetTrackVariation(pView->n2, 6, 0);
        Aud_EmiSetTrackVariation(pView->n3, 6, 1);
        Aud_EmiSetTrackStatus(pView->n2, 6, 1);
        Aud_EmiSetTrackStatus(pView->n3, 6, 1);
        break;
    }
}

// The special swing camera ends (outside speed golf): unmutes, stops track 1 (and kind 7's track 2)
// of the view's emitters 2 and 3, and plays the swing sound if it is still due (lbl_80282032).
void Gaud_ExitSpecialShot(u8 nPlayer) {
    GameAudioView* pView;
    int nKind;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    nKind = fn_800C7138(ViewController_GetCameraController(0));
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        Aud_Mute(0, 0);
        Aud_EmiSetTrackStatus(pView->n2, 1, 0);
        Aud_EmiSetTrackStatus(pView->n3, 1, 0);
        if (nKind == 7) {
            Aud_EmiSetTrackStatus(pView->n2, 2, 0);
            Aud_EmiSetTrackStatus(pView->n3, 2, 0);
        }
        if (lbl_80282032) {
            fn_800A5980(nPlayer);
        }
    }
}

// Slow motion's sound: variation range n of track 3 on the player's view's emitters 2 and 3 (not in
// speed golf, modes 6 to 8).
void Gaud_InitSlowMo(u8 nPlayer, u8 n) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        Aud_EmiSetTrackVarRange(pView->n2, 3, n);
        Aud_EmiSetTrackVarRange(pView->n3, 3, n);
        Aud_EmiSetTrackStatus(pView->n2, 3, 1);
        Aud_EmiSetTrackStatus(pView->n3, 3, 1);
    }
}

void fn_800A6D48(u8 nPlayer) {
    GameAudioView* pView;

    pView = &lbl_801F1790[gPlayers[nPlayer].nView[0]];
    if (Game_GetMode() < 6 || Game_GetMode() > 8) {
        Aud_EmiSetTrackStatus(pView->n2, 3, 0);
        Aud_EmiSetTrackStatus(pView->n3, 3, 0);
    }
}

// Plays crowd reaction nMusic (a variation range of tracks 0 and 1 on both crowd emitters), after
// stopping the crowd build-up. During the GameBreaker or a special shot it is kept in lbl_80281428
// and played when that ends; a second argument of 1 also clears lbl_80282033, which the crowd
// build-up needs.
void Gaud_InitCrowdReactionSound(int nMusic, int a) {
    u8 n = nMusic;

    ExitCrowdBuildup();
    fn_800A707C();
    if (a == 1) {
        lbl_80282033 = 0;
    }
    if (lbl_8028202F || lbl_80282032) {
        lbl_80281428 = nMusic;
        return;
    }
    Aud_EmiSetTrackVarRange(lbl_8028141C, 0, n);
    Aud_EmiSetTrackVarRange(lbl_8028141C, 1, n);
    Aud_EmiSetTrackVarRange(lbl_8028141D, 0, n);
    Aud_EmiSetTrackVarRange(lbl_8028141D, 1, n);
    Aud_EmiSetTrackStatus(lbl_8028141C, 0, 1);
    Aud_EmiSetTrackStatus(lbl_8028141C, 1, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 0, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 1, 1);
    lbl_80281424 = nMusic;
}

void fn_800A6EC8(void) {
    if (lbl_80282040) {
        ExitCrowdBuildup();
        fn_800A707C();
        Aud_EmiSetTrackStatus(lbl_8028141C, 0, 0);
        Aud_EmiSetTrackStatus(lbl_8028141C, 1, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 0, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 1, 0);
    }
}

void fn_800A6F38(void) {
    if (lbl_80282040) {
        Aud_EmiSetTrackVarRange(lbl_8028141C, 0, 2);
        Aud_EmiSetTrackVarRange(lbl_8028141C, 1, 2);
        Aud_EmiSetTrackVarRange(lbl_8028141D, 0, 2);
        Aud_EmiSetTrackVarRange(lbl_8028141D, 1, 2);
        Aud_EmiSetTrackStatus(lbl_8028141C, 0, 1);
        Aud_EmiSetTrackStatus(lbl_8028141C, 1, 1);
        Aud_EmiSetTrackStatus(lbl_8028141D, 0, 1);
        Aud_EmiSetTrackStatus(lbl_8028141D, 1, 1);
    }
}

void fn_800A6FE0(void) {
    if (lbl_8028202F || !lbl_80282040) return;
    if (gPlayers[lbl_80282278].ball.b99 == 0) {
        fn_800A6EC8();
    }
    Aud_EmiSetTrackStatus(lbl_8028141C, 4, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 4, 1);
    Aud_EmiSetTrackStatus(lbl_8028141C, 5, 1);
    Aud_EmiSetTrackStatus(lbl_8028141D, 5, 1);
}

void fn_800A707C(void) {
    if (lbl_80282040) {
        Aud_EmiSetTrackStatus(lbl_8028141C, 4, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 4, 0);
        Aud_EmiSetTrackStatus(lbl_8028141C, 5, 0);
        Aud_EmiSetTrackStatus(lbl_8028141D, 5, 0);
    }
}

void fn_800A70E4(int n) {
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackStatus(0, 0, 1);
        Aud_EmiAliasSetTrackVarRange(0, 1, n);
        Aud_EmiAliasSetTrackStatus(0, 1, 1);
    }
}

void fn_800A714C(void) {
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackStatus(0, 0, 0);
        Aud_EmiAliasSetTrackStatus(0, 1, 0);
    }
}

void fn_800A7198(int n) {
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackVarRange(1, 0, n);
        Aud_EmiAliasSetTrackStatus(1, 1, 1);
    }
}

void fn_800A71E4(void) {
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackStatus(1, 1, 0);
    }
}

void fn_800A7220(f32 fAmount) {
    u32 n;

    n = (u8)(int)(3.0f * fAmount);
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackVarRange(0, 2, (n <= 2) ? n : 2);
        Aud_EmiAliasSetTrackStatus(0, 2, 1);
    }
}

void fn_800A7294(void) {
    if (gpGame->b288) {
        Aud_EmiAliasSetTrackStatus(0, 2, 0);
        if (lbl_8028203C == 2) {
            Aud_EmiSetTrackStatus(lbl_8028141A, 2, 0);
        }
    }
}

// Switches the game's sounds off (bOff); switching back on restarts only the music, if bMusic.
void fn_800A72EC(u8 bOff, u8 bMusic) {
    if (lbl_8028202E ^ bOff) {
        lbl_8028202E = bOff;
        if (bOff) {
            StopAmbientStreamer(0);
            Gaud_StopMusic();
            fn_800A6EC8();
            Gaud_ExitGameBreaker(0);
            return;
        }
        if (bMusic) {
            Gaud_SetStreamingContext();
            StartBackgroundMusic();
        }
    }
}

void fn_800A7350(u8 bOn) {
    if ((lbl_8028202B ^ bOn) != 0) {
        lbl_8028202B = bOn;
        if (lbl_8028202E == 0) {
            Aud_Pause(bOn, 0);
            if (bOn) {
                fn_800A73C0(0, 1);
            } else {
                lbl_8028202C = 1;
            }
        }
    }
}

// a is unused.
void fn_800A73C0(u8 a, int n) {
    Aud_EmiSetTrackStep(lbl_8028141B, 0, n, 0);
}

void fn_800A73F0(s32 n) {
    switch (n) {
    case 1:
        Aud_EmiSetTrackStep(lbl_8028141B, 1, 0, 0);
        break;
    case 6:
        Aud_EmiSetTrackStep(lbl_8028141B, 1, 1, 0);
        break;
    default:
        Aud_EmiSetTrackStep(lbl_8028141B, 0, n, 0);
        break;
    }
}

// The long-drive contests' sounds: script 0 plays variation range n on track nTrack of emitter 5,
// script 1 steps track 0 of emitter 6 to n. Callers also pass a loop flag and a pan, which it
// ignores.
void Gaud_LongDriveUi_Play(s32 nKind, int nTrack, int n) {
    switch (nKind) {
    case 0:
        Aud_EmiAliasSetTrackVarRange(5, nTrack, n);
        Aud_EmiAliasSetTrackStatus(5, nTrack, 1);
        break;
    case 1:
        Aud_EmiAliasSetTrackStep(6, 0, n, 0);
        break;
    }
}

// Stops what Gaud_LongDriveUi_Play started with script 0; script 1 has nothing to stop. Callers
// also pass the range, which it ignores.
void Gaud_LongDriveUi_Stop(s32 nKind, int nTrack) {
    switch (nKind) {
    case 0:
        Aud_EmiAliasSetTrackStatus(5, nTrack, 0);
        break;
    case 1:
        break;
    }
}

s32 fn_800A7528(void) {
    return Gaud_GetCommentStatus();
}

// Plays track b of music play list a on the music emitter (track 0, streamed) and shows the song's
// names; only while music is what streams (lbl_8028203C 1).
void Gaud_StartMusic(u8 a, u16 b) {
    if (lbl_8028203C == 1) {
        Aud_EmiSetTrackStream(lbl_80281418, 0, a, b, 2);
        Aud_EmiSetTrackStatus(lbl_80281418, 0, 1);
        fn_800BA734(1, b);
    }
}

// Stops the background music; nothing when the ambience is what is streaming.
void Gaud_StopMusic(void) {
    if (lbl_8028203C == 1) {
        Aud_EmiSetTrackStatus(lbl_80281418, 0, 0);
        lbl_8028203C = 0;
    }
}

u8 fn_800A75F4(void) {
    int bResult;

    bResult = 0;
    if (lbl_8028203C == 1 && Aud_EmiGetTrackStatus(lbl_80281418, 0)) {
        bResult = 1;
    }
    return bResult;
}

void fn_800A7644(void) {
    Gaud_SetStreamingContext();
}

// Plays commentary line nMsg of playlist nKind on the commentary emitter (lbl_80281419), unless the
// commentary volume option is 0. Within 15 frames of Gaud_StopComment the line is queued, and
// Gaud_Cycle plays it when the wait runs out.
// port: the callers pass nKind and nMsg as full ints (their prototype takes int), but this body was
// compiled for a u8 nKind and a u16 nMsg: it stores and passes them on without masking.
void Gaud_StartComment(int nKind, int nMsg, int a) {
    if ((s8)gSession.options.a0[4] != 0) {
        if (lbl_80282054 != 0) {
            lbl_80282052 = nKind;
            lbl_80282038 = 1;
            lbl_80282050 = nMsg;
            lbl_8028204C = a;
            return;
        }
        // port: EA passes nKind and nMsg as ints, unmasked, to Aud_EmiSetTrackStream's u8 and u16
        // parameters
        ((void (*)(u8, u8, int, int, s32))Aud_EmiSetTrackStream)(lbl_80281419, 0, nKind, nMsg, a);
        Aud_EmiSetTrackStatus(lbl_80281419, 0, 1);
    }
}

// Stops the commentary and drops a queued line; for the next 15 frames a new line waits
// (Gaud_StartComment queues it).
void Gaud_StopComment(void) {
    Aud_EmiSetTrackStatus(lbl_80281419, 0, 0);
    lbl_80282054 = 15;
    lbl_80282038 = 0;
}

u8 Gaud_GetCommentStatus(void) {
    return Aud_EmiGetTrackStatus(lbl_80281419, 0);
}

u8 fn_800A7748(void) {
    return Aud_EmiGetTrackStatus(lbl_8028141A, 0);
}

u8 fn_800A7770(void) {
    int bResult;

    if (gSession.nGameType == 3) {
        return fn_800A75F4();
    }
    bResult = 0;
    if (Gaud_GetCommentStatus() || fn_800A7748() || fn_800A75F4()) {
        bResult = 1;
    }
    return bResult;
}

// Scales the volume curves 1-6, 13 and 16-31 and emitter lbl_8028141B's tracks 0 and 1 (the
// options menu passes 0.2 x options.a0[0]).
void fn_800A77E0(f32 fVolume) {
    u8 i;

    for (i = 1; i < 5; i++) {
        Aud_SetSubmixAttn(i, fVolume * lbl_8018E988[i]);
    }
    Aud_SetSubmixAttn(13, lbl_8018E988[13] * fVolume);
    Aud_SetSubmixAttn(5, lbl_8018E988[5] * fVolume);
    Aud_SetSubmixAttn(6, lbl_8018E988[6] * fVolume);
    Aud_EmiSetTrackAttenuation(lbl_8028141B, 0, fVolume);
    Aud_EmiSetTrackAttenuation(lbl_8028141B, 1, fVolume);
    for (i = 16; i < 32; i++) {
        Aud_SetSubmixAttn(i, fVolume * lbl_8018E988[i]);
    }
}

void fn_800A78F0(f32 fVolume) {
    fVolume *= lbl_8018E988[14];
    Aud_SetSubmixAttn(14, fVolume);
}

void fn_800A7924(f32 f) {
    Gaud_SetStreamingContext();
}

void fn_800A7944(void) {
    Gaud_SetStreamingContext();
    StartBackgroundMusic();
}

// Picks what a streamed track of instance nId plays: play list a, stream b, play mode c, packed
// into the track's controller value (mode in the top byte, list in the next, stream in the low 16
// bits) for AudTable.c to hand to the streamer.
void Aud_EmiSetTrackStream(u8 nId, u8 nTrack, u8 a, u16 b, s32 c) {
    Aud_EmiSetControllerInt(nId, nTrack, (c << 24) | (a << 16) | b);
}

void Aud_InitMovie(void) {
    Mov_Init();
}

void Aud_ExitMovie(void) {
    Mov_Exit();
}

void Aud_StartMovie(void) {
    Mov_Start();
}

void Aud_CycleMovie(void) {
    Mov_Tick();
}

void fn_800A7A14(u8 nSound) {
    fn_800B0858(nSound);
}

// Every caller passes a fourth argument; nothing here reads it.
s32 Aud_InitSession(u8 a, u8 b, u8 nListeners, int nUnused) {
    Aud_EmiInitSession();
    Aud_MicInitSession(nListeners);
    // port: EA passes an argument Ses_Init ignores
    ((u8 (*)(u8, u8, u8, int))Ses_Init)(a, b, nListeners, 0);
    return 1;
}

void Aud_ExitSession(s32 n) {
    Aud_EmiExitSession();
    Aud_MicExitSession();
    // port: EA passes an argument fn_800A8D88 ignores
    ((void (*)(s32))fn_800A8D88)(n);
}

// Turns reverb on (bOn 1) or off for track nTrack of sound nSound's template, so for every instance
// of that sound.
void Aud_SesTmplOvrTrackRvbMode(s16 nSound, u8 nTrack, u8 bOn) {
    fn_800A94F4(nSound, nTrack, bOn);
}
