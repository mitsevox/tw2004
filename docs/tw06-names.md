TW06 function names
===================

Names carried over from the Tiger Woods PGA Tour 06 Xbox beta (2005-07-12) PDB, which comes from
the same studio (EA Redwood Shores) and the same engine lineage as this game. How they were found
and the tools are in `tw2004-notes.md`, "Symbols from related builds". The PDB and executable are
reference material and are not in this repository.

**These are 2005 names.** TW06 is two years newer and partly C++; this game is C. A name here says
"this function does the same job as TW06's X", not "this is what EA called it in 2003". `::` in a
C++ name is written `_` in `symbols.txt`.

- **strong** (applied to `symbols.txt`): shared text, 3+ shared float constants, or 3+ call-graph
  neighbours that agree, with the pairing mutual.
- **medium** (not applied, suggestions only): 2 agreeing neighbours, a weak constant match the call
  graph confirms, or one call-order alignment. Apply one when the code agrees.

Evidence codes: `anchor(n)` = n shared unique strings/constants; `nbr(rK,n)` = n agreeing
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

| Address | Was | TW06 name | Tier | Evidence | Size GC / Xbox |
|---|---|---|---|---|---|
| `8000FBB0` | `fn_8000FBB0` | `TX_spParseTextureGroupFromStream` | strong | anchor(1) | 1020 / 1522 |
| `800107F4` | `fn_800107F4` | `FO_spLoadFontFromStream` | strong | anchor(1) | 2112 / 2226 |
| `80017320` | `fn_80017320` | `sGetUserTextureIdx` | strong | anchor(1) | 120 / 90 |
| `8001DFA8` | `fn_8001DFA8` | `Character_SetClubStatesForCharacter` | strong | anchor(12) | 2100 / 3513 |
| `8001EF78` | `fn_8001EF78` | `vec4flt_CrossProduct` | strong | nbr(r4,6) | 60 / 90 |
| `80037DD8` | `fn_80037DD8` | `PostFx_CopyScreenToBuffer` | strong | anchor(1) | 120 / 834 |
| `80042604` | `fn_80042604` | `CameraScript_RecordCurrentCam` | strong | anchor(1) | 484 / 475 |
| `80049B40` | `fn_80049B40` | `ActAnimal_SetWorldMatrix` | strong | anchor(5) | 1548 / 1975 |
| `80050CAC` | `fn_80050CAC` | `Physics_OutOfBounds` | strong | nbr(r3,4) | 120 / 82 |
| `800512BC` | `fn_800512BC` | `Physics_GetShotData` | strong | anchor(4) | 1884 / 1993 |
| `80052088` | `fn_80052088` | `Physics_GetSurfaceInfo` | strong | nbr(r6,4) | 480 / 842 |
| `80052598` | `fn_80052598` | `Physics_HandleCollision` | strong | anchor(6) | 3240 / 4561 |
| `800539F8` | `fn_800539F8` | `Physics_ProcessCollision` | strong | anchor(4) | 1184 / 2002 |
| `8005418C` | `fn_8005418C` | `Physics_FixBallHeight` | strong | nbr(r3,3) | 436 / 355 |
| `8005620C` | `fn_8005620C` | `PictInt_Decode` | strong | anchor(1) | 440 / 482 |
| `800564D8` | `fn_800564D8` | `PasswordManager::TestPassword` | strong | anchor(1) | 1716 / 2404 |
| `80057F18` | `fn_80057F18` | `FE_CrAP_InitCrAPInfo` | strong | anchor(1) | 164 / 167 |
| `80067074` | `fn_80067074` | `EVENT_Trigger` | strong | nbr(r1,4) | 52 / 97 |
| `800770AC` | `fn_800770AC` | `FE_MakeCameoMoviePath` | strong | anchor(1) | 40 / 44 |
| `80077168` | `fn_80077168` | `FE_GetBIOMovieName` | strong | anchor(1) | 376 / 249 |
| `80078840` | `fn_80078840` | `FE_CrAP_IsAssetUndesirable` | strong | anchor(10) | 492 / 440 |
| `80078C94` | `fn_80078C94` | `FE_CrAP_IsCrazyHat` | strong | anchor(2) | 144 / 203 |
| `8007BE28` | `fn_8007BE28` | `GetGolferName` | strong | anchor(1) | 196 / 172 |
| `8008BAB4` | `fn_8008BAB4` | `sFE_AdjustAndSetGolferPosition` | strong | anchor(4) | 3472 / 239 |
| `80093A80` | `fn_80093A80` | `BFX_vInit` | strong | anchor(1) | 84 / 230 |
| `80095B4C` | `fn_80095B4C` | `CharacterState_AddSKABlendData` | strong | anchor(1) | 1156 / 1630 |
| `80096690` | `fn_80096690` | `CharacterState_UpdateSKAState` | strong | nbr2(r2,4) | 2172 / 2115 |
| `800A3980` | `fn_800A3980` | `PsBallFx_TriggerTrail` | strong | nbr(r6,3) | 260 / 399 |
| `800A8934` | `fn_800A8934` | `Mov_Exit` | strong | anchor(1) | 180 / 138 |
| `800B251C` | `fn_800B251C` | `SH_vSetShadowIntensity` | strong | anchor(1) | 448 / 103 |
| `800B4E28` | `fn_800B4E28` | `SD_vShaderObject_Rain_Dynamic_Init` | strong | anchor(1) | 252 / 1279 |
| `800BAF04` | `fn_800BAF04` | ~~`vec4flt_LengthSquared3`~~ | **rejected** | nbr(r7,11); our code shows it normalises a vector (src, dst), which a squared-length function cannot | 84 / 89 |
| `800BB42C` | `fn_800BB42C` | `SitDev_LoadScripts` | strong | anchor(1) | 132 / 298 |
| `800BEEB0` | `fn_800BEEB0` | `GolfCamera_InitSteepSlopeCamera` | strong | anchor(1) | 120 / 78 |
| `800BEFAC` | `fn_800BEFAC` | `GolfCamera_InitElevatorCamera` | strong | anchor(1) | 232 / 146 |
| `800C0A30` | `fn_800C0A30` | `GolfCamera_InitPreShotCamera` | strong | nbr2(r4,3) | 476 / 398 |
| `800C353C` | `fn_800C353C` | `GolfCamera_InitTutorialWaitCamera` | strong | anchor(1) | 704 / 558 |
| `800C4AB0` | `fn_800C4AB0` | `GolfCamera_ClampLookAngle` | strong | nbr(r9,3) | 636 / 1147 |
| `800E276C` | `fn_800E276C` | `GameManager::GetHoleName` | strong | anchor(1) | 60 / 67 |
| `8010416C` | `fn_8010416C` | `sTurnOnLogo` | strong | anchor(2) | 356 / 399 |
| `80104398` | `fn_80104398` | `FE_CrAP_TurnOnAsset` | strong | anchor(2) | 872 / 1645 |
| `80105F00` | `fn_80105F00` | `FE_CheckSpecialCaseConnections` | strong | anchor(1) | 248 / 117 |
| `8010D5BC` | `fn_8010D5BC` | `CharSlider_CreateDefinitionsFromMem` | strong | anchor(1) | 1752 / 2251 |
| `8011CEB0` | `fn_8011CEB0` | `PrintStatsWithUnits` | strong | anchor(1) | 180 / 304 |
| `8011E0AC` | `fn_8011E0AC` | `RTClock_GetDateTimeString` | strong | anchor(1) | 196 / 116 |
| `8011EFE4` | `fn_8011EFE4` | `GrassRender_vBuildAndUploadOneTimeData` | strong | anchor(1) | 912 / 1463 |
| `801220D4` | `fn_801220D4` | `TibExtMemAlloc` | strong | anchor(1) | 84 / 93 |
| `80013950` | `fn_80013950` | `RC_vUpdateRenderCtxScreenMatricesAndInfo` | medium | anchor(2)+graph | 892 / 1117 |
| `800178C8` | `fn_800178C8` | `Character_UpdateFeetTerrainInfo` | medium | nbr2(r0,2) | 376 / 313 |
| `80017A40` | `fn_80017A40` | `Character_GetTerrainHeightAndNormal` | medium | anchor(1)+graph | 376 / 383 |
| `800187CC` | `fn_800187CC` | `Character_PlaceFeetOnGround` | medium | anchor(2)+graph | 464 / 904 |
| `80018B8C` | `fn_80018B8C` | `Character_IKLegToGround` | medium | anchor(2)+graph | 1704 / 2985 |
| `8001D95C` | `fn_8001D95C` | `Character_GetBallOnFingerPosition` | medium | nbr2(r0,2) | 168 / 260 |
| `80026BF4` | `fn_80026BF4` | `SKEL_TransformIKChain` | medium | calls(r0,1) | 292 / 675 |
| `80026D18` | `fn_80026D18` | `SKEL_ItterateIKChain` | medium | anchor(1)+graph | 632 / 1145 |
| `80027E8C` | `fn_80027E8C` | `SKEL_AdjustHipPosition` | medium | anchor(1)+graph | 604 / 550 |
| `80042EDC` | `fn_80042EDC` | `CameraScript_InterpToNewScript` | medium | nbr(r0,2) | 1196 / 1137 |
| `800437F8` | `fn_800437F8` | `CameraScript_IsDefaultSwingCam` | medium | calls(r0,1) | 296 / 601 |
| `8004B89C` | `fn_8004B89C` | `Ter_CheckObjectAndHazardObstruction` | medium | calls(r0,1) | 2200 / 1843 |
| `8004C134` | `fn_8004C134` | `Ter_SearchAreaForDropLocation` | medium | anchor(1)+graph | 1116 / 703 |
| `8004C590` | `fn_8004C590` | `Ter_CheckForDropLocation` | medium | nbr2(r0,2) | 484 / 391 |
| `8004C798` | `fn_8004C798` | `Ter_IsValidDropSurface` | medium | nbr2(r6,2) | 84 / 68 |
| `8004D734` | `fn_8004D734` | `Ter_GetSupportingGroundNormal` | medium | nbr(r5,2) | 216 / 422 |
| `8004D838` | `fn_8004D838` | `Ter_GetSupportingWorldMaterial` | medium | nbr2(r3,2) | 88 / 79 |
| `8004D890` | `fn_8004D890` | `Ter_GetSupportingGroundData` | medium | nbr(r5,2) | 280 / 459 |
| `8004D9E0` | `fn_8004D9E0` | `Ter_GetEnclosingGroundData` | medium | nbr(r5,2) | 464 / 955 |
| `8004E1B0` | `fn_8004E1B0` | `Ter_CheckForPinCollision` | medium | anchor(1)+graph | 936 / 938 |
| `80052268` | `fn_80052268` | `Physics_BallRollingandSlipping` | medium | nbr(r5,2) | 816 / 1128 |
| `80053240` | `fn_80053240` | `Physics_CheckObjectCollisions` | medium | nbr(r2,2) | 852 / 1122 |
| `80054040` | `fn_80054040` | `Physics_CheckActorCollisions` | medium | nbr(r0,2) | 332 / 247 |
| `80055324` | `fn_80055324` | `Physics_GetBallAltitude` | medium | nbr(r4,2) | 388 / 471 |
| `8005567C` | `fn_8005567C` | `Physics_Simulate` | medium | nbr2(r0,2) | 312 / 374 |
| `8005587C` | `fn_8005587C` | `Physics_DropBall` | medium | nbr2(r2,2) | 320 / 394 |
| `80069218` | `fn_80069218` | `PlaceBall_IsValidDropLocation` | medium | nbr2(r5,2) | 280 / 224 |
| `800694A0` | `fn_800694A0` | `PlaceBall_UpdateMomentums` | medium | nbr2(r4,2) | 1508 / 2448 |
| `8007706C` | `fn_8007706C` | `sfsnprintf` | medium | calls(r0,1) | 64 / 61 |
| `80078A2C` | `fn_80078A2C` | `FE_CrAP_RandomizeCategoryWithUndesirableTest` | medium | nbr(r1,2) | 344 / 229 |
| `800B52D4` | `fn_800B52D4` | `SD_vShaderObject_Rain_Dynamic_Refresh` | medium | anchor(2)+graph | 1504 / 1414 |
| `800BE504` | `fn_800BE504` | `GolfCamera_InitGreenZoomToAimCamera` | medium | nbr(r7,2) | 804 / 1052 |
| `800C4724` | `fn_800C4724` | `GolfCamera_ComputeSteepSlopeCamVectors` | medium | nbr(r8,2) | 908 / 1460 |
| `800DE180` | `fn_800DE180` | `GM_ShowPostShotAnimation` | medium | nbr2(r4,2) | 1624 / 1080 |
| `800DF824` | `fn_800DF824` | `GM_SimulateBallMovement` | medium | nbr2(r4,2) | 1012 / 1465 |
| `80104700` | `fn_80104700` | `FE_CrAP_TurnOnPart` | medium | nbr(r0,2) | 260 / 387 |
| `80114540` | `fn_80114540` | `DynChain_DoBoobSimulation` | medium | anchor(1)+graph | 1348 / 2786 |
| `8011E020` | `fn_8011E020` | `GetLocalTime` | medium | calls(r1,1) | 140 / 127 |
