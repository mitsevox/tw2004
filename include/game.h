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

#endif
