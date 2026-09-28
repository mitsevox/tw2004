# Naming leads for the next batches (from naming lanes nm1-nm8, 2026-09-27)

EA names the lanes found and checked briefly against the code but left for batch size. Leads, not
facts: the next lane reads each function before applying (tools/match/name.py; T2 E2 when TW07's
name fits the body). Addresses are 0x80XXXXXX.

## MC_Gc.c (memory card; TW07 MC_PS3.c / MC.c, confirmed by TibExt's SFIO callbacks)
F0F0 MC_FindFiles, F36C MC_GetFreeSpace, F3A0 MC_GetNumFreeEntries, F3D4 MC_OpenFile, F488
MC_CloseFile, F208 MC_ReadFile, F258 MC_WriteFile, F2D8 MC_SeekFile, F35C MC_FlushFile, F364
MC_SetAttributesOnFile, F514 MC_CreateDirectory, F5E4 MC_DeleteDirectory, E758 MC_DeleteFile, EB44
MC_NumEASaveGames. MC_Unmount (ours, T3) -> TW07 MC_UnmountCard. MC.c: 800A0A7C MC_DeleteSaveGame,
800A0868 MC_RefreshMCReplayInfo.

## Audio (AudTable.c = TW07 HLAudEmitter.c, Emi_*; GameAudio.c holds Gaud_* and Aud_*)
AudTable: 7AF0 Emi_InitModule, 7C24 Emi_InitSession, 7C2C Emi_ExitSession, 8248
Emi_SetTrackVariation, 82CC Emi_SetTrackVarRange, 834C Emi_SetTrackVarRangeTmpl, 8394
Emi_SetTrackStep, 84A4 Emi_SetTrackPitchFactor, 8524 Emi_CheckTemplate, 8584 Emi_TrackCallback;
TW07 inline-only statics: 7E44 FreeAllPerfs, 7EA4 Attenuation3D, 7F2C Doppler3D, 7FA8 Panning3D,
809C Distance3D, 8134 PreprocessControllers. hlaudemitter ADE54: probably vec4flt_Zero3 (check it
writes w). GameAudio, n1 A/B with TW07 signatures, need a full read: 6070 Gaud_CameraShake, 6C98
Gaud_InitSlowMo, 6660 Gaud_ExitGameBreaker, 68C0 Gaud_InitSpecialShot, 6AC8 Gaud_UpdtSpecialShot,
6BA8 Gaud_ExitSpecialShot, 6854 Gaud_ExitCamZoom, 754C Gaud_StartMusic. 47A0 StartBackgroundMusic,
484C StartAmbientStreamer, 4928 UpdateStreaming (n1 had these shifted by one).

## GameRound.c (= TW07 GameModeCore.c/.h, same source order)
E1018 GM_ClearPlayerHoleData, E1074 GM_ClearDataForNewGame, E16F4 GM_GetNextSelectedHole, E1CA8
GM_CurrentlyOnLastHole, E1CE8 GM_ConvertCourseAndHoleToPar5EagleIndex, E22E4 GM_UserHasEagledHole,
E234C GM_GetPar5EagleDate, E23B0 GM_IsShotOverLimit, E23EC GM_CanPlayerTakeMulligan, E2470
GM_ClearMulliganCounters, E27C0 GM_GetElapsedHoleTime, E292C GM_GetHonors, E295C GM_GetSecondHonors,
E299C GM_InitBallsToTee, E2A88 GM_SetupGolfer_IfAllWaiting, E2DB4 GM_CheckForBallInHole, E3AF8
GM_GetNeedToBuildPlayoffHoleList; GameManager.c E0A84 GM_SetNeedToBuildPlayoffHoleList.
Check: GM_GetGolferRelativeCurrentScore / ...CumulativeScore (E184C / E1904) may be swapped.

