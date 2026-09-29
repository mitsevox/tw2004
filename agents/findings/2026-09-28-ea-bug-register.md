# EA bug register (from 2026-09-28)

Every possible EA bug a lane reports ends here with a verdict, so none gets lost between rounds
(owner, 2026-09-28). Labelled bugs carry `// EA bug:` in the code (grep for it); this file keeps
the ones still open and why the others were dropped. naming.md rule 6: a lane settles bugs in its
own files in its pass; the rest come here.

## Labelled
| Where | What | Commit |
|---|---|---|
| GameModeStableford.c GameModeStableford_EndGame | stroke play's fewer-is-better prize test on Stableford points pays the losing side | 98ebd8d |
| GameModeDriverPGATour.c GameModeDriverPGATour_SetTournament | the player's green speed option (options.n18) is saved but never put back after a tour round | 22cb76c (ro6) |
| FE_CalendarPopups.c FE_GetDateTimeIfClockEarly | `nYear < 2003 && nMonth < 10` is no date cutoff (Oct-Dec of an earlier year is not "early") | 22cb76c (ro6) |
| GameAnalysis.c GameAnalysis_GetTipStat | fairway percentage divides by all holes, par 3s too | 22cb76c (ro6) |
| PGATourSimulation.c GM_PgaTourSim_CheckEndOfTournamentAward | the major-win count skips the bUser test: a major won by the pro in slot 0 of a skipped tournament counts as the player's (award 36) | round 15 (ro2) |
| PGATourSimulation.c GM_PgaTourSim_DistributeWinnings | no new place starts after row 70: entrants below it who made the cut share the last paid place's prize | round 15 (ro3) |
| PGATourSimulation.c CalcParBreakers | nBirdies already includes eagles and the formula adds nEagles again | round 15 (ro3) |
| FE_MessageTable.c GM_vMCLoadUser (menu message 35) | answers 1 or an error, never 0: the slot is backed up and marked loaded when the load failed | round 15 (ro5) |
| FE_MessageTable.c GM_vMaskString (menu message 399) | the terminator goes one past the stars; for 63 stars it writes szStars[64], past the buffer (64 or more: the loop does) | round 16 (rp6, label extended) |
| FE_Manager.c FE_CrAP_IsItemLocked, kind 20 | counts groups with aMedal != 0, but 0 is the best medal and 3 none: a new profile (all 3) unlocks every kind-20 item with n <= 29, and winning best medals lowers the count (message 175 tests != 3) | round 17 (rq2) |
| FE_Manager.c FE_CrAP_IsItemLocked, kind 24 | the loop `i = 23; i < 16` never runs: only n <= 0 unlocks (label made exact) | round 17 (rq2) |
| Code800B90F4.c MAD_ReadNextFile | the end-of-movie test repeats the NULL test above it, so nEnd is never set: MAD_IsAtEnd always answers 0 and LLVideo.c stops a movie only when it is starved | round 17 (rq5) |
| startUp.c Startup_CheckCards | hint 0x8B (card slot B empty) can never be sent: port 1 is only reached when port 0's status is 0, and that case tests port 0's status != 0 | round 18 (rr2) |
| hlaudtrackstm.c Stm_Exit | cancels its DMAs with the track as owner, but AudDma_ToAram's owner is always 0 or 1: the cancel never matches, so a queued block DMA writes into ARAM buffers just freed and its callback counts into the freed track | round 18 (rr4) |
| UAudContainers.c UList_InsertAt | an insert at the tail (not also the head) links after it, not in front: InsertSortWorldPerf leaves a higher-priority track at the tail, where Trk_AllocPerf steals first | round 18 (rr5) |
| gocamscripts.c CamScript_CircleCameras | fA8 (the roll) blends by fT (0..2) instead of fShare: the roll runs on to twice the difference and snaps back when the blend ends | round 19 (rs6) |
| rcmp_mad_codec.c madinit | `for (i = -256; i < 255; ...)` never writes gMadClamp[255] (.bss zero): a block value of +255 becomes pixel 0 (black) while +128..+254 become 255 | round 19 (rs2) |
| hlaudsession.c Ses_AllocBankHdr | the missing return (was labelled "fake match: EA bug") is EA's own form: now `EA bug:` + `port: return NULL` | round 19 (rs1, relabelled) |

