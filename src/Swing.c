// Swing.c: the human swing - the meter's error and power, and what the golfer's attributes do
// to them. Named by its assert string at 0x8028118C. CodeWarrior GC/2.5, -O4,p. The formulas
// and tables are written up in docs/gameplay.md.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

// The swing module's state; only the tuning values read here. Set up in Swing_Init.
typedef struct SwingState {
    TexBank*  pBank;            // 0x000  the bank of the three textures below
    TexEntry* pClubBack;        // 0x004  texture "clubback": the backswing trail's
    TexEntry* pClubDown;        // 0x008  texture "clubdown": the downswing trail's
    TexEntry* pTBall;           // 0x00C  texture "tball"
    u8   unk10[0x44 - 0x10];
    u8   mesh[2][0x28];         // 0x044  the trail mesh, per view
    f32* p94[2];               // 0x094  the trail's vertex positions, per view
    u8*  p9C[2];               // 0x09C  its vertex colours (RGBA)
    f32* pA4[2];               // 0x0A4  its texture coordinates
    f32  fCurveMin;             // 0x0AC  a club's shaping range, by gClubCurve
    f32  fCurveMax;             // 0x0B0
    f32  fB4;                   // 0x0B4
    f32  fKnot1Y;               // 0x0B8  } the backswing-angle response curve: two knots
    f32  fKnot1X;               // 0x0BC  0.4
    f32  fKnot2Y;               // 0x0C0
    f32  fKnot2X;               // 0x0C4  0.6
    f32  fTeeBonus;             // 0x0C8  0.1
    f32  fCC;                   // 0x0CC  40
    s32  nD0;                   // 0x0D0  1
    f32  fPutting;              // 0x0D4  player 1's golfer ratings, as floats
    f32  fBallStriking;         // 0x0D8
    f32  fApproach;             // 0x0DC
    f32  fRecovery;             // 0x0E0
    f32  fSpin;                 // 0x0E4
    f32  fPower;                // 0x0E8
    f32  fEC;                   // 0x0EC  0.5
    f32  fF0;                   // 0x0F0  0.5
    f32  fF4;                   // 0x0F4  0.5
    f32  fF8;                   // 0x0F8  0.05
    f32  fFC;                   // 0x0FC  0.5
    f32  f100;                  // 0x100  0.02395
    f32  f104;                  // 0x104  0.00161
    f32  f108;                  // 0x108  0.95
    f32  f10C;                  // 0x10C  0.95
    f32  f110;                  // 0x110  0.01
    f32  fMaxError;             // 0x114  the meter's largest miss, radians
    f32  fPuttFullPower;        // 0x118  0.75: a putt meter over this counts as full
} SwingState;

typedef struct SwingStack {
    s8   nState[5];             // 0x00
    s8   nTop;                  // 0x05  index of the current state, -1 when empty
} SwingStack;

// A second, single-slot state machine (no player argument): current index lbl_80281E09.
typedef struct ModeStateDef {
    void (*pfnEnter)(void);
    void (*pfnUpdate)(void);
    void (*pfnExit)(void);
} ModeStateDef;

typedef struct TrailDraw {
    s32  nPrims;                // 0x0
    s16  nFirst;                // 0x4
    s16  nCount;                // 0x6
} TrailDraw;

typedef struct TrailMeshDesc {
    s16        n0;              // 0x00
    s16        nVerts;          // 0x02
    TrailDraw* pDraw;           // 0x04
    s16*       pIndices;        // 0x08
    f32*       pPos;            // 0x0C
    u8*        pColour;         // 0x10
    f32*       pUV;             // 0x14
} TrailMeshDesc;

typedef struct Vec4 { f32 x, y, z, w; } Vec4;

// Rows of gForgivenessTable, in pairs (threshold, scale) unless noted.
enum {
    ROW_DRIVING     = 0,    // driving accuracy: clubs 0-8
    ROW_STRIKING_A  = 2,    // ball striking: clubs 9-12
    ROW_STRIKING_B  = 4,    // 13-16
    ROW_STRIKING_C  = 6,    // 17-24
    ROW_DRIVING_PWR = 8,    // distance lost to error, driving
    ROW_RECOVERY    = 10,
    ROW_RECOVERY_PWR = 12,
    ROW_APPROACH_A  = 14,   // shot kind 3
    ROW_APPROACH_B  = 16,   // shot kind 2
    ROW_PUTTING     = 20,
    ROW_BOOST       = 24,   // single: power per boost step
    ROW_RUMBLE      = 25,   // single: rumble frames per unit of error
    ROW_SPIN        = 26    // single: spin scale
};

// A table row interpolated by an attribute: 0..100 between the first two columns, 100..110
// between the last two.
#define TABLE_AT(row, attr)                                                                    \
    ((attr) <= 100                                                                             \
         ? gForgivenessTable[0][row] + ((f32)(attr) / 100.0f) *                                \
               (gForgivenessTable[1][row] - gForgivenessTable[0][row])                         \
         : gForgivenessTable[1][row] + (((f32)(attr) - 100.0f) / 10.0f) *                      \
               (gForgivenessTable[2][row] - gForgivenessTable[1][row]))

// Two rows at once (threshold and scale), one branch on the attribute.
#define TABLE_PAIR(rowT, rowS, attr, outT, outS)                                               \
    if ((attr) <= 100) {                                                                       \
        f32 t = (f32)(attr) / 100.0f;                                                          \
        outT = gForgivenessTable[0][rowT] +                                                    \
               t * (gForgivenessTable[1][rowT] - gForgivenessTable[0][rowT]);                  \
        outS = gForgivenessTable[0][rowS] +                                                    \
               t * (gForgivenessTable[1][rowS] - gForgivenessTable[0][rowS]);                  \
    } else {                                                                                   \
        f32 t = ((f32)(attr) - 100.0f) / 10.0f;                                                \
        outT = gForgivenessTable[1][rowT] +                                                    \
               t * (gForgivenessTable[2][rowT] - gForgivenessTable[1][rowT]);                  \
        outS = gForgivenessTable[1][rowS] +                                                    \
               t * (gForgivenessTable[2][rowS] - gForgivenessTable[1][rowS]);                  \
    }

extern SwingState*   gpSwing;                    // 0x80281188
extern f32           gForgivenessTable[3][27];  // 0x80188168  rows: value at attribute 0 / 100 / 110
extern s32           gBoostSteps[8];             // 0x80188148  power boost per level: 1 2 4 6 9 12 16 20
extern f32           lbl_80281B40[];             // FLT_MAX
extern f32           gPuttXScale[8];             // 0x801882AC  per shot kind: 0.03 for a putt, 0.2 otherwise
extern f32           gSwingXScale[8];            // 0x801882CC  the same values again
extern s32           gClubCurve[CLUB_MAX_e];      // 0x80183578  per club, 0..26: how much it can shape
extern SwingStack    gSwingStacks[];             // 0x801D5A90
extern u8            gInSwingExit;               // 0x80281E00  set while a state's exit callback runs
extern s8            lbl_80281E09;
extern u8            lbl_80281E08;
extern ModeStateDef  lbl_801883C0[];
extern u8            lbl_80281194[4];            // a neutral pad: both sticks centred (0x80)
extern u8 (*gSwingPhaseFns[])(int nPlayer);
extern f32           gSwingRange[8];             // 0x801882EC  backswing rate by shot kind: -, 0.85, 0.5, 0.8
extern u8            lbl_80281E10;
extern u8            lbl_80281E13;
extern u8            lbl_80281E11;
extern u8            lbl_80281E12;
extern Vec4          lbl_80183690;          // 0, 0, 0, 0.5 (assigned, not an initialiser: as one,
                                                //   STATEFUNC_RemoveBallExit drops to 42.5%)
extern u8*           lbl_80281DA0;               // per player, 0x104 bytes each
extern f32           gRealBallRadiusIn;          // 0x80283300  0.84: a real golf ball, in inches
extern Vec4          lbl_80183620;          // 0, 0, 0, 0.5 (assigned)

void  fn_800130F8(int nPad, int n);              // rumble on
void  fn_8006C2C8(int nPlayer, f32* pX, f32* pY);
void  Swing_FaceVector(int nPlayer, f32* pOut);
f32   Swing_MeterError(int nPlayer);
void  Swing_ShapeVector(int nPlayer, f32* pOut);
f32   fn_8005CC84(f32 fTan);                     // atanf
void  fn_8005CCA8(int nPlayer);
void  Vec_Sub(f32* pA, f32* pB, f32* pOut);      // 0x8005CBF4  a - b
void  Vec_Add(f32* pA, f32* pB, f32* pOut);      // 0x8005CBD0  a + b
void  Swing_ResetBoostAndSpin(int nPlayer);
void  fn_8006C5E0(void);
void  Swing_ClearFrameFlag(int nPlayer);
void  fn_800360A0(void* p);
void  Swing_LoadTuning(int nPlayer);
void  fn_80036054(void* p, int a, s32* pDesc);
void  fn_8005A788(int nPlayer, int a);
void  Swing_RumbleOff(int nPlayer);
void  fn_800AE3C4(int nPlayer);
u8*   Pad_State(int nPlayer, int nController);   // the pad's state: [1] main stick y, [3] C-stick y
int   Swing_StickX(int nPlayer, u8* pPad);       // 0x80058F04  main or C-stick by bUsingCStick
int   Swing_StickY(int nPlayer, u8* pPad);       // 0x80058F30
f32   Swing_TopTime(SwingData* pSw);             // 0x80058E98  fTimeSwingTop - 0.0076
f32   Swing_StartTime(SwingData* pSw);           // 0x80058EA8  fTimeSwingStart + 0.0076
int   fn_800204A0(BlendClip* pClip, f32* pOut, f32 fTime);
void  Character_UpdateAnimation(Character* pObj, int a, f32 f);
void  Swing_UpdatePower(int nPlayer);
void  Swing_BoostInput(int nPlayer);
void  fn_8005AD20(Character* pObj, SwingData* pSw, int nStickX);
int   fn_8005CC5C(void);
void  fn_800AE3F8(int nView);
void  fn_80036100(u8* pMesh, void* pDesc, int n);
void  fn_800360D4(u8* pMesh);
void  fn_80008BB8(f32* pOut, f32 x, f32 y, f32 z);
void  fn_80098C70(void);
int   fn_8001707C(int nView);                    // the player a view belongs to
void  GOLFERSTATE_Pop(int nPlayer);
void  fn_80067710(int nPlayer, int a, int b);
s8    GOLFERSTATE_GetPreviousState(int nPlayer);  // the state below the top of the stack
void  fn_8006C608(void);
void  fn_80017158(int nView);
void  fn_80039344(int nView, f32 f);             // a per-view float (0x801D5010[nView])
void  fn_80062B64(int nPlayer);
void  fn_80062B60(int nPlayer);
void  fn_80045824(int nPlayer);
u8    fn_80062B90(void);
u8    fn_80062B88(int nPlayer);
void  fn_80062B84(int a);
u8    fn_80062B7C(void);
void  fn_800A5980(u8 nPlayer);
void  fn_8006C28C(int nPlayer, int nController);
void  fn_80068AA8(int nPlayer);
f32   fn_800D04AC(int nPlayer);
void  fn_800170F4(int nView);
void  fn_8003349C(f32 a, f32 b, f32 c);
void  fn_800D8D10(int nPlayer);
void  fn_800693A4(int nPlayer);
void  fn_80062D98(void);
void  fn_80047EF0(Ball* pBall, int nPlayer, int a);  // tee the ball up
void  fn_800A573C(u8 nPlayer);
void  fn_800A3CB0(f32* pPos, int nPlayer);
void  fn_800A3D6C(f32* pPos, int nPlayer);
void  fn_800A3DF4(int nPlayer);
void  fn_80062DDC(f32* pA, f32* pB, f32* pOut);  // a - b
void  fn_8006BB5C(int nPlayer);
void  fn_800A562C(u8 nPlayer);
u8    fn_800DA264(void);                         // tips are on
void  fn_800D1DAC(int nPlayer);
void  fn_80062B68(int nPlayer);
void  fn_80062B6C(int nPlayer);

// How much spin the SPIN attribute allows: 0.15 at 0, 0.6 at 100, 1.0 at 110.
f32 Swing_SpinScale(int nSpin) {
    return TABLE_AT(ROW_SPIN, (s8)nSpin);
}

// Add the power boost: the pressed level's step times a per-point scale from POWER BOOST
// (0.005 at 0, 0.010 at 100). Base value only - equipment counts, modifiers do not.
f32 Swing_ApplyPowerBoost(int nPlayer, f32 fPower) {
    int nAttr  = Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_POWER_BOOST, ATTR_BASE);
    int nLevel = gPlayers[nPlayer].swing.nPowerBoost;
    s8  nBoost = nAttr;
    if (nLevel > 0) {
        f32 fScale = TABLE_AT(ROW_BOOST, nBoost);
        f32 fAdd   = fScale * (f32)gBoostSteps[nLevel - 1];
        fPower += fAdd;
        return fPower;
    }
    return fPower;
}

// The spin stick's result (a replay reads it back from the recording first).
void fn_8005C15C(int nPlayer, f32* pSpinY, f32* pSpinX) {
    if (gSession.bReplay) {
        fn_8006C2C8(nPlayer, &gPlayers[nPlayer].swing.fForwardSpin, &gPlayers[nPlayer].swing.fSideSpin);
    }
    *pSpinX = gPlayers[nPlayer].swing.fForwardSpin;
    *pSpinY = gPlayers[nPlayer].swing.fSideSpin;
}

// The backswing's sideways angle as a fraction of a quarter turn, mirrored by fn_8001EDF4.
f32 fn_8005C1EC(int nPlayer) {
    if (fn_8001EDF4(gPlayers[nPlayer].pChar)) {
        return gPlayers[nPlayer].swing.fControllerSliceAngle / 1.5707964f;
    }
    return -(gPlayers[nPlayer].swing.fControllerSliceAngle / 1.5707964f);
}

f32 fn_8005C268(int nPlayer) {
    return gPlayers[nPlayer].swing.fMishitAngle;
}

f32 fn_8005C280(int nPlayer) {
    return gPlayers[nPlayer].swing.fNonPowerShotPower;
}

void fn_8005C298(int nPlayer) {
    gPlayers[nPlayer].swing.bCanSpin = 0;
}

// Turn the spin input into the shot's spin: stick deflection (-1..1) times the amount asked
// for (0..20, over 20) times the SPIN scale (0.15 at 0, 0.6 at 100, 1.0 at 110).
void Swing_ApplySpin(int nPlayer) {
    SwingData* pSw = &gPlayers[nPlayer].swing;
    f32        fInv = 1.0f / 128.0f;
    int        nSpin;
    f32        fScale;
    if (gSession.bReplay) return;
    if (pSw->nSpinBoost == 0) {
        pSw->fForwardSpin = 0.0f;
        pSw->fSideSpin = 0.0f;
        return;
    }
    nSpin       = Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_SPIN, ATTR_TOTAL);
    {
        f32 fX = (f32)(pSw->nSpinCtrlX - 128) * fInv;
        f32 fY = (f32)(pSw->nSpinCtrlY - 128) * fInv;
        pSw->fSideSpin = fX * (f32)pSw->nSpinBoost / 20.0f;
        pSw->fForwardSpin = fY * (f32)pSw->nSpinBoost / 20.0f;
    }
    fScale = Swing_SpinScale(nSpin);
    pSw->fSideSpin *= fScale;
    pSw->fForwardSpin *= fScale;
    pSw->fForwardSpin *= -1.0f;
}

// Driver from the tee: up to +10% power when the tempo lands in the sweet-spot window.
f32 Swing_TeeSweetSpot(int nPlayer, f32 fPower) {
    Player* p = &gPlayers[nPlayer];
    f32     fT, fHalf;
    if (p->ball.nLie == 0 && p->nClub == 0 && p->swing.fControllerSliceAngle < 0.0f) {
        fT = -p->swing.fControllerSliceAngle / 1.5707964f;
        if (fT > gpSwing->fKnot1X && fT < gpSwing->fKnot2X) {
            f32 fBonus;
            fHalf  = (gpSwing->fKnot2X - gpSwing->fKnot1X) / 2.0f;
            fBonus = 1.0f - (f32)fabs(fHalf - (fT - gpSwing->fKnot1X)) / fHalf;
            fBonus *= gpSwing->fTeeBonus;
            return fPower + fBonus;
        }
    }
    return fPower;
}

// Shrink a human's swing error by the governing attribute: below a threshold the error is
// multiplied by a scale (at 100, misses under ~0.42 become 82% smaller). CPU players skip this.
// Putts under 2 units lose their error entirely.
void Swing_ApplyForgiveness(int nPlayer) {
    if (Player_IsCPU(nPlayer)) return;
    if (gPlayers[nPlayer].bPerfect) return;
    {
    f32  fError = gPlayers[nPlayer].swing.fMishitAngle;
    int  nRowScale, nRowThresh;
    int  nAttr;
    f32  fThresh, fScale;

    if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
        gPlayers[nPlayer].ball.nLie == 8 || gPlayers[nPlayer].ball.nLie == 3 ||
        gPlayers[nPlayer].ball.nLie == 4) {
        nRowScale  = ROW_RECOVERY + 1;
        nRowThresh = ROW_RECOVERY;
        nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
    } else {
        switch (gPlayers[nPlayer].nShotKind) {
        case SHOT_TYPE_PUTT_e:
            nRowScale  = ROW_PUTTING + 1;
            nRowThresh = ROW_PUTTING;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL);
            if (gPlayers[nPlayer].fDistance < 2.0f) {
                gPlayers[nPlayer].swing.fMishitAngle = 0.0f;
                return;
            }
            break;
        case SHOT_TYPE_CHIP_e:
            nRowScale  = ROW_APPROACH_B + 1;
            nRowThresh = ROW_APPROACH_B;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
            break;
        case SHOT_TYPE_PITCH_e:
            nRowScale  = ROW_APPROACH_A + 1;
            nRowThresh = ROW_APPROACH_A;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
            break;
        case 5:
        case 6:
        case 7:
            nRowScale  = ROW_RECOVERY + 1;
            nRowThresh = ROW_RECOVERY;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
            break;
        default:
            if (gPlayers[nPlayer].nClub >= 0 && gPlayers[nPlayer].nClub < 9) {
                nRowScale  = ROW_DRIVING + 1;
                nRowThresh = ROW_DRIVING;
                nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_DRIVING_ACCURACY, ATTR_TOTAL);
            } else if (gPlayers[nPlayer].nClub >= 9 && gPlayers[nPlayer].nClub < 13) {
                nRowScale  = ROW_STRIKING_A + 1;
                nRowThresh = ROW_STRIKING_A;
                nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
            } else if (gPlayers[nPlayer].nClub >= 13 && gPlayers[nPlayer].nClub < 17) {
                nRowScale  = ROW_STRIKING_B + 1;
                nRowThresh = ROW_STRIKING_B;
                nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
            } else if (gPlayers[nPlayer].nClub >= 17 && gPlayers[nPlayer].nClub < 25) {
                nRowScale  = ROW_STRIKING_C + 1;
                nRowThresh = ROW_STRIKING_C;
                nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
            } else {
                nRowScale  = ROW_STRIKING_C + 1;
                nRowThresh = ROW_STRIKING_C;
                nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
            }
            break;
        }
    }
    TABLE_PAIR(nRowThresh, nRowScale, nAttr, fThresh, fScale);
    if (fabs(fError) < fThresh) {
        fError *= fScale;
    }
    gPlayers[nPlayer].swing.fMishitAngle = fError;
    }
}