## GameUI.c (TW07 GameUI.c)
E3B28 GUI_Init, E3E0C GUI_UpdateAllUIData, E3E3C GUI_OpenPauseMenu, E41C8 GUI_FlagPostShotRequest,
E4238 GUI_PostShotUIStart, E3D90 GUI_HideAllToggleUI, E3DDC GUI_UIVisible, E4254
GUI_IsPostShotUIAnimating, E42F4 GUI_PostShotUIFinished, E46B4 GUI_CheckMessageQue, E4C20
GUI_BetweenHolesScorecard, E4D94 GUI_EndOfGameScorecard, E4F88 GUI_Online_OpponentQuit.

## FE_CrAPDB.c (TW07 FE_CrAPDB.c)
103920 FE_CrAP_ResetLastCategoryTables, 103A64 FE_CrAP_CloseModule, 103E88 sTurnOffAnimation,
103F94 FE_CrAP_TurnOffPart, 104094 sTurnOnAnimation, 104804 FE_CrAP_RestoreLastRemovedAsset, 10508C
FE_CrAP_RegisterStreamClients, 1050D0 FE_CrAP_SetupFirstAssetIDs, 105154
FE_CrAP_UnRegisterStreamClients; 105264..1053D0 (8, in order) FE_CrAP_GetPartName, GetPartColor1,
GetPartColor2, GetPartColor3, GetPartSponsor, GetPartRetailPrice, GetPartSalePrice, GetPartLevel;
105C0C FE_CrAP_GetPartLevelFromAssetIndex, 1060F0 FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory,
106244 FE_CrAP_GetFirstEquippedIndexForCategory, 1062C8
FE_CrAP_GetFirstEquippedIndexForCategoryAndSubcategory, 106374 FE_CrAP_IsItemEquipped, 10651C
FE_CrAP_GetUnlockMessageFrom, 106658 FE_CrAP_TryBallSwappingAsset, 106E48
FE_CrAP_GetNumEquippedItemsWithSponsor, 106ED8 FE_CrAP_GetNumItemsWithLockModeAndLockVal; by
behaviour: 107294 FE_CrAP_GetSponsorName, 10742C FE_CrAP_GetCategoryFromAssetID, 107444
FE_CrAP_GetLevelFromAssetID, 10745C FE_CrAP_GetSubcategoryNameFromAssetID, 10749C
FE_CrAP_GetAssetNameFromAssetID. From 107554 on the pairing is noise (EA Sports Bio UIS senders).

## SkinPart.c (no TW07 file; our pattern, T3)
CCB08 SkinPart_ChoosePartVariant, CCC1C SkinPart_ChoosePartOption, CDB70 SkinPart_FindPartByName,
CDCA0 SkinPart_FindSetByName, CDE80 SkinPart_FindSetOptionByName, CCD30/CCD84
SkinPart_GetPartVariant/Option, CD0B8/CD1D8 SkinPart_GetSetVariant/Option, CEE04
SkinPart_CopyChoices, CD404/CD56C SkinPart_AllocChoices/FreeChoices, CEF04 SkinPart_FixupDesc.

## GoRenderCtx_Gc.c
8001416C FB_fGetFrameBufferWidth, 8001415C FB_fGetFrameBufferHeight, 8001414C
VM_fGetViewportHeightOverWidth. Ter_GetMesh* have fn_ copies in UObject.c / GoAnimalActors.c.

## Comments the lanes think wrong or stale (for an audit pass; not edited)
engine.h RenderState nBC..nC8 = left, right, top, bottom (inclusive); camera.h CamLens fA8/fAC look
like near/far Z; GoGolfCam.c:38 and gocamscripts.c:51 "char.c: its parameter is u8*" (now CamLens*);
hlaudemitter.c header (TW07 HLAudEmitterPool.c); GameAudio.c's declarations of Aud_EmiInitOnce /
Aud_EmiCycle say "no C yet"; Gaud_StartComment's port note "stays at 91%"; GameRound.c / GameUI.c
headers say no counterpart (TW07 has GameModeCore.c / GameUI.c); GM_Pick_PlayOffHole and
GM_Currently_SkillZoneMode comments incomplete; SkinPart club-skin comments; fe.h CrAPDB.n4 /
CrAPAsset.n40 are the gender; FE_Manager.c local prototypes of FE_CrAP_UnequipSlot and
FE_CrAP_SetTriggerAnims disagree with the definitions (s16 / u8).

