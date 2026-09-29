// Swing.c (EA's name: its file string at 0x8028118C, in SW_vInitModule's allocations; TW07 keeps
// it as Golf/AI/Swing.c, SW_ prefix): the stick swing. Its states (gSwingPhaseFns, by
// SwingData.nState: waiting for the backswing, the backswing, holding at the top, the downswing,
// after impact); the impact (SW_vImpact: clubface and stroke direction, the miss, the power, then
// Physics_ShotImpact); what the golfer's attributes do to the miss, the power, the rumble and the
// spin (gSwingAttributeTable); the power boost and spin inputs; the club trail and IK drawn during
// the swing. The last functions are out-of-line copies of small header helpers
// (Character_GetTagTime, Vec_Add, RenderState_SetBankTexture, Math_Atan ...). The golfer state
// engine lives in StateGolfer.c and stateFunc.c. CodeWarrior GC/2.5, -O4,p. The formulas and tables
// are written up in docs/gameplay.md.

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
    u8   unk11C[0x128 - 0x11C];
} SwingState;

// Rows of gSwingAttributeTable, in pairs (threshold, scale) unless noted.
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
         ? gSwingAttributeTable[0][row] + ((f32)(attr) / 100.0f) *                                \
               (gSwingAttributeTable[1][row] - gSwingAttributeTable[0][row])                         \
         : gSwingAttributeTable[1][row] + (((f32)(attr) - 100.0f) / 10.0f) *                      \
               (gSwingAttributeTable[2][row] - gSwingAttributeTable[1][row]))

// Two rows at once (threshold and scale), one branch on the attribute.
#define TABLE_PAIR(rowT, rowS, attr, outT, outS)                                               \
    if ((attr) <= 100) {                                                                       \
        f32 t = (f32)(attr) / 100.0f;                                                          \
        outT = gSwingAttributeTable[0][rowT] +                                                    \
               t * (gSwingAttributeTable[1][rowT] - gSwingAttributeTable[0][rowT]);                  \
        outS = gSwingAttributeTable[0][rowS] +                                                    \
               t * (gSwingAttributeTable[1][rowS] - gSwingAttributeTable[0][rowS]);                  \
    } else {                                                                                   \
        f32 t = ((f32)(attr) - 100.0f) / 10.0f;                                                \
        outT = gSwingAttributeTable[1][rowT] +                                                    \
               t * (gSwingAttributeTable[2][rowT] - gSwingAttributeTable[1][rowT]);                  \
        outS = gSwingAttributeTable[1][rowS] +                                                    \
               t * (gSwingAttributeTable[2][rowS] - gSwingAttributeTable[1][rowS]);                  \
    }

// port: at these calls the original sign-extends Golfer_GetAttribute's result as if it returned s8,
//       while its definition in Golfer.c returns an int; the cast reproduces that. Calling through
//       the mismatched type is undefined in standard C: a port writes (s8)Golfer_GetAttribute(...).
#define GOLFER_GET_ATTRIBUTE_S8(p, nAttr, nMode) \
    (((s8 (*)(Player*, int, int))Golfer_GetAttribute)(p, nAttr, nMode))

extern f32           __float_max[];              // FLT_MAX

void  SW_vGetClubDirection(int nPlayer, f32* pOut);
f32   SW_vCalculateMishitAngle(int nPlayer);
void  SW_vGetStrokeDirection(int nPlayer, f32* pOut);
f32   Math_Atan(f32 fTan);                     // atanf
void  SW_vImpactForController8(int nPlayer);
void  Vec_Sub(f32* pA, f32* pB, f32* pOut);      // 0x8005CBF4  a - b
void  Vec_Add(f32* pA, f32* pB, f32* pOut);      // 0x8005CBD0  a + b
void  SW_vUIBlurReset(int nPlayer);
void  SD_FreeShaderObject(void* p);
void  SW_vUIInit(int nPlayer);
void  SD_InitShaderObject(void* p, int a, s32* pDesc);
void  SW_vSetDisplayBoostUI(int nPlayer, int bDisplay);
void  UI_Obj_ResetBoostRings(int nPlayer);
u8*   SW_vGetStickInfo(int nPlayer, int nController);   // [0]/[1] C stick x/y, [2]/[3] main stick
int   SW_vGetStickX(int nPlayer, u8* pPad);             // main or C stick by nStickUsed
int   SW_vGetStickY(int nPlayer, u8* pPad);
f32   SW_vGetControllerTopOfSwing(SwingData* pSw);      // fTimeSwingTop - 0.0076
f32   SW_vGetControllerStartOfSwing(SwingData* pSw);    // fTimeSwingStart + 0.0076
int   SKA_SampleBlendClip(Clip* pBlend, f32* pOut, f32 fTime);   // samples pBlend->pD8 at fTime
void  Character_UpdateAnimation(Character* pObj, int a, f32 f);
void  SW_vSetSwingStrength(int nPlayer);
void  SW_vCheckForSwingBoost(int nPlayer);
void  SW_vUIAdjustClub(Character* pObj, SwingData* pSw, int nStickX);
int   GM_GetCurrentLesson(void);
void  UI_Obj_RenderBoostUI(int nView);
void  SD_DrawShaderObject(u8* pMesh);
void  SW_vStateInitBackSwingFigit(int nPlayer);
void  SW_vImpact(int nPlayer);
f32   SW_vCalculateShotPower(int nPlayer);
f32   SW_fCalculateSliceAmount(s32* pClub, f32 fBackAngle);
f32   SW_fPowerAdjustForDraw(int nPlayer, f32 fPower);
f32   SW_fPowerBoostAdjustment(int nPlayer, f32 fPower);
void  SW_vUpdateSpinControl(int nPlayer);
void  SW_vCalculateSpinFactor(int nPlayer);
void  SW_vAdjustMishitFromAttribute(int nPlayer);
void  SW_vAdjustVibrationFromAttribute(int nPlayer);
f32   Vec4_LengthSqClamped(f32* pV);
u8    SW_vStateIdleSwing(int nPlayer);
u8    SW_vStateBackSwing(int nPlayer);
u8    SW_vStateBackSwingFigit(int nPlayer);
u8    SW_vStateDownSwing(int nPlayer);
u8    SW_vStateThroughSwing(int nPlayer);
u8    SW_vStatePostSwing(int nPlayer);
u8    SW_vStateCancelSwing(int nPlayer);

// The swing module's data; always reached through gpSwing.
SwingState gSwingState;

// 0x80183578  per club, how far its clubface can turn, out of 26: 0 puts it at gpSwing->fCurveMin,
// 26 would put it at fCurveMax (SW_fCalculateSliceAmount)
const s32 gClubCurve[CLUB_MAX_e] = {
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 17
};

SwingState* gpSwing = &gSwingState;               // 0x80281188

s32 gBoostSteps[8] = {1, 2, 4, 6, 9, 12, 16, 20};  // 0x80188148  power boost per level: 1 2 4 6 9 12 16 20

