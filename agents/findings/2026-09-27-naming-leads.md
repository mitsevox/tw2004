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

## Round 12 (rl1-rl6) leftovers
Owner decisions (2026-09-28): rename misfiled files to EA's names (GameModeDriver -> FE_Calendar,
GameManager -> GameMode, SwingTips -> CaddieTips, GameMode5 -> PlayNowMode, GameTargets ->
GameMode_SkillZoneBase, GameMode13 -> GameMode_SkillZoneTimed, GameMessages -> gameui_istudio if
proven); AnimPlayer -> TSKATime: yes; CharPool a[6]: a lane proves the Character* layout and fixes it
if main.dol stays OK; SkinDesc8C.aRecolor stays u8 with its comment (done by rl1).
Header lane done (rl1): CrAPState all fields, charstate.h skin fields, pgatour.h (bIsAMajor,
nTextureID, aPrize comment, aFieldLowScore, PgaSponsorship, PgaTour_WinInfo). Still open from the
Round 10/11 round-flow lists: save.h n4E98 / n4E94 / a1054C / line 38 / UserInfo stats / a104D0 / n113;
golfer.h GameState bD4 (playoff: GameModeDriverPGATour.c:708 sets it) / nD8, hooks pfn1E4 / 1EC / 1F0 /
1F8 / 208 / 234, Player n2DC / n2E0 / b2E4 / b2F6 / b310 / bC2D-bC2F / nLevel / bLowIQPenalty (mode 22
scores it -100), uFlags 0x8 tap-in, nStickUsed backwards, options.nC weather, Session a8[0] / b12;
game.h GameEffects b10 DoubleTime / b11 HalfTime (rl6 confirms; b9 + fC = timed double speed, TW07
SpeedyTimeStart), Session.uFlags 0x4000, CareerCalendar, gCalendarFillCell, record-check params,
TournamentPayout params, PlayNow_ params, GM_Earnings_PayRoundGoals (int, u8); earnings.h; challenge.h;
rte.h RTEvent.n14; camera.h script.nCamera; fe.h FE_CrAP n4; event.c fSpinY / fSpinX locals;
GM_PgaTourSim_DistributeWinnings third param = first prize; docs/format-byteorder.md aTriple.
New header items (round 12 lanes):
- golfer.h: pfnSetupNextGolfer (0x1D0) = the next turn, not "the hole starts" (GM_SetupGolfer_IfAllWaiting);
  pfn1EC before the round, pfn23C ball landed (events 35..38), pfn240 ball touched a surface (PsBallFx,
  returns a hit-effect id), pfn244 a shot is over in bounds (GM_Earnings_PayShotGoals; pfn250 = OOB);
  in the target modes pfn258 / pfn264 = TW07 PickTarget / PickPrevTarget, pfn26C = GreenType (GoDynObj);
  b285 = the mode allows GameBreakers; p130 current pin position; fC50 drive length from vA44; nC3C
  speed golf flag bits; nC44 mode 7 points; nC58 HUD slot (2/3 in speed golf); nC6C per-hole score
  (mode 8 seconds + 3/stroke, mode 7 points); nC40 written only; fA80 / fA84 placement cursor push;
  nCD0 / aCD4 surfaces scored this shot (cleared per shot, not per game); nD28 points per hole; nD70
  scoring landings per hole; nDB8 shot points; nDD8 also target-game winnings; nDDC longest drive;
  nDE0 bullseyes; nDE4 per-target hit count; nE8C / nE90 best / current target streak; nEA0..nEDC mode
  22 (drives taken, fair drives, longest fair, score, average, total length, counts per kind);
  Session.nPaused 2 = until every pulled controller is back.
- Options b84 = putting grid (lessons 7..9 switch it on); options a0[4] = commentary level 0..5.
- game.h lbl_80202B88 = per-controller "pulled out" marks; target-game prototypes' trailing comments
  stale / misaligned (~649-686) and params n, i, f, a; GameMessages prototypes a, b, c;
  GameMode22_IsActive u8 in GameUI.c vs s32 definition.
- mode22.h: n0 variant, n4 drives per player, n8 winner (5 none), bC decided, f10 longest drive, n14 its
  player, n18 winner countdown. fe.h FEScreen.b0 = fade-to-black flag (not "set by fn_80079AD4").
- GameEffects u48 / u4C = negative / positive post-GameBreaker commentary (weak); n4F crowd reaction
  level (SitDevTrigger.c fn_800BD7D0 says "music": wrong). gocamscripts.c fn_80045494 = SetHalfTime,
  fn_80045558 = SetDoubleTime (TW07 RenderPredictedGB).
Stale comments in finished files: GameRound.c GUI_SendMessage31 ("last pulled controller is back");
GameUI.c GUI_PauseMenuClosed ("unless the fade to black is running"); target.c local prototypes of
GUI_MoveTargetInfo / GUI_UpdateTargetInfo (a, b, c, d) and its fTilt / fStep = rise in feet / inches;
docs/decomp-notes.md calls the target functions "GameMode10".
Names for other files: GameUICommands fn_800872F8 = GM_vGetSpeedGolfHoleScore, fn_8008828C =
GM_vGetSpeedGolfWinner, fn_80088208 = GM_vTimerOut, fn_800882C0 = GM_vFullRound (TW07
APT_IG_GameMessages.c order). GameModeReplay.c's last three (fn_800F1960 / 196C / 199C) = TW07
SkillZoneBase GetCupCount / GetCupPosition / AddCup: GameTargets.c probably starts at 0x800F1960
(misfiled-units item); pairing B row "fn_800F1960 = GameModeReplay_GetName" is wrong.
Possible EA bugs, unlabelled: Lessons_GetShape / GetClub / GetShotKind index gLessons[gLessonNum - 1]
(one past at lesson 12, -1 at 0; call timing unchecked); Lessons_StartGamePreData overrides the
chosen golfer; SkillZoneBase_ScaleTargetPoints reads past its 13 / 15-entry tables with more targets;
SetupBonusBall case 4 never set; mode 8's pace always falls at the slow rate (nCB8 counts only in mode
7); speed golf state 24 never set; GameEffects TargetGameBreakerTrigger tests mode 16 twice;
GameMsg_SendPending bit 1 numbers players from 0 (GUI_StartPostShotUI from 1); gGameMsgPendingValue
shared by three messages. Tools: the refs-only refresh can leave comment lines past 110 that one
wraplong pass cascades; run wraplong until lint is clean (12 passes used this round).