// Rumble the pad on a mis-hit: frames = (135 at attribute 0 .. 35 at 100) x |error|, max 30.
void Swing_MisHitRumble(int nPlayer) {
    int nPad = gPlayers[nPlayer].nController;
    int nAttr;
    f32 fScale;
    switch (gPlayers[nPlayer].nShotKind) {
    case SHOT_TYPE_PUTT_e:
        nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL);
        break;
    case SHOT_TYPE_CHIP_e:
    case SHOT_TYPE_PITCH_e:
        nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
        break;
    case 5:
    case 6:
    case 7:
        nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
        break;
    default:
        nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
        break;
    }
    fScale = TABLE_AT(ROW_RUMBLE, nAttr);
    gPlayers[nPlayer].swing.nVibrateCount = (int)(fScale * fabs(gPlayers[nPlayer].swing.fMishitAngle));
    if (gPlayers[nPlayer].swing.nVibrateCount > 30) {
        gPlayers[nPlayer].swing.nVibrateCount = 30;
    }
    if (gPlayers[nPlayer].swing.nVibrateCount > 0) {
        gPlayers[nPlayer].swing.bVibrating = 1;
        fn_800130F8(nPad, 1);
        fn_80013130(nPad, 0xFF);
    }
}

int fn_8005CB48(int nPlayer) {
    return gPlayers[nPlayer].swing.nPowerBoost;
}

int fn_8005CB60(int nPlayer) {
    return gPlayers[nPlayer].swing.nSpinBoost;
}

// An animation event's time.
f32 fn_8005CB78(Character* pChar, u64 uEvent) {
    return pChar->events[(int)uEvent].fTime;
}

void fn_8005CB88(Character* pObj, f32 f) {
    if (pObj != NULL) {
        pObj->f162C = f;
    }
}

f32 Char_GetBackswing(Character* pObj) {
    if (pObj == NULL) {
        return 0.0f;
    }
    return pObj->fBackswing;
}

void fn_8005CBB0(Character* pObj, f32 f) {
    if (pObj != NULL) {
        pObj->f1630 = f;
    }
}

void fn_8005CBC0(Character* pObj, f32 f) {
    if (pObj != NULL) {
        pObj->f1634 = f;
    }
}

// Paired-single vector add over four floats (the fourth is carried along).
#ifdef __MWERKS__
asm void Vec_Add(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Vec_Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif

#ifdef __MWERKS__
asm void Vec_Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Vec_Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// A 4-vector's squared length, capped.
f32 fn_8005CC18(f32* pV) {
    f32 f = pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2] + pV[3] * pV[3];
    if (f > lbl_80281B40[0]) {
        f = lbl_80281B40[0];
    }
    return f;
}

int fn_8005CC5C(void) {
    return lbl_802823FC;
}

void fn_8005CC64(TexBank* pBank, TexEntry* pTex) {
    lbl_801B8980.p100 = pBank;
    lbl_801B8980.p104 = pTex;
    lbl_801B8980.uFlags |= 1;
}

f32 fn_8005CC84(f32 fTan) {
    return atan(fTan);
}

// The final power for the shot. A CPU just scales what it planned (putts +5%). A human gets
// the boost, then loses distance to the swing error: a scaled part of it under the threshold,
// all of it above. Putts over 75% on the meter count as full power.
f32 Swing_ComputePower(int nPlayer) {
    Player* p;
    f32*    pPower;
    f32     fPower, fError;
    int     nRowScale, nRowThresh;
    int     nAttr;
    f32     fThresh, fScale;

    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        p      = &gPlayers[nPlayer];
        fPower = p->fPower * AI_PowerScale(nPlayer);
        if (p->nShotKind == SHOT_TYPE_PUTT_e && !(p->uFlags & 8)) {
            fPower *= 1.05f;
            if (fPower < 0.1f) {
                fPower = 0.1f;
            }
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    p      = &gPlayers[nPlayer];
    fPower = p->fPower;
    pPower = &p->fPower;
    fError = fabs(p->swing.fMishitAngle);
    p->swing.fNonPowerShotPower = Swing_ApplyPowerBoost(nPlayer, fPower) - fError;
    switch (p->nShotKind) {
    case SHOT_TYPE_PUTT_e: {
        f32 fDist = p->fDistance < 1.0f ? 1.0f : p->fDistance;
        if (*pPower > gpSwing->fPuttFullPower) {
            *pPower = 1.0f;
        }
        fPower = *pPower * fn_80050D34(fDist);
        Golfer_GetAttribute(p, ATTR_PUTTING, ATTR_TOTAL);
        if (fPower < 0.1f) {
            fPower = 0.1f;
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    case SHOT_TYPE_CHIP_e:
    case SHOT_TYPE_PITCH_e: {
        f32 f = *pPower;
        if (p->nShotKind == SHOT_TYPE_CHIP_e) {
            f = *pPower * fn_80050F88(p->fDistance, &p->ball, SHOT_TYPE_CHIP_e, p->nClub);
        }
        fPower = Swing_ApplyPowerBoost(nPlayer, f);
        Golfer_GetAttribute(p, ATTR_APPROACH, ATTR_TOTAL);
        if (fPower < 0.1f) {
            fPower = 0.1f;
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    case 5:
    case 6:
    case 7:
        fPower     = Swing_ApplyPowerBoost(nPlayer, *pPower);
        nRowScale  = ROW_RECOVERY_PWR + 1;
        nRowThresh = ROW_RECOVERY_PWR;
        nAttr      = (s8)Golfer_GetAttribute(p, ATTR_RECOVERY, ATTR_TOTAL);
        break;
    default:
        if (p->ball.nLie == 6 || p->ball.nLie == 7 || p->ball.nLie == 8 || p->ball.nLie == 3 ||
            p->ball.nLie == 4) {
            nRowScale  = ROW_RECOVERY_PWR + 1;
            nRowThresh = ROW_RECOVERY_PWR;
            nAttr      = (s8)Golfer_GetAttribute(p, ATTR_RECOVERY, ATTR_TOTAL);
        } else {
            nRowScale  = ROW_DRIVING_PWR + 1;
            nRowThresh = ROW_DRIVING_PWR;
            nAttr      = (s8)Golfer_GetAttribute(p, ATTR_DRIVING_ACCURACY, ATTR_TOTAL);
        }
        fPower = *pPower * AI_PowerScale(nPlayer);
        fPower = Swing_ApplyPowerBoost(nPlayer, fPower);
        fPower = Swing_TeeSweetSpot(nPlayer, fPower);
        break;
    }
    TABLE_PAIR(nRowThresh, nRowScale, nAttr, fThresh, fScale);
    if (fError < fThresh) {
        fPower = fPower - fScale * fError;
    } else {
        fPower = fPower - fError;
    }
clamp:
    if (!(gPlayers[nPlayer].uFlags & 8)) {
        if (fPower > 1.5f) {
            fPower = 1.5f;
        } else if (fPower < 0.05f) {
            fPower = 0.05f;
        }
    }
    return fPower;
}

f32 fn_8005B64C(int nPlayer) {
    return gPlayers[nPlayer].swing.fShotPower;
}

// ---- the hit -----------------------------------------------------------------------------------

// The ball is struck. A replay (mode 10) reseeds the RNG and restores player 0's swing state;
// otherwise the lucky-shot swap runs first. Then the meter's miss (zero for a CPU or a perfect
// shot), the power, forgiveness, the launch blocks, and the aim - the player's aim plus the
// face vector's angle plus the miss - go to Ball_Launch.
void Swing_Launch(int nPlayer) {
    Player* p;
    f32*    pLaunchB;
    f32*    pLaunchA;
    Ball*   pBall;
    int     nClub, nTrajectory, nKind;
    f32     fAim;

    p = &gPlayers[nPlayer];
    if (Game_GetMode() == 10) {
        fn_8000B1D4(0, gReplayData.nSeed);
        // port: 0x630 of the swing data's 0x634 bytes (it holds no pointers)
        Mem_cpy(&gPlayers[0].swing, &gReplayData.player.swing, 0x630);
    } else if (gSession.bReplay == 0) {
        Luck_TakePerfectShot(nPlayer);
        fn_8006BF60(nPlayer);
    } else {
        fn_8006C300(nPlayer);
    }
    pLaunchA    = p->vLaunchA;
    nClub       = p->nClub;
    nTrajectory = p->nTrajectory;
    nKind       = p->nShotKind;
    pBall       = &p->ball;
    Swing_FaceVector(nPlayer, pLaunchA);
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        gPlayers[nPlayer].swing.fMishitAngle = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fMishitAngle = Swing_MeterError(nPlayer);
    }
    gPlayers[nPlayer].swing.fShotPower = Swing_ComputePower(nPlayer);
    Swing_ApplyForgiveness(nPlayer);
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e && gPlayers[nPlayer].fDistance < 2.0f) {
        gPlayers[nPlayer].vLaunchA[0] = 0.0f;
        gPlayers[nPlayer].vLaunchA[1] = 0.0f;
        gPlayers[nPlayer].vLaunchA[2] = 1.0f;
        gPlayers[nPlayer].vLaunchA[3] = 0.0f;
    }
    pLaunchB = p->vLaunchB;
    Swing_ShapeVector(nPlayer, pLaunchB);
    gPlayers[nPlayer].swing.fHookSlice = gPlayers[nPlayer].vLaunchA[0];
    if (Player_IsController8(nPlayer)) {
        fn_8005CCA8(nPlayer);
        nTrajectory = p->nTrajectory;
        nClub       = p->nClub;
        nKind       = p->nShotKind;
    }
    if (0.0f == gPlayers[nPlayer].vLaunchA[2]) {
        fAim = p->fAim + gPlayers[nPlayer].swing.fMishitAngle;
    } else {
        fAim = p->fAim + fn_8005CC84(gPlayers[nPlayer].vLaunchA[0] / gPlayers[nPlayer].vLaunchA[2]) +
               gPlayers[nPlayer].swing.fMishitAngle;
    }
    while (fAim < -PI) {
        fAim += 2 * PI;
    }
    while (fAim > PI) {
        fAim -= 2 * PI;
    }
    Ball_Launch(pBall, nClub, nKind, gPlayers[nPlayer].swing.fShotPower, fAim, nTrajectory, pLaunchA,
                pLaunchB);
}

// ---- the meter's miss ----------------------------------------------------------------------------

// The analog swing's error: the angle between the stick's path back (centre to the top of the
// backswing) and its path through (top to impact). Both x samples get a random +-15 (of a
// +-128 stick) before the x axis is scaled by 0.2 - 0.03 on a putt - so the wobble is worth up
// to about 1.7 degrees on a full shot; then atan of the deviation, clamped to the meter's
// maximum (gpSwing->fMaxError).
f32 Swing_MeterError(int nPlayer) {
    f32 fTopX;
    f32 fTopY;
    f32 fImpactX;
    f32 fCentreX;
    f32 fCentreY;
    f32 fImpactY;
    f32 vBack[4], vThrough[4], vDiff[4], vDir[4];
    f32 fAngle, fMax;

    fTopX    = gPlayers[nPlayer].swing.nBackSwingX;
    fTopY    = gPlayers[nPlayer].swing.nBackSwingY;
    fImpactX = gPlayers[nPlayer].swing.nFollowThroughX;
    fImpactY = gPlayers[nPlayer].swing.nFollowThroughY;
    fCentreX = gPlayers[nPlayer].swing.nCalibrateX;
    fCentreY = gPlayers[nPlayer].swing.nCalibrateY;

    fTopX    += Rand_Float(0) * 30.0f - 15.0f;
    fImpactX += Rand_Float(0) * 30.0f - 15.0f;
    vDir[0] = 0.0f;
    vDir[1] = 0.0f;
    vDir[2] = 1.0f;
    vDir[3] = 0.0f;
    vBack[0] = (fCentreX - fTopX) * gSwingXScale[gPlayers[nPlayer].nShotKind];
    vBack[1] = 0.0f;
    vBack[2] = fTopY - fCentreY;
    vBack[3] = 0.0f;
    vThrough[0] = (fImpactX - fCentreX) * gSwingXScale[gPlayers[nPlayer].nShotKind];
    vThrough[1] = 0.0f;
    vThrough[2] = fCentreY - fImpactY;
    vThrough[3] = 0.0f;
    if (0.0f != vThrough[0] || 0.0f != vThrough[2]) {
        Vec_Normalize(vThrough, vThrough);
    }
    if (0.0f != vBack[0] || 0.0f != vBack[2]) {
        Vec_Normalize(vBack, vBack);
    }
    Vec_Sub(vThrough, vBack, vDiff);
    Vec_Add(vDir, vDiff, vDir);
    if (vDir[2]) {
        fAngle = fn_8005CC84(vDir[0] / vDir[2]);
    } else {
        fAngle = (PI / 2) * (vDir[0] >= 0.0f ? 1.0f : -1.0f);
    }
    fMax = gpSwing->fMaxError;
    if (fAngle < -fMax) {
        fAngle = -fMax;
    } else if (fAngle > fMax) {
        fAngle = fMax;
    }
    return fAngle;
}

// ---- the clubface -------------------------------------------------------------------------------

// How far the face turns for a backswing angled fBackAngle off vertical: the angle as a fraction
// of a quarter turn goes through a three-piece curve (knots at gpSwing 0xB8..0xC4), scaled by
// the club's shaping range (fCurveMin..fCurveMax by gClubCurve/26) and a quarter turn.
f32 Swing_CurveAngle(s32* pClub, f32 fBackAngle) {
    f32 fOut   = 0.0f;
    f32 fT     = fBackAngle / (PI / 2);
    f32 fRange = gpSwing->fCurveMin +
                 ((f32)gClubCurve[*pClub] / 26.0f) * (gpSwing->fCurveMax - gpSwing->fCurveMin);

    fT = (f32)fabs(fT);
    if (fT < gpSwing->fKnot1X) {
        fOut = gpSwing->fKnot1Y * fT / gpSwing->fKnot1X;
    } else {
        fOut += gpSwing->fKnot1Y;
        if (fT < gpSwing->fKnot2X) {
            fOut += (gpSwing->fKnot2Y - gpSwing->fKnot1Y) *
                    ((fT - gpSwing->fKnot1X) / (gpSwing->fKnot2X - gpSwing->fKnot1X));
        } else {
            fOut += gpSwing->fKnot2Y - gpSwing->fKnot1Y;
            fOut += (1.0f - gpSwing->fKnot2Y) * ((fT - gpSwing->fKnot2X) / (1.0f - gpSwing->fKnot2X));
        }
    }
    fOut *= (PI / 2) * fRange;
    if (fBackAngle < 0.0f) {
        return -fOut;
    }
    return fOut;
}

// The first launch block: the human's clubface from the stick. A CPU or a perfect shot gets a
// square face. On a full shot the backswing's sideways angle (kept in fControllerSliceAngle for the tee
// bonus) becomes a face angle through Swing_CurveAngle; on a putt the face is a plain
// proportion of the stick's sideways offset. Session flags 0x4000 + 0x8000 force it square.
void Swing_FaceVector(int nPlayer, f32* pOut) {
    Player* p;
    f32     fCentreX, fTopX, fTopY, fDY;
    f32     fAngle, fSin, fCos, fK;

    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
        return;
    }
    p        = &gPlayers[nPlayer];
    fCentreX = p->swing.nCalibrateX;
    fTopX    = p->swing.nBackSwingX;
    fTopY    = p->swing.nBackSwingY;
    if (0.0f == fCentreX) {
        fCentreX = 1.0f;
    }
    fDY = fTopY - (f32)gPlayers[nPlayer].swing.nCalibrateY;
    if (0.0f == fDY) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    } else if (gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e) {
        fAngle = fn_8005CC84((fTopX - fCentreX) / fDY);
        gPlayers[nPlayer].swing.fControllerSliceAngle = fAngle;
        fAngle = Swing_CurveAngle(&gPlayers[nPlayer].nClub, fAngle);
        fSin   = fn_800095F0(fAngle);
        fCos   = fn_80009638(fAngle);
        pOut[0] = -fSin;
        pOut[1] = 0.0f;
        pOut[2] = fCos;
        pOut[3] = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fControllerSliceAngle = 0.0f;
        if (fTopX > fCentreX) {
            fK = ((fTopX - fCentreX) / (255.0f - fCentreX)) * gPuttXScale[gPlayers[nPlayer].nShotKind];
        } else {
            fK = ((fCentreX - fTopX) / fCentreX) * gPuttXScale[gPlayers[nPlayer].nShotKind];
        }
        pOut[0] = fK * (fCentreX - fTopX);
        pOut[1] = 0.0f;
        pOut[2] = fDY;
        pOut[3] = 0.0f;
    }
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
    Vec_Normalize(pOut, pOut);
}

// The second launch block: a CPU's (or a perfect shot's) shape vector; square for a human.
void Swing_ShapeVector(int nPlayer, f32* pOut) {
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        AI_FaceVector(nPlayer, pOut);
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
}

// ---- the swing state stack ----------------------------------------------------------------------
// Each player has a small stack of swing states (ids into sGolferStateEngineTable, whose entries carry the
// state's callbacks). The top is the current state; -1 is empty.

// The current swing state, or -1.
int GOLFERSTATE_GetCurrentState(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop == -1) return -1;
    return (u8)pStack->nState[pStack->nTop];
}

s8 GOLFERSTATE_GetPreviousState(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop < 1) return -1;
    return pStack->nState[pStack->nTop - 1];
}

u8 fn_8005D2DC(void) {
    return 0;
}

void fn_8005D2E4(void) {
    lbl_80281E09 = -1;
    lbl_80281E08 = 0;
}

void fn_8005D2F8(void) {
    if (lbl_80281E09 > -1 && lbl_801883C0[lbl_80281E09].pfnUpdate != NULL) {
        lbl_801883C0[lbl_80281E09].pfnUpdate();
    }
}

void fn_8005D348(void) {
    if (lbl_80281E09 > -1 && lbl_801883C0[lbl_80281E09].pfnExit != NULL) {
        lbl_801883C0[lbl_80281E09].pfnExit();
    }
    lbl_80281E09 = -1;
    lbl_80281E08 = 0;
}

void fn_8005D3A8(s8 nState) {
    void (*pfn)(void);
    if (lbl_80281E09 > -1 && (pfn = lbl_801883C0[lbl_80281E09].pfnExit) != NULL) {
        lbl_80281E08 = 1;
        pfn();
        lbl_80281E08 = 0;
    }
    lbl_80281E09 = nState;
    pfn = lbl_801883C0[lbl_80281E09].pfnEnter;
    if (pfn != NULL) {
        lbl_80281E08 = 1;
        pfn();
        lbl_80281E08 = 0;
    }
}

// Pop every state, running each one's exit callback.
void GOLFERSTATE_Kill(int nPlayer) {
    s8*         pTop;
    SwingStack* pStack = &gSwingStacks[nPlayer];
    pTop = &pStack->nTop;
    while (*pTop > -1) {
        if (sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
            gInSwingExit = 1;
            sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit(nPlayer);
            gInSwingExit = 0;
        }
        (*pTop)--;
    }
}

