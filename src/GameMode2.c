// GameMode2.c (our name): game mode 2, Skins. The lowest score on a hole wins its skin (n22C, its
// money); a tie carries the skin over to the next hole. Tied skins at the end go to a sudden-death
// playoff on random holes.

#include "golfer.h"
#include "ball.h"
#include "game.h"

u8    fn_800EC550(void);
void* fn_800D3C1C(void);
s32   fn_800D3D64(void* p, int nHole);      // a hole's skin value
u8    fn_801025F4(void);
void  fn_80102704(void);
void  fn_80102874(void);
void  fn_80125854(int a);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern u8* gpSaveData;
extern s32 lbl_80282278;                    // the player whose turn it is
extern s32 lbl_802823C0;                    // skins carried over
extern s32 lbl_802823C4;                    // the money carried over

void fn_800F81EC(void);
void fn_800F81FC(void);
s32  fn_800F8278(int nPlayer);
u8   fn_800F8624(int nPlayer, int a);
u8   fn_800F8880(u8 bCheck);
u8   fn_800F8B08(u8 bCheck);
void fn_800F8EDC(void);
void fn_800F9100(void);
s32  fn_800F9254(void);
s32  fn_800F9308(void);

// Mode 2 starts: up to four players, CPUs may concede.
void fn_800F80FC(void) {
    gpGame->pfn1C8 = fn_800F80FC;
    gpGame->pfn1D0 = fn_800F81FC;
    gpGame->pfn1D4 = fn_800F8278;
    gpGame->pfn1D8 = fn_800F8624;
    gpGame->pfn1DC = (u8 (*)(int))fn_800F8880;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800F8B08;
    gpGame->pfn1E8 = fn_800F8EDC;
    gpGame->pfn1EC = fn_800F81EC;
    gpGame->pfn1F4 = fn_800F9100;
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

// Hole start: the honor plays first, everyone else waits.
void fn_800F81FC(void) {
    int i;
    lbl_80282278 = gpGame->pfn1D4(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(1, i);
        } else {
            GOLFERSTATE_Set(19, i);
        }
    }
}

// TW06: GameModeSkins::GetHonors. Only players who can still win the hole (fewer strokes than the
// best holed score) play. On the tee: a player who won a skin (latest hole first), then anyone;
// otherwise the player farthest from the pin (off the green first).
s32 fn_800F8278(int nPlayer) {
    int i;
    int h;
    CourseInfo* pCourse;
    int nHole;
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
    nHole = Game_CurrentHole();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->nLie != LIE_GREEN &&
            PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            dx = *(f32*)(PLAYER(i)->ball + 0) - pCourse->pin[nHole].x;
            dz = *(f32*)(PLAYER(i)->ball + 8) - pCourse->pin[nHole].z;
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
                dx = *(f32*)(PLAYER(i)->ball + 0) - pCourse->pin[nHole].x;
                dz = *(f32*)(PLAYER(i)->ball + 8) - pCourse->pin[nHole].z;
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

// The hole is over when nobody still playing can beat (or, with a tie for the lead, tie) the best
// holed score.
u8 fn_800F8624(int nPlayer, int a) {
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

// TW06: GameModeSkins::GameFinished. In the playoff, a hole won ends it; otherwise play on to a
// new random hole.
u8 fn_800F8880(u8 bCheck) {
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
            fn_800E1480(Rand_Next(0) % 18);
        }
        gpGame->bHoleSelected[Game_CurHoleIndex()] = 1;
        // Every player's scores are cleared for the new playoff hole.
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                // fake match: one chained assignment (stored right to left, so nStrokes first, as in
                // fn_800F8B08) for the original register order
                gPlayers[i].b2E4[h] = gPlayers[i].b2F6[h] = gPlayers[i].n290[h] = gPlayers[i].n22C[h] =
                    gPlayers[i].nModePoints[h] = gPlayers[i].nPutts[h] = gPlayers[i].nStrokes[h] = 0;
            }
            gPlayers[i].n2D8 = 0;
            gPlayers[i].n2DC = 0;
            gPlayers[i].n2E0 = 0;
            gPlayers[i].n308 = 0;
        }
        fn_800E45C0();
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !fn_800F8B08(bCheck);
        }
    }
    return 0;
}

// TW06: GameModeSkins::GoToPlayoff. After the last hole, a skin still carried over goes to a playoff.
u8 fn_800F8B08(u8 bCheck) {
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
        fn_800E1480(Rand_Next(0) % 18);
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
    fn_800E45C0();
    return 1;
}

// TW06: GameModeSkins::EndHole. A clear winner takes the skin and everything carried over; a tie
// carries it over.
void fn_800F8EDC(void) {
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
        n = fn_800F9254();
        gPlayers[nBest].n22C[Game_CurHoleIndex()] = n;
        gPlayers[nBest].n274 += gPlayers[nBest].n22C[Game_CurHoleIndex()];
        gPlayers[nBest].nModePoints[Game_CurHoleIndex()] = 1;
        gPlayers[nBest].nHolesWon += fn_800F9308();
        lbl_802823C4 = 0;
        lbl_802823C0 = 0;
    }
}

// TW06: GameModeSkins::EndGame. Humans with a profile are paid their skins.
void fn_800F9100(void) {
    int i;
    int nProfile;
    u8 bFirst = 1;
    s32 bWon;
    if (!fn_800EC550() || fn_801025F4()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsCPU(i)) {
                nProfile = PLAYER(i)->nIndex;
                if (gpSaveData[nProfile * 0x10600]) {
                    if (PLAYER(i)->n274 != 0) {
                        if (bFirst) {
                            fn_80125854(1);
                            bFirst = 0;
                        }
                        fn_800E4364(0, 0x6C, PLAYER(i)->n274, nProfile);
                    }
                    fn_800D3548(i, PLAYER(i)->n274, 0);
                    PLAYER(i)->n32C += PLAYER(i)->n274;
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
                fn_80102704();
            } else {
                fn_80102874();
            }
        }
    }
}

// The skin on this hole: what is carried over plus this hole's value (the next selected hole's in
// some cases).
s32 fn_800F9254(void) {
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

// Skins won on this hole: one plus those carried over (only one in the playoff... as the count).
s32 fn_800F9308(void) {
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
