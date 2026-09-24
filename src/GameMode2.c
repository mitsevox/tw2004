// GameMode2.c (our name): game mode 2, Skins. The lowest score on a hole wins its skin (n22C, its
// money); a tie carries the skin over to the next hole. Tied skins at the end go to a sudden-death
// playoff on random holes.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

void  fn_80102704_WinSkinsEvent(void);
void  GameMode4_WinEvent(void);
extern s32 lbl_802823C0;                    // skins carried over
extern s32 lbl_802823C4;                    // the money carried over

void fn_800F81EC(void);
void GameModeSkins_SetupNextGolfer(void);
s32  GameModeSkins_GetHonors(int nPlayer);
u8   GameModeSkins_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeSkins_GameFinished(u8 bCheck);
u8   GameModeSkins_GoToPlayoff(u8 bCheck);
void GameModeSkins_EndHole(void);
void GameModeSkins_EndGame(void);
s32  GameModeSkins_CurrentHoleNumberSkins(void);

// Mode 2 starts: CPUs may concede, no mulligans, nothing carried over.
void fn_800F80FC(void) {
    gpGame->pfnInit = fn_800F80FC;
    gpGame->pfnSetupNextGolfer = GameModeSkins_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkins_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkins_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkins_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkins_GoToPlayoff;
    gpGame->pfnEndHole = GameModeSkins_EndHole;
    gpGame->pfn1EC = fn_800F81EC;
    gpGame->pfnEndGame = GameModeSkins_EndGame;
    gpGame->b274 = 0;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 2;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 2;
    gpGame->nDC = 0;
    lbl_802823C4 = 0;
    lbl_802823C0 = 0;
    gSession.nSplitScreen = 0;
}

void fn_800F81EC(void) {
    lbl_802823C4 = 0;
    lbl_802823C0 = 0;
}

// When everyone waits: the player GetHonors picks goes to pre-shot, the others wait.
void GameModeSkins_SetupNextGolfer(void) {
    int i;
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(1, i);
        } else {
            GOLFERSTATE_Set(19, i);
        }
    }
}

// Only players who can still win the hole (fewer strokes than the
// best holed score) play. On the tee: a player who won a skin (latest hole first), then anyone;
// otherwise the player farthest from the pin (off the green first).
s32 GameModeSkins_GetHonors(int nPlayer) {
    int i;
    int h;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    s32 nLow;
    f32 dx;
    f32 dz;
    f32 d;
    nLow = 999;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            nLow = PLAYER(i)->nStrokes[Game_CurHoleIndex()];
        }
    }
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (i != nPlayer && Player_OnTee(i) && gPlayers[(u32)i].n22C[h] != 0 &&
                    PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
                    return i;
                }
            }
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && Player_OnTee(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            return i;
        }
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            nBest = i;
            break;
        }
    }
    if (nPlayer == 5 && nBest == 5) {
        return 5;
    }
    pCourse = fn_8000C594();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->ball.nLie != LIE_GREEN_e &&
            PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
            dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
            d = fn_80009680(dx * dx + dz * dz);
            if (d > fBest) {
                fBest = d;
                nBest = i;
            }
        }
    }
    if (nBest == 5) {
        fBest = 0.0f;
        nBest = 5;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
                dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
                dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
                d = fn_80009680(dx * dx + dz * dz);
                if (d > fBest) {
                    fBest = d;
                    nBest = i;
                }
            }
        }
    }
    if (nBest == nPlayer) {
        return 5;
    }
    return nBest;
}

// The hole is over when nobody still playing can tie the best holed score (with a tie for the
// lead: beat it); not before someone has holed out.
u8 GameModeSkins_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    int nBest = 5;
    int nSecond = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i)) {
            if (nBest == 5 || nBest != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                            gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                nBest = i;
            }
            if (nBest != i && (nSecond == 5 || nSecond != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                                   gPlayers[nSecond].nStrokes[Game_CurHoleIndex()])) {
                nSecond = i;
            }
        }
    }
    if (nBest == 5) {
        return 0;
    }
    if (nSecond == 5 || nSecond != 5 && gPlayers[nSecond].nStrokes[Game_CurHoleIndex()] >
                                        gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] + 1 <=
                                      gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] + 1 <
                                      gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                return 0;
            }
        }
    }
    return 1;
}

// In the playoff, a hole won ends it; otherwise play on to a
// new random hole.
u8 GameModeSkins_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    int nHole;
    if (gpGame->bD4) {
        if (lbl_802823C0 == 0) {
            return 1;
        }
        if (bCheck) {
            return 0;
        }
        gpGame->nD8++;
        for (h = 0; h < 18; h++) {
            gpGame->bHoleSelected[h] = 0;
        }
        nHole = Game_CurHoleIndex();
        while (nHole == Game_CurHoleIndex()) {
            fn_800E1480(Misc_RandFunc(0) % 18);
        }
        gpGame->bHoleSelected[Game_CurHoleIndex()] = 1;
        // Every player's scores are cleared for the new playoff hole.
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                // fake match: one chained assignment (stored right to left, so nStrokes first, as in
                // GameModeSkins_GoToPlayoff) for the original register order
                gPlayers[i].b2E4[h] = gPlayers[i].b2F6[h] = gPlayers[i].n290[h] = gPlayers[i].n22C[h] =
                    gPlayers[i].nModePoints[h] = gPlayers[i].nPutts[h] = gPlayers[i].nStrokes[h] = 0;
            }
            gPlayers[i].n2D8 = 0;
            gPlayers[i].n2DC = 0;
            gPlayers[i].n2E0 = 0;
            gPlayers[i].n308 = 0;
        }
        GUI_GolfersTiedUIMessage();
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !GameModeSkins_GoToPlayoff(bCheck);
        }
    }
    return 0;
}

