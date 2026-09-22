// Golfer.c: golfer records, attribute access, and the CPU golfer's shot choice. No assert
// names this file; "Golfer.c" is our name. CodeWarrior GC/2.5, -O4,p. What the code does is
// written up in docs/gameplay.md; the record layout in docs/formats/game-data.md.

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
#define CLUB_PUTTER          25   // clubs 0..24 are the bag, 25 the putter
#define SHOT_PUTT            0    // shot kinds (Player.nShotKind)
#define SHOT_FULL            1
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
    u32  nCash;                 // 0x090
    u8   unk94[0x140 - 0x94];
} GolferRecord;

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
    u8   unk3C0[0x410 - 0x3C0];
    f32  fSwingError;           // 0x410
    u8   unk414[0x41C - 0x414];
    f32  fPowerAfterError;      // 0x41C
    u8   unk420[0x9F0 - 0x420];
    f32  fSpinX;                // 0x9F0
    f32  fSpinY;                // 0x9F4
    u8   unk9F8[0xA08 - 0x9F8];
    s32  nController;           // 0xA08  CONTROLLER_CPU for the AI
    u8   unkA0C[0xA14 - 0xA0C];
    f32  fTargetX;              // 0xA14
    f32  fTargetY;              // 0xA18
    f32  fTargetZ;              // 0xA1C
    u8   unkA20[0xA34 - 0xA20];
    f32  vTarget2[3];           // 0xA34
    u8   unkA40[0xA54 - 0xA40];
    f32  fDistance;             // 0xA54  to the target
    u8   unkA58[0xAF8 - 0xA58];
    s32  nLie;                  // 0xAF8
    u8   unkAFC[0xC29 - 0xAFC];
    u8   bLowIQPenalty;         // 0xC29  quarters the IQ overconfidence term when set
    u8   unkC2A[0xEE8 - 0xC2A];
    u32  uFlags;                // 0xEE8
    u8   unkEEC[0xEF8 - 0xEEC];
} Player;