// 0x80188168  values that follow a golfer attribute, one column per ROW_ (threshold and scale
// pairs for the miss and the power lost to it, then the power boost, rumble and spin scales); the
// three rows are the value at attribute 0 / 100 / 110 (TABLE_AT, TABLE_PAIR interpolate)
f32 gSwingAttributeTable[3][27] = {
    {0.1f, 0.85f, 0.125f, 0.825f, 0.13f, 0.8f, 0.135f, 0.775f, 0.1f,
     0.9f, 0.1f, 0.85f, 0.1f, 0.85f, 0.1f, 0.85f, 0.1f, 0.85f,
     0.1f, 0.85f, 0.0f, 1.0f, 3.14f, 0.0f, 0.005f, 135.0f, 0.15f},
    {0.415f, 0.18f, 0.42f, 0.175f, 0.425f, 0.17f, 0.43f, 0.165f, 0.435f,
     0.125f, 0.415f, 0.2f, 0.415f, 0.125f, 0.415f, 0.18f, 0.415f, 0.18f,
     0.415f, 0.125f, 0.436f, 0.125f, 3.14f, 0.0f, 0.01f, 35.0f, 0.6f},
    {0.435f, 0.165f, 0.445f, 0.145f, 0.465f, 0.125f, 0.485f, 0.115f, 0.5235f,
     0.1f, 0.5235f, 0.15f, 0.5235f, 0.1f, 0.5235f, 0.1f, 0.5235f, 0.1f,
     0.5235f, 0.1f, 0.5235f, 0.1f, 3.14f, 0.0f, 0.011f, 30.0f, 1.0f},
};

// 0x801882AC  per shot kind, the weight of the stick's sideways offset in a putt's clubface
// (SW_vGetClubDirection; only the putt's 0.03 is read)
f32 gPuttXScale[8] = {0.03f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f};

// 0x801882CC  per shot kind, the weight of the stick's sideways travel in the miss
// (SW_vCalculateMishitAngle): 0.03 for a putt, 0.2 otherwise
f32 gSwingXScale[8] = {0.03f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f};

// 0x801882EC  per shot kind, seconds the backswing animation takes with the stick fully back
// (SW_vStateBackSwing scales its time by the animation's backswing length over this): drive
// 0.85, chip 0.5, pitch 0.8; other kinds are not read
f32 gBackswingTime[8] = {0.0f, 0.85f, 0.5f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f};

// 0x8018830C  the power boost display's colour per level (RGB; the fourth value is not read):
// grey for levels 1..3, then redder, plain (half-bright) red from level 6 (uiObject.c
// UI_Obj_RenderBoostUI)
f32 gBoostLevelColours[8][4] = {
    {0.5f, 0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f, 0.5f},
    {0.5f, 0.3f, 0.3f, 0.5f},
    {0.5f, 0.15f, 0.15f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f},
};

// The swing's states, by SwingData.nState; the swing's update runs the current one.
u8 (*gSwingPhaseFns[7])(int nPlayer) = {
    SW_vStateIdleSwing,    SW_vStateBackSwing, SW_vStateBackSwingFigit, SW_vStateDownSwing,
    SW_vStateThroughSwing, SW_vStatePostSwing, SW_vStateCancelSwing,
};

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802835B0), before the 40.0f SW_vInitModule uses first; its body is unknown.
static f32 Swing_StrippedFn(f32 x) {
    return x + 1.0f;
}

// ---- starting the swing ---------------------------------------------------------------------------

// Set the swing module up when the in-game code starts (GO_vInitIG): player 1's base ratings (as
// floats) and the fixed tuning values into gpSwing, the two club-trail meshes (one per view), the
// trail textures (SW_vUIInit, called per player though gpSwing keeps one set), each player's boosts
// cleared and trail colour reset (grey, alpha 0), and per view the trail's three vertex buffers
// (positions, colours, texture coordinates).
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
    SD_InitShaderObject(gpSwing->mesh[0], 0, desc);
    SD_InitShaderObject(gpSwing->mesh[1], 0, desc);
    for (i = 0; i < gSession.nNumPlayers; i++) {
        SW_vUIInit(i);
        SW_vClearBoosts(i);
        gPlayers[i].swing.fBlueColor = 0.5f;
        gPlayers[i].swing.fRedColor = 0.5f;
        gPlayers[i].swing.fGreenColor = 0.5f;
        gPlayers[i].swing.fAlpha = 0.0f;
    }
    for (i = 0; i < 2; i++) {
        gpSwing->p94[i] = StaticMem_Alloc(0x138, 2, 0x10, "Swing.c", 555);
        gpSwing->p9C[i] = StaticMem_Alloc(0x68, 2, 0x10, "Swing.c", 556);
        gpSwing->pA4[i] = StaticMem_Alloc(0xD0, 2, 0x10, "Swing.c", 557);
    }
}

// Close the swing module (the in-game shutdown in gomainloop): free what SW_vInitModule made, the
// two trail meshes and the three trail buffers per view.
void SW_vCloseModule(void) {
    int i;
    SD_FreeShaderObject(gpSwing->mesh[0]);
    SD_FreeShaderObject(gpSwing->mesh[1]);
    for (i = 0; i < 2; i++) {
        StaticMem_Free(gpSwing->p94[i]);
        StaticMem_Free(gpSwing->p9C[i]);
        StaticMem_Free(gpSwing->pA4[i]);
    }
}

// At the end of each hole: hide every player's boost display (SW_vSetDisplayBoostUI(i, 0)).
void SW_vDeInitForHole(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        SW_vSetDisplayBoostUI(i, 0);
    }
}

// The animation time a stick pulled fully back maps to: the swing animation's top-of-backswing mark
// less 0.0076, so a full pull stops just short of the top. Used by the backswing and the hold at
// the top.
f32 SW_vGetControllerTopOfSwing(SwingData* pSw) {
    return pSw->fTimeSwingTop - 0.0076f;
}

// The animation time a stick at rest maps to: the swing animation's start-of-backswing mark plus
// 0.0076, just after the start.
f32 SW_vGetControllerStartOfSwing(SwingData* pSw) {
    return 0.0076f + pSw->fTimeSwingStart;
}

u8 lbl_80281194[8] = {0x80, 0x80, 0x80, 0x80};    // a neutral pad: both sticks centred (0x80)

// The sticks of pad nController as bytes 0..255 (Input_sGetStickInfo: [0] and [1] the C stick's x
// and y, [2] and [3] the main stick's; y grows as the stick is pulled back), or a neutral pad (both
// sticks centred at 128) when there is none or Lessons_UseNeutralPad() says so (it always returns 0 in this
// build). nPlayer is unused.
u8* SW_vGetStickInfo(int nPlayer, int nController) {
    u8* pPad = Input_sGetStickInfo(nController);
    if (pPad == NULL || Lessons_UseNeutralPad()) {
        return lbl_80281194;
    }
    return pPad;
}

// The swing stick's x (0..255, 128 centre): the main stick (pad byte 2) when the swing was started
// with it (nStickUsed), else the C stick (byte 0).
int SW_vGetStickX(int nPlayer, u8* pPad) {
    if (gPlayers[nPlayer].swing.nStickUsed != 0) {
        return pPad[2];
    }
    return pPad[0];
}

// The swing stick's y (0..255, 128 centre, larger is pulled further back): the main stick (pad byte
// 3) when the swing was started with it (nStickUsed), else the C stick (byte 1).
int SW_vGetStickY(int nPlayer, u8* pPad) {
    if (gPlayers[nPlayer].swing.nStickUsed != 0) {
        return pPad[3];
    }
    return pPad[1];
}

// Each frame of the swing (STATEFUNC_SwingUpdate, STATEFUNC_SimulateUpdate, GameMode8): run the
// player's current swing state, gSwingPhaseFns[nState] (0 idle, 1 backswing, 2 hold at the top, 3
// downswing, 4 through-swing, 5 post-swing, 6 cancelled). True on the frame the ball is struck.
u8 SW_vUpdateSwing(int nPlayer) {
    return gSwingPhaseFns[gPlayers[nPlayer].swing.nState](nPlayer);
}