## Round 13 (rm1-rm6) leftovers
EA identities found (misfiled-units renames, owner-approved batch): GameMode14.c = TW07
GameMode_SkillZoneCapture.cpp, GameMode15.c = SkillZoneHorse, GameMode16.c = SkillZoneTarget,
GameMode17.c = SkillZoneTargetToTarget, GameMode9.c = GameMode_Practice.cpp (TW06 GM_Practice_mode = 9),
GameModeBestBall.c = TW06 gamemode_bestball.cpp / TW07 GameMode_BestBall.cpp, EventInfo.c = TW07
FE_CalendarPopups.c (its three FE messages 536 / 544 / 696 are not in TW07's file). event.c's tail
0x80067608..0x80067710 (SitDev_vInitModule .. vUnregisterStreamClients, gSitDevData / gpSitDevData) +
Code80067710.c = TW07 SitDev.c: split event.c there. GameModePractice_ReadPlaceBallSticks is in
GameMode9's range but serves every mode's ball placing. GameModeReplay.c's last three = SkillZoneBase
(Round 12). Event numbers = TW06 EVENTID_e up to 59, one lower from 60 on.
Header lane done (rm1): CharPool (a[6] texture entries + pLoadingChar), AnimPlayer -> TSKATime, GameState
playoff fields / pPinPos / bAllowGameBreakers / 19 callbacks (TW07 GameModeBase), Player round stats,
GameEffects, mode22.h, options nWeather / bPuttingGrid, FEScreen.bFadeToBlack, Session bDemo / bEndLoop;
orchestrator renamed g*SavedOptionsC -> g*SavedWeather. Still open: save.h (n4E98 / n4E94 / a1054C /
line 38 / UserInfo stats / a104D0 / n113); golfer.h Player fC50, nC3C, nC44, nC58, nC6C, nC40, fA80 /
fA84, nCD0 / aCD4, nD28, nD70, nDB8, nDC0 (target games' balls), aDC4 ([0] multiplier shots, [1] extra
balls, [3] hits, [4] time frames), nDD8 (winnings in the target modes), nDDC, nDE0, nDE4, nE88 (HORSE
letters), nE8C / nE90, nE94 (mode 14 steals), bE9E, nEA0..nEDC, nShotKind comment wrong (SHOT_TYPE_: 0
putt, 1 drive, 2 chip, 3 pitch, 4 punch, 5 flop), b30C hit a course object (event 36), b30D hit the
flagstick (event 38); hooks pfn20C, pfn214..pfn230 (TW07 order), pfn228 is called from
STATEFUNC_SwingUpdate (not "shot setup"); game.h Session.uFlags 0x4000, CareerCalendar, gCalendarFillCell,
record-check / TournamentPayout / PlayNow_ params, GM_Earnings_PayRoundGoals (int, u8), target-game /
GameMessages / SkillZone getter prototypes' params (a, i, n) and misaligned trailing comments,
GameMode22_IsActive u8 vs s32; engine.h EVENT_Trigger 4th param = nArg (1 real ball, 0 simulated, -1
none), GameMode26_StartComment params / comment; ladder.h LadderEvent.n40 tour stop, "six region
finals"; mode26.h stale fn_ wording; earnings.h; challenge.h; rte.h RTEvent.n14; camera.h
script.nCamera; fe.h FE_CrAP n4; AnimPlayerEntry -> TSKATimeEvent (TW06, unproven);
GM_PgaTourSim_DistributeWinnings third param; GameManager.c raw 0x1EC / 0x1F0 hook calls.
Prototypes in other files: FE_MessageTable.c:48 / GoEntry.c:35 `u8* GameMode26_StartEvent(void)` (void
definition) and FE_MessageTable.c:66 SetTargetScore, both marked "// CharSliders.c"; GameUICommands.c
place / target parameter names; gocamscripts.c fn_80045494 / fn_80045558 = SetHalfTime / SetDoubleTime;
SitDevTrigger.c fn_800BD7D0 "music" = crowd reaction. Stale comment words in finished files:
GameMode6.c:2 (pfn1E4), GameMode16.c:3 (pfn268), GameAnalysis.c:121-170 (b2E4 / b2F6 / n2DC / n2E0).
Pairing TSV: event A/B rows shifted by one (80066214, 80066D54, 80066FA8, 8006702C).
Possible EA bugs, unlabelled: EVENT_PracticeSwing passes 21 to the lessons (not 9); EVENT_PrevClub 13 (not
14, harmless); EVENT_TopOfArc passes 13 to fn_80095744 (ignored); mode 16 SetupNextGolfer re-aims only at
nDC0 == 0 (copied from Timed); mode 16 re-pays the all-targets prize; mode 17 HoleFinished checks only
player 0; mode 6 no first-hole tips / no restart hook / stroke limit on; FE_GetDateTimeIfClockEarly
tests year < 2003 && month < 10; GetTipStat fairway % counts par 3s; tips 11 and 13 unreachable; mode 15
HoleFinished stored through a u8 cast (returns s32: port: candidate). Events never fired: 8, 12, 16,
17, 27, 30, 52, 71.

## Round 14 (rn1-rn6) leftovers
Process: rn2 / rn3 / rn5 / rn6 merged main into their branches although told not to (harmless this
time; merge_lane conflicts on files main already had were dropped). Say it louder next round.
EA identities: GameUICommands.c = the ancestor of TW07 UI_Core/InGame/APT_IG_GameMessages.c (~120
handlers paired by order and signature; this build's own file name unknown); GameMode2.c = TW07
GameMode_Skins.cpp; GameModeMatch.c = GameMode_Match.cpp; GameMode4.c's successor = TW07
GameModeDriver_TigerChallenge. Misplaced: SpeedGolf_Get*SelectedHole at GameMode2.c's end (only
GameMode8.c calls them; GameMode8.c starts right after), PlayNow_GetCurrentGroup /
GetGroupFirstChallenge at GameModeMatch.c's end.
Header lane done (rn1): save.h SponsorSlot / aSponsor, SaveProfile nTotalCash / nCurrentCash / bChanged
/ statistics, TourSeason nEventsStarted / nWinStreak / nMajorWins / nParRoundStreak; Player target-game
and mode 12 fields, bHitObject / bHitPin, nShotKind comment; GameState hooks pfn200..pfn260 (TW07
GameModeBase); EVENT_Trigger nArg. Still open: golfer.h speed golf (fC50, nC3C, nC40, nC44, nC58, nC6C,
fA80 / fA84) and mode 22 (nEA0..nEDC, vEAC) fields (rn1 checked, ready); Player n22C / n274 skin money
per hole / total (TW06 skinwin / skinwins); game.h CareerCalendar nDriver / n1C nPopupType,
gCalendarFillCell (pCellColor / pCellState), GetPopupRow nRow, GetBottomLine nLine; game.h prototype
params (PlayNow_SelectChallenge, SetCalendarFlag, ForceWeather, SendMessage18, SkillZone getters,
SetHudClock, SetShotClock, GetBullsEyeColor, ScaleTargetPoints, IsLongDrive, GetBonusIndex,
GM_Earnings_Check*Goals bPreview); GameManager.c:22 PayRoundGoals (int, u8); GameUI.c:121 GameMode22_IsActive
s32 (into mode22.h); DistributeWinnings / TournamentPayout n -> nFirstPrize; ladder.h, mode26.h,
earnings.h, challenge.h, rte.h, camera.h script.nCamera, fe.h FE_CrAP n4; GoEntry.c / FE_MessageTable.c
GameMode26_StartEvent; FE_MessageTable.c:188 PGADriver_ShowCalendar_AdvanceSeason takes (void); game.h
GameState f54 = fUITimeFactor; Ball n6C lie angle; PgaEntrantMC n18 winnings; course hole n04 rating;
gpGame n4 scoring type, b136 custom course, b137..b139 compilation / random course flags; save.h
b522F full caddie tips off; gReplayData nF12 weather option, nF14 forced weather x100; GameModeReplay.c
comments still say b30C; src/README.md:50 example pfn20C is named now.
Names for other files: FE_MessageTable.c fn_80084FF0 = MC_SetCurrentFileType, fn_80084FB4 =
MC_CallActionFnMemoryRequired (CardPos = TW07 MC_ArgsMemoryRequiredT), fn_8007C988 = GM_vMCfunction
(pairing C row wrong), fn_8007E9BC = front-end twin of GM_vIG_MCGetNumReplays; stateFunc.c fn_80062C1C =
Character_IsAnimationPaused, fn_80062C28 = Character_GetAnimationTimeLeft; GoGolfCam.c fn_800C6E44 =
post-shot camera done, fn_800C708C = camera tracking the player; GoDynamicCam.c fn_8003DCAC =
GameEffects_IsLetterboxOn; PGATourSimulation.c fn_80119808 = leaderboard tie check, fn_80117DE0 =
GM_PgaTourSim_DidUserQuit (TW07). Stale: GameUICommands.c fn_8008A8B8's "Battle mode: the winner" (=
last hole's winner); GameHoleContests.c calls in-round commands "FE message"; GameUICommands.c local
SpeedGolf_GetRoundScore(char*...) vs GameMode8.c (int nPlayer, ...).
Possible EA bugs, unlabelled: GameModeStableford_EndGame uses stroke play's prize test on points (fewer
points pays); GM_vGetOption / GM_vSetOption number options differently past 4 and set 5 stores the music
level without the volume; gSpeedGolfLogCycle written then always overwritten. Possible unlabelled fakes:
GameModeMatch.c PLAYER_AT next to PLAYER, gPlayers[(u32)i] in GetTeeHonors / GetHonors.

