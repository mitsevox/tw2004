TW06 function names
===================

Names carried over from Tiger Woods PGA Tour 06 (2005): the Xbox beta PDB and the PS2 build's
linker map (`MAPFILE.TXT`, SLUS-21264, 2005-07-12). Same studio (EA Redwood Shores) and the same
engine lineage as this game. How they were found
and the tools are in `tw2004-notes.md`, "Symbols from related builds". The PDB and executable are
reference material and are not in this repository.

**These are 2005 names.** TW06 is two years newer and partly C++; this game is C. A name here says
"this function does the same job as TW06's X", not "this is what EA called it in 2003". `::` in a
C++ name is written `_` in `symbols.txt`.

- **strong** (applied to `symbols.txt`): both builds agree, or one build with shared text, 3+ shared
  float constants, or 3+ call-graph neighbours that agree, with the pairing mutual.
- **conflict**: the Xbox and PS2 builds give different names. Not applied.
- **medium** (not applied, suggestions only): 2 agreeing neighbours, a weak constant match the call
  graph confirms, or one call-order alignment. Apply one when the code agrees.

The PS2 build is optimized (like this game), so its call graph is closer to ours than the Xbox
debug build's; it is seeded with the Xbox pairs. Evidence codes: `anchor(n)` = n shared unique strings/constants; `nbr(rK,n)` = n agreeing
call-graph neighbours in propagation round K; `+graph` = a weak anchor confirmed by the call
graph; `calls` = the only unpaired callee between two aligned calls.

Checks against names we had already given by hand (all agree): `Ball_GroundContact` =
`Physics_BallRolling`, `Ball_CupPull` = `Physics_CupDynamics`, `Ball_Collide` =
`Physics_CheckTerrainCollisions`, `Ball_FlightStep` = `Physics_BallFlying`, `Ball_SetLie` =
`Physics_SetLie`, `Ball_Stop` = `Physics_StopBall`, `Ball_Holed` = `Physics_ForceBallInHole`,
`Ball_Launch` = `Physics_ShotImpact`, `Ball_Tick` = `Physics_QuickSimulate`, `SwingState01_Update` =
`STATEFUNC_PreShotUpdate`, `Vec_Distance` = `vec4flt_DistanceBetween3`. One wrong pairing found
during tuning (`Vec_Copy`, a 36-byte copy, paired with an accessor) led to the rule that
functions under 48 bytes are never named from call-graph evidence.

Functions still unnamed or named from TW06:

