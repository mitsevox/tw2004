// Swing.c: the human swing - the meter's error and power, and what the golfer's attributes do
// to them. Named by its assert string at 0x8028118C. CodeWarrior GC/2.5, -O4,p. The formulas
// and tables are written up in docs/gameplay.md.

#include "golfer.h"

// The swing module's state; only the tuning values read here. Set up in Swing_Init.
typedef struct SwingState {
    u8   unk0[4];
    f32  fClubBack;             // 0x004  tuning "clubback"
    f32  fClubDown;             // 0x008  tuning "clubdown"
    f32  fTuningC;              // 0x00C
    u8   unk10[0xAC - 0x10];
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
    if (gSession.bReplay) return;
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
void fn_8000B1D4(int nStream, u32 uSeed);    // seed an RNG stream
void fn_80095744(int nHandle, int nAnim);    // play an animation
void fn_80096690(int nHandle);
void fn_8006BF60(int nPlayer);               // the replay recorder
void fn_8006C300(int nPlayer);
void Swing_FaceVector(int nPlayer, f32* pOut);
f32  Swing_MeterError(int nPlayer);
void Swing_ShapeVector(int nPlayer, f32* pOut);
f32  fn_8005CC84(f32 fTan);                  // atanf
void fn_8005CCA8(int nPlayer);

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


// ---- the swing state stack ----------------------------------------------------------------------
// Each player has a small stack of swing states (ids into gSwingStates, whose entries carry the
// state's callbacks). The top is the current state; -1 is empty.

typedef struct SwingStack {
    u8   nState[5];             // 0x00
    s8   nTop;                  // 0x05  index of the current state, -1 when empty
} SwingStack;

typedef struct SwingStateDef {
    void (*pfnEnter)(int nPlayer);  // 0x00
    void (*pfnUpdate)(int nPlayer); // 0x04
    void (*pfnExit)(int nPlayer);   // 0x08
} SwingStateDef;

extern SwingStack    gSwingStacks[];        // 0x801D5A90
extern SwingStateDef gSwingStates[];        // 0x801883D8
extern u8            gInSwingExit;          // 0x80281E00  set while a state's exit callback runs

// The current swing state, or -1.
int SwingStack_Top(int nPlayer) {
    SwingStack* pStack = &gSwingStacks[nPlayer];
    if (pStack->nTop == -1) return -1;
    return pStack->nState[pStack->nTop];
}

// Pop every state, running each one's exit callback.
void SwingStack_Clear(int nPlayer) {
    s8*         pTop;
    SwingStack* pStack = &gSwingStacks[nPlayer];
    pTop = &pStack->nTop;
    while (*pTop > -1) {
        if (gSwingStates[(s8)pStack->nState[*pTop]].pfnExit != NULL) {
            gInSwingExit = 1;
            gSwingStates[(s8)pStack->nState[*pTop]].pfnExit(nPlayer);
            gInSwingExit = 0;
        }
        (*pTop)--;
    }
}

// Empty in release.
void fn_8005CCA8(int nPlayer) {
}


// ---- state 2: thinking ----------------------------------------------------------------------------

void* fn_80017028(int nView);                // the view
u8    fn_800C7100(void* pView);              // its camera has settled
u8    fn_800FA118(int a, int b);
void  SwingStack_Push(int nState, int nPlayer);  // 0x8005D188
void  AI_ApplyError(int nPlayer);            // Golfer.c

#define CPU_TOLERANCE 0.0025f               // 0.05 yd squared: land within 1.8 in of the target

// A human goes straight on to state 10 (setting up). A CPU rehearses its shot here, one frame
// at a time, and moves on once the rehearsal has settled, at least a second has passed and the
// camera has settled - or when its time is up: 4 s, 1.5 s for a tee shot and 3.5 s after in
// modes 6 and 7, 3..4 s in mode 11. Out of time, the rehearsal is told to stop (best aim found,
// or +25 and a fresh target), then the skill error goes on and it is state 10.
void SwingState02_Update(int nPlayer) {
    f32*    pTime;
    void*   pView;
    u8      bDone;
    f32     fMax;
    f32     fMin;

    pView = fn_80017028(gPlayers[nPlayer].nView0);
    if (!Player_IsCPU(nPlayer)) {
        SwingStack_Push(10, nPlayer);
        return;
    }
    if (Player_IsCPU(nPlayer)) {
        gPlayers[nPlayer].fThinkTime += gSession.fFrameTime;
        pTime = &gPlayers[nPlayer].fThinkTime;
        bDone = AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
        fMin  = 1.0f;
        switch (Game_GetMode()) {
        case 6:
        case 7:
            if (fn_800FA118(0, 0)) return;
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > 0) {
                fMax = 3.5f;
            } else {
                fMax = 1.5f;
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
        if ((bDone && *pTime > fMin && fn_800C7100(pView)) || *pTime > fMax) {
            if (!bDone) {
                gPlayers[nPlayer].nRehearseState = 3;
                AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
            }
            AI_ApplyError(nPlayer);
            SwingStack_Push(10, nPlayer);
        }
    }
}


// ---- starting the swing ---------------------------------------------------------------------------

u8*  fn_80058EB8(int nPlayer, int nController);   // the pad's state: [1] main stick y, [3] C-stick y
f32  fn_8005CB78(int nHandle, int a, int nMark);   // an animation mark's time
void Swing_ResetBoostAndSpin(int nPlayer);
void fn_8006C5E0(void);
void Swing_ClearFrameFlag(int nPlayer);
void fn_80067074(int nPlayer, int nSound, int a, int b);

// The swing is under way: phase 1, the animation started, its three marks read, the 25-sample
// stick history filled with the centre, the spin stick centred.
void Swing_Begin(int nPlayer) {
    Player*    p      = &gPlayers[nPlayer];
    int        nHandle = p->nShotHandle;
    SwingData* pSw    = &p->swing;
    int        i;

    pSw->nPhase = 1;
    fn_80095744(nHandle, 6);
    fn_80096690(nHandle);
    pSw->f10 = 0.0f;
    pSw->f14 = 0.0f;
    pSw->fMark0 = fn_8005CB78(nHandle, 0, 0);
    pSw->fMark1 = fn_8005CB78(nHandle, 0, 1);
    pSw->fMark2 = fn_8005CB78(nHandle, 0, 2);
    for (i = 0; i < 25; i++) {
        pSw->nHistX[i] = pSw->nCentreX;
        pSw->nHistY[i] = pSw->nCentreY;
    }
    pSw->nHistIndex = 0;
    pSw->f604       = 0.0f;
    pSw->b60C       = 0;
    pSw->b60D       = 0;
    Swing_ResetBoostAndSpin(nPlayer);
    pSw->nSpinStickX = 128;
    pSw->nSpinStickY = 128;
    pSw->f628        = 0.0f;
    pSw->f62C        = 0.0f;
}

// Waiting for the backswing. A CPU (or a replay) starts at once. A human starts the frame
// either stick is pulled past 160 of 255 - more than a quarter of its travel - and that stick's
// rest position becomes the centre sample; the C-stick can swing too (bUsingCStick). While
// nothing is pulled the rest positions are held at 128.
int Swing_WaitForBackswing(int nPlayer) {
    Player* p          = &gPlayers[nPlayer];
    int     nController = p->nController;
    int     nHandle    = p->nShotHandle;
    u8*     pPad;
    u8      bMain, bCStick;

    if (Controller_IsCPU(nController) || Game_GetMode() == 10) {
        gPlayers[nPlayer].swing.nPhase = 1;
        Swing_Begin(nPlayer);
        *(u32*)(nHandle + 0x168) &= ~1;
        Swing_ClearFrameFlag(nPlayer);
        return 0;
    }
    pPad    = fn_80058EB8(nPlayer, nController);
    bMain   = pPad[1] <= 0xFF && pPad[1] > 0xA0;
    bCStick = pPad[3] <= 0xFF && pPad[3] > 0xA0;
    if (bMain || bCStick) {
        if (bCStick) {
            gPlayers[nPlayer].swing.nCentreY      = gPlayers[nPlayer].swing.nRestCY;
            gPlayers[nPlayer].swing.nCentreX      = gPlayers[nPlayer].swing.nRestCX;
            gPlayers[nPlayer].swing.bUsingCStick  = 1;
        } else {
            gPlayers[nPlayer].swing.nCentreY      = gPlayers[nPlayer].swing.nRestY;
            gPlayers[nPlayer].swing.nCentreX      = gPlayers[nPlayer].swing.nRestX;
            gPlayers[nPlayer].swing.bUsingCStick  = 0;
        }
        fn_80067074(nPlayer, 0x2C, 0, 0);
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

// The shot object at Player.nShotHandle; only the fields the swing touches.
typedef struct ShotObj {
    u8    unk0[0x38];
    u8*   pView;                // 0x038  -> +0x38 -> a struct with +0x10E4
    u8    unk3C[0x164 - 0x3C];
    u8    anim[4];              // 0x164  the animation: +0x14 is its playback rate
    u32   uFlags;               // 0x168  bit 0x40: the backswing is being backed down
    u8    unk16C[0x17C - 0x16C];
    f32   fAnimTime;            // 0x17C
    u8    unk180[0x5CC - 0x180];
    s32   n5CC;                 // 0x5CC
    u8    unk5D0[0x1624 - 0x5D0];
    u8*   pClip;                // 0x1624 -> +0xCC blend, +0xD4/+0xD8 clips
    f32   f1628;
    f32   f162C;
    f32   f1630;
    f32   f1634;
    f32   v1638[3];             // 0x1638
    f32   f1644;
} ShotObj;

extern f32 gSwingRange[8];                   // 0x801882EC  backswing rate per shot kind: -, 0.85, 0.5, 0.8

u8*  Pad_State(int nPlayer, int nController);          // 0x80058EB8
int  Swing_StickX(int nPlayer, u8* pPad);              // 0x80058F04  main or C-stick by bUsingCStick
int  Swing_StickY(int nPlayer, u8* pPad);              // 0x80058F30
f32  Swing_TopTime(SwingData* pSw);                    // 0x80058E98  fMark1 - 0.0076
f32  Swing_StartTime(SwingData* pSw);                  // 0x80058EA8  fMark0 + 0.0076
f32  ShotObj_GetBlend(ShotObj* pObj);                  // 0x8005CB98  f1628
void ShotObj_Set162C(ShotObj* pObj, f32 f);            // 0x8005CB88
void ShotObj_Set1630(ShotObj* pObj, f32 f);            // 0x8005CBB0
void ShotObj_Set1634(ShotObj* pObj, f32 f);            // 0x8005CBC0
void Anim_SetRate(u8* pAnim, f32 fRate);               // 0x8001F084
void Anim_SetTime(u8* pAnim, f32 fTime);               // 0x8007327C
int  fn_800204A0(u8* pClip, f32* pOut, f32 fTime);
void fn_80017EF4(ShotObj* pObj, int a, f32 f);
void Swing_UpdatePower(int nPlayer);
void Swing_BoostInput(int nPlayer);
u8   fn_80100AF8(void);                                // lesson 5 of mode 11

// Freeze the backswing at the top: the animation stops (rate 0.008) where it is.
void Swing_HoldAtTop(int nPlayer) {
    Player*    p    = &gPlayers[nPlayer];
    SwingData* pSw  = &p->swing;
    ShotObj*   pObj = (ShotObj*)p->nShotHandle;
    pSw->fHoldTime = pSw->fTopTime;
    Anim_SetTime(pObj->anim, pSw->fHoldTime);
    fn_80017EF4(pObj, 0, 0.0f);
    Anim_SetRate(pObj->anim, 0.008f);
    pObj->uFlags |= 0x40;
    pSw->fHoldAtTop = 0.0f;
    pSw->f47C = 0.0f;
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
    ShotObj*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;
    f32        fAnimTime, fTop, fStart, fMag, fTarget, fDelta, fRate, fRange;

    p           = &gPlayers[nPlayer];
    pObj        = (ShotObj*)p->nShotHandle;
    pSw         = &p->swing;
    nController = p->nController;
    if (*(u8**)(pObj->pView + 0x38) != NULL) {
        *(s32*)(*(u8**)(pObj->pView + 0x38) + 0x10E4) = 4;
    }
    if (pObj->pClip != NULL) {
        pObj->f1628 = *(f32*)(pObj->pClip + 0xCC);
    } else {
        pObj->f1628 = 0.0f;
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
        if (fAnimTime >= pSw->fMark0 + fFrac * (pSw->fMark1 - pSw->fMark0)) {
            Anim_SetRate(pObj->anim, 1.0f);
            fn_80095744((int)pObj, 7);
            if (fn_800204A0(*(u8**)(pObj->pClip + 0xD8), pObj->v1638, *(f32*)(*(u8**)(pObj->pClip + 0xD8) + 8) + (pObj->fAnimTime - pSw->fMark0))) {
                pSw->fMark2 = pObj->fAnimTime + (*(f32*)(*(u8**)(pObj->pClip + 0xD4) + 0x24) - pObj->v1638[1]) + pObj->f1644;
            }
            fn_80096690((int)pObj);
            pObj->n5CC = 2;
            pSw->fMark1 = pObj->fAnimTime;
            ShotObj_Set1634(pObj, 0.0f);
            ShotObj_Set1630(pObj, 0.0f);
            ShotObj_Set162C(pObj, 1.4f * ShotObj_GetBlend(pObj));
            pSw->nPhase = 3;
        }
        return 0;
    }
    pPad   = Pad_State(nPlayer, nController);
    nX     = Swing_StickX(nPlayer, pPad);
    nY     = Swing_StickY(nPlayer, pPad);
    fTop   = Swing_TopTime(pSw);
    fStart = Swing_StartTime(pSw);
    if ((nY <= 255 && nY > 96) || (ShotObj_GetBlend(pObj) < 0.1f && nY < 96)) {
        if (nY < 96) {
            fMag = 0.0f;
        } else {
            int nDX = Swing_DeadZone(nX) - 128;
            int nDY = Swing_DeadZone(nY) - 128;
            fMag = (f32)fn_80009680(nDX * nDX + nDY * nDY);
        }
        if (fMag > 100.0f) fMag = 100.0f;
        fRange  = fTop - fStart;
        fTarget = fStart + (fMag / 100.0f) * fRange;
        fDelta  = fTarget - fAnimTime;
        fRate   = 1.0f + (f32)fn_8000AE94(fDelta) / fRange;
        fRate   = fRate * fRate - 1.0f;
        if (fRate >= 1.0f) fRate = 1.0f;
        if (gPlayers[nPlayer].nShotKind == 1 || gPlayers[nPlayer].nShotKind == 2 || gPlayers[nPlayer].nShotKind == 3) {
            fRange = fRange / gSwingRange[gPlayers[nPlayer].nShotKind];
        } else {
            fRange = 1.0f;
        }
        Anim_SetRate(pObj->anim, fRate * fRange);
        if (fMag > 93.0f) {
            Anim_SetRate(pObj->anim, fRange);
        }
        if (fDelta > -0.05f && fDelta < 0.05f) {
            pSw->fTopTime = pObj->fAnimTime;
            pSw->nTopStickY = nY;
            Swing_HoldAtTop(nPlayer);
            pSw->nPhase = 2;
        } else if (pObj->uFlags & 0x40) {
            if (fDelta > 0.0f) pObj->uFlags &= ~0x40;
        } else if (fDelta < 0.0f) {
            pObj->uFlags |= 0x40;
            pSw->fBackDown = 1.0f / 12.0f;
        }
        pSw->nHistX[pSw->nHistIndex] = nX;
        pSw->nHistY[pSw->nHistIndex] = nY;
        pSw->nHistIndex++;
        pSw->nHistIndex %= 25;
        Swing_UpdatePower(nPlayer);
        pSw->nTopX = nX;
        pSw->nTopY = nY;
        Swing_BoostInput(nPlayer);
    } else if (nY <= 96) {
        // The stick has come forward: the top of the backswing is the furthest-back sample.
        int i;
        pSw->nTopY = pSw->nCentreY;
        for (i = 0; i < 25; i++) {
            if (pSw->nHistY[i] > pSw->nTopY) {
                pSw->nTopX = pSw->nHistX[i];
                pSw->nTopY = pSw->nHistY[i];
            }
        }
        Swing_UpdatePower(nPlayer);
        Anim_SetRate(pObj->anim, 1.0f);
        fn_80095744((int)pObj, 7);
        if (fn_800204A0(*(u8**)(pObj->pClip + 0xD8), pObj->v1638, *(f32*)(*(u8**)(pObj->pClip + 0xD8) + 8) + (pObj->fAnimTime - pSw->fMark0))) {
            pSw->fMark2 = pObj->fAnimTime + (*(f32*)(*(u8**)(pObj->pClip + 0xD4) + 0x24) - pObj->v1638[1]) + pObj->f1644;
        }
        fn_80096690((int)pObj);
        pObj->n5CC = 2;
        pSw->fMark1 = pObj->fAnimTime;
        ShotObj_Set1634(pObj, 0.0f);
        ShotObj_Set1630(pObj, 0.0f);
        ShotObj_Set162C(pObj, 1.4f * ShotObj_GetBlend(pObj));
        pSw->nPhase = 3;
        fn_80067074(nPlayer, 0x2F, 0, 0);
        pSw->nImpactX = nX;
        pSw->nImpactY = nY;
        pSw->nImpactX2 = nX;
        pSw->nImpactY2 = nY;
        Swing_ClearFrameFlag(nPlayer);
    }
    return 0;
}


// ---- at the top, and the downswing ----------------------------------------------------------------

void fn_8002792C(u8* p);

// Phase 2, holding at the top. Any stick movement drops back to phase 1 (backing down if it
// came forward). While it is steady the animation waggles +-0.0076 around the top. If the
// stick sits near centre (y at or below 160, x within 64..192) for over 0.1 s the swing is
// abandoned: phase 0, the address animation, sound 9.
int Swing_UpdateAtTop(int nPlayer) {
    Player*    p;
    ShotObj*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;
    f32        fTop, fStart, fAnimTime;

    p           = &gPlayers[nPlayer];
    pObj        = (ShotObj*)p->nShotHandle;
    pSw         = &p->swing;
    nController = p->nController;
    if (*(u8**)(pObj->pView + 0x38) != NULL) {
        *(s32*)(*(u8**)(pObj->pView + 0x38) + 0x10E4) = 4;
    }
    pPad   = Pad_State(nPlayer, nController);
    nX     = Swing_StickX(nPlayer, pPad);
    nY     = Swing_StickY(nPlayer, pPad);
    fTop   = Swing_TopTime(pSw);
    fStart = Swing_StartTime(pSw);
    fAnimTime = pObj->fAnimTime;
    pSw->fHoldAtTop += gSession.fFrameTime;
    if (pSw->nTopStickY != nY) {
        if (nY < gPlayers[nPlayer].swing.nTopStickY) {
            pObj->uFlags |= 0x40;
        } else {
            pObj->uFlags &= ~0x40;
        }
        pSw->fBackDown = 1.0f / 12.0f;
        pSw->nPhase    = 1;
    } else if (((pObj->uFlags & 0x40) && fAnimTime < pSw->fHoldTime) || fAnimTime < fStart) {
        pSw->fHoldTime = 0.0076f + pSw->fTopTime;
        pObj->uFlags &= ~0x40;
    } else if ((!(pObj->uFlags & 0x40) && fAnimTime > pSw->fHoldTime) || fAnimTime > fTop) {
        pSw->fHoldTime = pSw->fTopTime - 0.0076f;
        pObj->uFlags |= 0x40;
    } else if (pSw->nTopStickY <= 160 && nX <= 192 && nX >= 64) {
        pSw->f47C += gSession.fFrameTime;
        if (pSw->f47C > 0.1f) {
            pSw->nPhase = 0;
            fn_80095744((int)pObj, 5);
            Anim_SetRate(pObj->anim, 1.0f);
            fn_80067074(nPlayer, 9, 0, 0);
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
    ShotObj*   pObj;
    SwingData* pSw;
    int        nController;
    int        nX, nY;
    u8*        pPad;

    p           = &gPlayers[nPlayer];
    pSw         = &p->swing;
    pObj        = (ShotObj*)p->nShotHandle;
    nController = p->nController;
    if (*(u8**)(pObj->pView + 0x38) != NULL) {
        *(s32*)(*(u8**)(pObj->pView + 0x38) + 0x10E4) = 4;
    }
    if (!Controller_IsCPU(nController) && Game_GetMode() != 10) {
        int nDX, nDY;
        pPad = Pad_State(nPlayer, nController);
        nX   = Swing_StickX(nPlayer, pPad);
        nY   = Swing_StickY(nPlayer, pPad);
        nDX  = nX - pSw->nCentreX;
        nDY  = nY - pSw->nCentreY;
        if (nY <= 96 && (f32)(nDX * nDX + nDY * nDY) > 300.0f) {
            pSw->nImpactX  = nX;
            pSw->nImpactY  = nY;
            pSw->nImpactX2 = nX;
            pSw->nImpactY2 = nY;
        }
    }
    if (pObj->n5CC < 0) {
        fn_8002792C(*(u8**)(pObj->pView + 0x38));
        gPlayers[nPlayer].swing.nPhase = 5;
        Swing_Launch(nPlayer);
        if (!Controller_IsCPU(nController) && gSession.bReplay == 0) {
            gPlayers[nPlayer].swing.bShotTaken = 1;
        }
        if (Controller_IsPad(nController) && gSession.bReplay == 0) {
            Swing_MisHitRumble(nPlayer);
        }
        if (gPlayers[nPlayer].nShotKind == SHOT_PUTT) {
            fn_80067074(nPlayer, 0x2B, 0, 0);
        }
        return 1;
    }
    return 0;
}


// ---- after impact ---------------------------------------------------------------------------------

u32  fn_800136DC(int nController);           // buttons held
u32  fn_800142AC(int nButton, int a);        // a button's mask
unsigned long long fn_8000BEE4(char* pName);   // a tuning name's 64-bit hash
void fn_800102DC(unsigned long long uHash, void* pSwing, f32* pOut);   // bind a tuning value
extern char lbl_801883A8[];                  // "clubback"
extern char lbl_801883B4[];                  // "clubdown"
extern char lbl_8028119C[];

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
        pFrames     = &p->swing.nRumbleFrames;
        pController = &p->nController;
        fn_800130F8(p->nController, 0);
        fn_80013130(*pController, 0);
        gPlayers[nPlayer].swing.bRumble = 0;
        *pFrames = 0;
    }
}

// Count the mis-hit rumble down and stop it when it runs out.
void Swing_RumbleTick(int nPlayer) {
    if (Player_HasPad(nPlayer)) {
        Player* p = &gPlayers[nPlayer];
        if (p->swing.bRumble) {
            s32* pFrames = &p->swing.nRumbleFrames;
            if (*pFrames <= 0) {
                Swing_RumbleOff(nPlayer);
            } else {
                (*pFrames)--;
            }
        }
    }
}

// Spin, added after the ball is away: with the spin option on and the spin button (mask
// 0x20) held after a real shot, the amount grows by one a frame up to 20 - a third of a second
// for full spin - and the direction is the stick, whenever it is outside the 96..160 dead zone
// (the first press starts it at straight back, 255).
void Swing_SpinInput(int nPlayer) {
    s32*    pController;
    s32*    pAmount;
    u32     uButtons;
    int     nX, nY;
    if (Player_IsCPU(nPlayer)) return;
    if (SESSION_OPTIONS->bSpinEnabled == 0) return;
    pController = &gPlayers[nPlayer].nController;
    uButtons    = fn_800136DC(*pController);
    if (!(uButtons & fn_800142AC(0x20, 0))) return;
    if (gPlayers[nPlayer].swing.bShotTaken == 0) return;
    fn_80067074(nPlayer, 0x2E, 0, 0);
    nX = Swing_StickX(nPlayer, Pad_State(nPlayer, *pController));
    nY = Swing_StickY(nPlayer, Pad_State(nPlayer, *pController));
    pAmount = &gPlayers[nPlayer].swing.nSpinAmount;
    if (*pAmount == 0) {
        gPlayers[nPlayer].swing.nSpinStickX = 128;
        gPlayers[nPlayer].swing.nSpinStickY = 255;
    }
    if (nX < 96 || nX > 160 || nY < 96 || nY > 160) {
        gPlayers[nPlayer].swing.nSpinStickX = nX;
        gPlayers[nPlayer].swing.nSpinStickY = nY;
    }
    if (*pAmount < 20) {
        (*pAmount)++;
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
    gPlayers[nPlayer].swing.n370 = 0;
}

// Bind the swing module's tuning values by name.
void Swing_LoadTuning(void) {
    unsigned long long uHash;
    uHash = fn_8000BEE4(lbl_801883A8);
    fn_800102DC(uHash, gpSwing, &gpSwing->fClubBack);
    uHash = fn_8000BEE4(lbl_801883B4);
    fn_800102DC(uHash, gpSwing, &gpSwing->fClubDown);
    uHash = fn_8000BEE4(lbl_8028119C);
    fn_800102DC(uHash, gpSwing, &gpSwing->fTuningC);
}


// ---- the power meter -----------------------------------------------------------------------------

// Every frame of the backswing: power is the square root of how far along the backswing is
// (a CPU takes it straight), snapping to 1 within 3% of the top. Holding at the top of a full
// backswing on anything but a putt costs (hold - 0.05)^2, at most 0.3.
void Swing_UpdatePower(int nPlayer) {
    f32  fPower = ((ShotObj*)gPlayers[nPlayer].nShotHandle)->f1628;
    f32  fPenalty;
    f32* pPower;
    if (!Player_IsCPU(nPlayer)) {
        fPower = (f32)fn_80009680(fPower);
    }
    if (1.0f - fPower < 0.03f) fPower = 1.0f;
    if (gPlayers[nPlayer].swing.fHoldAtTop < 0.05f || gPlayers[nPlayer].nShotKind == SHOT_PUTT || 1.0f != fPower) {
        fPenalty = 0.0f;
    } else {
        fPenalty = gPlayers[nPlayer].swing.fHoldAtTop - 0.05f;
        fPenalty = -(fPenalty * fPenalty);
    }
    if (fPenalty < -0.3f) fPenalty = -0.3f;
    gPlayers[nPlayer].fPower = fPower + fPenalty;
    pPower = &gPlayers[nPlayer].fPower;
    if (*pPower < 0.0f) *pPower = 0.0f;
}


// ---- the power boost input --------------------------------------------------------------------

void fn_800AE3C4(int nPlayer);

// Every backswing frame for a human with the boost option on: while a boost button (mask 0x1F)
// is held with the stick pulled past 93 of its range, the boost level rises one a frame to 8.
// Once the stick has backed down for 1/12 s (fBackDown, set by the backswing) the level and the
// spin offsets are cleared.
void Swing_BoostInput(int nPlayer) {
    s32* pController;
    s32* pLevel;
    f32* pTimer;
    u32  uButtons;
    int  nX, nY;
    f32  fMag;

    if (Player_IsCPU(nPlayer)) return;
    if (SESSION_OPTIONS->bBoostEnabled == 0) return;
    pController = &gPlayers[nPlayer].nController;
    uButtons    = fn_800136DC(*pController);
    nY   = Swing_StickY(nPlayer, Pad_State(nPlayer, *pController));
    nX   = Swing_StickX(nPlayer, Pad_State(nPlayer, *pController));
    fMag = (f32)fn_80009680((nX - 128) * (nX - 128) + (nY - 128) * (nY - 128));
    if ((uButtons & fn_800142AC(0x1F, 0)) && fMag > 93.0f) {
        pLevel = &gPlayers[nPlayer].swing.nBoostLevel;
        if (*pLevel < 8) {
            (*pLevel)++;
            fn_80067074(nPlayer, 0x2D, 0, 0);
        }
    }
    pTimer = &gPlayers[nPlayer].swing.fBackDown;
    if (*pTimer > 0.0f) {
        *pTimer -= gSession.fFrameTime;
        if (*pTimer <= 0.0f) {
            gPlayers[nPlayer].swing.nBoostLevel = 0;
            gPlayers[nPlayer].swing.f628 = 0.0f;
            gPlayers[nPlayer].swing.f62C = 0.0f;
            fn_800AE3C4(nPlayer);
        }
    }
}


void fn_8005A788(int nPlayer, int a);

// At the start of a swing: no boost, no spin, no back-down timer.
void Swing_ResetBoostAndSpin(int nPlayer) {
    gPlayers[nPlayer].swing.nBoostLevel = 0;
    gPlayers[nPlayer].swing.nSpinAmount = 0;
    gPlayers[nPlayer].swing.fBackDown   = 0.0f;
    fn_8005A788(nPlayer, 1);
    fn_800AE3C4(nPlayer);
}


// ---- swing states: the small ones -----------------------------------------------------------------
// gSwingStates is a table of 27 (enter, update, exit) callbacks; the current state is the top of
// the player's SwingStack. Most of these drive the camera, HUD and sounds around the swing.

void  View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);   // 0x800632E4
void  fn_800E3D38(int nPlayer, int a);
void  fn_8006AD68(int nPlayer);
void  fn_800DF280(void);
void  fn_800E3C0C(int a);
void  fn_80063C90(void* pView);
void  fn_80063BF4(void* pView, f32 f, f32* pVec);
void  fn_800DD904(void);
extern u8  lbl_80281E10;
typedef struct Vec4 { f32 x, y, z, w; } Vec4;
extern Vec4 lbl_80183660;

void SwingState15_Exit(int nPlayer) {
}

void SwingState16_Exit(int nPlayer) {
}

void SwingState17_Exit(int nPlayer) {
}

void SwingState23_Exit(int nPlayer) {
}

void SwingState23_Update(int nPlayer) {
    fn_800DF280();
}

void SwingState22_Exit(int nPlayer) {
    fn_800E3C0C(1);
}

void SwingState10_Exit(int nPlayer) {
    u8* pFlag = &gPlayers[nPlayer].swing.unk630;
    if (*pFlag != 0) {
        *pFlag = 0;
    }
}

void SwingState19_Update(int nPlayer) {
    fn_80063C90(fn_80017028(gPlayers[nPlayer].nView0));
}

void SwingState14_Exit(int nPlayer) {
    if (gPlayers[nPlayer].unkC2E == 0 && lbl_80281E10 == 0) {
        fn_800DD904();
    }
    lbl_80281E10 = 0;
}

void SwingState07_Enter(int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 6, nPlayer, nView);
}

void SwingState08_Enter(int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 7, nPlayer, nView);
    fn_8006AD68(nPlayer);
}

void SwingState05_Enter(int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

void SwingState09_Enter(int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

void SwingState17_Enter(int nPlayer) {
    Vec4 vOffset = lbl_80183660;
    fn_80063BF4(fn_80017028(gPlayers[nPlayer].nView0), 0.5f, (f32*)&vOffset);
}


int   fn_8001707C(int nView);                 // the player a view belongs to
void  fn_8005CFD4(int nPlayer);
void  fn_80067710(int nPlayer, int a, int b);
void  fn_80045494(int a, int nPlayer);
void  fn_800DC524(int a, int nPlayer, f32 f);
void  fn_800C6E14(void);
extern u8 lbl_80281E13;

void SwingState02_Exit(int nPlayer) {
    if (gSession.nGameType != 8 && gPlayers[nPlayer].nLie != LIE_HOLED && !Player_IsCPU(nPlayer)) {
        fn_800E3D38(nPlayer, 1);
    }
}

void SwingState19_Enter(int nPlayer) {
    s32* pView = &gPlayers[nPlayer].nView0;
    if (fn_8001707C(*pView) == nPlayer) {
        int nView = *pView;
        View_SetCamera(fn_80017028(nView), 0x19, nPlayer, nView);
    }
    fn_8006AD68(nPlayer);
}

void SwingState07_Update(int nPlayer) {
    if (!Player_IsCPU(nPlayer) && !(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(5, 1))) {
        fn_8005CFD4(nPlayer);
    } else {
        fn_800E3D38(nPlayer, 0);
    }
}

// Keep a copy of the ball as it lies before the shot.
void SwingState14_Enter(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView0);
    fn_80005628(gPlayers[nPlayer].ballBefore, gPlayers[nPlayer].ball, 0xBC);
    fn_8006AD68(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E13 = 1;
}

void SwingState11_Exit(int nPlayer) {
    fn_80045494(0, nPlayer);
    fn_800DC524(0, nPlayer, 0.0f);
    fn_800C6E14();
    if (gSession.bReplay != 0) {
        fn_80067074(nPlayer, 0x3B, 0, 0);
    } else {
        fn_80067074(nPlayer, 0x3B, 0, 1);
    }
}


s8    fn_8005D2A8(int nPlayer);               // the state below the top of the stack
u8    fn_80063C50(void* pView);
void  fn_80063CBC(void* pView, f32* pVec);
void  fn_80045558(int a, int nPlayer);
void  fn_8006C608(void);
void  fn_80062CE0(int a);
void  fn_800C1790(void* pView, int nPlayer);
void  fn_80062DB8(void* pView, int a);
void  fn_80062D38(int a, int b, int nPlayer);
extern Vec4 lbl_80183670;
extern u8   lbl_80281E11;

// Neither button 2 nor button 3 held: keep polling.
void SwingState05_Update(int nPlayer) {
    s32* pController;
    u32  uMask = fn_800142AC(2, 1);
    pController = &gPlayers[nPlayer].nController;
    if (!(fn_800136DC(*pController) & uMask)) {
        uMask = fn_800142AC(3, 1);
        if (!(fn_800136DC(*pController) & uMask)) {
            fn_8005CFD4(nPlayer);
        }
    }
}

void SwingState17_Update(int nPlayer) {
    Vec4 vOffset = lbl_80183670;
    s32* pView   = &gPlayers[nPlayer].nView0;
    if (fn_80063C50(fn_80017028(*pView))) {
        fn_80063CBC(fn_80017028(*pView), (f32*)&vOffset);
        SwingStack_Push(0x12, nPlayer);
    }
}

void SwingState08_Exit(int nPlayer) {
    int nView;
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
}

void SwingState12_Exit(int nPlayer) {
    s32*  pView  = &gPlayers[nPlayer].nView0;
    void* pViewObj = fn_80017028(*pView);
    fn_80045558(0, nPlayer);
    fn_80045494(0, nPlayer);
    fn_8006C608();
    fn_80062CE0(0);
    Swing_RumbleOff(nPlayer);
    fn_800C1790(fn_80017028(*pView), nPlayer);
    fn_80062DB8(pViewObj, 0);
}

void SwingState03_Enter(int nPlayer) {
    int nView;
    if (gPlayers[nPlayer].nShotKind == SHOT_PUTT) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 2, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 1, nPlayer, nView);
    }
    fn_80062D38(0x62, 1, nPlayer);
    lbl_80281E11 = 1;
}


void  fn_8006AAB4(int nPlayer, int a);
void  fn_80063B98(void* pView, f32 f, f32* pVec);
extern Vec4 lbl_801835F0;
extern u8   lbl_80281E12;

// Leaving the aiming states 4, 5 and 7: back to camera 12 if state 10 is underneath, else 0.
void SwingState04_Exit(int nPlayer) {
    int nView;
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void SwingState05_Exit(int nPlayer) {
    int nView;
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void SwingState07_Exit(int nPlayer) {
    int nView;
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void SwingState13_Enter(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView0);
    fn_80005628(gPlayers[nPlayer].ballBefore, gPlayers[nPlayer].ball, 0xBC);
    fn_8006AD68(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E12 = 1;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        fn_8006AAB4(nPlayer, 2);
    }
}

// Leaving the swing: the ball goes back to where it lay; on cameras 1, 3 and 4 a quarter-second
// camera move.
void SwingState01_Exit(int nPlayer) {
    Vec4  vOffset = lbl_801835F0;
    void* pView   = fn_80017028(gPlayers[nPlayer].nView0);
    int   nCamera;
    fn_80005628(gPlayers[nPlayer].ball, gPlayers[nPlayer].ballBefore, 0xBC);
    nCamera = *(s32*)((u8*)pView + 0x144);
    if (nCamera == 1 || nCamera == 3 || nCamera == 4) {
        fn_80063B98(pView, 0.25f, (f32*)&vOffset);
    }
}


// A view (one per split-screen half); only what the swing states touch.
typedef struct View {
    u8   unk0[0x50];
    f32  f50;                   // 0x050
    f32  f54;                   // 0x054
    f32  f58;                   // 0x058
    u8   unk5C[0x70 - 0x5C];
    s32  nCurCamera;            // 0x070
    u8   unk74[0xC4 - 0x74];
    f32  vC4[4];                // 0x0C4
    u8   unkD4[0x114 - 0xD4];
    f32  f114;                  // 0x114
    f32  f118;                  // 0x118
    u8   unk11C[0x144 - 0x11C];
    s32  nCamera;               // 0x144
    u8   unk148[0x25C - 0x148];
    s32  nSavedCamera;          // 0x25C
} View;

extern u8*  lbl_80281F78;                    // a game object: +0x1C0 non-zero enables camera 19
extern u8   gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
u8    fn_800C4650(View* pView, int nPlayer);
void  fn_800E5714(int a);
void  fn_800E5724(int nPlayer);
void  fn_80017158(int nView);
void  fn_800170C4(int nView, int a);
void  fn_800C7178(View* pView, int nPlayer);
extern Vec4 lbl_80183690;
extern Vec4 lbl_80183610;

void SwingState18_Exit(int nPlayer) {
    s32*  pView = &gPlayers[nPlayer].nView0;
    View* pV;
    Vec4  vOffset;
    pV      = (View*)fn_80017028(*pView);
    vOffset = lbl_80183690;
    fn_80005628(gPlayers[nPlayer].ballBefore, gPlayers[nPlayer].ball, 0xBC);
    if (pV->nCamera == 3) {
        fn_80063B98(fn_80017028(*pView), 0.75f, (f32*)&vOffset);
    }
}

void SwingState04_Enter(int nPlayer) {
    int nView;
    if (*(s32*)(lbl_80281F78 + 0x1C0) != 0) {
        s32* pView = &gPlayers[nPlayer].nView0;
        if (fn_800C4650((View*)fn_80017028(*pView), nPlayer)) {
            nView = *pView;
            View_SetCamera(fn_80017028(nView), 0x13, nPlayer, nView);
            return;
        }
    }
    nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 3, nPlayer, nView);
}

void SwingState03_Exit(int nPlayer) {
    int nView;
    fn_80067074(nPlayer, 0x31, 0, -1);
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E5714(0x10);
    fn_800E5724(nPlayer);
}

// Leaving state 20: every set-up player's view takes this view's camera.
void SwingState20_Exit(int nPlayer) {
    s32*  pView = &gPlayers[nPlayer].nView0;
    View* pSrc;
    View* pDst;
    int   i;
    fn_80017158(*pView);
    pSrc = (View*)fn_80017028(*pView);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        pDst = (View*)fn_80017028(gPlayers[i].nView0);
        fn_800170C4(gPlayers[i].nView0, 1);
        pDst->nCamera = pSrc->nCamera;
        pDst->f118    = pSrc->f118;
        Vec_Copy(pSrc->vC4, pDst->vC4);
        pDst->f114    = pSrc->f114;
    }
    gpGame->pfn24C(nPlayer);
}

void SwingState23_Enter(int nPlayer) {
    Vec4  vOffset = lbl_80183610;
    s32*  pView   = &gPlayers[nPlayer].nView0;
    View* pV      = (View*)fn_80017028(*pView);
    fn_800C7178(pV, nPlayer);
    View_SetCamera(pV, 0x19, nPlayer, *pView);
    if (pV->nCamera == 1 || pV->nCamera == 3 || pV->nCamera == 4) {
        fn_80063B98(pV, 0.25f, (f32*)&vOffset);
    }
}


void  fn_80039344(View* pView, f32 f);
void  fn_800DC9D4(int a);

void SwingState06_Exit(int nPlayer) {
    int nView;
    fn_80005628(gPlayers[nPlayer].ballBefore, gPlayers[nPlayer].ball, 0xBC);
    *(s32*)(gPlayers[nPlayer].ballBefore + 0x94) = -1;     // nobody's ball
    *(s32*)(gPlayers[nPlayer].ballBefore + 0x64) = 1;      // stopped
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void SwingState09_Exit(int nPlayer) {
    s32* pView = &gPlayers[nPlayer].nView0;
    int  nView;
    ((View*)fn_80017028(*pView))->f50 = 1.0f;
    ((View*)fn_80017028(*pView))->f54 = 1.0f;
    ((View*)fn_80017028(*pView))->f58 = 1.0f;
    fn_80039344((View*)fn_80017028(*pView), 0.0f);
    if (fn_8005D2A8(nPlayer) == 10) {
        nView = *pView;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = *pView;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

// Each of the player's two views goes back to its saved camera, unless an earlier view of the
// player's is the same view.
void SwingState21_Exit(int nPlayer) {
    s32*  pViews = &gPlayers[nPlayer].nView0;
    View* pV;
    int   k, j;
    u8    bShared;
    for (k = 0; k < 2; k++) {
        pV      = (View*)fn_80017028(pViews[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == (View*)fn_80017028(pViews[j])) bShared = 1;
        }
        if (!bShared) {
            int nView = pViews[k];
            View_SetCamera(fn_80017028(nView), pV->nSavedCamera, nPlayer, nView);
        }
    }
    fn_800E3D38(nPlayer, 1);
    fn_800DC9D4(0);
}


void  fn_80062B64(int nPlayer);
void  fn_80062B60(int nPlayer);
void  fn_80019234(int nHandle, f32* pPos, int a);
void  fn_8005587C(u8* pBall, f32* pPos);          // put the ball at a point
void  fn_800C7140(int a);
void  fn_80045824(int nPlayer);
extern Vec4 lbl_80183630;

// State 15: plan the next shot from where the ball lies. The ball position is saved, the
// player's ball is moved to the lie, Shot_Plan runs, and the ball position is put back.
void SwingState15_Enter(int nPlayer) {
    Vec4 vOffset = lbl_80183630;
    f32  vSaved[4];
    f32* pBallPos;
    fn_80062B64(nPlayer);
    fn_80062B60(nPlayer);
    fn_80063BF4(fn_80017028(gPlayers[nPlayer].nView0), 0.75f, (f32*)&vOffset);
    pBallPos = &gPlayers[nPlayer].fBallX;
    Vec3Copy(pBallPos, vSaved);
    Vec3Copy((f32*)gPlayers[nPlayer].ball, pBallPos);
    Shot_Plan(nPlayer, 0);
    Vec3Copy(vSaved, pBallPos);
    gPlayers[nPlayer].bPlanReady = 0;
    gPlayers[nPlayer].bRehearsalDone = 0;
}

// State 18: holed out. The ball goes to the pin, the lie becomes LIE_HOLED, animation 12.
void SwingState18_Enter(int nPlayer) {
    u8*  pBall       = gPlayers[nPlayer].ball;
    u8*  pBallBefore = gPlayers[nPlayer].ballBefore;
    int  nHole;
    CourseInfo* pCourse;
    fn_80005628(pBallBefore, pBall, 0xBC);
    fn_8006AD68(nPlayer);
    nHole   = Game_CurrentHole();
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        f32* pPin = (f32*)&pCourse->pin[nHole];
        fn_80019234(gPlayers[nPlayer].nShotHandle, pPin, 1);
        fn_8005587C(pBall, pPin);
        gPlayers[nPlayer].nLie = LIE_HOLED;
        Vec3Copy(pBall, pBallBefore);
    }
    fn_80095744(gPlayers[nPlayer].nShotHandle, 12);
}

// State 21: each of the player's views saves its camera and takes camera 10.
void SwingState21_Enter(int nPlayer) {
    s32*  pViews;
    View* pV;
    int   k, j;
    u8    bShared;
    fn_80067074(nPlayer, 0x4A, 0, -1);
    pViews = &gPlayers[nPlayer].nView0;
    for (k = 0; k < 2; k++) {
        pV      = (View*)fn_80017028(pViews[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == (View*)fn_80017028(pViews[j])) bShared = 1;
        }
        if (!bShared) {
            int nView;
            pV->nSavedCamera = pV->nCurCamera;
            nView = pViews[k];
            View_SetCamera(fn_80017028(nView), 10, nPlayer, nView);
        }
    }
    fn_800C7140(0);
    fn_800E3D38(nPlayer, 0);
    fn_80045824(nPlayer);
    fn_800DC9D4(1);
}


void  fn_800DED60(void);
void  fn_80062BFC(int nHandle);
void  fn_80062BE8(int nHandle);
void  fn_8001C804(int nPlayer, int a, int b);
f32   fn_800D0478(int nPlayer);               // a distance (compared with 250 in Golfer_IsLucky)
void  fn_800E4204(void);
u8*   fn_80016CFC(int nView);
void  Caddie_ApplyTip(int nPlayer);           // Golfer.c

void SwingState16_Enter(int nPlayer) {
    s32* pHandle;
    fn_800DED60();
    pHandle = &gPlayers[nPlayer].nShotHandle;
    fn_80095744(*pHandle, 11);
    fn_80062BFC(*pHandle);
    fn_80062BE8(*pHandle);
    fn_8001C774(*pHandle, gPlayers[nPlayer].nClub);
    fn_8001C724(*pHandle, gPlayers[nPlayer].nShotKind);
    fn_8001C804(nPlayer, 1, 1);
    gPlayers[nPlayer].uFlags |= 8;
    gPlayers[nPlayer].fA64 = fn_800D0478(nPlayer);
    fn_800E4204();
    fn_80016CFC(gPlayers[nPlayer].nView0)[0x275] = 1;
}

// State 6: the putt preview. The caddie's solved shot is taken, the player is made a CPU for
// one call so Swing_Launch fires it clean (no error, no luck swap), and the launched ball is
// kept in ballBefore as nobody's ball - the ghost that draws the preview. Everything the
// player had (shot block, ball, controller) is put back afterwards.
void SwingState06_Enter(int nPlayer) {
    u8    shotSaved[0x5C];
    u8    ballSaved[0xBC];
    u8*   pShot;
    u8*   pBall;
    s32*  pController;
    int   nController;
    int   nView;

    nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 5, nPlayer, nView);
    pShot = (u8*)&gPlayers[nPlayer].nClub;
    fn_80005628(shotSaved, pShot, 0x5C);
    Caddie_ApplyTip(nPlayer);
    pBall = gPlayers[nPlayer].ball;
    fn_80005628(ballSaved, pBall, 0xBC);
    pController  = &gPlayers[nPlayer].nController;
    nController  = *pController;
    *pController = CONTROLLER_CPU;
    Swing_Launch(nPlayer);
    *pController = nController;
    fn_80005628(gPlayers[nPlayer].ballBefore, pBall, 0xBC);
    *(s32*)(gPlayers[nPlayer].ballBefore + 0x94) = -1;
    fn_80005628(pBall, ballSaved, 0xBC);
    fn_80005628(pShot, shotSaved, 0x5C);
    fn_8006BF60(nPlayer);
    fn_800E3D38(nPlayer, 0);
}


u8    fn_80100294(void);                      // in a lesson (mode 11)
u8    fn_800172C4(void* pView);               // the camera move has finished
u8    fn_80014300(u32 uMask);                 // any pad pressed these buttons
void  fn_800A76E4(void);
u8    fn_80062B90(void);
u8    fn_80062B88(int nPlayer);
void  fn_80062B84(int a);
u8    fn_80062B7C(void);

// State 21 (a camera flyover): over when the option skips cameras, the camera finishes, or a
// button is pressed (any pad for a CPU's shot). Lessons wait for the camera.
void SwingState21_Update(int nPlayer) {
    u8  bDone = 0;
    u32 uMask;
    if (SESSION_OPTIONS->bSkipCameras) {
        bDone = 1;
    } else if (fn_80100294()) {
        return;
    }
    if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView0))) {
        bDone = 1;
    } else if (!Player_IsCPU(nPlayer)) {
        uMask = fn_800142AC(0, 0);
        if (fn_800136DC(gPlayers[nPlayer].nController) & uMask) {
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
        fn_8005CFD4(nPlayer);
    }
}

// State 20 (another camera state), the same idea with a confirm step.
void SwingState20_Update(int nPlayer) {
    u8 bDone = 0;
    if (gSession.unk8[0] != 0 && fn_80014300(0)) {
        return;
    }
    if (SESSION_OPTIONS->bSkipCameras) {
        bDone = 1;
    } else if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView0))) {
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
    if (bDone) {
        fn_8005CFD4(nPlayer);
    }
}


