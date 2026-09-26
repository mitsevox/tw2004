// Swing.c (named by its file string at 0x8028118C, in SW_vInitModule's allocations): the stick
// swing - its phases, the miss and the power, and what the golfer's attributes do to them; the
// club trail and the boost display; the golfer state engine (GOLFERSTATE_*) and its states'
// callbacks (STATEFUNC_*). CodeWarrior GC/2.5, -O4,p. The formulas and tables are written up in
// docs/gameplay.md.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "dynobj.h"

// The swing module's data (gpSwing): the trail textures, meshes and buffers and the tuning
// values. Set up in SW_vInitModule and SW_vUIInit.
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

// port: at these calls the original sign-extends Golfer_GetAttribute's result as if it returned
// s8, while its definition in Golfer.c returns an int; the cast reproduces that. Calling through
// the mismatched type is undefined in standard C: a port writes (s8)Golfer_GetAttribute(...).
#define GOLFER_GET_ATTRIBUTE_S8(p, nAttr, nMode) \
    (((s8 (*)(Player*, int, int))Golfer_GetAttribute)(p, nAttr, nMode))

extern SwingState*   gpSwing;                    // 0x80281188
extern f32           gForgivenessTable[3][27];  // 0x80188168  rows: value at attribute 0 / 100 / 110
extern s32           gBoostSteps[8];             // 0x80188148  power boost per level: 1 2 4 6 9 12 16 20
extern f32           lbl_80281B40[];             // FLT_MAX
extern f32           gPuttXScale[8];             // 0x801882AC  per shot kind: 0.03 for a putt, 0.2 otherwise
extern f32           gSwingXScale[8];            // 0x801882CC  the same values again
extern s32           gClubCurve[CLUB_MAX_e];      // 0x80183578  per club, 0..26: how much it can shape
extern u8            lbl_80281194[4];            // a neutral pad: both sticks centred (0x80)
extern u8 (*gSwingPhaseFns[])(int nPlayer);
extern f32           gSwingRange[8];             // 0x801882EC  backswing rate by shot kind: -, 0.85, 0.5, 0.8

void  Swing_FaceVector(int nPlayer, f32* pOut);
f32   fn_8005BA94_MishitAngle(int nPlayer);
void  fn_8005B8C8_ShapeVector(int nPlayer, f32* pOut);
f32   fn_8005CC84(f32 fTan);                     // atanf
void  fn_8005CCA8(int nPlayer);
void  Vec_Sub(f32* pA, f32* pB, f32* pOut);      // 0x8005CBF4  a - b
void  Vec_Add(f32* pA, f32* pB, f32* pOut);      // 0x8005CBD0  a + b
void  SW_vUIBlurReset(int nPlayer);
void  fn_800360A0(void* p);
void  SW_vUIInit(int nPlayer);
void  fn_80036054(void* p, int a, s32* pDesc);
void  fn_8005A788(int nPlayer, int a);
void  fn_800AE3C4(int nPlayer);
u8*   Pad_State(int nPlayer, int nController);   // the pad's state: [1] main stick y, [3] C-stick y
int   Swing_StickX(int nPlayer, u8* pPad);       // 0x80058F04  main or C-stick by nStickUsed
int   Swing_StickY(int nPlayer, u8* pPad);       // 0x80058F30
f32   Swing_TopTime(SwingData* pSw);             // 0x80058E98  fTimeSwingTop - 0.0076
f32   Swing_StartTime(SwingData* pSw);           // 0x80058EA8  fTimeSwingStart + 0.0076
int   fn_800204A0(Clip* pBlend, f32* pOut, f32 fTime);   // samples pBlend->pD8 at fTime
void  Character_UpdateAnimation(Character* pObj, int a, f32 f);
void  SW_vSetSwingStrength(int nPlayer);
void  SW_vCheckForSwingBoost(int nPlayer);
void  SW_vUIAdjustClub(Character* pObj, SwingData* pSw, int nStickX);
int   fn_8005CC5C(void);
void  UI_Obj_RenderBoostUI(int nView);
void  fn_800360D4(u8* pMesh);

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
        REPLAY_GetSpin(nPlayer, &gPlayers[nPlayer].swing.fForwardSpin, &gPlayers[nPlayer].swing.fSideSpin);
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

