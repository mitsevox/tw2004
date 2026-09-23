// psmgr.h (our name): PsMgr.c (EA's name, from its asserts), the manager of the weather particle
// effects. Its callers ask for an effect by kind (0-3); on the GameCube only kind 0, the rain
// (GoShaderObject_Rain_Gc.c), does anything.

#ifndef PSMGR_H
#define PSMGR_H

#include "engine.h"

#define PS_NUM_KINDS 4

// PsMgr.c's state, reached through lbl_802813F8 (it points at lbl_80282010).
typedef struct PsMgrState {
    s32  n0;                    // 0x0
    s32  n4;                    // 0x4  flipped by fn_800A2BA8, cleared by fn_800A2934
} PsMgrState;
LAYOUT_ASSERT(PsMgrState, 0x8);

extern PsMgrState* lbl_802813F8;
extern void* lbl_801F16F4[PS_NUM_KINDS];    // each kind's effect object (NULL: none)

void fn_800A2934(void);
void fn_800A2958(void);
void fn_800A295C(void);         // register the 'sfxd' stream handler (fn_800A29B4)
void fn_800A298C(void);         // and unregister it
// Start the effect of kind nKind; pArg points at its strength (NULL: 0.9). n3 is not used.
s32  fn_800A2A80(s32 nKind, f32* pArg, s32 n3);
void fn_800A2B34(s32 nKind);    // stop it
void fn_800A2BA8(void);
void fn_800A2BBC(void);         // move the rain on by a frame
void fn_800A2C08(void);         // draw it

// ---- PsBallFx.c (EA's name, from its asserts): the ball's particle effects ----------------------

// One of PsBallFx.c's particle emitters; only the fields read here. Its size is not known.
typedef struct PsEmitter {
    u8   unk0[0x30];
    f32  v30[4];                // 0x30  a position (fn_800A3D6C)
    u8   unk40[0x50 - 0x40];
    s32  n50;                   // 0x50  fn_800A3DF4 sets 1000000
    u8   unk54[0xB8 - 0x54];
    u32  uB8;                   // 0xB8  flags; the functions below only act with 0x20000 set
    u8   unkBC[0xE0 - 0xBC];
    f32  vE0[4];                // 0xE0  a position (fn_800A3D6C)
} PsEmitter;

// PsBallFx.c's state (lbl_801F1708, 0x88 bytes), reached through lbl_80281408.
typedef struct PsBallFxState {
    u8   mesh[0x28];            // 0x00  a mesh object (Skin.c's fn_80036054; fn_800A2E14 frees it)
    void* p28;                  // 0x28  } freed by fn_800A2E14
    void* p2C;                  // 0x2C  }
    void* p30;                  // 0x30  }
    u8   unk34[0x50 - 0x34];
    void* p50;                  // 0x50  freed by fn_800A2E14
    u8   unk54[0x7C - 0x54];
    PsEmitter* apEmitter[(0x88 - 0x7C) / 4];    // 0x7C  one per view (Player.nView[0])
} PsBallFxState;
LAYOUT_ASSERT(PsBallFxState, 0x88);

extern PsBallFxState* lbl_80281408;

void fn_800A3D6C(f32* pPos, int nPlayer);
void fn_800A3DF4(int nPlayer);

#endif