// Push a state and run its enter callback.
void GOLFERSTATE_Push(int nState, int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    void (*pfn)(int);
    pStack->nTop++;
    pStack->nState[pStack->nTop] = nState;
    pfn = sGolferStateEngineTable[pStack->nState[pStack->nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// Pop the current state, running its exit callback.
void GOLFERSTATE_Pop(int nPlayer) {
    void (*pfn)(int);
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
    gSwingStacks[nPlayer].nTop--;
}

// Pop everything and start again from one state.
void GOLFERSTATE_Set(int nState, int nPlayer) {
    s8*         pTop;
    SwingStack* pStack = &gSwingStacks[nPlayer];
    void (*pfn)(int);
    pTop = &pStack->nTop;
    while (*pTop > -1) {
        if (sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
            gInSwingExit = 1;
            sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit(nPlayer);
            gInSwingExit = 0;
        }
        (*pTop)--;
    }
    gSwingStacks[nPlayer].nTop = 0;
    gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop] = nState;
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// Replace the current state: its exit, then the new state's enter.
void GOLFERSTATE_Switch(int nState, int nPlayer) {
    void (*pfn)(int);
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnExit;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
    gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop] = nState;
    pfn = sGolferStateEngineTable[gSwingStacks[nPlayer].nState[gSwingStacks[nPlayer].nTop]].pfnEnter;
    if (pfn != NULL) {
        gInSwingExit = 1;
        pfn(nPlayer);
        gInSwingExit = 0;
    }
}

// Empty in release.
void fn_8005CCA8(int nPlayer) {
}

// Empty every player's swing stack.
void GOLFERSTATE_OpenONCE(void) {
    int i = 0;
    while (i < 5) {
        gSwingStacks[i++].nTop = -1;
    }
    gInSwingExit = 0;
}

// Run the current state's update for every player (not while the game is held).
void GOLFERSTATE_Update(void) {
    int i;
    if (fn_800E415C()) return;
    switch (fn_8005D2DC()) {
    case 0:
        for (i = 0; i < 5; i++) {
            SwingStack* pStack = &gSwingStacks[(u32)i];
            void (*pfn)(int);
            if (pStack->nTop > -1) {
                pfn = sGolferStateEngineTable[pStack->nState[pStack->nTop]].pfnUpdate;
                if (pfn != NULL) {
                    pfn(i);
                }
            }
        }
        break;
    }
}

// Pop every player's states.
void fn_8005CD94(void) {
    s8*         pTop;
    SwingStack* pStack;
    int         i;
    for (i = 0; i < 5; i++) {
        pStack = &gSwingStacks[(u32)i];
        pTop   = &pStack->nTop;
        while (*pTop > -1) {
            if (sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
                gInSwingExit = 1;
                sGolferStateEngineTable[(s8)pStack->nState[*pTop]].pfnExit(i);
                gInSwingExit = 0;
            }
            (*pTop)--;
        }
    }
    gInSwingExit = 0;
}

// ---- state 2: thinking ----------------------------------------------------------------------------

#define CPU_TOLERANCE 0.0025f               // 0.05 yd squared: land within 1.8 in of the target

// A human goes straight on to state 10 (setting up). A CPU rehearses its shot here, one frame
// at a time, and moves on once the rehearsal has settled, at least a second has passed and the
// camera has settled - or when its time is up: 4 s, 1.5 s for a tee shot and 3.5 s after in
// modes 6 and 7, 3..4 s in mode 11. Out of time, the rehearsal is told to stop (best aim found,
// or +25 and a fresh target), then the skill error goes on and it is state 10.
void STATEFUNC_ShotSetupUpdate(int nPlayer) {
    View*   pView;
    u8      bDone;
    f32     fMax;
    f32     fMin;

    pView = fn_80017028(gPlayers[nPlayer].nView[0]);
    if (!Player_IsCPU(nPlayer)) {
        GOLFERSTATE_Switch(GS_SWING, nPlayer);
        return;
    }
    if (Player_IsCPU(nPlayer)) {
        gPlayers[nPlayer].fThinkTime += gSession.fFrameTime;
        bDone = AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
        fMin  = 1.0f;
        switch (Game_GetMode()) {
        case 6:
        case 7:
            if (fn_800FA118(0, 0)) return;
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] <= 0) {
                fMax = 1.5f;
            } else {
                fMax = 3.5f;
            }
            break;
        case 11:
            fMin = 3.0f;
            fMax = 4.0f;
            break;
        default:
            fMax = 4.0f;
            break;
        }
        if ((bDone && gPlayers[nPlayer].fThinkTime > fMin && fn_800C7100(pView)) ||
            gPlayers[nPlayer].fThinkTime > fMax) {
            if (!bDone) {
                gPlayers[nPlayer].nRehearseState = 3;
                AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
            }
            AI_ApplyError(nPlayer);
            GOLFERSTATE_Switch(GS_SWING, nPlayer);
        }
    }
}

// ---- starting the swing ---------------------------------------------------------------------------

// The swing is under way: phase 1, the animation started, its three marks read, the 25-sample
// stick history filled with the centre, the spin stick centred.
// The backswing's top mark, a hair early.

// Release the swing's loaded resources: two blocks in the tuning data and three pairs of handles.

// Set the swing module up: player 1's ratings and the fixed tuning into gpSwing, the two resource
// blocks, each player's tuning and swing state, and three pairs of buffers.
void Swing_Init(void) {
    s32 desc[2];
    int i;
    gpSwing->fCC = 40.0f;
    gpSwing->nD0 = 1;
    gpSwing->fPutting      = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_PUTTING, ATTR_BASE);
    gpSwing->fBallStriking = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_BALL_STRIKING, ATTR_BASE);
    gpSwing->fApproach     = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_APPROACH, ATTR_BASE);
    gpSwing->fRecovery     = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_RECOVERY, ATTR_BASE);
    gpSwing->fSpin         = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_SPIN, ATTR_BASE);
    gpSwing->fPower        = (s8)Golfer_GetAttribute(&gPlayers[0], ATTR_POWER, ATTR_BASE);
    gpSwing->fEC = gpSwing->fF0 = gpSwing->fF4 = 0.5f;
    gpSwing->fF8 = 0.05f;
    gpSwing->fFC = 0.5f;
    gpSwing->f104 = 0.00161f;
    gpSwing->f100 = 0.02395f;
    gpSwing->f108 = 0.95f;
    gpSwing->f10C = 0.95f;
    gpSwing->f110 = 0.01f;
    gpSwing->fMaxError = 1.5f;
    gpSwing->fCurveMin = 0.13f;
    gpSwing->fCurveMax = 0.4f;
    gpSwing->fB4 = 0.143f;
    gpSwing->fKnot1Y = 0.22f;
    gpSwing->fKnot1X = 0.4f;
    gpSwing->fKnot2Y = 0.22f;
    gpSwing->fKnot2X = 0.6f;
    gpSwing->fTeeBonus = 0.1f;
    gpSwing->fPuttFullPower = 0.75f;
    desc[0] = 0x1A;
    desc[1] = 1;
    fn_80036054(gpSwing->mesh[0], 0, desc);
    fn_80036054(gpSwing->mesh[1], 0, desc);
    for (i = 0; i < gSession.nNumPlayers; i++) {
        Swing_LoadTuning(i);
        Swing_ResetBoostAndSpin(i);
        gPlayers[i].swing.fBlueColor = 0.5f;
        gPlayers[i].swing.fRedColor = 0.5f;
        gPlayers[i].swing.fGreenColor = 0.5f;
        gPlayers[i].swing.fAlpha = 0.0f;
    }
    for (i = 0; i < 2; i++) {
        gpSwing->p94[i] = fn_80009B34(0x138, 2, 0x10, "Swing.c", 555);
        gpSwing->p9C[i] = fn_80009B34(0x68, 2, 0x10, "Swing.c", 556);
        gpSwing->pA4[i] = fn_80009B34(0xD0, 2, 0x10, "Swing.c", 557);
    }
}

void fn_80058DB4(void) {
    int i;
    fn_800360A0(gpSwing->mesh[0]);
    fn_800360A0(gpSwing->mesh[1]);
    for (i = 0; i < 2; i++) {
        fn_80009E70(gpSwing->p94[i]);
        fn_80009E70(gpSwing->p9C[i]);
        fn_80009E70(gpSwing->pA4[i]);
    }
}

// Clear every player's bDrawBoostUI.
void fn_80058E40(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        fn_8005A788(i, 0);
    }
}

f32 Swing_TopTime(SwingData* pSw) {
    return pSw->fTimeSwingTop - 0.0076f;
}

// The start mark, a hair late.
f32 Swing_StartTime(SwingData* pSw) {
    return 0.0076f + pSw->fTimeSwingStart;
}

// A controller's pad state; the neutral pad when there is none or input is locked.
u8* Pad_State(int nPlayer, int nController) {
    u8* pPad = fn_800136C4(nController);
    if (pPad == NULL || fn_80100C00()) {
        return lbl_80281194;
    }
    return pPad;
}

// The swing stick's X: the C-stick (pad byte 2) when the player swings with it, else the main stick.
int Swing_StickX(int nPlayer, u8* pPad) {
    if (gPlayers[nPlayer].swing.nStickUsed != 0) {
        return pPad[2];
    }
    return pPad[0];
}

int Swing_StickY(int nPlayer, u8* pPad) {
    if (gPlayers[nPlayer].swing.nStickUsed != 0) {
        return pPad[3];
    }
    return pPad[1];
}

// Run the swing's current phase; true once the ball is struck.
u8 fn_80058F5C(int nPlayer) {
    return gSwingPhaseFns[gPlayers[nPlayer].swing.nState](nPlayer);
}

// Reset a player's swing: phase 0, sticks and spin stick centred, animation 5, no rumble.
void fn_80058FA4(int nPlayer) {
    gPlayers[nPlayer].swing.nState = 0;
    gPlayers[nPlayer].swing.nRestCX = gPlayers[nPlayer].swing.nRestCY = 0x80;
    gPlayers[nPlayer].swing.nRestX = gPlayers[nPlayer].swing.nRestY = 0x80;
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    Swing_RumbleOff(nPlayer);
    Swing_ResetBoostAndSpin(nPlayer);
    gPlayers[nPlayer].swing.nSpinCtrlX = 0x80;
    gPlayers[nPlayer].swing.nSpinCtrlY = 0x80;
    gPlayers[nPlayer].swing.fTargetTurnAngle = 0.0f;
    gPlayers[nPlayer].swing.fCurrentTurnAngle = 0.0f;
    gPlayers[nPlayer].swing.f10 = 0.0f;
    gPlayers[nPlayer].swing.f14 = 0.0f;
    fn_800AE3C4(nPlayer);
}

void Swing_Begin(int nPlayer) {
    Player*    p      = &gPlayers[nPlayer];
    Character* pChar  = p->pChar;
    SwingData* pSw    = &p->swing;
    int        i;

    pSw->nState = 1;
    fn_80095744(pChar, 6);
    CharacterState_UpdateSKAState(pChar);
    pSw->f10 = 0.0f;
    pSw->f14 = 0.0f;
    pSw->fTimeSwingStart = fn_8005CB78(pChar, 0);
    pSw->fTimeSwingTop = fn_8005CB78(pChar, 1);
    pSw->fTimeBallHit = fn_8005CB78(pChar, 2);
    for (i = 0; i < 25; i++) {
        pSw->nCtrlListX[i] = pSw->nCalibrateX;
        pSw->nCtrlListY[i] = pSw->nCalibrateY;
    }
    pSw->nCtrlListIndex = 0;
    pSw->fTimeSinceContact       = 0.0f;
    pSw->bSpun       = 0;
    pSw->bSpinning       = 0;
    Swing_ResetBoostAndSpin(nPlayer);
    pSw->nSpinCtrlX = 128;
    pSw->nSpinCtrlY = 128;
    pSw->fTargetTurnAngle        = 0.0f;
    pSw->fCurrentTurnAngle        = 0.0f;
}

// Waiting for the backswing. A CPU (or a replay) starts at once. A human starts the frame
// either stick is pulled past 160 of 255 - more than a quarter of its travel - and that stick's
// rest position becomes the centre sample; the C-stick can swing too (bUsingCStick). While
// nothing is pulled the rest positions are held at 128.
int Swing_WaitForBackswing(int nPlayer) {
    Player* p          = &gPlayers[nPlayer];
    int     nController = p->nController;
    Character* pChar   = p->pChar;
    u8*     pPad;
    u8      bMain, bCStick;

    if (Controller_IsCPU(nController) || Game_GetMode() == 10) {
        gPlayers[nPlayer].swing.nState = 1;
        Swing_Begin(nPlayer);
        pChar->uFlags &= ~1;
        Swing_ClearFrameFlag(nPlayer);
        return 0;
    }
    pPad    = Pad_State(nPlayer, nController);
    bMain   = pPad[1] <= 0xFF && pPad[1] > 0xA0;
    bCStick = pPad[3] <= 0xFF && pPad[3] > 0xA0;
    if (bMain || bCStick) {
        if (bCStick) {
            gPlayers[nPlayer].swing.nCalibrateY      = gPlayers[nPlayer].swing.nRestCY;
            gPlayers[nPlayer].swing.nCalibrateX      = gPlayers[nPlayer].swing.nRestCX;
            gPlayers[nPlayer].swing.nStickUsed  = 1;
        } else {
            gPlayers[nPlayer].swing.nCalibrateY      = gPlayers[nPlayer].swing.nRestY;
            gPlayers[nPlayer].swing.nCalibrateX      = gPlayers[nPlayer].swing.nRestX;
            gPlayers[nPlayer].swing.nStickUsed  = 0;
        }
        EVENT_Trigger(nPlayer, 0x2C, 0, 0);
        Swing_Begin(nPlayer);
        Swing_ClearFrameFlag(nPlayer);
        fn_8006C5E0();
    } else {
        gPlayers[nPlayer].swing.nRestCY = 128;
        gPlayers[nPlayer].swing.nRestCX = 128;
        gPlayers[nPlayer].swing.nRestY  = 128;
        gPlayers[nPlayer].swing.nRestX  = 128;
    }
    return 0;
}

// ---- the backswing --------------------------------------------------------------------------------

// Freeze the backswing at the top: the animation stops (rate 0.008) where it is.
void Swing_HoldAtTop(int nPlayer) {
    Player*    p    = &gPlayers[nPlayer];
    SwingData* pSw  = &p->swing;
    Character*   pObj = p->pChar;
    pSw->fFidgetTargetTime = pSw->fFidgetPauseTime;
    Anim_SetTime(pObj->anim, pSw->fFidgetTargetTime);
    Character_UpdateAnimation(pObj, 0, 0.0f);
    Anim_SetRate(pObj->anim, 0.008f);
    pObj->uFlags |= 0x40;
    pSw->fFidgetTimeElapsed = 0.0f;
    pSw->fFidgetWaitToIdle = 0.0f;
}

// One stick axis through the dead zone: 96..160 reads as centre (128); forward of it runs
// smoothly down to 1, back of it jumps to ~179 and runs to 255.
static inline int Swing_DeadZone(int v) {
    if (v > 160) return 127 + (v - 96) * 128 / 159;
    if (v < 96) return 128 - (96 - v) * 127 / 96;
    return 128;
}

// Phase 1, the backswing. A CPU (or a replay) plays the backswing animation to 98% of the way
// to the top (65% in lesson 5) and then swings down. A human's backswing follows the stick:
// the further back it is pulled (dead-zoned magnitude, capped at 100), the further along the
// backswing the animation is asked to be; the animation chases that at a rate that grows with
// the gap. While the stick is still back, every frame's sample goes into the 25-sample ring
// and is provisionally the top. The moment the stick comes forward of 96 (once the backswing
// is at least 0.1 along) the top is the furthest-back sample in the ring, the downswing
// animation starts, and it is phase 3 with the impact sample seeded from this frame.
int Swing_UpdateBackswing(int nPlayer) {
    Player*    p;
    Character*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;
    f32        fAnimTime, fTop, fStart, fMag, fTarget, fDelta, fRate, fRange;

    p           = &gPlayers[nPlayer];
    pObj        = p->pChar;
    pSw         = &p->swing;
    nController = p->nController;
    if (pObj->pModel->pSkel != NULL) {
        pObj->pModel->pSkel->n10E4 = 4;
    }
    if (pObj->pBlend != NULL) {
        pObj->fBackswing = pObj->pBlend->fCC;
    } else {
        pObj->fBackswing = 0.0f;
    }
    fAnimTime = pObj->fAnimTime;
    if (Controller_IsCPU(nController) || Game_GetMode() == 10) {
        f32 fFrac;
        AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        if (fn_80100AF8()) {
            fFrac = 0.65f;
        } else {
            fFrac = 0.98f;
        }
        if (fAnimTime >= pSw->fTimeSwingStart + fFrac * (pSw->fTimeSwingTop - pSw->fTimeSwingStart)) {
            Anim_SetRate(pObj->anim, 1.0f);
            fn_80095744(pObj, 7);
            if (fn_800204A0(pObj->pBlend->pD8, pObj->v1638,
                            pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
                pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pD4->f24 - pObj->v1638[1]) + pObj->f1644;
            }
            CharacterState_UpdateSKAState(pObj);
            pObj->n5CC = 2;
            pSw->fTimeSwingTop = pObj->fAnimTime;
            fn_8005CBC0(pObj, 0.0f);
            fn_8005CBB0(pObj, 0.0f);
            fn_8005CB88(pObj, 1.4f * Char_GetBackswing(pObj));
            pSw->nState = 3;
        }
        return 0;
    }
    pPad   = Pad_State(nPlayer, nController);
    nX     = Swing_StickX(nPlayer, pPad);
    nY     = Swing_StickY(nPlayer, pPad);
    fTop   = Swing_TopTime(pSw);
    fStart = Swing_StartTime(pSw);
    if ((nY <= 255 && nY > 96) || (Char_GetBackswing(pObj) < 0.1f && nY < 96)) {
        if (nY < 96) {
            fMag = 0.0f;
        } else {
            int nDX = Swing_DeadZone(nX) - 128;
            int nDY = Swing_DeadZone(nY) - 128;
            fMag = (f32)fn_80009680(nDX * nDX + nDY * nDY);
        }
        if (fMag > 100.0f) {
            fMag = 100.0f;
        }
        fRange  = fTop - fStart;
        fTarget = fStart + (fMag / 100.0f) * fRange;
        fDelta  = fTarget - fAnimTime;
        fRate   = 1.0f + (f32)fabs(fDelta) / fRange;
        fRate   = fRate * fRate - 1.0f;
        if (fRate >= 1.0f) {
            fRate = 1.0f;
        }
        if (gPlayers[nPlayer].nShotKind == 1 || gPlayers[nPlayer].nShotKind == 2 ||
            gPlayers[nPlayer].nShotKind == 3) {
            fRange = fRange / gSwingRange[gPlayers[nPlayer].nShotKind];
        } else {
            fRange = 1.0f;
        }
        Anim_SetRate(pObj->anim, fRate * fRange);
        if (fMag > 93.0f) {
            Anim_SetRate(pObj->anim, fRange);
        }
        if (fDelta > -0.05f && fDelta < 0.05f) {
            pSw->fFidgetPauseTime = pObj->fAnimTime;
            pSw->nFidgetPauseStickY = nY;
            Swing_HoldAtTop(nPlayer);
            pSw->nState = 2;
        } else if (pObj->uFlags & 0x40) {
            if (fDelta > 0.0f) {
                pObj->uFlags &= ~0x40;
            }
        } else if (fDelta < 0.0f) {
            pObj->uFlags |= 0x40;
            pSw->fPowerBoostDieTime = 1.0f / 12.0f;
        }
        pSw->nCtrlListX[pSw->nCtrlListIndex] = nX;
        pSw->nCtrlListY[pSw->nCtrlListIndex] = nY;
        pSw->nCtrlListIndex++;
        pSw->nCtrlListIndex %= 25;
        Swing_UpdatePower(nPlayer);
        pSw->nBackSwingX = nX;
        pSw->nBackSwingY = nY;
        Swing_BoostInput(nPlayer);
    } else if (nY <= 96) {
        // The stick has come forward: the top of the backswing is the furthest-back sample.
        int i;
        pSw->nBackSwingY = pSw->nCalibrateY;
        for (i = 0; i < 25; i++) {
            if (pSw->nCtrlListY[i] > pSw->nBackSwingY) {
                pSw->nBackSwingX = pSw->nCtrlListX[i];
                pSw->nBackSwingY = pSw->nCtrlListY[i];
            }
        }
        Swing_UpdatePower(nPlayer);
        Anim_SetRate(pObj->anim, 1.0f);
        fn_80095744(pObj, 7);
        if (fn_800204A0(pObj->pBlend->pD8, pObj->v1638,
                        pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
            pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pD4->f24 - pObj->v1638[1]) + pObj->f1644;
        }
        CharacterState_UpdateSKAState(pObj);
        pObj->n5CC = 2;
        pSw->fTimeSwingTop = pObj->fAnimTime;
        fn_8005CBC0(pObj, 0.0f);
        fn_8005CBB0(pObj, 0.0f);
        fn_8005CB88(pObj, 1.4f * Char_GetBackswing(pObj));
        pSw->nState = 3;
        EVENT_Trigger(nPlayer, 0x2F, 0, 0);
        pSw->nFollowThroughX = nX;
        pSw->nFollowThroughY = nY;
        pSw->nMishitX = nX;
        pSw->nMishitY = nY;
        Swing_ClearFrameFlag(nPlayer);
    }
    return 0;
}

