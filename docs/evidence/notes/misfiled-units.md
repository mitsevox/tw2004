# Misfiled units: the to-do list (from 2026-09-29)

Code that sits in the wrong file or under the wrong name. Owner (2026-09-29): keep every one as a
to-do, keep adding as the naming pass finds them, and tackle them all at the end, once naming and
comments are done. The orchestrator appends each round's findings here (with the round and the
evidence); nothing is moved until then. Done items move to "Done" with the commit.
Tiers: **proven** (evidence settles it), **strong** (evidence points one way; a lane must pin the
boundaries), **weak** (a hint: settle or drop with evidence). "Passed" = the file is already through
the naming pass. Details and evidence: agents/findings/2026-09-27-naming-leads.md (round given; removed
2026-10-03, in git history at 7ff1105).

## Open

| Item | Tier | Passed | Evidence (source) |
|---|---|---|---|
| unsorted/sweep_80097E98.c fn_80097E98 = TW07 CameraTuning_Close (frees gpCamTuning; right after CameraTuning_Init); declares gpCamTuning `extern s32` | proven | no | round 19 (rs4) |
| Quaternion.c = EA's MathQuat.c | proven | yes | matching-era audit (state.md) |
| TibExt.c's tail = EA's SharedFileIOCallbacks.c | proven | no | matching-era audit |
| Code8009A928.c = EA's GlowMgr.c | proven | no | matching-era audit |
| Code8009AA28.c = EA's SunFlr.c | proven | no | matching-era audit |
| Ball.c = EA's Physics.c + Wind.c (Wind.c split off; see the round 20 row) | strong | yes | matching-era audit |
| particles.c = the OS heap + LLTime | strong | no | matching-era audit |
| goballfx.c = EA's GoLightFogEnv.c | strong | no | matching-era audit |
| OBFData.c = weather code (name open) | strong | no | matching-era audit |
| Glows: two files | strong | no | matching-era audit (no boundary evidence yet) |
| char.c's tail (ByteSwap_Records, MtaLib_SwapAndLink) belongs to mtalib.c (right after it) | strong | yes | rounds 3-6 leads |
| Unit boundaries inside units: per-unit vector-helper runs then another system at Char_Vec3Sub..Char_Vec4Add (then Camera_GetLensFovScale 0x8001EFFC), SKEL_Vec3Add..SKEL_VecSub (then BitArray_ShiftUp 0x80029C60), GameEffects_Vec3Sub (then GM_vInitModuleONCE 0x800DCBA8) | strong | yes | 2026-09-29 splits (sp2) |
| HLAudMaster.c starts with Ses_GetEmitterTemplateFromID (TW07 HLAudSession.c) and audfrac_Mul (a TW07 inline): hlaudsession.c's? | strong | yes | round 18 (rr5) |
| Golfer.c's club part | strong | yes | matching-era (no boundary evidence yet) |
| PasswordManager.c's SaveProfile_* tail (no TW07 PasswordManager counterpart): user.c's unit? | weak | yes | round 17 (rq2) |
| SkinBurn_CheckSignatureFile (start-up signature check) likely not SkinBurn's | weak | yes | rounds 3-6 leads |
| RealtimeMode.c may be EA's GameModeDriverRTE.c (nothing in the code ties it) | weak | no | 2026-09-28 file renames |
| GameMessages.c may be TW06's gameui_istudio.c | weak | yes | 2026-09-28 file renames |
| FO_vSetCurrentColor (and LLVideo.c's last setters) sit in LLVideo.c's range; TW07 has it in UFont.c | weak | yes | round 19 (rs2) |
| gInterruptsOffDepth / gpInterruptsOffDepth defined in uiEATrax.c's data, used by LLDisp_Gc.c | weak | yes | round 17 (rq4) |
| Code80090940.c: which neighbour it belongs to (no data of its own) | weak | yes | round 17 (rq4) |
| Placeholder names to settle: Code800B1AA8.c (ball-against-object test), Code800B1D3C.c (start-up UI commands), the Create-A-Player ball unit | weak | yes | 2026-09-29 splits |
| Ball.c = EA's Physics.c (Wind.c already split off at 0x80055F14): its 58 functions follow TW07's Physics.c order exactly, Physics_ForceBallInHole .. Physics_CloseModule, then the small rotate / sin-cos / vector helpers TW07 inlines there. Rename to Physics.c; the start may be 0x80050BEC (GoTerrainCollision.c's MaterialTypes_getMaterialID sits right before, TW07 Physics_GetSurfaceID) or stay at 0x80050C2C (TW06 names it MaterialTypes::getMaterialID) | proven (name), weak (start) | yes | round 20 (rt5, rt2) |
| CourseData.c = EA's CourseInfo.c: TW07 golf/gamemode/CourseInfo.c has these functions in this order; TW06 lists golf\gamemode\courseinfo.c | proven | yes | round 20 (rt4) |
| TerrainData.c's network code = EA's UNetwork.c (TW07 order: Network_FreeDownloadData .. Network_RayNetworkDoesIntersect); its tail belongs elsewhere: Ter_GetTGD (TW07 GoTerrain_TGD.c), UObject_ComposeRotation (only the object files call it), LLMath_AddScale3 / Vec3_Dot (math inlines) | strong | yes | round 20 (rt2) |
| TerrainGround.c's three functions are TW07 GoTerrainUtils.c / GoTerrainCollision_Headgate.c: the GoTerrainCollision group | weak | yes | round 20 (rt2) |
| UKernel.c Kernel_InitObjectFromDef (0x80049514, type 0's setup) is called only by GoDynObjBase.c and GoAnimalActors.c: GoDynObjBase.c's? TW07's UKernel.c has only DownloadActors / InitModule / CloseModule | weak | yes | round 20 (rt4) |
| GoDynObj.c's Character_IsTagSet (0x80048574) and Object_SetLod (0x80048584) sit away from char.c / UObject.c (header inlines?); DynObj_InitModuleEmpty / CloseModuleEmpty (0x800486EC / F0) sit right before UObject.c (its module hooks?) | weak | yes | round 20 (rt3) |
| GoDynObjBase.c / GoDynObjTypes.c: no EA file in TW06 or TW07; TW06's golf/hi-rendering/gomiscactors.c could hold types 6 and 9 (nothing proves it) | weak | yes | round 20 (rt3) |
| HoleScore.c = EA's AnalysisUtilities.c: every function is in TW07's golf/gamemode/AnalysisUtilities.c in the same order (TW06: golf/gamemode/analysisutilities.c) | proven | yes | round 21 (ru3) |
| GoTerrain.c 0x800355E0..0x80035640 (CharacterRender_SetCurrentBuffer / StartNewFrame / RenderSetup, gCharRendCurrentBuffer) = EA's CharRend.c: TW07 golf/animation/CharRend.c in this order (only SetSkinType missing); Skin.c starts at 0x80035640 | strong | yes | round 21 (ru2) |
| GoRenderCtx_Gc.c 0x800142A4 to its end + .data gauInputButtonMap (0x80186AF0) = EA's LLInput.c: TW07 legacy/ll/LLInput.c has Input_vSelectControlSet, Input_uiMap, Input_AnyPadPressed, Input_vStopAllVibration in this order with these signatures; the table's 8-byte alignment (existing fake-match note) marks the boundary | strong | yes | round 21 (ru6) |
| unsorted/sweep_800136F4.c = GoRenderCtx_Gc.c's RC_vInitModule (calls RC_vSetCurrentRenderCtx(0), TW07's inline) and RC_vCloseModule (empty), right before RC_spCreateRenderCtx as in TW07's order | strong | no | round 21 (ru6) |
| streammanagerhole.c's last function Game_GetCurHoleNum (a gpGame accessor): no TW06 / TW07 name or file ties it to the stream manager | weak | yes | round 21 (ru5) |
| src/unsorted/: 28 sweep files, 23 not through the pass; place each in its home file (TW07 order + neighbour references; sweep_80155F40 is C library code, sweep_800055D8 is main()) | - | 5 of 28 | plan-readability.md |

## Done

| Item | Commit |
|---|---|
| 14 files renamed to EA's names (FE_Calendar.c, GameMode.c, CaddieTips.c, PlayNowMode.c, LadderedMode.c, ...) | 9826752 |
| event.c's tail -> SitDev.c | 356313a |
| fe_movies.c -> uiProcessPolygon.c, Trax.c -> uiEATrax.c, uiobject.c -> uiObject.c; LogoTexture.c + sweep_8010FF5C.c into FE_LogoDesign.c | 2026-09-29 |
| UAudMem.c out of UAudMemStack.c; startUp.c -> + UAudVector.c, Code800B1AA8.c, Code800B1D3C.c; hlaudmovie.c -> hlaudmic.c / hlaudmovie.c / hlaudsession.c | 37c5c96, eb2462e, b2f7315 |
| rcmp_mad_codec.c -> maddec.c / maddeca.c / madidct.c; Code800B90F4.c -> rcmp_mad_codec.c (EA's) + Code800B9944.c (the Create-A-Player ball, placeholder name) | sp4, 2026-09-29 |
