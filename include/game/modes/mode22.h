// mode22.h (our name): the state of game mode 22 (GameMode22.c).

#ifndef GAME_MODES_MODE22_H
#define GAME_MODES_MODE22_H

#include "game_types.h"
#include "platform.h"

// Game mode 22's state (gGameMode22, 0x1C bytes in .data).
typedef struct GameMode22State {
    s32 n0;                     // 0x0  GameMode22_GetVariant returns it
    s32 n4;                     // 0x4  set by GameMode22_SetNumDrives
    s32 n8;                     // 0x8  GameMode22_RestartHole sets 5
    u8  bC;                     // 0xC  GameMode22_GetWinner returns it
    u8  unkD[3];
    f32 f10;                    // 0x10  GameMode22_StartEvent sets 0
    s32 n14;                    // 0x14  GameMode22_StartEvent sets 5
    s32 n18;                    // 0x18  GameMode22_StartEvent sets 120; GameMode22_IsShowingWinner: n8 not 5 and this above 0
} GameMode22State;
LAYOUT_ASSERT(GameMode22State, 0x1C);

extern GameMode22State gGameMode22;
extern s32 gGameMode22LengthCheckFrames;        // frames to the next shot-length check (GameMode22_UpdateFrame), from 15
extern s32 gGameMode22ShowIntro;        // set: GameMode22_UpdateFrame shows the mode's messages once
extern s32 gGameMode22LastLength;        // the shot length at the last check
extern s32 gGameMode22LengthSoundOn;        // the track GameMode22_UpdateFrame starts is playing
extern u8 gGameMode22Reached400[5];     // } per player, cleared by the mode's setup (GameMode22_Init)
extern u8 gGameMode22Reached800[5];     // }
extern u8 gGameMode22Reached1200[5];     // }
extern char* gTourTrophyTitles[4];  // the trophies' titles ("Rookie of the Year", "Player of the Year", ...)
// The text GameMode22_ShowDrivesLeft prints (starts as "D"; 0xE zero bytes follow it, so likely 16 bytes).
extern char gGameMode22DrivesLeftText[];

#endif
