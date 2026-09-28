// CaddieTips.c (EA's name: TW07's CaddieTips.c, whose CTIP_ functions are these, in the same
// order): the caddie tips shown as a swing starts (CTIP_ShowCaddieTip, from STATEFUNC_SwingInit).
// Each trigger (the wind, the lie, the slope to the target, the weather, the hole's par and length
// on the tee shot, the club's reach, a short pitch) picks a tip; the first time a save profile
// meets one it gets the full tip (and a flag in the save, SaveProfile.aTipSeen), later a random
// short version.

#include "game.h"
#include "game/save.h"

u8 CTIP_CheckGeneralWindTrigger(int nPlayer);
u8 CTIP_CheckIntoWindTrigger(int nPlayer);
u8 CTIP_CheckWithWindTrigger(int nPlayer);
u8 CTIP_CheckPenaltyLieTrigger(int nPlayer);
u8 CTIP_CheckRoughLieTrigger(int nPlayer);
u8 CTIP_CheckSandLieTrigger(int nPlayer);
u8 CTIP_CheckDownhillLieTrigger(int nPlayer);
u8 CTIP_CheckUphillLieTrigger(int nPlayer);
u8 CTIP_CheckWeatherTrigger(void);
u8 CTIP_LongDistanceTeeShotTrigger(int nPlayer);
u8 CTIP_MediumDistanceTeeShotTrigger(int nPlayer);
u8 CTIP_SpinnaShotTrigger(int nPlayer);
u8 CTIP_TeeSpinnaShotTrigger(int nPlayer);
u8 CTIP_CheckDisabledTrigger(void);
u8 CTIP_FlopShotTrigger(int nPlayer);
void GUI_ShowSwingTip(u8 nKind, int nTip);   // GameUI.c: show a tip (1 full, 2 short)

// The general wind tip's test (tip 0, two short versions): true for any shot but a putt when the
// wind's speed (Wind_Get) is over 6.
u8 CTIP_CheckGeneralWindTrigger(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == 0) return 0;
    return Wind_Get(NULL) > 6.0f;
}

// The into-the-wind tip's test (tip 2, four short versions): a shot other than a putt, in a wind
// over 6 whose vector (Wind_Get) lies 135 to 225 degrees off the aim (the angle measured like the
// aim's, from +z).
u8 CTIP_CheckIntoWindTrigger(int nPlayer) {
    f32 fAim;
    f32 fAngle;
    f32 vWind[4];

    if (gPlayers[nPlayer].nShotKind == 0) return 0;
    if (Wind_Get(NULL) > 6.0f) {
        fAim = gPlayers[nPlayer].fAim;
        Wind_Get(vWind);
        fAngle = (f32)atan2(-vWind[0], vWind[2]) - fAim;
        while (fAngle < 0.0f) {
            fAngle += TWOPI;
        }
        while (fAngle > TWOPI) {
            fAngle -= TWOPI;
        }
        if (fAngle > PI * 0.75f && fAngle < PI * 1.25f) {
            return 1;
        }
        return 0;
    }
    return 0;
}

// The downwind tip's test (tip 6, two short versions): as CTIP_CheckIntoWindTrigger, with the
// wind's vector -45 to 45 degrees off the aim (in effect 0 to 45: see the EA bug below).
u8 CTIP_CheckWithWindTrigger(int nPlayer) {
    f32 fAim;
    f32 fAngle;
    f32 vWind[4];

    if (gPlayers[nPlayer].nShotKind == 0) return 0;
    if (Wind_Get(NULL) > 6.0f) {
        fAim = gPlayers[nPlayer].fAim;
        Wind_Get(vWind);
        fAngle = (f32)atan2(-vWind[0], vWind[2]) - fAim;
        while (fAngle < 0.0f) {
            fAngle += TWOPI;
        }
        while (fAngle > TWOPI) {
            fAngle -= TWOPI;
        }
        // EA bug: the angle was wrapped to 0..2pi, so only 0 to 45 degrees count, not -45 to 0
        if (fAngle > -PI / 4.0f && fAngle < PI / 4.0f) {
            return 1;
        }
        return 0;
    }
    return 0;
}

