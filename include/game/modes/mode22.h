// mode22.h (our name): the state of game mode 22 (GameMode22.c).

#ifndef GAME_MODES_MODE22_H
#define GAME_MODES_MODE22_H

#include "game_types.h"
#include "platform.h"

// Game mode 22's state (lbl_80195498, 0x1C bytes in .data).
typedef struct GameMode22State {
    s32 n0;                     // 0x0  fn_80126FA0 returns it
    s32 n4;                     // 0x4  set by fn_80126F84
    s32 n8;                     // 0x8  fn_80126E88 sets 5
    u8  bC;                     // 0xC  fn_80126FB0 returns it
    u8  unkD[3];
    f32 f10;                    // 0x10  fn_801260C0 sets 0
    s32 n14;                    // 0x14  fn_801260C0 sets 5
    s32 n18;                    // 0x18  fn_801260C0 sets 120; fn_80127004: n8 not 5 and this above 0
} GameMode22State;
LAYOUT_ASSERT(GameMode22State, 0x1C);

extern GameMode22State lbl_80195498;
// The text fn_80127034 prints (starts as "D"; 0xE zero bytes follow it, so likely 16 bytes).
extern char lbl_80195488[];

#endif