void  fn_8006B2C4(int nPlayer, int a);
int   fn_80095780(int nHandle);               // the animation playing
u8    fn_80101738(void);
u8    fn_800C6CB0(void);
void  fn_8006ACF8(int nPlayer, int a);
void  fn_800DB714(int nPlayer);
void  fn_80062DC0(void* pView);
void  fn_80050D2C(u8 bOn);                    // Ball.c: the second sim flag
void  fn_800DCE5C(int nPlayer);
extern Vec4 lbl_80183640;

// State 12: the ball is away. In a replay with the kept ball unset, a special path; otherwise
// the ball as it lies is kept. Camera 14 unless the swing animation is 11 or the view says no.
void SwingState12_Enter(int nPlayer) {
    s32* pView = &gPlayers[nPlayer].nView0;
    void* pV   = fn_80017028(*pView);
    u8*  pBallBefore = gPlayers[nPlayer].ballBefore;
    if (gSession.bReplay != 0 && *(s32*)(pBallBefore + 0x64) == 0) {
        fn_8006B2C4(nPlayer, 1);
    } else {
        fn_80005628(pBallBefore, gPlayers[nPlayer].ball, 0xBC);
    }
    *(s32*)(pBallBefore + 0x94) = -1;
    if (fn_80095780(gPlayers[nPlayer].nShotHandle) != 11 && fn_80101738() && !fn_800C6CB0()) {
        int nView = *pView;
        View_SetCamera(fn_80017028(nView), 0xE, nPlayer, nView);
    }
    fn_8006ACF8(nPlayer, 0);
    fn_800DB714(nPlayer);
    gPlayers[nPlayer].bRehearsalDone = 0;
    Swing_RumbleTick(nPlayer);
    fn_80062DC0(pV);
    fn_80062DB8(pV, 0);
}