// Lie 0 with club 0 (a driver off the tee): up to +0.1 power when the backswing's sideways angle
// is negative and its size falls between the curve's two knots, most at their midpoint.
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
        nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
    } else {
        switch (gPlayers[nPlayer].nShotKind) {
        case SHOT_TYPE_PUTT_e:
            nRowScale  = ROW_PUTTING + 1;
            nRowThresh = ROW_PUTTING;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL);
            if (gPlayers[nPlayer].fDistance < 2.0f) {
                gPlayers[nPlayer].swing.fMishitAngle = 0.0f;
                return;
            }
            break;
        case SHOT_TYPE_CHIP_e:
            nRowScale  = ROW_APPROACH_B + 1;
            nRowThresh = ROW_APPROACH_B;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
            break;
        case SHOT_TYPE_PITCH_e:
            nRowScale  = ROW_APPROACH_A + 1;
            nRowThresh = ROW_APPROACH_A;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
            break;
        case 5:
        case 6:
        case 7:
            nRowScale  = ROW_RECOVERY + 1;
            nRowThresh = ROW_RECOVERY;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
            break;
        default:
            switch (gPlayers[nPlayer].nClub) {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
                nRowScale  = ROW_DRIVING + 1;
                nRowThresh = ROW_DRIVING;
                nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_DRIVING_ACCURACY, ATTR_TOTAL);
                break;
            case 9: case 10: case 11: case 12:
                nRowScale  = ROW_STRIKING_A + 1;
                nRowThresh = ROW_STRIKING_A;
                nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
                break;
            case 13: case 14: case 15: case 16:
                nRowScale  = ROW_STRIKING_B + 1;
                nRowThresh = ROW_STRIKING_B;
                nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
                break;
            case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24:
                nRowScale  = ROW_STRIKING_C + 1;
                nRowThresh = ROW_STRIKING_C;
                nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
                break;
            default:
                nRowScale  = ROW_STRIKING_C + 1;
                nRowThresh = ROW_STRIKING_C;
                nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL);
                break;
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