// ---- at the top, and the downswing ----------------------------------------------------------------

// Phase 2, holding at the top. Any stick movement drops back to phase 1 (backing down if it
// came forward). While it is steady the animation waggles +-0.0076 around the top. If the
// stick sits near centre (y at or below 160, x within 64..192) for over 0.1 s the swing is
// abandoned: phase 0, the address animation, sound 9.
int Swing_UpdateAtTop(int nPlayer) {
    Player*    p;
    Character*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;
    f32        fTop, fStart, fAnimTime;

    p           = &gPlayers[nPlayer];
    pObj        = p->pChar;
    pSw         = &p->swing;
    nController = p->nController;
    if (pObj->pModel->pSkel != NULL) {
        pObj->pModel->pSkel->n10E4 = 4;
    }
    pPad   = Pad_State(nPlayer, nController);
    nX     = Swing_StickX(nPlayer, pPad);
    nY     = Swing_StickY(nPlayer, pPad);
    fTop   = Swing_TopTime(pSw);
    fStart = Swing_StartTime(pSw);
    fAnimTime = pObj->fAnimTime;
    pSw->fFidgetTimeElapsed += gSession.fFrameTime;
    if (pSw->nFidgetPauseStickY != nY) {
        if (nY < gPlayers[nPlayer].swing.nFidgetPauseStickY) {
            pObj->uFlags |= 0x40;
        } else {
            pObj->uFlags &= ~0x40;
        }
        pSw->fPowerBoostDieTime = 1.0f / 12.0f;
        pSw->nState    = 1;
    } else if (((pObj->uFlags & 0x40) && fAnimTime < pSw->fFidgetTargetTime) || fAnimTime < fStart) {
        pSw->fFidgetTargetTime = 0.0076f + pSw->fFidgetPauseTime;
        pObj->uFlags &= ~0x40;
    } else if ((!(pObj->uFlags & 0x40) && fAnimTime > pSw->fFidgetTargetTime) || fAnimTime > fTop) {
        pSw->fFidgetTargetTime = pSw->fFidgetPauseTime - 0.0076f;
        pObj->uFlags |= 0x40;
    } else if (pSw->nFidgetPauseStickY <= 160 && nX <= 192 && nX >= 64) {
        pSw->fFidgetWaitToIdle += gSession.fFrameTime;
        if (pSw->fFidgetWaitToIdle > 0.1f) {
            pSw->nState = 0;
            fn_80095744(pObj, 5);
            Anim_SetRate(pObj->anim, 1.0f);
            EVENT_Trigger(nPlayer, 9, 0, 0);
        }
    }
    Swing_UpdatePower(nPlayer);
    return 0;
}

// Phase 3, the downswing. Every frame the stick is forward of 96 and more than ~17 units from
// the centre sample, that reading becomes the impact sample - so what counts is where the
// stick was pointing on the way through, not when. The ball goes when the animation reports
// impact (n5CC < 0): phase 5, Swing_Launch, the mis-hit rumble, the putt sound.
int Swing_UpdateDownswing(int nPlayer) {
    Player*    p;
    Character*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;

    p           = &gPlayers[nPlayer];
    pSw         = &p->swing;
    pObj        = p->pChar;
    nController = p->nController;
    if (pObj->pModel->pSkel != NULL) {
        pObj->pModel->pSkel->n10E4 = 4;
    }
    if (!Controller_IsCPU(nController) && Game_GetMode() != 10) {
        int nDX, nDY;
        pPad = Pad_State(nPlayer, nController);
        nX   = Swing_StickX(nPlayer, pPad);
        nY   = Swing_StickY(nPlayer, pPad);
        nDX  = nX - pSw->nCalibrateX;
        nDY  = nY - pSw->nCalibrateY;
        if (nY <= 96 && (f32)(nDX * nDX + nDY * nDY) > 300.0f) {
            pSw->nFollowThroughX  = nX;
            pSw->nFollowThroughY  = nY;
            pSw->nMishitX = nX;
            pSw->nMishitY = nY;
        }
    }
    if (pObj->n5CC < 0) {
        fn_8002792C(pObj->pModel->pSkel);
        gPlayers[nPlayer].swing.nState = 5;
        Swing_Launch(nPlayer);
        if (!Controller_IsCPU(nController) && gSession.bReplay == 0) {
            gPlayers[nPlayer].swing.bCanSpin = 1;
        }
        if (Controller_IsPad(nController) && gSession.bReplay == 0) {
            Swing_MisHitRumble(nPlayer);
        }
        if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            EVENT_Trigger(nPlayer, 0x2B, 0, 0);
        }
        return 1;
    }
    return 0;
}

// ---- after impact ---------------------------------------------------------------------------------

int Swing_PhaseIdle4(int nPlayer) {
    return 0;
}

int Swing_PhaseIdle6(int nPlayer) {
    return 0;
}

// Stop the pad rumble.
void Swing_RumbleOff(int nPlayer) {
    Player* p;
    s32*    pFrames;
    s32*    pController;
    if (Player_HasPad(nPlayer)) {
        p           = &gPlayers[nPlayer];
        pFrames     = &p->swing.nVibrateCount;
        pController = &p->nController;
        fn_800130F8(p->nController, 0);
        fn_80013130(*pController, 0);
        gPlayers[nPlayer].swing.bVibrating = 0;
        *pFrames = 0;
    }
}

// Count the mis-hit rumble down and stop it when it runs out.
void Swing_RumbleTick(int nPlayer) {
    if (Player_HasPad(nPlayer)) {
        if (gPlayers[nPlayer].swing.bVibrating) {
            if (gPlayers[nPlayer].swing.nVibrateCount <= 0) {
                Swing_RumbleOff(nPlayer);
            } else {
                gPlayers[nPlayer].swing.nVibrateCount--;
            }
        }
    }
}

// Spin, added after the ball is away: with the spin option on and the spin button (mask
// 0x20) held after a real shot, the amount grows by one a frame up to 20 - a third of a second
// for full spin - and the direction is the stick, whenever it is outside the 96..160 dead zone
// (the first press starts it at straight back, 255).
void Swing_SpinInput(int nPlayer) {
    u32     uButtons;
    int     nX, nY;
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.options.bSpinEnabled == 0) return;
    uButtons    = fn_800136DC(gPlayers[nPlayer].nController);
    if (!(uButtons & fn_800142AC(0x20, 0))) return;
    if (gPlayers[nPlayer].swing.bCanSpin == 0) return;
    EVENT_Trigger(nPlayer, 0x2E, 0, 0);
    nX = Swing_StickX(nPlayer, Pad_State(nPlayer, gPlayers[nPlayer].nController));
    nY = Swing_StickY(nPlayer, Pad_State(nPlayer, gPlayers[nPlayer].nController));
    if (gPlayers[nPlayer].swing.nSpinBoost == 0) {
        gPlayers[nPlayer].swing.nSpinCtrlX = 128;
        gPlayers[nPlayer].swing.nSpinCtrlY = 255;
    }
    if (nX < 96 || nX > 160 || nY < 96 || nY > 160) {
        gPlayers[nPlayer].swing.nSpinCtrlX = nX;
        gPlayers[nPlayer].swing.nSpinCtrlY = nY;
    }
    if (gPlayers[nPlayer].swing.nSpinBoost < 20) {
        (gPlayers[nPlayer].swing.nSpinBoost)++;
    }
}

// Phase 5, the ball is away: the rumble counts down and spin can be added.
int Swing_UpdateAfterImpact(int nPlayer) {
    int nController = gPlayers[nPlayer].nController;
    u8* pPad;
    if (Player_IsCPU(nPlayer) || gSession.bReplay != 0) {
        return 0;
    }
    pPad = Pad_State(nPlayer, nController);
    Swing_StickX(nPlayer, pPad);
    Swing_StickY(nPlayer, pPad);
    Swing_RumbleTick(nPlayer);
    Swing_SpinInput(nPlayer);
    Swing_ApplySpin(nPlayer);
    return 0;
}

void Swing_ClearFrameFlag(int nPlayer) {
    gPlayers[nPlayer].swing.nNumInBlurQueue = 0;
}

// Record the club for its trail: the head (bone 0x53) and grip (0x52) go on the front of the
// 25-entry history. When the head has moved more than 0.3 since the last entry, five in-between
// entries are added instead, each blended from the last entry to now with the shaft re-extended
// to the club's current length.
void fn_8005A0FC(int nPlayer) {
    f32        vB8[4];
    f32        vA8[4];
    f32        v98[4];
    f32        v88[4];
    f32        v78[4];
    f32        v68[4];
    f32        v58[4];
    f32        v48[4];
    f32        v38[4];
    f32        v28[4];
    f32        v18[4];
    f32        v8[4];
    Character*   pObj = gPlayers[nPlayer].pChar;
    SwingData* pSw  = &gPlayers[nPlayer].swing;
    int        nHead = fn_8001EED8(pObj->pModel, 0x53);
    int        nGrip = fn_8001EED8(pObj->pModel, 0x52);
    f32        f;                  // the head's move, then the blend step
    f32        fLen;
    int        k;
    int        i;

    Vec_Copy(pObj->pModel->pMatrices[nHead][3], v98);
    Vec_Sub(v98, pSw->prevClub[0].vClubPos, vB8);
    f = fn_80009680(fn_8005CC18(vB8));
    Vec_Copy(v98, vA8);
    if (f > 0.3f && pSw->nNumInBlurQueue != 0) {
        v18[3] = 0.0f;
        v28[3] = 0.0f;
        Vec_Copy(pObj->pModel->pMatrices[nHead][3], v88);
        Vec_Copy(pObj->pModel->pMatrices[nGrip][3], v68);
        Vec_Copy(pSw->prevClub[0].vClubPos, v78);
        Vec_Copy(pSw->prevClub[0].vHandPos, v58);
        Vec_Sub(v68, v58, v48);
        Vec_Sub(v88, v78, v38);
        Vec_Sub(v88, v68, v8);
        fLen = fn_80009680(fn_8005CC18(v8));
        for (i = 1; i <= 5; i++) {
            f = (f32)i / 5.0f;
            fn_8001EF34(v38, f, v18);
            fn_8001EF34(v48, f, v28);
            Vec_Add(v18, v78, v18);
            Vec_Add(v28, v58, v28);
            Vec_Sub(v18, v28, v8);
            if (v8[0] != 0.0f || v8[1] != 0.0f || v8[2] != 0.0f || v8[3] != 0.0f) {
                v8[3] = 0.0f;
                Vec_Normalize(v8, v8);
            }
            fn_8001EF34(v8, fLen, v8);
            Vec_Add(v8, v28, v8);
            for (k = 24; k > 0; k--) {
                Mem_cpy(&pSw->prevClub[k], &pSw->prevClub[k - 1], sizeof(pSw->prevClub[k]));
            }
            Vec_Copy(v8, pSw->prevClub[0].vClubPos);
            pSw->prevClub[0].vClubPos[3] = 1.0f;
            Vec_Copy(v68, pSw->prevClub[0].vHandPos);
            if (pSw->nNumInBlurQueue < 25) {
                pSw->nNumInBlurQueue++;
            }
        }
    } else {
        for (k = 24; k > 0; k--) {
            Mem_cpy(&pSw->prevClub[k], &pSw->prevClub[k - 1], sizeof(pSw->prevClub[k]));
        }
        Vec_Copy(vA8, pSw->prevClub[0].vClubPos);
        pSw->prevClub[0].vClubPos[3] = 1.0f;
        Vec_Copy(pObj->pModel->pMatrices[nGrip][3], pSw->prevClub[0].vHandPos);
        if (pSw->nNumInBlurQueue < 25) {
            pSw->nNumInBlurQueue++;
        }
    }
}

// Pose the golfer from the stick each frame of the backswing (animation 6) and downswing (7): the
// blend weights at 0x484..0x490 and the twist. A human's stick is read; a CPU only does this in
// mode 11 (when fn_8005CC5C() is 8 or 9), with the stick hard to one side.
void fn_8005A478(int nPlayer) {
    Character*   pObj  = gPlayers[nPlayer].pChar;
    int        nBone = fn_8001EED8(pObj->pModel, 0x53);
    SwingData* pSw;
    int        nStickX;
    u8*        pPad;
    f32        vPos[3];
    f32        fT;

    if (Player_IsCPU(nPlayer)) {
        if (Game_GetMode() != 11) return;
        if (fn_8005CC5C() != 8) {
            switch (fn_8005CC5C()) {
            case 9:
                break;
            default:
                return;
            }
        }
    }
    {
        if ((pObj->nAnim == 6 || pObj->nAnim == 7) && gPlayers[nPlayer].nShotKind != 0 && pObj->n1698 == 0) {
            if (fn_8001EE90(pObj) == 2) return;
            if (pObj->nAnim == 6 || pObj->nAnim == 7) {
                pSw = &gPlayers[nPlayer].swing;
                if (Game_GetMode() == 11 && Player_IsCPU(nPlayer)) {
                    if (fn_8005CC5C() == 8) {
                        nStickX = 0;
                    } else {
                        nStickX = 0xFF;
                    }
                } else {
                    pPad    = Pad_State(nPlayer, gPlayers[nPlayer].nController);
                    nStickX = Swing_StickX(nPlayer, pPad);
                    Swing_StickY(nPlayer, pPad);
                }
                Vec_Copy(pObj->pModel->pMatrices[nBone][3], vPos);
                if (pObj->nAnim == 6) {
                    if (nStickX < pSw->nCalibrateX) {
                        pSw->fRedColor = 0.0f;
                        pSw->fGreenColor = 0.0f;
                        pSw->fBlueColor = 0.5f;
                        pSw->fAlpha =
                            gpSwing->fFC * (f32)(pSw->nCalibrateX - nStickX) / (f32)pSw->nCalibrateX;
                    } else {
                        pSw->fRedColor = 0.5f;
                        pSw->fGreenColor = 0.5f;
                        pSw->fBlueColor = 0.0f;
                        pSw->fAlpha = gpSwing->fFC * (f32)(nStickX - pSw->nCalibrateX) /
                                      (f32)(0xFF - pSw->nCalibrateX);
                    }
                    fn_8005AD20(pObj, pSw, nStickX);
                } else if (pObj->nAnim == 7) {
                    pSw->fBlueColor = gpSwing->fF4;
                    pSw->fGreenColor = gpSwing->fF0;
                    pSw->fRedColor = gpSwing->fEC;
                    fT = pObj->fAnimTime - pSw->fTimeBallHit;
                    if (fT >= 0.0f && fT <= 1.0f) {
                        pSw->fAlpha = gpSwing->fF8 * (1.0f - fT);
                    } else if (fT < 0.0f) {
                        pSw->fAlpha = gpSwing->fF8;
                    } else {
                        pSw->fAlpha = 0.0f;
                    }
                    fn_8005AD20(pObj, pSw, pSw->nBackSwingX);
                }
            }
        }
    }
}

void fn_8005A788(int nPlayer, int a) {
    gPlayers[nPlayer].swing.bDrawBoostUI = a;
}

void fn_8005A7A0(int nPlayer) {
    if ((gPlayers[nPlayer].swing.nPowerBoost > 0 || gPlayers[nPlayer].swing.nSpinBoost > 0) &&
        gPlayers[nPlayer].nShotKind != 0 && gPlayers[nPlayer].swing.bDrawBoostUI != 0 &&
        gSession.bReplay == 0 &&
        gSession.nPaused == 0 && !fn_800C6CB0()) {
        fn_800AE3F8(gPlayers[nPlayer].nView[0]);
    }
}