// State 15: while the camera flies to the ball, the CPU's rehearsal runs on the player - human
// or not, made a CPU for the call, in fast mode - until it settles. So the club and aim a
// human is handed on arriving at the ball are the rehearsal's solution for the authored aim
// point. When the camera finishes: solved -> camera move and state 16; not yet -> a "still
// working" call and the camera move.
void SwingState15_Update(int nPlayer) {
    Vec4  vOffset = lbl_80183640;
    u8*   pDone;
    s32*  pView;
    f32   vSaved[4];

    fn_80050D2C(1);
    pDone = &gPlayers[nPlayer].bRehearsalDone;
    if (*pDone == 0) {
        s32* pController = &gPlayers[nPlayer].nController;
        f32* pBallPos    = &gPlayers[nPlayer].fBallX;
        int  nController = *pController;
        *pController = CONTROLLER_CPU;
        Vec3Copy(pBallPos, vSaved);
        Vec3Copy((f32*)gPlayers[nPlayer].ball, pBallPos);
        *pDone = AI_RehearseShot(nPlayer, NULL, 1, CPU_TOLERANCE);
        Vec3Copy(vSaved, pBallPos);
        *pController = nController;
    }
    pView = &gPlayers[nPlayer].nView0;
    if (fn_80063C50(fn_80017028(*pView))) {
        if (*pDone != 0) {
            fn_80063CBC(fn_80017028(*pView), (f32*)&vOffset);
            SwingStack_Push(0x10, nPlayer);
            gPlayers[nPlayer].bPlanReady = 1;
        } else {
            if (gPlayers[nPlayer].bPlanReady == 0) {
                fn_800DCE5C(nPlayer);
            }
            fn_80063CBC(fn_80017028(*pView), (f32*)&vOffset);
        }
    }
    fn_80050D2C(0);
}


