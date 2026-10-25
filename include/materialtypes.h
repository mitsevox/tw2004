// materialtypes.h (our name): the TGD_MaterialInfo row, split out of ball.h so that MaterialTypes.c,
// which defines the const table, need not see ball.h's non-const extern of it. ball.h includes
// this file.

#ifndef MATERIALTYPES_H
#define MATERIALTYPES_H

#include "game_types.h"
#include "platform.h"

// A row of gSurfaceTypes (0x44 bytes): how a ball behaves on one kind of ground. Its field names
// after "TW06:" agree with what the code here does with each field.
typedef struct TGD_MaterialInfo {
    f32  f00;                   // 0x00  launch: share of the speed kept; + the ball's fLieModifier
                                //       (Physics_GetLiePowerPercentage). TW06: impactV
    f32  f04;                   // 0x04  lie: size of the random lie quality (Physics_SetLie).
                                //       TW06: impactV_Modifier
    f32  f08;                   // 0x08  launch: spin factor. TW06: impactSpin
    f32  f0C;                   // 0x0C  bounce restitution; below 0: branches/leaves (randomised, LUCK).
                                //       TW06: restitution
    f32  f10;                   // 0x10  bounce: friction at the contact. TW06: grip
    f32  f14;                   // 0x14  skid: 1 - this scales the slope pull. TW06: kinetic
    f32  f18;                   // 0x18  skid: friction building roll spin. TW06: transKinetic
    f32  f1C;                   // 0x1C  0.375 on surfaces a ball may stop on; roll: break strength.
                                //       TW06: precession
    f32  f20;                   // 0x20  roll: rolling friction. TW06: rolling
    f32  f24;                   // 0x24  bounce: how hard a landing it takes to bend the normal (softness).
                                //       TW06: terminalVy
    f32  f28;                   // 0x28  bounce: base softness. TW06: surfaceFriction
    u32  nClass;                // 0x2C  surface class (TW06: lieID), not a Lie_t. Physics_SetLie makes the
                                //       lie from it: 1, 2 fairway; 3 green; 4 fringe; 5, 11 rough; 6, 20
                                //       sand; 7, 16 water; 8 cart path; 12 the cup; 18 green (holes a ball,
                                //       as 12). 17 = tree; 19 = not playable
                                //       (Ter_CalcLowestPlayableWorldHeight)
    u32  nSoundId;              // 0x30  TW06: soundID; nothing here reads it yet
    u32  uFlags;                // 0x34  Bit 0x1: a ball may lie or be dropped here (without it
                                //       GameManager takes the ball out); 0x2: taking it out sets the
                                //       player's bPenaltyShot; 0x10: event 0x25 on landing; 0x80 is
                                //       read by GoTerrainCollision; MATERIAL_NO_SHADOW
    u32  nCollisionEffectId;    // 0x38  TW06: uiCollisionEffectID
    u32  nSwingSoundId;         // 0x3C  TW06: uiSwingSoundID
    u32  nSwingEffectId;        // 0x40  TW06: uiSwingEffectID
} TGD_MaterialInfo;
LAYOUT_ASSERT(TGD_MaterialInfo, 0x44);

// TGD_MaterialInfo.uFlags: bits 5 and 6, either of which keeps the golfer's shadow off the surface
// (the cup has both; a cart path row and three others have 0x20)
#define MATERIAL_NO_SHADOW 0x60

#define NUM_SURFACE_TYPES 156   // rows in gSurfaceTypes

#endif