// The final power for the shot. A CPU just scales what it planned (putts +5%). A human's full
// shot gets the boost, then loses distance to the swing error: a scaled part of it under the
// threshold, all of it above. Putts, chips and pitches skip the error; a putt over 75% on the
// meter counts as full power.
f32 SW_vCalculateShotPower(int nPlayer) {
    f32     fPower, fError;
    f32*    pPower;
    int     nRowScale, nRowThresh;
    int     nAttr;
    int     nKind;
    f32     fThresh, fScale;

    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        fPower = gPlayers[nPlayer].fPower;
        fPower *= AI_PowerScale(nPlayer);
        if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e && !(gPlayers[nPlayer].uFlags & 8)) {
            fPower *= 1.05f;
            if (fPower < 0.1f) {
                fPower = 0.1f;
            }
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    fPower = gPlayers[nPlayer].fPower;
    // fake match: pPower through PLAYER(), everything else through gPlayers[] (all gPlayers[]:
    // 94.2%; one Player* local: 84.9%)
    pPower = &PLAYER(nPlayer)->fPower;
    fError = fabs(gPlayers[nPlayer].swing.fMishitAngle);
    gPlayers[nPlayer].swing.fNonPowerShotPower = Swing_ApplyPowerBoost(nPlayer, fPower) - fError;
    nKind = gPlayers[nPlayer].nShotKind;
    switch (nKind) {
    case SHOT_TYPE_PUTT_e: {
        f32 fDist = gPlayers[nPlayer].fDistance < 1.0f ? 1.0f : gPlayers[nPlayer].fDistance;
        if (*pPower > gpSwing->fPuttFullPower) {
            *pPower = 1.0f;
        }
        fPower = *pPower * fn_80050D34(fDist);
        Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL);
        if (fPower < 0.1f) {
            fPower = 0.1f;
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    case SHOT_TYPE_CHIP_e:
    case SHOT_TYPE_PITCH_e: {
        f32 f;
        if (nKind == SHOT_TYPE_CHIP_e) {
            f = *pPower * Physics_EstimateShotPower(gPlayers[nPlayer].fDistance, &gPlayers[nPlayer].ball,
                                                    SHOT_TYPE_CHIP_e, gPlayers[nPlayer].nClub);
        } else {
            f = *pPower;
        }
        fPower = Swing_ApplyPowerBoost(nPlayer, f);
        Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
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
        nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
        break;
    default:
        if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
            gPlayers[nPlayer].ball.nLie == 8 || gPlayers[nPlayer].ball.nLie == 3 ||
            gPlayers[nPlayer].ball.nLie == 4) {
            nRowScale  = ROW_RECOVERY_PWR + 1;
            nRowThresh = ROW_RECOVERY_PWR;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
        } else {
            nRowScale  = ROW_DRIVING_PWR + 1;
            nRowThresh = ROW_DRIVING_PWR;
            nAttr      = GOLFER_GET_ATTRIBUTE_S8(&gPlayers[nPlayer], ATTR_DRIVING_ACCURACY, ATTR_TOTAL);
        }
        fPower = *pPower;
        fPower *= AI_PowerScale(nPlayer);
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

f32 SW_vGetShotPower(int nPlayer) {
    return gPlayers[nPlayer].swing.fShotPower;
}

// ---- the hit -----------------------------------------------------------------------------------

// The ball is struck. In game mode 10 the recorded seed and player 0's recorded swing data are
// restored; otherwise a live shot runs Luck_TakePerfectShot and REPLAY_Save, a replay REPLAY_Play.
// Then the miss (zero for a CPU or a perfect shot), the power, forgiveness, the launch blocks, and
// the aim - the player's aim plus the face vector's angle plus the miss - go to Physics_ShotImpact.
void SW_vImpact(int nPlayer) {
    Player* p;
    Ball*   pBall;
    int     nClub, nTrajectory, nKind;
    f32     fAim;

    p = &gPlayers[nPlayer];
    if (Game_GetMode() == 10) {
        Misc_SetSeedFunc(0, gReplayData.nSeed);
        // port: 0x630 of the swing data's 0x634 bytes (it holds no pointers)
        Mem_cpy(&gPlayers[0].swing, &gReplayData.player.swing, 0x630);
    } else if (gSession.bReplay == 0) {
        Luck_TakePerfectShot(nPlayer);
        REPLAY_Save(nPlayer);
    } else {
        REPLAY_Play(nPlayer);
    }
    nClub      = p->nClub;
    nTrajectory = p->nTrajectory;
    nKind       = p->nShotKind;
    pBall       = &p->ball;
    Swing_FaceVector(nPlayer, gPlayers[nPlayer].vLaunchA);
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        gPlayers[nPlayer].swing.fMishitAngle = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fMishitAngle = fn_8005BA94_MishitAngle(nPlayer);
    }
    gPlayers[nPlayer].swing.fShotPower = SW_vCalculateShotPower(nPlayer);
    Swing_ApplyForgiveness(nPlayer);
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e && gPlayers[nPlayer].fDistance < 2.0f) {
        gPlayers[nPlayer].vLaunchA[0] = 0.0f;
        gPlayers[nPlayer].vLaunchA[1] = 0.0f;
        gPlayers[nPlayer].vLaunchA[2] = 1.0f;
        gPlayers[nPlayer].vLaunchA[3] = 0.0f;
    }
    fn_8005B8C8_ShapeVector(nPlayer, gPlayers[nPlayer].vLaunchB);
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
    Physics_ShotImpact(pBall, nClub, nKind, gPlayers[nPlayer].swing.fShotPower, fAim, nTrajectory,
                gPlayers[nPlayer].vLaunchA, gPlayers[nPlayer].vLaunchB);
}

// ---- the meter's miss ----------------------------------------------------------------------------

// The swing's miss: the back vector (x centre - top, z top - centre) and the through vector (x
// impact - centre, z centre - impact), both normalised; the angle of (0, 0, 1) plus through - back,
// clamped to gpSwing->fMaxError. Both x samples first get a random +-15 (of a +-128 stick), and the
// x differences are scaled by gSwingXScale (0.2; 0.03 on a putt).
f32 fn_8005BA94_MishitAngle(int nPlayer) {
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

    fTopX    += Misc_RandFuncf(0) * 30.0f - 15.0f;
    fImpactX += Misc_RandFuncf(0) * 30.0f - 15.0f;
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
    f32 fOut;
    f32 fT;
    f32 fRange;

    fOut   = 0.0f;
    fRange = gpSwing->fCurveMin +
             ((f32)gClubCurve[*pClub] / 26.0f) * (gpSwing->fCurveMax - gpSwing->fCurveMin);
    fT     = (f32)fabs(fBackAngle / (PI / 2));
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
    f32     fTopY, fTopX, fCentreX, fDY;
    f32     fAngle, fSin, fCos, fK;

    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
        return;
    }
    fTopX    = gPlayers[nPlayer].swing.nBackSwingX;
    fTopY    = gPlayers[nPlayer].swing.nBackSwingY;
    fCentreX = gPlayers[nPlayer].swing.nCalibrateX;
    if (0.0f == fCentreX) {
        fCentreX = 1.0f;
    }
    if (0.0f == (fDY = fTopY - (f32)gPlayers[nPlayer].swing.nCalibrateY)) {
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
void fn_8005B8C8_ShapeVector(int nPlayer, f32* pOut) {
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        fn_8002D560_ShapeDir(nPlayer, pOut);
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
}

// Empty.
void fn_8005CCA8(int nPlayer) {
}

// ---- starting the swing ---------------------------------------------------------------------------

// The swing is under way: phase 1, the animation started, its three marks read, the 25-sample
// stick history filled with the centre, the spin stick centred.
// The backswing's top mark, a hair early.

// Free what SW_vInitModule set up: the two trail meshes and the three buffers per view.

// Set the swing module up: player 1's ratings and the fixed tuning into gpSwing, the two trail
// meshes, each player's trail textures (SW_vUIInit), cleared boosts and trail colour, and three
// buffers per view.
void SW_vInitModule(void) {
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
        SW_vUIInit(i);
        SW_vClearBoosts(i);
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

// A controller's pad state; the neutral pad when there is none or fn_80100C00() is true.
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
void SW_vInitSwing(int nPlayer) {
    gPlayers[nPlayer].swing.nState = 0;
    gPlayers[nPlayer].swing.nRestCX = gPlayers[nPlayer].swing.nRestCY = 0x80;
    gPlayers[nPlayer].swing.nRestX = gPlayers[nPlayer].swing.nRestY = 0x80;
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    SW_KillVibration(nPlayer);
    SW_vClearBoosts(nPlayer);
    gPlayers[nPlayer].swing.nSpinCtrlX = 0x80;
    gPlayers[nPlayer].swing.nSpinCtrlY = 0x80;
    gPlayers[nPlayer].swing.fTargetTurnAngle = 0.0f;
    gPlayers[nPlayer].swing.fCurrentTurnAngle = 0.0f;
    gPlayers[nPlayer].swing.f10 = 0.0f;
    gPlayers[nPlayer].swing.f14 = 0.0f;
    fn_800AE3C4(nPlayer);
}

void SW_vStateInitBackSwing(int nPlayer) {
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
    SW_vClearBoosts(nPlayer);
    pSw->nSpinCtrlX = 128;
    pSw->nSpinCtrlY = 128;
    pSw->fTargetTurnAngle        = 0.0f;
    pSw->fCurrentTurnAngle        = 0.0f;
}

// Waiting for the backswing. A CPU (or game mode 10) starts at once. A human starts the frame
// either stick is pulled past 160 of 255 - more than a quarter of its travel - and that stick's
// rest position becomes the centre sample; the C-stick can swing too (nStickUsed). While
// nothing is pulled the rest positions are held at 128.
int Swing_WaitForBackswing(int nPlayer) {
    Player* p          = &gPlayers[nPlayer];
    int     nController = p->nController;
    Character* pChar   = p->pChar;
    u8*     pPad;
    u8      bMain, bCStick;

    if (Controller_IsCPU(nController) || Game_GetMode() == 10) {
        gPlayers[nPlayer].swing.nState = 1;
        SW_vStateInitBackSwing(nPlayer);
        pChar->uFlags &= ~1;
        SW_vUIBlurReset(nPlayer);
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
        SW_vStateInitBackSwing(nPlayer);
        SW_vUIBlurReset(nPlayer);
        REPLAY_RecordStart();
    } else {
        gPlayers[nPlayer].swing.nRestCY = 128;
        gPlayers[nPlayer].swing.nRestCX = 128;
        gPlayers[nPlayer].swing.nRestY  = 128;
        gPlayers[nPlayer].swing.nRestX  = 128;
    }
    return 0;
}

// ---- the backswing --------------------------------------------------------------------------------

// Enter the hold at the top: the animation goes to the pause time and slows to rate 0.008 (flag
// 0x40), and the hold timers are cleared.
void SW_vStateInitBackSwingFigit(int nPlayer) {
    Player*    p    = &gPlayers[nPlayer];
    Character* pObj = p->pChar;
    SwingData* pSw  = &p->swing;
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

// Phase 1, the backswing. A CPU (or game mode 10) plays the backswing animation to 98% of the way
// to the top (65% when fn_80100AF8()) and then swings down. A human's backswing follows the stick:
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
            if (fn_800204A0(pObj->pBlend, pObj->v1638,
                            pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
                pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pEvents[2].fTime - pObj->v1638[1])
                                  + pObj->f1644;
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
            int nDX = Swing_DeadZone(nX);
            int nDY = Swing_DeadZone(nY);
            fMag = (f32)fn_80009680((nDX - 128) * (nDX - 128) + (nDY - 128) * (nDY - 128));
        }
        if (fMag > 100.0f) {
            fMag = 100.0f;
        }
        fRange  = fTop - fStart;
        fTarget = fStart + (fMag / 100.0f) * fRange;
        fDelta  = fTarget - fAnimTime;
        fRate   = 1.0f + (f32)fabs(fDelta) / fRange;
        fRate   = fRate * fRate - 1.0f;
        fRate   = (fRate < 1.0f) ? fRate : 1.0f;
        {
            int nKind = gPlayers[nPlayer].nShotKind;
            // fake match: written as == 1 || == 2 || == 3, CW merges the first two tests into one
            // range compare; the negated form keeps three compares on the loaded kind
            if (!(nKind != 1 && nKind != 2 && nKind != 3)) {
                fRange = fRange / gSwingRange[nKind];
            } else {
                fRange = 1.0f;
            }
        }
        Anim_SetRate(pObj->anim, fRate * fRange);
        if (fMag > 93.0f) {
            Anim_SetRate(pObj->anim, fRange);
        }
        if (fDelta > -0.05f && fDelta < 0.05f) {
            pSw->fFidgetPauseTime = pObj->fAnimTime;
            pSw->nFidgetPauseStickY = nY;
            SW_vStateInitBackSwingFigit(nPlayer);
            pSw->nState = 2;
        } else if ((pObj->uFlags & 0x40) && fDelta > 0.0f) {
            pObj->uFlags &= ~0x40;
        } else if (!(pObj->uFlags & 0x40) && fDelta < 0.0f) {
            pObj->uFlags |= 0x40;
            pSw->fPowerBoostDieTime = 1.0f / 12.0f;
        }
        pSw->nCtrlListX[pSw->nCtrlListIndex] = nX;
        pSw->nCtrlListY[pSw->nCtrlListIndex] = nY;
        pSw->nCtrlListIndex++;
        pSw->nCtrlListIndex %= 25;
        SW_vSetSwingStrength(nPlayer);
        pSw->nBackSwingX = nX;
        pSw->nBackSwingY = nY;
        SW_vCheckForSwingBoost(nPlayer);
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
        SW_vSetSwingStrength(nPlayer);
        Anim_SetRate(pObj->anim, 1.0f);
        fn_80095744(pObj, 7);
        if (fn_800204A0(pObj->pBlend, pObj->v1638,
                        pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
            pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pEvents[2].fTime - pObj->v1638[1])
                              + pObj->f1644;
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
        SW_vUIBlurReset(nPlayer);
    }
    return 0;
}

// ---- at the top, and the downswing ----------------------------------------------------------------

// Phase 2, holding at the top. Any change in the stick's y drops back to phase 1 (backing down if it
// came forward). While it is steady the animation waggles +-0.0076 around the top. If the
// stick sits near centre (y at or below 160, x within 64..192) for over 0.1 s the swing is
// abandoned: phase 0, the address animation, event 9.
int SW_vStateBackSwingFigit(int nPlayer) {
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
    SW_vSetSwingStrength(nPlayer);
    return 0;
}

// Phase 3, the downswing. For a human, every frame the stick is forward of 96 and more than ~17
// units from the centre sample, that reading becomes the impact sample - so what counts is where
// the stick was pointing on the way through, not when. The ball goes when the animation reports
// impact (n5CC < 0): phase 5, SW_vImpact, the mis-hit rumble, event 0x2B on a putt.
int SW_vStateDownSwing(int nPlayer) {
    Player*    p;
    int        nController;
    Character* pObj;
    SwingData* pSw;
    s32        nX, nY;
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
        f32 fDist2;
        pPad   = Pad_State(nPlayer, nController);
        nX     = Swing_StickX(nPlayer, pPad);
        nY     = Swing_StickY(nPlayer, pPad);
        nDX    = nX - pSw->nCalibrateX;
        nDY    = nY - pSw->nCalibrateY;
        fDist2 = nDX * nDX + nDY * nDY;
        if (nY <= 96 && fDist2 > 300.0f) {
            pSw->nFollowThroughX  = nX;
            pSw->nFollowThroughY  = nY;
            pSw->nMishitX = nX;
            pSw->nMishitY = nY;
        }
    }
    if (pObj->n5CC < 0) {
        SKEL_RelaxIK(pObj->pModel->pSkel);
        gPlayers[nPlayer].swing.nState = 5;
        SW_vImpact(nPlayer);
        if (!Controller_IsCPU(nController) && gSession.bReplay == 0) {
            gPlayers[nPlayer].swing.bCanSpin = 1;
        }
        if (fn_8002E898_IsPad(nController) && gSession.bReplay == 0) {
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
void SW_KillVibration(int nPlayer) {
    s32*    pFrames;
    s32*    pController;
    Player* p;
    if (fn_8002E868_HasPad(nPlayer)) {
        p           = &gPlayers[nPlayer];
        pFrames     = &p->swing.nVibrateCount;
        pController = &p->nController;
        fn_800130F8(*pController, 0);
        fn_80013130(*pController, 0);
        gPlayers[nPlayer].swing.bVibrating = 0;
        *pFrames = 0;
    }
}

// Count the mis-hit rumble down and stop it when it runs out.
void SW_UpdateVibration(int nPlayer) {
    if (fn_8002E868_HasPad(nPlayer)) {
        if (gPlayers[nPlayer].swing.bVibrating) {
            if (gPlayers[nPlayer].swing.nVibrateCount <= 0) {
                SW_KillVibration(nPlayer);
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
    SW_UpdateVibration(nPlayer);
    Swing_SpinInput(nPlayer);
    Swing_ApplySpin(nPlayer);
    return 0;
}

void SW_vUIBlurReset(int nPlayer) {
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
            fn_8001EF34(f, v38, v18);
            fn_8001EF34(f, v48, v28);
            Vec_Add(v18, v78, v18);
            Vec_Add(v28, v58, v28);
            Vec_Sub(v18, v28, v8);
            if (v8[0] != 0.0f || v8[1] != 0.0f || v8[2] != 0.0f || v8[3] != 0.0f) {
                v8[3] = 0.0f;
                Vec_Normalize(v8, v8);
            }
            fn_8001EF34(fLen, v8, v8);
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

// Each frame of the backswing (animation 6) and downswing (7): the club trail's colour and alpha
// (blue with the stick left of centre, yellow right, on the backswing; gpSwing's colours fading
// after the ball-hit mark on the downswing) and the club twist (SW_vUIAdjustClub). A human's stick
// is read; a CPU only does this in mode 11 (when fn_8005CC5C() is 8 or 9), with the stick hard to
// one side.
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
    if ((pObj->nAnim != 6 && pObj->nAnim != 7) || gPlayers[nPlayer].nShotKind == 0 || pObj->n1698 != 0 ||
        fn_8001EE90(pObj) == 2) {
        return;
    }
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
                pSw->fAlpha = gpSwing->fFC * (f32)(pSw->nCalibrateX - nStickX) / (f32)pSw->nCalibrateX;
            } else {
                pSw->fRedColor = 0.5f;
                pSw->fGreenColor = 0.5f;
                pSw->fBlueColor = 0.0f;
                pSw->fAlpha = gpSwing->fFC * (f32)(nStickX - pSw->nCalibrateX) /
                              (f32)(0xFF - pSw->nCalibrateX);
            }
            SW_vUIAdjustClub(pObj, pSw, nStickX);
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
            SW_vUIAdjustClub(pObj, pSw, pSw->nBackSwingX);
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
        UI_Obj_RenderBoostUI(gPlayers[nPlayer].nView[0]);
    }
}

// Draw the club's trail (session option a24[7]): a ribbon from the grip through the recorded
// club-head positions, coloured by the trail colour and fading along its length,
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
            fn_80036100((ShaderObject*)gpSwing->mesh[nView], &mesh, 1);
            fn_800360D4(gpSwing->mesh[nView]);
            fn_80012F50(1, 6, 0x80);
            fn_80012F34(1);
            fn_80012EF8();
        }
    }
}

// Twist the club with the stick: how far through the backswing (animation 6, eased in) or the
// downswing (7, eased out) the animation is, times 0.75 and a smoothed copy of the stick's X,
// becomes a Z rotation on the model (fn_80027808), negated when fn_8001EDF4().
void SW_vUIAdjustClub(Character* pObj, SwingData* pSw, int nStickX) {
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
    Quat_EulerAngles(0.0f, 0.0f, fAmount, vRot);
    fn_80027808(pObj->pModel, vRot);
}

// Find the swing's textures by name (all three are in one bank).
void SW_vUIInit(int nPlayer) {
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
void SW_vSetSwingStrength(int nPlayer) {
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
// is held with the stick more than 93 from centre, the boost level rises one a frame to 8 (event
// 0x2D). fPowerBoostDieTime (1/12 s, set when the backswing changes direction) counts down; when
// it runs out the level and the turn angles are cleared.
void SW_vCheckForSwingBoost(int nPlayer) {
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

// No power boost, no spin boost, no back-down timer; the boost display flag back on (fn_8005A788).
void SW_vClearBoosts(int nPlayer) {
    gPlayers[nPlayer].swing.nPowerBoost = 0;
    gPlayers[nPlayer].swing.nSpinBoost = 0;
    gPlayers[nPlayer].swing.fPowerBoostDieTime   = 0.0f;
    fn_8005A788(nPlayer, 1);
    fn_800AE3C4(nPlayer);
}