int   fn_80048574(int nHandle, int a, int b);
int   fn_80062BB0(int nHandle, int a, int b);
void  fn_80062B98(int nHandle, int a, int b);
void  fn_800A5980(u8 nPlayer);
void  fn_8006C28C(int nPlayer, int nController);
extern Vec4 lbl_80183650;

// State 16: a shot the game plays for the player. With animation 11 running and camera 12
// set, the launch is made with the controller set to the CPU for the call (so no meter, no
// error, no luck swap), then it is state 12 with the ball away.
void SwingState16_Update(int nPlayer) {
    Vec4  vOffset = lbl_80183650;
    s32*  pView   = &gPlayers[nPlayer].nView0;
    View* pV      = (View*)fn_80017028(*pView);
    int   nHandle = gPlayers[nPlayer].nShotHandle;
    s32*  pController;
    int   nController;

    if (fn_80095780(nHandle) != 11) return;
    if (pV->nCurCamera != 0xC) {
        View_SetCamera(pV, 0xC, nPlayer, *pView);
        fn_80063B98(pV, 0.75f, (f32*)&vOffset);
    }
    if (pV->nCamera == 1 || pV->nCamera == 4 || pV->nCamera == 3) return;
    if (fn_80048574(nHandle, 0, 2)) {
        if (!fn_80062BB0(nHandle, 0, 2)) return;
        fn_80062B98(nHandle, 0, 2);
        pController  = &gPlayers[nPlayer].nController;
        nController  = *pController;
        *pController = CONTROLLER_CPU;
        Swing_Launch(nPlayer);
        *pController = nController;
        fn_800A5980((u8)nPlayer);
        fn_8006C28C(nPlayer, nController);
        SwingStack_Push(0xC, nPlayer);
    } else {
        pController  = &gPlayers[nPlayer].nController;
        nController  = *pController;
        *pController = CONTROLLER_CPU;
        Swing_Launch(nPlayer);
        *pController = nController;
        fn_800A5980((u8)nPlayer);
        fn_8006C28C(nPlayer, nController);
        SwingStack_Push(0xC, nPlayer);
    }
}