// The penalty lie tip's test (tip 8, three short versions): the lie keeps too little of the shot's
// power. The share the surface under the ball keeps (SurfaceType.f00, plus the ball's f70 times the
// golfer's recovery / 100), in percent, plus the lie's random spread (SurfaceType.f04 times 100
// less the recovery) is under 75.
u8 CTIP_CheckPenaltyLieTrigger(int nPlayer) {
    f32 fQuality;
    f32 fSpread;

    fQuality = 100.0f * (gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].f00 +
                         0.01f * (gPlayers[nPlayer].ball.f70 *
                                  (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL)));
    // the surface's f04, times 100 less the recovery
    fSpread = gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].f04 *
              (100.0f - (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL));
    return fQuality + fSpread < 75.0f;
}

// The rough lie tip's test (tip 11, two short versions): the ball lies in the rough (lie 3, 4 or 5:
// LIE_ROUGH_HIGH_e to LIE_THICK_ROUGH_e).
u8 CTIP_CheckRoughLieTrigger(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 3 || gPlayers[nPlayer].ball.nLie == 4 ||
        gPlayers[nPlayer].ball.nLie == 5) {
        return 1;
    }
    return 0;
}

// The sand lie tip's test (tip 13, two short versions): the ball lies in sand (lie 6, 7 or 8:
// LIE_SAND_HIGH_e to LIE_SAND_DEEP_e).
u8 CTIP_CheckSandLieTrigger(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
        gPlayers[nPlayer].ball.nLie == 8) {
        return 1;
    }
    return 0;
}

// The downhill tip's test (tip 15, two short versions): the target is more than 17 feet below the
// ball (world units are yards).
u8 CTIP_CheckDownhillLieTrigger(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) < -17.0f;
}

// The uphill tip's test (tip 17, two short versions): the target is more than 17 feet above the
// ball.
u8 CTIP_CheckUphillLieTrigger(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) > 17.0f;
}

// The weather tip's test (tip 19, three short versions): the hole's weather has flag 0x2
// (lbl_802811F0; fn_8006F650 rolls it per course, and fn_8006FB10 then starts particle effects 0 to
// 2 at a strength and sets the turf speed). Takes no player, though CTIP_ShowCaddieTip passes one.
u8 CTIP_CheckWeatherTrigger(void) {
    return fn_80035574() != 0;
}

// The long tee shot tip's test (tip 22, three short versions): the player's first shot on a par 5
// of 500 yards or more (the hole's length from the player's tees).
u8 CTIP_LongDistanceTeeShotTrigger(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = Course_GetCurHolePar();
    s32 nHoleYardage = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 5 && nStrokes == 0 && nHoleYardage >= 500) {
        return 1;
    }
    return 0;
}

// The medium tee shot tip's test (tip 25, one short version): the player's first shot on a par 4 of
// 325 yards or less.
u8 CTIP_MediumDistanceTeeShotTrigger(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = Course_GetCurHolePar();
    s32 nHoleYardage = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 4 && nStrokes == 0 && nHoleYardage <= 325) {
        return 1;
    }
    return 0;
}

// The spin shot tip's test (tip 26, two short versions): the club can hit the ball farther
// (AI_MaxDistance for the shot and club) than the ball lies from the pin.
u8 CTIP_SpinnaShotTrigger(int nPlayer) {
    f32 fClubDistance = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
    return fClubDistance > fn_800D0478(nPlayer);
}

// The tee spin shot tip's test (tip 28, one short version): the player's first shot on a par 4 of
// 425 yards or more.
u8 CTIP_TeeSpinnaShotTrigger(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = Course_GetCurHolePar();
    s32 nHoleYardage = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 4 && nStrokes == 0 && nHoleYardage >= 425) {
        return 1;
    }
    return 0;
}

// Tip 29's test: always false in this build, so tip 29 is never shown. Takes no player, though
// CTIP_ShowCaddieTip passes one.
u8 CTIP_CheckDisabledTrigger(void) {
    return 0;
}

// The flop shot tip's test (tip 30, two short versions): the target is 25 to 33 yards away
// (fDistance) and no more than 3 feet above or below the ball.
u8 CTIP_FlopShotTrigger(int nPlayer) {
    f32 fTargetHeight = 3.0f * (gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].vBall[1]);
    if (gPlayers[nPlayer].fDistance >= 25.0f && gPlayers[nPlayer].fDistance <= 33.0f &&
        fTargetHeight >= -3.0f && fTargetHeight <= 3.0f) {
        return 1;
    }
    return 0;
}

// Which of a tip's nCount short versions to show. EA wrote the count as a parameter: with one
// version (% 1) the compiler still divides, where a literal % 1 folds away.
static inline u32 SwingTips_Pick(u32 nCount) {
    return Misc_RandFunc(0) % nCount;
}

