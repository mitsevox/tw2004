// Ball physics enums. Names and values from the Tiger Woods PGA Tour 06 Xbox beta PDB (2005),
// same studio and engine lineage (see docs/tw2004-notes.md, "Symbols from related builds").
// Every value used here was checked against this game's code; the rest are TW06's list.
#ifndef PHYSICS_H
#define PHYSICS_H

// Ball->nLie. This game already uses 3/4/5 (the roughs), 12 (in the cup) and 16 (out of bounds,
// which it also uses for water).
typedef enum Lie_t {
    LIE_TEE_e = 0,
    LIE_FAIRWAY_e = 1,
    LIE_FAIRWAY_TIGHT_e = 2,
    LIE_ROUGH_HIGH_e = 3,
    LIE_ROUGH_e = 4,
    LIE_THICK_ROUGH_e = 5,
    LIE_SAND_HIGH_e = 6,
    LIE_SAND_MEDIUM_e = 7,
    LIE_SAND_DEEP_e = 8,
    LIE_GREEN_e = 9,
    LIE_FRINGE_e = 10,
    LIE_CARTPATH_e = 11,
    LIE_INCUP_e = 12,
    LIE_WATER_e = 13,
    LIE_ICE_e = 14,
    LIE_SNOW_e = 15,
    LIE_OUT_OF_BOUNDS_e = 16,
    LIE_MISC_e = 17,
    LIE_MAX_e = 18
} Lie_t;

// Ball->nState. State 5 is also where water leaves the ball in this game.
typedef enum physicsBallState_t {
    PHYSICS_BALLSTATE_BallDead_e = 0,
    PHYSICS_BALLSTATE_BallWaiting_e = 1,
    PHYSICS_BALLSTATE_BallFlying_e = 2,
    PHYSICS_BALLSTATE_BallRollingAndSlipping_e = 3,
    PHYSICS_BALLSTATE_BallPureRolling_e = 4,
    PHYSICS_BALLSTATE_BallOutOfBounds_e = 5,
    PHYSICS_BALLSTATE_BallScripted_e = 6
} physicsBallState_t;

typedef enum Club_t {
    CLUB_DRIVER1_e = 0, CLUB_DRIVER2_e, CLUB_DRIVER3_e, CLUB_DRIVER4_e, CLUB_DRIVER5_e, CLUB_DRIVER6_e,
    CLUB_3WOOD_e, CLUB_5WOOD_e, CLUB_7WOOD_e,
    CLUB_1IRON_e, CLUB_2IRON_e, CLUB_3IRON_e, CLUB_4IRON_e, CLUB_5IRON_e, CLUB_6IRON_e, CLUB_7IRON_e,
    CLUB_8IRON_e, CLUB_9IRON_e,
    CLUB_PITCHINGWEDGE_e, CLUB_APPROACHWEDGE_e, CLUB_GAPWEDGE_e, CLUB_SANDWEDGE_e,
    CLUB_LOWBOUNCEWEDGE_e, CLUB_LOBWEDGE_e, CLUB_HIGHLOBWEDGE_e,
    CLUB_PUTTER_e = 25,
    CLUB_MAX_e = 26
} Club_t;

typedef enum ShotType_t {
    SHOT_TYPE_PUTT_e = 0,
    SHOT_TYPE_DRIVE_e = 1,
    SHOT_TYPE_CHIP_e = 2,
    SHOT_TYPE_PITCH_e = 3,
    SHOT_TYPE_PUNCH_e = 4,
    SHOT_TYPE_FLOP_e = 5
} ShotType_t;

typedef enum PhysicsMishitType {
    PMT_NONE = 0,
    PMT_CHILI_DIP = 1,
    PMT_BLADED = 2,
    PMT_SHANK = 3,
    PMT_DUCK_HOOK = 4,
    PMT_TOPPED = 5
} PhysicsMishitType;

#endif
