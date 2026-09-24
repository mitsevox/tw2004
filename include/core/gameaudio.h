// gameaudio.h (our name): GameAudio.c's data, the game's side of the sound engine. Only the fields
// the matched code proves are named; the rest keep their offsets.

#ifndef CORE_GAMEAUDIO_H
#define CORE_GAMEAUDIO_H

#include "game.h"

// A view's sound (lbl_801F1790, one per view; a player's nView[0] picks it).
typedef struct GameAudioView {
    u8   n0;                    // 0x0    an emitter id, 0xFF when none
    u8   n1;                    // 0x1    an emitter id, 0xFF when none
    u8   n2;                    // 0x2    an emitter id, 0xFF when none
    u8   n3;                    // 0x3    an emitter id, 0xFF when none
    u8   n4;                    // 0x4
    u8   n5;                    // 0x5
    u8   unk6[0x8 - 0x6];
    f32  f8;                    // 0x8
    f32  fC;                    // 0xC
    u64  tLast;                 // 0x10   TI_sReadCounter(1)'s reading when fn_800A5CA4 last ran
    s32  n18;                   // 0x18   the swing state fn_800A573C last saw
    u8   unk1C[0x20 - 0x1C];
} GameAudioView;
LAYOUT_ASSERT(GameAudioView, 0x20);

// An entry of lbl_8018EA08: the sound fn_800A4A24 picks for a course and a number.
typedef struct GameAudioCourseSound {
    s32  nCourse;               // 0x0    Game_GetCourse()'s course
    u8   n4;                    // 0x4    fn_800A4A24's n + 1
    u8   nSound;                // 0x5    what fn_800A4A24 returns
    u8   unk6[0x8 - 0x6];
} GameAudioCourseSound;
LAYOUT_ASSERT(GameAudioCourseSound, 0x8);

// A world object's sound as fn_800A4CB8 is handed it (through a pointer to a pointer to it).
typedef struct GameAudioSource {
    u8   unk0[0x10];
    f32  vPos[3];               // 0x10   where it plays (kinds other than 0, 3 and 5)
    u8   unk1C[0x22 - 0x1C];
    s16  nSound;                // 0x22   the sound, 0 for none
    u32  nKind;                 // 0x24   passed on to fn_800AD280; 0, 3 and 5 play as a stereo pair
} GameAudioSource;

// Called from other files: music by the menus (FE_MessageTable.c fn_80084BE8); startUp.c's sound.
void fn_800A44A0(void);                 // picks music or ambience and records it in lbl_8028203C
void fn_800A754C(u8 a, u16 b);
u8   fn_800A75F4(void);
void fn_800A7944(void);
void fn_800A7A14(u8 nSound);

#endif