// Reset a player's swing to state 0 (waiting for the backswing): the sticks' rest positions centred
// (128), character animation 5 asked for, rumble stopped, boosts cleared, the spin stick centred,
// the turn angles and the club twist smoothing (f10, f14) zeroed, and the boost display reset.
// Called as the golfer steps up (STATEFUNC_SwingInit, STATEFUNC_ShotSetupInit) and by the lessons
// mode.
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
    UI_Obj_ResetBoostRings(nPlayer);
}

// Start the backswing: state 1, character animation 6 (the backswing) asked for and the animation
// state updated, the club twist smoothing zeroed, the animation's three marks read (start of
// backswing, top, ball hit: fTimeSwingStart, fTimeSwingTop, fTimeBallHit), the 25-sample stick
// history filled with the centre sample (nCalibrateX/Y), spin cleared (bSpun, bSpinning), boosts
// cleared, the spin stick centred and the turn angles zeroed.
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
    pSw->fTimeSwingStart = Character_GetTagTime(pChar, 0);
    pSw->fTimeSwingTop = Character_GetTagTime(pChar, 1);
    pSw->fTimeBallHit = Character_GetTagTime(pChar, 2);
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

// Swing state 0, waiting for the backswing; always returns false. A CPU, or any player in the
// replay mode (10), starts at once: state 1, SW_vStateInitBackSwing, character flag 1 cleared, the
// trail emptied. A human starts on the frame either stick's y is pulled back past 160 (of 0..255,
// centre 128): the stick used is kept in nStickUsed (the main stick when both are pulled), its rest
// position becomes the centre sample (nCalibrateX/Y), event 0x2C fires, the backswing starts, the
// trail is emptied and replay recording starts. While neither is pulled every rest position is held
// at 128.
u8 SW_vStateIdleSwing(int nPlayer) {
    Player* p          = &gPlayers[nPlayer];
    int     nController = p->nController;
    Character* pChar   = p->pChar;
    u8*     pPad;
    u8      bCStick, bMain;     // each stick's y pulled back past 160 (pad bytes 1 and 3)

    if (Controller_IsCPU(nController) || Game_GetMode() == 10) {
        gPlayers[nPlayer].swing.nState = 1;
        SW_vStateInitBackSwing(nPlayer);
        pChar->uFlags &= ~1;
        SW_vUIBlurReset(nPlayer);
        return 0;
    }
    pPad    = SW_vGetStickInfo(nPlayer, nController);
    bCStick = pPad[1] <= 0xFF && pPad[1] > 0xA0;
    bMain   = pPad[3] <= 0xFF && pPad[3] > 0xA0;
    if (bCStick || bMain) {
        if (bMain) {
            gPlayers[nPlayer].swing.nCalibrateY = gPlayers[nPlayer].swing.nRestCY;
            gPlayers[nPlayer].swing.nCalibrateX = gPlayers[nPlayer].swing.nRestCX;
            gPlayers[nPlayer].swing.nStickUsed  = 1;
        } else {
            gPlayers[nPlayer].swing.nCalibrateY = gPlayers[nPlayer].swing.nRestY;
            gPlayers[nPlayer].swing.nCalibrateX = gPlayers[nPlayer].swing.nRestX;
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

// One stick axis through the dead zone: 96..160 reads as centre (128); forward of it runs
// smoothly down to 1, back of it jumps to ~179 and runs to 255.
static inline int Swing_DeadZone(int v) {
    if (v > 160) return 127 + (v - 96) * 128 / 159;
    if (v < 96) return 128 - (96 - v) * 127 / 96;
    return 128;
}

// Swing state 1, the backswing; always returns false. A CPU (or the replay mode, 10) lets the
// backswing animation run until it is 98% of the way from the start mark to the top (65% in lesson
// 5 of the lessons mode) and then starts the downswing. A human's stick drives it: the stick's
// distance from centre, dead-zoned (Swing_DeadZone) and capped at 100, says how far along the
// backswing the animation should be, and the animation chases that at a rate that grows with the
// gap (for shot kinds 1-3 scaled by the backswing's length over gBackswingTime[kind]), playing
// backwards when it must back down (character flag 0x40, which also starts the boost's 1/12 s die
// timer). Within 0.05 of the target it settles into the hold at the top (state 2). Each frame the
// stick is back, its sample goes into the 25-sample ring and is the top for now, and the power
// (SW_vSetSwingStrength) and the boost input (SW_vCheckForSwingBoost) are updated. Once the stick
// comes forward to 96 or less (and the backswing is at least 0.1 along), the top is the ring's
// furthest-back sample, the downswing animation (7) starts, event 0x2F fires, and it is state 3
// with the impact sample seeded from this frame.
u8 SW_vStateBackSwing(int nPlayer) {
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
        pObj->pModel->pSkel->nShoulderFrames = 4;
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
        if (Lessons_IsShortBackswingLesson()) {
            fFrac = 0.65f;
        } else {
            fFrac = 0.98f;
        }
        if (fAnimTime >= pSw->fTimeSwingStart + fFrac * (pSw->fTimeSwingTop - pSw->fTimeSwingStart)) {
            SKATime_SetTimeScale(pObj->anim, 1.0f);
            fn_80095744(pObj, 7);
            if (SKA_SampleBlendClip(pObj->pBlend, pObj->afSwingTop,
                            pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
                pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pEvents[2].fTime - pObj->afSwingTop[1])
                                  + pObj->afSwingTop[3];
            }
            CharacterState_UpdateSKAState(pObj);
            pObj->nClampEvent = 2;
            pSw->fTimeSwingTop = pObj->fAnimTime;
            Character_Set1634(pObj, 0.0f);
            Character_Set1630(pObj, 0.0f);
            Character_Set162C(pObj, 1.4f * Character_GetBackswing(pObj));
            pSw->nState = 3;
        }
        return 0;
    }
    pPad   = SW_vGetStickInfo(nPlayer, nController);
    nX     = SW_vGetStickX(nPlayer, pPad);
    nY     = SW_vGetStickY(nPlayer, pPad);
    fTop   = SW_vGetControllerTopOfSwing(pSw);
    fStart = SW_vGetControllerStartOfSwing(pSw);
    if ((nY <= 255 && nY > 96) || (Character_GetBackswing(pObj) < 0.1f && nY < 96)) {
        if (nY < 96) {
            fMag = 0.0f;
        } else {
            int nDX = Swing_DeadZone(nX);
            int nDY = Swing_DeadZone(nY);
            fMag = (f32)Math_Sqrt((nDX - 128) * (nDX - 128) + (nDY - 128) * (nDY - 128));
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
                fRange = fRange / gBackswingTime[nKind];
            } else {
                fRange = 1.0f;
            }
        }
        SKATime_SetTimeScale(pObj->anim, fRate * fRange);
        if (fMag > 93.0f) {
            SKATime_SetTimeScale(pObj->anim, fRange);
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
        SKATime_SetTimeScale(pObj->anim, 1.0f);
        fn_80095744(pObj, 7);
        if (SKA_SampleBlendClip(pObj->pBlend, pObj->afSwingTop,
                        pObj->pBlend->pD8->f08 + (pObj->fAnimTime - pSw->fTimeSwingStart))) {
            pSw->fTimeBallHit = pObj->fAnimTime + (pObj->pBlend->pEvents[2].fTime - pObj->afSwingTop[1])
                              + pObj->afSwingTop[3];
        }
        CharacterState_UpdateSKAState(pObj);
        pObj->nClampEvent = 2;
        pSw->fTimeSwingTop = pObj->fAnimTime;
        Character_Set1634(pObj, 0.0f);
        Character_Set1630(pObj, 0.0f);
        Character_Set162C(pObj, 1.4f * Character_GetBackswing(pObj));
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

// ---- the backswing --------------------------------------------------------------------------------

// Enter the hold at the top of the backswing (state 2): the animation is set to the time it settled
// at (fFidgetPauseTime, the waggle's first target) and runs backwards (character flag 0x40) at rate
// 0.008; the hold timers are cleared.
void SW_vStateInitBackSwingFigit(int nPlayer) {
    Player*    p    = &gPlayers[nPlayer];
    Character* pObj = p->pChar;
    SwingData* pSw  = &p->swing;
    pSw->fFidgetTargetTime = pSw->fFidgetPauseTime;
    SKATime_SetTime(pObj->anim, pSw->fFidgetTargetTime);
    Character_UpdateAnimation(pObj, 0, 0.0f);
    SKATime_SetTimeScale(pObj->anim, 0.008f);
    pObj->uFlags |= 0x40;
    pSw->fFidgetTimeElapsed = 0.0f;
    pSw->fFidgetWaitToIdle = 0.0f;
}

// ---- at the top, and the downswing ----------------------------------------------------------------

// Swing state 2, the hold at the top of the backswing; always returns false. Any change in the
// stick's y sends it back to state 1 (the animation running backwards if the stick came forward)
// and starts the boost's 1/12 s die timer. While the stick is steady the animation waggles 0.0076
// either side of the pause time, kept between the start and top marks. When the stick is held near
// centre (y at most 160, x within 64..192) for over 0.1 s the swing is abandoned: state 0,
// animation 5, normal speed, event 9. The power (SW_vSetSwingStrength) is updated every frame, so a
// long hold costs power.
u8 SW_vStateBackSwingFigit(int nPlayer) {
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
        pObj->pModel->pSkel->nShoulderFrames = 4;
    }
    pPad   = SW_vGetStickInfo(nPlayer, nController);
    nX     = SW_vGetStickX(nPlayer, pPad);
    nY     = SW_vGetStickY(nPlayer, pPad);
    fTop   = SW_vGetControllerTopOfSwing(pSw);
    fStart = SW_vGetControllerStartOfSwing(pSw);
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
            SKATime_SetTimeScale(pObj->anim, 1.0f);
            EVENT_Trigger(nPlayer, 9, 0, 0);
        }
    }
    SW_vSetSwingStrength(nPlayer);
    return 0;
}

// Swing state 3, the downswing; returns true on the frame the ball is struck. For a human (outside
// the replay mode), each frame the stick is forward (y at most 96) and more than about 17 units
// (squared distance over 300) from the centre sample, that reading becomes the follow-through and
// mis-hit sample, so what counts is the last such reading before impact. When the animation reports
// impact (nClampEvent < 0): IK relaxed, state 5, SW_vImpact, spin allowed for a human outside a
// replay, the mis-hit rumble on a pad outside a replay, and event 0x2B on a putt.
u8 SW_vStateDownSwing(int nPlayer) {
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
        pObj->pModel->pSkel->nShoulderFrames = 4;
    }
    if (!Controller_IsCPU(nController) && Game_GetMode() != 10) {
        int nDX, nDY;
        f32 fDist2;
        pPad   = SW_vGetStickInfo(nPlayer, nController);
        nX     = SW_vGetStickX(nPlayer, pPad);
        nY     = SW_vGetStickY(nPlayer, pPad);
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
    if (pObj->nClampEvent < 0) {
        SKEL_RelaxIK(pObj->pModel->pSkel);
        gPlayers[nPlayer].swing.nState = 5;
        SW_vImpact(nPlayer);
        if (!Controller_IsCPU(nController) && gSession.bReplay == 0) {
            gPlayers[nPlayer].swing.bCanSpin = 1;
        }
        if (fn_8002E898_IsPad(nController) && gSession.bReplay == 0) {
            SW_vAdjustVibrationFromAttribute(nPlayer);
        }
        if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            EVENT_Trigger(nPlayer, 0x2B, 0, 0);
        }
        return 1;
    }
    return 0;
}

