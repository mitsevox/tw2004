// mode22.h (our name): the state of game mode 22 (GameMode22.c).

#ifndef GAME_MODES_MODE22_H
#define GAME_MODES_MODE22_H

#include "game_types.h"
#include "platform.h"

// Game mode 22's state (gGameMode22, 0x1C bytes in .data).
typedef struct GameMode22State {
    s32 nVariant;               // 0x0  0 the drives' points add up, 1 only the best drive counts
                                //      (GameMode22_SetVariant, from the menu)
    s32 nDrives;                // 0x4  drives each player gets (GameMode22_SetNumDrives)
    s32 nWinner;                // 0x8  the winning player, 5 none yet
    u8  bDecided;               // 0xC  a winner is decided (GameMode22_GetWinner)
    u8  unkD[3];
    f32 fLongestDrive;          // 0x10  the longest scoring drive so far (0 none)
    s32 nLongestDriver;         // 0x14  whose it is (5 none)
    s32 nWinnerFrames;          // 0x18  the winner countdown: 120 frames, run down once a winner
                                //       is decided (GameMode22_IsShowingWinner)
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

// GameMode22.c: what the post-shot UI asks (GameUI.c GUI_IsPostShotUIAnimating)
u8   GameMode22_IsActive(void);          // game mode 22 is being played
u8   GameMode22_IsShowingWinner(void);   // a winner is decided and his 120 frames still run

#endif
