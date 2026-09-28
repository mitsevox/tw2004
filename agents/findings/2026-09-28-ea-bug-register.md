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
| EventInfo.c FE_GetDateTimeIfClockEarly | `nYear < 2003 && nMonth < 10` is no date cutoff (Oct-Dec of an earlier year is not "early") | 22cb76c (ro6) |
| GameAnalysis.c GameAnalysis_GetTipStat | fairway percentage divides by all holes, par 3s too | 22cb76c (ro6) |
| PGATourSimulation.c GM_PgaTourSim_CheckEndOfTournamentAward | the major-win count skips the bUser test: a major won by the pro in slot 0 of a skipped tournament counts as the player's (award 36) | round 15 (ro2) |
| PGATourSimulation.c GM_PgaTourSim_DistributeWinnings | no new place starts after row 70: entrants below it who made the cut share the last paid place's prize | round 15 (ro3) |
| PGATourSimulation.c CalcParBreakers | nBirdies already includes eagles and the formula adds nEagles again | round 15 (ro3) |
| FE_MessageTable.c GM_vMCLoadUser (menu message 35) | answers 1 or an error, never 0: the slot is backed up and marked loaded when the load failed | round 15 (ro5) |

## Open: behaviour proven possible, needs data or intent to settle
| Where | What | What settles it |
|---|---|---|
| GameModeDriverRTE.c GameModeDriverRTE_EndGame | calls gRTEChallengeEndGame unchecked; NULL (and aChallenge[-1] at line 266) only for an event with bOff or nChallenge 0 | the disc's 'RTEc' data; whether the calendar can start such an event |
| GameModeDriverPGATour.c GetCourses | aTourEvent[nTourEvent - 1] with nTourEvent 0 (other functions test for 0) | the 'PGAc' tournament data |
| GameMode5.c PlayNow_LoadBallSpot | gPlayNowBallSpots[-1] for a type-10 object with challenge 0 | the course files' 'Cact' type-10 objects |
| Earnings.c last hole | hole goals checked twice (PayHoledGoals, then PayRoundGoals): a money goal (nAward 39) with bEachHole would pay twice | the 'ERN ' table: any enabled bEachHole money goal |
| GameMode11.c Lessons_GetShape / GetClub / GetShotKind | gLessons[gLessonNum - 1] at lesson 0 or 12; no calling path found | whether Shot_Prepare can run during the closing line at lesson 12 |
| GameMode11.c Lessons_StartGamePreData | overrides the golfer the menu chose (Session_SetGolfer(1, 0)); both sides deliberate | intent only |
| GameTargets.c GameModeSkillZoneBase_ScaleTargetPoints | reads past its 13 / 15 / 15-entry tables if a hole has more targets | target counts per hole in the target course's data |
| GameMode8.c SpeedGolf_UpdatePlayers | mode 8's run pace never uses the fast idle decay (nCB8 counts only in mode 7) | intent only |
| GameEffects.c GameEffects_TargetGameBreakerTrigger | tests mode 16 twice; if 13 was meant, mode 13 never gets the scripted GameBreaker | intent only (TW07 has names, no bodies) |
| GameMessages.c GameMsg_SendPending | bit 1 sends the player from 0 (GUI_StartPostShotUI from 1) | the disc's UI script for message 5 type 15 |
| GameMessages.c gGameMsgPendingValue | one value shared by three pending messages | a frame setting two of bits 1 / 0x10 / 0x20; message 0x62's use in the UI |
| GameMode16.c / GameMode17.c | the all-targets prize pays again on every later hit (mode 13 pays once) | intent only |
| GameMode17.c HoleFinished | checks only player 0's targets | whether the front end can start mode 17 with 2+ players |
| GameMode6.c | no first-hole tips, no restart hook, stroke limit on (modes 7 / 8 differ) | intent only |
| GameUICommands.c GM_vGetOption / GM_vSetOption | option numbers differ past 4; set 5 stores the music level without the volume | the disc's UI script (which numbers the in-round menu sends) |
| PGATourSimulation.c SimTournamentWinner / CheckEndOfTournamentAward | bFirst is set before the playoff winner: a player tied first who loses the playoff may still get nWinStreak++ / nMajorWins++ | trace a real playoff (AdvancePlayer moves entrant 0 back to the playoff hole first) |
| PGATourSimulation.c CalculateCutRow | with no row below 70th it returns the last row, so one entrant is cut anyway | needs a 30-way tie at the cut; intent only |
| FE_CrAPMessages.c GM_vGetNumCrAPSaleItemsOwned / GM_vIsCrAPItemOnSale | empty sale slots stay -1 (fn_80077C1C): owned bit (u32)-1 read ~512 MB past aB1CC; "on sale" for a choice that does not exist | the disc's 'CR_A' data: any sale category with fewer than 5 eligible assets |
| FE_CrAPMessages.c GM_vIsCrAPAnimInGolferLib | reads pB4->pChar->pLib with no NULL test on pChar | whether the menu golfer can be unloaded when the message comes |
| FE_CrAPMessages.c GM_vIsCrAPItemEquipped | 16-byte variant-name buffer where the same call elsewhere uses 64 | the longest variant name in the data |
| GameModeFourBall.c EndGame | adds nMoney to money.n14 where GameModeMatch adds nPrize | intent only |

## Dropped (checked: harmless or not a bug)
AnimStream_AssignSlots / StartRead (streaming is never on: AnimStream_Init clears bOn);
HwsBurn_CopySetOptions round-up (a fake-match question); GM_ShowPostShotAnimation's NULL test (pSurf
cannot be NULL there); GameManager.c raw 0x1EC / 0x1F0 hook calls (EA form vs fake, no behaviour);
PlayNow_GameFinished bCheck (only caller passes 0); Earnings gpSaveData by player number
(Player.nIndex always equals the slot); SetupBonusBall case 4 (never set); speed golf state 24 (only
redundant tests); event.c PracticeSwing 21 / PrevClub 13 / TopOfArc 13 (no visible effect);
GameModeSkillZoneTarget_SetupNextGolfer re-aim (dead call); tips 11 and 13 (never shown, nothing
breaks); GameModeSkillZoneHorse_HoleFinished u8 cast (a `port:` candidate); gSpeedGolfLogCycle
(overwritten before read). Existing labels that may be harmless for the same nIndex reason:
Earnings.c:1256, GameMode4.c:365 (review when those files are next touched).
