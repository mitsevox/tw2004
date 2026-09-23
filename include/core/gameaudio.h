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
    s32  n10;                   // 0x10
    s32  n14;                   // 0x14
    u8   unk18[0x20 - 0x18];
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

#endif
