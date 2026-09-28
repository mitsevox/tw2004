// GameTargets.c (our name; TW07's GameMode_SkillZoneBase.cpp, the base of its five skill-zone
// modes, which are our target games, modes 13..17): the code those modes share. The target list
// sorted by distance from the tee and aimed at (the pin and flag move to the player's target), the
// target nearest the ball or the aim point, which ring of a target a landing surface is, each
// player's per-hole and per-shot target-game state, the random shot multiplier, the all-targets
// prize, each hole's per-target points factor, the drive line, the bonus objects' index, the
// commentary, and dispatchers into each mode's own file (TW07's virtual methods).
// Split from GameModeReplay.c (mode 10) because the two halves each have their own copy of the
// int-to-float constant; the bytes cannot prove the exact split point. TW07's source order puts
// GetCupCount, GetCupPosition and AddCup right before SortCupsByDistanceFromTee, and
// GameModeReplay.c ends with three functions that do exactly that (fn_800F1960, fn_800F196C,
// fn_800F199C, the last the only user of the 1.0f at .sdata2 0x80284698, which could as well open
// this file's .sdata2): the file probably starts there.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// Each target's points factor on holes 0, 1 and 2 (Game_GetCurHoleNum), by its place in the list
// sorted nearest the tee first (GameModeSkillZoneBase_ScaleTargetPoints).
f32 gSkillZoneHole0TargetScale[13] = {
    1.0f, 1.0f, 1.3f, 1.1f, 1.0f, 1.1f, 1.2f, 1.3f, 1.3f, 1.4f, 1.4f, 1.3f, 1.0f,
};
f32 gSkillZoneHole1TargetScale[15] = {
    1.0f, 1.0f, 1.2f, 1.1f, 1.3f, 1.1f, 1.4f, 1.2f, 1.1f, 1.0f, 1.0f, 1.0f, 1.2f, 1.4f, 1.3f,
};
f32 gSkillZoneHole2TargetScale[15] = {
    1.1f, 1.1f, 1.0f, 1.0f, 1.0f, 1.1f, 1.2f, 1.3f, 1.0f, 1.3f, 1.4f, 1.4f, 1.3f, 1.0f, 1.0f,
};

void  Gaud_MultiplierBonus(void);

u8   GameModeSkillZoneBase_FirstShot(int nPlayer);

// Sorts the target list (lbl_80211D38, lbl_80282360 points) by distance from player 0's tee (the
// tee of gSession.nTeeSet[0]), nearest first, by swapping pairs. Each target mode's hole start
// calls it, so target 0 is the nearest.
void GameModeSkillZoneBase_SortCupsByDistanceFromTee(void) {
    f32 tmp[4];
    int i;
    int j;
    f32* pTee = &Ter_GetTGD()->tee[gSession.nTeeSet[0]].x;
    for (i = 0; i < lbl_80282360 - 1; i++) {
        for (j = i + 1; j < lbl_80282360; j++) {
            if (LLMath_DistanceBetween3(lbl_80211D38[i], pTee)
                > LLMath_DistanceBetween3(lbl_80211D38[j], pTee)) {
                LLMath_CopyVec(lbl_80211D38[i], tmp);
                LLMath_CopyVec(lbl_80211D38[j], lbl_80211D38[i]);
                LLMath_CopyVec(tmp, lbl_80211D38[j]);
            }
        }
    }
}

// Aims player nPlayer at target n, taken modulo the target count (one past the last is target 0):
// Player.nTarget is set, the hole's pin (Ter_GetTGD()->pin) moves to the target, and so does the
// flag model (skeletal object 100) when there is one.
void GameModeSkillZoneBase_SetCup(int nPlayer, s8 n) {
    Character* pChar;
    gPlayers[nPlayer].nTarget = n % lbl_80282360;
    LLMath_CopyVec(lbl_80211D38[gPlayers[nPlayer].nTarget], (f32*)Ter_GetTGD()->pin);
    pChar = SkeletalObject_FindObject(100);
    if (pChar != NULL) {
        Character_SetPosition(pChar, lbl_80211D38[gPlayers[nPlayer].nTarget], 1);
    }
}