## Round 15 (ro1-ro6) leftovers
EA bugs: now tracked in agents/findings/2026-09-28-ea-bug-register.md (ro6 settled rounds 6-14; this
round's labels and open items are there). EA file names: FE_CrAPMessages.c = TW06 / TW07
FE_CrAPMessages.c; Calendar.c = TW07 Calendar.c / TW06 calendar.c; fe_stats.c = TW07 FE_Stats.c;
FE_MessageTable.c's handlers = TW07 APT_FE_GameMessages.c (different order: pairing by order fails).
Header lane done (ro1): Player placement momentum (fMomentumTurn / X / Z, fPlaceHeading), speed golf
nSGFlags / nSGPoints / nSGHoleScore / nUISlot / fDriveLength / nRunStartLie, long-drive nDrivesTaken ..
nPenaltyDrives, skins nSkinsWon / nSkinsTotal, CareerCalendar.nPopupType, 21 game.h prototypes,
GameMode22_IsActive in EA form (same bytes, no __cntlzw), fUITimeFactor, nLieAngle, bCaddieTipsOff.
Still open: ladder.h, mode26.h, earnings.h, challenge.h, rte.h; camera.h script.nCamera; fe.h FE_CrAP n4;
GoEntry.c GameMode26_StartEvent; PgaEntrantMC n18 nWinnings; PgaStatCounts n44 nMonthWinnings; PgaPro f50
historical score rank; course hole n04 rating; gpGame n4 scoring type, b136 custom course, b137..b139;
gReplayData nF12 / nF14 weather; pgatoursim.h prototype "TW06:" tags are TW07 names; save.h aB1CC owned /
aB344 newly unlocked / aB4BC marked new, SkinChoices.n5A7A gender, LogoRecord.b1020 kept; fe.h
FEProfile aKind / aPart / aChoice = the day's sale items (part / list entry / choice; categories -1 / -2
/ -3), b10640 logo editor works on logo106E0; GameModeReplay.c comments say b30C; src/README.md:50
example pfn20C; event.c Event 22 / 23 comments call fMomentumX the turn (it moves sideways; fMomentumTurn
turns). ~400 other prototype / definition mismatches: /home/user/scratch/tw/agents/ro1/protodiff.py
(output protodiff_after.txt; 108 in include/, 150 differ in type): a later tool / lane item.
Names for other files: MC.c MC_MemoryRequiredForOptions, fn_800A270C, fn_800A2740 declared void but their
value is used through r3 (EA wrote return ...: same bytes); FE_Manager.c fn_80079AD4's comment says player
1, code uses player 0; Code8002DB80.c Player_IsHoledNotState23 = in the cup and not GS_CONCEDED.
Style questions for the owner: stub handlers named for their slot (GM_vCrAPMessage404_Empty,
GM_vMessage25_Returns1, GM_vCommand133_Returns7, IG_vNoOp147): consistent across lanes? PGATourSimulation.c
explicit_zero_data moved file-wide (was around one global): fake-match inventory item.

