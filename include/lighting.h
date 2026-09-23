// lighting.h (our name): the scene's light sets. goballfx.c (TW06's name for the file) keeps four
// sets of lights and a pointer to the current one (lbl_80281380); GoTerrain.c chooses the current
// set (fn_80035338) and hands it to the renderer. The lights themselves are GoLighting.c's objects.

#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine.h"

#define NUM_LIGHT_SETS      4
#define NUM_SET_LIGHTS      5   // four point lights, then one directional light

// A light (GoLighting.c's object); only what goballfx.c writes. What follows nType depends on it.
typedef struct GoLight {
    u8   unk0[8];
    s32  nType;                 // 0x08  1: directional (no position), 2: a point light
    union {
        struct {
            f32  fC;            // 0x0C
            f32  f10;           // 0x10
            f32  vColor[4];     // 0x14  half the course light's colour
        } dir;
        struct {
            f32  fC;            // 0x0C
            f32  f10;           // 0x10
            f32  f14;           // 0x14
            f32  f18;           // 0x18
            f32  vColor[4];     // 0x1C  half the course light's colour
            f32  vPos[4];       // 0x2C  the course light's position
        } point;
    } u;
} GoLight;

// One set of lights (0x8C bytes).
typedef struct LightSet {
    u8   unk0[0x54];
    GoLight* apLight[NUM_SET_LIGHTS];   // 0x54  [4] is the directional light
    u8   unk68[0x7C - 0x68];
    f32  v7C[3];                // 0x7C
    u8   unk88[0x8C - 0x88];
} LightSet;
LAYOUT_ASSERT(LightSet, 0x8C);

// lbl_801D9278, reached through lbl_80281380 (0x238 bytes).
typedef struct LightSets {
    LightSet aSet[NUM_LIGHT_SETS];  // 0x000
    LightSet* pCur;             // 0x230  the set in use (fn_80035338 picks it, fn_8003532C reads it)
    u8   unk234[4];
} LightSets;
LAYOUT_ASSERT(LightSets, 0x238);

// Settings for the current light set (0x30 bytes; FEgolferanim.c and Skin.c pass one, fn_80093854).
typedef struct LightParams {
    f32  v0[3];                 // 0x00  -> LightSet.v7C
    u8   unkC[0x10 - 0xC];
    f32  f10;                   // 0x10  -> the directional light's f10
    f32  f14;                   // 0x14  -> point light 3's fC
    f32  f18;                   // 0x18  -> point light 2's fC
    f32  f1C;                   // 0x1C  -> point light 1's fC
    f32  f20;                   // 0x20  -> point light 0's fC
    f32  f24;                   // 0x24  -> point light 0's f10 and f14
    u8   unk28[0x30 - 0x28];
} LightParams;
LAYOUT_ASSERT(LightParams, 0x30);

extern LightSets* lbl_80281380;

LightSet* fn_8003532C(void);    // lbl_80281380->pCur
void fn_80035338(s32 nSet);     // make aSet[nSet] the current set
void fn_80035308(void);
s32  fn_800352E4(void);

void fn_80093524(void);         // create the lights of every set, and use set 0
void fn_80093580(void);         // and free them
void fn_800935CC(struct CourseLights* pLights);        // fill the current set from a hole's lights
void fn_80093854(LightParams* pParams);
void fn_80093900(struct CourseLightBlock* pBlock);

// ---- goballfx.c's ball marker: a quad drawn on the ground under the ball ("marker" texture) ----

extern u8    lbl_80281F40;      // set by fn_80093AD4; fn_80093AE0 passes it on and clears it
extern TexBank*  lbl_80281F44;  // the "marker" texture's bank (BFX_vInit)
extern TexEntry* lbl_80281F48;  // and the texture
extern u8    lbl_801D94B0[0x28];    // the marker's mesh object (Skin.c's fn_80036054 sets it up)

void BFX_vInit(void);

#endif
