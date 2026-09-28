// mode26.h (our name): the state of game mode 26 (GameMode26.c). The same values as game mode 22's
// state struct (mode22.h), kept here in separate .sdata/.sbss variables.

#ifndef GAME_MODES_MODE26_H
#define GAME_MODES_MODE26_H

#include "game_types.h"
#include "platform.h"

extern s32 gGameMode26TargetScore;        // the score that wins (GameMode26_SetTargetScore sets it)
extern s32 gGameMode26Winner;        // the player who reached it (5 = nobody yet)
extern s32 gGameMode26LongestPlayer;        // the player with the longest scoring shot (5 = nobody yet)
extern s32 gGameMode26WinnerCountdown;        // counts down every frame once there is a winner, from 120
extern s32 gGameMode26LengthCheckFrames;        // frames to the next shot-length check (GameMode26_UpdateFrame), from 15
extern s32 gGameMode26LastLength[2];     // per player: the shot length at the last check
extern s32 gGameMode26LengthSoundOn[2];     // per player: the track GameMode26_UpdateFrame starts is playing
extern u8  gGameMode26IntroSaid;        // set once GameMode26_UpdateFrame has shown the mode's first message
extern f32 gGameMode26LongestLength;        // the longest scoring shot's length
extern u8  gGameMode26Reached400[5];     // } per player: the track for reaching 400, 800 and 1200 points
extern u8  gGameMode26Reached800[5];     // } has played (400, 800, 1200 in this order)
extern u8  gGameMode26Reached1200[5];     // }

// GameMode26.c: what the post-shot UI asks (GameUI.c GUI_IsPostShotUIAnimating)
u8   GameMode26_IsActive(void);          // game mode 26 is being played
u8   GameMode26_IsShowingWinner(void);   // somebody has won and the 120 frames still run

#endif
