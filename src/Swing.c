// Swing.c: the human swing - the meter's error and power, and what the golfer's attributes do
// to them. Named by its assert string at 0x8028118C. CodeWarrior GC/2.5, -O4,p. The formulas
// and tables are written up in docs/gameplay.md.

#include "golfer.h"

// The swing module's state; only the tuning values read here. Set up in Swing_Init.
typedef struct SwingState {
    u8   unk0[0xAC];
    f32  fCurveMin;             // 0x0AC  a club's shaping range, by gClubCurve
    f32  fCurveMax;             // 0x0B0
    u8   unkB4[4];
    f32  fKnot1Y;               // 0x0B8  } the backswing-angle response curve: two knots
    f32  fKnot1X;               // 0x0BC  0.4
    f32  fKnot2Y;               // 0x0C0
    f32  fKnot2X;               // 0x0C4  0.6
    f32  fTeeBonus;             // 0x0C8  0.1
    u8   unkCC[0x114 - 0xCC];
    f32  fMaxError;             // 0x114  the meter's largest miss, radians
    f32  fPuttFullPower;        // 0x118  0.75: a putt meter over this counts as full
} SwingState;

extern SwingState* gpSwing;                  // 0x80281188
extern f32         gForgivenessTable[3][27]; // 0x80188168  rows: value at attribute 0 / 100 / 110
extern s32         gBoostSteps[8];           // 0x80188148  power boost per level: 1 2 4 6 9 12 16 20

double fn_8000AE94(double x);                // fabs
f32    fn_80050D34(f32 fDist);               // putt power for a distance
f32    fn_80050F88(f32 fDist, u8* pParams, int nKind, int nClub);   // chip power
void   fn_800130F8(int nPad, int n);         // rumble on
void   fn_80013130(int nPad, int n);         // rumble strength

f32 AI_PowerScale(int nPlayer);              // Golfer.c

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
#define TABLE_PAIR(rowT, rowS, attr, outT, outS)                                                   if ((attr) <= 100) {                                                                               f32 t = (f32)(attr) / 100.0f;                                                                  outT = gForgivenessTable[0][rowT] + t * (gForgivenessTable[1][rowT] - gForgivenessTable[0][rowT]);         outS = gForgivenessTable[0][rowS] + t * (gForgivenessTable[1][rowS] - gForgivenessTable[0][rowS]);     } else {                                                                                           f32 t = ((f32)(attr) - 100.0f) / 10.0f;                                                        outT = gForgivenessTable[1][rowT] + t * (gForgivenessTable[2][rowT] - gForgivenessTable[1][rowT]);         outS = gForgivenessTable[1][rowS] + t * (gForgivenessTable[2][rowS] - gForgivenessTable[1][rowS]);     }

// How much spin the SPIN attribute allows: 0.15 at 0, 0.6 at 100, 1.0 at 110.
f32 Swing_SpinScale(int nSpin) {
    return TABLE_AT(ROW_SPIN, (s8)nSpin);
}

// Add the power boost: the pressed level's step times a per-point scale from POWER BOOST
// (0.005 at 0, 0.010 at 100). Base value only - equipment counts, modifiers do not.
f32 Swing_ApplyPowerBoost(int nPlayer, f32 fPower) {
    int nBoost = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_POWER_BOOST, ATTR_BASE);
    int nLevel = gPlayers[nPlayer].swing.nBoostLevel;
    if (nLevel > 0) {
        f32 fScale = TABLE_AT(ROW_BOOST, nBoost);
        f32 fAdd   = fScale * (f32)gBoostSteps[nLevel - 1];
        fPower += fAdd;
        return fPower;
    }
    return fPower;
}

// Turn the spin input into the shot's spin: stick deflection (-1..1) times the amount asked
// for (0..20, over 20) times the SPIN scale (0.15 at 0, 0.6 at 100, 1.0 at 110).
void Swing_ApplySpin(int nPlayer) {
    SwingData* pSw = &gPlayers[nPlayer].swing;
    int        nSpin;
    f32        fScale;
    if (gSession.bNoSpin) return;
    if (pSw->nSpinAmount == 0) {
        pSw->fSpinX = 0.0f;
        pSw->fSpinY = 0.0f;
        return;
    }
    nSpin       = Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_SPIN, ATTR_TOTAL);
    {
        f32 fX = (f32)(pSw->nSpinStickX - 128) * (1.0f / 128.0f);
        f32 fY = (f32)(pSw->nSpinStickY - 128) * (1.0f / 128.0f);
        pSw->fSpinY = fX * (f32)pSw->nSpinAmount / 20.0f;
        pSw->fSpinX = fY * (f32)pSw->nSpinAmount / 20.0f;
    }
    fScale = Swing_SpinScale(nSpin);
    pSw->fSpinY *= fScale;
    pSw->fSpinX *= fScale;
    pSw->fSpinX *= -1.0f;
}

