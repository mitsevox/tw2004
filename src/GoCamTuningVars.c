// GoCamTuningVars.c: the camera tuning values, the fixed speeds, distances, blend times and angles
// the cameras read, set up when the front end or a round starts and freed when it shuts down.

#include "game_types.h"
#include "engine.h"
#include "camera.h"

// The camera tuning values, allocated and set by CameraTuning_Init (NULL once CameraTuning_Close
// has freed them).
sCameraTuningVars* gpCamTuning;

// Allocate the camera tuning values (gpCamTuning) and set each to its fixed value; run when the
// front end and each round start (GO_vInitFE, GO_vInitIG). fMaxPitchUp and fMaxPitchDown are in
// degrees (GolfCamera_ClampLookAngle converts them).
void CameraTuning_Init(void) {
    gpCamTuning = Circular_MemAllocFunc(sizeof(sCameraTuningVars), MEMTYPE_LOW, 0, "GoCamTuningVars.c", 24);
    gpCamTuning->fZoomToAimSpeed = 10.0f;
    gpCamTuning->fZoomToAimBack = 15.0f;
    gpCamTuning->fZoomToAimRampDist = 0.0f;
    gpCamTuning->fZoomToAimNearRange = 10.0f;
    gpCamTuning->fZoomToAimNearShare = 0.9f;
    gpCamTuning->fZoomToAimHeight = 5.0f;
    gpCamTuning->fZoomToAimLag = 5.0f;
    gpCamTuning->fZoomBlur = 0.45f;
    gpCamTuning->fZoomToAimWaitBlur = 0.35f;
    gpCamTuning->fZoomArriveDist = 0.01f;
    gpCamTuning->fZoomToAimHeightShare = 0.1f;
    gpCamTuning->fZoomToAimStartLag = 15.0f;
    gpCamTuning->fGreenZoomRampDist = 0.25f;
    gpCamTuning->fGreenZoomSpeed = 3.0f;
    gpCamTuning->fZoomToAimPuttBack = 0.25f;
    gpCamTuning->fGreenZoomLookDrop = 0.01f;
    gpCamTuning->fGreenZoomHeightStep = 0.5f;
    gpCamTuning->fGreenZoomHeight = 4.0f;
    gpCamTuning->fGreenZoomLag = 1.25f;
    gpCamTuning->fGreenRollHeight = 0.2f;
    gpCamTuning->fGreenRollDist = 4.0f;
    gpCamTuning->fGreenRollTime = 2.0f;
    gpCamTuning->fReturnBlendTime = 1.0f;
    gpCamTuning->fMatrixBlendTime = 1.5f;
    gpCamTuning->fMatrixStepTime = 0.1f;
    gpCamTuning->fMatrixBlur = 0.45f;
    gpCamTuning->vReplayTint[0] = 0.5f;
    gpCamTuning->vReplayTint[1] = 0.4f;
    gpCamTuning->vReplayTint[2] = 0.4f;
    gpCamTuning->vReplayTint[3] = 0.5f;
    gpCamTuning->fReplayTintTime = 0.1f;
    gpCamTuning->fSuperZoomStartFov = 140.0f / 180.0f * PI;
    gpCamTuning->fSuperZoomEndFov = 10.0f / 180.0f * PI;
    gpCamTuning->fSuperZoomBlendTime = 0.5f;
    gpCamTuning->fSuperZoomBlur = 0.4f;
    gpCamTuning->fSuperZoomShake = 0.01f;
    gpCamTuning->fBallLowEventHeight = 2.0f;
    gpCamTuning->fElevatorBlendTime = 2.0f;
    gpCamTuning->fHeightKeepShare = 0.95f;
    gpCamTuning->fSwingCamMinPower = 0.9f;
    gpCamTuning->fSwingCamMidPower = 0.95f;
    gpCamTuning->fSwingCamHighPower = 0.99f;
    gpCamTuning->fSwingCamLowChance = 10.0f;
    gpCamTuning->fSwingCamMidChance = 35.0f;
    gpCamTuning->fSwingCamHighChance = 100.0f;
    gpCamTuning->fSwingCamShortClubPower = 1.15f;
    gpCamTuning->fPostShotSandWait = 0.65f;
    gpCamTuning->nBeats = 4;
    gpCamTuning->nBeatFrames = 2;
    gpCamTuning->fBeatFadeInTime = 0.25f;
    gpCamTuning->fBeatFadeOutTime = 0.5f;
    gpCamTuning->fBeatHoldTime = 0.0f;
    gpCamTuning->fAboveMaxHeightShare = 0.6f;
    gpCamTuning->fScriptEaseInTime = 0.5f;
    gpCamTuning->fAimMarkerLag = 20.0f;
    gpCamTuning->fAimMarkerHeightShare = 0.1f;
    gpCamTuning->fGroundHeightShare = 0.01f;
    gpCamTuning->fBallLagAngle = PI / 10.0f;
    gpCamTuning->fGolferLagAngle = 0.16f;
    gpCamTuning->fOutOfBoundsDelay = 0.5f;
    gpCamTuning->fAimMarkerMinDist = 0.05f;
    gpCamTuning->fFairwaySpotMaxDot = 0.5f;
    gpCamTuning->fUnread1 = 0.1f;
    gpCamTuning->fDefaultSwingCamMinDot = 0.95f;
    gpCamTuning->fFairwayCamMoveDist = 80.0f;
    gpCamTuning->fFairwayCamMinTime = 2.0f;
    gpCamTuning->fFairwayCamMaxDot = 0.25f;
    gpCamTuning->fFairwayCamHeight = 4.0f;
    gpCamTuning->fFairwayCamMinFov = 15.0f / 180.0f * PI;
    gpCamTuning->fFairwayCamFov = DEG(50.0f);
    gpCamTuning->fFairwayCamFovStep = 0.001f;
    gpCamTuning->fShoulderShakeScale = 1.0f;
    gpCamTuning->fFlagClearDist = 1.0f;
    gpCamTuning->fFlagClearHeight = 2.5f;
    gpCamTuning->fSteepAimMinDist = 5.0f;
    gpCamTuning->fAimMarkerMaxDrop = 5.0f;
    gpCamTuning->fPlayableHeadroom = 10.0f;
    gpCamTuning->fLookNearScale = 7.0f;
    gpCamTuning->fBallLagMaxShare = 0.8f;
    gpCamTuning->fBallLagFullDist = 3.0f;
    gpCamTuning->fBallLagLowShare = 0.1f;
    gpCamTuning->fLookLevelShare = 0.2f;
    gpCamTuning->fLookHeightShare = 0.35f;
    gpCamTuning->fTrackCloseRate = 0.05f;
    gpCamTuning->fTrackTurnRate = 0.005f;
    gpCamTuning->fTrackEaseInTime = 1.0f;
    gpCamTuning->fCutEaseInTime = 2.0f;
    gpCamTuning->fCutHeightEaseTime = 4.0f;
    gpCamTuning->fCutHeightEasePower = 2.0f;
    gpCamTuning->fGroundLookHeightShare = 0.1f;
    gpCamTuning->fGroundClearance = 0.2f;
    gpCamTuning->fBallObstructionDist = 5.2f;
    gpCamTuning->fFadeTime = 0.25f;
    gpCamTuning->fHeightCeilingEase = 1.0f;
    gpCamTuning->fFlyByFadeTime = 0.25f;
    gpCamTuning->vFlyByFadeColor[0] = 0.5f;
    gpCamTuning->vFlyByFadeColor[1] = 0.5f;
    gpCamTuning->vFlyByFadeColor[2] = 0.5f;
    gpCamTuning->vFlyByFadeColor[3] = 0.5f;
    gpCamTuning->fAboveMaxDistShare = 1.0f;
    gpCamTuning->fBelowMinDistShare = 1.0f;
    gpCamTuning->fRiseStep = 0.01f;
    gpCamTuning->fTrackMinBallSpeed = 0.7f;
    gpCamTuning->fMaxFlightTilt = DEG(10.0f);
    gpCamTuning->fUnread2 = 0.2f;
    gpCamTuning->fObstructionMaxSlope = 1.0f;
    gpCamTuning->fBallUpdatesStep = 0.001f;
    gpCamTuning->fUnread4 = 0.0f;
    gpCamTuning->fUnread3 = 0.01f;
    gpCamTuning->bSteepSlopeCam = 0;
    gpCamTuning->nUnread1 = 0;
    gpCamTuning->bCheckSlope = 1;
    gpCamTuning->bCheckTerrain = 1;
    gpCamTuning->fSteepSlopeHeightStep = 0.1f;
    gpCamTuning->fSteepSlopeBackStep = 0.05f;
    gpCamTuning->fSteepSlopeBack = 5.0f;
    gpCamTuning->nSteepSlopeMaxTries = 30;
    gpCamTuning->fSlopeUp = 0.25f;
    gpCamTuning->fSlopeDown = 0.25f;
    gpCamTuning->fSteepSlopeOffsetX = -0.2f;
    gpCamTuning->fSteepSlopeHeight = 1.1f;
    gpCamTuning->fSteepSlopeMaxCamMove = 0.05f;
    gpCamTuning->fSteepSlopeMaxAimMove = 1.5f;
    gpCamTuning->fMaxPitchUp = 12.0f;
    gpCamTuning->fMaxPitchDown = 30.0f;
    gpCamTuning->fCollideMinMoveCos = 0.7f;
    gpCamTuning->fCollideMinLookCos = 0.3f;
    gpCamTuning->fCollideMaxMove = 0.2f;
    gpCamTuning->fSwingShakeAmount = 0.1f;
    gpCamTuning->fSwingShakeTime = 0.5f;
    gpCamTuning->fScreenDarkenAlpha = 0.25f;
    gpCamTuning->fGreenCamMaxScale = 2.0f;
    gpCamTuning->fGreenCamMoveInSpeed = 5.0f;
    gpCamTuning->fGreenCamScaleSpeed = 1.0f;
    gpCamTuning->fGreenCamOrbitSpeed = 0.8f;
    gpCamTuning->fGreenCamArcSide = 9.75f;
    gpCamTuning->fGreenCamClearance = 1.5f;
    gpCamTuning->fGreenCamAimRaise = 1.5f;
    gpCamTuning->bGreenCamLevelAim = 0;
    gpCamTuning->fRisingHeightShare = 0.25f;
    gpCamTuning->fFallingHeightShare = 0.05f;
    gpCamTuning->fUnread5 = 1.5f;
    gpCamTuning->fUnread6 = 1.5f;
    gpCamTuning->fTrackOffsetStartShare = 0.5f;
    gpCamTuning->fTrackOffsetStartScale = 1.0f;
    gpCamTuning->fTrackOffsetEndShare = 0.95f;
    gpCamTuning->fTrackOffsetEndScale = 0.0025f;
    gpCamTuning->nUnread2 = 0;
    gpCamTuning->fUnread7 = 50.0f;
    gpCamTuning->fUnread8 = 0.7f;
    gpCamTuning->fUnread9 = 0.005f;
}

// Free the camera tuning values; run when the front end and each round shut down.
void CameraTuning_Close(void) {
    Circular_MemFreeFunc(gpCamTuning);
    gpCamTuning = NULL;
}
