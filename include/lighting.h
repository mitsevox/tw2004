// lighting.h (our name): the light-fog environments of GoLightFogEnv.c (TW07: GoLightFogEnv.h) and
// the lights of GoLighting.c they are built from. There are four environments, each a fog and a group
// of lights; one is current, and the renderer lights and fogs the scene with it.

#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine.h"
#include "terrain.h"
#include "fog.h"

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
    f32  v18[4];                // 0x18  given to Vec3_Scale with the lights' colours (fn_8006E460)
    f32  v28[4];                // 0x28  goballfx.c: LightParams.v0
} LightGroup;
LAYOUT_ASSERT(LightGroup, 0x38);

// A light-fog environment (TW07: LF_SLightFogEnvironment): a fog and the lights lit with it.
typedef struct LF_SLightFogEnvironment {
    FG_SFogEnvironment fog;     // 0x00  copied to the renderer's fog when the environment is used
    LightGroup group;           // 0x54
} LF_SLightFogEnvironment;
LAYOUT_ASSERT(LF_SLightFogEnvironment, 0x8C);

// Every light-fog environment and the current one (TW06: LF_SLightFogEnvMgr).
typedef struct LF_SLightFogEnvMgr {
    LF_SLightFogEnvironment aSet[NUM_LIGHT_SETS];   // 0x000
    LF_SLightFogEnvironment* pCur;  // 0x230
    u8   unk234[4];
} LF_SLightFogEnvMgr;
LAYOUT_ASSERT(LF_SLightFogEnvMgr, 0x238);

// Settings for the current light set (0x30 bytes; FEgolferanim.c and Skin.c pass one, LF_vSetLightModifiers).
typedef struct LightParams {
    f32  v0[3];                 // 0x00  -> LF_SLightFogEnvironment.v7C
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

extern LF_SLightFogEnvMgr* gpLightFogEnvMgr;    // gLightFogEnvMgr
extern LF_SLightFogEnvMgr gLightFogEnvMgr;

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
    f32  aPointColour2[NUM_POINT_LIGHTS][4]; // 0x07C  aPointColour through LLMath_MultiplyVec
    f32  vAmbient2[4];          // 0x0BC  vAmbient through LLMath_MultiplyVec
    f32  afPointX[NUM_POINT_LIGHTS];    // 0x0CC  } the point lights' positions
    f32  afPointY[NUM_POINT_LIGHTS];    // 0x0DC  }
    f32  afPointZ[NUM_POINT_LIGHTS];    // 0x0EC  }
    f32  vFC[4];                // 0x0FC  set by vec4flt_Zero
    f32  aPointPos[NUM_POINT_LIGHTS][4];    // 0x10C
    s32  nPoints;               // 0x14C  point lights loaded
} GoLighting;
LAYOUT_ASSERT(GoLighting, 0x150);
extern GoLighting* lbl_802811D8;

// GoLighting.c
void fn_8006E5A8(LightGroup* pGroup, s32 nLights);  // take the group's lights from the pool
void fn_8006E62C(LightGroup* pGroup);               // and give them back
void fn_8006EDC0(LightGroup* pGroup);               // the default lights
void fn_8006F144(struct LF_SLightFogEnvironment* pEnv);
void fn_8006F148(struct LF_SLightFogEnvironment* pEnv);

// Skin.c
void LF_LoadCurrentLights(void);

LF_SLightFogEnvironment* LF_spGetCurrentLightFogEnvironment(void);
void LF_vSetCurrentLightFogEnvironment(s32 nSet);
void LF_ResetCurrentFogSettings(void); // reset the current set's terrain colours to the defaults
void LF_UseCurrentFogSettings(void);
void LF_UpdateFog(void);

void LF_vInitModule(void);
void LF_vCloseModule(void);
void LF_vSetLightingEnvironment(struct TGD_LightingData* pLighting);
void LF_vSetDynamicLightModifiers(void);
void LF_vSetLightModifiers(LightParams* pParams);
void LF_vSetFoggingEnvironment(struct TGD_FoggingData* pFogging);
void LF_vInitLightFogEnvironment(LF_SLightFogEnvironment* pEnv);
void LF_vFreeLightFogEnvironment(LF_SLightFogEnvironment* pEnv);
f32  LF_fComputeAngleBetweenLightAndCamera(s32 nLight, struct CamLens* pLens);
f32  LF_fComputeAngleToLight(GoLight* pLight, struct CamLens* pLens);

// ---- goballfx.c's ball marker: a quad drawn on the ground under the ball ("marker" texture) ----

extern u8    lbl_80281F40;      // set by fn_80093AD4; BFX_vRender passes it on and clears it
extern TexBank*  lbl_80281F44;  // the "marker" texture's bank (BFX_vInit)
extern TexEntry* lbl_80281F48;  // and the texture
extern u8    lbl_801D94B0[0x28];    // the marker's mesh object (Skin.c's SD_InitShaderObject sets it up)
extern u8    lbl_80189CB0[6][4];    // each player's marker colour (the last two are 0)
extern f32   lbl_801D94D8[5][8];    // per player: the marker's texture coordinates
extern u8    lbl_801D9578[5][16];   // per player: its vertex colours
extern f32   lbl_801D95C8[5][12];   // per player: its four corners

void BFX_vInit(void);

#endif