## Round 3 (nm9-nm12) leftovers
Applied: the GameRound/GameUI, FE_CrAPDB, MC_Gc and audio leads above (except where noted below).
- MC.c: 0610 MC_ReplaceReplay, 09EC MC_GetNumReplay, 1758 MC_GetNumOptions, 19F4 MC_Debug, 2064
  MC_OpenONCE, 2630/2668/26A0 MC_IsOptions/Replay/UserDataCorrupt, 270C MC_MemoryRequiredForUser,
  2740 MC_MemoryRequiredForReplay, 27BC MC_ConvertCharToWideChar, maybe 27F4
  MC_LastSavedUserFileIndex. MC_Gc: 8009ED34 probably MC_CheckSaveGame_WithRestore; T3 CARD
  wrappers 8009CDA0 MC_CardRename, DFD8 MC_CardCreate, E130 MC_CardWrite, E280 MC_CardGetStatus,
  E360 MC_CardSetStatus, E918 MC_CardFormat. TibExt.c: 80122834 SFIO_vFlushCallback, 80122868
  SFIO_vSetAttrCallback. Save-exists group 8009F6A0 / 800A2194 / 800A218C: no TW2004 name yet.
- GameAudio (TW07 order): 4084 FirstFrameInit, 4170 HeartBeatLoopCallback, 41A4
  UpdateCommentVolDucking, 42B0 InitCrowdBuildup, 4374 ExitCrowdBuildup, 43DC UpdateCrowdBuildup,
  49A4 StopAmbientStreamer, 4A24 GetAmbientStreamRange, 4C54 Gaud_Monitor, 75F4
  Gaud_GetMusicStatus, 7644 Gaud_InitFlyBy, 7528 Gaud_RewardCommentaryIsPlaying, 6D48
  Gaud_ExitSlowMo, 6148 Gaud_TextFall, 6448 Gaud_Tappa, 644C Gaud_Spina, 6450
  Gaud_PlayTappaFeedback, 5980 Gaud_SwingBallHit. 62A4..640C: seven target-game hit sounds (TW07
  BullsEye/BonusBall/LetterGained set, order differs: unnamed).
- FE_CrAPDB: 801042D0 (sApplySlider?), 801048B0 (IsFadeOutCategory?), 80105644, 80105EFC left.
- GameRound/GameUI: 800E4F88 (not GUI_Online_OpponentQuit), 800E2520 (1/2/4 per mode), 800E3B04
  (message 31), the mode callback stubs E3AA0-E3AF4 left.
More comments to re-check: GameAudio.c fn_8006BEA4 prototype says GoGolfCam.c (it is emotion.c);
event.c calls fn_800A6448 / fn_800A644C (u8 nPlayer) but they are void(void); MC.c above
fn_800A2630 ("Three callbacks of a table in .data") is MC.c's own lbl_8018C7D8 IsDataCorrupt
column; E1CE8 / E22E4 / E234C and save.h a5004 could say par-5 eagle records.

## Round 4 (rd1-rd4, core area) leftovers
Header pass for the core area (include/ was off limits to file lanes):
- engine.h RenderState: struct comment "GoTerrain.c's setters write one group each" is stale; fields
  can be named: n0 depth compare, b4 depth writes, n8/bC/bD alpha compare/ref/enable, n10/n14 blend
  source/destination, n18 blend mode, u110 changed groups. DS_vSetZBufferMode / DS_vEnableZBufferUpdate
  / DS_vSetAlphaTestMode prototypes still have a, b, c; RenderState_SetConstantAlphaActive has v.
- engine.h:352: lbl_80280DC8 is defined in LLTex.c, not streammanagerhole.c. engine.h 352/537/1452
  now past 110 columns. engine.h:22-27 alignment; line 22 "the StaticMem_Alloc mode TibExtMemAlloc
  uses". engine.h:1450 LLMath_Normalize3 "three floats, w kept"; :176 "three-float squared distance".
