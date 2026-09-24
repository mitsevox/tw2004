// mode26.h (our name): the state of game mode 26 (GameMode26.c). The same values as game mode 22's
// state struct (mode22.h), kept here in separate .sdata/.sbss variables.

#ifndef GAME_MODES_MODE26_H
#define GAME_MODES_MODE26_H

#include "game_types.h"
#include "platform.h"

extern s32 lbl_80281760;        // the score that wins (fn_8010D334 sets it)
extern s32 lbl_80281764;        // the player who reached it (5 = nobody yet)
extern s32 lbl_80281768;        // the player with the longest scoring shot (5 = nobody yet)
extern s32 lbl_8028176C;        // counts down every frame once there is a winner, from 120
extern s32 lbl_80281770;        // frames to the next shot-length check (fn_8010C764), from 15
extern s32 lbl_80281774[2];     // per player: the shot length at the last check
extern s32 lbl_8028177C[2];     // per player: the track fn_8010C764 starts is playing
extern u8  lbl_80282490;        // set once fn_8010C764 has shown the mode's first message
extern f32 lbl_80282494;        // the longest scoring shot's length
extern u8  lbl_80282498[5];     // } per player: the track for reaching 400, 800 and 1200 points
extern u8  lbl_802824A0[5];     // } has played (400, 800, 1200 in this order)
extern u8  lbl_802824A8[5];     // }

#endif
