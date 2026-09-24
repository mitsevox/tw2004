// lighting.h (our name): the scene's light sets. goballfx.c (TW06's name for the file) keeps four
// sets of lights and a pointer to the current one (lbl_80281380); GoTerrain.c chooses the current
// set (fn_80035338) and hands it to the renderer. The lights themselves are GoLighting.c's objects.

#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine.h"
#include "terrain.h"

#define NUM_LIGHT_SETS      4
#define NUM_SET_LIGHTS      5   // four point lights, then one directional light

// A light (GoLighting.c's object, 0x3C bytes; a pool of 25 in GoLighting). What follows nType
// depends on it.
typedef struct GoLight {
    struct GoLight* pNext;      // 0x00  } a ring: the pool's free or used lights
    struct GoLight* pPrev;      // 0x04  }
    s32  nType;                 // 0x08  1: directional (no position), 2: a point light; 0 when
                                //       just taken from the pool
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

// A group of lights lit together (GoLighting.c, 0x38 bytes): fn_8006E5A8 takes its lights from
// the pool, fn_8006E7A4 loads them.
typedef struct LightGroup {
    GoLight* apLight[NUM_SET_LIGHTS];   // 0x00  goballfx.c: [4] is the directional light
    s32  nLights;               // 0x14
    f32  v18[4];                // 0x18  given to fn_8001EF34 with the lights' colours (fn_8006E460)
    f32  v28[4];                // 0x28  goballfx.c: LightParams.v0
} LightGroup;
LAYOUT_ASSERT(LightGroup, 0x38);

// One set of lights (0x8C bytes).
typedef struct LightSet {
    TerSettings settings;       // 0x00  the terrain colours: fn_80035308 copies them to the renderer's
                                //       (fn_80035440), fn_8003534C resets them (fn_8006F334)
    LightGroup group;           // 0x54
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

// GoLighting.c's state (lbl_801D6F58, 0x150 bytes, reached through lbl_802811D8): the pool of
// lights, and the colours and positions of the group last loaded (fn_8006E7A4), up to four point
// lights and an ambient colour.
#define NUM_POOL_LIGHTS  25
#define NUM_POINT_LIGHTS 4
typedef struct GoLighting {
    GoLight* p0;                // 0x000  the pool's first light
    GoLight* pUsed;             // 0x004  } the rings of taken and free lights
    GoLight* pFree;             // 0x008  }
    GoLight* pPool;             // 0x00C  the pool (NUM_POOL_LIGHTS lights)
    s32  nPool;                 // 0x010
    s32  nUsed;                 // 0x014
    s32  nFree;                 // 0x018
    f32  v1C[3];                // 0x01C
    u8   unk28[4];
    f32  aPointColour[NUM_POINT_LIGHTS][4]; // 0x02C  0..255
    f32  vAmbient[4];           // 0x06C  0..255 (from a directional light)
    f32  aPointColour2[NUM_POINT_LIGHTS][4]; // 0x07C  aPointColour through fn_8000AE48
    f32  vAmbient2[4];          // 0x0BC  vAmbient through fn_8000AE48
    f32  afPointX[NUM_POINT_LIGHTS];    // 0x0CC  } the point lights' positions
    f32  afPointY[NUM_POINT_LIGHTS];    // 0x0DC  }
    f32  afPointZ[NUM_POINT_LIGHTS];    // 0x0EC  }
    f32  vFC[4];                // 0x0FC  set by fn_80029BC8
    f32  aPointPos[NUM_POINT_LIGHTS][4];    // 0x10C
    s32  nPoints;               // 0x14C  point lights loaded
} GoLighting;
LAYOUT_ASSERT(GoLighting, 0x150);
extern GoLighting* lbl_802811D8;

// GoLighting.c
void fn_8006E5A8(LightGroup* pGroup, s32 nLights);  // take the group's lights from the pool
void fn_8006E62C(LightGroup* pGroup);               // and give them back
void fn_8006EDC0(LightGroup* pGroup);               // the default lights

LightSet* fn_8003532C(void);    // lbl_80281380->pCur
void fn_80035338(s32 nSet);     // make aSet[nSet] the current set
void fn_8003534C(void);         // reset the current set's terrain colours to the defaults
void fn_80035308(void);
void fn_800352E4(void);

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
