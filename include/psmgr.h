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

#endif