// After the last hole, a skin still carried over goes to a playoff.
u8 GameModeSkins_GoToPlayoff(u8 bCheck) {
    int h;
    int i;
    int nHole;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->n22C[Game_CurHoleIndex()] != 0) {
            return 0;
        }
    }
    if (bCheck) {
        return 1;
    }
    gpGame->bD5 = 1;
    for (h = 0; h < 18; h++) {
        if (!gpGame->bHoleSelected[h]) {
            gpGame->bD5 = 0;
        }
    }
    h = 0;
    while (h < 18) {
        gpGame->bHoleSelected[h++] = 0;
    }
    nHole = Game_CurHoleIndex();
    while (nHole == Game_CurHoleIndex()) {
        fn_800E1480(Misc_RandFunc(0) % 18);
    }
    gpGame->bHoleSelected[Game_CurHoleIndex()] = 1;
    // Every player's scores are cleared for the playoff.
    for (i = 0; i < gNumPlayersSetUp; i++) {
        for (h = 0; h < 18; h++) {
            PLAYER(i)->nStrokes[h] = 0;
            PLAYER(i)->nPutts[h] = 0;
            PLAYER(i)->nModePoints[h] = 0;
            PLAYER(i)->n22C[h] = 0;
            PLAYER(i)->n290[h] = 0;
            PLAYER(i)->b2F6[h] = 0;
            PLAYER(i)->b2E4[h] = 0;
        }
        PLAYER(i)->n2D8 = 0;
        PLAYER(i)->n2DC = 0;
        PLAYER(i)->n2E0 = 0;
        PLAYER(i)->n308 = 0;
    }
    gpGame->bD4 = 1;
    gpGame->nD8++;
    GUI_GolfersTiedUIMessage();
    return 1;
}

// A clear winner takes the skin and everything carried over; a tie
// carries it over.
void GameModeSkins_EndHole(void) {
    int i;
    int nBest = 5;
    int nSecond = 5;
    s32 n;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i)) {
            if (nBest == 5 || nBest != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                            gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                nBest = i;
            }
            if (nBest != i && (nSecond == 5 || nSecond != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                                   gPlayers[nSecond].nStrokes[Game_CurHoleIndex()])) {
                nSecond = i;
            }
        }
    }
    if (nSecond != 5 && gPlayers[nBest].nStrokes[Game_CurHoleIndex()] ==
                        gPlayers[nSecond].nStrokes[Game_CurHoleIndex()]) {
        if (!gpGame->bD4) {
            lbl_802823C4 += fn_800D3D64(fn_800D3C1C(), Game_CurHoleIndex());
            lbl_802823C0++;
        }
    } else {
        n = GameModeSkins_CurrentHoleValue();
        gPlayers[nBest].n22C[Game_CurHoleIndex()] = n;
        gPlayers[nBest].n274 += gPlayers[nBest].n22C[Game_CurHoleIndex()];
        gPlayers[nBest].nModePoints[Game_CurHoleIndex()] = 1;
        gPlayers[nBest].nHolesWon += GameModeSkins_CurrentHoleNumberSkins();
        lbl_802823C4 = 0;
        lbl_802823C0 = 0;
    }
}

// Humans with a profile are paid their skins.
void GameModeSkins_EndGame(void) {
    int i;
    int nProfile;
    u8 bFirst = 1;
    s32 bWon;
    if (!fn_800EC550() || fn_801025F4()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsCPU(i)) {
                nProfile = PLAYER(i)->nIndex;
                if (gpSaveData[nProfile].bActive) {
                    if (PLAYER(i)->n274 != 0) {
                        if (bFirst) {
                            EASBio_IncrementGamesWon(1);
                            bFirst = 0;
                        }
                        fn_800E4364(0, 0x6C, PLAYER(i)->n274, nProfile);
                    }
                    fn_800D3548(i, PLAYER(i)->n274, 0);
                    PLAYER(i)->money.n18 += PLAYER(i)->n274;
                }
            }
        }
        if (fn_801025F4()) {
            bWon = 1;
            for (i = 1; i < gNumPlayersSetUp; i++) {
                if (gPlayers[0].n274 <= gPlayers[i].n274) {
                    bWon = 0;
                }
            }
            if (bWon) {
                fn_80102704_WinSkinsEvent();
            } else {
                // EA bug: a lost ladder event is scored as won as well: GameMode4_WinEvent marks its
                // award, unlocks its pro and reward (asm 800F923C; GameMode4 calls it only on a win)
                GameMode4_WinEvent();
            }
        }
    }
}

// The skin on this hole: what is carried over plus this hole's value (the next selected hole's in
// some cases).
s32 GameModeSkins_CurrentHoleValue(void) {
    int h;
    if (gpGame->bD4) {
        return lbl_802823C4;
    }
    if (fn_800E4BF8()) {
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                return lbl_802823C4 + fn_800D3D64(fn_800D3C1C(), h);
            }
        }
    }
    h = Game_CurHoleIndex();
    return lbl_802823C4 + fn_800D3D64(fn_800D3C1C(), h);
}

// Skins at stake on this hole: those carried over, plus one outside the playoff.
s32 GameModeSkins_CurrentHoleNumberSkins(void) {
    if (gpGame->bD4) {
        return lbl_802823C0;
    }
    return lbl_802823C0 + 1;
}

// The first selected hole.
s32 fn_800F9328(void) {
    int h;
    for (h = 0; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}

// The next selected hole after h.
s32 fn_800F93D8(int h) {
    for (h = h + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}

// The selected hole before h.
s32 fn_800F9414(int h) {
    for (h = h - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}
