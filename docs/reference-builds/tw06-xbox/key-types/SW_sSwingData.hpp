//
// Information extracted with resym v0.4.0
//
// PDB file: default.pdb
// Image architecture: X86
//


struct SW_sBoostNodeS;

struct SW_sBonePathPt { /* Size=0x20 */
  /* 0x0000 */ public: float xClubPos[4];
  /* 0x0010 */ public: float xHandPos[4];
};

enum SW_eSwingState : int {
  SW_IDLE_SWING = 0x0000,
  SW_BACK_SWING = 0x0001,
  SW_BACK_FIGIT_SWING = 0x0002,
  SW_DOWN_SWING = 0x0003,
  SW_FOLLOW_SWING = 0x0004,
  SW_POST_SWING = 0x0005,
  SW_CANCEL_SWING = 0x0006,
  SW_NUM_STATES = 0x0007,
};

struct SW_sSwingData { /* Size=0x648 */
  /* 0x0000 */ public: SW_eSwingState state;
  /* 0x0004 */ public: float time_swingTop;
  /* 0x0008 */ public: float time_swingStart;
  /* 0x000c */ public: float time_ballHit;
  /* 0x0010 */ public: float time_followEnd;
  /* 0x0014 */ public: float fClubOffScale;
  /* 0x0018 */ public: float fClubOffTargetScale;
  /* 0x001c */ public: int backSwingX;
  /* 0x0020 */ public: int backSwingY;
  /* 0x0024 */ public: int backSwingXstick2;
  /* 0x0028 */ public: int backSwingYstick2;
  /* 0x002c */ public: bool secondAnalogUsed;
  /* 0x002d */ public: bool secondAnalogPenaltyZone;
  /* 0x0030 */ public: int followThroughX;
  /* 0x0034 */ public: int followThroughY;
  /* 0x0038 */ public: int mishitX;
  /* 0x003c */ public: int mishitY;
  /* 0x0040 */ public: int ballFlightX;
  /* 0x0044 */ public: int ballFlightY;
  /* 0x0048 */ public: float fForwardSwingMagnitude;
  /* 0x004c */ public: float fMishitAngle;
  /* 0x0050 */ public: float fShotPower;
  /* 0x0054 */ public: float fHookSlice;
  /* 0x0058 */ public: float fNonPowerAttribAffectedShotPower;
  /* 0x005c */ public: float fControllerSliceAngle;
  /* 0x0060 */ public: SW_sBonePathPt xPrevClubInfo[25];
  /* 0x0380 */ public: int iNumInBlurQueue;
  /* 0x0384 */ public: bool bUIInit;
  /* 0x0385 */ public: bool bDrawBoostUI;
  /* 0x0388 */ public: int iCalibrateX;
  /* 0x038c */ public: int iCalibrateY;
  /* 0x0390 */ public: int iCalibrateXstick2;
  /* 0x0394 */ public: int iCalibrateYstick2;
  /* 0x0398 */ public: int iCalibrateLeftX;
  /* 0x039c */ public: int iCalibrateLeftY;
  /* 0x03a0 */ public: int iCalibrateRightX;
  /* 0x03a4 */ public: int iCalibrateRightY;
  /* 0x03a8 */ public: int iStickUsed;
  /* 0x03ac */ public: int xCtrlListX[25];
  /* 0x0410 */ public: int xCtrlListY[25];
  /* 0x0474 */ public: int iCurrentCtrlListIndex;
  /* 0x0478 */ public: int iVibrateCount;
  /* 0x047c */ public: bool bVibrating;
  /* 0x0480 */ public: int iVibrateStrength;
  /* 0x0484 */ public: float fCurrentStickPower;
  /* 0x0488 */ public: float fFigitPauseTime;
  /* 0x048c */ public: int iFigitControllerPauseVal;
  /* 0x0490 */ public: float fFigitTargetTime;
  /* 0x0494 */ public: float fFigitWaitToIdleReturn;
  /* 0x0498 */ public: float fFigitTimeElapsed;
  /* 0x049c */ public: float fBlueColor;
  /* 0x04a0 */ public: float fRedColor;
  /* 0x04a4 */ public: float fGreenColor;
  /* 0x04a8 */ public: float fAlpha;
  /* 0x04ac */ public: int iPowerBoost;
  /* 0x04b0 */ public: float fPowerBoostDieTime;
  /* 0x04b4 */ public: int bSwingBoostsOn[4];
  /* 0x04c4 */ public: float fSwingBoostAmount[4];
  /* 0x04d4 */ public: SW_sBoostNodeS* pCurBoostState;
  /* 0x04d8 */ public: char xCurBoostStrs[10][32];
  /* 0x0618 */ public: int iCurBoostNum;
  /* 0x061c */ public: float fTimeSinceContact;
  /* 0x0620 */ public: float fSpinAmount;
  /* 0x0624 */ public: bool bSpun;
  /* 0x0625 */ public: bool bSpinning;
  /* 0x0628 */ public: int iSpinBoost;
  /* 0x062c */ public: int iSpinCtrlX;
  /* 0x0630 */ public: int iSpinCtrlY;
  /* 0x0634 */ public: float fForwardSpin;
  /* 0x0638 */ public: float fSideSpin;
  /* 0x063c */ public: bool bCanSpin;
  /* 0x0640 */ public: float fTargetTurnAngle;
  /* 0x0644 */ public: float fCurrentTurnAngle;
};
