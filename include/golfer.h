// golfer.h: the golfer record, the player struct and the attribute accessor, shared by
// Golfer.c (the CPU golfer) and Swing.c (the human swing). Layouts are from the reads in
// those files; see docs/formats/game-data.md and docs/gameplay.md.

#ifndef GOLFER_H
#define GOLFER_H

#include "game_types.h"

// ---- attributes -----------------------------------------------------------------------------

enum {
    ATTR_POWER,             // 0
    ATTR_POWER_BOOST,       // 1
    ATTR_AGGRESSION,        // 2   hidden: CPU target choice
    ATTR_BALL_STRIKING,     // 3
    ATTR_DRIVING_ACCURACY,  // 4
    ATTR_APPROACH,          // 5
    ATTR_PUTTING,           // 6
    ATTR_RECOVERY,          // 7
    ATTR_IQ,                // 8   hidden: CPU overconfidence
    ATTR_SPEED,             // 9   hidden: animation
    ATTR_SPIN,              // 10
    ATTR_LUCK,              // 11
    NUM_ATTRS
};

#define ATTR_BASE      0    // Golfer_GetAttribute modes: the golfer's own value plus equipment
#define ATTR_MODIFIERS 1    // the per-player modifier only
#define ATTR_TOTAL     2    // both

#define FIRST_CREATED_GOLFER 30   // table slots 30..33 are the created golfers
#define CONTROLLER_CPU       9
#define NUM_CLUBS            26   // clubs 0..24 are the bag, 25 the putter
#define CLUB_PUTTER          25
#define CLUB_SAND_WEDGE      21
#define SHOT_PUTT            0    // shot kinds (Player.nShotKind)
#define SHOT_FULL            1
#define SHOT_CHIP            2
#define SHOT_PITCH           3
#define LIE_GREEN            9
#define NUM_AI_LINKS         10   // candidate aim points per zone

#define PI    3.14159265f
#define TWOPI 6.28318531f
#define DEG(x) ((x) * (PI / 180.0f))

#define IABS(v) (((v) ^ ((v) >> 31)) - ((v) >> 31))   // what the compiler emits for abs()

// One 320-byte row of STATS_GC.BIN as it sits in gGolferTable. The file has a 2-byte header,
// so every field is 2 bytes later than in the file; the game reuses the first byte as the
// golfer index once a row is copied into a player.
typedef struct GolferRecord {
    u8   nIndex;                // 0x000
    u8   unk1;                  // 0x001
    char szFirst[32];           // 0x002
    char szLast[32];            // 0x022
    char szNick[32];            // 0x042
    u8   unk62[6];              // 0x062
    s8   attr[NUM_ATTRS];       // 0x068  block A
    s8   attrAlt[NUM_ATTRS];    // 0x074  block B: used for CPU pros in game mode 4
    s8   tier[15];              // 0x080  equipment tiers 0..4, one per attribute
    u8   unk8F;                 // 0x08F
    u32  uBagMask;              // 0x090  bit n set = club n is in the bag
    u8   unk94[0x140 - 0x94];
} GolferRecord;

// The swing meter's per-player state, embedded in Player at 0x3D4 (offsets below are within
// this struct; add 0x3D4 for the player offset).
typedef struct SwingData {
    u8   unk0[0x3C];
    f32  fSwingError;           // 0x03C  (0x410) the stick's miss, after forgiveness
    u8   unk40[0x48 - 0x40];
    f32  fPowerAfterError;      // 0x048  (0x41C)
    f32  fTempo;                // 0x04C  (0x420) driver sweet-spot input (negative when armed)
    u8   unk50[0x460 - 0x50];
    s32  nRumbleFrames;         // 0x460  (0x834)
    u8   bRumble;               // 0x464  (0x838)
    u8   unk465[0x494 - 0x465];
    s32  nBoostLevel;           // 0x494  (0x868) power boost level pressed, 0..8
    u8   unk498[0x610 - 0x498];
    s32  nSpinAmount;           // 0x610  (0x9E4) how much spin was asked for, 0..20
    s32  nSpinStickX;           // 0x614  (0x9E8) 0..255, 128 centre
    s32  nSpinStickY;           // 0x618  (0x9EC)
    f32  fSpinX;                // 0x61C  (0x9F0)
    f32  fSpinY;                // 0x620  (0x9F4)
    u8   unk624[0x634 - 0x624];
} SwingData;