## Open: behaviour proven possible, needs data or intent to settle
| Where | What | What settles it |
|---|---|---|
| GameModeDriverRTE.c GameModeDriverRTE_EndGame | calls gRTEChallengeEndGame unchecked; NULL (and aChallenge[-1] at line 266) only for an event with bOff or nChallenge 0 | the disc's 'RTEc' data; whether the calendar can start such an event |
| GameModeDriverPGATour.c GetCourses | aTourEvent[nTourEvent - 1] with nTourEvent 0 (other functions test for 0) | the 'PGAc' tournament data |
| PlayNowMode.c PlayNow_LoadBallSpot | gPlayNowBallSpots[-1] for a type-10 object with challenge 0 | the course files' 'Cact' type-10 objects |
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
| FE_MessageTable.c GM_vQueueMovie (message 327) | proven: nothing writes FEMovie.nBio (fn_800770FC's three callers store only nKind; the state is .bss), so FE_movieFade always plays bios/bio01 (rq4) | whether the menus send pArgs[0] != 0 (the disc's UI script); whether the disc has bio02 and up |
| gbacable.c Gba_*CashToMove / GM_vGbaAddCashToMove (message 624) | gGbaPortInUse is -1 from GM_vGbaStartLink until a GBA answers; the five getters and setters then index gGbaChannels[-1], which is TibExt.c's card record: Gba_SetCashToMove writes into its szFileName; link states 6 and 8 and message 623 read it (rq6) | the disc's UI script: messages 593 / 594 / 623 / 624 with nothing linked |
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
| AudLock.c AudLock_LockReadQueue | skips the take when the semaphore is held; the unlock then raises it to 2: no exclusion if two threads collide | whether the file and main threads overlap in the read queue |
| UAudMemStack.c AudMemStack_FreeTop | lowers the top by the freed block's size: freeing out of order corrupts the stack (Ses_Init frees bank 1 then 0) | the load order above the banks |
| SitDevFile.c SitDev_LoadScripts | allocates pD4 on every 'sscr' chunk, freed once at round end | whether a round loads more than one 'sscr' chunk |
| SitDevTrigger.c SitDev_InvokeMultipleActions | bPlayed is the last action's: an earlier line that played can still clear gSitDevPredictionVoiced | intent only |
| SitDev.c / SitDevCommentaryZones.c | the event queue and the zone list (10 slots each) have no bounds check | the scripts' data |
| SitDevTrigger.c SitDev_InvokeCommentaryBank | `% nLeft` with an empty list divides by zero | the commentary banks' data |
| GoGolfCam.c GolfCamera_ChooseReactionCam | tests ball.nSurface >= 0 but reads gSurfaceTypes[ballBefore.nSurface] | whether ballBefore.nSurface can be negative while ball.nSurface is not |
| GoDynamicCam.c ChooseSequence / ChoosePreFlightSequence | `uRand % (int)(1000 * fTotal)` divides by zero if the weights add up to under 0.001 | the camera data |
| GoStaticCam.c Parse*CameraActor | no check against the 10 static / 30 fly-by slots | the course camera data |
| LLVideo.c LLVideo_PreloadQueue / ReadNextFile / QueueAdd | ignores the stream's end (a movie under 16 chunks loops forever); apChunk[32] vs nMore up to 255; no full-queue check (1024) | the movie data |
| LLVideo.c (MAD_IsAtEnd always 0) | a movie ends only when starved: also mid-movie if the disc falls behind | already labelled at MAD_ReadNextFile |

## Dropped (checked: harmless or not a bug)
AnimStream_AssignSlots / StartRead (streaming is never on: AnimStream_Init clears bOn);
HwsBurn_CopySetOptions round-up (a fake-match question); GM_ShowPostShotAnimation's NULL test (pSurf
cannot be NULL there); GameMode.c raw 0x1EC / 0x1F0 hook calls (EA form vs fake, no behaviour);
PlayNow_GameFinished bCheck (only caller passes 0); Earnings gpSaveData by player number
(Player.nIndex always equals the slot); SetupBonusBall case 4 (never set); speed golf state 24 (only
redundant tests); event.c PracticeSwing 21 / PrevClub 13 / TopOfArc 13 (no visible effect);
GameModeSkillZoneTarget_SetupNextGolfer re-aim (dead call); tips 11 and 13 (never shown, nothing
breaks); GameModeSkillZoneHorse_HoleFinished u8 cast (a `port:` candidate); gSpeedGolfLogCycle
(overwritten before read). Existing labels that may be harmless for the same nIndex reason:
Earnings.c:1256, LadderedMode.c:365 (review when those files are next touched). Round 16:
GM_vIsGolferUnlocked (golfers 30..33) / GM_vIsCourseUnlocked... (course 23) read one past their
arrays as sized, but the answer is already 1 then (a header-size question, not a bug).
