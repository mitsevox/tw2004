// gameaudio.h (our name): GameAudio.c's data, the game's side of the sound engine. Only the fields
// the matched code proves are named; the rest keep their offsets.

#ifndef CORE_GAMEAUDIO_H
#define CORE_GAMEAUDIO_H

#include "game.h"

// A view's sound (lbl_801F1790, one per view; a player's nView[0] picks it).
typedef struct GameAudioView {
    u8   n0;                    // 0x0    emitter id (0xFF: none), sound 1: the swing (track 0 the
                                //        club's swish, track 1 the club hitting the ball)
    u8   n1;                    // 0x1    emitter id, sound 2: the ball (bounces, the cup, the pole)
    u8   n2;                    // 0x2    } emitter ids, sound 4 left and right of the listener: the
    u8   n3;                    // 0x3    } effects (zoom, heartbeat, slow motion, special shots,
                                //        camera shake, text ditties)
    u8   n4;                    // 0x4
    u8   n5;                    // 0x5
    u8   unk6[0x8 - 0x6];
    f32  f8;                    // 0x8    only ever cleared
    f32  fC;                    // 0xC    the last frame's time, for the swish's club speed
    u64  tLast;                 // 0x10   TI_sReadCounter(1)'s reading at the ball's last hit or bounce
    s32  n18;                   // 0x18   the swing state Gaud_UpdtSwing last saw
    u8   unk1C[0x20 - 0x1C];
} GameAudioView;
LAYOUT_ASSERT(GameAudioView, 0x20);

// An entry of lbl_8018EA08: a hole whose ambience plays an extra track (GetAmbientStreamRange).
typedef struct GameAudioCourseSound {
    s32  nCourse;               // 0x0    Game_GetCourse()'s course
    u8   n4;                    // 0x4    the hole's number (hole index + 1)
    u8   nSound;                // 0x5    the variation range of the ambience's track 4
    u8   unk6[0x8 - 0x6];
} GameAudioCourseSound;
LAYOUT_ASSERT(GameAudioCourseSound, 0x8);

// A world object's sound as Gaud_ActorDownloadCallback is handed it (through a pointer to a pointer to it).
typedef struct GameAudioSource {
    u8   unk0[0x10];
    f32  vPos[3];               // 0x10   where it plays (kinds other than 0, 3 and 5)
    u8   unk1C[0x22 - 0x1C];
    s16  nSound;                // 0x22   the sound, 0 for none
    u32  nKind;                 // 0x24   passed on to Aud_EmiAdd; 0, 3 and 5 play as a stereo pair
} GameAudioSource;

// Called from other files: music by the menus (FE_MessageTable.c fn_80084BE8); startUp.c's sound.
void Gaud_SetStreamingContext(void);                 // picks music or ambience and records it in lbl_8028203C
void Gaud_StartMusic(u8 a, u16 b);
u8   Gaud_GetMusicStatus(void);
void Gaud_RestartMusic(void);
void Aud_PlayBuiltInSound(u8 nSound);

#endif