// Stop a player's rumble: the buzz and wave motors off, bVibrating and the countdown cleared.
// Nothing for a player without a pad (controllers 0..7).
void SW_KillVibration(int nPlayer) {
    if (fn_8002E868_HasPad(nPlayer)) {
        // fake match: an empty test (a compiled-away assert?) makes the frontend share the
        // nVibrateCount address, which EA computes before the calls
        if (gPlayers[nPlayer].swing.nVibrateCount) {
        } else {
        }
        Input_vVibrateBuzz(gPlayers[nPlayer].nController, 0);
        Input_vVibrateWave(gPlayers[nPlayer].nController, 0);
        gPlayers[nPlayer].swing.bVibrating = 0;
        gPlayers[nPlayer].swing.nVibrateCount = 0;
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

// ---- after impact ---------------------------------------------------------------------------------

// Swing state 4, the through-swing: does nothing and returns false. Nothing in this build sets
// state 4 (the downswing goes straight to 5).
u8 SW_vStateThroughSwing(int nPlayer) {
    return 0;
}

// Swing state 5, after the ball is struck; always returns false. For a human outside a replay: the
// sticks are read (the values unused), the mis-hit rumble counts down (SW_UpdateVibration) and spin
// can be put on the ball (SW_vUpdateSpinControl, SW_vCalculateSpinFactor).
u8 SW_vStatePostSwing(int nPlayer) {
    int nController = gPlayers[nPlayer].nController;
    u8* pPad;
    if (Player_IsCPU(nPlayer) || gSession.bReplay != 0) {
        return 0;
    }
    pPad = SW_vGetStickInfo(nPlayer, nController);
    SW_vGetStickX(nPlayer, pPad);
    SW_vGetStickY(nPlayer, pPad);
    SW_UpdateVibration(nPlayer);
    SW_vUpdateSpinControl(nPlayer);
    SW_vCalculateSpinFactor(nPlayer);
    return 0;
}

// Swing state 6, a cancelled swing: does nothing and returns false. Nothing in this build sets
// state 6.
u8 SW_vStateCancelSwing(int nPlayer) {
    return 0;
}

// Look up the club trail's textures by name hash, "clubback" (the backswing's), "clubdown" (the
// downswing's) and "tball", into gpSwing (all three in one bank, gpSwing->pBank). nPlayer is
// unused: gpSwing holds one set for everyone.
void SW_vUIInit(int nPlayer) {
    u64 uHash;
    uHash = fn_8000BEE4("clubback");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pClubBack);
    uHash = fn_8000BEE4("clubdown");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pClubDown);
    uHash = fn_8000BEE4("tball");
    fn_800102DC(uHash, &gpSwing->pBank, &gpSwing->pTBall);
}

// Empty a player's club trail history (nNumInBlurQueue = 0).
void SW_vUIBlurReset(int nPlayer) {
    gPlayers[nPlayer].swing.nNumInBlurQueue = 0;
}