// SetCup, then the golfer is set up for the new target: the default aim (AI_DefaultTarget), a fresh
// shot (Shot_Prepare), the stance turned to the target, the golfer's animation 5 and UI message 7
// (fn_80062C38). The target modes' SetupNextGolfer calls it before a player's first shot.
void GameModeSkillZoneBase_SetCup_AlignGolfer(int nPlayer, s8 n) {
    GameModeSkillZoneBase_SetCup(nPlayer, n);
    AI_DefaultTarget(nPlayer);
    Shot_Prepare(nPlayer, 1);
    Character_AlignShotWithTarget(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    fn_80062C38();
}

// Aims the player at the previous target (the last one after target 0) with SetCup; always returns
// 1. Modes 13, 14 and 16 install it as gpGame->pfn264 (button 6 while setting up a shot); mode 15
// calls it from its own.
u8 GameModeSkillZoneBase_PickPrevTarget(int nPlayer) {
    if (gPlayers[nPlayer].nTarget == 0) {
        GameModeSkillZoneBase_SetCup(nPlayer, lbl_80282360 - 1);
    } else {
        GameModeSkillZoneBase_SetCup(nPlayer, gPlayers[nPlayer].nTarget - 1);
    }
    return 1;
}

// Aims the player at the next target (wrapping to 0 after the last) with SetCup; always returns 1.
// Modes 13, 14 and 16 install it as gpGame->pfn258, so the re-plan button (47) picks the next
// target; mode 15 calls it from its own.
u8 GameModeSkillZoneBase_PickTarget(int nPlayer) {
    GameModeSkillZoneBase_SetCup(nPlayer, gPlayers[nPlayer].nTarget + 1);
    return 1;
}

// The target nearest the player's ball (its index in the sorted list): which target a shot landed
// on. Every target mode's shot scoring calls it.
s8 GameModeSkillZoneBase_GetGreenIndexHit(int nPlayer) {
    f32* pBall = gPlayers[nPlayer].ball.vPos;
    s8 i;
    s8 nBest = 0;
    f32 fBest = LLMath_DistanceBetween3(lbl_80211D38[0], pBall);
    for (i = 1; i < lbl_80282360; i++) {
        f32 f = LLMath_DistanceBetween3(lbl_80211D38[i], pBall);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

// The target nearest the player's aim point (Player.vTarget), i.e. the one being played at. The
// target HUD (GameEffects.c) and mode 14 use it.
int GameModeSkillZoneBase_GetGreenTargetted(int nPlayer) {
    f32* pTarget = gPlayers[nPlayer].vTarget;
    int i;
    int nBest = 0;
    f32 fBest = LLMath_DistanceBetween3(lbl_80211D38[0], pTarget);
    for (i = 1; i < lbl_80282360; i++) {
        f32 f = LLMath_DistanceBetween3(lbl_80211D38[i], pTarget);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

// The HUD clock ran out (UI command fn_80088208, in a target mode): mode 13's
// GameModeSkillZoneTimed_TimerOut; the other target modes have no timer.
void GameModeSkillZoneBase_TimerOut(void) {
    if (Game_GetMode() == 0xD) {
        GameModeSkillZoneTimed_TimerOut();
    }
}

// The shot clock ran out (UI command fn_80088804, in a target mode): modes 14 and 15 forfeit the
// shot (GameModeSkillZoneCapture_ShotClockOut, GameModeSkillZoneHorse_ShotClockOut); the other
// target modes do nothing.
void GameModeSkillZoneBase_ShotClockOut(void) {
    if (Game_GetMode() == 0xE) {
        GameModeSkillZoneCapture_ShotClockOut();
    }
    if (Game_GetMode() == 0xF) {
        GameModeSkillZoneHorse_ShotClockOut();
    }
}

// Which ring of a target a landing surface is, 0 being the bullseye: surfaces 0x85..0x87 are rings
// 0..2, 0x88..0x8B rings 0..3 and 0x8C..0x90 rings 0..4; any other surface gives 5. The target
// modes pick their ring comments by it.
s32 GameModeSkillZoneBase_GetBullsEyeColor(s32 nSurface) {
    switch (nSurface) {
    case 0x85: return 0;
    case 0x86: return 1;
    case 0x87: return 2;
    case 0x88: return 0;
    case 0x89: return 1;
    case 0x8A: return 2;
    case 0x8B: return 3;
    case 0x8C: return 0;
    case 0x8D: return 1;
    case 0x8E: return 2;
    case 0x8F: return 3;
    case 0x90: return 4;
    default: return 5;
    }
}

// Clears all five players' target-game state for a new hole: shots taken (nDC0), the counters aDC4,
// the winnings (nDD8), the longest drive (nDDC), bullseyes (nDE0), nE88..nE98, the flags bE9D and
// bE9E, and each target's hit count (nDE4); each player is aimed at target 0 (SetCup). Then
// ClearPerShotData.
void GameModeSkillZoneBase_ClearPerHoleData(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        Player* p = PLAYER(i);
        p->nDC0 = 0;
        for (j = 0; j < 5; j++) {
            p->aDC4[j] = 0;
        }
        p->nDD8 = 0;
        p->nDDC = 0;
        p->nDE0 = 0;
        p->nE88 = 0;
        p->nE8C = 0;
        p->nE90 = 0;
        p->nE94 = 0;
        p->nE98 = 0;
        p->bE9D = 0;
        p->bE9E = 0;
        for (j = 0; j < 40; j++) {
            p->nDE4[j] = 0;
        }
        GameModeSkillZoneBase_SetCup(i, 0);
    }
    GameModeSkillZoneBase_ClearPerShotData();
}

// For each player in the game: the shot's multiplier back to 1 (nDBC), the list of surfaces the
// shot scored on emptied (nCD0 and its 20 entries aCD4), and nDB8 cleared. Each target mode's
// SetupNextGolfer calls it before every shot.
void GameModeSkillZoneBase_ClearPerShotData(void) {
    int i;
    int j;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nCD0 = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nDB8 = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aCD4[j] = 0;
        }
    }
}

// How many of the 40 target slots the player has hit at least once (Player.nDE4).
s32 GameModeSkillZoneBase_CountGreensHit(int nPlayer) {
    int i;
    s32 n = 0;
    for (i = 0; i < 40; i++) {
        if (gPlayers[nPlayer].nDE4[i] != 0) {
            n++;
        }
    }
    return n;
}

// Before a shot (SetupNextGolfer of modes 13 and 16): unless FirstShot says to skip it, a random
// multiplier for the shot (nDBC, which ClearPerShotData set to 1): x5 on a roll of 0..5, x3 on
// 6..10, x2 on 11..20, the roll taken from 0..99, or from 0..19 (so a multiplier is certain) once
// 10 shots in a row had none (nE98, the count this keeps). A multiplier plays Gaud_MultiplierBonus
// and comment 0x3A, 0x3B or 0x3D.
void GameModeSkillZoneBase_SetupBonusBall(int nPlayer) {
    s32 nMsg = -1;
    s32 r;
    if (!GameModeSkillZoneBase_FirstShot(nPlayer)) {
        if (gPlayers[nPlayer].nE98 >= 10) {
            r = Misc_RandFunc(0) % 20;
        } else {
            r = Misc_RandFunc(0) % 100;
        }
        if (r <= 5) {
            gPlayers[nPlayer].nDBC = 5;
        } else if (r <= 10) {
            gPlayers[nPlayer].nDBC = 3;
        } else if (r <= 20) {
            gPlayers[nPlayer].nDBC = 2;
        }
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].nE98 = 0;
    } else {
        gPlayers[nPlayer].nE98++;
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        Gaud_MultiplierBonus();
        switch (gPlayers[nPlayer].nDBC) {
        case 2:
            nMsg = 0x3A;
            break;
        case 3:
            nMsg = 0x3B;
            break;
        case 4:
            break;
        case 5:
            nMsg = 0x3D;
            break;
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_PlayComment((u16)nMsg, 0);
    }
}

// Whether SetupBonusBall skips the multiplier roll for this shot: in mode 16 when 20 shots have
// been taken (Player.nDC0), in mode 17 at 5, in the other modes before the first shot.
u8 GameModeSkillZoneBase_FirstShot(int nPlayer) {
    if (Game_GetMode() == 0x10) {
        if (gPlayers[nPlayer].nDC0 == 20) {
            return 1;
        }
    } else if (Game_GetMode() == 0x11) {
        if (gPlayers[nPlayer].nDC0 == 5) {
            return 1;
        }
    } else if (gPlayers[nPlayer].nDC0 == 0) {
        return 1;
    }
    return 0;
}

// The points the last shot earned, for the HUD (UI command fn_80088660, case 4): the getter of mode
// 13, 14, 16 or 17 (which ignore the player), else 0.
s32 GameModeSkillZoneBase_GetShotEarned(s32 nPlayer) {
    if (Game_GetMode() == 0xD) {
        return GameModeSkillZoneTimed_GetShotEarned(nPlayer);
    }
    if (Game_GetMode() == 0xE) {
        return GameModeSkillZoneCapture_GetShotEarned(nPlayer);
    }
    if (Game_GetMode() == 0x10) {
        return GameModeSkillZoneTarget_GetShotEarned(nPlayer);
    }
    if (Game_GetMode() == 0x11) {
        return GameModeSkillZoneTargetToTarget_GetShotEarned(nPlayer);
    }
    return 0;
}

// The seconds the last shot added, for the HUD (UI command fn_80088660, case 5): mode 13's only,
// else 0.
s32 GameModeSkillZoneBase_GetTimeEarned(s32 nPlayer) {
    if (Game_GetMode() == 0xD) {
        return GameModeSkillZoneTimed_GetTimeEarned(nPlayer);
    }
    return 0;
}

// The bonus multiplier for the HUD (UI command fn_80088660, case 7): mode 13's or mode 16's, else
// 0.
s32 GameModeSkillZoneBase_GetDriveMultiplier(s32 nPlayer) {
    if (Game_GetMode() == 0xD) {
        return GameModeSkillZoneTimed_GetDriveMultiplier(nPlayer);
    }
    if (Game_GetMode() == 0x10) {
        return GameModeSkillZoneTarget_GetDriveMultiplier(nPlayer);
    }
    return 0;
}

// The balls the last shot earned, for the HUD (UI command fn_80088660, case 6): mode 17's only,
// else 0.
s32 GameModeSkillZoneBase_GetExtraBallsEarned(s32 nPlayer) {
    if (Game_GetMode() == 0x11) {
        return GameModeSkillZoneTargetToTarget_GetExtraBallsEarned(nPlayer);
    }
    return 0;
}

// The prize for hitting every target: the gEarningsTable.aMini row with id 999, its mode 13 column
// (n4), mode 16's (n8) or mode 17's (nC); 0 in other modes or without such a row.
s32 GameModeSkillZoneBase_GetHitAllTargetsBonus(void) {
    int i;
    for (i = 0; i < 20; i++) {
        if (gEarningsTable.aMini[i].nId == 999) {
            if (Game_GetMode() == 0xD) {
                return gEarningsTable.aMini[i].n4;
            }
            if (Game_GetMode() == 0x10) {
                return gEarningsTable.aMini[i].n8;
            }
            if (Game_GetMode() == 0x11) {
                return gEarningsTable.aMini[i].nC;
            }
        }
    }
    return 0;
}

// Plays target-game commentary line nMsg (GameModeSkillZoneBase_PlayComment, last argument 1).
void GameModeSkillZoneBase_StartComment(s32 nMsg) {
    GameModeSkillZoneBase_PlayComment((nMsg & 0xFFFF), 1);
}

// Empty in this build. The shot scoring of modes 13, 16 and 17 calls it last, with the player,
// before GameModeSkillZoneBase_PostShotAwards2.
void GameModeSkillZoneBase_PostShotAwards1(int nPlayer) {
}

// Empty in this build. The shot scoring of modes 13, 16 and 17 calls it last, with the player,
// after GameModeSkillZoneBase_PostShotAwards1.
void GameModeSkillZoneBase_PostShotAwards2(int nPlayer) {
}

// Scales nPoints by target nTarget's factor on the current hole (Game_GetCurHoleNum 0, 1 or 2: 13,
// 15 and 15 factors from 1.0 to 1.4, per target in the sorted list); the product is truncated to an
// int. On any other hole nPoints comes back unchanged.
s32 GameModeSkillZoneBase_ScaleTargetPoints(s32 nPoints, int nTarget) {
    if (Game_GetCurHoleNum() == 0) {
        return nPoints * gSkillZoneHole0TargetScale[nTarget];
    }
    if (Game_GetCurHoleNum() == 1) {
        return nPoints * gSkillZoneHole1TargetScale[nTarget];
    }
    if (Game_GetCurHoleNum() == 2) {
        return nPoints * gSkillZoneHole2TargetScale[nTarget];
    }
    return nPoints;
}

// Whether a shot of length fLength (fn_800D0550) reaches the drive line of the player's tee set:
// 313 from tee set 0, 300 from 1, 293 from 2 and 3; never from another. The target modes count a
// target hit only short of it; modes 13, 16 and 17 score a target surface reached that far as a
// drive.
u8 GameModeSkillZoneBase_IsLongDrive(int nPlayer, f32 fLength) {
    switch (gSession.nTeeSet[nPlayer]) {
    case 0:
        if (fLength >= 313.0f) {
            return 1;
        }
        break;
    case 1:
        if (fLength >= 300.0f) {
            return 1;
        }
        break;
    case 2:
    case 3:
        if (fLength >= 293.0f) {
            return 1;
        }
        break;
    }
    return 0;
}

// The index 0..4 of a bonus object the ball hit (the id the pfn268 hook gets, Ball.n140) on the
// current hole: hole 0's objects 0xD7, 0xD5, 0xD4, 0xD6, 0xD8, hole 1's 0x3B..0x3F, hole 2's
// 24..28; 4 for anything else. Modes 13 and 16 raise their multiplier by the index plus 2.
s32 GameModeSkillZoneBase_GetBonusIndex(s32 nId) {
    if (Game_GetCurHoleNum() == 0) {
        switch (nId) {
        case 0xD7: return 0;
        case 0xD5: return 1;
        case 0xD4: return 2;
        case 0xD6: return 3;
        case 0xD8: return 4;
        }
    } else if (Game_GetCurHoleNum() == 1) {
        switch (nId) {
        case 0x3B: return 0;
        case 0x3C: return 1;
        case 0x3D: return 2;
        case 0x3E: return 3;
        case 0x3F: return 4;
        }
    } else if (Game_GetCurHoleNum() == 2) {
        switch (nId) {
        case 24: return 0;
        case 25: return 1;
        case 26: return 2;
        case 27: return 3;
        case 28: return 4;
        }
    }
    return 4;
}

// Plays commentary line nMsg from playlist 7, the target games' lines (Gaud_StartComment, with a
// passed on: StartComment passes 1, SetupBonusBall and modes 14 and 15 pass 0).
void GameModeSkillZoneBase_PlayComment(s32 nMsg, s32 a) {
    Gaud_StartComment(7, nMsg, a);
}
