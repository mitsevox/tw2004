EA's bugs
=========

Bugs in the shipped game's code, found while decompiling it. The decomp reproduces each one, since
the bytes require it; they're listed here instead of being commented in the source.

Confirmed
---------

The code does this, and the effect can happen in play.

| Where | What |
|---|---|
| GameModeStableford.c GameModeStableford_EndGame | stroke play's fewer-is-better prize test on Stableford points pays the losing side |
| GameModeDriverPGATour.c GameModeDriverPGATour_SetTournament | the player's green speed option (options.n18) is saved but never put back after a tour round |
| FE_CalendarPopups.c FE_GetDateTimeIfClockEarly | `nYear < 2003 && nMonth < 10` is no date cutoff (Oct-Dec of an earlier year is not "early") |
| GameAnalysis.c GameAnalysis_GetTipStat | fairway percentage divides by all holes, par 3s too |
| PGATourSimulation.c GM_PgaTourSim_CheckEndOfTournamentAward | the major-win count skips the bUser test: a major won by the pro in slot 0 of a skipped tournament counts as the player's (award 36) |
| PGATourSimulation.c GM_PgaTourSim_DistributeWinnings | no new place starts after row 70: entrants below it who made the cut share the last paid place's prize |
| PGATourSimulation.c CalcParBreakers | nBirdies already includes eagles and the formula adds nEagles again |
| FE_MessageTable.c GM_vMCLoadUser (menu message 35) | answers 1 or an error, never 0: the slot is backed up and marked loaded when the load failed |
| FE_MessageTable.c GM_vMaskString (menu message 399) | the terminator goes one past the stars; for 63 stars it writes szStars[64], past the buffer (64 or more: the loop does) |
| FE_Manager.c FE_CrAP_IsItemLocked, kind 20 | counts groups with aMedal != 0, but 0 is the best medal and 3 none: a new profile (all 3) unlocks every kind-20 item with n <= 29, and winning best medals lowers the count (message 175 tests != 3) |
| FE_Manager.c FE_CrAP_IsItemLocked, kind 24 | the loop `i = 23; i < 16` never runs: only n <= 0 unlocks |
| Code800B90F4.c MAD_ReadNextFile | the end-of-movie test repeats the NULL test above it, so nEnd is never set: MAD_IsAtEnd always answers 0 and LLVideo.c stops a movie only when it is starved |
| startUp.c Startup_CheckCards | hint 0x8B (card slot B empty) can never be sent: port 1 is only reached when port 0's status is 0, and that case tests port 0's status != 0 |
| hlaudtrackstm.c Stm_Exit | cancels its DMAs with the track as owner, but AudDma_ToAram's owner is always 0 or 1: the cancel never matches, so a queued block DMA writes into ARAM buffers just freed and its callback counts into the freed track |
| UAudContainers.c UList_InsertAt | an insert at the tail (not also the head) links after it, not in front: InsertSortWorldPerf leaves a higher-priority track at the tail, where Trk_AllocPerf steals first |
| gocamscripts.c CamScript_CircleCameras | fA8 (the roll) blends by fT (0..2) instead of fShare: the roll runs on to twice the difference and snaps back when the blend ends |
| rcmp_mad_codec.c madinit | `for (i = -256; i < 255; ...)` never writes gMadClamp[255] (.bss zero): a block value of +255 becomes pixel 0 (black) while +128..+254 become 255 |
| GoTerrain.c Ter_vCloseModule | StaticMem_Free(pCourse) frees the 'tgd ' chunk's pData, which UStream.c:224 sets past the stream object's header: StaticMem_Free reads the two words before it as block start and size, so it frees the wrong range (Ter_UnloadHole frees pCourseStreamData, the right pointer); harmless only if Ter_UnloadHole ran first |
| hlaudsession.c Ses_AllocBankHdr | a path through the function reaches its end without returning a value |
| hlaudemitter.c Emi_UpdInstance / Doppler3D | the new nearest distance is stored in fDist before Doppler3D measures how much nearer the sound came since the last frame, so the change is always 0 and the doppler pitch always 1 |
| LLRTClock.c RTClock_GetDateTimeString | "AM" goes with clock hours 1 to 12 and "PM" with hours 0 and 13 to 23, so the hour after noon shows as "12:MM AM" and the hour after midnight as "12:MM PM": on the real-time events calendar's "Today:" line, and in front-end message 544's text (FE_GetDateTimeIfClockEarly, written only when the clock's year is before 2003 and its month before October) |
| urandom.c Misc_RandFuncg | takes logf of a uniform number in [0, 1), not (0, 1]: when Misc_RandFuncf draws 0 (one draw in 2^23), the pair it makes is infinite or NaN, handed to the tour simulation or Create-A-Player like any other |

Possible
--------

The code allows it, but whether it ever happens depends on the game's data or on what EA meant.

| Where | What | What settles it |
|---|---|---|
| Calendar.c IsLeapYear | 1900 counts as a leap year (it was not): DaysInMonth gives February 1900 29 days, menu message 756 answers 1 for 1900, and the weekdays of 1 January to 28 February 1900 come out one day early (1 January 1900, a Monday, as Sunday); the day numbers rely on it (CalDate_SetMDY counts 1 January 2000 as day 36526) | whether the menus or any date ever use the year 1900; intent only |
| Calendar.c CalDate_GetMDY | day 36525 (31 December 1999, CalDate_SetMDY) splits into day 0 of January 2000: the test for counting from 2000 is `>= 36525` where 2000's days start at 36526 | whether any stored date or the console's clock (which starts at 2000) can be 31 December 1999 |
| GoTerrain.c Ter_vCloseModule path | whether close is ever reached with a course still loaded (then the wrong free listed under Confirmed bites) | the shutdown path from the front end / reset |
| GoTerrain.c Ter_LODLoadCallback | adds the remainder of the step where rounding up would add (step - remainder): a value not a multiple stays one | the real 'tLOD' values |
| GoTerrain.c Ter_vCloseModule / Ter_UnRegisterStreamClients | xpGrassPatchList never freed; 'tLOD' left registered | whether close / unregister ever runs outside shutdown |
| GoTerrainCollision.c Ter_FreeDropNetworkLoadCallback / Ter_OOBNetworkLoadCallback | store with no MAX_FREE_DROP_NETWORKS / MAX_OOB_NETWORKS check (TW07's are 0x80 bytes, ours 0x14) | the largest outline count per hole in the course files |
| Ball.c Physics_GetFeetAngle_ForwardBackward | a flat normal ( | y |
| GoDynamicCam.c:914 DynamicCam_TrackBallVelocityLag | tests `f98 < f154` but eases by (f154 - fCamTime) / f154 squared: grows again if fCamTime passes f154 while f98 is under it | whether f98 and fCamTime can drift apart (f98 reset alone at gocamscripts.c:1949) |
| GameModeDriverRTE.c GameModeDriverRTE_EndGame | calls gRTEChallengeEndGame unchecked; NULL (and aChallenge[-1] at line 266) only for an event with bOff or nChallenge 0 | the disc's 'RTEc' data; whether the calendar can start such an event |
| FE_CalendarPopups.c PGATourPopup_GetRow_EventUpcoming | the champion's score shows only above 0, else "N/A"; a tournament played on the tour keeps its winner's score to par (GameModeDriverPGATour_AdvanceEvent), so a winner at or under par shows "N/A" | the 'PGAc' data's starting champion scores (strokes or to par); the popup of a tournament a season after it was played |
| GameModeDriverPGATour.c GetCourses | aTourEvent[nTourEvent - 1] with nTourEvent 0 (other functions test for 0) | the 'PGAc' tournament data |
| PlayNowMode.c PlayNow_LoadBallSpot | gPlayNowBallSpots[-1] for a type-10 object with challenge 0 | the course files' 'Cact' type-10 objects |
| hlaudemitter.c Emi_UpdInstance | hands the controllers on (PreprocessControllers) only when track 0, 1 or 2's changed (`uChanged & 7`): a new controller value for track 3-7 alone waits until one of those changes too | whether any sound has a streamed track past its third, or EA meant all eight bits |
| Earnings.c last hole | hole goals checked twice (PayHoledGoals, then PayRoundGoals): a money goal (nAward 39) with bEachHole would pay twice | the 'ERN ' table: any enabled bEachHole money goal |
| GameMode11.c Lessons_GetShape / GetClub / GetShotKind | gLessons[gLessonNum - 1] at lesson 0 or 12; no calling path found | whether Shot_Prepare can run during the closing line at lesson 12 |
| GameMode11.c Lessons_StartGamePreData | overrides the golfer the menu chose (Session_SetGolfer(1, 0)); both sides deliberate | intent only |
| GameMode_SkillZoneBase.c GameModeSkillZoneBase_ScaleTargetPoints | reads past its 13 / 15 / 15-entry tables if a hole has more targets | target counts per hole in the target course's data |
| GameMode8.c SpeedGolf_UpdatePlayers | mode 8's run pace never uses the fast idle decay (nCB8 counts only in mode 7) | intent only |
| GameEffects.c GameEffects_TargetGameBreakerTrigger | tests mode 16 twice; if 13 was meant, mode 13 never gets the scripted GameBreaker | intent only (TW07 has names, no bodies) |
| GameMessages.c GameMsg_SendPending | bit 1 sends the player from 0 (GUI_StartPostShotUI from 1) | the disc's UI script for message 5 type 15 |
| GameMessages.c gGameMsgPendingValue | one value shared by three pending messages | a frame setting two of bits 1 / 0x10 / 0x20; message 0x62's use in the UI |
| GameMode_SkillZoneTarget.c / GameMode_SkillZoneTargetToTarget.c | the all-targets prize pays again on every later hit (mode 13 pays once) | intent only |
| GameMode_SkillZoneTargetToTarget.c HoleFinished | checks only player 0's targets | whether the front end can start mode 17 with 2+ players |
| GameMode6.c | no first-hole tips, no restart hook, stroke limit on (modes 7 / 8 differ) | intent only |
| GameUICommands.c GM_vGetOption / GM_vSetOption | option numbers differ past 4; set 5 stores the music level without the volume | the disc's UI script (which numbers the in-round menu sends) |
| PGATourSimulation.c SimTournamentWinner / CheckEndOfTournamentAward | bFirst is set before the playoff winner: a player tied first who loses the playoff may still get nWinStreak++ / nMajorWins++ | trace a real playoff (AdvancePlayer moves entrant 0 back to the playoff hole first) |
| PGATourSimulation.c CalculateCutRow | with no row below 70th it returns the last row, so one entrant is cut anyway | needs a 30-way tie at the cut; intent only |
| FE_CrAPMessages.c GM_vGetNumCrAPSaleItemsOwned / GM_vIsCrAPItemOnSale | empty sale slots stay -1 (fn_80077C1C): owned bit (u32)-1 read ~512 MB past aB1CC; "on sale" for a choice that does not exist | the disc's 'CR_A' data: any sale category with fewer than 5 eligible assets |
| FE_CrAPMessages.c GM_vIsCrAPAnimInGolferLib | reads pB4->pChar->pLib with no NULL test on pChar | whether the menu golfer can be unloaded when the message comes |
| FE_CrAPMessages.c GM_vIsCrAPItemEquipped | 16-byte variant-name buffer where the same call elsewhere uses 64 | the longest variant name in the data |
| GameModeFourBall.c EndGame | adds nMoney to money.n14 where GameModeMatch adds nPrize | intent only |
| FE_MessageTable.c GM_vFindGolferBio | no matching bio reads entry 29, one past FE_NUM_BIOS | whether the menus ask for a golfer without a bio |
| FE_MessageTable.c GM_vSetProfileSecondName | an empty string makes the space-trim loop test szName[9], which can cut the name | whether a profile name can have a space as its 10th byte |
| FE_MessageTable.c GM_vSetCustomRoundName (message 212) | copies pArgs[3] characters with no bound: 20 or more overwrite n15 and the hole list | the longest name the menus pass |
| FE_MessageTable.c GM_vSetRandomCustomRoundHole (message 228) | `% 0` with no course from its list unlocked; loops forever with only course 0 unlocked and all its holes in the round | whether course 0 is always unlocked; the state the menus call it in |
| FE_MessageTable.c GM_vTestPassword | copies pArgs[1] characters into a 0x20-byte buffer | the longest code the menus pass |
| FE_MessageTable.c GM_vQueueMovie (message 327) | proven: nothing writes FEMovie.nBio (fn_800770FC's three callers store only nKind; the state is .bss), so FE_movieFade always plays bios/bio01 | whether the menus send pArgs[0] != 0 (the disc's UI script); whether the disc has bio02 and up |
| gbacable.c Gba_*CashToMove / GM_vGbaAddCashToMove (message 624) | gGbaPortInUse is -1 from GM_vGbaStartLink until a GBA answers; the five getters and setters then index gGbaChannels[-1], which is TibExt.c's card record: Gba_SetCashToMove writes into its szFileName; link states 6 and 8 and message 623 read it | the disc's UI script: messages 593 / 594 / 623 / 624 with nothing linked |
| gbacable.c Gba_UpdateLinkState state 6 | with 0 cash on the GBA, request 0x90 leaves n6C as asked and the profile still gets += n6C | the GBA game's side of request 0x90 |
| gbacable.c Gba_UpdateLinkState state 8 | best round: a set profile value is overwritten by the GBA's 0; holes in one: the GBA's count is added again on every swap | the GBA game's data |
| Code80090940.c UI_FreeMarkedEntryPictures | clears an entry's whole u0 (kind 2 too), so a picture element cannot decode its picture again in one UI session | UI data with a picture element loaded twice in a session |
| uiArc.c UIArc_Draw | stops at the first segment whose four corners are all transparent (later visible segments skipped) | intent only |
| uiProcessInterface.c UI_GetMoneyString | -123 prints as "-,123" | whether the UI scripts pass negative money |
| uiTransform.c UITransform_HandleOp / uiLoadFile.c UI_StreamLoadTextures | no check against the stack's 8 levels / the 5 texture slots | the UI file's data |
| startUp.c slot numbering | Startup_FindNextCardWithStatus answers slot + 1, Startup_GetNextCardStatus the slot; command 0 subtracts 1, commands 7 / 15 / 17 / 18 do not: a 1-based slot passed to format is out of range | the start-up UI script |
| startUp.c command 4 | its table entry stays NULL: calling it crashes | whether the UI sends it |
| startUp.c Startup_LoadLegalPicture | copies uSize plus up to 128 bytes of padding from the stream object | whether the stream allocation is padded |
| hlaudtrackseq.c Seq_Tick (step path) | picks the event as n64 * n3 + n66, ignoring the variation set n68 that SetVarCmdBounds uses | a stepped template with n8 > 1 in the bank data |
| AudLock.c AudLock_LockReadQueue | skips the take when the semaphore is held, but the caller's unlock still signals: once both give it back the count is 2 and the queue has no exclusion from then on | Stm_Tick and the reader thread (Stm_ReadDoneCB) both hold the AudLock mutex; whether a DMA-done callback (Stm_BlockDmaDoneCB, Stm_SilenceDmaDoneCB) comes in while one of them is in the queue |
| UAudMemStack.c AudMemStack_FreeTop | lowers the top by the freed block's size: freeing out of order corrupts the stack (Ses_Init frees bank 1 then 0) | the load order above the banks |
| SitDevFile.c SitDev_LoadScripts | allocates pD4 on every 'sscr' chunk, freed once at round end | whether a round loads more than one 'sscr' chunk |
| SitDevTrigger.c SitDev_InvokeMultipleActions | bPlayed is the last action's: an earlier line that played can still clear gSitDevPredictionVoiced | intent only |
| SitDev.c / SitDevCommentaryZones.c | the event queue and the zone list (10 slots each) have no bounds check | the scripts' data |
| SitDevTrigger.c SitDev_InvokeCommentaryBank | `% nLeft` with an empty list divides by zero | the commentary banks' data |
| GoGolfCam.c GolfCamera_ChooseReactionCam | tests ball.nSurface >= 0 but reads gSurfaceTypes[ballBefore.nSurface] | whether ballBefore.nSurface can be negative while ball.nSurface is not |
| GoDynamicCam.c ChooseSequence / ChoosePreFlightSequence | `uRand % (int)(1000 * fTotal)` divides by zero if the weights add up to under 0.001 | the camera data |
| GoStaticCam.c Parse*CameraActor | no check against the 10 static / 30 fly-by slots | the course camera data |
| CamUtils.c CamUtils_vCalcArcPosition, CAMERA_ARC_EITHER_WAY | only an end angle more than half a turn above the start's is brought round; one more than half a turn below it is left (a start at 72 degrees and an end at -126, or a start with x 0 on the -z side, which is given 270 degrees), so the camera swings the long way round | whether CamScript_PickArcDirection's "either way" was meant as the short way |
| CamUtils.c CamScript_fEvaluateCurve | when no key's start.x..end.x holds the time, the search ends at nKeys and the curve is read from the key after the last (past aKeys when nKeys is 20); nKeys 0 reads aKeys[-1] at time 1 | the course files' 'CAMC' keys: whether they always cover 0..1 without gaps |
| LLVideo.c LLVideo_PreloadQueue / ReadNextFile / QueueAdd | ignores the stream's end (a movie under 16 chunks loops forever); apChunk[32] vs nMore up to 255; no full-queue check (1024) | the movie data |
| GoLightFogEnv.c LF_vSetLightingEnvironment | lights 0-2 step over the directional description, but light 3 always takes the fourth description, even when that is the directional one | whether any hole puts its directional light in slot 3 of TGD_LightingData |
| TibExt.c TibExtMemAlloc / TibExtMemFree | the shared file library (ChecksumCRC32.c, TagFile.c, SharedFileIO.c) calls them with an allocator pointer, and TibExtMemAlloc also with its __FILE__ and __LINE__ (5 arguments); TibExt.c defines them with a u32 heap id, TibExtMemAlloc with 3 arguments, as EASB.c and EASBStorage.c call them. On the GameCube the extra arguments are ignored and the pointer and the u32 travel in the same register, so nothing goes wrong | nothing in play; a build with another calling convention or 64-bit pointers (Common/TibExtMem.h is the library's side) |
| URes.c Resources_GetDataInBuffer | with no chunk of that tag and id it returns a pointer past the buffer, not NULL (Resources_GetChunkInBuffer returns NULL); UKernel.c Kernel_DownloadActors reads the 'tACT' chunk through it untested | whether every 'Cact' object in the course files has its 'tACT' chunk |
| DynChain.c fn_80114540, fn_80114A84 | each turns a chain bone with Quat_Multiply(q0, qTurn, q0), output over the first input (fn_80114540 the chain's first bone, fn_80114A84 each bone of the chain): MathQuat.c Quat_Multiply writes w, x, y, z in turn while it still reads Q1, so x, y and z are built from the new w (y also from the new x, z from the new x and y), and the bone's new rotation is not q0 x qTurn (nor unit length) | whether any character model has a bone chain (DynChain.c) that reaches this turn |

Checked and not bugs
--------------------

AnimStream_AssignSlots / StartRead (streaming is never on: AnimStream_Init clears bOn);
HwsBurn_CopySetOptions round-up (no effect on behaviour); GM_ShowPostShotAnimation's NULL test (pSurf
cannot be NULL there); GameMode.c raw 0x1EC / 0x1F0 hook calls (EA form vs fake, no behaviour);
PlayNow_GameFinished bCheck (only caller passes 0); Earnings gpSaveData by player number
(Player.nIndex always equals the slot); SetupBonusBall case 4 (never set); speed golf state 24 (only
redundant tests); event.c PracticeSwing 21 / PrevClub 13 / TopOfArc 13 (no visible effect);
GameModeSkillZoneTarget_SetupNextGolfer re-aim (dead call); tips 11 and 13 (never shown, nothing
breaks); GameModeSkillZoneHorse_HoleFinished u8 cast; gSpeedGolfLogCycle
(overwritten before read).