// Driver from the tee: up to +10% power when the tempo lands in the sweet-spot window.
f32 Swing_TeeSweetSpot(int nPlayer, f32 fPower) {
    Player* p = &gPlayers[nPlayer];
    f32     fT, fHalf;
    if (p->nLie == 0 && p->nClub == 0 && p->swing.fBackAngle < 0.0f) {
        fT = -p->swing.fBackAngle / 1.5707964f;
        if (fT > gpSwing->fKnot1X && fT < gpSwing->fKnot2X) {
            fHalf = (gpSwing->fKnot2X - gpSwing->fKnot1X) * 0.5f;
            return fPower + (1.0f - (f32)fn_8000AE94(fHalf - (fT - gpSwing->fKnot1X)) / fHalf) * gpSwing->fTeeBonus;
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
    f32* pError = &gPlayers[nPlayer].swing.fSwingError;
    f32  fError = *pError;
    int  nRowScale, nRowThresh;
    int  nAttr;
    f32  fThresh, fScale;

    if (gPlayers[nPlayer].nLie == 6 || gPlayers[nPlayer].nLie == 7 || gPlayers[nPlayer].nLie == 8 ||
        gPlayers[nPlayer].nLie == 3 || gPlayers[nPlayer].nLie == 4) {
        nRowScale  = ROW_RECOVERY + 1;
        nRowThresh = ROW_RECOVERY;
        nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL);
    } else {
        switch (gPlayers[nPlayer].nShotKind) {
        case SHOT_PUTT:
            nRowScale  = ROW_PUTTING + 1;
            nRowThresh = ROW_PUTTING;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL);
            if (gPlayers[nPlayer].fDistance < 2.0f) {
                *pError = 0.0f;
                return;
            }
            break;
        case SHOT_CHIP:
            nRowScale  = ROW_APPROACH_B + 1;
            nRowThresh = ROW_APPROACH_B;
            nAttr      = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
            break;
        case SHOT_PITCH:
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
    if (fn_8000AE94(fError) < fThresh) {
        fError *= fScale;
    }
    *pError = fError;
    }
}

// Rumble the pad on a mis-hit: frames = (135 at attribute 0 .. 35 at 100) x |error|, max 30.
void Swing_MisHitRumble(int nPlayer) {
    int nPad = gPlayers[nPlayer].nController;
    int nAttr;
    f32 fScale;
    switch (gPlayers[nPlayer].nShotKind) {
    case SHOT_PUTT:  nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_PUTTING, ATTR_TOTAL); break;
    case SHOT_CHIP:
    case SHOT_PITCH: nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL); break;
    case 5:
    case 6:
    case 7:          nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL); break;
    default:         nAttr = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_BALL_STRIKING, ATTR_TOTAL); break;
    }
    fScale = TABLE_AT(ROW_RUMBLE, nAttr);
    gPlayers[nPlayer].swing.nRumbleFrames = (int)(fScale * fn_8000AE94(gPlayers[nPlayer].swing.fSwingError));
    if (gPlayers[nPlayer].swing.nRumbleFrames > 30) gPlayers[nPlayer].swing.nRumbleFrames = 30;
    if (gPlayers[nPlayer].swing.nRumbleFrames > 0) {
        gPlayers[nPlayer].swing.bRumble = 1;
        fn_800130F8(nPad, 1);
        fn_80013130(nPad, 0xFF);
    }
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
        if (p->nShotKind == SHOT_PUTT && !(p->uFlags & 8)) {
            fPower *= 1.05f;
            if (fPower < 0.1f) fPower = 0.1f;
        }
        goto clamp;
    }
    p      = &gPlayers[nPlayer];
    fPower = p->fPower;
    pPower = &p->fPower;
    fError = fn_8000AE94(p->swing.fSwingError);
    p->swing.fPowerAfterError = Swing_ApplyPowerBoost(nPlayer, fPower) - fError;
    switch (p->nShotKind) {
    case SHOT_PUTT: {
        f32 fDist = p->fDistance < 1.0f ? 1.0f : p->fDistance;
        if (*pPower > gpSwing->fPuttFullPower) *pPower = 1.0f;
        fPower = *pPower * fn_80050D34(fDist);
        Golfer_GetAttribute(p, ATTR_PUTTING, ATTR_TOTAL);
        if (fPower < 0.1f) fPower = 0.1f;
        goto clamp;
    }
    case SHOT_CHIP:
    case SHOT_PITCH: {
        f32 f = *pPower;
        if (p->nShotKind == SHOT_CHIP) {
            f = *pPower * fn_80050F88(p->fDistance, p->ball, SHOT_CHIP, p->nClub);
        }
        fPower = Swing_ApplyPowerBoost(nPlayer, f);
        Golfer_GetAttribute(p, ATTR_APPROACH, ATTR_TOTAL);
        if (fPower < 0.1f) fPower = 0.1f;
        goto clamp;
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
        if (p->nLie == 6 || p->nLie == 7 || p->nLie == 8 || p->nLie == 3 || p->nLie == 4) {
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


// ---- the hit -----------------------------------------------------------------------------------

extern u8 gReplayData[];                     // 0x801D6030  saved seed at +0, swing state at +0x3DC

void Ball_Launch(void* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB);
void Luck_TakePerfectShot(int nPlayer);      // Golfer.c
u8   Player_IsController8(int nPlayer);      // Golfer.c
void fn_8000B1D4(int nStream, u32 uSeed);    // seed an RNG stream
void fn_8006BF60(int nPlayer);
void fn_8006C300(int nPlayer);
void Swing_FaceVector(int nPlayer, f32* pOut);
f32  Swing_MeterError(int nPlayer);
void Swing_ShapeVector(int nPlayer, f32* pOut);
void fn_8005CCA8(int nPlayer);
f32  fn_8005CC84(f32 fTan);                  // atanf

// The ball is struck. A replay (mode 10) reseeds the RNG and restores player 0's swing state;
// otherwise the lucky-shot swap runs first. Then the meter's miss (zero for a CPU or a perfect
// shot), the power, forgiveness, the launch blocks, and the aim - the player's aim plus the
// face vector's angle plus the miss - go to Ball_Launch.
void Swing_Launch(int nPlayer) {
    Player* p;
    f32*    pLaunchB;
    f32*    pLaunchA;
    f32*    pPower;
    u8*     pBall;
    int     nClub, nTrajectory, nKind;
    f32     fAim;

    p = &gPlayers[nPlayer];
    if (Game_GetMode() == 10) {
        fn_8000B1D4(0, *(u32*)gReplayData);
        fn_80005628(&gPlayers[0].swing, gReplayData + 0x3DC, 0x630);
    } else if (gSession.bNoSpin == 0) {
        Luck_TakePerfectShot(nPlayer);
        fn_8006BF60(nPlayer);
    } else {
        fn_8006C300(nPlayer);
    }
    pLaunchA    = p->vLaunchA;
    nClub       = p->nClub;
    nTrajectory = p->nTrajectory;
    nKind       = p->nShotKind;
    pBall       = p->ball;
    Swing_FaceVector(nPlayer, pLaunchA);
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        gPlayers[nPlayer].swing.fSwingError = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fSwingError = Swing_MeterError(nPlayer);
    }
    pPower  = &gPlayers[nPlayer].swing.fLaunchPower;
    *pPower = Swing_ComputePower(nPlayer);
    Swing_ApplyForgiveness(nPlayer);
    if (gPlayers[nPlayer].nShotKind == SHOT_PUTT && gPlayers[nPlayer].fDistance < 2.0f) {
        gPlayers[nPlayer].vLaunchA[0] = 0.0f;
        gPlayers[nPlayer].vLaunchA[1] = 0.0f;
        gPlayers[nPlayer].vLaunchA[2] = 1.0f;
        gPlayers[nPlayer].vLaunchA[3] = 0.0f;
    }
    pLaunchB = p->vLaunchB;
    Swing_ShapeVector(nPlayer, pLaunchB);
    gPlayers[nPlayer].swing.fLaunchAX = gPlayers[nPlayer].vLaunchA[0];
    if (Player_IsController8(nPlayer)) {
        fn_8005CCA8(nPlayer);
        nTrajectory = p->nTrajectory;
        nClub       = p->nClub;
        nKind       = p->nShotKind;
    }
    if (0.0f == gPlayers[nPlayer].vLaunchA[2]) {
        fAim = p->fAim + gPlayers[nPlayer].swing.fSwingError;
    } else {
        fAim = p->fAim + fn_8005CC84(gPlayers[nPlayer].vLaunchA[0] / gPlayers[nPlayer].vLaunchA[2]);
        fAim = gPlayers[nPlayer].swing.fSwingError + fAim;
    }
    while (fAim < -PI) fAim += 2 * PI;
    while (fAim > PI) fAim -= 2 * PI;
    Ball_Launch(pBall, nClub, nKind, *pPower, fAim, nTrajectory, pLaunchA, pLaunchB);
}


// ---- the meter's miss ----------------------------------------------------------------------------

extern f32 gPuttXScale[8];                   // 0x801882AC  per shot kind: 0.03 for a putt, 0.2 otherwise
extern f32 gSwingXScale[8];                  // 0x801882CC  the same values again

void Vec_Sub(f32* pA, f32* pB, f32* pOut);   // 0x8005CBF4  a - b
void Vec_Add(f32* pA, f32* pB, f32* pOut);   // 0x8005CBD0  a + b
f32  Rand_Float(int nStream);                // 0x8000B1B8  0..1

// The analog swing's error: the angle between the stick's path back (centre to the top of the
// backswing) and its path through (top to impact). Both x samples get a random +-15 (of a
// +-128 stick) before the x axis is scaled by 0.2 - 0.03 on a putt - so the wobble is worth up
// to about 1.7 degrees on a full shot; then atan of the deviation, clamped to the meter's
// maximum (gpSwing->fMaxError).
f32 Swing_MeterError(int nPlayer) {
    f32 fTopX    = gPlayers[nPlayer].swing.nTopX;
    f32 fTopY    = gPlayers[nPlayer].swing.nTopY;
    f32 fImpactX = gPlayers[nPlayer].swing.nImpactX;
    f32 fImpactY = gPlayers[nPlayer].swing.nImpactY;
    f32 fCentreX = gPlayers[nPlayer].swing.nCentreX;
    f32 fCentreY = gPlayers[nPlayer].swing.nCentreY;
    f32 vBack[4], vThrough[4], vDiff[4], vDir[4];
    f32 fAngle, fMax;

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
    if (vDir[2] != 0.0f) {
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

extern s32 gClubCurve[NUM_CLUBS];            // 0x80183578  per club, 0..26: how much it can shape

void AI_FaceVector(int nPlayer, f32* pOut);  // Golfer.c

// How far the face turns for a backswing angled fBackAngle off vertical: the angle as a fraction
// of a quarter turn goes through a three-piece curve (knots at gpSwing 0xB8..0xC4), scaled by
// the club's shaping range (fCurveMin..fCurveMax by gClubCurve/26) and a quarter turn.
f32 Swing_CurveAngle(s32* pClub, f32 fBackAngle) {
    f32 fOut   = 0.0f;
    f32 fT     = fBackAngle / (PI / 2);
    f32 fRange = gpSwing->fCurveMin + ((f32)gClubCurve[*pClub] / 26.0f) * (gpSwing->fCurveMax - gpSwing->fCurveMin);

    fT = (f32)fn_8000AE94(fT);
    if (fT < gpSwing->fKnot1X) {
        fOut = gpSwing->fKnot1Y * fT / gpSwing->fKnot1X;
    } else {
        fOut += gpSwing->fKnot1Y;
        if (fT < gpSwing->fKnot2X) {
            fOut += (gpSwing->fKnot2Y - gpSwing->fKnot1Y) * ((fT - gpSwing->fKnot1X) / (gpSwing->fKnot2X - gpSwing->fKnot1X));
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
// square face. On a full shot the backswing's sideways angle (kept in fBackAngle for the tee
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
    fCentreX = p->swing.nCentreX;
    fTopX    = p->swing.nTopX;
    fTopY    = p->swing.nTopY;
    if (0.0f == fCentreX) fCentreX = 1.0f;
    fDY = fTopY - (f32)gPlayers[nPlayer].swing.nCentreY;
    if (0.0f == fDY) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    } else if (gPlayers[nPlayer].nShotKind != SHOT_PUTT) {
        fAngle = fn_8005CC84((fTopX - fCentreX) / fDY);
        gPlayers[nPlayer].swing.fBackAngle = fAngle;
        fAngle = Swing_CurveAngle(&gPlayers[nPlayer].nClub, fAngle);
        fSin   = fn_800095F0(fAngle);
        fCos   = fn_80009638(fAngle);
        pOut[0] = -fSin;
        pOut[1] = 0.0f;
        pOut[2] = fCos;
        pOut[3] = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fBackAngle = 0.0f;
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
