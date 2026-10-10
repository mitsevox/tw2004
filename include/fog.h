// fog.h (TW06: hi-rendering/headers/gofog.h): the distance fog of GoFog.c. The fog has a colour and
// distance for each of four directions a quarter turn apart; each frame the two the camera faces are
// blended into the fog the renderer draws with.

#ifndef FOG_H
#define FOG_H

#include "game_types.h"
#include "platform.h"

#define NUM_FOG_DIRECTIONS 4

// A fog environment (TW06: FG_SFogEnvironment). Each colour is red, green and blue (0..1) followed by
// the fog distance.
typedef struct FG_SFogEnvironment {
    f32  aDirection[NUM_FOG_DIRECTIONS][4];  // 0x00  a quarter turn apart, starting along +x
    f32  fRotation;             // 0x40  radians added to the camera's heading before blending
    f32  vCurrent[4];           // 0x44  the blend for the camera: colour 0..255, then the distance
} FG_SFogEnvironment;
LAYOUT_ASSERT(FG_SFogEnvironment, 0x54);

extern FG_SFogEnvironment* gpFogEnvironment;   // the fog the renderer uses: gFogEnvironment
extern FG_SFogEnvironment gFogEnvironment;

void FG_vBlendFogForCamera(void);
void FG_vSetDefaultFog(FG_SFogEnvironment* pFog);
f32* FG_spSetFogDirection(FG_SFogEnvironment* pFog, int nDirection, f32* pColour, f32 fDistance);
void FG_vSetFogRotation(FG_SFogEnvironment* pFog, f32 fRotation);

#endif