// Draw the club's trail (the swing-trail option): a ribbon from the grip through the recorded
// club-head positions, coloured by the swing's blend weights and fading along its length,
// textured "clubback" on the backswing and "clubdown" on the downswing.
void fn_8005A850(int nPlayer) {
    s16           idx[26];
    TrailMeshDesc mesh;
    f32           vGrip[4];
    TrailDraw     draw;
    Character*      pObj;
    u8            nBlue;
    s8            nRed;
    u8            nGreen;
    SwingData*    pSw;
    int           nView;
    int           nGrip;
    int           i;

    pSw   = &gPlayers[nPlayer].swing;
    pObj  = gPlayers[nPlayer].pChar;
    nView = gPlayers[nPlayer].nView[0];
    nGrip = fn_8001EED8(pObj->pModel, 0x52);
    if (Player_IsCPU(nPlayer)) {
        if (Game_GetMode() != 11) return;
        if (fn_8005CC5C() != 8) {
            switch (fn_8005CC5C()) {
            case 9:
                break;
            default:
                return;
            }
        }
    }
    {
        if ((pObj->nAnim == 6 || pObj->nAnim == 7) && fn_8001EE90(pObj) != 2 && pSw->nNumInBlurQueue >= 2 &&
            gSession.options.a24[7] != 0) {
            Vec_Copy(pObj->pModel->pMatrices[nGrip][3], vGrip);
            nBlue = 255.0f * pSw->fBlueColor;
            nRed = 255.0f * pSw->fRedColor;
            nGreen = 255.0f * pSw->fGreenColor;
            Vec3Copy(vGrip, gpSwing->p94[nView]);
            gpSwing->p94[nView][0] = vGrip[0];
            gpSwing->p94[nView][1] = vGrip[1];
            gpSwing->p94[nView][2] = vGrip[2];
            gpSwing->pA4[nView][0] = 1.0f;
            gpSwing->pA4[nView][1] = 1.0f;
            gpSwing->p9C[nView][0] = 0x80;
            gpSwing->p9C[nView][1] = 0x80;
            gpSwing->p9C[nView][2] = 0x80;
            gpSwing->p9C[nView][3] = 128.0f * pSw->fAlpha * (1.0f / pSw->nNumInBlurQueue);
            idx[0] = 0;
            for (i = 0; i < pSw->nNumInBlurQueue; i++) {
                gpSwing->p94[nView][i * 3 + 3] = pSw->prevClub[i].vClubPos[0];
                gpSwing->p94[nView][i * 3 + 4] = pSw->prevClub[i].vClubPos[1];
                gpSwing->p94[nView][i * 3 + 5] = pSw->prevClub[i].vClubPos[2];
                gpSwing->pA4[nView][i * 2 + 2] = 0.0f;
                gpSwing->pA4[nView][i * 2 + 3] = 1.0f - 0.2f * i / pSw->nNumInBlurQueue;
                gpSwing->p9C[nView][i * 4 + 4] = nRed;    // vertex colour R
                gpSwing->p9C[nView][i * 4 + 5] = nGreen;  // G
                gpSwing->p9C[nView][i * 4 + 6] = nBlue;   // B
                gpSwing->p9C[nView][i * 4 + 7] =
                    128.0f * pSw->fAlpha * (1.0f - (f32)i / pSw->nNumInBlurQueue);
                idx[i + 1] = i + 1;
            }
            fn_80035138(0);
            fn_80016B9C();
            fn_80013EEC(fn_8001614C());
            fn_80014118(0x50);
            fn_80035118(4, 5);
            fn_80012F50(0, 6, 0x80);
            fn_80012F34(0);
            if (pObj->nAnim == 6) {
                fn_8005CC64(gpSwing->pBank, gpSwing->pClubBack);
            } else if (pObj->nAnim == 7) {
                fn_8005CC64(gpSwing->pBank, gpSwing->pClubDown);
            }
            fn_80012EF8();
            draw.nPrims   = 1;
            draw.nFirst   = 0;
            draw.nCount   = pSw->nNumInBlurQueue + 1;
            mesh.n0       = 1;
            mesh.nVerts   = pSw->nNumInBlurQueue + 1;
            mesh.pDraw    = &draw;
            mesh.pIndices = idx;
            mesh.pPos     = gpSwing->p94[nView];
            mesh.pColour  = gpSwing->p9C[nView];
            mesh.pUV      = gpSwing->pA4[nView];
            fn_80036100(gpSwing->mesh[nView], &mesh, 1);
            fn_800360D4(gpSwing->mesh[nView]);
            fn_80012F50(1, 6, 0x80);
            fn_80012F34(1);
            fn_80012EF8();
        }
    }
}

// Twist the golfer with the stick: how far through the backswing (animation 6, eased in) or the
// downswing (7, eased out) the animation is, times a smoothed copy of the stick's X, becomes a
// rotation on the skeleton (mirrored for a left-hander).
void fn_8005AD20(Character* pObj, SwingData* pSw, int nStickX) {
    f32 fAmount = 0.0f;
    f32 fDelta;
    f32 fRate;
    f32 vRot[3];
    fn_8001EEE4(pObj->pModel, 0x24);
    fn_8001EEE4(pObj->pModel, 0x11);
    fn_8001EED8(pObj->pModel, 0x52);
    if (pObj->nAnim == 6) {
        fAmount = (pObj->fAnimTime - pSw->fTimeSwingStart) / (pSw->fTimeSwingTop - pSw->fTimeSwingStart);
        fAmount *= fAmount;
    } else if (pObj->nAnim == 7) {
        fAmount = (pObj->fAnimTime - pSw->fTimeSwingTop) / (pSw->fTimeBallHit - pSw->fTimeSwingTop);
        fAmount *= fAmount;
        fAmount = 1.0f - fAmount;
        if (fAmount < 0.0f) {
            fAmount = 0.0f;
        }
        if (fAmount > 1.0f) {
            fAmount = 1.0f;
        }
    }
    pSw->f14 = (f32)nStickX / 255.0f - 0.5f;
    fDelta = pSw->f14 - pSw->f10;
    fRate = fabsf(fDelta);
    fRate = (fRate < 0.5f) ? 0.5f : ((fRate > 1.0f) ? 1.0f : fRate);
    pSw->f10 = 0.33333334f * (fDelta * fRate) + pSw->f10;
    fAmount = 0.75f * fAmount * pSw->f10;
    if (fn_8001EDF4(pObj)) {
        fAmount = -fAmount;
    }
    fn_80008BB8(vRot, 0.0f, 0.0f, fAmount);
    fn_80027808(pObj->pModel, vRot);
}

// Find the swing's textures by name (all three are in one bank).
void Swing_LoadTuning(int nPlayer) {
    u64 uHash;
    uHash = fn_8000BEE4("clubback");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pClubBack);
    uHash = fn_8000BEE4("clubdown");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pClubDown);
    uHash = fn_8000BEE4("tball");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pTBall);
}

// ---- the power meter -----------------------------------------------------------------------------

// Every frame of the backswing: power is the square root of how far along the backswing is
// (a CPU takes it straight), snapping to 1 within 3% of the top. Holding at the top of a full
// backswing on anything but a putt costs (hold - 0.05)^2, at most 0.3.
void Swing_UpdatePower(int nPlayer) {
    f32  fPower = gPlayers[nPlayer].pChar->fBackswing;
    f32  fPenalty;
    if (!Player_IsCPU(nPlayer)) {
        fPower = (f32)fn_80009680(fPower);
    }
    if (1.0f - fPower < 0.03f) {
        fPower = 1.0f;
    }
    if (gPlayers[nPlayer].swing.fFidgetTimeElapsed < 0.05f ||
        gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e || 1.0f != fPower) {
        fPenalty = 0.0f;
    } else {
        fPenalty = gPlayers[nPlayer].swing.fFidgetTimeElapsed - 0.05f;
        fPenalty = -(fPenalty * fPenalty);
    }
    if (fPenalty < -0.3f) {
        fPenalty = -0.3f;
    }
    gPlayers[nPlayer].fPower = fPower + fPenalty;
    if (gPlayers[nPlayer].fPower < 0.0f) {
        gPlayers[nPlayer].fPower = 0.0f;
    }
}

// ---- the power boost input --------------------------------------------------------------------

// Every backswing frame for a human with the boost option on: while a boost button (mask 0x1F)
// is held with the stick pulled past 93 of its range, the boost level rises one a frame to 8.
// Once the stick has backed down for 1/12 s (fPowerBoostDieTime, set by the backswing) the level and the
// spin offsets are cleared.
void Swing_BoostInput(int nPlayer) {
    u32  uButtons;
    int  nX, nY;
    f32  fMag;

    if (Player_IsCPU(nPlayer)) return;
    if (gSession.options.bBoostEnabled == 0) return;
    uButtons    = fn_800136DC(gPlayers[nPlayer].nController);
    nY   = Swing_StickY(nPlayer, Pad_State(nPlayer, gPlayers[nPlayer].nController));
    nX   = Swing_StickX(nPlayer, Pad_State(nPlayer, gPlayers[nPlayer].nController));
    fMag = (f32)fn_80009680((nX - 128) * (nX - 128) + (nY - 128) * (nY - 128));
    if ((uButtons & fn_800142AC(0x1F, 0)) && fMag > 93.0f) {
        if (gPlayers[nPlayer].swing.nPowerBoost < 8) {
            (gPlayers[nPlayer].swing.nPowerBoost)++;
            EVENT_Trigger(nPlayer, 0x2D, 0, 0);
        }
    }
    if (gPlayers[nPlayer].swing.fPowerBoostDieTime > 0.0f) {
        gPlayers[nPlayer].swing.fPowerBoostDieTime -= gSession.fFrameTime;
        if (gPlayers[nPlayer].swing.fPowerBoostDieTime <= 0.0f) {
            gPlayers[nPlayer].swing.nPowerBoost = 0;
            gPlayers[nPlayer].swing.fTargetTurnAngle = 0.0f;
            gPlayers[nPlayer].swing.fCurrentTurnAngle = 0.0f;
            fn_800AE3C4(nPlayer);
        }
    }
}

// At the start of a swing: no boost, no spin, no back-down timer.
void Swing_ResetBoostAndSpin(int nPlayer) {
    gPlayers[nPlayer].swing.nPowerBoost = 0;
    gPlayers[nPlayer].swing.nSpinBoost = 0;
    gPlayers[nPlayer].swing.fPowerBoostDieTime   = 0.0f;
    fn_8005A788(nPlayer, 1);
    fn_800AE3C4(nPlayer);
}

// ---- swing states: the small ones -----------------------------------------------------------------
// sGolferStateEngineTable is a table of 27 (enter, update, exit) callbacks; the current state is the top of
// the player's SwingStack. Most of these drive the camera, HUD and sounds around the swing.

void STATEFUNC_FadeToTapInExit(int nPlayer) {
}

void STATEFUNC_TapInExit(int nPlayer) {
}

void STATEFUNC_FadeToRemoveBallExit(int nPlayer) {
}

void STATEFUNC_ConcededExit(int nPlayer) {
}

void STATEFUNC_ConcededUpdate(int nPlayer) {
    GM_DoPostShotInHoleUI(nPlayer);
}

void STATEFUNC_PlaceBallExit(int nPlayer) {
    fn_800E3C0C(1);
}

void fn_80062B60(int nPlayer) {
}

void fn_80062B64(int nPlayer) {
}

void fn_80062B68(int nPlayer) {
}

void fn_80062B6C(int nPlayer) {
}

void fn_80062B70(void) {
}

void fn_80062B74(int nPlayer) {
}

void fn_80062B78(int nPlayer) {
}

u8 fn_80062B7C(void) {
    return 1;
}

void fn_80062B84(int a) {
}

u8 fn_80062B88(int nPlayer) {
    return 1;
}

u8 fn_80062B90(void) {
    return 0;
}

// Clear an animation event's "reached" flag.
int fn_80062B98(Character* pChar, u64 uEvent) {
    pChar->events[(int)uEvent].bSet = 0;
    return 0;
}

// An animation event has been reached: it is set and the animation time is past it.
int fn_80062BB0(Character* pChar, u64 uEvent) {
    if (pChar->events[(int)uEvent].bSet != 0 && pChar->fAnimTime >= pChar->events[(int)uEvent].fTime) {
        return 1;
    }
    return 0;
}

void fn_80062BE8(Character* pChar) {
    if (pChar != NULL) {
        pChar->p1794 = NULL;
    }
}

void fn_80062BFC(Character* pChar) {
    if (pChar != NULL) {
        pChar->p1790 = NULL;
    }
}

int fn_80062C10(Character* pChar) {
    return pChar->uFlags & 4;
}

int fn_80062C1C(Character* pChar) {
    return pChar->uFlags & 1;
}

// How far the animation still has to run.
f32 fn_80062C28(Character* pChar) {
    return pChar->fAnimEnd - pChar->fAnimTime;
}

void fn_80062C38(void) {
    fn_800E58B4(7);
}

void fn_80062C5C(void) {
    fn_800E58B4(8);
}

void fn_80062C80(int a, u8 b) {
    fn_80062D38(0x1E, a, b);
}

void fn_80062CB0(int a, u8 b) {
    fn_80062D38(0x14, a, b);
}

void fn_80062CE0(u8 a) {
    fn_80062D6C(0x23, a);
}

void fn_80062D0C(int nPlayer) {
    fn_80062D6C(9, nPlayer + 1);
}

void fn_80062D38(int nMsg, int nA, int nB) {
    fn_800E5998(nMsg, 0, &nA, &nB);
}

void fn_80062D6C(int nMsg, int nValue) {
    fn_800E590C(nMsg, 0, &nValue);
}

void fn_80062D98(void) {
    fn_80098C70();
}

void fn_80062DB8(View* pView, int a) {
    pView->bFade = a;
}

void fn_80062DC0(View* pView) {
    pView->fFade = 0.0f;
}

f32 fn_80062DCC(View* pView) {
    return pView->fFade;
}

u8 fn_80062DD4(View* pView) {
    return pView->bFade;
}

// a - b over three floats (paired singles; the third is a single).
#ifdef __MWERKS__
asm void fn_80062DDC(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80062DDC(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

void fn_80062E00(void) {
    fn_800BD894();
}

void fn_80062E20(void) {
    fn_800BDA04();
}

void STATEFUNC_SwingExit(int nPlayer) {
    if (gPlayers[nPlayer].swing.unk630 != 0) {
        gPlayers[nPlayer].swing.unk630 = 0;
    }
}

void STATEFUNC_WaitUpdate(int nPlayer) {
    fn_80063C90(fn_80017028(gPlayers[nPlayer].nView[0]));
}

void STATEFUNC_ShowYardageExit(int nPlayer) {
    if (gPlayers[nPlayer].bC2E == 0 && lbl_80281E10 == 0) {
        GM_BumpBallForObstructions(nPlayer);
    }
    lbl_80281E10 = 0;
}

void STATEFUNC_GreenReversePuttInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 6, nPlayer, nView);
}

void STATEFUNC_KneeCamInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 7, nPlayer, nView);
    Emotion_UpdatePlayerEmotion(nPlayer);
}

void STATEFUNC_GreenInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

void STATEFUNC_GreenMorphInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

void STATEFUNC_FadeToRemoveBallInit(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    fn_80063BF4(fn_80017028(gPlayers[nPlayer].nView[0]), 0.5f, (f32*)&vOffset);
}

void STATEFUNC_ShotSetupExit(int nPlayer) {
    if (gSession.nGameType != 8 && gPlayers[nPlayer].ball.nLie != LIE_INCUP_e && !Player_IsCPU(nPlayer)) {
        fn_800E3D38(nPlayer, 1);
    }
}

void STATEFUNC_WaitInit(int nPlayer) {
    if (fn_8001707C(gPlayers[nPlayer].nView[0]) == nPlayer) {
        int nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0x19, nPlayer, nView);
    }
    Emotion_UpdatePlayerEmotion(nPlayer);
}

void STATEFUNC_GreenReversePuttUpdate(int nPlayer) {
    if (!Player_IsCPU(nPlayer) && !(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(5, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        fn_800E3D38(nPlayer, 0);
    }
}

// Keep a copy of the ball as it lies before the shot.
void STATEFUNC_ShowYardageInit(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView[0]);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E13 = 1;
}

void STATEFUNC_ReplaySwingExit(int nPlayer) {
    fn_80045494(0, nPlayer);
    GameEffects_SetSuperSlowMo(0, nPlayer, 0.0f);
    fn_800C6E14();
    if (gSession.bReplay != 0) {
        EVENT_Trigger(nPlayer, 0x3B, 0, 0);
    } else {
        EVENT_Trigger(nPlayer, 0x3B, 0, 1);
    }
}

// Neither button 2 nor button 3 held: keep polling.
void STATEFUNC_GreenUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(2, 1))) {
        if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(3, 1))) {
            GOLFERSTATE_Pop(nPlayer);
        }
    }
}

void STATEFUNC_FadeToRemoveBallUpdate(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
        GOLFERSTATE_Switch(GS_REMOVE_BALL, nPlayer);
    }
}

void STATEFUNC_KneeCamExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
}

void STATEFUNC_SimulateExit(int nPlayer) {
    View* pViewObj = fn_80017028(gPlayers[nPlayer].nView[0]);
    fn_80045558(0, nPlayer);
    fn_80045494(0, nPlayer);
    fn_8006C608();
    fn_80062CE0(0);
    Swing_RumbleOff(nPlayer);
    fn_800C1790(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    fn_80062DB8(pViewObj, 0);
}

void STATEFUNC_ZoomInit(int nPlayer) {
    int nView;
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 2, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 1, nPlayer, nView);
    }
    fn_80062D38(0x62, 1, nPlayer);
    lbl_80281E11 = 1;
}

// Leaving the aiming states 4, 5 and 7: back to camera 12 if state 10 is underneath, else 0.
void STATEFUNC_ElevatorExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_GreenExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_GreenReversePuttExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_InTheHoleInit(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView[0]);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E12 = 1;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        fn_8006AAB4(nPlayer, 2);
    }
}

// Leaving the swing: the ball goes back to where it lay; on cameras 1, 3 and 4 a quarter-second
// camera move.
void STATEFUNC_PreShotExit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View* pView   = fn_80017028(gPlayers[nPlayer].nView[0]);
    int   nCamera;
    Mem_cpy(&gPlayers[nPlayer].ball, &gPlayers[nPlayer].ballBefore, sizeof(Ball));
    nCamera = pView->nCamera;
    if (nCamera == 1 || nCamera == 3 || nCamera == 4) {
        fn_80063B98(pView, 0.25f, (f32*)&vOffset);
    }
}

void STATEFUNC_RemoveBallExit(int nPlayer) {
    View* pV;
    Vec4  vOffset;
    pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    vOffset = lbl_80183690;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    if (pV->nCamera == 3) {
        fn_80063B98(fn_80017028(gPlayers[nPlayer].nView[0]), 0.75f, (f32*)&vOffset);
    }
}

void STATEFUNC_ElevatorInit(int nPlayer) {
    int nView;
    if (lbl_80281F78->n1C0 != 0) {
        if (fn_800C4650(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer)) {
            nView = gPlayers[nPlayer].nView[0];
            View_SetCamera(fn_80017028(nView), 0x13, nPlayer, nView);
            return;
        }
    }
    nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 3, nPlayer, nView);
}

void STATEFUNC_ZoomExit(int nPlayer) {
    int nView;
    EVENT_Trigger(nPlayer, 0x31, 0, -1);
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E5714(0x10);
    fn_800E5724(nPlayer);
}

// Leaving state 20: every set-up player's view takes this view's camera.
void STATEFUNC_InitialFlyByExit(int nPlayer) {
    int   i;
    View* pSrc;
    View* pDst;
    fn_80017158(gPlayers[nPlayer].nView[0]);
    pSrc = fn_80017028(gPlayers[nPlayer].nView[0]);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        pDst = fn_80017028(gPlayers[i].nView[0]);
        fn_800170C4(gPlayers[i].nView[0], 1);
        pDst->nCamera = pSrc->nCamera;
        pDst->f118    = pSrc->f118;
        Vec_Copy(pSrc->vC4, pDst->vC4);
        pDst->f114    = pSrc->f114;
    }
    gpGame->pfn24C(nPlayer);
}

void STATEFUNC_ConcededInit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View* pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    fn_800C7178(pV, nPlayer);
    View_SetCamera(pV, 0x19, nPlayer, gPlayers[nPlayer].nView[0]);
    if (pV->nCamera == 1 || pV->nCamera == 3 || pV->nCamera == 4) {
        fn_80063B98(pV, 0.25f, (f32*)&vOffset);
    }
}

