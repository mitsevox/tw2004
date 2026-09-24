// SwingTips.c (our name): the tips shown as a swing starts (fn_800D1DAC, from
// STATEFUNC_SwingInit). Each test (wind, lie, slope, the golfer's attributes, par and score) picks
// a tip; the first time a save profile meets one it gets the full tip (and a flag in the save),
// later a short random one (the flags are SaveProfile.aTipSeen).

#include "game.h"
#include "game/save.h"

u8 fn_800D1698(int nPlayer);
u8 fn_800D16F0(int nPlayer);
u8 fn_800D17E4(int nPlayer);
u8 fn_800D18D8(int nPlayer);
u8 fn_800D19F8(int nPlayer);
u8 fn_800D1A34(int nPlayer);
u8 fn_800D1A70(int nPlayer);
u8 fn_800D1AA8(int nPlayer);
u8 fn_800D1AE0(void);
u8 fn_800D1B10(int nPlayer);
u8 fn_800D1BA4(int nPlayer);
u8 fn_800D1C38(int nPlayer);
u8 fn_800D1C9C(int nPlayer);
u8 fn_800D1D30(void);
u8 fn_800D1D38(int nPlayer);
void fn_800E4FB0(u8 nKind, int nTip);   // GameUI.c: show a tip (1 full, 2 short)

// A tip test: a shot other than a putt, with the wind's speed over 6.
u8 fn_800D1698(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == 0) return 0;
    return Wind_Get(NULL) > 6.0f;
}

// A tip test: a shot other than a putt, in a wind over 6 blowing from 135 to 225 degrees off the
// aim (the angle measured like the aim's, from +z).
u8 fn_800D16F0(int nPlayer) {
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

// A tip test: as fn_800D16F0, with the wind from -45 to 45 degrees off the aim.
u8 fn_800D17E4(int nPlayer) {
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

// A tip test: a poor lie. The lie's quality comes from the surface under the ball, the ball's
// f70 and the golfer's recovery; under 75 fires.
u8 fn_800D18D8(int nPlayer) {
    f32 fQuality;
    f32 fSpread;

    fQuality = 100.0f * (gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].f00 +
                         0.01f * (gPlayers[nPlayer].ball.f70 *
                                  (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL)));
    // the surface's random lie range (f04), narrowed by recovery
    fSpread = gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].f04 *
              (100.0f - (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_RECOVERY, ATTR_TOTAL));
    return fQuality + fSpread < 75.0f;
}

// A tip test: the ball lies in lie 3, 4 or 5.
u8 fn_800D19F8(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 3 || gPlayers[nPlayer].ball.nLie == 4 ||
        gPlayers[nPlayer].ball.nLie == 5) {
        return 1;
    }
    return 0;
}

// A tip test: the ball lies in lie 6, 7 or 8.
u8 fn_800D1A34(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
        gPlayers[nPlayer].ball.nLie == 8) {
        return 1;
    }
    return 0;
}

// A tip test: the target is more than 17 feet below the ball.
u8 fn_800D1A70(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) < -17.0f;
}

// A tip test: the target is more than 17 feet above the ball.
u8 fn_800D1AA8(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) > 17.0f;
}

// A tip test: lbl_802811F0's flag 0x2 is set.
u8 fn_800D1AE0(void) {
    return fn_80035574() != 0;
}

// A tip test: the player's first shot on a par 5 of 500 or more (the hole's value for the tee).
u8 fn_800D1B10(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = fn_800D2B08();
    s32 nLength = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 5 && nStrokes == 0 && nLength >= 500) {
        return 1;
    }
    return 0;
}

// A tip test: the player's first shot on a par 4 of 325 or less.
u8 fn_800D1BA4(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = fn_800D2B08();
    s32 nLength = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 4 && nStrokes == 0 && nLength <= 325) {
        return 1;
    }
    return 0;
}

// A tip test: the club reaches past the pin (the longest the player can hit it is more than the
// ball's distance from the pin).
u8 fn_800D1C38(int nPlayer) {
    f32 fMax = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
    return fMax > fn_800D0478(nPlayer);
}

// A tip test: the player's first shot on a par 4 of 425 or more.
u8 fn_800D1C9C(int nPlayer) {
    s32 nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    int nPar = fn_800D2B08();
    s32 nLength = fn_800D2C68(gSession.nTeeSet[nPlayer]);
    if (nPar == 4 && nStrokes == 0 && nLength >= 425) {
        return 1;
    }
    return 0;
}