// Each frame (gomainloop), record the club for its trail: the club head (bone 0x53) and grip (bone
// 0x52) positions go on the front of the 25-entry history (prevClub), dropping the oldest. When the
// head has moved more than 0.3 since the last entry (and the history is not empty), five in-between
// entries are added instead, each blended from the last entry to now with the shaft re-extended to
// the club's current length, so a fast swing still draws a smooth arc.
void SW_vUIUpdateBlurBuffer(int nPlayer) {
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
    int        nHead = CharModel_GetBoneIndex(pObj->pModel, 0x53);
    int        nGrip = CharModel_GetBoneIndex(pObj->pModel, 0x52);
    f32        f;                  // the head's move, then the blend step
    f32        fLen;
    int        k;
    int        i;

    LLMath_CopyVec(pObj->pModel->pMatrices[nHead][3], v98);
    Vec_Sub(v98, pSw->prevClub[0].vClubPos, vB8);
    f = Math_Sqrt(Vec4_LengthSqClamped(vB8));
    LLMath_CopyVec(v98, vA8);
    if (f > 0.3f && pSw->nNumInBlurQueue != 0) {
        v18[3] = 0.0f;
        v28[3] = 0.0f;
        LLMath_CopyVec(pObj->pModel->pMatrices[nHead][3], v88);
        LLMath_CopyVec(pObj->pModel->pMatrices[nGrip][3], v68);
        LLMath_CopyVec(pSw->prevClub[0].vClubPos, v78);
        LLMath_CopyVec(pSw->prevClub[0].vHandPos, v58);
        Vec_Sub(v68, v58, v48);
        Vec_Sub(v88, v78, v38);
        Vec_Sub(v88, v68, v8);
        fLen = Math_Sqrt(Vec4_LengthSqClamped(v8));
        for (i = 1; i <= 5; i++) {
            f = (f32)i / 5.0f;
            Vec3_Scale(f, v38, v18);
            Vec3_Scale(f, v48, v28);
            Vec_Add(v18, v78, v18);
            Vec_Add(v28, v58, v28);
            Vec_Sub(v18, v28, v8);
            if (v8[0] != 0.0f || v8[1] != 0.0f || v8[2] != 0.0f || v8[3] != 0.0f) {
                v8[3] = 0.0f;
                LLMath_Normalize(v8, v8);
            }
            Vec3_Scale(fLen, v8, v8);
            Vec_Add(v8, v28, v8);
            for (k = 24; k > 0; k--) {
                Mem_cpy(&pSw->prevClub[k], &pSw->prevClub[k - 1], sizeof(pSw->prevClub[k]));
            }
            LLMath_CopyVec(v8, pSw->prevClub[0].vClubPos);
            pSw->prevClub[0].vClubPos[3] = 1.0f;
            LLMath_CopyVec(v68, pSw->prevClub[0].vHandPos);
            if (pSw->nNumInBlurQueue < 25) {
                pSw->nNumInBlurQueue++;
            }
        }
    } else {
        for (k = 24; k > 0; k--) {
            Mem_cpy(&pSw->prevClub[k], &pSw->prevClub[k - 1], sizeof(pSw->prevClub[k]));
        }
        LLMath_CopyVec(vA8, pSw->prevClub[0].vClubPos);
        pSw->prevClub[0].vClubPos[3] = 1.0f;
        LLMath_CopyVec(pObj->pModel->pMatrices[nGrip][3], pSw->prevClub[0].vHandPos);
        if (pSw->nNumInBlurQueue < 25) {
            pSw->nNumInBlurQueue++;
        }
    }
}