- engine.h:162 Quat_EulerAngles and :145 mat44flt_ExtractEulerAngles params -> yaw/pitch/roll
  (yaw about z, pitch about y, roll about x). cull.h:72 LLMath_mat44fltMultiply params mtx/src/dst.
- camera.h ViewController: b274 -> bActive (TW07 IsActive / TurnOnViewController); prototypes of
  ViewController_TurnOnViewController and VM_vSetViewportRect: parameter names as the definitions.
  RenderCamera is TW07's RC_SRenderCtx (pRect = the viewport). camera.h:29-30 fB4/fB8: a view width
  and height (Mtx_OrthoScale).
Code:
- gomainloop.c:64 declares ViewController_SetCurrentViewController(void) and calls it with no
  argument inside fn_8006C8EC(int nView) (TW07 GO_vSetGenericViewStates(iViewID)): it relies on nView
  still being in r3. The prototype must match the definition (int nView) if the match allows.
- GoRenderCtx_Gc.c:17 "matrix builders, not decompiled yet, f1..f5": stale.
- mat44flt_ExtractEulerAngles: -atan2f(m[0][0], m[0][0]) in the locked-pitch case: probable EA bug.
- TW07 LLMath_PS3.h names for Vec_Copy, Vec_Scale, Vec_Add, Vec_Sub, Vec3_Add, Vec3_Sub, Vec3_Dot,
  fn_8000AE6C (AddScale), Mtx_Copy, Mtx_Identity, vec4flt_CrossProduct: LLMath_Scale, Add, Subtract3,
  DotProduct3, AddScale, CopyMat44, IdentifyMat, CrossProduct3W0 (check each).
- Camera_GetCurrentLens (8001F004) = RC_spGetCurrentRenderCtxCamera; fn_80013E40 =
  RC_spGetRenderCtxFrameBuffer; RenderState_SetBlendFactors / SetDrawFlags may be
  DS_vSetAlphaBlendingFunction / PR_vSetCurrentUsage.
- hotnames comment_state misses a comment above a two-line asm signature with an #else C copy
  (LLMath_mat44fltMultiplyList / List33 are commented).

## Round 5 (re1-re4) leftovers
Core (for the next core-area touch; the core header pass itself is done, re2):
- GoRenderCtx_Gc.c 0x800142A4-0x800143B8 is TW07 LLInput.c in order: 800142A4 Input_vSelectControlSet,
  Controller_GetButtonMask = Input_uiMap, Controller_AnyPadHasButtons = Input_AnyPadPressed,
  8001437C Input_vStopAllVibration. GoRenderCtx_Gc.c:17 local Mtx_OrthoScale prototype is stale.
- camera.h View: f50-f58 the view scale, f5C-f64 the view offset (GetCameraViewScale / Offset); v0 is
  the camera origin (TW07 GetCameraOrigin). Misaligned trailing comments camera.h ~626-628, engine.h
  ~1345-1358; ~40 camera.h lines past 110 columns.