// The caddie tips as a swing starts (STATEFUNC_SwingInit): only when the tips option is on (options
// a24[0]), session flag 0x4000 is clear, the shot is not a putt, the player's save profile is
// active and every controller in use is plugged in. Each test that passes shows its tip: the full
// tip the first time for this profile (unless bCaddieTipsOff is set; the profile's aTipSeen flag is
// then set), else a random one of its short versions. After a full tip the round's UI is hidden
// (GUI_ToggleUI) and fn_80062C80 is called with the player's nUISlot.
void CTIP_ShowCaddieTip(int nPlayer) {
    u8 bTipShown;
    u8 bControllerIsPulled;
    int nChan;
    int i;
    int nProfile;

    bControllerIsPulled = 0;
    if (gSession.uFlags & 0x4000) {
        return;
    }
    if (!gSession.options.a24[0]) {
        return;
    }
    if (gPlayers[nPlayer].nShotKind == 0) {
        return;
    }
    nProfile = gPlayers[nPlayer].nIndex;
    bTipShown = 0;
    if (gpSaveData[nProfile].bActive != 1) {
        return;
    }
    for (nChan = 0; nChan < 4; nChan++) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (!Input_bDoesPadExist(nChan) && nChan == gSession.nController[i]) {
                bControllerIsPulled = 1;
            }
        }
    }
    if (bControllerIsPulled) {
        return;
    }
    if (CTIP_CheckGeneralWindTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[0]) {
            GUI_ShowSwingTip(1, 0);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[0] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 0);
        }
    }
    if (CTIP_CheckIntoWindTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[1]) {
            GUI_ShowSwingTip(1, 2);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[1] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(4) + 2);
        }
    }
    if (CTIP_CheckWithWindTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[2]) {
            GUI_ShowSwingTip(1, 6);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[2] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 6);
        }
    }
    if (CTIP_CheckPenaltyLieTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[3]) {
            GUI_ShowSwingTip(1, 8);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[3] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(3) + 8);
        }
    }
    if (CTIP_CheckRoughLieTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[4]) {
            GUI_ShowSwingTip(1, 11);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[4] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 11);
        }
    }
    if (CTIP_CheckSandLieTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[5]) {
            GUI_ShowSwingTip(1, 13);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[5] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 13);
        }
    }
    if (CTIP_CheckDownhillLieTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[6]) {
            GUI_ShowSwingTip(1, 15);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[6] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 15);
        }
    }
    if (CTIP_CheckUphillLieTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[7]) {
            GUI_ShowSwingTip(1, 17);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[7] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 17);
        }
    }
    // port: EA passes an argument CTIP_CheckWeatherTrigger ignores
    if (((u8 (*)(int))CTIP_CheckWeatherTrigger)(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[8]) {
            GUI_ShowSwingTip(1, 19);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[8] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(3) + 19);
        }
    }
    if (CTIP_LongDistanceTeeShotTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[9]) {
            GUI_ShowSwingTip(1, 22);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[9] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(3) + 22);
        }
    }
    if (CTIP_MediumDistanceTeeShotTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[10]) {
            GUI_ShowSwingTip(1, 25);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[10] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(1) + 25);
        }
    }
    if (CTIP_SpinnaShotTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[11]) {
            GUI_ShowSwingTip(1, 26);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[11] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 26);
        }
    }
    if (CTIP_TeeSpinnaShotTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[12]) {
            GUI_ShowSwingTip(1, 28);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[12] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(1) + 28);
        }
    }
    // port: EA passes an argument CTIP_CheckDisabledTrigger ignores
    if (((u8 (*)(int))CTIP_CheckDisabledTrigger)(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[13]) {
            GUI_ShowSwingTip(1, 29);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[13] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(1) + 29);
        }
    }
    if (CTIP_FlopShotTrigger(nPlayer)) {
        if (!gpSaveData[nProfile].bCaddieTipsOff && !gpSaveData[nProfile].aTipSeen[14]) {
            GUI_ShowSwingTip(1, 30);
            bTipShown = 1;
            gpSaveData[nProfile].aTipSeen[14] = 1;
        } else {
            GUI_ShowSwingTip(2, SwingTips_Pick(2) + 30);
        }
    }
    if (bTipShown) {
        GUI_ToggleUI(nPlayer, 0);
        fn_80062C80(gPlayers[nPlayer].nUISlot, 0);
    }
}
