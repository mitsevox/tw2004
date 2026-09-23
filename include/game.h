// game.h (our name): the round and the game modes: the current hole, the golfer states, and the
// game manager and game-mode code at 0x800D0000..0x80100000 that more than one file calls.
// Players and golfers are in golfer.h, the ball in ball.h.

#ifndef GAME_H
#define GAME_H

#include "ball.h"

// ---- the round -------------------------------------------------------------------------------

int  Game_GetCourse(void);              // 0x80008830
int  Game_CurHoleIndex(void);           // 0..17 in the round (Golfer.c)
int  Game_CurrentHole(void);            // the index mapped through the hole order (Golfer.c)
void Session_SetNumPlayers(int n);      // Golfer.c

// ---- golfer states (Swing.c) -----------------------------------------------------------------

void GOLFERSTATE_Set(int nState, int nPlayer);      // pop everything and start again from one state
void GOLFERSTATE_Switch(int nState, int nPlayer);   // replace the current state
int  GOLFERSTATE_GetCurrentState(int nPlayer);      // GS_*, or -1
void fn_80062C38(void);
void fn_80062C80(int a, u8 b);
void fn_80062D38(int a, int b, int nPlayer);
void fn_80062D6C(int a, int nPlayer);

// ---- the game manager ------------------------------------------------------------------------

f32  fn_800D0478(int nPlayer);          // the ball's distance from the pin (yards)
f32  fn_800D0550(int nPlayer);          // the shot's length
int  Hole_ScoreAfterTapIn(int nPlayer); // HoleScore.c
int  fn_800D2AD8(int nHole);            // a hole's par
void fn_800D3548(int nPlayer, int nMoney, s32* pTotals);   // pTotals may be NULL
int  fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize);
s32  fn_800D6A70(s32 nPoints, int nPlayer, int a, int b, int c, int d);
int  fn_800D7220(int nReward, int a, s32* pOut);
void GameEffects_ResetGameEffectSettings(void);
void fn_800DC9D4(int a);                // pause or resume a GameBreaker
void GM_EndOfGolferTurn(int nPlayer);

// GameRound.c
void fn_800E1074(void);
void fn_800E1260(int nPreset);          // a hole-selection preset for the round
void fn_800E1434(void);
void fn_800E1480(int nHole);            // make a hole of the round the current one
u8   fn_800E1BBC(void);                 // whether the round plays every hole
u8   fn_800E2B40(int nPlayer, Ball* pBall);   // out of bounds
void fn_800E2BA4(void);                 // a random hole from the selection
u8   fn_800E39F0(void);

// GameUI.c
void fn_800E3BEC(void);
void fn_800E3C0C(u8 b);                 // show or hide the HUD on the single screen
void fn_800E3D90(void);                 // hide every HUD
void fn_800E4204(void);
u8   fn_800E4254(int nPlayer);          // whether a message or screen still holds the player
void fn_800E4364(u32 nQueue, int a, int b, int c);    // add an item to a display queue
void fn_800E45C0(void);
u8   fn_800E4BF8(void);
void fn_800E4C20(u8 bHuman);            // the end-of-round screen
void fn_800E4D94(u8 bHuman);            // the end-of-hole screen

// GameMessages.c
u8   fn_800E5110(void);
u8   fn_800E53B8(void);
void fn_800E53F0(int nMsg, int a, int b, int c);      // send a message with three ints
void fn_800E5714(int a);
void fn_800E5724(int a);
void fn_800E58B4(int nMsg);             // send a message with no values

// ---- the game modes --------------------------------------------------------------------------

// GameModeMatch.c: match play, which the other two-player modes build on
void fn_800E9F14(void);
s32  fn_800EA084(int nPlayer);
u8   fn_800EA278(int nPlayer, u8 bCheck);
u8   fn_800EA548(u8 bCheck);            // the game is over
u8   fn_800EA758(u8 bCheck);
int  fn_800EAC94(int n);

void fn_800EADD8(void);                 // GameMode5.c
u8   fn_800EC550(void);                 // GameMode5.c
void fn_800EDE78(void);                 // GameModeDriverPGATour.c
u8   fn_800EE470(void);                 // GameModeDriverPGATour.c
u8   fn_800F0818(void);                 // GameModeDriverRTE.c

// GameTargets.c: what the target games (modes 13..17) share
void fn_800F19D4(void);                 // sort the targets by distance from the tee
void fn_800F1ABC(int nPlayer, s8 n);
void fn_800F1B60(int nPlayer, s8 n);
u8   fn_800F1BD8(int nPlayer);          // previous target
u8   fn_800F1C34(int nPlayer);          // next target
s8   fn_800F1C74(int nPlayer);          // the target nearest the ball
s32  fn_800F1E58(s32 n);
void fn_800F1EE4(void);                 // every player's target-game state cleared
void fn_800F2030(void);
s32  fn_800F20C0(int nPlayer);          // how many targets the player has hit
s32  fn_800F2578(void);                 // the target game's prize
void fn_800F263C(s32 nMsg);
void fn_800F2664(int nPlayer);
void fn_800F2668(int nPlayer);
u8   fn_800F2788(int nPlayer, f32 f);   // whether f is far enough for the player's tees
void fn_800F2958(s32 nMsg, s32 a);
void fn_800F3828(void);                 // GameMode14.c
void fn_800F3980(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e);    // GameMode14.c
void fn_800F48C4(void);                 // GameMode15.c
void fn_800F7DE8(void);                 // GameMode13.c

void fn_800F9824(void);                 // GameMode7.c

// GameMode8.c
void fn_800F9A58(void);
void fn_800F9AB0(void);
void fn_800F9B34(void);
s32  fn_800F9BF8(int nPlayer);
u8   fn_800FA118(int nPlayer, int a);
void fn_800FA570(void);
void fn_800FDA30(int nPlayer, int a);
u8   fn_800FDF58(int nPlayer);
u8   fn_800FDF60(void);

// after the Stableford code (0x800FF634..)
void fn_800FF7DC(void);
u8   fn_80100294(void);                 // in a lesson (mode 11)

#endif