void STATEFUNC_GreenWatchRollExit(int nPlayer) {
    int nView;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nPlayer = -1;     // nobody's ball
    gPlayers[nPlayer].ballBefore.nState = 1;      // stopped
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_GreenMorphExit(int nPlayer) {
    int  nView;
    fn_80017028(gPlayers[nPlayer].nView[0])->f50 = 1.0f;
    fn_80017028(gPlayers[nPlayer].nView[0])->f54 = 1.0f;
    fn_80017028(gPlayers[nPlayer].nView[0])->f58 = 1.0f;
    fn_80039344(gPlayers[nPlayer].nView[0], 0.0f);
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

// Each of the player's two views goes back to its saved camera, unless an earlier view of the
// player's is the same view.
void STATEFUNC_MidHoleFlyByExit(int nPlayer) {
    View* pV;
    int   nView;
    int   k, j;
    u8    bShared;
    for (k = 0; k < 2; k++) {
        pV      = fn_80017028(gPlayers[nPlayer].nView[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == fn_80017028(gPlayers[nPlayer].nView[j])) {
                bShared = 1;
            }
        }
        if (!bShared) {
            nView = gPlayers[nPlayer].nView[k];
            View_SetCamera(fn_80017028(nView), pV->nSavedCamera, nPlayer, nView);
        }
    }
    fn_800E3D38(nPlayer, 1);
    fn_800DC9D4(0);
}

// State 15 (fade to tap-in), entered only when a gimme is allowed: plan the tap-in from where the
// ball lies. The ball position is saved, the player's ball is moved to the lie, Shot_Plan runs,
// and the ball position is put back.
void STATEFUNC_FadeToTapInInit(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    f32  vSaved[4];
    fn_80062B64(nPlayer);
    fn_80062B60(nPlayer);
    fn_80063BF4(fn_80017028(gPlayers[nPlayer].nView[0]), 0.75f, (f32*)&vOffset);
    Vec3Copy(gPlayers[nPlayer].vBall, vSaved);
    Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
    Shot_Plan(nPlayer, 0);
    Vec3Copy(vSaved, gPlayers[nPlayer].vBall);
    gPlayers[nPlayer].bPlanReady = 0;
    gPlayers[nPlayer].bRehearsalDone = 0;
}

// State 18: holed out. The ball goes to the pin, the lie becomes LIE_INCUP_e, animation 12.
void STATEFUNC_RemoveBallInit(int nPlayer) {
    Ball* pBall       = &gPlayers[nPlayer].ball;
    Ball* pBallBefore = &gPlayers[nPlayer].ballBefore;
    int  nPinSet;
    CourseInfo* pCourse;
    Mem_cpy(pBallBefore, pBall, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        Character_SetPosition(gPlayers[nPlayer].pChar, &pCourse->pin[nPinSet].x, 1);
        Physics_DropBall(pBall, &pCourse->pin[nPinSet].x);
        gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
        Vec3Copy(pBall->vPos, pBallBefore->vPos);
    }
    fn_80095744(gPlayers[nPlayer].pChar, 12);
}

// State 21: each of the player's views saves its camera and takes camera 10.
void STATEFUNC_MidHoleFlyByInit(int nPlayer) {
    View* pV;
    int   nView;
    int   k, j;
    u8    bShared;
    EVENT_Trigger(nPlayer, 0x4A, 0, -1);
    for (k = 0; k < 2; k++) {
        pV      = fn_80017028(gPlayers[nPlayer].nView[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == fn_80017028(gPlayers[nPlayer].nView[j])) {
                bShared = 1;
            }
        }
        if (!bShared) {
            pV->nSavedCamera = pV->nCurCamera;
            nView = gPlayers[nPlayer].nView[k];
            View_SetCamera(fn_80017028(nView), 10, nPlayer, nView);
        }
    }
    fn_800C7140(0);
    fn_800E3D38(nPlayer, 0);
    fn_80045824(nPlayer);
    fn_800DC9D4(1);
}

void STATEFUNC_TapInInit(int nPlayer) {
    GM_MovePlayerToBall(nPlayer);
    fn_80095744(gPlayers[nPlayer].pChar, 11);
    fn_80062BFC(gPlayers[nPlayer].pChar);
    fn_80062BE8(gPlayers[nPlayer].pChar);
    fn_8001C774(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
    fn_8001C724(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    fn_8001C804(nPlayer, 1, 1);
    gPlayers[nPlayer].uFlags |= 8;
    gPlayers[nPlayer].fA64 = fn_800D0478(nPlayer);
    fn_800E4204();
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
}

// State 6: the putt preview. The caddie's solved shot is taken, the player is made a CPU for
// one call so Swing_Launch fires it clean (no error, no luck swap), and the launched ball is
// kept in ballBefore as nobody's ball - the ghost that draws the preview. Everything the
// player had (shot block, ball, controller) is put back afterwards.
void STATEFUNC_GreenWatchRollInit(int nPlayer) {
    u8    shotSaved[0x5C];
    Ball  ballSaved;
    u8*   pShot;
    Ball* pBall;
    int   nController;
    int   nView;

    nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 5, nPlayer, nView);
    pShot = (u8*)&gPlayers[nPlayer].nClub;
    Mem_cpy(shotSaved, pShot, 0x5C);   // port: the shot block, nClub..unk3AD (no pointers)
    Caddie_ApplyTip(nPlayer);
    pBall = &gPlayers[nPlayer].ball;
    Mem_cpy(&ballSaved, pBall, sizeof(Ball));
    nController  = gPlayers[nPlayer].nController;
    gPlayers[nPlayer].nController = CONTROLLER_CPU;
    Swing_Launch(nPlayer);
    gPlayers[nPlayer].nController = nController;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nPlayer = -1;
    Mem_cpy(pBall, &ballSaved, sizeof(Ball));
    Mem_cpy(pShot, shotSaved, 0x5C);   // port: as above
    fn_8006BF60(nPlayer);
    fn_800E3D38(nPlayer, 0);
}

// State 21 (a camera flyover): over when the option skips cameras, the camera finishes, or a
// button is pressed (any pad for a CPU's shot). Lessons wait for the camera.
void STATEFUNC_MidHoleFlyByUpdate(int nPlayer) {
    u8  bDone = 0;
    u32 uMask;
    if (gSession.options.bSkipCameras) {
        bDone = 1;
    } else if (fn_80100294()) {
        return;
    }
    if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        bDone = 1;
    } else if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            bDone = 1;
            fn_800A76E4();
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            bDone = 1;
            fn_800A76E4();
        }
    }
    if (bDone) {
        GOLFERSTATE_Pop(nPlayer);
    }
}

// State 20 (another camera state), the same idea with a confirm step.
void STATEFUNC_InitialFlyByUpdate(int nPlayer) {
    u8 bDone = 0;
    if (gSession.a8[0] == 0 || !fn_80014300(0)) {
        if (gSession.options.bSkipCameras) {
            bDone = 1;
        } else if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView[0]))) {
            bDone = 1;
        } else if (!fn_80100294()) {
            if (fn_80014300(fn_800142AC(0, 0)) || fn_80062B90()) {
                if (fn_80062B88(nPlayer)) {
                    bDone = 1;
                    fn_80062B84(0);
                    fn_800A76E4();
                } else if (fn_80062B7C()) {
                    fn_80062B84(0);
                }
            }
        }
    }
    if (bDone) {
        GOLFERSTATE_Pop(nPlayer);
    }
}

// State 12: the ball is away. In a replay with the kept ball unset, a special path; otherwise
// the ball as it lies is kept. Camera 14 unless the swing animation is 11 or the view says no.
void STATEFUNC_SimulateInit(int nPlayer) {
    View* pV   = fn_80017028(gPlayers[nPlayer].nView[0]);
    Ball* pBallBefore = &gPlayers[nPlayer].ballBefore;
    if (gSession.bReplay != 0 && pBallBefore->nState == 0) {
        fn_8006B2C4(nPlayer, 1);
    } else {
        Mem_cpy(pBallBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    }
    pBallBefore->nPlayer = -1;
    if (fn_80095780(gPlayers[nPlayer].pChar) != 11 && fn_80101738() && !fn_800C6CB0()) {
        int nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xE, nPlayer, nView);
    }
    fn_8006ACF8(nPlayer, 0);
    fn_800DB714(nPlayer);
    gPlayers[nPlayer].bRehearsalDone = 0;
    Swing_RumbleTick(nPlayer);
    fn_80062DC0(pV);
    fn_80062DB8(pV, 0);
}

// State 15 (fade to tap-in): while the camera moves, the CPU's rehearsal runs on the player -
// human or not, made a CPU for the call, in fast mode - until it solves the tap-in. When the
// camera finishes: solved -> camera move and state 16 (the tap-in is played for the player);
// not yet -> the turn ends (GM_EndOfGolferTurn) unless it had been solved before.
void STATEFUNC_FadeToTapInUpdate(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    f32   vSaved[4];

    fn_80050D2C(1);
    if (gPlayers[nPlayer].bRehearsalDone == 0) {
        int  nController = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        Vec3Copy(gPlayers[nPlayer].vBall, vSaved);
        Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
        gPlayers[nPlayer].bRehearsalDone = AI_RehearseShot(nPlayer, NULL, 1, CPU_TOLERANCE);
        Vec3Copy(vSaved, gPlayers[nPlayer].vBall);
        gPlayers[nPlayer].nController = nController;
    }
    if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        if (gPlayers[nPlayer].bRehearsalDone != 0) {
            fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
            GOLFERSTATE_Switch(GS_TAP_IN, nPlayer);
            gPlayers[nPlayer].bPlanReady = 1;
        } else {
            if (gPlayers[nPlayer].bPlanReady == 0) {
                GM_EndOfGolferTurn(nPlayer);
            }
            fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
        }
    }
    fn_80050D2C(0);
}

// State 16: a shot the game plays for the player. With animation 11 running and camera 12
// set, the launch is made with the controller set to the CPU for the call (so no meter, no
// error, no luck swap), then it is state 12 with the ball away.
void STATEFUNC_TapInUpdate(int nPlayer) {
    int        nController;
    Vec4       vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View*      pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    Character* pChar   = gPlayers[nPlayer].pChar;

    if (fn_80095780(pChar) != 11) return;
    if (pV->nCurCamera != 0xC) {
        View_SetCamera(pV, 0xC, nPlayer, gPlayers[nPlayer].nView[0]);
        fn_80063B98(pV, 0.75f, (f32*)&vOffset);
    }
    if (pV->nCamera == 1) return;
    if (pV->nCamera == 4) return;
    if (pV->nCamera == 3) return;
    if (fn_80048574(pChar, 2)) {
        if (!fn_80062BB0(pChar, 2)) return;
        fn_80062B98(pChar, 2);
        nController  = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        Swing_Launch(nPlayer);
        gPlayers[nPlayer].nController = nController;
        fn_800A5980((u8)nPlayer);
        fn_8006C28C(nPlayer, nController);
        GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
    } else {
        nController  = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        Swing_Launch(nPlayer);
        gPlayers[nPlayer].nController = nController;
        fn_800A5980((u8)nPlayer);
        fn_8006C28C(nPlayer, nController);
        GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
    }
}

// State 8: a free camera while button 19 is held (release pops the state). Buttons 11/12 and
// 13/14 play the four pan sounds; button 4 switches between two camera modes.
void STATEFUNC_KneeCamUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x13, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1)) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1)) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1)) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1)) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(4, 1)) {
        fn_800C6010(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    } else {
        fn_800C60E8(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    }
    fn_80068AC8(nPlayer);
}

// State 4: an aiming camera held while button 7 is down (the caddie keeps updating). Buttons
// 9/10 and 11..14 play the pan sounds.
void STATEFUNC_ElevatorUpdate(int nPlayer) {
    Caddie_Update(nPlayer);
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(7, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(9, 0)) {
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xA, 0)) {
                EVENT_Trigger(nPlayer, 0xE, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1)) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1)) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1)) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1)) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    fn_80068AC8(nPlayer);
}

// State 2, a shot begins. A CPU takes camera 0 (with gpGame+0x290) or 12 and, with that flag,
// animation 2. The ball is kept as it lies and marked stopped, the live ball's state cleared,
// the player's distance to the pin stored, and the "shot begins" sound played.
void STATEFUNC_ShotSetupInit(int nPlayer) {
    int   nView;
    Ball* pBall;
    if (Player_IsCPU(nPlayer) && gpGame->n290 != 0) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    } else if (Player_IsCPU(nPlayer)) {
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    }
    if (gpGame->n290 != 0) {
        fn_80095744(gPlayers[nPlayer].pChar, 2);
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
    }
    fn_80062C38();
    pBall = &gPlayers[nPlayer].ball;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nState = 1;
    gPlayers[nPlayer].ball.nState       = 0;
    GameEffects_ResetGameEffectSettings();
    fn_80058FA4(nPlayer);
    gPlayers[nPlayer].swing.unk630 = 1;
    fn_80068AA8(nPlayer);
    if (gpGame->b276 != 0) {
        fn_800689D4(nPlayer);
    }
    if (gPlayers[nPlayer].pChar->n2C == 4 ||
        gPlayers[nPlayer].pChar->n2C == 5) {
        fn_800957FC(gPlayers[nPlayer].pChar, 1);
        fn_800957B0(gPlayers[nPlayer].pChar, 1);
    }
    fn_80054A6C(pBall);
    gPlayers[nPlayer].fA64 = fn_800D04AC(nPlayer);
    EVENT_Trigger(nPlayer, 6, 0, -1);
}

// State 20: the walk to the tee. Camera 10 on this player's view, every other player's views
// detached, this player attached to both of its views, Shot_Plan with the HUD told, animation 1.
void STATEFUNC_InitialFlyByInit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    int   i, k;

    EVENT_Trigger(nPlayer, 0x4A, 0, -1);
    fn_800170C4(gPlayers[nPlayer].nView[0], 1);
    View_SetCamera(fn_80017028(gPlayers[nPlayer].nView[0]), 10, nPlayer, gPlayers[nPlayer].nView[0]);
    fn_80063B98(fn_80017028(gPlayers[nPlayer].nView[0]), 0.5f, (f32*)&vOffset);
    fn_800C7140(0);
    fn_800170F4(gPlayers[nPlayer].nView[0]);
    fn_800171D8(fn_80012EF0(fn_80017004(gPlayers[nPlayer].nView[0])), 0.0f, 0.0f, 1.0f, 1.0f);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        for (k = 0; k < 2; k++) {
            if (gPlayers[i].nView[k] != gPlayers[nPlayer].nView[0]) {
                fn_800170C4(gPlayers[i].nView[k], 0);
            }
        }
    }
    for (k = 0; k < 2; k++) {
        fn_8001704C(gPlayers[nPlayer].nView[k], nPlayer);
    }
    fn_80045824(nPlayer);
    GM_MovePlayerToBall(nPlayer);
    Shot_Plan(nPlayer, 1);
    fn_8001D8DC(nPlayer);
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    fn_80095744(gPlayers[nPlayer].pChar, 1);
    fn_8003349C(1.0f, 12.0f, 0.1f);
    GameEffects_ResetGameEffectSettings();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_80016CFC(gPlayers[i].nView[0])->bFlagOut = 0;
    }
}

// State 22: arriving at the ball for a new shot. The ball is put on the ground: the upper of the
// two ground heights under it, unless there is none or it is more than 0.25 above the ball; then
// the lower one (or the ball's own height if that is missing too). Then Shot_Plan with the HUD
// told, the think timer cleared, camera 8, boost and spin reset.
void STATEFUNC_PlaceBallInit(int nPlayer) {
    f32         fTmp[4];
    f32         fLow, fHigh;
    int         i;
    CourseInfo* pCourse;

    gSession.bReplay = 0;
    Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        f32 fY;
        Ter_GetEnclosingGroundHeight(pCourse, gPlayers[nPlayer].vBall, &fLow, &fHigh);
        fY = fHigh;
        if (-65536.1f == fHigh || fHigh > 0.25f + gPlayers[nPlayer].vBall[1]) {
            fY = fLow;
            if (-65536.1f == fLow) {
                fY = gPlayers[nPlayer].vBall[1];
            }
        }
        gPlayers[nPlayer].vBall[1] = fY;
    }
    Shot_Plan(nPlayer, 1);
    gPlayers[nPlayer].fThinkTime = 0.0f;
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    GameEffects_ResetGameEffectSettings();
    fn_800D8D10(nPlayer);
    fn_800693A4(nPlayer);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (gPlayers[i].bPlayerCut == 0 && gPlayers[i].nView[0] == gPlayers[nPlayer].nView[0] &&
            gPlayers[i].ball.nLie != 10 && gPlayers[i].ball.nLie != LIE_GREEN_e &&
            gPlayers[i].ball.nLie != LIE_INCUP_e) {
            fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
        }
    }
    fn_80045824(nPlayer);
    View_SetCamera(fn_80017028(gPlayers[nPlayer].nView[0]), 8, nPlayer, gPlayers[nPlayer].nView[0]);
    Vec_Copy(gPlayers[nPlayer].vBall, fTmp);
    fn_80069330(nPlayer, fTmp);
    fn_8006A6C4(nPlayer);
    Swing_ResetBoostAndSpin(nPlayer);
}

// State 9: the putt-line view. Pops when neither button 46 nor 48 is held and the view's fade
// is complete. For a human outside a replay, before the swing starts, the caddie updates and
// button 6 re-plans: the game's 0x264 callback (with gpGame+0x284) or a putt aims at the pin,
// anything else re-chooses a target; then Shot_Prepare, the break line, animation 5.
void STATEFUNC_GreenMorphUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2E, 1))) {
        if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x30, 1))) {
            if (1.0f == fn_80017028(gPlayers[nPlayer].nView[0])->f54) {
                GOLFERSTATE_Pop(nPlayer);
                return;
            }
        }
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bReplay != 0) return;
    if (gPlayers[nPlayer].swing.nState != 0) return;
    Caddie_Update(nPlayer);
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(6, 0)) {
        if (gpGame->b284 != 0) {
            if (gpGame->pfn264(nPlayer)) {
                AI_DefaultTarget(nPlayer);
            } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
                AI_DefaultTarget(nPlayer);
            } else {
                AI_ChooseTarget(nPlayer);
            }
            Shot_Prepare(nPlayer, 1);
            BreakLine_Start(gPlayers[nPlayer].nView[0]);
            fn_8001C804(nPlayer, 1, 1);
            fn_800957D8(gPlayers[nPlayer].pChar);
            fn_80095744(gPlayers[nPlayer].pChar, 5);
            fn_80062C38();
            if (gSession.nSplitScreen != 0) {
                fn_80062CB0(gPlayers[nPlayer].nC58, 1);
            }
        }
        gpGame->pfn22C(nPlayer);
    } else {
        GM_CheckForShotChanges(nPlayer);
    }
}