- engine.h: PadAnalog is TW07 Controller_StickInfo; the Input_sGetStickInfo prototype comment ("stick
  bytes at +0, +2, +3") to check. Code8002DB80.c:51 local prototype of Input_vSetVibrationStatus
  (int, u8) vs definition (int, int). LLVideo.c scissor setter params nWidth/nHeight are right/bottom.
  GoCamera.c fn_80076948 params fB4/fB8 -> fFlatWidth/fFlatHeight. RenderState nF4/nF8 unnamed.
Golfer area header pass (char.c / SkinPart.c lanes, include/ not edited):
- character.h 0x16E4 aPoints: 0 right toe, 1 left toe, 2 right ankle, 3 left ankle (bones
  0x3A/0x48/0x39/0x47, swapped for a lefty), 4 the club point. 0x179C a179C: the average ground normal
  under the four foot points. 0x16A8 n16A8: bone 0x15, the right wrist; 0x16A4 nGripBone: bone 0x52
  "IGdriver". 0x4AC events: EA's SKA tags. character.h:1020 two prototypes on one line;
  Character_AddTextureLoadRequest params pfnBegin/pfnEnd. lldyntex.h DynTexJob pfnA/pfnB = begin/end.
- charstate.h: SkinIterArgs "(or, from SkinPart_GetOptionSize, a SkinVariant.nC index)" misleading;
  SkinDesc7C.n10 = the entry of a SkinDesc14 pB8 run (texture scale/offset) the variant picks;
  Skin.p10CC one bit per SkinModel.p54 blended matrix Skin.c computes, p10D0 one bit per SkinModel.p44
  entry Skin.c draws; u10D4 bit 1 (value 0x1) cleared by SkinPart_UpdateMarks; Skin.aParts copy 3
  newest, copy 0 drawn; Skin.a1048 the toe/ankle points. game/save.h 182-183 past 100 columns.
- mtalib.c bone table names: 0x36 rhip, 0x38 rknee, 0x39 rankl, 0x3A rtoe, 0x44-0x48 left leg, 0x52
  IGdriver, 0x53 clubhead, 0x15 rwrst.
- FEgolferanim.c: 8008E918 likely TW07 FE_SetTextureSwapState, 8008E944 FE_SetDelayTextureSwap.
- char.c file header is stale ("Not all of it matches yet"): for the lane that finishes char.c.
Continue: char.c from 0x8001A288 (120 to go); SkinPart.c from 0x800CDAFC SkinPart_FindPart (42 to go).

## Round 6 (rf1-rf4, golfer area) leftovers
- char.c: continue from 0x8001C5B4 Character_SelectClub (80 to go). fn_8001EE00 / fn_8001ED44 are TW07's
  inline Character_ComputeMaxVisableShadowDistance / Character_ComputeMaxVisableDistance. fn_8001A484 is
  empty (unnamed on purpose). char.c:184 local prototype SkinPart_CopyChoices(.., int a, int b) -> nFrom/nTo.
  The char.c file header ("Not all of it matches yet") is for the lane that finishes char.c.
- character.h, still offset names: n1654/n1658 (Character_ClipTest result for the body / 3-unit shadow
  sphere, 2 = out of view), n1698 (1 once posed, Skin.c SKN_PoseCharacter), f165C/f1660 (max draw
  distance shadow / body), v1668/f1674 (bounding sphere centre/radius), n1784 (which half of the foot
  points to update), u10 bits (0x1 hidden, 0x2 flagstick, 0x40 dyn textures given back, 0x1000 cleared
  each frame by Character_PreRenderAll, 0x4000 club hangs from the root), p17AC typed void* but is a
  CharSliderDefs*, Character.p44 = per-course lighting (SKN_GetLightCourse, read as LightParams); the
  rest listed in rf4's report (nC, f14, n18.., p178C-p1798). CharModel fC/f10 = left/right foot length.
- charstate.h: SkinListEntry p8/nC -> pRecolor/nRecolorMode; SkinDesc8C a08 = f32[3][3] colour matrix,
  n2C its mode; SkinTarget is really a DynTex (SkinPart_SetupMaterials could take DynTex*); CharPool
  a[6] used past the 2 live entries; CharPoolEntry.p is a DynTex*; lbl_80280E20 = most clips a blend
  tree holds (animblender.c fn_800724C0). Skin p1088/p108C/p1090/a1098/a10A0/f10D8/f10DC unnamed.
- Unused-argument hooks: SkinPart_DrawPart (callers pass the view), SkinPart_InitSkin / ShutdownSkin /
  UpdateSkin / InitTextures are (void) but called with arguments (casts + port: notes in Skin.c/char.c).
- hwsRender_Gc.c declares `s32 SKN_CloseModule();` vs `void SKN_CloseModule(void)`. Code80016198.c
  fn_80016978's comment says corners, RC_ApplyViewport passes left, top, width, height.
- Tool: name.py drops the `port:` continuation indent ("//       ") when it rewraps a comment.