| Address | Now | TW06 name | Tier | Evidence |
|---|---|---|---|---|
| `80005628` | `Mem_cpy` | `Mem_cpy` | strong | PS2 nbr(r3,4) |
| `8000FBB0` | `TX_spParseTextureGroupFromStream` | `TX_spParseTextureGroupFromStream` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800107F4` | `FO_spLoadFontFromStream` | `FO_spLoadFontFromStream` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `80013950` | `RC_vUpdateRenderCtxScreenMatricesAndInfo` | `RC_vUpdateRenderCtxScreenMatricesAndInfo` | strong | both builds: Xbox anchor(2)+graph, PS2 xbox:anchor(2)+graph |
| `8001497C` | `StreamManagerHole_StreamFiles` | `StreamManagerHole::StreamFiles` | strong | PS2 anchor(1) |
| `80017320` | `sGetUserTextureIdx` | `sGetUserTextureIdx` | strong | Xbox anchor(1) |
| `800178C8` | `Character_UpdateFeetTerrainInfo` | `Character_UpdateFeetTerrainInfo` | strong | both builds: Xbox nbr2(r0,2), PS2 xbox:nbr2(r0,2) |
| `80017A40` | `Character_GetTerrainHeightAndNormal` | `Character_GetTerrainHeightAndNormal` | strong | both builds: Xbox anchor(1)+graph, PS2 xbox:anchor(1)+graph |
| `80017BB8` | `Character_UpdateTestPoints` | `Character_UpdateTestPoints` | strong | PS2 nbr(r6,3) |
| `80017EF4` | `Character_UpdateAnimation` | `Character_UpdateAnimation` | strong | PS2 nbr(r0,3) |
| `800187CC` | `Character_PlaceFeetOnGround` | `Character_PlaceFeetOnGround` | strong | both builds: Xbox anchor(2)+graph, PS2 xbox:anchor(2)+graph |
| `80018B8C` | `Character_IKLegToGround` | `Character_IKLegToGround` | strong | both builds: Xbox anchor(2)+graph, PS2 xbox:anchor(2)+graph |
| `80019234` | `Character_SetPosition` | `Character_SetPosition` | strong | PS2 nbr(r5,3) |
| `8001D95C` | `Character_GetBallOnFingerPosition` | `Character_GetBallOnFingerPosition` | strong | both builds: Xbox nbr2(r0,2), PS2 xbox:nbr2(r0,2) |
| `8001DFA8` | `Character_SetClubStatesForCharacter` | `Character_SetClubStatesForCharacter` | strong | both builds: Xbox anchor(12), PS2 xbox:anchor(12) |
| `8001EF78` | `vec4flt_CrossProduct` | `vec4flt_CrossProduct` | strong | Xbox nbr(r4,6) |
| `80027764` | `SKEL_SetIKSolutionWeight` | `SKEL_SetIKSolutionWeight` | strong | PS2 nbr(r2,3) |
| `80028AE0` | `SKEL_TransformBones` | `SKEL_TransformBones` | strong | PS2 nbr(r6,3) |
| `800290DC` | `SKEL_UpdateState` | `SKEL_UpdateState` | strong | PS2 nbr(r7,3) |
| `80037DD8` | `PostFx_CopyScreenToBuffer` | `PostFx_CopyScreenToBuffer` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8003B728` | `DynamicCam_GetLocation` | `DynamicCam_GetLocation` | strong | PS2 nbr2(r7,3) |
| `8003C2E0` | `DynamicCam_ChoosePreFlightSequence` | `DynamicCam_ChoosePreFlightSequence` | strong | PS2 nbr(r0,3) |
| `8004048C` | `CamScript_SplineCameras` | `CamScript_SplineCameras` | strong | PS2 nbr2(r6,3) |
| `80040EF0` | `CamScript_GetLookAtPoint` | `CamScript_GetLookAtPoint` | strong | PS2 nbr2(r4,4) |
| `80041A24` | `CameraScript_LagAimMarker` | `CameraScript_LagAimMarker` | strong | PS2 nbr2(r3,3) |
| `80042604` | `CameraScript_RecordCurrentCam` | `CameraScript_RecordCurrentCam` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800427E8` | `CameraScript_UpdateLandingEstimate` | `CameraScript_UpdateLandingEstimate` | strong | PS2 nbr2(r6,3) |
| `80042B6C` | `CameraScript_GoToNewScript` | `CameraScript_GoToNewScript` | strong | PS2 nbr2(r5,3) |
| `80042EDC` | `CameraScript_InterpToNewScript` | `CameraScript_InterpToNewScript` | strong | both builds: Xbox nbr(r0,2), PS2 xbox:nbr(r0,2) |
| `800437F8` | `CameraScript_IsDefaultSwingCam` | `CameraScript_IsDefaultSwingCam` | strong | both builds: Xbox calls(r0,1), PS2 xbox:calls(r0,1) |
| `80043AFC` | `CamScript_CheckOutOfBounds` | `CamScript_CheckOutOfBounds` | strong | PS2 nbr2(r6,3) |
| `800442E8` | `CamScript_PutBackOnFairway` | `CamScript_PutBackOnFairway` | strong | PS2 nbr2(r5,4) |
| `800444BC` | `CamScript_UpdateFairwayCam` | `CamScript_UpdateFairwayCam` | strong | PS2 nbr2(r5,4) |
| `80044BD4` | `CamScript_KeepAboveGround` | `CamScript_KeepAboveGround` | strong | PS2 nbr2(r8,4) |
| `80045038` | `CameraScript_WillGolferBeOccludedInThisView` | `CameraScript_WillGolferBeOccludedInThisView` | strong | PS2 nbr(r4,3) |
| `80045908` | `DF_vDrawBufferToScreen` | `DF_vDrawBufferToScreen` | strong | PS2 anchor(3) |
| `80048AF4` | `Kernel_InitModule` | `Kernel_InitModule` | strong | PS2 anchor(1) |
| `80049B40` | `ActAnimal_SetWorldMatrix` | `ActAnimal_SetWorldMatrix` | strong | Xbox anchor(5) |
| `8004B5A8` | `Ter_PointInFreeDropNetwork` | `Ter_PointInFreeDropNetwork` | strong | PS2 nbr(r8,4) |
| `8004B664` | `Ter_PointInOOBNetwork` | `Ter_PointInOOBNetwork` | strong | PS2 nbr(r5,4) |
| `8004B89C` | `Ter_CheckObjectAndHazardObstruction` | `Ter_CheckObjectAndHazardObstruction` | strong | both builds: Xbox calls(r0,1), PS2 xbox:calls(r0,1) |
| `8004C134` | `Ter_SearchAreaForDropLocation` | `Ter_SearchAreaForDropLocation` | strong | both builds: Xbox anchor(1)+graph, PS2 xbox:anchor(1)+graph |
| `8004C590` | `Ter_CheckForDropLocation` | `Ter_CheckForDropLocation` | strong | both builds: Xbox nbr2(r0,2), PS2 xbox:nbr2(r0,2) |
| `8004C798` | `Ter_IsValidDropSurface` | `Ter_IsValidDropSurface` | strong | both builds: Xbox nbr2(r6,2), PS2 xbox:nbr2(r6,2) |
| `8004D734` | `Ter_GetSupportingGroundNormal` | `Ter_GetSupportingGroundNormal` | strong | both builds: Xbox nbr(r5,2), PS2 xbox:nbr(r5,2) |
| `8004D838` | `Ter_GetSupportingWorldMaterial` | `Ter_GetSupportingWorldMaterial` | strong | both builds: Xbox nbr2(r3,2), PS2 xbox:nbr2(r3,2) |
| `8004D890` | `Ter_GetSupportingGroundData` | `Ter_GetSupportingGroundData` | strong | both builds: Xbox nbr(r5,2), PS2 xbox:nbr(r5,2) |
| `8004D9A8` | `Ter_GetEnclosingGroundHeight` | `Ter_GetEnclosingGroundHeight` | strong | PS2 nbr2(r0,3) |
| `8004D9E0` | `Ter_GetEnclosingGroundData` | `Ter_GetEnclosingGroundData` | strong | both builds: Xbox nbr(r5,2), PS2 xbox:nbr(r5,2) |
| `8004E1B0` | `Ter_CheckForPinCollision` | `Ter_CheckForPinCollision` | strong | both builds: Xbox anchor(1)+graph, PS2 xbox:anchor(1)+graph |
| `8004F6F4` | `Ter_CheckForGroundCollision` | `Ter_CheckForGroundCollision` | strong | PS2 nbr2(r7,3) |
| `80050CAC` | `Physics_OutOfBounds` | `Physics_OutOfBounds` | strong | Xbox nbr(r3,4) |
| `800512BC` | `Physics_GetShotData` | `Physics_GetShotData` | strong | both builds: Xbox anchor(4), PS2 xbox:anchor(4) |
| `80052088` | `Physics_GetSurfaceInfo` | `Physics_GetSurfaceInfo` | strong | Xbox nbr(r6,4) |
| `80052598` | `Physics_HandleCollision` | `Physics_HandleCollision` | strong | Xbox anchor(6) |
| `800539F8` | `Physics_ProcessCollision` | `Physics_ProcessCollision` | strong | Xbox anchor(4) |
| `8005418C` | `Physics_FixBallHeight` | `Physics_FixBallHeight` | strong | Xbox nbr(r3,3) |
| `8005567C` | `Physics_Simulate` | `Physics_Simulate` | strong | both builds: Xbox nbr2(r0,2), PS2 xbox:nbr2(r0,2) |
| `8005587C` | `Physics_DropBall` | `Physics_DropBall` | strong | both builds: Xbox nbr2(r2,2), PS2 xbox:nbr2(r2,2) |
| `8005620C` | `PictInt_Decode` | `PictInt_Decode` | strong | Xbox anchor(1) |
| `800564D8` | `PasswordManager_TestPassword` | `PasswordManager::TestPassword` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `80057F18` | `FE_CrAP_InitCrAPInfo` | `FE_CrAP_InitCrAPInfo` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `80062F38` | `CameraController_Idle` | `CameraController_Idle` | strong | PS2 nbr(r5,3) |
| `80067074` | `EVENT_Trigger` | `EVENT_Trigger` | strong | both builds: Xbox nbr(r1,4), PS2 xbox:nbr(r1,4) |
| `800694A0` | `PlaceBall_UpdateMomentums` | `PlaceBall_UpdateMomentums` | strong | both builds: Xbox nbr2(r4,2), PS2 xbox:nbr2(r4,2) |
| `8006AD68` | `Emotion_UpdatePlayerEmotion` | `Emotion_UpdatePlayerEmotion` | strong | PS2 nbr(r3,3) |
| `8006CBDC` | `GO_vInitIG` | `GO_vInitIG` | strong | PS2 nbr(r0,4) |
| `80077030` | `FE_MakeMoviePath` | `FE_MakeMoviePath` | strong | PS2 anchor(1) |
| `800770AC` | `FE_MakeCameoMoviePath` | `FE_MakeCameoMoviePath` | strong | Xbox anchor(1) |
| `80077168` | `FE_GetBIOMovieName` | `FE_GetBIOMovieName` | strong | Xbox anchor(1) |
| `80078840` | `FE_CrAP_IsAssetUndesirable` | `FE_CrAP_IsAssetUndesirable` | strong | Xbox anchor(10) |
| `80078C94` | `FE_CrAP_IsCrazyHat` | `FE_CrAP_IsCrazyHat` | strong | Xbox anchor(2) |
| `8007BE28` | `GetGolferName` | `GetGolferName` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8008BAB4` | `sFE_AdjustAndSetGolferPosition` | `sFE_AdjustAndSetGolferPosition` | strong | Xbox anchor(4) |
| `80093A80` | `BFX_vInit` | `BFX_vInit` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `80095B4C` | `CharacterState_AddSKABlendData` | `CharacterState_AddSKABlendData` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `80096690` | `CharacterState_UpdateSKAState` | `CharacterState_UpdateSKAState` | strong | both builds: Xbox nbr2(r2,4), PS2 xbox:nbr2(r2,4) |
| `8009B630` | `GR_vInit` | `GR_vInit` | strong | PS2 anchor(1) |
| `800A2C3C` | `PsBallFx_InitModule` | `PsBallFx_InitModule` | strong | PS2 anchor(1) |
| `800A3980` | `PsBallFx_TriggerTrail` | `PsBallFx_TriggerTrail` | strong | both builds: Xbox nbr(r6,3), PS2 xbox:nbr(r6,3) |
| `800A8934` | `Mov_Exit` | `Mov_Exit` | strong | Xbox anchor(1) |
| `800AE1F8` | `UI_Obj_InitModule` | `UI_Obj_InitModule` | strong | PS2 anchor(1) |
| `800B251C` | `SH_vSetShadowIntensity` | `SH_vSetShadowIntensity` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800B4E28` | `SD_vShaderObject_Rain_Dynamic_Init` | `SD_vShaderObject_Rain_Dynamic_Init` | strong | Xbox anchor(1) |
| `800BB42C` | `SitDev_LoadScripts` | `SitDev_LoadScripts` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800BDE54` | `GolfCamera_ProcessZoomToAimCamera` | `GolfCamera_ProcessZoomToAimCamera` | strong | PS2 nbr2(r1,4) |
| `800BE504` | `GolfCamera_InitGreenZoomToAimCamera` | `GolfCamera_InitGreenZoomToAimCamera` | strong | both builds: Xbox nbr(r7,2), PS2 xbox:nbr(r7,2) |
| `800BE828` | `GolfCamera_ProcessGreenZoomToAimCamera` | `GolfCamera_ProcessGreenZoomToAimCamera` | strong | PS2 nbr2(r2,3) |
| `800BEEB0` | `GolfCamera_InitSteepSlopeCamera` | `GolfCamera_InitSteepSlopeCamera` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800BEFAC` | `GolfCamera_InitElevatorCamera` | `GolfCamera_InitElevatorCamera` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800C0A30` | `GolfCamera_InitPreShotCamera` | `GolfCamera_InitPreShotCamera` | strong | both builds: Xbox nbr2(r4,3), PS2 xbox:nbr2(r4,3) |
| `800C0D70` | `GolfCamera_InitSwingCamera` | `GolfCamera_InitSwingCamera` | strong | PS2 nbr(r5,4) |
| `800C1824` | `GolfCamera_InitHeartBeatCamera` | `GolfCamera_InitHeartBeatCamera` | strong | PS2 nbr(r6,3) |
| `800C1A08` | `GolfCamera_ProcessHeartBeatCamera` | `GolfCamera_ProcessHeartBeatCamera` | strong | PS2 nbr(r5,3) |
| `800C1C50` | `GolfCamera_InitShutterCamera` | `GolfCamera_InitShutterCamera` | strong | PS2 nbr(r8,3) |
| `800C2004` | `GolfCamera_InitBallFlightCamera` | `GolfCamera_InitBallFlightCamera` | strong | PS2 nbr(r0,3) |
| `800C2C80` | `GolfCamera_InitPostShotCamera` | `GolfCamera_InitPostShotCamera` | strong | PS2 nbr(r6,3) |
| `800C2ED8` | `GolfCamera_ProcessPostShotCamera` | `GolfCamera_ProcessPostShotCamera` | strong | PS2 nbr(r5,3) |
| `800C353C` | `GolfCamera_InitTutorialWaitCamera` | `GolfCamera_InitTutorialWaitCamera` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `800C3C40` | `GolfCamera_SwitchCrAPCamera` | `GolfCamera_SwitchCrAPCamera` | strong | PS2 nbr2(r8,3) |
| `800C4724` | `GolfCamera_ComputeSteepSlopeCamVectors` | `GolfCamera_ComputeSteepSlopeCamVectors` | strong | both builds: Xbox nbr(r8,2), PS2 xbox:nbr(r8,2) |
| `800C6358` | `GolfCamera_CutToGolferDoneAnimatingCam` | `GolfCamera_CutToGolferDoneAnimatingCam` | strong | PS2 nbr(r4,4) |
| `800DAE84` | `GameEffects_ResetGameEffectSettings` | `GameEffects_ResetGameEffectSettings` | strong | PS2 nbr(r3,3) |
| `800DB1C4` | `GameEffects_BallUpdatesThisFrame` | `GameEffects_BallUpdatesThisFrame` | strong | PS2 nbr(r3,3) |
| `800DC524` | `GameEffects_SetSuperSlowMo` | `GameEffects_SetSuperSlowMo` | strong | PS2 nbr(r7,4) |
| `800DCE5C` | `GM_EndOfGolferTurn` | `GM_EndOfGolferTurn` | strong | PS2 nbr(r6,3) |
| `800DDDC8` | `GM_PlayerTakeMulligan` | `GM_PlayerTakeMulligan` | strong | PS2 nbr(r4,3) |
| `800DE180` | `GM_ShowPostShotAnimation` | `GM_ShowPostShotAnimation` | strong | both builds: Xbox nbr2(r4,2), PS2 xbox:nbr2(r4,2) |
| `800DED60` | `GM_MovePlayerToBall` | `GM_MovePlayerToBall` | strong | PS2 nbr2(r1,3) |
| `800DEE4C` | `GM_CheckForShotChanges` | `GM_CheckForShotChanges` | strong | PS2 nbr(r4,3) |
| `800DF280` | `GM_DoPostShotInHoleUI` | `GM_DoPostShotInHoleUI` | strong | PS2 nbr(r5,4) |
| `800DF824` | `GM_SimulateBallMovement` | `GM_SimulateBallMovement` | strong | both builds: Xbox nbr2(r4,2), PS2 xbox:nbr2(r4,2) |
| `800E276C` | `GameManager_GetHoleName` | `GameManager::GetHoleName` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8010416C` | `sTurnOnLogo` | `sTurnOnLogo` | strong | Xbox anchor(2) |
| `80104398` | `FE_CrAP_TurnOnAsset` | `FE_CrAP_TurnOnAsset` | strong | both builds: Xbox anchor(2), PS2 xbox:anchor(2) |
| `80104700` | `FE_CrAP_TurnOnPart` | `FE_CrAP_TurnOnPart` | strong | both builds: Xbox nbr(r0,2), PS2 xbox:nbr(r0,2) |
| `80105F00` | `FE_CheckSpecialCaseConnections` | `FE_CheckSpecialCaseConnections` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8010D5BC` | `CharSlider_CreateDefinitionsFromMem` | `CharSlider_CreateDefinitionsFromMem` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8011CEB0` | `PrintStatsWithUnits` | `PrintStatsWithUnits` | strong | Xbox anchor(1) |
| `8011E0AC` | `RTClock_GetDateTimeString` | `RTClock_GetDateTimeString` | strong | Xbox anchor(1) |
| `8011EFE4` | `GrassRender_vBuildAndUploadOneTimeData` | `GrassRender_vBuildAndUploadOneTimeData` | strong | Xbox anchor(1) |
| `801220D4` | `TibExtMemAlloc` | `TibExtMemAlloc` | strong | both builds: Xbox anchor(1), PS2 xbox:anchor(1) |
| `8000C140` | `fn_8000C140` | `wn_PnPoly` | medium | PS2 nbr(r7,2) |
| `8000C594` | `fn_8000C594` | `Ter_TerrainGameDataMgr::GetTGD` | medium | PS2 calls(r0,8) |
| `8001966C` | `fn_8001966C` | `Character_AlignCharacterForShotImpact` | medium | PS2 nbr(r1,2) |
| `8001FCF4` | `fn_8001FCF4` | `SKA_Update` | medium | PS2 nbr(r4,2) |
| `80026BF4` | `fn_80026BF4` | `SKEL_TransformIKChain` | medium | Xbox calls(r0,1) |
| `80026D18` | `fn_80026D18` | `SKEL_ItterateIKChain` | medium | Xbox anchor(1)+graph |
| `80027964` | `fn_80027964` | `SKEL_TransitionIK` | medium | PS2 calls(r0,1) |
| `80027E28` | `fn_80027E28` | `SKEL_TranslateIKChainY` | medium | PS2 calls(r0,1) |
| `80027E8C` | `fn_80027E8C` | `SKEL_AdjustHipPosition` | medium | Xbox anchor(1)+graph |
| `800280E8` | `fn_800280E8` | `SKEL_InitIKSkeleton` | medium | PS2 nbr(r3,2) |
| `80028564` | `fn_80028564` | `_HLBListAddChangeBuddy` | medium | PS2 nbr(r1,2) |
| `80030254` | `fn_80030254` | `Ter_vInitModule` | medium | PS2 anchor(1)+graph |
| `8003A7C8` | `fn_8003A7C8` | `DynamicCam_ChooseScript` | medium | PS2 calls(r0,1) |
| `8003DCE8` | `fn_8003DCE8` | `CamScript_RunScript` | medium | PS2 nbr(r3,2) |
| `8003E624` | `fn_8003E624` | `CamScript_RunFEScript` | medium | PS2 nbr2(r7,2) |
| `8003EA50` | `fn_8003EA50` | `CamScript_RunFlybyCamera` | medium | PS2 nbr2(r5,2) |
| `80040CF0` | `fn_80040CF0` | `CamScript_PickArcDirection` | medium | PS2 nbr2(r5,2) |
| `800439E4` | `fn_800439E4` | `CamScript_CheckObstructedCamera` | medium | PS2 nbr(r4,2) |
| `80043C74` | `fn_80043C74` | `CamScript_GetCameraOnFairwayPos` | medium | PS2 nbr2(r6,2) |
| `80052268` | `fn_80052268` | `Physics_BallRollingandSlipping` | medium | Xbox nbr(r5,2) |
| `80053240` | `fn_80053240` | `Physics_CheckObjectCollisions` | medium | Xbox nbr(r2,2) |
| `80054040` | `fn_80054040` | `Physics_CheckActorCollisions` | medium | Xbox nbr(r0,2) |
| `80055324` | `fn_80055324` | `Physics_GetBallAltitude` | medium | Xbox nbr(r4,2) |
| `800642D0` | `fn_800642D0` | `CameraController_ResetAimMarkerInSwingCamera` | medium | PS2 nbr(r7,2) |
| `80069218` | `fn_80069218` | `PlaceBall_IsValidDropLocation` | medium | Xbox nbr2(r5,2) |
| `80069330` | `fn_80069330` | `PlaceBall_Set` | medium | PS2 calls(r0,1) |
| `800693A4` | `fn_800693A4` | `PlaceBall_ResetMomentums` | medium | PS2 nbr(r7,2) |
| `80069428` | `fn_80069428` | `PlaceBall_CheckInBounds` | medium | PS2 nbr(r6,2) |
| `8006ACF8` | `fn_8006ACF8` | `Emotion_PostBallEvent` | medium | PS2 nbr(r6,2) |
| `8006C300` | `fn_8006C300` | `REPLAY_Play` | medium | PS2 nbr(r4,2) |
| `8006C4C0` | `fn_8006C4C0` | `REPLAY_Restore` | medium | PS2 nbr(r5,2) |
| `80072ED8` | `fn_80072ED8` | `SKATime_Update` | medium | PS2 nbr2(r0,2) |
| `80073108` | `fn_80073108` | `SKATime_Idle` | medium | PS2 anchor(1)+graph |
| `80076400` | `fn_80076400` | `CA_spCreateCamera` | medium | PS2 nbr(r1,2) |
| `800768E0` | `fn_800768E0` | `CA_vUpdateInternalFieldOfViewData` | medium | PS2 nbr(r2,2) |
| `80077A80` | `fn_80077A80` | `FE_spGetGolfer` | medium | PS2 calls(r0,1) |
| `80078A2C` | `fn_80078A2C` | `FE_CrAP_RandomizeCategoryWithUndesirableTest` | medium | Xbox nbr(r1,2) |
| `80091EE8` | `fn_80091EE8` | `FE_movieFade` | medium | PS2 nbr(r0,2) |
| `800954A4` | `fn_800954A4` | `TI_sReadCounter` | medium | PS2 calls(r0,1) |
| `80096530` | `fn_80096530` | `CharacterState_UpdateGameEmotionState` | medium | PS2 calls(r0,1) |
| `8009C0BC` | `fn_8009C0BC` | `GR_BuildGridRenderData` | medium | PS2 anchor(1)+graph |
| `800B52D4` | `fn_800B52D4` | `SD_vShaderObject_Rain_Dynamic_Refresh` | medium | Xbox anchor(2)+graph |
| `800BB6FC` | `fn_800BB6FC` | `SitDev_GetCommentaryZones` | medium | PS2 nbr2(r1,2) |
| `800BCA60` | `fn_800BCA60` | `modifyFinalLie` | medium | PS2 nbr2(r1,2) |
| `800BDC20` | `fn_800BDC20` | `GolfCamera_InitZoomToAimCamera` | medium | PS2 nbr(r1,2) |
| `800BEF28` | `fn_800BEF28` | `GolfCamera_ProcessSteepSlopeCamera` | medium | PS2 nbr(r4,2) |
| `800C2514` | `fn_800C2514` | `GolfCamera_ProcessBallFlightCamera` | medium | PS2 nbr(r0,2) |
| `800C324C` | `fn_800C324C` | `GolfCamera_InitInHoleCamera` | medium | PS2 nbr(r3,2) |
| `800C336C` | `fn_800C336C` | `GolfCamera_ProcessInHoleCamera` | medium | PS2 nbr(r4,2) |
| `800C6D9C` | `fn_800C6D9C` | `GolfCamera_IsFreezeTimeActive` | medium | PS2 nbr2(r7,2) |
| `800D1530` | `fn_800D1530` | `GameAnalysis_GetSidehillLie` | medium | PS2 anchor(1)+graph |
| `800DD904` | `GM_BumpBallForObstructions` | `GM_BumpBallForObstructions` | medium | PS2 nbr(r0,2) |
| `800DDFB4` | `fn_800DDFB4` | `GM_DoPreshotAnimation` | medium | PS2 nbr(r1,2) |
| `800DEA44` | `GM_ReplaceOOBBall` | `GM_ReplaceOOBBall` | medium | PS2 nbr(r4,2) |
| `800E53B8` | `fn_800E53B8` | `GUI_IsAwardUIAnimating` | medium | PS2 calls(r0,2) |
| `80114540` | `fn_80114540` | `DynChain_DoBoobSimulation` | medium | Xbox anchor(1)+graph |
| `8011E020` | `fn_8011E020` | `GetLocalTime` | medium | Xbox calls(r1,1) |
| `8007706C` | `fn_8007706C` | - | conflict | Xbox: sfsnprintf calls(r0,1) / PS2: FE_MakeMoviePathWithSubDir anchor(1) |
| `800C4AB0` | `GolfCamera_ClampLookAngle` | - | conflict | Xbox: GolfCamera_ClampLookAngle nbr(r9,3) / PS2: CameraController_BallIsOnScreen nbr2(r8,2) |
| `800BAF04` | `fn_800BAF04` | ~~-~~ | rejected | Xbox: vec4flt_LengthSquared3 nbr(r7,11) / PS2: GetGamePlayerCoreShotInfo nbr(r1,7); our code shows it normalises a vector |

Functions we had named by hand, with their TW06 equivalents (our names kept):

| Address | Our name | TW06 name | Tier | Evidence |
|---|---|---|---|---|
| `8000B130` | `Rand_Next` | `Misc_RandFunc` | strong | both builds: Xbox nbr2(r3,3), PS2 xbox:nbr2(r3,3) |
| `8000C968` | `UStream_BeginObject` | `Stream_InitModule` | strong | PS2 anchor(1) |
| `8000CD60` | `UStream_StripStreamExt` | `Stream_RemoveExtensionInStreamName` | strong | Xbox anchor(1) |
| `8002B030` | `AI_RehearseShot` | `AIBrain::Think` | strong | both builds: Xbox nbr2(r2,2), PS2 xbox:nbr2(r2,2) |
| `80051AF4` | `Ball_Launch` | `Physics_ShotImpact` | strong | both builds: Xbox nbr(r2,2), PS2 xbox:nbr(r2,2) |
| `80054340` | `Ball_Stop` | `Physics_StopBall` | strong | Xbox nbr(r2,4) |
| `80054450` | `Ball_Collide` | `Physics_CheckTerrainCollisions` | strong | Xbox anchor(3) |
| `80054AB0` | `Ball_CupPull` | `Physics_CupDynamics` | strong | Xbox anchor(4) |
| `80054D28` | `Ball_GroundContact` | `Physics_BallRolling` | strong | Xbox anchor(9) |
| `8005A080` | `Swing_LoadTuning` | `SW_vUIInit` | strong | both builds: Xbox anchor(2), PS2 anchor(2) |
| `8005AEE0` | `Swing_Launch` | `SW_vImpact` | strong | PS2 nbr(r5,3) |
| `8005D278` | `SwingStack_Top` | `GOLFERSTATE_GetCurrentState` | strong | PS2 nbr(r2,3) |
| `8005D44C` | `SwingState01_Enter` | `STATEFUNC_PreShotInit` | strong | PS2 nbr(r2,4) |
| `8005D7A0` | `SwingState01_Update` | `STATEFUNC_PreShotUpdate` | strong | both builds: Xbox nbr2(r1,2), PS2 xbox:nbr2(r1,2) |
| `8005DED0` | `SwingState02_Enter` | `STATEFUNC_ShotSetupInit` | strong | PS2 nbr(r4,5) |
| `8005E2F4` | `SwingState10_Enter` | `STATEFUNC_SwingInit` | strong | PS2 nbr(r5,4) |
| `8005E6F8` | `SwingState10_Update` | `STATEFUNC_SwingUpdate` | strong | PS2 nbr(r6,3) |
| `8005EB60` | `SwingState11_Enter` | `STATEFUNC_ReplaySwingInit` | strong | both builds: Xbox nbr(r1,2), PS2 xbox:nbr(r1,2) |
| `8005EE38` | `SwingState11_Update` | `STATEFUNC_ReplaySwingUpdate` | strong | PS2 nbr(r6,3) |
| `8005F978` | `SwingState06_Enter` | `STATEFUNC_GreenWatchRollInit` | strong | PS2 nbr(r6,3) |
| `8006055C` | `SwingState12_Enter` | `STATEFUNC_SimulateInit` | strong | PS2 nbr(r7,3) |
| `80060688` | `SwingState12_Update` | `STATEFUNC_SimulateUpdate` | strong | PS2 nbr(r5,4) |
| `80060F84` | `SwingState14_Update` | `STATEFUNC_ShowYardageUpdate` | strong | PS2 nbr(r3,3) |
| `80061A54` | `SwingState18_Enter` | `STATEFUNC_RemoveBallInit` | strong | PS2 nbr(r4,3) |
| `80061B44` | `SwingState18_Update` | `Character_UpdateBallAnimation` | strong | both builds: Xbox anchor(1)+graph, PS2 xbox:anchor(1)+graph |
| `8006205C` | `SwingState20_Enter` | `STATEFUNC_InitialFlyByInit` | strong | PS2 nbr(r6,3) |
| `800626EC` | `SwingState22_Enter` | `STATEFUNC_PlaceBallInit` | strong | PS2 nbr(r6,3) |
| `80062900` | `SwingState22_Update` | `STATEFUNC_PlaceBallUpdate` | strong | both builds: Xbox nbr2(r3,2), PS2 xbox:nbr2(r3,2) |
| `800632E4` | `View_SetCamera` | `CameraController_SetCameraMode` | strong | both builds: Xbox nbr(r0,3), PS2 xbox:nbr(r0,3) |
| `800965DC` | `CharAnim_StartTapIn` | `CharacterState_SetTapInState` | strong | both builds: Xbox nbr2(r3,3), PS2 xbox:nbr2(r3,3) |
| `800BB050` | `Vec_Distance` | `vec4flt_DistanceBetween3` | strong | Xbox nbr(r5,6) |
| `800D0AA0` | `Hole_ScoreAfterTapIn` | `GameAnalysis_IsPuttFor` | strong | both builds: Xbox nbr2(r5,2), PS2 xbox:nbr2(r5,2) |
| `8016FE1C` | `SFIOBeginSave` | `SFIOCreateStart` | strong | PS2 nbr(r0,5) |
| `8000B428` | `Rand_Float` | `Physics_ComputeBallLieModifier` | medium | PS2 nbr(r1,2) |
| `8002DB80` | `Caddie_Start` | `AICore::CreatePhantom` | medium | PS2 nbr(r6,2) |
| `8002DC50` | `Caddie_Update` | `AICore::PhantomThink` | medium | PS2 nbr(r5,2) |
| `80050C2C` | `Ball_Holed` | `Physics_ForceBallInHole` | medium | Xbox anchor(1)+graph |
| `80051CD0` | `Ball_FlightStep` | `Physics_BallFlying` | medium | Xbox nbr(r3,2) |
| `80053594` | `Ball_SetLie` | `Physics_SetLie` | medium | Xbox nbr(r4,2) |
| `80054A10` | `Ball_DistanceToPin` | `Physics_GetDistanceToCup` | medium | Xbox nbr(r6,2) |
| `800554A8` | `Ball_Tick` | `Physics_QuickSimulate` | medium | Xbox nbr(r0,2) |
| `800589F8` | `Swing_Init` | `SW_vInitModule` | medium | PS2 nbr(r1,2) |
| `8005FA84` | `SwingState06_Update` | `STATEFUNC_GreenWatchRollUpdate` | medium | PS2 nbr(r4,2) |
| `80060250` | `SwingState09_Update` | `STATEFUNC_GreenMorphUpdate` | medium | PS2 nbr(r7,2) |
| `80060BC4` | `SwingState13_Update` | `STATEFUNC_InTheHoleUpdate` | medium | PS2 nbr(r4,2) |
| `800617A8` | `SwingState16_Update` | `STATEFUNC_TapInUpdate` | medium | PS2 nbr(r7,2) |
| `800BBB10` | `FE_GolferAttributes` | `SitDev_SetupStateVector` | medium | PS2 nbr2(r0,2) |

Structs
-------

TW06's types come from the Xbox PDB (`tpiread.py struct <name>`). Plain C structs lay out the
same on both machines, so a TW06 struct whose fields sit at our offsets (allowing for fields
EA added later) names ours. Only fields our code confirms are renamed; the rest get a comment.

| Ours | TW06 | Notes |
|---|---|---|
| `Ball` (`src/Ball.c`) | `PhysicsBall_t` | TW06 inserted `terrainHeight` at 0x58. |
| `SwingData` (`include/golfer.h`) | `SW_sSwingData` (0x648; ours 0x634) | TW06 added 4 bytes after 0x14, the second stick's fields after 0x2C (12 bytes) and two calibration fields after 0x37C, 0x18 in all. The swing-boost list at 0x49C has exactly TW06's size. |
| swing states | `SW_eSwingState` | idle 0, back 1, fidget at the top 2, down 3, follow 4, post 5, cancel 6. Our code sets 2 when the backswing settles, goes back to 1 when the stick moves and to 0 after 0.1 s near centre. |
| CPU shot shapes | `ShotShape_t` | normal 0, fade 1, draw 2, high 3, low 4, slice 5, hook 6: fade/slice share a sign in our code (+0.02/+0.04), draw/hook the other. We had 3 and 4 swapped: shape 3 picks `gTrajLoft[2]` = +5 degrees of launch angle, a high shot. |
| `GolferRecord` | `GolferData_t` (0x1F0; ours 0x140) | Same to 0x62, then TW06 is 4 bytes later (fields added in 0x62..0x6C). Renamed: `nModelID` (0x01, `Golfer_FindById` searches on it), `bAvailable` (0x8E; TW06 declares it `char`, and our code reads it with a signed cast), and `tier` shrinks to 12 with TW06's `stance[2]` after it. The outfit byte at 0x62 does not fit TW06's `ballID`, so 0x62..0x67 stay unnamed. |
| `Player` | `GamePlayer` (0xFE0; ours 0xEF8) | EA regrouped the player into sub-structs (`CoreShotInfo_t`, `GolferScore_t`, `GolferBonus_t`) in a different order, so only blocks match. Score: our strokes at 0x154 and holes won at 0x278 sit at `strokes` (+0) and `matchwins` (+0x124), and the first 0x200 bytes of `GolferScore_t` fill 0x154..0x354 exactly. Shot: 0x354..0x3B0 is `AIshot_t` (club, preferred clubs (8 here, 6 in TW06), direction, strength, type, face and path vectors, shape, perfect). TW06 names are in comments; our names are kept. |

SwingData checks that decided it:
- `nNumInBlurQueue` (was `n370`) counts the trail points drawn, capped at 25.
- The trail colour fields are blue, red, green in TW06 (0x484, 0x488, 0x48C). Our code writes the second into the vertex colour's red byte, the third into green and the first into blue, so TW06 is right and our earlier local names `r`/`g`/`b` were wrong. The trail is blue with the stick left of centre, yellow (red + green) right.
- `fPowerBoostDieTime` (was `fBackDown`): when it runs out, the power boost level is cleared.
- `bCanSpin` (was `bShotTaken`) gates the spin input.
- `fForwardSpin` / `fSideSpin`: the CPU sets the first from its distance error and the second from its aim error.
- `bDrawBoostUI`: the boost/spin display is drawn only when it is set.
- Not renamed (no evidence either way): `f10`/`f14` (TW06 has three floats there), the four stick rest fields at 0x380 (TW06 has six), and `unk630` (not in TW06).

Golfer states
-------------

Our `gSwingStates` is TW06's `sGolferStateEngineTable` (PS2 `0x00410690`): the same rows of
(init, update, exit) callbacks, read out of the PS2 executable with `ps2states.py` and named
from `MAPFILE.TXT`. All 23 of our states are now named `STATEFUNC_<State>Init/Update/Exit` and
the stack functions `GOLFERSTATE_*` (the `SwingStateNN_*` / `SwingStack_*` names in the table
above are the old ones). The state ids are `GS_*` in `include/golfer.h`.

| Ours | TW06 row | State | How it was placed |
|---|---|---|---|
| 1 | 1 | PreShot | shared callees (8 in init) |
| 2 | 4 | ShotSetup | sets camera 0 (`kCameraMode_ShotSetup`); matcher pairing |
| 3 | 5 | Zoom | camera 1 / 2 on a putt (`ZoomToAim` / `GreenZoomToAim`) |
| 4 | 6 | Elevator | camera 3 (`ElevatorCam`) |
| 5 | 7 | Green | camera 4 (`GreenCam`) |
| 6 | 8 | GreenWatchRoll | shared callees (3 / 4 / 2); the putt preview |
| 7 | 9 | GreenReversePutt | camera 6 (`ReversePuttCam`) |
| 8 | - | (KneeCam, our name) | camera 7 (`KneeCam`); no such state in TW06 |
| 9 | 10 | GreenMorph | shared callees; putt-line view |
| 10 | 11 | Swing | matcher pairing (init, update) |
| 11 | 12 | ReplaySwing | shared callees (6 / 4 / 2) |
| 12 | 13 | Simulate | shared callees (3 / 6) |
| 13 | 14 | InTheHole | the only row with no exit in both tables |
| 14 | 15 | ShowYardage | shared callees (9, and the exit's 1 is unique) |
| 15 | 16 | FadeToTapIn | entered only after `Gimme_Allowed` (this corrected our reading) |
| 16 | 17 | TapIn | plays animation 11, the tap-in |
| 17 | 18 | FadeToRemoveBall | TW06's update waits for the fade, then switches, like ours |
| 18 | 19 | RemoveBall | picks the ball out of the cup |
| 19 | 20 | Wait | position |
| 20 | 21 | InitialFlyBy | shared callees; the hole flyover |
| 21 | 22 | MidHoleFlyBy | position; a camera flyover |
| 22 | 23 | PlaceBall | shared callees (7 / 4) |
| 23 | 24 | Conceded | position; `Golfer.c` treats state 23 like a holed ball |

TW06 added `TrinityCutSceneNIS` and `CutSceneNIS` (rows 2, 3) and states from row 25 on
(ellipse orbit, psyche-out, inactivity, shot caller, Skills 18, camera debug). The camera
numbers match TW06's `CameraMode_t` up to 7 at least; higher ones differ.

The call-graph matcher had paired 22 of these functions before the table was read, and 21
agree with it. The exception: it named our state-18 update `Character_UpdateBallAnimation`.
Ours is the full pick-out-and-toss routine (0x3B8 bytes); by TW06 that code had moved into
`Character_UpdateBallAnimation` and `STATEFUNC_RemoveBallUpdate` is a small camera wrapper, so
the table name stands.

Stack functions: `GetCurrentState` (was `SwingStack_Top`), `GetPreviousState`, `Push`, `Pop`,
`Set` (clear, then push one), `Switch` (replace the top; our old name `SwingStack_Push` was
wrong), `Kill` (was `SwingStack_Clear`), `OpenONCE` (empty all stacks), `Update` (run every
player's current update).