// State 22: waiting at the ball. Button 35 with the "ball can be placed" flag: the ball is put
// at the placement point (a class-1 surface gets the extra placement call), the player's
// position follows it, and the swing (state 1) begins. Otherwise the cursor moves, buttons
// 26..29 play the pan sounds, and button 25 off the tee (if allowed) re-does the setup.
void STATEFUNC_PlaceBallUpdate(int nPlayer) {
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x23, 0)) {
        Ball*   pBall;
        Player* p = &gPlayers[nPlayer];
        if (p->uFlagsEF0 & 1) {
            pBall = &p->ball;
            if (Physics_DropBall(pBall, p->vPlacement)) {
                if (gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].nClass == 1) {
                    fn_80055AA8(pBall, pBall->vPos, nPlayer);
                }
                Vec_Copy(pBall->vPos, p->vBall);
                GOLFERSTATE_Switch(GS_PRE_SHOT, nPlayer);
            }
        }
    } else {
        fn_800EDAE0(nPlayer);
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1A, 1)) {
            EVENT_Trigger(nPlayer, 0x16, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1B, 1)) {
                EVENT_Trigger(nPlayer, 0x17, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1C, 1)) {
            EVENT_Trigger(nPlayer, 0x18, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1D, 1)) {
                EVENT_Trigger(nPlayer, 0x19, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) &&
            gPlayers[nPlayer].ball.nLie != 0 && GM_PlayerTakeMulligan(nPlayer)) {
            fn_8001D8DC(nPlayer);
            fn_800689D4(nPlayer);
        }
    }
    PlaceBall_UpdateMomentums(nPlayer, 1.0f);
}

// State 3: the aiming camera, held while button 8 is down (the caddie keeps updating). Pan
// sounds on buttons 9/10, 30 and 11..14; once the camera has arrived (flag from the enter) a
// one-off 0x67 event.
void STATEFUNC_ZoomUpdate(int nPlayer) {
    Caddie_Update(nPlayer);
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(9, 0))) {
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xA, 0))) {
                EVENT_Trigger(nPlayer, 0xE, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1E, 0))) {
            EVENT_Trigger(nPlayer, 0xF, 0, -1);
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1))) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1))) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1))) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1))) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    fn_80068AC8(nPlayer);
    if (lbl_80281E11 != 0) {
        if (fn_800C7340(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer)) {
            fn_80062D6C(0x67, nPlayer);
            lbl_80281E11 = 0;
        }
    }
}

// State 6, the putt preview playing. Any button ends it (any pad for a CPU). Otherwise the
// ghost ball (in ballBefore) is stepped preview-speed times a frame, 20 ticks each, with the
// simulating flag up; the camera follows with quarter-second moves while the ghost is still
// closing on the pin, and the state pops once the ghost has stopped and the camera settled.
void STATEFUNC_GreenWatchRollUpdate(int nPlayer) {
    Vec4        vOffset = {0.45f, 0.45f, 0.45f, 0.5f};
    CourseInfo* pCourse = fn_8000C594();
    int         nSteps, i;
    Player*     p;
    Ball*       pGhost;
    s32*        pGhostState;
    s32*        pnView;
    f32*        pGhostMinDist;

    Caddie_Update(nPlayer);
    if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            GOLFERSTATE_Pop(nPlayer);
            return;
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            GOLFERSTATE_Pop(nPlayer);
            return;
        }
    }
    nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
    p = &gPlayers[nPlayer];
    if (p->ballBefore.nState == 1 || p->ballBefore.nState == 5) {
        if (fn_80063C50(fn_80017028(p->nView[0]))) {
            GOLFERSTATE_Pop(nPlayer);
        }
        return;
    }
    if (fn_80063C50(fn_80017028(p->nView[0]))) {
        GOLFERSTATE_Pop(nPlayer);
        return;
    }
    pGhost        = &p->ballBefore;
    pGhostState   = &p->ballBefore.nState;
    pnView        = p->nView;
    pGhostMinDist = &p->ballBefore.fClosest;
    for (i = 0; i < nSteps; i++) {
        Ball_SetSimulating(1);
        Physics_Simulate(pGhost, 20);
        Ball_SetSimulating(0);
        if (*pGhostState == 1 || *pGhostState == 5) {
            if (!fn_80063C90(fn_80017028(*pnView))) {
                fn_80063BF4(fn_80017028(*pnView), 0.25f, (f32*)&vOffset);
            }
        } else if (!fn_80063C90(fn_80017028(*pnView)) && pCourse != NULL) {
            int nPinSet = Game_CurrentPinSet();
            if (*pGhostMinDist < 0.5f ||
                !(*pGhostMinDist < Vec_Distance(pGhost->vPos, &pCourse->pin[nPinSet].x) - 0.1f)) {
                fn_80063BF4(fn_80017028(*pnView), 0.25f, (f32*)&vOffset);
            }
        }
    }
}

// State 13: the ball has come to rest and the golfer reacts. On the first frame the result is
// handed to the view; a good result (gpGame+0x294, not on cameras 1/4) plays the reaction
// animation 9 or cuts to camera 16. When the view is done and the camera is not 16, flag bit 2
// is set and it is state 17 (the hole-out sequence). While animation 9 plays, camera 16 is
// taken once the animation allows, and the reaction shot lines up.
void STATEFUNC_InTheHoleUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);

    if (lbl_80281E12 != 0) {
        lbl_80281E12 = 0;
        fn_800C7168(pV, GM_ChooseRemoveBallState(nPlayer));
        if (fn_800C7170(pV)) {
            fn_800C7158(pV, 1);
            return;
        }
        fn_800C7158(pV, GM_ShowPostShotAnimation(nPlayer));
        if (gpGame->n294 != 0 && pV->nCamera != 1 && pV->nCamera != 4) {
            if (!(gPlayers[nPlayer].uFlags & 8) || fn_8006AA9C(nPlayer) == 2) {
                if (fn_80095780(gPlayers[nPlayer].pChar) != 9 && fn_80095798(gPlayers[nPlayer].pChar) != 9 &&
                    fn_800C7160(pV)) {
                    fn_80095744(gPlayers[nPlayer].pChar, 9);
                } else {
                    View_SetCamera(pV, 0x10, nPlayer, gPlayers[nPlayer].nView[0]);
                }
            }
        }
    }
    if (fn_800C7170(pV) && pV->nCurCamera != 0x10 && pV->nCamera != 1 && pV->nCamera != 4) {
        gPlayers[nPlayer].uFlags |= 2;
        GOLFERSTATE_Switch(GS_FADE_TO_REMOVE_BALL, nPlayer);
        return;
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9 && pV->nCurCamera != 0x10 && Game_GetMode() != 11 &&
        !fn_800734A0(&gPlayers[nPlayer].pChar->blend)) {
        View_SetCamera(pV, 0x10, nPlayer, gPlayers[nPlayer].nView[0]);
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9) {
        if ((fn_80062C1C(gPlayers[nPlayer].pChar) != 0 || fn_80062C10(gPlayers[nPlayer].pChar) != 0) &&
            !fn_800C6604(pV)) {
            GolfCamera_CutToGolferDoneAnimatingCam(pV, nPlayer);
        }
    }
    GM_DoPostShotInHoleUI(nPlayer);
}

// State 11 begins: the swing animation. The camera is 12 in a replay, else one of the three
// special swing cameras (20..22) the view offers, else 13. The animation's event hooks are
// registered (a wider window on the special cameras), the ball is teed up on the tee, and the
// live ball's state is cleared for the launch.
void STATEFUNC_ReplaySwingInit(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    Ball* pBall;

    if (gSession.bReplay != 0) {
        View_SetCamera(pV, 0xC, nPlayer, gPlayers[nPlayer].nView[0]);
    } else {
        fn_80062D98();
        if (fn_800C44A8(pV, nPlayer)) {
            View_SetCamera(pV, 0x14, nPlayer, gPlayers[nPlayer].nView[0]);
        } else if (fn_800C44CC(pV, nPlayer)) {
            View_SetCamera(pV, 0x15, nPlayer, gPlayers[nPlayer].nView[0]);
        } else if (fn_800C44E0(pV, nPlayer)) {
            View_SetCamera(pV, 0x16, nPlayer, gPlayers[nPlayer].nView[0]);
        } else {
            View_SetCamera(pV, 0xD, nPlayer, gPlayers[nPlayer].nView[0]);
        }
    }
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    gPlayers[nPlayer].pChar->n20 = 7;
    gPlayers[nPlayer].pChar->nAnim = 7;
    if (gPlayers[nPlayer].uFlags & 8) {
        CharAnim_StartTapIn(gPlayers[nPlayer].pChar);
    } else {
        if (fn_800C6D80() || fn_800C44A8(pV, nPlayer) || fn_800C44CC(pV, nPlayer) ||
            fn_800C44E0(pV, nPlayer)) {
            CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                           -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
        } else {
            CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                           -20000.0f, -90000.0f, -10000.0f, 0.0f, -10000.0f);
        }
        SKEL_SetIKSolutionWeight(gPlayers[nPlayer].pChar->pModel->pSkel, 1.0f);
    }
    Swing_ClearFrameFlag(nPlayer);
    if (gPlayers[nPlayer].ball.nLie == 0) {
        fn_80047EF0(&gPlayers[nPlayer].ball, nPlayer, 1);
    }
    pBall = &gPlayers[nPlayer].ball;
    fn_80047B6C(pBall, nPlayer);
    fn_80047BC0(pBall, nPlayer);
    if (gSession.bReplay != 0) {
        GameEffects_ResetGameEffectSettings();
    } else {
        fn_800DAF74();
    }
    GameEffects_SetSuperSlowMo(1, nPlayer, fn_800C6B7C(pV));
    gPlayers[nPlayer].ball.nState = 0;
    if (gSession.bReplay != 0) {
        fn_80062CE0(1);
    }
}

// State 11: the swing animation. Nothing more until it passes its impact event (2). Then in a
// replay the ball launches and it is state 12; otherwise once the camera has reached its mark the
// impact sound plays, the special swing camera is chosen, the ball launches and it is state 12;
// before that the camera keeps moving and one of two follow-through blends plays.
void STATEFUNC_ReplaySwingUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    u8    bSpecial = 0;
    GameEffects_SetSuperSlowMo(1, nPlayer, fn_800C6B7C(fn_80017028(gPlayers[nPlayer].nView[0])));
    if (!fn_80048574(gPlayers[nPlayer].pChar, 2) ||
        fn_8005CB78(gPlayers[nPlayer].pChar, 2) < gPlayers[nPlayer].pChar->fAnimTime) {
        if (gSession.bReplay) {
            fn_8002792C(gPlayers[nPlayer].pChar->pModel->pSkel);
            Swing_Launch(nPlayer);
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        } else if (fn_800C4518(pV) >= fn_800C6B38(pV)) {
            EVENT_Trigger(nPlayer, 0xA, &gPlayers[nPlayer].ball, 1);
            fn_800C44A8(pV, nPlayer);
            fn_8002792C(gPlayers[nPlayer].pChar->pModel->pSkel);
            Swing_Launch(nPlayer);
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        } else {
            fn_800C5CEC(pV, nPlayer);
            GameEffects_SetSuperSlowMo(1, nPlayer, fn_800C6B7C(pV));
            if (fn_800C4518(pV) >= fn_800C6B38(pV)) {
                bSpecial = fn_800C5FE4(pV, nPlayer);
            }
            if (bSpecial) {
                CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0x12, fn_80072ACC, 1, 8,
                                               -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
            } else {
                CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                               -20000.0f, -90000.0f, -10000.0f, 0.0f, -10000.0f);
            }
            fn_800957D8(gPlayers[nPlayer].pChar);
            Swing_ClearFrameFlag(nPlayer);
            fn_80062D98();
        }
        fn_800A5980(nPlayer);
    }
    if (gPlayers[nPlayer].nClub != CLUB_PUTTER_e) {
        fn_800A573C(nPlayer);
    }
}

// State 1 begins: addressing the ball. Camera 25 and the game's 0x20C hook; a fresh Shot_Plan
// when the game asks (gpGame+0x276, and it ends any replay); the glove comes off for a putt;
// both views attached; the ball teed up on the tee and kept as it lies; the distance to the
// pin stored; a CPU (outside game type 8) told to hide its HUD; and if the swing is to be
// shown, the address animation - 10 for a low-IQ golfer off the tee outside a lesson, else 1 -
// and camera 11.
void STATEFUNC_PreShotInit(int nPlayer) {
    Ball* pBall;
    int   nView, k;

    fn_80062F1C(fn_80017028(gPlayers[nPlayer].nView[0]));
    nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 0x19, nPlayer, nView);
    gpGame->pfn20C(nPlayer);
    fn_80062B64(nPlayer);
    fn_80062B60(nPlayer);
    GM_MovePlayerToBall(nPlayer);
    if (gpGame->b276 != 0) {
        gSession.bReplay = 0;
        Shot_Plan(nPlayer, 1);
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        fn_800CC5C0(gPlayers[nPlayer].pChar, "Glove", "GloveOff");
    } else {
        fn_800CC5C0(gPlayers[nPlayer].pChar, "Glove", "GloveOn");
    }
    gPlayers[nPlayer].fThinkTime = 0.0f;
    for (k = 0; k < 2; k++) {
        fn_8001704C(gPlayers[nPlayer].nView[k], nPlayer);
    }
    EVENT_Trigger(nPlayer, 0x2A, 0, -1);
    fn_8001D8DC(nPlayer);
    fn_8001C804(nPlayer, 1, 1);
    if (gpGame->b277 != 0) {
        fn_800957D8(gPlayers[nPlayer].pChar);
    }
    GameEffects_ResetGameEffectSettings();
    fn_800D8D10(nPlayer);
    fn_80068AA8(nPlayer);
    fn_800957FC(gPlayers[nPlayer].pChar, 1);
    Emotion_UpdatePlayerEmotion(nPlayer);
    if (gPlayers[nPlayer].pChar->n2C == 0) {
        fn_800957B0(gPlayers[nPlayer].pChar, 1);
    }
    fn_80045824(nPlayer);
    Swing_ResetBoostAndSpin(nPlayer);
    BreakLine_Start(gPlayers[nPlayer].nView[0]);
    fn_8009B970(gPlayers[nPlayer].nView[0]);
    if (gPlayers[nPlayer].ball.nLie == 0) {
        fn_80047EF0(&gPlayers[nPlayer].ball, nPlayer, 1);
    }
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
    if (gPlayers[nPlayer].ball.nLie != 10 && gPlayers[nPlayer].ball.nLie != LIE_GREEN_e &&
        gPlayers[nPlayer].ball.nLie != LIE_INCUP_e) {
        fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
    }
    pBall = &gPlayers[nPlayer].ball;
    fn_80054A6C(pBall);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].fA64 = fn_800D04AC(nPlayer);
    EVENT_Trigger(nPlayer, 3, 0, -1);
    if (gSession.nGameType != 8 && Player_IsCPU(nPlayer)) {
        fn_800E3D38(nPlayer, 1);
    }
    if (fn_800DDFB4(nPlayer) != 0) {
        fn_8007326C(gPlayers[nPlayer].pChar->anim);
        if (gPlayers[nPlayer].bLowIQPenalty != 0 && gPlayers[nPlayer].ball.nLie != 0 && !fn_80100294()) {
            fn_80095744(gPlayers[nPlayer].pChar, 10);
        } else {
            fn_80095744(gPlayers[nPlayer].pChar, 1);
        }
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
        nView = gPlayers[nPlayer].nView[0];
        View_SetCamera(fn_80017028(nView), 0xB, nPlayer, nView);
    }
}

// State 1 update: addressing the ball. The game's 0x238 hook can skip it (unless the camera is
// 0 or 11). The golfer's animation places the ball: while its "ball" event (3; 0x10 and 0x11 for
// two other props) is pending the ball rides in the hand, and once it fires the ball is set down
// - on the tee, 2 in up and teed. A ball still moving is stepped. Button 0 hurries the camera
// (a CPU also rehearses here once its ball is still). When the camera is done the ball is put
// in place and it is state 2; after 10 s on a camera it is state 2 anyway.
void STATEFUNC_PreShotUpdate(int nPlayer) {
    Vec4    vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    u8      bInHand = 0;
    View*   pV;
    u8*     pSlot;
    int     nSteps, i;
    s32*    pState;
    f32     vHand[4];

    pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    pSlot = lbl_80281DA0 + nPlayer * 0x104;
    if (pV->nCurCamera != 11 && pV->nCurCamera != 0 && gpGame->pfn238(nPlayer)) {
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
        return;
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 10) {
        if (fn_80048574(gPlayers[nPlayer].pChar, 3)) {
            if (fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
                fn_80062B98(gPlayers[nPlayer].pChar, 3);
                if (!fn_800559BC(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos)) {
                    Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos);
                }
            } else {
                Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, gPlayers[nPlayer].ball.vPos);
                bInHand = 1;
            }
        }
    } else if (fn_80048574(gPlayers[nPlayer].pChar, 0x10)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 0x10)) {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, vHand);
            fn_80062B98(gPlayers[nPlayer].pChar, 0x10);
            fn_800A3CB0(vHand, nPlayer);
        }
    } else if (fn_80048574(gPlayers[nPlayer].pChar, 0x11)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 0x11)) {
            fn_80062B98(gPlayers[nPlayer].pChar, 0x11);
            fn_800A3DF4(nPlayer);
        } else {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, vHand);
            fn_800A3D6C(vHand, nPlayer);
        }
    }
    if (gPlayers[nPlayer].ball.nLie == 0 && fn_80048574(gPlayers[nPlayer].pChar, 3)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
            fn_80062B98(gPlayers[nPlayer].pChar, 3);
            gPlayers[nPlayer].ball.vPos[1] += 2.0f;
            fn_80055AA8(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos, nPlayer);
            fn_80047EF0(&gPlayers[nPlayer].ball, nPlayer, 0);
        } else {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, gPlayers[nPlayer].ball.vPos);
            fn_8001DA04(gPlayers[nPlayer].pChar, pSlot + 0x30, pSlot + 0x50);
            bInHand = 1;
        }
    } else if (gPlayers[nPlayer].ball.nLie == 0) {
        fn_80047EF0(&gPlayers[nPlayer].ball, nPlayer, 0);
    }
    pState = &gPlayers[nPlayer].ball.nState;
    if (*pState != 0 && gPlayers[nPlayer].ball.nCollideCount == 0) {
        nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
        Ball_SetSimulating(1);
        for (i = 0; i < nSteps; i++) {
            Physics_Simulate(&gPlayers[nPlayer].ball, 20);
        }
        Ball_SetSimulating(0);
    } else if (*pState != 0) {
        Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos);
    }
    if (!Player_IsCPU(nPlayer)) {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) && !fn_80100294()) {
            fn_800C70F8(pV, 1);
            if (!fn_80063C90(pV)) {
                fn_80063BF4(pV, 0.25f, (f32*)&vOffset);
                fn_800C7080(pV);
            }
        }
    } else {
        if (*pState == 0 && !bInHand) {
            AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
        }
        if (!fn_80100294() && fn_80014300(fn_800142AC(0, 0))) {
            fn_800C70F8(pV, 1);
            if (!fn_80063C90(pV)) {
                fn_80063BF4(pV, 0.25f, (f32*)&vOffset);
                fn_800C7080(pV);
            }
        }
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) != 10 && fn_80095780(gPlayers[nPlayer].pChar) != 1 &&
        fn_800C6E88(pV, nPlayer)) {
        fn_80095744(gPlayers[nPlayer].pChar, 5);
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
        fn_80063CF0(pV, 0x17, nPlayer);
    }
    if (fn_80063C7C(pV) && (fn_800C6F7C(pV, nPlayer, 0.25f) || !fn_800C6E88(pV, nPlayer))) {
        if (bInHand) {
            if (fn_80095780(gPlayers[nPlayer].pChar) == 10) {
                Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].vBall);
            } else {
                fn_80055AA8(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPrev, nPlayer);
            }
        }
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
        return;
    }
    if (pV->fCamTime > 10.0f) {
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) != 10 && fn_80095780(gPlayers[nPlayer].pChar) != 1 &&
        fn_800C6F7C(pV, nPlayer, 0.25f) && !fn_80063C90(pV)) {
        fn_80063BF4(pV, 0.25f, (f32*)&vOffset);
    } else if ((fn_80095780(gPlayers[nPlayer].pChar) == 10 || fn_80095780(gPlayers[nPlayer].pChar) == 1) &&
               gPlayers[nPlayer].pChar->fAnimTime > gPlayers[nPlayer].pChar->fAnimEnd - 0.25f &&
               !fn_80063C90(pV)) {
        fn_80063BF4(pV, 0.25f, (f32*)&vOffset);
    }
}