// A tip test that never fires.
u8 fn_800D1D30(void) {
    return 0;
}

// A tip test: a target 25 to 33 away, no more than 3 feet above or below the ball.
u8 fn_800D1D38(int nPlayer) {
    f32 fRise = 3.0f * (gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].vBall[1]);
    if (gPlayers[nPlayer].fDistance >= 25.0f && gPlayers[nPlayer].fDistance <= 33.0f && fRise >= -3.0f &&
        fRise <= 3.0f) {
        return 1;
    }
    return 0;
}

// Which of a tip's nCount short versions to show. EA wrote the count as a parameter: with one
// version (% 1) the compiler still divides, where a literal % 1 folds away.
static inline u32 SwingTips_Pick(u32 nCount) {
    return Rand_Next(0) % nCount;
}

// The tips as a swing starts, when the tips option is on and the player has a save profile (and
// no controller in use is unplugged). Each test that passes shows its tip: the full one the
// first time for this profile (unless b522F is set), else a random short one.
void fn_800D1DAC(int nPlayer) {
    u8 bFull;
    u8 bUnplugged;
    int nChan;
    int i;
    int nProfile;

    bUnplugged = 0;
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
    bFull = 0;
    if (gpSaveData[nProfile].bActive != 1) {
        return;
    }
    for (nChan = 0; nChan < 4; nChan++) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (!fn_80013070(nChan) && nChan == gSession.nController[i]) {
                bUnplugged = 1;
            }
        }
    }
    if (bUnplugged) {
        return;
    }
    if (fn_800D1698(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[0]) {
            fn_800E4FB0(1, 0);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[0] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 0);
        }
    }
    if (fn_800D16F0(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[1]) {
            fn_800E4FB0(1, 2);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[1] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(4) + 2);
        }
    }
    if (fn_800D17E4(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[2]) {
            fn_800E4FB0(1, 6);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[2] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 6);
        }
    }
    if (fn_800D18D8(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[3]) {
            fn_800E4FB0(1, 8);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[3] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(3) + 8);
        }
    }
    if (fn_800D19F8(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[4]) {
            fn_800E4FB0(1, 11);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[4] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 11);
        }
    }
    if (fn_800D1A34(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[5]) {
            fn_800E4FB0(1, 13);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[5] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 13);
        }
    }
    if (fn_800D1A70(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[6]) {
            fn_800E4FB0(1, 15);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[6] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 15);
        }
    }
    if (fn_800D1AA8(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[7]) {
            fn_800E4FB0(1, 17);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[7] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 17);
        }
    }
    if (((u8 (*)(int))fn_800D1AE0)(nPlayer)) {   // port: EA passes an argument fn_800D1AE0 ignores
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[8]) {
            fn_800E4FB0(1, 19);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[8] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(3) + 19);
        }
    }
    if (fn_800D1B10(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[9]) {
            fn_800E4FB0(1, 22);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[9] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(3) + 22);
        }
    }
    if (fn_800D1BA4(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[10]) {
            fn_800E4FB0(1, 25);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[10] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(1) + 25);
        }
    }
    if (fn_800D1C38(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[11]) {
            fn_800E4FB0(1, 26);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[11] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 26);
        }
    }
    if (fn_800D1C9C(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[12]) {
            fn_800E4FB0(1, 28);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[12] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(1) + 28);
        }
    }
    if (((u8 (*)(int))fn_800D1D30)(nPlayer)) {   // port: EA passes an argument fn_800D1D30 ignores
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[13]) {
            fn_800E4FB0(1, 29);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[13] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(1) + 29);
        }
    }
    if (fn_800D1D38(nPlayer)) {
        if (!gpSaveData[nProfile].b522F && !gpSaveData[nProfile].aTipSeen[14]) {
            fn_800E4FB0(1, 30);
            bFull = 1;
            gpSaveData[nProfile].aTipSeen[14] = 1;
        } else {
            fn_800E4FB0(2, SwingTips_Pick(2) + 30);
        }
    }
    if (bFull) {
        fn_800E3D38(nPlayer, 0);
        fn_80062C80(gPlayers[nPlayer].nC58, 0);
    }
}