void  fn_800C6010(void* pView, int nPlayer);
void  fn_800C60E8(void* pView, int nPlayer);
void  fn_80068AC8(int nPlayer);

// State 8: a free camera while button 19 is held (release pops the state). Buttons 11/12 and
// 13/14 play the four pan sounds; button 4 switches between two camera modes.
void SwingState08_Update(int nPlayer) {
    u32  uMask = fn_800142AC(0x13, 1);
    s32* pController = &gPlayers[nPlayer].nController;
    if (!(fn_800136DC(*pController) & uMask)) {
        fn_8005CFD4(nPlayer);
    } else {
        uMask = fn_800142AC(0xB, 1);
        if (fn_800136DC(*pController) & uMask) {
            fn_80067074(nPlayer, 0x12, 0, -1);
        } else {
            uMask = fn_800142AC(0xC, 1);
            if (fn_800136DC(*pController) & uMask) {
                fn_80067074(nPlayer, 0x13, 0, -1);
            }
        }
        uMask = fn_800142AC(0xD, 1);
        if (fn_800136DC(*pController) & uMask) {
            fn_80067074(nPlayer, 0x14, 0, -1);
        } else {
            uMask = fn_800142AC(0xE, 1);
            if (fn_800136DC(*pController) & uMask) {
                fn_80067074(nPlayer, 0x15, 0, -1);
            }
        }
    }
    uMask = fn_800142AC(4, 1);
    if (fn_800136DC(*pController) & uMask) {
        fn_800C6010(fn_80017028(gPlayers[nPlayer].nView0), nPlayer);
    } else {
        fn_800C60E8(fn_80017028(gPlayers[nPlayer].nView0), nPlayer);
    }
    fn_80068AC8(nPlayer);
}