// State 18, holed out: animation 12, the golfer picks the ball out of the cup and tosses it.
// While the ball is in the hand it follows the hand bone (kept at least a real ball's radius,
// 0.84 in, above the ground). At the animation's event 4 the ball is thrown: a real launch of
// the kept ball along the hand's motion, at 0.5 x 60 x 60 x FRAME_RATE x (yards moved / 1760) - the
// hand's speed in miles per hour, halved. Then, until event 3, the thrown ball is stepped 20
// ticks a frame and the live ball follows it. Camera 16 over the whole thing.
void STATEFUNC_RemoveBallUpdate(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    s32*  pnView  = gPlayers[nPlayer].nView;
    View* pV      = fn_80017028(*pnView);
    Character** ppChar = &gPlayers[nPlayer].pChar;
    Ball* pBall;
    f32   vDir[4];
    f32   vPos[4];
    f32   fGround, fSpeed;

    if (fn_80095780(*ppChar) == 12 && pV->nCurCamera != 0x10) {
        int nView = *pnView;
        View_SetCamera(fn_80017028(nView), 0x10, nPlayer, nView);
        fn_80063B98(fn_80017028(*pnView), 0.75f, (f32*)&vOffset);
    }
    if (fn_8001DBF4(*ppChar)) {
        int   nBone = fn_8001EED8((*ppChar)->pModel, 0x54);
        f32*  pBallY;
        pBall = &gPlayers[nPlayer].ball;
        Vec3Copy((*ppChar)->pModel->pMatrices[nBone][3], pBall->vPos);
        Vec3Copy(pBall->vPos, vPos);
        vPos[1] += 30.0f;
        fGround = fn_8004D620(fn_8000C594(), vPos);
        if (fGround >= -60000.0f) {
            pBallY = &gPlayers[nPlayer].ball.vPos[1];
            if (*pBallY - gRealBallRadiusIn / 36.0f < fGround) {
                *pBallY = fGround + gRealBallRadiusIn / 36.0f;
            }
        }
    } else if (gSession.fFrameTime > 0.0f) {
        if (fn_80048574(*ppChar, 4) && fn_80062BB0(*ppChar, 4)) {
            Player* p     = &gPlayers[nPlayer];
            Ball*   pB    = &p->ball;
            f32*    pPrev = p->ball.vPrev;
            Vec3Copy(pB->vPos, pPrev);
            Character_GetBallOnFingerPosition(*ppChar, pB->vPos);
            if (fn_80062BB0(*ppChar, 3)) {
                f32 fT4 = fn_8005CB78(*ppChar, 4);
                if (fn_8005CB78(*ppChar, 3) > fT4) {
                    fn_80062DDC(pB->vPos, pPrev, vDir);
                    fSpeed = (f32)fn_80009680(fn_80009744(vDir));
                    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
                        fn_800BAF04(vDir, vDir);
                    }
                    fSpeed = 60.0f * (60.0f * (FRAME_RATE * (fSpeed / 1760.0f))) * 0.5f;
                    fn_80062B98(*ppChar, 4);
                    gPlayers[nPlayer].ballBefore.bHoled = 0;
                    fn_80051A18(&p->ballBefore, vDir, fSpeed, pB->vPos);
                }
            }
        }
        if (fn_80048574(*ppChar, 3) && fn_80062BB0(*ppChar, 3)) {
            f32 fT4 = fn_8005CB78(*ppChar, 4);
            if (fn_8005CB78(*ppChar, 3) > fT4) {
                s32 nState = gPlayers[nPlayer].ballBefore.nState;
                if (nState != 1 && nState != 5 && nState != 0) {
                    Player* p = &gPlayers[nPlayer];
                    Ball_SetSimulating(1);
                    Physics_Simulate(&p->ballBefore, 20);
                    Vec3Copy(p->ballBefore.vPos, p->ball.vPos);
                    Ball_SetSimulating(0);
                }
            }
        }
    }
    GM_DoPostShotInHoleUI(nPlayer);
}

// State 12: the ball is in the air. Once the view has faded past half way: holed -> state 13,
// else state 14. Before the fade, a ball that has come to rest is marked (lie 12 with flag 8)
// and the fade starts. While flying, buttons 22/23 (any pad for a CPU) drive the two flight
// camera toggles. A human outside split screen and lessons: button 24 with a replay recorded
// (and the game allowing it) replays the shot (state 11 via the replay launch); button 25 with
// the mulligan allowed takes the shot back.
void STATEFUNC_SimulateUpdate(int nPlayer) {
    u8    bA = 0;
    u8    bB = 0;
    View* pV;

    pV = fn_80017028(gPlayers[nPlayer].nView[0]);
    if (fn_800C6D9C()) return;
    GM_SimulateBallMovement(nPlayer);
    Swing_RumbleTick(nPlayer);
    if (fn_80062DD4(pV) && fn_80062DCC(pV) > 0.5f) {
        GM_PlayerTookShot(nPlayer);
        fn_8006C4A0();
        fn_800DBDA8(nPlayer);
        if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
            GOLFERSTATE_Switch(GS_IN_THE_HOLE, nPlayer);
        } else {
            GOLFERSTATE_Switch(GS_SHOW_YARDAGE, nPlayer);
        }
        return;
    }
    if (!fn_80062DD4(pV)) {
        s32* pState = &gPlayers[nPlayer].ball.nState;
        if (*pState == 1 || *pState == 5 || *pState == 0) {
            if (*pState != 5) {
                *pState = 0;
            }
            if (gPlayers[nPlayer].uFlags & 8) {
                gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
            }
            fn_80062DB8(pV, 1);
            return;
        }
    }
    fn_80058F5C(nPlayer);
    fn_8006BB5C(nPlayer);
    if (gpGame->b286 != 0 && !fn_8004560C()) {
        if (Player_IsCPU(nPlayer)) {
            if (fn_80014300(fn_800142AC(0x16, 1))) {
                bA = 1;
            } else if (fn_80014300(fn_800142AC(0x17, 1))) {
                bB = 1;
            }
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x16, 1)) {
                bA = 1;
            } else {
                if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x17, 1)) {
                    bB = 1;
                }
            }
        }
        fn_80045558(bA, nPlayer);
        if (bA) {
            bB = 0;
        }
        fn_80045494(bB, nPlayer);
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.nSplitScreen != 0) return;
    if (fn_800E430C(nPlayer)) return;
    if (fn_80100294()) return;
    if (gReplayData.bF10 != 0 && gpGame->b287 != 0) {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x18, 0)) &&
            !(gPlayers[nPlayer].uFlags & 8) &&
            (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED && !fn_800E53B8() &&
            !(gPlayers[nPlayer].pChar->u10 & 0x40)) {
            s32* pKeptState = &gPlayers[nPlayer].ballBefore.nState;
            if (*pKeptState != 0) {
                *pKeptState = 1;
            }
            if (fn_800C6D28()) {
                fn_800C6DE4();
            }
            if (fn_800C6D64()) {
                fn_800C6DFC();
            }
            fn_8006C300(nPlayer);
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
            return;
        }
    }
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) &&
        !(gPlayers[nPlayer].uFlags & 8) && GM_PlayerTakeMulligan(nPlayer)) {
        fn_8006C4A0();
        if (fn_800C6D28()) {
            fn_800C6DE4();
        }
        if (fn_800C6D64()) {
            fn_800C6DFC();
        }
    }
}

// State 10 begins: setting up the shot. The address animation and the HUD's club and shot
// kind; camera 12; the tutorial tips (first tee, first approach, first putt) for a human when
// tips are on; the caddie starts; on the tee every player's ball is set up; the shot flags
// are cleared; sound 7.
void STATEFUNC_SwingInit(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    int   nView;
    Ball* pBall;
    int   i;

    if (pV->nCurCamera == 0 && Player_IsCPU(nPlayer) && Game_GetMode() != 11) {
        fn_800957D8(gPlayers[nPlayer].pChar);
    }
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    fn_8001C774(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
    fn_8001C724(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    fn_80062BFC(gPlayers[nPlayer].pChar);
    fn_80062BE8(gPlayers[nPlayer].pChar);
    fn_8007326C(gPlayers[nPlayer].pChar->anim);
    if (gPlayers[nPlayer].swing.unk630 == 0) {
        fn_80058FA4(nPlayer);
        gPlayers[nPlayer].swing.unk630 = 1;
    }
    nView = gPlayers[nPlayer].nView[0];
    View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    GameEffects_ResetGameEffectSettings();
    fn_8001C804(nPlayer, 1, 1);
    fn_80068AA8(nPlayer);
    fn_800A562C((u8)nPlayer);
    fn_8006BAA8(nPlayer);
    EVENT_Trigger(nPlayer, 0x2A, 0, -1);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_8006ACF8(nPlayer, 5);
    gPlayers[nPlayer].fC20 = 0.0f;
    if (gPlayers[nPlayer].bC2E == 0 && gpGame->b281 != 0 && !Player_IsCPU(nPlayer)) {
        if (fn_800DA264() && fn_800DA174()) {
            fn_800E505C(0);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else if (fn_800DA264() && fn_800DA1D4()) {
            fn_800E505C(1);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else if (fn_800DA264() && fn_800DA234()) {
            fn_800E505C(2);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else {
            fn_800D1DAC(nPlayer);
        }
    }
    if (gPlayers[nPlayer].bC2E == 0 && !Player_IsCPU(nPlayer) && fn_800EC550() && fn_800ED540()) {
        if (fn_800F0818()) {
            fn_800E502C(fn_800EAC7C());
        } else {
            fn_800E4FFC(fn_800EAC7C());
        }
        fn_800E3D38(nPlayer, 0);
        fn_80062C80(gPlayers[nPlayer].nC58, 0);
        fn_800ED548();
    }
    Caddie_Start(nPlayer);
    fn_80062C5C();
    if (gPlayers[nPlayer].ball.nLie == 0) {
        fn_80047EF0(&gPlayers[nPlayer].ball, nPlayer, 1);
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pBall = &PLAYER(i)->ball;
            fn_80047B6C(pBall, i);
            fn_80047BC0(pBall, i);
        }
    }
    pBall = &gPlayers[nPlayer].ball;
    fn_80047B6C(pBall, nPlayer);
    fn_80047BC0(pBall, nPlayer);
    gPlayers[nPlayer].bC2E = 0;
    gPlayers[nPlayer].bPlanReady = 0;
    gPlayers[nPlayer].uFlags     = 0;
    EVENT_Trigger(nPlayer, 7, 0, -1);
    fn_800DB4E8(nPlayer);
    fn_80062B68(nPlayer);
}

// State 10 every frame: the shot setup. An idle timer (any button or the swing resets it, 10 s
// wraps). When the swing has been made (the per-frame swing poll says so): the RNG stream 1 is
// reseeded from the session seed, the view told, and it is state 11 (the swing animation) on a
// special camera - or state 12 straight away. Otherwise, for a human outside a replay before
// the swing starts: the caddie updates; button 6 re-plans the shot (the 0x264 hook or a putt
// aims at the pin, else a fresh target) with the break line and HUD redone; button 5 on a
// putt goes to the putt-line camera (state 7).
void STATEFUNC_SwingUpdate(int nPlayer) {
    int   nClub = gPlayers[nPlayer].nClub;
    u8    bSwung;

    if (fn_800E5098()) return;
    bSwung = fn_80058F5C(nPlayer);
    gPlayers[nPlayer].fC20 += gSession.fFrameTime;
    if (fn_80014300(gPlayers[nPlayer].nController) || gPlayers[nPlayer].swing.nState != 0) {
        gPlayers[nPlayer].fC20 = 0.0f;
    }
    if (gPlayers[nPlayer].fC20 >= 10.0f) {
        gPlayers[nPlayer].fC20 = 0.0f;
    }
    if (nClub != CLUB_PUTTER_e) {
        fn_800A573C((u8)nPlayer);
    }
    if (gpGame->b282 != 0) {
        if (fn_800E3DDC(nPlayer)) {
            if (gPlayers[nPlayer].swing.nState != 0) {
                fn_800E3D38(nPlayer, 0);
            }
        } else {
            if (gPlayers[nPlayer].swing.nState == 0) {
                fn_800E3D38(nPlayer, 1);
            }
        }
    }
    gpGame->pfn228(nPlayer);
    if (bSwung) {
        View* pV;
        GM_BallHit(nPlayer);
        fn_80062B6C(nPlayer);
        fn_8000B1D4(1, gSession.nSeed);
        pV = fn_80017028(gPlayers[nPlayer].nView[0]);
        fn_800C6618(pV, nPlayer);
        fn_800A5980((u8)nPlayer);
        if (fn_800C7138(pV) == 0) {
            EVENT_Trigger(nPlayer, 0x3B, 0, 0);
        }
        if (gpGame->b283 != 0 &&
            (fn_800C441C(pV, nPlayer) || fn_800C44A8(pV, nPlayer) || fn_800C44CC(pV, nPlayer) ||
             fn_800C44E0(pV, nPlayer))) {
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
        } else {
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        }
        return;
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bReplay != 0) return;
    if (gPlayers[nPlayer].swing.nState != 0) return;
    Caddie_Update(nPlayer);
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(6, 0)) {
        if (gpGame->b284 != 0) {
            if (gpGame->pfn264(nPlayer)) {
                AI_DefaultTarget(nPlayer);
            } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
                AI_DefaultTarget(nPlayer);
            } else {
                AI_ChooseTarget(nPlayer);
            }
            Shot_Prepare(nPlayer, 1);
            BreakLine_Start(gPlayers[nPlayer].nView[0]);
            fn_8009B970(gPlayers[nPlayer].nView[0]);
            fn_8001C804(nPlayer, 1, 1);
            fn_800957D8(gPlayers[nPlayer].pChar);
            fn_80095744(gPlayers[nPlayer].pChar, 5);
            fn_800689D4(nPlayer);
            fn_80062C38();
            if (gSession.nSplitScreen != 0) {
                fn_80062CB0(gPlayers[nPlayer].nC58, 1);
            }
            fn_80068AA8(nPlayer);
            fn_800642D0(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
            fn_800E3D38(nPlayer, 1);
        } else {
            fn_80068AA8(nPlayer);
        }
        gpGame->pfn22C(nPlayer);
    } else {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(5, 0)) &&
            gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            GOLFERSTATE_Push(GS_GREEN_REVERSE_PUTT, nPlayer);
        } else {
            GM_CheckForShotChanges(nPlayer);
        }
    }
}

// State 14: the ball has come to rest, not holed. First frame: the result to the view and the
// reaction (animation 9 or camera 15). Camera 15 once the animation allows; the reaction shot
// lines up. Then, unless the score display is up: with no menu, once the camera has settled
// (and the reaction animation is far enough along) the hole state is updated and it is either
// state 15 (the next shot) or a camera move. With the menu: a human outside split screen can
// take a mulligan (button 25, if allowed), watch the replay (button 24, if recorded and
// allowed) or continue (button 0); a CPU continues on any pad's button 0.
void STATEFUNC_ShowYardageUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    Vec4  vOffset;

    if (lbl_80281E13 != 0) {
        fn_800C7158(pV, GM_ShowPostShotAnimation(nPlayer));
        if (fn_80095780(gPlayers[nPlayer].pChar) != 9 && fn_80095798(gPlayers[nPlayer].pChar) != 9 &&
            fn_800C7160(pV)) {
            fn_80095744(gPlayers[nPlayer].pChar, 9);
        } else {
            View_SetCamera(pV, 0xF, nPlayer, gPlayers[nPlayer].nView[0]);
        }
        Swing_RumbleTick(nPlayer);
        lbl_80281E13 = 0;
    }
    vOffset = lbl_80183620;
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9 && pV->nCurCamera != 0xF && Game_GetMode() != 11 &&
        !fn_800734A0(&gPlayers[nPlayer].pChar->blend)) {
        View_SetCamera(pV, 0xF, nPlayer, gPlayers[nPlayer].nView[0]);
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) != 0 || fn_80062C10(gPlayers[nPlayer].pChar) != 0) &&
        fn_80095780(gPlayers[nPlayer].pChar) == 9 && !fn_800C6604(pV)) {
        GolfCamera_CutToGolferDoneAnimatingCam(pV, nPlayer);
    }
    if (fn_800E46B4()) return;
    if (!fn_800E4254(nPlayer)) {
        if (fn_80063C7C(pV)) {
            if (gPlayers[nPlayer].bLowIQPenalty != 0) {
                GM_ReplaceOOBBall(nPlayer);
            }
            GM_EndOfGolferTurn(nPlayer);
            return;
        }
        if (fn_80063C90(pV)) return;
        if (!fn_800C6604(pV) && fn_80095780(gPlayers[nPlayer].pChar) == 9) {
            if (!(fn_80062C28(gPlayers[nPlayer].pChar) >= lbl_80281F78->f170 / 2.0f ||
                  pV->f11C > 1.0f)) {
                return;
            }
        }
        fn_80062B78(nPlayer);
        fn_80062B74(nPlayer);
        fn_80062B70();
        if (Gimme_Allowed(nPlayer)) {
            GOLFERSTATE_Switch(GS_FADE_TO_TAP_IN, nPlayer);
        } else {
            fn_80063BF4(pV, lbl_80281F78->f170, (f32*)&vOffset);
        }
        return;
    }
    if (!Player_IsCPU(nPlayer) && gSession.nSplitScreen == 0) {
        if (gSession.bReplay == 0) {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) {
                if (!GM_PlayerTakeMulligan(nPlayer)) return;
                fn_80062D0C(nPlayer);
                return;
            }
        }
        if (gReplayData.bF10 != 0) {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x18, 0)) && gpGame->b287 != 0 &&
                !(gPlayers[nPlayer].uFlags & 8) && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED &&
                !fn_800E53B8() &&
                !(gPlayers[nPlayer].pChar->u10 & 0x40)) {
                lbl_80281E10 = 1;
                fn_80062D0C(nPlayer);
                fn_8006C300(nPlayer);
                GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
                return;
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            fn_800E41D4(nPlayer);
        }
    } else if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            fn_800E41D4(nPlayer);
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            fn_800E41D4(nPlayer);
        }
    }
}