## Round 16 (rp1-rp6) leftovers
FE_MessageTable.c is DONE (464/464 reviewed; header rewritten by the orchestrator). Name clashes
between lanes settled on replay (scratch r16_clashes.py): GM_vSaveGolferModel stays on message 69
(TW07's is 0xE4 bytes and also sets the session golfer); message 286 = GM_vSetEditedGolferModelID;
GM_vBackupProfileClaimRow (260) / GM_vBackupProfile (294); GM_vGetNumLadderEventsWon (74, player 0) /
GM_vGetProfileNumLadderEventsWon (174); GM_vIsCourseUnlockedOnAnyProfile (79) / GM_vIsCourseUnlocked
(207). EA bugs: register updated (8 open items from this round, GM_vMaskString's label extended).
Header lane done (rp1): ladder.h, mode26.h, earnings.h, challenge.h, rte.h, camera.h CamScript fade
fields, fe.h CrAP / sale / logo-copy fields, pgatoursim.h, save.h, GameState.nScoringType /
bCustomRound / bRandom18 / bDream18 / nRegionalRound, HoleData.nRating, MC.c's three MemoryRequired
functions now return s32 (same bytes), stale comments.
Still open (headers): golfer.h PlayerProfile.n0 = shirt (nShirt; Character_SetClubsAndClothes
"shirt<n0>", TW07 shirtsAvailable[4]); GameOptions n14 (only messages 231/243), n18 green speed, n1C
rough, n20 fairway speed, a24 [0] caddie tips [1] putting tip [2] break line [7] swing trail, a0[2]
(no mixer), a7[0] vibration, rows[4][19] = a flag per EA Trax track per music row; fe.h FEScreen.n38
= controllers plugged in, a2C = controller given to a player; FEProfile.b11702 = every golfer counts
as unlocked, b0 = trophy-ball replay (records mode 27), n3 / n4 = save slot / custom round being
edited, n11704 = the last EA Sports Bio error; FEState.n1C = memory card reward money, b0F = TW07's
firstTime (intro movie, no loading screen), b11 = demo, b18 = TOUR card withheld; FEMovie.nBio is
never written; lbl_80281374 comment ("not placed yet": it is fe_movies.c's 0.25f, only written);
camera.h CrAPState.b83 = dims the menu golfer; save.h SavedRound.n0 = in use, SaveProfile.n54C2's
"-> PlayerProfile.unk2" is stale (now n2, the glove variant), aAward = trophy balls (0..22 game
progress, 23..38 bonus); FE_CrAP_GetCategoryInfo pB1CC / pB344 params (pOwned / pNew: definition
edit); fe.h fn_80077B18 prototype nGolfer vs definition n; long lines already there: camera.h 37,
golfer.h 23, game.h 14. ~400 prototype / definition mismatches: ro1's protodiff.py.
Globals (other files): lbl_801D7148 = the front end's state (FEState), lbl_80281ED4 = the profile
being worked on (FEProfile*), lbl_80191990 = course names, lbl_801894E8 = the 16 golfer ids unlocked
by default (FE_Manager.c), lbl_80281DF4 = cheat-code unlocks, lbl_80281FFC = gMCCurrentFileType,
lbl_8018C7D8 = the MC action-function table.
Names for other files: MC.c fn_800A2630 / fn_800A26A0 / fn_800A2668 = TW07 MC_IsOptionsDataCorrupt /
MC_IsUserDataCorrupt / MC_IsReplayDataCorrupt, fn_800A09EC = MC_GetNumReplay, fn_800A270C =
MC_MemoryRequiredForUser, fn_800A218C (always MC_ERR_NOFILE), fn_800A2194 (searches the card for
"BASLUS-20572", a PS2 product code not ours: which game is unknown); Code8002DB80.c
Player_IsHoledNotState23 (holed out, not conceded: rename); FE_MessageTable.c's local prototypes
`u8* GameMode26_StartEvent` / SetTargetScore tagged "// CharSliders.c" (GameMode26.c, void: use
mode26.h); GameUICommands.c ~1540 comment names MC_SetCurrentFileType twice; locals GameModeReplay.c
nF12 / nF14 (nWeather / nWeatherAmount), stateFunc.c nCamera (nFade).
Least-sure names this round: GM_v*ProfileSecondName, GM_vLookUpEarningsRange, GM_vQueueMovieKind2,
GM_vSet/GetTapinsOption, GM_vGetStringWidth, GM_vSet/GetSwingAidOption (position only; the code's
option is the swing trail), GM_vSet/GetOnOffOption3-6, GM_vSet/GetOptionN14, GM_vGetTourCardWithheld,
GM_vBonusTrophyBallsWon (TW07 has GM_vTrackingTigerBallsWon there), GM_vFEMessage151_Return30Or60,
GM_vMCHasSLUS20572Save, GM_vGet/SetFEStateB10, GM_vGbaIsReadPending (always 0),
GM_vGetThreeLevelsOneRaised, GM_vEASBioIsNotWrongFile.

## Round 17 (rq1-rq6) leftovers
The front-end area is DONE (FE_Manager, PasswordManager, uiProcessInterface, uiLoadFile, uiTransform,
fe_movies, Code80090940, uiText, uiArc, uiobject, Trax, fe_craputils, FE_LogoDesign, LogoTexture,
Code800B90F4, gbacable). Replay notes: rq1's field scripts matched lbl_801D7148 / lbl_801D87C0 /
lbl_80281ED4, which rq2 / rq3 renamed (gFEState / gUIState / gpFEProfile) the same round: the
orchestrator re-applied them through the new names (scratch r17_fieldfix.py). Next time a header lane
renames fields of a struct whose global another lane renames, run the header scripts on the new names.
EA file names proven (for the owner; not renamed yet): fe_movies.c = EA's uiProcessPolygon.c (TW07's
file holds UI_InitLoadingBar / UI_DrawLoadingScreenAndProgressBar; TW06 lists uiprocesspolygon.c
among the iStudio runtime files; link order uiProcessInterface < uiProcessPolygon < uiText);
Trax.c = uiEATrax.c (TW2003's string "uiEATrax.c" right after "crcmp_mad_codec.c"; TW07 order);
uiobject.c = uiObject.c (TW07 order). Misfiled-unit evidence: LogoTexture.c is FE_LogoDesign.c's tail
(code starts at its end, no data of its own, TW07's FE_LogoDesign.c ends with pixel helpers), and so
probably is unsorted/sweep_8010FF5C.c; Code800B90F4.c is two units: 0x800B90F4..0x800B9944 carries
EA's only "rcmp_mad_codec.c" string (so our rcmp_mad_codec.c, the block decoder and IDCT, may be TW06's
maddec.c / madidct.c), then the Create-A-Player ball from 0x800B9944 (an .sbss gap at 0x802821BC);
PasswordManager.c's SaveProfile_* tail has no TW07 PasswordManager counterpart (maybe user.c's unit);
Code80090940.c: no evidence either way.
Still open (headers): fe.h FE_DateToInt / FE_IntToDate prototypes (a, b, c) / (n, pA, pB, pC) vs the
definitions (nMonth, nDay, nYear) / (nDate, pMonth, pDay, pYear); fe.h extern comment columns after
the renames (gStartUnlockedGolfers, gpFEBios, gFEBackupSize, gFEBackupAramAddr); fe.h LogoEdit.n0 =
nLogo (0..4); fe.h FE801D8858 = the loading bar's timing (n0 set never read, f4 next tile time, f8 next
redraw, fC seconds per tile, f10 elapsed, b18 running, n1C last tile) and its "0x8009170C" comment is
wrong; FE801D8890 per 'txf2' bank (n4 > 0: pixels freed before a movie); FEQuad n2 table (-1 none),
n0 entry, nA ignored; fe.h section heading "the front end's movies (fe_movies.c)" stale; fe.h
FE801D880C = the delayed hint (n0 frame counter, n4 hint value); FEScreen is the UI's input and fade
state (a28 last frame's a1, a30 per-controller input enabled, b40 blocks buttons, b49 hides the menu
golfer): the type name misleads; FEProfile.a1C0 (lock kinds 25 / 26) unnamed; FEState.b10 unnamed;
save.h gPasswordEnteredBits comment says "the code at 0x80056480" (PasswordManager_IsPasswordEntered);
save.h SkinChoices n0 / a1 / n81 / a82 / n102 / sz103 = the custom animation lists; save.h a10548 =
the UserInfo flag bits; engine.h UFontContext.fB4 = line spacing; uistudio.h UISMessageFncT n1..n5 =
group, screen, param count, params, return (Madden's UISCallbackMessageFnc); UISPluginFncT = Madden's
_BlankProcess(pObjData, ProcessID, nParam, pParam, pReturn); game/frontend.h FrontEnd.pC = the picture
list ('GRPS' / 'MPCS'), UIFileEntry / UIMovieData are picture entries, not movie entries; trax.h
TraxTrack sz0 / szSong / sz100 = the three lines shown; core/gbacable.h GbaChannel n0 port steps,
uStatus = JSTAT byte, u5C SI error values, n74 never read; gGbaSearchStartTick comment; the GBA
prototypes FE_MessageTable.c and gomainloop.c declare locally belong in core/gbacable.h;
OSGetResetButtonState / OSResetSystem declared locally in gbacable.c (OSResetSystem as s32 there,
void in extern/). Prototype mismatches: gomainloop.c UI_DrawInterface(int) / UI_UpdateInterface(int)
vs (s32 nTicks) / (u32 uEvent); uiProcessInterface.c UIText_SetFontDrawQueued / UI_EATraxFreeLogo
declared s32, defined void; LLPict_Gc.c MAD_InitDecoder (int), MAD_CloseDecoder (void), MAD_IsAtEnd
(MadDecoder*), MAD_GetNextFrame / MAD_ReleaseFrame (void*); FEgolferanim.c FE_CrAPBall_Render(int) vs
(u8 bTarget); Code8002DB80.c Input_vSetVibrationStatus (int nPad, u8) vs (int nController, int
bEnable); the MAD_ / FE_CrAPBall_ prototypes belong in llpict.h / fe.h. ~370 others: protodiff.py.
Globals (other files): lbl_80281DF4 (user.c) = cheat-code unlocks for every profile;
gInterruptsOffDepth / gpInterruptsOffDepth are defined in Trax.c's data but used by LLDisp_Gc.c.
Least-sure names this round: FE_CloseManager (TW07 splits it in two), FE_CrAP_RandomizeFace (also hats),
FE_CrAP_EquipDefaults (order and size only), FE_PlayRTEMovie / FE_PlayLadderMovie,
UserInfo_InitCrAPItemBitArrays, FE_LogoDesign_UploadCustomLogo (0x8010FA00; the pairing row said
0x8010FAF4), UserInfo_*CourseSlot21/22, UI_OpenInterface / UI_vInitModule (TW07 bodies differ),
gUIState / UI_BlankProcess1-6, gbUIFirstMenuDraw, gUITxf2Bank*, UIText_SetFontAlignPoint,
Gba_ReadHandshakeCode, the GBA "UndoTransfer" / "Unsaved" flags (never raised).

## Round 18 (rr1-rr6) leftovers
The AUDIO area and the commentary scripts are DONE (startUp, hlaudtrack, hlaudtrackseq, hlaudtrackstm,
hlaudmovie, hlaudvoice, HLAudMaster, UAudContainers, UAudMemStack, AudLock, AudReverb, SitDev*).
name.py now lets an EA name spelled as EA did through for E1 (EA's text in this build) as well as E2
(rr4, a428185). Kept EA's spelling `_SetStateVecAndCondition` (TW07's inline; a reserved identifier
in C, same bytes).
Misfiled-unit evidence (report only; nothing moved): startUp.c 0x800B1960..0x800B1AA8 = EA's
UAudVector.c (TW07's four functions; the two header inlines out of line at a unit's end); the ball test
0x800B1AA8..0x800B1D3C (DynObj / Ball only) another unit, perhaps GoDynObj's area; whether the start-up
command table (0x800B1D3C on) is startUp.c's is open. hlaudmovie.c = three TW06 files in link order:
hlaudmic.c 0x800A8754-0x800A87D8 (.sbss 0x80282068-6F), hlaudmovie.c 0x800A87D8-0x800A8D00 (.bss
0x801F1850), hlaudsession.c 0x800A8D00-0x800A9590 (.sdata 0x80281468, .sbss 0x80282070-95);
TrkRender3D / TrkRenderStereo (0x800A9590-0x800A9808) open TW07's HLAudTrack.c (so hlaudtrack.c's);
UAudMemStack.c's AudMem_ part (from 0x800B5B80) = TW07's UAudMem.c (TW06 lists uaudmem.c too);
HLAudMaster.c starts with Ses_GetEmitterTemplateFromID (TW07 HLAudSession.c) and audfrac_Mul.
Still open (headers): audtrack.h AudVoice.bA_1 = tone loops (request b9), bA_6 = no reverb (request
b14); AudTrackTmpl.n0 bits 0x02 loops, 0x04 switched by hand, 0x10 no restart while playing, 0x20
reverb off; AudTrack.f48 = distance attenuation (TW07 distAttn), not a priority (Trk_AllocPerf /
Trk_UpdatePerf parameter fPriority); f50 = pitch ramp; AudVoiceParams.a8 = ADSR volumes, nC = start
offset; AudTrackFlags.b7; AudTrackStmFlags.b6 never set (state 5 never happens), b3 = silence queued
after the end; AudStreamRead.bRestart queues a silence block; Trk_InitSession(nSession, nSubSession);
AudBlock48 = the listeners' block (gMicData); audcontainers.h UList.n8 = TW07 nslots (UList_Reset
prototype says n8), UAudMemStack.n14 (bottom blocks? a guess), AudReverb's globals declared there;
core/startup.h SoundHeader u0 start / current, u4 end, u8 loop start, uC loop flag; startup.h line 59
nState comment past 110; game.h GameEffects b47 / b4A comments wrong (u48 / u4C are lines to PLAY at
the GameBreaker's end: fails / works, TW07 SetPostGB[Negative]Commentary), nCrowdReaction = crowdLevel;
sitdev.h SitDevEntry.aOp "3 <, 4 >" reads backwards (3 tests value > argument, 4 value < argument),
SitDevEntry / SitDevAction / SitDevEntry8 / SitDevScripts = TW07 Situation / Action / Response /
SitDevHeader, SitDevEntry.n0 = the group whose flag is in pD4, b2.s.n5 = TW07 iFileIndex,
gSitDevPredictedHitClass is the look-ahead ball's (event 29), its "// SitDevFile.c" prototype heading
and top comment stale, renamed extern lines past 110; engine.h / game.h notes "SitDevFile.c:" on
Gaud_StartRegularComment and the GM_ wrappers should say SitDevTrigger.c / SitDevStateVector.c;
UStream.c prototypes Ses_AllocBankHdr(u32, int) vs (u32, u32), Ses_AllocSampleAram void* vs u32;
Ses_AllocBankHdr's inline "fake match: EA bug: no return" label: the missing return is EA's form (the
"fake match:" part looks wrong); FE_CrAPBall_* local prototypes (streammanagerhole.c, FEgolferanim.c,
gomainloop.c) and MAD_SetReadCallback (LLPict_Gc.c) belong in fe.h / llpict.h; UI_BlankProcess1-6
still use (n2, pn3, n4); FEVertex f0 / f4 texture coordinates, f8..f10 position; SaveProfile.a200
probably TW07 AwardInfoPGAMoneyList; TraxTrack sz0 / sz100 (artist or album: the 'TRAX' data);
FEState.b10 (only messages 333 / 334); ~370 protodiff rows.
Globals not renamed: startUp.c's sound-data arrays lbl_8018F040 / lbl_8018F640 (tools/build/gendata.py
looks them up by name: a coordinated rename) and three KEEP_UNUSED words; gSesUnread8C / 90 / 94 / 95
(cleared, never read); lbl_80281DF4; gInterruptsOffDepth lives in uiEATrax.c's data.
Least-sure names: AudDma_InitModule / InitSession / ExitSession, BootSound_InitModule (empty, placed by
position), Startup_LoadLegalPicture ('LEGL' = legal: TW07 FE_GetLegalChunkID), GM_vStartupPlaySound,
GM_vStartupSkipCardLoad, GM_vStartupLoadOptionsCheckDisc, the GM_vStartup prefix itself (rule 5 names
only FE and IG), Ses_GetInstrumentTone, Mas_GetUpdateRateScale (always 1), Ses_IsSessionZero,
Ses_ProcessArticulationData, Voc_StartStream, Voc_ResetModule, the empty AudMem_* steps,
SitDev_BindHeader, SitDev_BeginLoadScripts, SitDev_Weather* (bit 1 probably rain),
Gaud_StartPlaylist2Comment.

