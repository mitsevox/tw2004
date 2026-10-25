// materialtypes.h (our name): the TGD_MaterialInfo row, split out of ball.h so that MaterialTypes.c,
// which defines the const table, need not see ball.h's non-const extern of it. ball.h includes
// this file.

#ifndef MATERIALTYPES_H
#define MATERIALTYPES_H

#include "game_types.h"
#include "platform.h"

// TGD_MaterialInfo.nClass: the kind of ground a surface is. Physics_SetLie makes the ball's lie
// (Lie_t) from it and DynamicCam_MaterialToCameraLie the camera's. Classes 13 and 14 (one row of
// the data each) have no name: no code tests them. 21 has none either: no row has it, and only
// modifyFinalLie sets it, for the situation scripts.
typedef enum SurfaceClass_t {
    SURFCLASS_NONE_e = 0,           // no surface found (what callers use then)
    SURFCLASS_TEE_e = 1,            // the tee box: a fairway lie; the camera's tee under a teed ball
    SURFCLASS_FAIRWAY_e = 2,
    SURFCLASS_GREEN_e = 3,
    SURFCLASS_FRINGE_e = 4,
    SURFCLASS_ROUGH_e = 5,          // a random rough lie, high or not
    SURFCLASS_SAND_e = 6,           // a bunker: a random sand lie, and the ball takes that lie's row
    SURFCLASS_WATER_e = 7,          // water hazard: speed golf drops the ball beside it
    SURFCLASS_CARTPATH_e = 8,
    SURFCLASS_OFF_LIMITS_e = 9,     // ground the camera keeps off as it does SURFCLASS_OTHER_e
                                    // (CamScript_PlayableSurface); a LIE_MISC_e lie. 46 of its 48
                                    // rows let no ball lie and carry penalty bit 0x2, so
                                    // GM_CheckForBallOOB takes the ball off with a penalty stroke;
                                    // row 132 (0x8) takes it off without one, row 149 (0x8D) lets
                                    // it lie
    SURFCLASS_OTHER_e = 10,         // any other ground a ball may stop on (LIE_MISC_e); no drops on
                                    // it, the camera keeps off it, and the cameras assume it when
                                    // the look-ahead ball found no surface
    SURFCLASS_THICK_ROUGH_e = 11,   // thick rough, or by luck a plain rough lie
    SURFCLASS_CUP_e = 12,           // the cup: a ball on it is holed
    SURFCLASS_HIDES_BALL_e = 15,    // a ball out of bounds on it is not drawn, as in water
                                    // (DynObj_bDrawBall), and the placement cursor crosses it at
                                    // water's pace; a LIE_MISC_e lie
    SURFCLASS_WATER_OOB_e = 16,     // water: a water lie, but speed golf replaces the ball as out
                                    // of bounds instead of dropping it beside
    SURFCLASS_TREE_e = 17,          // a tree: a random bounce off it
    SURFCLASS_CUP_GREEN_e = 18,     // green at the cup: a green lie, and a ball sunk below the pin's
                                    // foot on it drops in
    SURFCLASS_NOT_PLAYABLE_e = 19,  // ground off the playable course; the camera's out of bounds
    SURFCLASS_SAND_KEEPS_ROW_e = 20 // sand whose ball gets a random sand lie but keeps this row (146)
} SurfaceClass_t;

// A row of gSurfaceTypes (0x44 bytes): how a ball behaves on one kind of ground. Its field names
// after "TW06:" agree with what the code here does with each field.
typedef struct TGD_MaterialInfo {
    f32  f00;                   // 0x00  launch: share of the speed kept; + the ball's fLieModifier
                                //       (Physics_GetLiePowerPercentage). TW06: impactV
    f32  f04;                   // 0x04  lie: size of the random lie quality (Physics_SetLie).
                                //       TW06: impactV_Modifier
    f32  f08;                   // 0x08  launch: spin factor. TW06: impactSpin
    f32  fRestitution;          // 0x0C  bounce restitution. Below 0 (rows 96 and 97): a random one,
                                //       swayed by ATTR_LUCK, when the ball's rolls are random
                                //       (Physics_HandleCollision); Ter_CheckForSolidWorldCollision
                                //       ignores the surface
    f32  f10;                   // 0x10  bounce: friction at the contact. TW06: grip
    f32  f14;                   // 0x14  skid: 1 - this scales the slope pull. TW06: kinetic
    f32  f18;                   // 0x18  skid: friction building roll spin. TW06: transKinetic
    f32  f1C;                   // 0x1C  0.375 on surfaces a ball may stop on; roll: break strength.
                                //       TW06: precession
    f32  f20;                   // 0x20  roll: rolling friction. TW06: rolling
    f32  f24;                   // 0x24  bounce: how hard a landing it takes to bend the normal (softness).
                                //       TW06: terminalVy
    f32  f28;                   // 0x28  bounce: base softness. TW06: surfaceFriction
    u32  nClass;                // 0x2C  surface class (SurfaceClass_t), not a Lie_t
    u32  nSoundId;              // 0x30  TW06: soundID; nothing here reads it yet
    u32  uFlags;                // 0x34  MATERIAL_BALL_CAN_LIE; 0x2: taking the ball out sets the
                                //       player's bPenaltyShot; 0x10: event 0x25 on landing;
                                //       MATERIAL_NO_SHADOW; MATERIAL_COLLIDES_AS_OBJECT
    u32  nCollisionEffectId;    // 0x38  TW06: uiCollisionEffectID
    u32  nSwingSoundId;         // 0x3C  TW06: uiSwingSoundID
    u32  nSwingEffectId;        // 0x40  TW06: uiSwingEffectID
} TGD_MaterialInfo;
LAYOUT_ASSERT(TGD_MaterialInfo, 0x44);

// TGD_MaterialInfo.uFlags: a ball may lie or be dropped here (without it GameManager takes the
// ball out)
#define MATERIAL_BALL_CAN_LIE 0x01

// TGD_MaterialInfo.uFlags: bits 5 and 6, either of which keeps the golfer's shadow off the surface
// (the cup has both; a cart path row and three others have 0x20)
#define MATERIAL_NO_SHADOW 0x60

// TGD_MaterialInfo.uFlags: ground that Ter_CheckForObjectCollision tests with the course objects,
// so a ball on the ground runs into it (Physics_CheckObjectCollisions); only row 149 has it
#define MATERIAL_COLLIDES_AS_OBJECT 0x80

#define NUM_SURFACE_TYPES 156   // rows in gSurfaceTypes

// The flagstick's row of gSurfaceTypes: the surface Ter_CheckForPinCollision gives a ball that hits
// the stick. A hit on it fires the flagstick's event (EVENT_CollisionPin), and a ball in the air
// that hits it loses its spin (Physics_CheckTerrainCollisions).
#define SURFACE_FLAGSTICK 90

// ---- MaterialTypes.c ------------------------------------------------------------------------

s32 MaterialTypes_getMaterialID(const TGD_MaterialInfo* material);   // a surface's row, or -1

#endif