void  Caddie_Update(int nPlayer);             // Golfer.c
void  fn_80062C38(void);
void  fn_800DAE84(void);
void  fn_80058FA4(int nPlayer);
void  fn_80068AA8(int nPlayer);
void  fn_800689D4(int nPlayer);
void  fn_800957FC(int nHandle, int a);
void  fn_800957B0(int nHandle, int a);
void  fn_80054A6C(u8* pBall);
f32   fn_800D04AC(int nPlayer);

// State 4: an aiming camera held while button 7 is down (the caddie keeps updating). Buttons
// 9/10 and 11..14 play the pan sounds.
void SwingState04_Update(int nPlayer) {
    u32  uMask;
    s32* pController;
    Caddie_Update(nPlayer);
    uMask       = fn_800142AC(7, 1);
    pController = &gPlayers[nPlayer].nController;
    if (!(fn_800136DC(*pController) & uMask)) {
        fn_8005CFD4(nPlayer);
    } else {
        uMask = fn_800142AC(9, 0);
        if (fn_800136DC(*pController) & uMask) {
            fn_80067074(nPlayer, 0xD, 0, -1);
        } else {
            uMask = fn_800142AC(0xA, 0);
            if (fn_800136DC(*pController) & uMask) {
                fn_80067074(nPlayer, 0xE, 0, -1);
            }
        }
        uMask = fn_800142AC(0xB, 1);
        if (fn_800136DC(*pController) & uMask) {
            fn_80067074(nPlayer, 0x12, 0, -1);
        } else {
            uMask = fn_800142AC(0xC, 1);
            if (fn_800136DC(*pController) & uMask) {
                fn_80067074(nPlayer, 0x13, 0, -1);
            }
        }
        uMask = fn_800142AC(0xD, 1);
        if (fn_800136DC(*pController) & uMask) {
            fn_80067074(nPlayer, 0x14, 0, -1);
        } else {
            uMask = fn_800142AC(0xE, 1);
            if (fn_800136DC(*pController) & uMask) {
                fn_80067074(nPlayer, 0x15, 0, -1);
            }
        }
    }
    fn_80068AC8(nPlayer);
}