// Each frame (gomainloop), during the backswing (animation 6) and downswing (7), set the club
// trail's colour and alpha and twist the club with the stick (SW_vUIAdjustClub). On the backswing
// the trail is blue with the stick left of centre and yellow right of it, more opaque the further
// out; on the downswing it takes gpSwing's colours and fades to nothing over 1.0 of animation time
// after the ball-hit mark, and the twist uses the stick x recorded at the top. Not for putts (shot
// kind 0), nor when n1698 is set or the clip result is 2. A CPU only does this in lessons 8 and 9
// of the lessons mode (11), with the stick hard left (8) or right (9).
void SW_vUIUpdateIK(int nPlayer) {
    Character*   pObj  = gPlayers[nPlayer].pChar;
    int        nBone = CharModel_GetBoneIndex(pObj->pModel, 0x53);
    SwingData* pSw;
    int        nStickX;
    u8*        pPad;
    f32        vPos[3];
    f32        fT;

    if (Player_IsCPU(nPlayer)) {
        if (Game_GetMode() != 11) return;
        if (GM_GetCurrentLesson() != 8) {
            switch (GM_GetCurrentLesson()) {
            case 9:
                break;
            default:
                return;
            }
        }
    }
    if ((pObj->nTargetState != 6 && pObj->nTargetState != 7) || gPlayers[nPlayer].nShotKind == 0
        || pObj->bPosed != 0 ||
        Character_GetClipResult(pObj) == 2) {
        return;
    }
    if (pObj->nTargetState == 6 || pObj->nTargetState == 7) {
        pSw = &gPlayers[nPlayer].swing;
        if (Game_GetMode() == 11 && Player_IsCPU(nPlayer)) {
            if (GM_GetCurrentLesson() == 8) {
                nStickX = 0;
            } else {
                nStickX = 0xFF;
            }
        } else {
            pPad    = SW_vGetStickInfo(nPlayer, gPlayers[nPlayer].nController);
            nStickX = SW_vGetStickX(nPlayer, pPad);
            SW_vGetStickY(nPlayer, pPad);
        }
        LLMath_CopyVec(pObj->pModel->pMatrices[nBone][3], vPos);
        if (pObj->nTargetState == 6) {
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
        } else if (pObj->nTargetState == 7) {
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

// Show (nonzero) or hide (0) a player's boost display (bDrawBoostUI), which SW_vUIRender2D draws.
void SW_vSetDisplayBoostUI(int nPlayer, int bDisplay) {
    gPlayers[nPlayer].swing.bDrawBoostUI = bDisplay;
}

// Each frame (gomainloop), draw the player's boost display (UI_Obj_RenderBoostUI in the player's
// first view) while it has a power or spin boost, on any shot but a putt, when the display is
// switched on (SW_vSetDisplayBoostUI), outside a replay, unpaused, and not while the golf camera's
// GolfCamera_bIs3ScreenCamOn() flag is set.
void SW_vUIRender2D(int nPlayer) {
    if ((gPlayers[nPlayer].swing.nPowerBoost > 0 || gPlayers[nPlayer].swing.nSpinBoost > 0) &&
        gPlayers[nPlayer].nShotKind != 0 && gPlayers[nPlayer].swing.bDrawBoostUI != 0 &&
        gSession.bReplay == 0 &&
        gSession.nPaused == 0 && !GolfCamera_bIs3ScreenCamOn()) {
        UI_Obj_RenderBoostUI(gPlayers[nPlayer].nView[0]);
    }
}

// Each frame (gomainloop), draw the club's trail when the trail option is on (session option
// a24[7]): during the backswing (animation 6, texture "clubback") or downswing (7, "clubdown"),
// with at least two recorded positions and the clip result not 2, a ribbon from the grip through
// the recorded club-head positions (SW_vUIUpdateBlurBuffer) in the trail colour, fading along its
// length, blended and without depth writes. A CPU's trail is drawn only in lessons 8 and 9 of the
// lessons mode (11).
void SW_vUIRender3D(int nPlayer) {
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
    nGrip = CharModel_GetBoneIndex(pObj->pModel, 0x52);
    if (Player_IsCPU(nPlayer)) {
        if (Game_GetMode() != 11) return;
        if (GM_GetCurrentLesson() != 8) {
            switch (GM_GetCurrentLesson()) {
            case 9:
                break;
            default:
                return;
            }
        }
    }
    {
        if ((pObj->nTargetState == 6 || pObj->nTargetState == 7) && Character_GetClipResult(pObj) != 2 &&
            pSw->nNumInBlurQueue >= 2 && gSession.options.a24[7] != 0) {
            LLMath_CopyVec(pObj->pModel->pMatrices[nGrip][3], vGrip);
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
            RenderState_SetClipMode(0);
            RenderState_SetCameraMatrices();
            RenderState_SetViewport(RC_spGetCurrentRenderCtx());
            RenderState_SetDrawFlags(0x50);
            RenderState_SetBlendFactors(4, 5);
            DS_vSetAlphaTestMode(0, 6, 0x80);
            DS_vEnableZBufferUpdate(0);
            if (pObj->nTargetState == 6) {
                RenderState_SetBankTexture(gpSwing->pBank, gpSwing->pClubBack);
            } else if (pObj->nTargetState == 7) {
                RenderState_SetBankTexture(gpSwing->pBank, gpSwing->pClubDown);
            }
            RenderState_Flush();
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
            SD_FillShaderObject((ShaderObject*)gpSwing->mesh[nView], &mesh, 1);
            SD_DrawShaderObject(gpSwing->mesh[nView]);
            DS_vSetAlphaTestMode(1, 6, 0x80);
            DS_vEnableZBufferUpdate(1);
            RenderState_Flush();
        }
    }
}

// fake match: stands in for a function the original linker stripped. The pool has 2.0f
// (0x8028363C) here, before the 0.33333334f SW_vUIAdjustClub uses first; its body is unknown.
static f32 Swing_StrippedFn2(f32 x) {
    return x + 2.0f;
}

// Twist the club with the stick: how far through the backswing (animation 6, eased in) or the
// downswing (7, eased out) the animation is, times 0.75 and a smoothed copy of the stick's X,
// becomes a Z rotation on the model (SKEL_SetExtraRightShoulderRotation), negated when
// Character_IsLeftHanded().
void SW_vUIAdjustClub(Character* pObj, SwingData* pSw, int nStickX) {
    f32 fAmount = 0.0f;
    f32 fDelta;
    f32 fRate;
    f32 vRot[3];
    CharModel_GetBoneIndexMapped(pObj->pModel, 0x24);
    CharModel_GetBoneIndexMapped(pObj->pModel, 0x11);
    CharModel_GetBoneIndex(pObj->pModel, 0x52);
    if (pObj->nTargetState == 6) {
        fAmount = (pObj->fAnimTime - pSw->fTimeSwingStart) / (pSw->fTimeSwingTop - pSw->fTimeSwingStart);
        fAmount *= fAmount;
    } else if (pObj->nTargetState == 7) {
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
    if (Character_IsLeftHanded(pObj)) {
        fAmount = -fAmount;
    }
    Quat_EulerAngles(0.0f, 0.0f, fAmount, vRot);
    SKEL_SetExtraRightShoulderRotation(pObj->pModel, vRot);
}

// ---- the hit -----------------------------------------------------------------------------------

// The ball is struck (from the downswing, a replayed swing, a tap-in or the green-watch roll). In
// the replay mode (10) the recorded random seed and the first player's recorded swing data are
// restored; otherwise a live shot runs Luck_TakePerfectShot and REPLAY_Save, a replayed one
// REPLAY_Play. Then the face vector (the first launch block), the mis-hit angle (zero for a CPU or
// a perfect shot), the power (SW_vCalculateShotPower) and forgiveness; a putt from under 2.0
// (fDistance) goes dead straight. The second launch block follows, and the aim (the player's aim
// plus the face vector's angle plus the mis-hit angle, wrapped to -pi..pi) goes with the club, shot
// kind, power and trajectory to Physics_ShotImpact.
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
    SW_vGetClubDirection(nPlayer, gPlayers[nPlayer].vLaunchA);
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        gPlayers[nPlayer].swing.fMishitAngle = 0.0f;
    } else {
        gPlayers[nPlayer].swing.fMishitAngle = SW_vCalculateMishitAngle(nPlayer);
    }
    gPlayers[nPlayer].swing.fShotPower = SW_vCalculateShotPower(nPlayer);
    SW_vAdjustMishitFromAttribute(nPlayer);
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e && gPlayers[nPlayer].fDistance < 2.0f) {
        gPlayers[nPlayer].vLaunchA[0] = 0.0f;
        gPlayers[nPlayer].vLaunchA[1] = 0.0f;
        gPlayers[nPlayer].vLaunchA[2] = 1.0f;
        gPlayers[nPlayer].vLaunchA[3] = 0.0f;
    }
    SW_vGetStrokeDirection(nPlayer, gPlayers[nPlayer].vLaunchB);
    gPlayers[nPlayer].swing.fHookSlice = gPlayers[nPlayer].vLaunchA[0];
    if (Player_IsController8(nPlayer)) {
        SW_vImpactForController8(nPlayer);
        nTrajectory = p->nTrajectory;
        nClub       = p->nClub;
        nKind       = p->nShotKind;
    }
    if (0.0f == gPlayers[nPlayer].vLaunchA[2]) {
        fAim = p->fAim + gPlayers[nPlayer].swing.fMishitAngle;
    } else {
        fAim = p->fAim + Math_Atan(gPlayers[nPlayer].vLaunchA[0] / gPlayers[nPlayer].vLaunchA[2]) +
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

// ---- the power meter -----------------------------------------------------------------------------

// Each frame of the backswing and of the hold at the top: the player's power (fPower, 0..1) is the
// square root of how far along the backswing the animation is (a CPU takes it straight), snapping
// to 1 within 0.03 of the top. Holding at the top of a full backswing for over 0.05 s costs (hold -
// 0.05)^2, at most 0.3, except on a putt.
void SW_vSetSwingStrength(int nPlayer) {
    f32  fPower = gPlayers[nPlayer].pChar->fBackswing;
    f32  fPenalty;
    if (!Player_IsCPU(nPlayer)) {
        fPower = (f32)Math_Sqrt(fPower);
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

// The shot's final power, for SW_vImpact. A CPU or a perfect shot takes its planned power times
// AI_PowerScale (a putt that is not a tap-in gets 5% more, at least 0.1). For a human,
// fNonPowerShotPower is set to the boosted power less the mis-hit angle's size; then by shot kind:
// a putt (a meter over gpSwing->fPuttFullPower counts as full) is scaled by the putt table for its
// distance, a chip (scaled by Physics_EstimateShotPower) or a pitch gets the boost, each at least
// 0.1. Any other shot takes the boost (all but kinds 5-7 also AI_PowerScale and the tee sweet spot)
// and then loses power to the mis-hit angle: a scaled part of it below a threshold, all of it above
// (both from gSwingAttributeTable by the recovery rating for kinds 5-7 or a ball in the rough, lies
// 3-4, or sand, 6-8; else by driving accuracy). The result is kept within 0.05..1.5, except on a
// tap-in.
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
    gPlayers[nPlayer].swing.fNonPowerShotPower = SW_fPowerBoostAdjustment(nPlayer, fPower) - fError;
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
        fPower = SW_fPowerBoostAdjustment(nPlayer, f);
        Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_APPROACH, ATTR_TOTAL);
        if (fPower < 0.1f) {
            fPower = 0.1f;
        }
        goto clamp;         // fake match: the shared clamp as a jump (without the gotos: 83.9%, not 84.9%)
    }
    case 5:
    case 6:
    case 7:
        fPower     = SW_fPowerBoostAdjustment(nPlayer, *pPower);
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
        fPower = SW_fPowerBoostAdjustment(nPlayer, fPower);
        fPower = SW_fPowerAdjustForDraw(nPlayer, fPower);
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

// The clubface direction at impact (Player.vLaunchA, TW06 clubDirection), a unit vector in pOut: x
// sideways, z along the aim line. Square (0, 0, 1) for a CPU, a perfect shot, a stick that ended
// level with its calibrated centre, or with session flags 0x4000 and 0x8000 both set. On a full
// shot the stick's angle at the top of the backswing (atan of its sideways over its vertical offset
// from the calibrated centre) is kept in fControllerSliceAngle and turned into a face angle a by
// SW_fCalculateSliceAmount: (-sin a, 0, cos a). On a putt fControllerSliceAngle is 0 and the
// sideways part is the stick's sideways offset times how far toward that edge it went times
// gPuttXScale (0.03).
void SW_vGetClubDirection(int nPlayer, f32* pOut) {
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
        fAngle = Math_Atan((fTopX - fCentreX) / fDY);
        gPlayers[nPlayer].swing.fControllerSliceAngle = fAngle;
        fAngle = SW_fCalculateSliceAmount(&gPlayers[nPlayer].nClub, fAngle);
        fSin   = Math_Sin(fAngle);
        fCos   = Math_Cos(fAngle);
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
    LLMath_Normalize(pOut, pOut);
}

// The swing path direction at impact (Player.vLaunchB, TW06 strokeDirection), which carries the
// shot's shape: for a CPU or a perfect shot the planned shape's direction (fn_8002D560_ShapeDir),
// for a human straight, (0, 0, 1).
void SW_vGetStrokeDirection(int nPlayer, f32* pOut) {
    if (Player_IsCPU(nPlayer) || gPlayers[nPlayer].bPerfect) {
        fn_8002D560_ShapeDir(nPlayer, pOut);
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
}

// ---- the clubface -------------------------------------------------------------------------------

// How far the clubface turns, in radians and signed like fBackAngle, for a stick angle fBackAngle
// (radians off vertical at the top of the backswing). The angle as a fraction of a quarter turn
// goes through a three-piece linear curve, (0, 0) to (fKnot1X, fKnot1Y) to (fKnot2X, fKnot2Y) to
// (1, 1) (gpSwing), then is scaled by a quarter turn and by the club's shaping range,
// fCurveMin..fCurveMax by gClubCurve[club] / 26. pClub points at Player.nClub.
f32 SW_fCalculateSliceAmount(s32* pClub, f32 fBackAngle) {
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

// ---- the meter's miss ----------------------------------------------------------------------------

// The swing's miss in radians, added to the aim; SW_vImpact asks only for a human's shot that is
// not perfect. Back is (centre - top, 0, top - centre) from the stick's calibrated centre and its
// position at the top of the backswing, through is (through - centre, 0, centre - through) from its
// downswing sample, each normalised; the miss is the angle (atan x/z) of (0, 0, 1) + through -
// back, clamped to +-gpSwing->fMaxError, so a straight back-and-through gives about 0. Both x
// samples first get a random +-15 (the stick reads 0..255), and the x differences are scaled by
// gSwingXScale (0.03 on a putt, 0.2 otherwise).
f32 SW_vCalculateMishitAngle(int nPlayer) {
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
        LLMath_Normalize(vThrough, vThrough);
    }
    if (0.0f != vBack[0] || 0.0f != vBack[2]) {
        LLMath_Normalize(vBack, vBack);
    }
    Vec_Sub(vThrough, vBack, vDiff);
    Vec_Add(vDir, vDiff, vDir);
    if (vDir[2]) {
        fAngle = Math_Atan(vDir[0] / vDir[2]);
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

// fPower plus the draw bonus: a driver (club 0) off the tee (lie 0) swung with a draw (a negative
// fControllerSliceAngle) gains up to gpSwing->fTeeBonus (0.1), the most when the stick's angle as a
// fraction of a quarter turn sits midway between the curve's knots fKnot1X (0.4) and fKnot2X (0.6),
// nothing at or outside them. Any other shot gets fPower back unchanged.
f32 SW_fPowerAdjustForDraw(int nPlayer, f32 fPower) {
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

// ---- the power boost input --------------------------------------------------------------------

// Every backswing frame, for a human with the power boost option on: while the boost button
// (Controller_GetButtonMask(0x1F)) is held with the stick more than 93 from centre, the boost level
// (nPowerBoost) rises one a frame up to 8, sending event 0x2D at each step. A running
// fPowerBoostDieTime (1/12 s once the backswing backs down) counts down; when it runs out the level
// and the turn angles are cleared and the boost display is reset (UI_Obj_ResetBoostRings).
void SW_vCheckForSwingBoost(int nPlayer) {
    u32  uButtons;
    int  nX, nY;
    f32  fMag;

    if (Player_IsCPU(nPlayer)) return;
    if (gSession.options.bBoostEnabled == 0) return;
    uButtons    = Input_ReadControlPad(gPlayers[nPlayer].nController);
    nY   = SW_vGetStickY(nPlayer, SW_vGetStickInfo(nPlayer, gPlayers[nPlayer].nController));
    nX   = SW_vGetStickX(nPlayer, SW_vGetStickInfo(nPlayer, gPlayers[nPlayer].nController));
    fMag = (f32)Math_Sqrt((nX - 128) * (nX - 128) + (nY - 128) * (nY - 128));
    if ((uButtons & Controller_GetButtonMask(0x1F, 0)) && fMag > 93.0f) {
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
            UI_Obj_ResetBoostRings(nPlayer);
        }
    }
}

// Clears the power boost level, the spin amount and the boost back-down timer, shows the boost
// display again (SW_vSetDisplayBoostUI) and resets it (UI_Obj_ResetBoostRings). Called when a shot is set up
// and when the swing starts.
void SW_vClearBoosts(int nPlayer) {
    gPlayers[nPlayer].swing.nPowerBoost = 0;
    gPlayers[nPlayer].swing.nSpinBoost = 0;
    gPlayers[nPlayer].swing.fPowerBoostDieTime   = 0.0f;
    SW_vSetDisplayBoostUI(nPlayer, 1);
    UI_Obj_ResetBoostRings(nPlayer);
}

// fPower plus the power boost: the step of the level pressed (gBoostSteps: 1, 2, 4 ... 20 for
// levels 1..8) times a scale from the POWER BOOST attribute's base value (0.005 at 0, 0.01 at 100,
// 0.011 at 110; per-player modifiers do not count). Level 0 adds nothing.
f32 SW_fPowerBoostAdjustment(int nPlayer, f32 fPower) {
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

// The spin asked for: fSideSpin into *pfSide, fForwardSpin into *pfForward (EA's parameter
// names). In a replay both are first read back from the recording into the swing data
// (REPLAY_GetSpin).
void SW_vGetCurrentSpin(int nPlayer, f32* pfSide, f32* pfForward) {
    if (gSession.bReplay) {
        REPLAY_GetSpin(nPlayer, &gPlayers[nPlayer].swing.fForwardSpin, &gPlayers[nPlayer].swing.fSideSpin);
    }
    *pfForward = gPlayers[nPlayer].swing.fForwardSpin;
    *pfSide = gPlayers[nPlayer].swing.fSideSpin;
}

// The stick's sideways angle at the top of the backswing (fControllerSliceAngle) as a fraction of a
// quarter turn, -1..1, sign flipped for a right-handed golfer so it reads the same for either hand.
// Read by the situation scripts (SitDev) and the lessons (mode 11).
f32 SW_vGetHookSlice(int nPlayer) {
    if (Character_IsLeftHanded(gPlayers[nPlayer].pChar)) {
        return gPlayers[nPlayer].swing.fControllerSliceAngle / 1.5707964f;
    }
    return -(gPlayers[nPlayer].swing.fControllerSliceAngle / 1.5707964f);
}

f32 SW_vGetMishitAngle(int nPlayer) {
    return gPlayers[nPlayer].swing.fMishitAngle;
}

f32 SW_vGetNonPowerAttributeAffectedShotPower(int nPlayer) {
    return gPlayers[nPlayer].swing.fNonPowerShotPower;
}

// Ends the spin input for this shot (bCanSpin = 0): SW_vUpdateSpinControl takes no more stick.
// Called from the event code (EVENT_LastBounceForSpinna).
void SW_vCloseSpinWindow(int nPlayer) {
    gPlayers[nPlayer].swing.bCanSpin = 0;
}

// Spin input, every frame after impact (SW_vStatePostSwing), for a human with the spin option
// on while the spin button (Controller_GetButtonMask(0x20)) is held and the window is open
// (bCanSpin): sends event 0x2E, grows the amount (nSpinBoost) one a frame up to 20 (a third of a
// second for full spin), and takes the stick as the direction (nSpinCtrlX/Y) whenever either axis
// is outside 96..160. The direction starts at x 128, y 255 (full y deflection) until the stick
// leaves the dead zone.
void SW_vUpdateSpinControl(int nPlayer) {
    u32     uButtons;
    int     nX, nY;
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.options.bSpinEnabled == 0) return;
    uButtons    = Input_ReadControlPad(gPlayers[nPlayer].nController);
    if (!(uButtons & Controller_GetButtonMask(0x20, 0))) return;
    if (gPlayers[nPlayer].swing.bCanSpin == 0) return;
    EVENT_Trigger(nPlayer, 0x2E, 0, 0);
    nX = SW_vGetStickX(nPlayer, SW_vGetStickInfo(nPlayer, gPlayers[nPlayer].nController));
    nY = SW_vGetStickY(nPlayer, SW_vGetStickInfo(nPlayer, gPlayers[nPlayer].nController));
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
// How much spin the SPIN attribute allows: 0.15 at 0, 0.6 at 100, 1.0 at 110.
f32 SW_GetSpinScale(int nSpin) {
    return TABLE_AT(ROW_SPIN, (s8)nSpin);
}

// Turns the spin input into the shot's spin: each stick axis (-1..1 from centre) times the amount
// asked for (nSpinBoost / 20) times the SPIN scale (SW_GetSpinScale: 0.15 at 0, 0.6 at 100, 1.0 at
// 110) gives fSideSpin (x) and fForwardSpin (y, sign flipped). No input gives no spin; a replay
// keeps the recorded spin.
void SW_vCalculateSpinFactor(int nPlayer) {
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
    fScale = SW_GetSpinScale(nSpin);
    pSw->fSideSpin *= fScale;
    pSw->fForwardSpin *= fScale;
    pSw->fForwardSpin *= -1.0f;
}

// Shrinks a human's miss (fMishitAngle) by the governing attribute: when the miss is smaller than
// the attribute's threshold it is multiplied by its scale (at 100, a drive's misses under 0.415
// become 82% smaller). RECOVERY in sand, high rough and rough (lies 3, 4, 6..8) and for shot kinds
// 5..7, PUTTING on a putt, APPROACH on a chip or pitch, otherwise DRIVING ACCURACY for clubs 0..8
// and BALL STRIKING (three rows by club) for the rest. A putt from under 2 units loses its miss
// entirely. CPU and perfect shots are left alone.
void SW_vAdjustMishitFromAttribute(int nPlayer) {
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

// Rumbles the pad for a miss: nVibrateCount = |fMishitAngle| times a rate from the shot's attribute
// (135 frames per radian at 0, 35 at 100, 30 at 110; PUTTING on a putt, APPROACH on a chip or
// pitch, RECOVERY for shot kinds 5..7, BALL STRIKING otherwise), at most 30; any at all starts the
// buzz and the wave (0xFF) and sets bVibrating. Called from the downswing (SW_vStateDownSwing).
void SW_vAdjustVibrationFromAttribute(int nPlayer) {
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
        Input_vVibrateBuzz(nPad, 1);
        Input_vVibrateWave(nPad, 0xFF);
    }
}

int SW_fGetBoostMagnitude(int nPlayer) {
    return gPlayers[nPlayer].swing.nPowerBoost;
}

int SW_fGetSpinMagnitude(int nPlayer) {
    return gPlayers[nPlayer].swing.nSpinBoost;
}

// The time a SKA tag (a timed event of the character's clip, by id: aTags[uEvent].fTime) comes;
// TW07's char.h inline, out of line here. Read by the swing, the cameras and the golfer states.
f32 Character_GetTagTime(Character* pChar, u64 uEvent) {
    return pChar->aTags[(int)uEvent].fTime;
}

// Sets Character.f162C (nothing without a character). Its only caller, the backswing
// (SW_vStateBackSwing), sets it to 1.4 x Character_GetBackswing at the top of the swing;
// Character_Create starts it at 1.0. No code in this build reads it.
void Character_Set162C(Character* pObj, f32 f) {
    if (pObj != NULL) {
        pObj->f162C = f;
    }
}

// How far along the backswing the character is (Character.fBackswing, 0..1); 0 without a character.
f32 Character_GetBackswing(Character* pObj) {
    if (pObj == NULL) {
        return 0.0f;
    }
    return pObj->fBackswing;
}

// Sets Character.f1630 (nothing without a character). Its only caller, the backswing
// (SW_vStateBackSwing), sets it to 0 at the top of the swing; Character_Create starts it at 0.5.
// No code in this build reads it.
void Character_Set1630(Character* pObj, f32 f) {
    if (pObj != NULL) {
        pObj->f1630 = f;
    }
}

// Sets Character.f1634 (nothing without a character). Its only caller, the backswing
// (SW_vStateBackSwing), sets it to 0 at the top of the swing; Character_Create starts it at 0.2.
// No code in this build reads it.
void Character_Set1634(Character* pObj, f32 f) {
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

// Paired-single vector subtract over four floats: pOut = pA - pB.
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

// A 4-float vector's squared length, at most FLT_MAX (__float_max).
f32 Vec4_LengthSqClamped(f32* pV) {
    f32 f = pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2] + pV[3] * pV[3];
    if (f > __float_max[0]) {
        f = __float_max[0];
    }
    return f;
}

// The lesson being played in game mode 11 (gLessonNum: 1..11, 12 when all are done). The swing's
// trail and IK read it to pull a CPU demonstrator's stick hard to one side in lessons 8 and 9.
int GM_GetCurrentLesson(void) {
    return gLessonNum;
}

// The texture of the next draw: entry pTex of bank pBank (gRenderState.pTexBank, pTexEntry; uFlags
// bit 1 says they are set).
void RenderState_SetBankTexture(TexBank* pBank, TexEntry* pTex) {
    gRenderState.pTexBank = pBank;
    gRenderState.pTexEntry = pTex;
    gRenderState.uFlags |= 1;
}

f32 Math_Atan(f32 fTan) {
    return atan(fTan);
}

// Empty in this build. SW_vImpact calls it only for a player on controller 8 (neither a pad nor the
// CPU) and re-reads the player's club, trajectory and shot kind after it, so it presumably once set
// them.
void SW_vImpactForController8(int nPlayer) {
}
