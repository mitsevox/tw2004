# mwcc-debugger batch, 2026-09-26 (all 80 non-exact functions)

`python tools/match/mwccdbg.py --all` on main at 84f7d39. Per function: how many allocator variables sit in a
different register from EA's ("differ"), and the first such variable from the top of the priority
list (where to start). Full dumps: rerun `mwccdbg.py src/<Unit>.c <fn>` (build/ is not in git).

Buckets: 0 differ = registers already EA's, the miss is instruction order/choice (read the
backend-NN pass dumps); 1-2 = a single misplaced variable (best targets); 3-12 = usually one root cause
cascading; 13+ = structurally different code (rewrite first). The EA register is a majority vote over
instructions that line up, so on code far from EA's (below ~95%) a 0 can hide register differences.

| differ | unit | function | match % | first differing variable (ours -> reg, EA reg, neighbours, name) |
|---|---|---|---|---|
| 0 | CamSpline | fn_800C7A9C | 98.5 |  |
| 0 | Code8002EE1C | Session_SetupProfiles | 95.8 |  |
| 0 | GameModeBestBall | fn_800E8A68 | 95.1 |  |
| 0 | GoBreakLine | BreakLine_Render | 99.9 |  |
| 0 | GoShaderObject_Grass_Gc | SD_vShaderObject_Grass_Static_Render | 99.3 |  |
| 0 | GoShaderObject_Particle_Gc | fn_800949D0 | 95.7 |  |
| 0 | GoStaticCam | StaticCam_GetFlybyInformation | 100.0 |  |
| 0 | GoTerrain | fn_80032B7C | 98.5 |  |
| 0 | LLPictInt | PictInt_Decode | 93.3 |  |
| 0 | LLTex | fn_8000EA1C | 99.0 |  |
| 0 | MC | fn_8009F8C8 | 96.0 |  |
| 0 | SunFlr_Gc | fn_8009A708 | 98.7 |  |
| 0 | Swing | SW_KillVibration | 92.8 |  |
| 0 | UISApi | fn_80168F5C | 88.1 |  |
| 0 | UISApi | fn_80169DC4 | 96.7 |  |
| 0 | UISEvent | fn_80165670 | 97.0 |  |
| 0 | UISEvent | fn_80165B90 | 99.6 |  |
| 0 | UISEvent | fn_80165E9C | 95.6 |  |
| 0 | UISScreen | fn_8016A510 | 94.5 |  |
| 0 | UISScreen | fn_8016B188 | 99.9 |  |
| 0 | UISScreen | fn_8016C6C4 | 90.5 |  |
| 0 | char | fn_80017DDC | 98.6 |  |
| 0 | char | fn_8001F110 | 99.1 |  |
| 0 | gocamscripts | fn_8003F2E0 | 98.3 |  |
| 0 | hlaudtrackstm | Stm_Tick | 99.1 |  |
| 0 | hlaudtrackstm | fn_800AB860 | 84.5 |  |
| 0 | hwsBurn | fn_8011172C | 99.8 |  |
| 0 | rcmp_mad_codec | fn_800B769C | 98.5 |  |
| 0 | startUp | fn_800B0748 | 99.4 |  |
| 0 | uiText | fn_800922A8 | 99.8 |  |
| 1 | GameMode22 | fn_801264B8 | 85.9 | r44   -> r8   !EA r6    14 nb  @228 |
| 1 | GoGreenGrid | GR_BuildGridRenderData | 99.0 | f51   -> f24  !EA f21   57 nb  @268 |
| 1 | GoShaderObject_Particle_Gc | fn_800951A0 | 92.0 | r35   -> r5   !EA r3    13 nb  pData |
| 1 | UISApi | fn_80168CD8 | 99.6 | r41   -> r22  !EA r20   30 nb  pCopy |
| 1 | UISApi | fn_80169308 | 99.8 | r35   -> r30  !EA r29   34 nb  n |
| 1 | UISApi | fn_801694A0 | 93.0 | r40   -> r3   !EA r5     9 nb |
| 1 | UISApi | fn_80169D90 | 66.8 | r39   -> r4   !EA r0     6 nb |
| 1 | char | Character_SetupForShot | 99.9 | r51   -> r7   !EA r6    11 nb  @1457 |
| 1 | skalib | AnimLib_WasLastPlayed | 89.1 | r39   -> r8   !EA r6    12 nb  @1460 |
| 1 | skalib | fn_80026844 | 99.2 | r58   -> r9   !EA r4    20 nb  @1805 |
| 2 | SkinPart | fn_800CE52C | 99.6 | r47   -> r31  !EA r26   31 nb  @868 |
| 2 | Swing | SW_vImpact | 99.8 | r40   -> r29  !EA r27   34 nb  @374 |
| 2 | UISEvent | fn_80165ACC | 95.1 | r40   -> r30  !EA r28   27 nb  uG |
| 2 | UISScreen | fn_8016B4D4 | 98.7 | r56   -> r29  !EA r8    56 nb  @1006 |
| 2 | UISScreen | fn_8016C614 | 90.2 | r40   -> r5   !EA r6    11 nb  @1357 |
| 2 | gocamscripts | CameraScript_LagAimMarker | 99.8 | f39   -> f30  !EA f26   33 nb  fMin |
| 2 | hlaudmovie | fn_800A8AD4 | 99.6 | r53   -> r30  !EA r29   24 nb |
| 3 | Earnings | fn_800D4F14 | 99.8 | r69   -> r10  !EA r11   27 nb  @843 |
| 3 | GoShaderObject_Rain_Gc | fn_800B52D4 | 99.8 | r37   -> r22  !EA r19   61 nb  pData |
| 3 | UIStudio | fn_80168918 | 97.4 | r55   -> r27  !EA r22   33 nb  @704 |
| 4 | SkinPart | fn_800CE224 | 99.4 | r52   -> r30  !EA r29   58 nb  @823 |
| 4 | UISApi | fn_80169858 | 98.0 | r32   -> r27  !EA r24   73 nb  pStudio |
| 4 | goballfx | BFX_vRender | 90.1 | r62   -> r6   !EA r7    11 nb |
| 4 | uiProcessInterface | fn_8008F820 | 99.7 | r40   -> r22  !EA r24   52 nb  @260 |
| 5 | UISApi | fn_8016A030 | 94.8 | r47   -> r31  !EA r25   54 nb  @474 |
| 5 | UISScreen | fn_8016ABBC | 96.2 | r44   -> r30  !EA r29   31 nb  @781 |
| 6 | UISApi | fn_80169590 | 98.3 | r59   -> r31  !EA r30   35 nb  @258 |
| 6 | char | fn_80019798 | 99.5 | r47   -> r28  !EA r25   51 nb  @626 |
| 7 | Earnings | fn_800D477C | 99.8 | r235  -> r19  !EA r17   37 nb |
| 7 | GoTerrain | fn_80031E58 | 83.6 | r180  -> r7   !EA r8    17 nb |
| 7 | UISScreen | fn_8016AD54 | 93.0 | r35   -> r31  !EA r27   31 nb  p |
| 8 | LLFont | fn_8001144C | 89.8 | f66   -> f27  !EA f24   54 nb  @250 |
| 8 | LLTex | TX_spParseTextureGroupFromStream | 84.9 | r46   -> r31  !EA r27   95 nb  pBank |
| 9 | UISScreen | fn_8016AEEC | 97.6 | r56   -> r31  !EA r27   49 nb  @853 |
| 11 | LLDynTex | fn_8010A930 | 93.5 | r35   -> r31  !EA r27   85 nb  nMode |
| 11 | UISScreen | fn_8016A2D4 | 96.6 | r36   -> r31  !EA r28   53 nb  uEvent |
| 11 | uiArc | fn_80102AC8 | 95.9 | f39   -> f20  !EA f15  165 nb  fSin |
| 12 | LLFont | fn_8001208C | 96.3 | r39   -> r8   !EA r31   32 nb  pNext |
| 14 | GoShaderObject_Particle_Gc | fn_80094534 | 97.7 | f73   -> f31  !EA f28  111 nb |
| 19 | UISApi | fn_80169C0C | 69.4 | r84   -> r8   !EA r28   34 nb |
| 28 | Golfer | AI_ChooseTarget | 98.7 | r41   -> r31  !EA r21  137 nb  p |
| 29 | DepthField | DF_vDrawBufferToScreen | 97.9 | f105  -> f1   !EA f0    16 nb |
| 30 | LogoTexture | fn_8010FC3C | 78.2 | r39   -> r12  !EA r22   38 nb  nBase |
| 40 | UISScreen | fn_8016B844 | 91.6 | r48   -> r29  !EA r26  130 nb  pStart |
| 53 | GoShaderObject_Grass_Gc | SD_vShaderObject_Grass_Static_Init | 82.1 | r50   -> r31  !EA r29   88 nb  pNewStep |
| 62 | skalib | AnimLib_PlanBank | 91.6 | r43   -> r27  !EA r25   64 nb  j |
| 66 | skalib | AnimLib_MergeOverlay | 87.7 | r58   -> r31  !EA r15  100 nb  nCopied |
| 77 | UISScreen | fn_8016C270 | 85.5 | r46   -> r31  !EA r30  122 nb  pFrame |
| 110 | UIStudio | fn_80166098 | 97.8 | r120  -> r6   !EA r7   478 nb  pTop |
| 126 | LLFont | FO_spLoadFontFromStream | 84.8 | f87   -> f2   !EA f1     2 nb |