// State 2, a shot begins. A CPU takes camera 0 (with gpGame+0x290) or 12 and, with that flag,
// animation 2. The ball is kept as it lies and marked stopped, the live ball's state cleared,
// the player's distance to the pin stored, and the "shot begins" sound played.
void SwingState02_Enter(int nPlayer) {
    int   nView;
    s32*  pHandle;
    u8*   pBall;
    if (Player_IsCPU(nPlayer) && gpGame->n290 != 0) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0, nPlayer, nView);
    } else if (Player_IsCPU(nPlayer)) {
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0xC, nPlayer, nView);
    }
    if (gpGame->n290 != 0) {
        pHandle = &gPlayers[nPlayer].nShotHandle;
        fn_80095744(*pHandle, 2);
        fn_80096690(*pHandle);
    }
    fn_80062C38();
    pBall = gPlayers[nPlayer].ball;
    fn_80005628(gPlayers[nPlayer].ballBefore, pBall, 0xBC);
    *(s32*)(gPlayers[nPlayer].ballBefore + 0x64) = 1;
    *(s32*)(gPlayers[nPlayer].ball + 0x64)       = 0;
    fn_800DAE84();
    fn_80058FA4(nPlayer);
    gPlayers[nPlayer].swing.unk630 = 1;
    fn_80068AA8(nPlayer);
    if (gpGame->b276 != 0) {
        fn_800689D4(nPlayer);
    }
    pHandle = &gPlayers[nPlayer].nShotHandle;
    if (*(s32*)(*pHandle + 0x2C) == 4 || *(s32*)(*pHandle + 0x2C) == 5) {
        fn_800957FC(*pHandle, 1);
        fn_800957B0(*pHandle, 1);
    }
    fn_80054A6C(pBall);
    gPlayers[nPlayer].fA64 = fn_800D04AC(nPlayer);
    fn_80067074(nPlayer, 6, 0, -1);
}
