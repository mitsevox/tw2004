// GameModeBattle.c (TW06's GameModeBattle): game mode 25, two-player match play where the winner of
// a hole takes a club from the loser's bag. Three clubs can never be taken; a player left with one
// club that can be taken loses as soon as the other wins a hole.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

extern u32 lbl_8020315C[5];                 // the bags at the start of the round
extern s32 lbl_80203148[5];                 // how many clubs each bag had then
extern s32 lbl_801925B8[5];                 // per player: the club taken from them this hole (26 = none)
extern s32 lbl_80281650;                    // the winner of the last hole (5 = nobody)
extern u8  lbl_802822E8;                    // a club is to be taken

void fn_800E7A7C(void);
void fn_800E7A9C(void);
u8   fn_800E7ABC(u8 bCheck);
void fn_800E7B58(void);
void fn_800E7CBC(void);
u8   fn_800E7E64(int nClub);
int  fn_800E7E88(int nPlayer);
void fn_800E7F88(void);
void fn_800E800C(void);

// TW06: GameModeBattle::Init. Two players; the CPU may concede.
void fn_800E7980(void) {
    gpGame->pfnInit = fn_800E7980;
    gpGame->pfnShutdown = fn_800E7A7C;
    gpGame->pfnSetupNextGolfer = fn_800E9F14;
    gpGame->pfnGetHonors = fn_800EA084;
    gpGame->pfnHoleFinished = fn_800EA278;
    gpGame->pfnGameFinished = fn_800E7ABC;
    gpGame->pfnGoToPlayoff = fn_800EA758;
    gpGame->pfnEndHole = fn_800E7B58;
    gpGame->pfnEndGame = fn_800E7CBC;
    gpGame->pfn1F0 = fn_800E7A9C;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 2;
    gpGame->n10 = 2;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// TW06: GameModeBattle::Shutdown. The bags go back to how they were.
void fn_800E7A7C(void) {
    fn_800E800C();
}

// The round starts: the bags are saved.
void fn_800E7A9C(void) {
    fn_800E7F88();
}

// TW06: GameModeBattle::GameFinished. The usual match-play end, or a player with at most one club
// left to lose who has just lost a hole.
u8 fn_800E7ABC(u8 bCheck) {
    if (fn_800EA548(bCheck)) {
        return 1;
    }
    if ((fn_800E7E88(0) <= 1 && gPlayers[1].nModePoints[Game_CurHoleIndex()] != 0) ||
        (fn_800E7E88(1) <= 1 && gPlayers[0].nModePoints[Game_CurHoleIndex()] != 0)) {
        return 1;
    }
    return 0;
}

// TW06: GameModeBattle::EndHole. The player who holed out in fewer strokes wins the hole and may
// take a club (not when that ends the game).
void fn_800E7B58(void) {
    lbl_802822E8 = 0;
    lbl_80281650 = 5;
    if (Player_IsHoled(0) && gPlayers[0].nStrokes[Game_CurHoleIndex()] < gPlayers[1].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
        lbl_802822E8 = 1;
        lbl_80281650 = 0;
        gPlayers[0].nHolesWon++;
    }
    if (Player_IsHoled(1) && gPlayers[1].nStrokes[Game_CurHoleIndex()] < gPlayers[0].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
        lbl_802822E8 = 1;
        lbl_80281650 = 1;
        gPlayers[1].nHolesWon++;
    }
    if (fn_800E7ABC(1)) {
        lbl_802822E8 = 0;
    }
    lbl_801925B8[0] = 26;
    lbl_801925B8[1] = 26;
    lbl_801925B8[2] = 26;
    lbl_801925B8[3] = 26;
    lbl_801925B8[4] = 26;
}

// TW06: GameModeBattle::EndGame. Prize money for a human winner with a profile (no margin when the
// game ended on clubs).
void fn_800E7CBC(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        if (fn_800E7E88(0) <= 1 && gPlayers[1].nModePoints[Game_CurHoleIndex()] != 0) {
            nWinner = 1;
            nLoser = 0;
            nMargin = 0;
        } else if (fn_800E7E88(1) <= 1 && gPlayers[0].nModePoints[Game_CurHoleIndex()] != 0) {
            nWinner = 0;
            nLoser = 1;
            nMargin = 0;
        } else if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
            nMargin = gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
            nWinner = 0;
            nLoser = 1;
        } else {
            nMargin = gPlayers[1].nHolesWon - gPlayers[0].nHolesWon;
            nWinner = 1;
            nLoser = 0;
        }
        nMoney = fn_800D36E0(nWinner, nLoser, nMargin, &nPrize);
        if (!Player_IsCPU(nWinner)) {
            nProfile = gPlayers[nWinner].nIndex;
            if (gpSaveData[nProfile].bActive) {
                fn_80125910(1);
                if (nMoney) {
                    fn_800E4364(0, 0x6B, nPrize, nProfile);
                    fn_800D3548(nWinner, nMoney, 0);
                    gPlayers[nWinner].money.n14 += nMoney;
                }
            }
        }
    }
}

// TW06: GameModeBattle::ClubIsRequired. Clubs 13, 21 and 25 cannot be taken.
u8 fn_800E7E64(int nClub) {
    u8 bRequired = 0;
    if ((1 << nClub) & 0x2202000) {
        bRequired = 1;
    }
    return bRequired;
}

// TW06: GameModeBattle::NumRemovableClubsLeft.
int fn_800E7E88(int nPlayer) {
    int i;
    int n = 0;
    for (i = 0; i < 26; i++) {
        if (Bag_HasClub(nPlayer, i) && !fn_800E7E64(i)) {
            n++;
        }
    }
    return n;
}

// TW06: GameModeBattle::RemoveClub.
u8 fn_800E7F00(int nPlayer, int nClub) {
    u8 bRemoved;
    if (fn_800E7E64(nClub)) {
        return 0;
    }
    bRemoved = Bag_RemoveClub(nPlayer, nClub);
    lbl_801925B8[nPlayer] = nClub;
    return bRemoved;
}

// TW06: GameModeBattle::AddClub.
void fn_800E7F68(int nPlayer, int nClub) {
    Bag_AddClub(nPlayer, nClub);
}

// TW06: GameModeBattle::SaveClubSetup.
void fn_800E7F88(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        *(u32*)((u8*)lbl_8020315C + i * sizeof(u32)) = PLAYER(i)->golfer.uBagMask;
        *(s32*)((u8*)lbl_80203148 + i * sizeof(s32)) = Bag_CountClubs(i);
    }
}

// TW06: GameModeBattle::RestoreClubSetup.
void fn_800E800C(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        PLAYER(i)->golfer.uBagMask = *(u32*)((u8*)lbl_8020315C + i * sizeof(u32));
    }
}

s32 fn_800E8114(int nPlayer) {
    return lbl_80203148[nPlayer];
}

// TW06: GameModeBattle::CanAddClub. A club the player started with and no longer has.
int fn_800E8128(int nPlayer, int nClub) {
    u32 uBit = 1 << nClub;
    int bCan = 0;
    if (Bag_HasClub(nPlayer, nClub)) {
        return 0;
    }
    if (uBit & lbl_8020315C[nPlayer]) {
        bCan = 1;
    }
    return bCan;
}

s32 fn_800E81A0(int nPlayer) {
    return lbl_801925B8[nPlayer];
}

// TW06: GameModeBattle::IsClubStealingFinished (inverted: nonzero while a club is to be taken).
u8 fn_800E81B4(void) {
    return lbl_802822E8;
}

// TW06: GameModeBattle::GetWinner.
s32 fn_800E81BC(void) {
    return lbl_80281650;
}
