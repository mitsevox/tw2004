# Misfiled units: the to-do list (from 2026-09-29)

Code that sits in the wrong file or under the wrong name. Owner (2026-09-29): keep every one as a
to-do, keep adding as the naming pass finds them, and tackle them all at the end, once naming and
comments are done. The orchestrator appends each round's findings here (with the round and the
evidence); nothing is moved until then. Done items move to "Done" with the commit.
Tiers: **proven** (evidence settles it), **strong** (evidence points one way; a lane must pin the
boundaries), **weak** (a hint: settle or drop with evidence). "Passed" = the file is already through
the naming pass. Details and evidence: agents/findings/2026-09-27-naming-leads.md (round given).

## Open

| Item | Tier | Passed | Evidence (source) |
|---|---|---|---|
| unsorted/sweep_80097E98.c fn_80097E98 = TW07 CameraTuning_Close (frees gpCamTuning; right after CameraTuning_Init); declares gpCamTuning `extern s32` | proven | no | round 19 (rs4) |
| Quaternion.c = EA's MathQuat.c | proven | yes | matching-era audit (state.md) |
| TibExt.c's tail = EA's SharedFileIOCallbacks.c | proven | no | matching-era audit |
| Code8009A928.c = EA's GlowMgr.c | proven | no | matching-era audit |
| Code8009AA28.c = EA's SunFlr.c | proven | no | matching-era audit |
| Ball.c = EA's Physics.c + Wind.c | strong | no | matching-era audit |
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
| src/unsorted/: 28 sweep files, 23 not through the pass; place each in its home file (TW07 order + neighbour references; sweep_80155F40 is C library code, sweep_800055D8 is main()) | - | 5 of 28 | plan-readability.md |

## Done

| Item | Commit |
|---|---|
| 14 files renamed to EA's names (FE_Calendar.c, GameMode.c, CaddieTips.c, PlayNowMode.c, LadderedMode.c, ...) | 9826752 |
| event.c's tail -> SitDev.c | 356313a |
| fe_movies.c -> uiProcessPolygon.c, Trax.c -> uiEATrax.c, uiobject.c -> uiObject.c; LogoTexture.c + sweep_8010FF5C.c into FE_LogoDesign.c | 2026-09-29 |
| UAudMem.c out of UAudMemStack.c; startUp.c -> + UAudVector.c, Code800B1AA8.c, Code800B1D3C.c; hlaudmovie.c -> hlaudmic.c / hlaudmovie.c / hlaudsession.c | 37c5c96, eb2462e, b2f7315 |
| rcmp_mad_codec.c -> maddec / maddeca / madidct; Code800B90F4.c's first part = rcmp_mad_codec.c | in progress (sp4) |