// The round / session state at gSession (0x5BD0 bytes); only what this file reads.
typedef struct Session {
    u32  uFlags;                // 0x000  bit 1: use the alternate attribute block everywhere
    u8   unk4[0x2C - 0x4];
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

int  Game_GetMode(void);                // 0x8000BED8
int  fn_800D2B08(void);
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
double fn_8015F824(double x, double y); // pow
void fn_8000AD10(f32* pDst, f32* pSrc); // vector copy
CourseInfo* fn_8000C594(void);
int  fn_80015464(void);
u8   fn_80101DF4(void);
f32  fn_8005C418(int nSpin);

int  AI_ShotKindForDistance(int nPlayer, f32 fDist);          // 0x8002CF18
int  AI_ClubForShot(int nPlayer, int nKind, int nFlag, f32 fDist);   // 0x8002D1B0
int  fn_8002CC58(int nPlayer, int nKind);
f32  AI_MaxDistance(int nPlayer, int nKind, int nClubInfo);   // 0x8002D074  reads POWER
f32  AI_PowerForTarget(int nPlayer);                          // 0x8002D428
int  AI_ZoneAt(f32* pPos, int nFlag);                         // 0x8002C230  s8: -1 = none
void AI_DefaultTarget(int nPlayer);                           // 0x8002C1A8
void AI_PlanShot(int nPlayer, f32* pTarget);                  // 0x8002BDEC

// ---- small accessors ------------------------------------------------------------------------

u8 Player_IsCPU(int nPlayer) {
    return gPlayers[nPlayer].nController == CONTROLLER_CPU;
}

u8 Controller_IsCPU(int nController) {
    return nController == CONTROLLER_CPU;
}

int Game_CurrentHole(void) {
    return gpGame->holeOrder[gpGame->nCurHole];
}

// Equipment tier -> bonus points. Tiers are 0..4; anything else counts as nothing.
int Golfer_TierBonus(int nTier) {
    int nBonus = 0;
    switch (nTier) {
    case 1: nBonus = 1; break;
    case 2: nBonus = 2; break;
    case 3: nBonus = 3; break;
    case 4: nBonus = 4; break;
    }
    return nBonus;
}

// Which attribute governs a shot: the putter, then bad lies, then a full swing with a long
// club from a good lie, then everything else is an approach.
int Shot_GoverningAttribute(int nPlayer, int nClub, int nLie, int nKind) {
    if (nClub == CLUB_PUTTER) {
        return ATTR_PUTTING;
    }
    if (nLie == 8 || nLie == 13 || nLie == 11 || nLie == 3 || nLie == 4 || nLie == 6 || nLie == 7) {
        return ATTR_RECOVERY;
    }
    if (nKind == SHOT_FULL && nClub < 13 && fn_800D2B08() == 3) {
        return ATTR_BALL_STRIKING;
    }
    return ATTR_APPROACH;
}

// The one place attributes are read. Base = the record value (block B for a CPU pro in game
// mode 4) plus the equipment tier bonus; modifiers are the per-player adjustments.
int Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode) {
    int nValue = 0;
    if (nMode == ATTR_BASE || nMode == ATTR_TOTAL) {
        if (Controller_IsCPU(pPlayer->nController) && pPlayer->golfer.nIndex < FIRST_CREATED_GOLFER &&
            (Game_GetMode() == 4 || (gSession.uFlags & 2))) {
            nValue = (s8)((s32)pPlayer->golfer.attrAlt[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        } else {
            nValue = (s8)((s32)pPlayer->golfer.attr[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        }
    }
    if (nMode == ATTR_MODIFIERS || nMode == ATTR_TOTAL) {
        nValue += pPlayer->attrMod[nAttr];
    }
    return nValue;
}

// Keep base + modifiers inside [10, cap]: 110 for power, IQ and aggression, 100 for the rest.
#define CLAMP_ATTR(pPlayer, nAttr, nCap)                                                         if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) > (nCap)) {                              (pPlayer)->attrMod[nAttr] = (nCap) - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);     } else if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) < 10) {                           (pPlayer)->attrMod[nAttr] = 10 - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);         }

void Golfer_ClampModifiers(Player* pPlayer) {
    CLAMP_ATTR(pPlayer, ATTR_POWER, 110);
    CLAMP_ATTR(pPlayer, ATTR_IQ, 110);
    CLAMP_ATTR(pPlayer, ATTR_AGGRESSION, 110);
    CLAMP_ATTR(pPlayer, ATTR_BALL_STRIKING, 100);
    CLAMP_ATTR(pPlayer, ATTR_APPROACH, 100);
    CLAMP_ATTR(pPlayer, ATTR_PUTTING, 100);
    CLAMP_ATTR(pPlayer, ATTR_RECOVERY, 100);
}

// ---- the CPU golfer ---------------------------------------------------------------------------

f32 AI_Pow(f32 fBase, f32 fExp) {
    return fn_8015F824(fBase, fExp);
}

// Pick where the CPU aims: the most demanding authored aim point it qualifies for. A human
// only ever gets a priority point from here (the default aim on walking up to the ball).
void AI_ChooseTarget(int nPlayer) {
    Player*     p;
    AITarget*   t;
    int         nKind;
    int         nPower;
    int         nAggr;
    int         nHole;
    s8          k;
    s8          nBest;
    s8          nCand;
    int         nSkill;
    int         nIQ;
    f32         fDumb;         // (100 - IQ): the overconfidence term
    f32         fDist2;
    f32         fBestDist2;
    f32         fDist;
    CourseInfo* pCourse;
    s8          nZone;
    int         nClub;
    f32         fDX, fDZ;
    AITarget*   pBest;

    nHole   = Game_CurrentHole();
    pCourse = fn_8000C594();
    p       = &gPlayers[nPlayer];
    nAggr   = Golfer_GetAttribute(p, ATTR_AGGRESSION, ATTR_TOTAL);
    nIQ     = Golfer_GetAttribute(p, ATTR_IQ, ATTR_TOTAL);
    nPower  = Golfer_GetAttribute(p, ATTR_POWER, ATTR_TOTAL);
    nZone   = AI_ZoneAt(&p->fBallX, 0);
    if (nZone == -1) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    nBest      = -1;
    fBestDist2 = 100000000.0f;
    if (gAITargets[nZone].pDef != NULL) {
        fDumb = 100.0f - (f32)(s8)nIQ;
        for (k = 0; k < NUM_AI_LINKS; k++) {
            nCand = (s8)gAITargets[nZone].pDef->nLinks[k];
            if (nCand == -1) continue;
            t = &gAITargets[nCand];
            if (!t->bEnabled) continue;
            fDZ    = pCourse->pin[nHole].z - t->pDef->z;
            fDX    = pCourse->pin[nHole].x - t->pDef->x;
            fDist2 = fDX * fDX + fDZ * fDZ;
            if (t->nTeeSet != -1 && t->nTeeSet != gSession.nTeeSet[nPlayer]) continue;
            if (t->nHole != -1 && t->nHole != nHole) continue;
            if (nBest == -1 && t->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (!Player_IsCPU(nPlayer)) continue;

            fDZ    = p->fBallZ - t->pDef->z;
            fDX    = p->fBallX - t->pDef->x;
            fDist  = fn_80009680(fDX * fDX + fDZ * fDZ);
            nKind  = AI_ShotKindForDistance(nPlayer, fDist);
            nClub  = AI_ClubForShot(nPlayer, nKind, 0, fDist);
            nSkill = Golfer_GetAttribute(p, Shot_GoverningAttribute(nPlayer, nClub, p->nLie, nKind), ATTR_TOTAL);
            if (Player_IsCPU(nPlayer)) {
                // Low IQ makes the golfer think it is better than it is.
                if (p->bLowIQPenalty) {
                    nSkill += (int)(10.0f * (AI_Pow(fDumb, 2.0f) / 100.0f) / 100.0f);
                } else {
                    nSkill += (int)(40.0f * (AI_Pow(fDumb, 2.0f) / 100.0f) / 100.0f);
                }
                if ((s8)nSkill > 100) nSkill = 100;
            }
            if (t->nSkillReq < 0 && (s8)nSkill > IABS((int)t->nSkillReq)) continue;
            if ((s8)nSkill < t->nSkillReq) continue;
            if (t->nAggrReq < 0 && (s8)nAggr > IABS((int)t->nAggrReq)) continue;
            if ((s8)nAggr < t->nAggrReq) continue;
            if (t->nPowerReq < 0 && (s8)nPower > IABS((int)t->nPowerReq)) continue;
            if ((s8)nPower < t->nPowerReq) continue;
            if (fDist > AI_MaxDistance(nPlayer, nKind, fn_8002CC58(nPlayer, nKind))) continue;

            if (nBest == -1) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            pBest = &gAITargets[nBest];
            if (pBest->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            // Prefer the more demanding point: power, then aggression, then skill, then closer.
            if (t->nPowerReq > pBest->nPowerReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nPowerReq != IABS((int)t->nPowerReq)) continue;
            if (t->nAggrReq > pBest->nAggrReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nAggrReq != IABS((int)t->nAggrReq)) continue;
            if (t->nSkillReq > pBest->nSkillReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nSkillReq != IABS((int)t->nSkillReq)) continue;
            if (fDist2 < fBestDist2) {
                nBest      = nCand;
                fBestDist2 = fDist2;
            }
        }
    }
    if (nBest == -1) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    // Already closer to the pin than the chosen point: aim normally instead.
    fDZ = p->fBallZ - pCourse->pin[nHole].z;
    fDX = p->fBallX - pCourse->pin[nHole].x;
    if (fDX * fDX + fDZ * fDZ < fBestDist2 && !(gpGame->unk14 == 3 && fn_80015464() == 17)) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    pBest          = &gAITargets[nBest];
    p->fTargetX    = pBest->pDef->x;
    p->fTargetZ    = pBest->pDef->z;
    p->nTargetType = pBest->nType;
    AI_PlanShot(nPlayer, &p->fTargetX);
    fn_8000AD10(&p->fTargetX, p->vTarget2);
}

// Make the CPU miss: move its aim and distance by up to a shot-type limit scaled by
// (100 - skill), then re-plan for the moved target. Putts under 1.5 are never missed.
void AI_ApplyError(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    f32 fCos;
    f32 fMiss;
    f32 fAimErr;
    f32 fDistErr;
    f32 fSkill;
    f32 fMaxAngle;
    f32 fDist1;
    f32 fDist2;
    f32 fSin;
    f32 fRand;
    int nSpin;
    int nAttr;

    p->fSpinX = 0.0f;
    p->fSpinY = 0.0f;
    if (p->fDistance < 1.0f) return;
    if (p->bPerfect) return;

    nAttr = Shot_GoverningAttribute(nPlayer, p->nClub, p->nLie, p->nShotKind);
    if (nAttr == ATTR_PUTTING) {
        fSkill    = (f32)(s8)Golfer_GetAttribute(p, ATTR_PUTTING, ATTR_TOTAL);
        nSpin     = 0;
        fMaxAngle = DEG(8.0f);
        fDist1    = 20.0f;
        fDist2    = 10.0f;
        if (p->fDistance < 1.5f) return;
        if (p->fDistance < 5.0f) fMaxAngle *= 0.5f;
    } else if (nAttr == ATTR_RECOVERY) {
        fSkill = (f32)(s8)Golfer_GetAttribute(p, ATTR_RECOVERY, ATTR_TOTAL);
        nSpin  = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        if (p->fDistance > 200.0f) {
            fMaxAngle = DEG(2.5f); fDist1 = 6.25f; fDist2 = 2.5f;
        } else if (p->fDistance > 100.0f) {
            fMaxAngle = DEG(5.0f);  fDist1 = 12.5f; fDist2 = 5.0f;
        } else {
            fMaxAngle = DEG(10.0f); fDist1 = 25.0f; fDist2 = 10.0f;
        }
    } else if (nAttr == ATTR_BALL_STRIKING) {
        fSkill    = (f32)(s8)Golfer_GetAttribute(p, ATTR_BALL_STRIKING, ATTR_TOTAL);
        nSpin     = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        fMaxAngle = DEG(2.75f);
        fDist1    = 4.5f;
        fDist2    = 1.0f;
        if (p->fDistance < 150.0f) {
            fMaxAngle *= 2.0f;
            fDist2    *= 2.0f;
        }
    } else {
        fSkill = (f32)(s8)Golfer_GetAttribute(p, ATTR_APPROACH, ATTR_TOTAL);
        nSpin  = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        if (p->fDistance > 100.0f) {
            fMaxAngle = DEG(3.75f); fDist1 = 7.5f; fDist2 = 5.25f;
        } else if (p->fDistance > 50.0f) {
            fMaxAngle = DEG(4.25f); fDist1 = 8.5f; fDist2 = 5.95f;
        } else {
            fMaxAngle = DEG(5.0f);  fDist1 = 10.0f; fDist2 = 7.0f;
        }
        // Even a good golfer occasionally blows an approach.
        if (fSkill > 80.0f) {
            if (Rand_Next(0) % 20 == 0) {
                fMaxAngle *= 4.0f; fDist1 *= 4.0f; fDist2 *= 4.0f; fSkill *= 0.25f;
            } else if (Rand_Next(0) % 10 == 0) {
                fMaxAngle *= 2.0f; fDist1 *= 2.0f; fDist2 *= 2.0f; fSkill *= 0.5f;
            }
        } else if (fSkill > 60.0f) {
            if (Rand_Next(0) % 20 == 0) {
                fMaxAngle *= 2.0f; fDist1 *= 2.0f; fDist2 *= 2.0f; fSkill *= 0.5f;
            }
        }
    }

    if (fSkill > 98.0f) fSkill = 98.0f;
    if (fSkill < 100.0f) {
        f32 fDX, fDZ;
        if (fSkill < 15.0f) fSkill = 15.0f;

        // Aim: up to the angle limit, at least a quarter of a degree, either side.
        fRand   = Rand_Float(0);
        fMiss   = 100.0f - fSkill;
        fAimErr = fMaxAngle * (fMiss * fRand) / 100.0f;
        if (fAimErr < DEG(0.25f)) fAimErr += DEG(0.25f);
        if (Rand_Next(0) & 1) fAimErr *= -1.0f;
        p->fAim += fAimErr;
        if (p->fAim < -PI) {
            p->fAim += TWOPI;
        } else if (p->fAim > PI) {
            p->fAim -= TWOPI;
        }
        fSin = fn_800095F0(p->fAim);
        fCos = fn_80009638(p->fAim);

        // Distance error (in fSkill from here): two percentage terms, either side; one shot
        // kind always comes up short.
        fRand   = Rand_Float(0);
        fDistErr  = fDist1 * (fMiss * fRand) / 100.0f;
        fRand     = Rand_Float(0);
        fDistErr += fDist2 * (fMiss * fRand) / 100.0f;
        if (p->nShotKind != SHOT_FULL || fn_800D2B08() == 3) {
            if (Rand_Next(0) & 1) fDistErr *= -1.0f;
        } else {
            fDistErr *= -1.0f;
        }
        p->fDistance = p->fDistance * ((100.0f + fDistErr) / 100.0f);
        fDX = p->fDistance * -fSin;
        fDZ = p->fDistance * fCos;
        p->fTargetX = p->fBallX + fDX;
        p->fTargetZ = p->fBallZ + fDZ;
        AI_PlanShot(nPlayer, &p->fTargetX);
        p->fPower = AI_PowerForTarget(nPlayer);

        // Spin in proportion to the error, scaled by the SPIN attribute. (Both clamps store +1.)
        if ((s8)nSpin != 0 && fn_80101DF4()) {
            f32 fScale = fn_8005C418(nSpin);
            p->fSpinX  = fDistErr / fDist1;
            p->fSpinX *= fScale;
            p->fSpinY  = fAimErr / fMaxAngle;
            p->fSpinY *= fScale;
            p->fSpinY *= -1.0f;
            if (p->fSpinX > 1.0f) {
                p->fSpinX = 1.0f;
            } else if (p->fSpinX < -1.0f) {
                p->fSpinX = 1.0f;
            }
            if (p->fSpinY > 1.0f) {
                p->fSpinY = 1.0f;
            } else if (p->fSpinY < -1.0f) {
                p->fSpinY = 1.0f;
            }
        }
    }
}