// A player in the current round (human or CPU). 0xEF8 bytes; only the fields read so far.
typedef struct Player {
    s32  nIndex;                // 0x000
    s32  unk4;                  // 0x004
    GolferRecord golfer;        // 0x008
    s8   attrMod[NUM_ATTRS];    // 0x148  modifiers on top of the record
    u8   unk154[0x354 - 0x154];
    s32  nClub;                 // 0x354
    u8   unk358[0x378 - 0x358];
    f32  fAim;                  // 0x378  aim angle, radians
    f32  fPower;                // 0x37C  0..1 (up to 1.5)
    s32  nShotKind;             // 0x380  0 putt, 2/3 approach, 5..7 recovery
    u8   unk384[0x3A8 - 0x384];
    s32  nTargetType;           // 0x3A8
    u8   bPerfect;              // 0x3AC  no error / no forgiveness when set
    u8   unk3AD[0x3B4 - 0x3AD];
    f32  fBallX;                // 0x3B4
    f32  fBallY;                // 0x3B8
    f32  fBallZ;                // 0x3BC
    u8   unk3C0[0x3D4 - 0x3C0];
    SwingData swing;            // 0x3D4  the swing meter's state for this player
    s32  nController;           // 0xA08  CONTROLLER_CPU for the AI
    u8   unkA0C[0xA14 - 0xA0C];
    f32  fTargetX;              // 0xA14
    f32  fTargetY;              // 0xA18
    f32  fTargetZ;              // 0xA1C
    u8   unkA20[0xA24 - 0xA20];
    f32  vTargetCopy[4];        // 0xA24  copy of the planned target
    f32  vTarget2[4];           // 0xA34  copy of the chosen aim point
    u8   unkA44[0xA54 - 0xA44];
    f32  fDistance;             // 0xA54  to the target
    f32  fDistance2;            // 0xA58
    u8   unkA5C[0xA68 - 0xA5C];
    s32  nSurface;              // 0xA68  surface type under the target, -1 none, 16 water
    u8   unkA6C[0xA90 - 0xA6C];
    u8   unkA90[0xAF8 - 0xA90]; // 0xA90  club/shot parameters (opaque here)
    s32  nLie;                  // 0xAF8
    u8   unkAFC[0xC29 - 0xAFC];
    u8   bLowIQPenalty;         // 0xC29  quarters the IQ overconfidence term when set
    u8   unkC2A[0xEE8 - 0xC2A];
    u32  uFlags;                // 0xEE8
    u8   unkEEC[0xEF0 - 0xEEC];
    u32  uFlagsEF0;             // 0xEF0  bit 1: target is over water
    u8   unkEF4[0xEF8 - 0xEF4];
} Player;

// Terrain surface descriptors (0x44 bytes each); only the index of one is used here.
typedef struct SurfaceType {
    u8   unk[0x44];
} SurfaceType;

// The round / session state at gSession (0x5BD0 bytes); only what this file reads.
typedef struct Session {
    u32  uFlags;                // 0x000  bit 1: use the alternate attribute block everywhere
    u8   unk4[0x13 - 0x4];
    u8   bNoSpin;               // 0x013  spin control switched off
    u8   unk14[0x2C - 0x14];
    s32  nNumPlayers;           // 0x02C
    u8   unk30[0x44 - 0x30];
    s32  nGolfer[4];            // 0x044  golfer index per player
    u8   unk54[0x58 - 0x54];
    s32  nTeeSet[4];            // 0x058
    u8   unk68[0x5BD0 - 0x68];
} Session;

// The game state gpGame points at; only what this file reads.
typedef struct GameState {
    s32  nMode;                 // 0x000
    u8   unk4[0x14 - 0x4];
    s32  unk14;                 // 0x014
    u8   unk18[0x64 - 0x18];
    s32  nCurHole;              // 0x064  index into holeOrder
    u8   unk68[0xE4 - 0x68];
    s32  holeOrder[18];         // 0x0E4
} GameState;