## Round 19 (rs1-rs6) leftovers
MOVIES and CAMERAS are DONE (LLPict_Gc, LLVideo, LLPictInt, rcmp_mad_codec, GoGolfCam, CamSpline,
GoComicCam, GoCamTuningVars, GoDynamicCam, GoStaticCam, gocamscripts, GoCamCont). EA's own spellings kept:
CamScript_IngoreCameraCollisionSurface, IsDefualtSeq. Replay: rs5's globals.tsv had a row with no tab
(fixed copy in scratch r19/); the replay helper now skips the slow reference refresh with NOREFS=1
(one refresh per round, scratch replay_r19.sh).
File names (owner's call; nothing moved): our rcmp_mad_codec.c is EA's maddec.c / maddeca.c / madidct.c
(TW06 PDB; NFSMW's rcmp package keeps rcmp_mad_codec.cpp apart; a second static discardbits at
0x800B8A2C means maddeca.c is its own file; the .sbss pad at 0x802821BC-C0 marks a boundary), and EA's
rcmp_mad_codec.c is Code800B90F4.c's first part (its "rcmp_mad_codec.c" string, MAD_AllocFrame /
MAD_GetNextFrame). unsorted/sweep_80097E98.c's fn_80097E98 = TW07 CameraTuning_Close (frees gpCamTuning;
declares it `extern s32`). FO_vSetCurrentColor sits in LLVideo.c's range though TW07 has it in UFont.c.
Still open (headers): camera.h CamShot = TW07 CameraScript_t (f78 / f7C fov start / end over f48, f88 depth
of field, f8C / f90 blur, f94 / f98 shake amount / speed, f9C roll, f70 / f74 look-at side / up offsets,
f60 / f64 distance-or-share / side offset, f68 / f6C min / max height, bAA shows the golfer, bAB blend kind,
bAC look-at kind, bAD state / shot kind, bAF / bB0 tracking points (16: at the ball), bB1 tracking mode,
bB2 height reference); CamScript (v0 / v10 current / next camera positions, a20[0..3] / [4..7] look-at
points, fA8 roll, nBC blend kind, nC4 requested event, fF0 / fF4 shake length / intensity, fEC ball-update
rate, fF8 blend share, nD0 arc direction, nE0 / fE4 time-trigger event / time, fDC lag angle, bCF fairway
fix running, bE8 no ground, n110 completed replay cams, n114 final-cut flag, f8C moves on to pNextShot);
CamSequence (b44 state, b46 shot type, b49 / b4A start / end lie masks, f24..f28 distance range, f2C..f30
pin height over ball); View (v20 side vector, n260 special swing kind, b268 skip fancy pre-shot cams, b269
show post-shot anims, b26A post-shot ball removal); GolfCamState (maybe TW06 GolfCamManager_t: f64 heart
beat slow-mo rate, b54 matrix, b55 matrix mode, b56 comic, b58 super zoom, b59 slow-mo swing, b5A heart
beat, b5D, n60 fly-by route); camera.h f154 comment; dyncam.h CamChoice = TW07 CameraEventTrigger_t (b14
event, b15 interp type, f0 time, f4 max speed, b16 cut event, f8 its time), DynCamSet = CameraAnimPairs_t,
DynCamTables = TW06 DynamicCamManager_t; comiccam.h bNext = frozen screen; camera.h long lines (80, 757,
~774 CamSpline prototypes); llvideo.h:65 long; Video p1018 always NULL, b1021 never 1, b1020 running, bEnded
= stopped starved; LLPict f6C / f70 far-corner texture coordinates; PictStream.pDecoder is MadDecoder*;
MAD_SetReadCallback / MAD_initdecode / MAD_decodemacroblock into llpict.h (retype fn_8002FEB0 /
LLVideo_ReadNextFile's reader chain first); RenderState_SetScissor / SetPicture / RenderView_MakePictUV /
FO_vSetCurrentColor into engine.h (local copies in FEgolferanim.c, uiText.c, GoGrass.c, shadow.c,
ScreenClear.c, uiEATrax.c differ); gMadCoefCodes externs misaligned; local prototypes left: hlaudvoice.c
HwVoice_*, GameAudio.c Mas_ / Mic_ / Ses_ / Mov_ (Ses_Init(nSession, nSubsession)), SitDev.c, char.c
SitDev_SetupClubCondition (says SitDevFile.c), hlaudtrackstm.c File_ReadAsyncEx; AudTrackSeq n64..n69.
Least-sure names: GolfCamera_Choose3ShotCam, GreenRoll (camera 5), the camera 24 names,
GolfCamera_IsThereACameraGoingToBeTimeTriggered, GolfCamera_ZoomGreenCamera / UnZoomGreenCamera,
GolfCamera_CreateReplayCamera, GolfCamera_ChooseSuperSwing, GolfCamera_IsSetUpCameraDone,
CameraController_ResetAimMarkerInSwingCamera, CamScript_LerpFixedTargetCameras, DynamicCam_LoadFilesFromDisk(FE),
DynamicCam_bIsSwingCamera, StaticCam_ProcessScript, madvlctbl2 / 3, idctprescale / idctinput,
Pict_AfterFree / StartMovie / OnFirstMovieFrame, LLVideo_Begin/EndPlayback, LLVideo_DarkenScreen.

## Round 20 (rt1-rt6) leftovers
DONE: GoTerrainCollision, TerrainData, TerrainGround (rt2), GoDynObj, GoDynObjBase, GoDynObjTypes,
GoAnimalActors, UObject (rt3), CourseData, UKernel, Wind (rt4), Ball (rt5), GoTerrain 0x80030254..
0x80034AE4 (rt6; 49 left from 0x80034CAC: next round). EA's names: Physics_* (TW07 Physics.c),
GM_* / GM_CourseInfo_* (TW07 CourseInfo.c), Network_* / wn_PnPoly (TW07 UNetwork.c), DynObj_* (TW07
GoDynObj.c), Wind_* (TW07 Wind.c), MaterialTypes_getMaterialID (TW07 MaterialTypes::getMaterialID).
TW06 names taken from earlier comments that quote the TW06 Xbox PDB with signatures
(Ter_LineTriangleIntersection, Ter_GetSupportingAndCoveringGroundTriangles, Ter_LineSphereIntersection,
the four *OneGrid walkers, Ter_GetBarycentricCoords): the PDB's function list is not in docs/, so
these cannot be re-checked in the repo (owner question: export it). docs/tw06-names.md's
80048AF4 = Kernel_InitModule was wrong (it allocates; the init is 0x80048DD0): row corrected.
Pipeline: wraplong split a macro line (Ball.c BALL_LANDING_EVENTS) and broke the build: fixed
(afb91b2 / dbdf43e); lanes rewrapped header lines their renames lengthened (include/ is the header
lane's): round 21 rule, lanes list them and the orchestrator rewraps once; rename.py leaves extern /
prototype trailing comments out of column (terrain.h rt6 globals, fixed by hand): tool fix pending.
rt1 edited one of its repo-changing scripts with a heredoc (the saved script was re-run clean).
Still open (headers): ball.h Ball.f70 = the random lie quality (Physics_SetLie; added to the launch
factor by Physics_GetLiePowerPercentage), b99 / bHitTopArc 1 for a putt 0 for a full shot
(Physics_ShotImpact), b9B set when the first bounce hands the spin-stick input to the ball (event
0x1F); ball.h:252 Network_RegisterLoadNetworkCallback's pfn gets the network type
(TNetwork.nExportType), not a chunk; dynobj.h GoDynObjPlayerA = the tee record ('TEO ' 10004, b70
struck), GoDynObjPlayerB = the divot record (10002), GoDynObjMgr.apPlayer = divot holes, apRing =
the ten pitch marks, fA94..fAAC the tee / divot throw tuning, the three "'TEO ' 1000x object"
prototype comments, UObjMeshInfo.a24 flag bytes; game.h HoleData nRating = EA's handicap, n08..n14
hole length from tees 3..0, b34 split-screen low detail, b35 longest-drive contest, b37 counts for
driving stats, unk1C..0x2B maybe TW07 DriveDistanceNSideGame, aTeeSets[].n0 maybe rating / slope;
gCourseNames declared twice (fe.h:535 [30], game.h:22 [NUM_COURSES]); camera leftovers (rt1): CamShot
f48 / f4C / f80 / f84 / f8C / f90 / nA4 / bA8, CamScript f84..fA4 / bCC..bCE / f100..f10C / n110 /
n114, GolfCamState b57 / b5B..b5D, CamChoice p10 / b17, DynCamTables n1C; nBlendKind vs EA's
interpType (owner style call); type renames to TW06 names not done (unconfirmed pairing);
GameAudio.c's startUp.c prototype block (needs startup.h); File_ReadAsyncEx: hlaudtrackstm.c's local
copy differs from LLFileIO_Gc.c's definition (callback type and parameters): needs a decision.
Least-sure names: Physics_CheckObjectCollisions (C pairing), Physics_GetDistanceToCup /
ApplySuperSucka (order only), Ball_Vec3DotMul, the divot trio (DivotAdd / ShotDivotHoleHide /
DivotHide), DynObj_PitchMarkAdd, DynObjBase_TakeAmount, DynObjType6_* / 9_*, Kernel_CloseModule,
Kernel_PostPairByKey147 / 148, GM_GetHoleIndexDrivingSideGame, GM_GetCurrentHoleSplitScreenLowDetail,
isLeft, UObject_ComposeRotation, Network_RayIntersection, Ter_DrawFarClipPatches,
gTerCrowdPoseStepsFlag40, gTerUnreadToggle, nHeightRef, nTrackMode, bFairwayFix.
