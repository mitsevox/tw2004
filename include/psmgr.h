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
extern char lbl_801F1640[20][9];            // the names listed by the 'sfxd' object (fn_800A29B4)

void fn_800A2934(void);
void fn_800A2958(void);
void fn_800A295C(void);         // register the 'sfxd' stream handler (fn_800A29B4)
void fn_800A298C(void);         // and unregister it
// Start the effect of kind nKind; pArg points at its strength (NULL: 0.9). n3 is not used.
s32  fn_800A2A80(s32 nKind, f32* pArg, s32 n3);
void fn_800A2B34(s32 nKind);    // stop it
void fn_800A2BA8(void);
void fn_800A2BBC(int nView);    // move the rain on by a frame (the view is not used)
void fn_800A2C08(int nView);    // draw it (the view is not used)

// ---- PsBallFx.c (EA's name, from its asserts): the ball's particle effects ----------------------

// One of UFstPart.c's particle emitters (PsBallFx.c starts them); only the fields read so far.
// 0x1A8 bytes: fn_80098A98 allocates the six fixed ones at that size.
typedef struct PsEmitter {
    u8   unk0[0x30];
    f32  v30[4];                // 0x30  a position (fn_800A3D6C)
    struct PsEmitter* p40;      // 0x40  the next in the list fn_80099EA4 pushes onto
    u8   unk44[0x4C - 0x44];
    f32  f4C;                   // 0x4C  the last time fn_80099344 moved it on
    s32  n50;                   // 0x50  fn_800A3DF4 sets 1000000
    u8   unk54[0x58 - 0x54];
    s32  n58;                   // 0x58  cleared by fn_80099B74; counted up by fn_80099344
    s8   b5C;                   // 0x5C  cleared by fn_80098BDC
    u8   unk5D[0x60 - 0x5D];
    u8   a60[0x84 - 0x60];      // 0x60  handed to the mesh draw (fn_800990BC); its layout is not known
    f32  f84;                   // 0x84  the radius fn_80099AE4 tests it with (over 1000: always)
    u8   unk88[0xB4 - 0x88];
    s32  nB4;                   // 0xB4  below 0: fn_80098BDC frees the emitter
    u32  uB8;                   // 0xB8  flags; the functions below only act with 0x20000 set;
                                //       fn_80098C70 sets 0x80000000 on the six fixed emitters
    u8   unkBC[0xD0 - 0xBC];
    f32  vD0[4];                // 0xD0  a point fn_80099AE4 puts through the camera's view matrix
    f32  vE0[4];                // 0xE0  a position (fn_800A3D6C)
    u8   unkF0[0x180 - 0xF0];
    u8   mesh[0x28];            // 0x180 a mesh object (Skin.c)
} PsEmitter;
LAYOUT_ASSERT(PsEmitter, 0x1A8);

// What fn_800990BC hands an emitter's mesh (our name): Skin.c's fn_80036100 passes it on.
typedef struct PsEmitterDraw {
    s32  n0;                    // 0x00  1
    u8*  p4;                    // 0x04  the emitter's a60
    PsEmitter* pEmitter;        // 0x08
    s32  nC;                    // 0x0C  fn_800990BC's n, also added to the emitter's n50
    f32  f10;                   // 0x10  } fn_800990BC's two floats
    f32  f14;                   // 0x14  }
} PsEmitterDraw;

// UFstPart.c's six fixed emitters.
extern PsEmitter* lbl_801DB888[6];
extern f32 lbl_801DB878[4];     // UFstPart.c: the point fn_80099E34 orders emitters by distance from

// PsBallFx.c's state (lbl_801F1708, 0x88 bytes), reached through lbl_80281408.
typedef struct PsBallFxState {
    u8   mesh[0x28];            // 0x00  a mesh object (Skin.c's fn_80036054; fn_800A2E14 frees it)
    void* p28;                  // 0x28  } freed by fn_800A2E14; PsBallFx_InitModule allocates
    f32*  p2C;                  // 0x2C  } 0x960, 0x640 and 0x320 bytes; p2C holds the corners
    void* p30;                  // 0x30  } (0,0) (1,0) (0,1) (1,1) of 50 quads
    s32  n34;                   // 0x34  } cleared by fn_800A2E68
    s32  n38;                   // 0x38  }
    s32  n3C;                   // 0x3C  }
    s32  n40;                   // 0x40  }
    s32  n44;                   // 0x44  }
    TexBank*  pBank;            // 0x48  the "sandtrl" texture's bank (PsBallFx_InitModule)
    TexEntry* pTex;             // 0x4C  and the texture
    void* p50;                  // 0x50  freed by fn_800A2E14 (0x4B0 bytes)
    f32  a54[8];                // 0x54  cleared by fn_800A2E68
    PsEmitter* ap74[2];         // 0x74  one per view (Player.nView[0]); fn_800A2FFC
    PsEmitter* apEmitter[(0x88 - 0x7C) / 4];    // 0x7C  one per view (Player.nView[0])
} PsBallFxState;
LAYOUT_ASSERT(PsBallFxState, 0x88);

// An emitter's settings (0x120 bytes): lbl_8018CA98 holds 25 of them; UFstPart.c's fn_80099758
// starts an emitter from one. Only the fields written here.
typedef struct PsEmitterDef {
    u8   unk0[0x40];
    f32  f40;                   // 0x40  written by fn_800A30E4
    u8   unk44[0x80 - 0x44];
    f32  v80[3];                // 0x80  a position (the ball's, fn_800A2FFC)
    u8   unk8C[0xA0 - 0x8C];
    f32  vA0[3];                // 0xA0  fn_800A3CB0 puts the wind vector x 0.1 here
    u8   unkAC[0xF0 - 0xAC];
    f32  vF0[4];                // 0xF0  set per course by fn_800A2E68 (emitters 0, 6 and 14)
    u8   unk100[0x120 - 0x100];
} PsEmitterDef;
LAYOUT_ASSERT(PsEmitterDef, 0x120);

// lbl_8018C868[14][5]: for each kind of ball effect, up to five emitters (fn_800A30E4).
typedef struct PsBallFxEmit {
    s32  nDef;                  // 0x0  the emitter (lbl_8018CA98 index); -1: none
    f32  fMin;                  // 0x4  started only when the caller's value is at least this
} PsBallFxEmit;
extern PsBallFxEmit lbl_8018C868[14][5];

// Two vectors per course for emitters 0, 6 and 14 (fn_800A2E68); lbl_8018E958 replaces them on
// course 18 in some modes.
extern f32 lbl_8018E6B8[21][2][4];         // 21: NUM_COURSE_DATA (game.h)
extern f32 lbl_8018E958[2][4];

extern PsBallFxState* lbl_80281408;
extern PsEmitterDef lbl_8018CA98[25];
extern PsEmitter* lbl_80281F88;             // UFstPart.c's list head (fn_80099EA4)

// UFstPart.c
PsEmitter* fn_80099758(PsEmitterDef* pDef);  // start an emitter from pDef (may return NULL)
void fn_80099EA4(PsEmitter* pEmitter);       // push pEmitter onto the list at lbl_80281F88

void fn_800A3D6C(f32* pPos, int nPlayer);
void fn_800A3DF4(int nPlayer);

#endif