// An authored aim point. pDef points at its position and the up-to-ten other points a golfer
// standing in its zone may aim at; the bytes are filters and requirements (negative = at most).
typedef struct AITargetDef {
    f32  x, y, z;               // 0x00
    u32  unkC;                  // 0x0C
    s16  nLinks[NUM_AI_LINKS];  // 0x10  indices into gAITargets, -1 = none
} AITargetDef;

typedef struct AITarget {
    AITargetDef* pDef;          // 0x00
    u8   bEnabled;              // 0x04
    s8   nTeeSet;               // 0x05  -1 = any
    s8   nHole;                 // 0x06  -1 = any
    s8   nSkillReq;             // 0x07
    s8   nAggrReq;              // 0x08
    s8   bPriority;             // 0x09  taken when nothing else qualifies (and by humans)
    s8   nType;                 // 0x0A
    s8   nPowerReq;             // 0x0B
} AITarget;

// Per-course data: only the pin positions are read here.
typedef struct PinPos {
    f32  x, y, z, w;
} PinPos;

typedef struct CourseInfo {
    u8     unk0[0x70];
    PinPos pin[18];             // 0x70
} CourseInfo;

extern GolferRecord gGolferTable[34];   // 0x801CB300  STATS_GC.BIN as loaded
extern GolferRecord gCurGolferRecord;   // 0x801CB1C0  the created golfer being edited
extern Player       gPlayers[5];        // 0x801C66E8
extern Session      gSession;           // 0x801CDD80
extern GameState*   gpGame;             // 0x80281588

extern AITarget     gAITargets[25];     // 0x801C65B8
extern s32          gNumAITargets;      // 0x80281D44
extern u8           gClubKindTable[8][NUM_CLUBS];   // 0x801874B0  which clubs each shot kind allows
extern f32          gClubDistAtPower0[NUM_CLUBS];   // 0x80187580  reach at POWER 0 (245 for the woods)
extern f32          gClubPowerStep[NUM_CLUBS];      // 0x801875E8  reach gained per POWER point over 100
extern SurfaceType  gSurfaceTypes[];    // 0x8017E9B8

int  Game_GetMode(void);                // 0x8000BED8
int  fn_800D2B08(void);
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
double fn_8015F824(double x, double y); // pow
void Vec_Copy(f32* pSrc, f32* pDst);   // 0x8000AD10
f32  Terrain_HeightAt(f32* pPos, SurfaceType** ppSurface);   // 0x800447DC
f32  fn_80050D34(f32 fDist);            // putt power for a distance
f32  fn_80050F44(int nKind, int nClub); // a club's table reach for a shot kind
f32  fn_80050F88(f32 fDist, u8* pParams, int nKind, int nClub);   // chip power
f32  fn_800510EC(u8* pParams);          // distance scale
u8   fn_8002CEDC(int nPlayer);
int  fn_80100744(void);                 // shot kind override, 8 = none
int  fn_801006F0(int nPlayer);          // club override, 26 = none
CourseInfo* fn_8000C594(void);
int  fn_80015464(void);
u8   fn_80101DF4(void);
f32  fn_8005C418(int nSpin);

u8   Club_UsableForKind(int nPlayer, int nClub, int nKind);
int  AI_FirstUsableClub(int nPlayer, int nKind);
f32  AI_MaxDistance(int nPlayer, int nKind, int nClub);
int  AI_ShotKindForDistance(int nPlayer, f32 fDist);
int  AI_ClubForShot(int nPlayer, int nKind, u8 bUnderOnly, f32 fDist);
f32  AI_PowerForTarget(int nPlayer);
u8   AI_WithinOfPin(int nPlayer, f32 fDist);                  // 0x8002CD9C
s8   AI_NearestTarget(f32* pPos, f32* pOut);
void AI_DefaultTarget(int nPlayer);
void AI_PlanShot(int nPlayer, f32* pTarget);

#endif
