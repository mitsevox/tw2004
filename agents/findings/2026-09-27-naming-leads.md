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

## Round 7 (rg1-rg4, golfer area) leftovers
Golfer header pass 2 (character.h / charstate.h / camera.h / fe.h; not edited this round):
- Character: n1654 nClipResult, n1658 nShadowClipResult (getters Character_GetClipResult /
  GetShadowClipResult), f1660 fMaxVisibleDist, f165C fMaxShadowDist; nAnim = the target body state
  (EA's CharacterAnimStateE), n20 the current one, n18 bit 0 a state change waiting, n24 / n25 the
  ambient / idle fidget counts, n26 a fidget playing, n2C / n30 the morph player's target / current
  state, anim29C + node3E0 the morph player and its tree. AnimPlayer.f14 fTimeScale (TW07
  SKATime_SetTimeScale; its prototype could take AnimPlayer*), AnimPlayer.nC / f10 the queued
  transition state and time. Clip.pF4 the clip's own MtaLib.
- Skeleton: f1074 / f1078 the IK weight blend's time left (negative blending in) and length, v10A4 the
  hip offset (y only), v10B4 it times the weight, p20 / p24 / a10 the IK rotations / weighted / bones
  turned, q10D4 the extra right-shoulder rotation, n10E4 its frames. IKLink.f4 the share of each turn,
  n8 a locked axis (-1 none); IKChain.n18 / f1C most iterations / tolerance. CharModel: a140 per-bone
  scale, p760 / p764 / p768 bone poses / world-to-bone / skinning matrices, bEE left-handed.
- Prototypes: CharacterState_AddSKABlendData params nTransitionState / fTransitionTime;
  fe.h FE_setupStreaming (nOtherA, nOtherB); char.c declares SKEL_InitIKSkeleton void, it returns f32.
  camera.h CrAPGolfer.b18 = the golfer is loaded/ready (not "the camera script runs"), b19 = to be freed.
  Extern comments out of column in charstate.h / character.h after the global renames; three
  character.h lines past 100 columns (uId comment, RequestClothesUpdateFE, SkeletalObject_FindObject).
Other:
- FEgolferanim.c: continue from after 0x8008B790 (69 to go); 8008B990 is probably TW07
  FE_StreamGetCurrentState; 8008E918 / 8008E944 FE_SetTextureSwapState / FE_SetDelayTextureSwap.
  Pairing rows wrong: 800962F8 / 80096338 / 8009637C shifted by one; 800957FC not SetSKAState;
  8008B61C / B674 / B694 / B6E4 (FEAnimManager_*) are state 4's handlers.
- fn_80021980 is TW07 BitArray_MergeArrayWithOr. Char_Vec3Add/Sub, Vec3_Scale, Char_Vec4Add/Sub,
  Vec4_Dot are copies of TW07 UVecFlt.h inlines (vec4flt_Add3 ...): a project-wide naming call.
  Character_IsGolfer may be TW07 Character_IsHuman (unsure). ByteSwap_Records / MtaLib_SwapAndLink at
  the end of char.c may belong to mtalib.c (misfiled units list).
- Skeleton.c Math_Sqrtf is MSL's inline sqrtf emitted out of line; Skeleton_StrippedFn's fake match
  likely stands for MSL's _half/_three: for a matching lane.
- Tools: name.py does not find definitions returning a pointer to an array (`f32 (*Fn(...))[4]`);
  wraplong does not wrap long initializer rows.

## Round 8 (rh1-rh4, golfer area) leftovers
Golfer header pass (with Round 6/7's lists):
- camera.h CrAPGolfer b18 = loaded/ready, b19 = to be freed. CrAPState: n4 camera idle state (0 "Crap
  Idle", 1 "Crap Face"), n8 render state (0 golfer, 1 clubs, 2 ball) / nC + b80 its temporary one,
  sz20 / sz30 queued animation / camera shot, sz54 queued ball texture, n50 repeats left, n74 texture
  swap (1 loading, 2 loaded, 0 done), b78 / f7C delay swap and time, b81 new-textures flag (unread),
  b86 hidden, b8A cache clear pending, n1B4 idle-anim count, n1B8 / n1BC club / temp club (-1),
  n1C0 queued-anim state (0 wait end, 1 fade out, 2 playing, 3 fade in, 4 none), b1C8 / n1C4 pad lock,
  f1CC fade-out timer, n1D0 fade at end, b1D1 club dressing allowed, b1D2 swap due, n1D4 / n1D8 last
  asset / category, b1DC zoom (TW07 `zoom`), f14C display alpha 0..0.5, f19C / f1A0 facing now /
  target (radians), f140..f148 written only.
- fe.h prototypes: FE_vTriggerCrAPAnimAndCamera bBlend (not bNoBlend), FE_QueueCrAPAnim bFade /
  bWaitForEnd, FE_SetDelayTextureSwap bDelay / fTime, others as rh2 dd92e44; FE_PauseFECharStreaming
  comment ("the golfer loader"). FE_MessageTable.c: FE_SetOffscreenBufferRender(s32) vs u8,
  FE_SetProfileLeftHanded(int, int) vs (int nProfile, s8). char.c: FE_SetTextureSwapState(s32).
  rh1's FE_vLoadNextCrAPAnim param bNoBlend means bBlend. save.h n113: the created golfer is left-handed.
- character.h: AnimPlayer = EA's TSKATime; uFlags: SKATime_Pause sets 0x2, UnPause clears 0x3 (same fix
  in GoGolfCam.c:55's prototype comment); n08 play count. SKABlendNode.bC = bFreeASAP. Prototypes
  SKABlendData_Init (bFreeASAP), SKABlender_AddBlenderData (pInfo, bFreeASAP), SKABlender_ClearMorph
  (nMorph). charstate.h: SkinMorphWork p10020 / p10024 current / next target, SkinMeshBit.unk0 = x, y, z
  s16, SkinDesc44.u24 "bit 1" means value 2, SkinMorph_SetTargetWeight param (TW07 iTarget).
- Misfiled units: SkinBurn_CheckSignatureFile (startup signature check) likely not SkinBurn's;
  char.c's ByteSwap_Records / MtaLib_SwapAndLink sit right before mtalib.c.
- Pairing TSV: 8008EAB0..8008EB04 shifted by two. Unnamed on purpose: FE lbl_80189A40/60/80,
  lbl_80281338, lbl_80281348; mtalib lbl_801B9668.
- Tools: tools/agents/merge_lane.sh (3-way merge of a lane's hand edits) replaces taking whole files.

## Round 9 (ri1-ri4, golfer area) leftovers
Golfer header pass (add to Rounds 6-8):
- character.h: Character.v1638 is 6 floats (SKA_SampleBlendClip writes 0x1638-0x164F; f1644 = v1638[3],
  unk1648 = [4]/[5]). Clip: "first/second frame stream" = 16-bit / 8-bit angle streams, pE0 the 8-bit
  stream's base angles (u16 x3 per bone), pE8 fixed rotations (16-bit, n60 bones), uFlags 0x2 has a
  BlendClip. CharBuffer comment "the code at 0x8001FE50" = SKA_Update. ClipRecord.n12 1 = flagged to
  drop (not "keep"), 4 linked, 8 copied; n14 byte size; n18 unknown (drop order). LibOverlay.n10 the sac
  stream id (model id + 3, -1 once merged), n14 model id. LibSlot.n150 clips copied; AnimLib.n140 clip
  bytes, n12C built size; ClipBank.uId a byte counter in a planned bank; AnimLeaf.uMask 2 = unused by
  the merged library. AnimStreamClips.b8 bRead, AnimStreamPlayer.nId nSlot, AnimStream.p0 pReadBuf,
  p4 pReadClips, n1CC8 nReadPlayer. Extern comments misaligned after the skalib global renames.
- engine.h SKA_UnpackName comment (SKA_PackName's output needs SKA_UnpackSwappedName).
- charstate.h SkinDesc.n2C = meshes in p34 (not "bits an HwsBurn keeps"); n30 = one past the last mesh
  flagged 0x100000 after a burn; SkinDesc44.n8 vs HwsBurn_MarkEntry to check. game.h Session.uFlags
  0x4000 also forces clear weather (fn_8006F650) and turns caddie tips off.
- Prototypes: skalib.c local SKA_SwapClip(void*) vs (u8*), SKA_PatchMemory void vs Clip*;
  Code8006F438.c AnimStream_WaitForRead s32 vs void.
Other:
- Possible EA bugs, unlabelled: AnimStream_AssignSlots (no break), AnimStream_StartRead (hang on a
  failed start; streaming is off in this build). HwsBurn_CopySetOptions round-up may be an unlabelled
  fake match. SwingTips.c is EA's CaddieTips.c (file rename = misfiled-units item).
- Unnamed data: lbl_80281D18 (skalib, written only). Game type 6 still unexplained.
- Golfer area left: Swing (65), CharSliders (12), then the golfer HEADER pass (Rounds 6-9 lists).

## Round 10 (rj1-rj6) leftovers
Golfer header pass 3 (rj3 stopped at its cap; its "left" list): character.h Clip n04 / n0A / n36 / n38 /
pE0 / pE8 / pF4 (own MtaLib) / uFlags 0x2 / stream wording; CharBuffer comment (= SKA_Update); skalib
structs (ClipRecord n12 1 = drop, n14 nBytes; LibOverlay n10 / n14; LibSlot n150; AnimLib n140 / n12C;
ClipBank uId; AnimLeaf uMask); AnimStream fields (Round 9); Character nC, p178C-p1798, u10 bits 0x100 /
0x2000 unset, Skeleton n112C / n1130; charstate.h (Round 6/8/9 lists); camera.h CrAPGolfer / CrAPState
(Round 8); fe.h prototypes; engine.h SKA_UnpackName; game.h Session.uFlags 0x4000; save.h n113;
prototype mismatches (SKEL_InitIKSkeleton, SKA_SwapClip, SKA_PatchMemory, AnimStream_WaitForRead);
extern comments out of column; old lines past 100 columns in character.h. AnimPlayer -> TSKATime type
rename (project-wide call).
Golfer (from Swing lanes): golfer.h nStickUsed is backwards (nonzero = main stick, bytes 2/3), nRestCX/CY
go with the main stick; Player.uFlags 0x8 = tap-in (not score display); fForwardSpin / fSideSpin also from
the spin stick; Character f162C/f1630/f1634 written only. Prototypes: Swing.c fn_800AE3C4(int) vs (void);
Code8006F438.c SW_vDeInitForHole s32 vs void; event.c / uiobject.c SW_vSetDisplayBoostUI(a) and
SW_vGetCurrentSpin(pSpinY/pSpinX) -> pfSide / pfForward. docs/gameplay.md: gForgivenessTable, the
quarter-pull and stick claims (gameplay.md rewrite item). Pairing TSV: 8010E35C not SetAnimModifiers.
Round flow header lane (later): game.h GameEffects b10 = DoubleTime (not half speed), b11 = HalfTime;
golfer.h Player bC2D ShotLimitExceeded, bC2E UsedMulligan, bC2F UsedMulliganThisHole, nLevel = penalty
shots in a row (CPU +25 attribute points each), bLowIQPenalty = last shot cost a stroke; game-state hooks
pfn1EC / pfn1F0 pre / post data stream, pfn234 CheckControllerPulled, pfn1F8 IsPuttForLead; Session a8[0]
attract-demo flag, b12 ends the round; camera.h script.nCamera 1 / 2 / 4 colour fade out / in / held.
game.h CareerCalendar.nDriver 0 = online driver (stub), n1C popup type (nPopupType), the "no career"
block comment; gCalendarFillCell extern and CalendarScreen.c pLook / pButton -> pCellColor / pCellState.
rte.h RTEvent.n14 icon index; GM_RealtimeMode_TodaysEventCompleted comment; save.h a104D0 event indexes.
Leads: fn_800EFC80 = TW07 GM_PgaTourMode_GetEventInfoByDate; EventInfo.c fn_8011D280 / 4DC / 658 / 858 /
878 / A44 / C30 = TW07 FE_CalendarPopups.c PGATourPopup_GetRow_* / RealtimePopup_GetRow_*;
FE_PGATourMessages.c fn_8010EEA8 = PGADriver_ShowCalendar_AdvanceSeason. GameUI.c:349 comment stale.
Misfiled: GameModeDriver.c is EA's FE_Calendar.c; GameManager.c is EA's GameMode.c and its first 8
functions serve GameEffects.c; GM_Vec4Sub / GM_Vec3Sub at GameRound.c's start. RTE = "real-time events"
(TW07 GameModeDriver_RealTimeEvents.cpp; my "Road to the Emerald" gloss in the prompt was wrong).
Unlabelled possible EA bugs: GameModeDriverRTE_EndGame calls mode 5's end game unchecked;
GM_ShowPostShotAnimation reads pSurf->nClass after a NULL test. GameManager hook calls through raw
offsets pass gpGame to (void) hooks: check EA form vs fake match. GetRankText EA bug labelled.

## Round 11 (rk1-rk6) leftovers
Golfer header items still open (rk1's "left" list, each verified by rk1 against FEgolferanim.c / the
skin code, ready to apply): camera.h CrAPState n4 nCamIdleState, n8 nRenderState, nC / b80
nTempRenderState / bTempRenderState, sz10 szCurAnim, sz20 / sz30 szQueuedAnim / szQueuedShot, n50
nAnimRepeats, sz54 szQueuedBall, n74 nTexSwapState, b78 / f7C bDelayTexSwap / fTexSwapDelay, b81
bNewTextures, b86 bHidden, b8A bClearCache, f14C fAlpha, f19C / f1A0 fFacing / fTargetFacing, n1B4
nIdleCount, n1B8 / n1BC nClub / nTempClub, n1C0 nQueuedState, n1C4 / b1C8 nUnlockState / bPadLocked,
f1CC fFadeOutTime, n1D0 bFadeAtEnd, b1D1 bClubStatesAllowed, b1D2 bTexSwapDue, n1D4 / n1D8 nLastAsset /
nLastCategory, b1DC bZoom. charstate.h SkinListEntry p8 / nC pRecolor / nRecolorMode; SkinDesc8C a08
f32[3][3] colour matrix, n2C its mode; SkinDesc n2C mesh count of p34, n30 one past the last 0x100000
mesh; SkinDesc44.u24 "bit 1" = 0x2; SkinMeshBit.unk0 x/y/z s16; SkinMorphWork p10020 / p10024 current /
next target, n10018 vertex count; Skin p1088 world-to-bone, p108C skinning matrices, a1098 / a10A0
per-view memory blocks / override tables, p1090 freed only, f10D8 / f10DC written only. CharPool a[6].p
holds a Character* (layout question for the owner). fn_800AE3C4 (void) vs callers passing nPlayer;
hwsRender_Gc.c SKN_CloseModule s32 vs void; event.c locals fSpinY / fSpinX = side / forward;
FE_SetProfileLeftHanded(int nProfile, s8) breaks the match (sign-extension): leave (int, int).
Round flow header lane: pgatour.h Tournament.aPrize [0] purse, [1] first prize (comment backwards;
PGATourSimulation.c:295's locals swapped too), nC isAMajor, nTourEvent TW06 tournament, n10 icon /
texture id, TourEvent.a40 low score per bracket, PgaTriple = sponsorship offer (n0 progress, n4
nStartCash, n8 nBonusCash), Pga80205F30 = PgaTour_WinInfo (b0 placed, n4 position, n8 winnings),
AwardMoney prototype (nPlayer, nCash); save.h n4E98 rounds at or under par in a row, n4E94 tournaments
started, a1054C sponsorship slots, line 38 comment repeats itself, UserInfo stats (rk5's list: n88
drives ... nAC-nC4 hole results); golfer.h GameState bD4 in a playoff, nD8 playoff hole count, hooks
pfn1E4 PostHoleLoadInit / pfn1EC StartGamePreData / pfn208 GetPotentialHoleResult, Player n2DC drive
distance, n2E0 longest putt, b2E4 fairway hit, b2F6 green in regulation, b310 bunker this hole;
options.nC is the weather choice (fn_8006F650: 0/4 bit 0, 1 random, 2 changing, 3 bit 1, 5/6 none);
earnings.h record-kind comments and misaligned externs, HoleGoal / uLies lines past 110; game.h
record-check params (setrecord, firstPlaceOnly / bPreview), GM_Earnings_TournamentPayout (purse,
firstPrize, place), game.h:254 Earnings_CheckShotAwards comment; challenge.h n0 / n4 group name /
description, b4D sets options.nC 3, externs misaligned; game.h PlayNow_ prototypes' old param names;
GameManager.c's local GM_Earnings_PayRoundGoals(int, int) vs (int, u8 bRoundOver); fe.h FE_CrAP n4
~130 columns.
Other: mode 5 = Play Now challenges (docs/journal.md's "Tiger Challenge" guess wrong; src/README mode
table); GameMode5.c file rename to EA's PlayNowMode.c = misfiled-units item. Pairing TSV A rows 800D8D5C /
800D8DB4 one function off. Possible EA bugs, unlabelled: GetCourses aTourEvent[-1]; SetTournament never
restores options.n18; PlayNow_LoadBallSpot index -1; PlayNow_GameFinished adds totals when only asked;
Earnings goal checks index gpSaveData by player number; possible double hole-goal payout on the last
hole. docs/decomp-notes.md "Earnings fn_800D6A70" stale. Tools: wraplong splits casts `(\n void* (*)(u16))`
and does not wrap trailing field comments; name.py writes column-8 comments after renaming, so a comment
must already use the batch's new names.
