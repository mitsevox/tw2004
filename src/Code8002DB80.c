// Code8002DB80.c (our name): own unit, split off Golfer.c at 0x8002DB80: the caddie's putt tip,
// the golfer table, the players, bags and options. Its .sdata2 starts on the 8-aligned address
// after Golfer.c's padding at 0x80282EEC..0x80282EF0 and ends padded at 0x80282F1C..0x80282F20;
// its .sbss (0x80281D48..0x80281D50) is used only by these functions. The cut is before
// Caddie_Start: it and Caddie_Stop use only this unit's .sbss, as Caddie_Update does.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"


// ---- the caddie ---------------------------------------------------------------------------------
// The putt tip is the CPU's shot rehearsal run on a copy of the human in player slot 4: the copy
// is made a CPU, aimed at the pin, and AI_RehearseShot is stepped once per frame until the
// simulated ball stops within 0.05 of the target. 600 frames and it gives up.

#define CADDIE_SLOT       4
#define CADDIE_TOLERANCE  (0.05f * 0.05f)   // 0.05 squared
#define CADDIE_MAX_FRAMES 599

// Defined here, last address first (CodeWarrior lays out .sbss in reverse); gNumPlayersSetUp
// (0x80281D48) is declared in golfer.h.
s32 gCaddieFrames;           // 0x80281D4C
u8  gCaddieActive;           // 0x80281D4A
u8  gCaddieDone;             // 0x80281D49
u8  gNumPlayersSetUp;

void Luck_ResetOdds(int nPlayer);
void Golfer_OnStatsLoaded(UStreamObject* pObject);
void Golfer_TableByteSwap(void);
int  Golfer_TierBonus(int nTier);
void Player_SetGolfer(int nPlayer, int nGolfer, int nController, u32 uBag, int bRightSide);

// ---- the golfer table's arrival ---------------------------------------------------------------

void Golfer_TableSetup(void);

// ---- small queries ------------------------------------------------------------------------------

void fn_80013200(int nPad, u8 nValue);

// ---- setting up the players ---------------------------------------------------------------------

#define BAG_ALL      0x03FFFFFF     // every club
#define BAG_DEFAULT  0x01FFFC7F     // a bag with no clubs 7, 8, 9 (TW06's 5-wood, 7-wood, 1-iron)
                                    // or 25 (the putter)


void  fn_80016D18(int nView, f32 x, f32 y, f32 w, f32 h);       // open it (screen fractions)

void Caddie_Start(int nPlayer) {
    if ((!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e) ||
        Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        return;
    }
    Mem_cpy(&gPlayers[CADDIE_SLOT], &gPlayers[nPlayer], sizeof(Player));
    gPlayers[CADDIE_SLOT].nController = CONTROLLER_CPU;
    AI_DefaultTarget(CADDIE_SLOT);
    gPlayers[CADDIE_SLOT].nRehearseState = 2;
    gCaddieDone   = 0;
    gCaddieActive = 1;
    gCaddieFrames = 0;
}

// Stop the caddie.
void Caddie_Stop(void) {
    gCaddieActive = 0;
}

void Caddie_Update(int nPlayer) {
    f32 fDist2;
    if ((!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e) ||
        Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        return;
    }
    if (!gCaddieActive) return;
    if (gPlayers[nPlayer].bPerfect) {
        AI_RehearseShot(CADDIE_SLOT, &fDist2, 0, CADDIE_TOLERANCE);
        if (fDist2 < 0.0625f) {
            gCaddieDone = 1;
        }
    } else {
        if (gCaddieDone) return;
        if (AI_RehearseShot(CADDIE_SLOT, NULL, 0, CADDIE_TOLERANCE)) {
            gCaddieDone = 1;
        }
    }
    gCaddieFrames++;
}

// Returns 0 when there is no tip for this shot or none yet, 1 when a tip is ready (the aim point
// in pOut), 2 when it gave up.
s8 Caddie_GetTip(int nPlayer, f32* pOut) {
    if (gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e || Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        pOut[3] = 0.0f;
        return 0;
    }
    if (gCaddieFrames > CADDIE_MAX_FRAMES) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        pOut[3] = 0.0f;
        gCaddieDone = 1;
        return 2;
    }
    Vec_Copy(gPlayers[CADDIE_SLOT].vTarget, pOut);
    return (s8)gCaddieDone;
}

// ---- the lucky shot ---------------------------------------------------------------------------
// When a human wins the luck roll (Player.bPerfect), Caddie_Start has been running the CPU's
// rehearsal on a copy of them in slot 4, aimed at the pin, since the shot was planned. At the
// moment the ball is struck this swaps the rehearsed shot in for theirs, if the rehearsal is done -
// club, trajectory, kind, power and aim - provided they were playing roughly the same shot: a club
// within two of the rehearsed one, the same shot kind, an aim within 5 degrees. Otherwise the shot
// is not perfect after all. A taken lucky shot puts the player's odds back to 1 in 12; a matching
// shot stops the caddie.
void Luck_TakePerfectShot(int nPlayer) {
    f32  fDiff;

    if (gPlayers[nPlayer].bPerfect == 0 || Player_IsCPU(nPlayer) || gSession.nSplitScreen != 0) return;
    {
        if (gPlayers[nPlayer].nClub > gPlayers[CADDIE_SLOT].nClub + 2) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gPlayers[CADDIE_SLOT].nClub > gPlayers[nPlayer].nClub + 2) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gPlayers[CADDIE_SLOT].nShotKind != gPlayers[nPlayer].nShotKind) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        fDiff = gPlayers[nPlayer].fAim - gPlayers[CADDIE_SLOT].fAim;
        while (fDiff < -PI) {
            fDiff += 2 * PI;
        }
        while (fDiff > PI) {
            fDiff -= 2 * PI;
        }
        if (fabs(fDiff) > DEG(5.0f)) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gCaddieDone) {
            Luck_ResetOdds(nPlayer);
            gPlayers[nPlayer].nClub = gPlayers[CADDIE_SLOT].nClub;
            gPlayers[nPlayer].nTrajectory = gPlayers[CADDIE_SLOT].nTrajectory;
            gPlayers[nPlayer].nShotKind = gPlayers[CADDIE_SLOT].nShotKind;
            gPlayers[nPlayer].fPower = gPlayers[CADDIE_SLOT].fPower;
            gPlayers[nPlayer].fAim = gPlayers[CADDIE_SLOT].fAim;
        }
        Caddie_Stop();
    }
}

// Take the caddie's solved shot (slot 4) as the player's own.
void Caddie_ApplyTip(int nPlayer) {
    gPlayers[nPlayer].nClub       = gPlayers[CADDIE_SLOT].nClub;
    gPlayers[nPlayer].nTrajectory = gPlayers[CADDIE_SLOT].nTrajectory;
    gPlayers[nPlayer].nShotKind   = gPlayers[CADDIE_SLOT].nShotKind;
    gPlayers[nPlayer].fPower      = gPlayers[CADDIE_SLOT].fPower;
    gPlayers[nPlayer].fAim        = gPlayers[CADDIE_SLOT].fAim;
}

void Golfer_RegisterStatsHandler(void) {
    Stream_RegisterLoadChunkCallback('stat', Golfer_OnStatsLoaded);
}

void Golfer_UnregisterStatsHandler(void) {
    Stream_UnregisterLoadChunkCallback('stat');
}

// The 'stat' handler: copy the file over the table, fix its endianness, set it up.
void Golfer_OnStatsLoaded(UStreamObject* pObject) {
    Mem_cpy(gGolferTable, pObject->pData, pObject->uSize);
    Golfer_TableByteSwap();
    fn_80009E70(pObject);
    Golfer_TableSetup();
}

// The table after the 'stat' file arrives: the created golfer being edited goes into slot 30
// when it has one, and the debug "105%" flag (session bit 0x4000) maxes everyone out.
void Golfer_TableSetup(void) {
    int i, k;
    gNumPlayersSetUp = 0;
    if ((s8)gCurGolferRecord.bAvailable != 0) {
        Mem_cpy(&gGolferTable[FIRST_CREATED_GOLFER], &gCurGolferRecord, sizeof(GolferRecord));
    }
    if (gSession.uFlags & 0x4000) {
        for (i = 0; i < NUM_GOLFERS; i++) {
            for (k = 0; k < NUM_ATTRS; k++) {
                gGolferTable[i].attr[k] = 105;
            }
            gGolferTable[i].uBagMask = 0x02A7FC44;
        }
    }
}

// A player's attribute. Base = the record value (attrAlt for a CPU pro in game mode 4 or with
// session flag 2) plus the tier bonus; modifiers are the per-player adjustments; total, both.
int Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode) {
    int nValue = 0;
    if (nMode == ATTR_BASE || nMode == ATTR_TOTAL) {
        // fake match: the (s8) casts on the s8 record values keep the original's extsb before the add
        if (Controller_IsCPU(pPlayer->nController) && pPlayer->golfer.nIndex < FIRST_CREATED_GOLFER &&
            (Game_GetMode() == 4 || (gSession.uFlags & 2))) {
            nValue =
                (s8)((s8)pPlayer->golfer.attrAlt[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        } else {
            nValue = (s8)((s8)pPlayer->golfer.attr[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        }
    }
    if (nMode == ATTR_MODIFIERS || nMode == ATTR_TOTAL) {
        nValue += pPlayer->attrMod[nAttr];
    }
    return nValue;
}

void fn_8002E258(void) {
}

void fn_8002E25C(void) {
}

// Every player in the session, from the session's golfer, controller and bag arrays; odd
// players take the right half of a split screen.
void Players_SetupAll(void) {
    u8 i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (i % 2 != 0) {
            Player_SetGolfer(i, gSession.nGolfer[i], gSession.nController[i], gSession.uBag[i], 1);
        } else {
            Player_SetGolfer(i, gSession.nGolfer[i], gSession.nController[i], gSession.uBag[i], 0);
        }
    }
}

// Fill a player slot from the golfer table and set up its view(s). uBag overrides the record's
// bag when non-zero (or everything, with session flag 0x200); an empty bag gets the default.
// In split screen, bRightSide picks the half; otherwise view 0, plus view 2 in game type 4.
void Player_SetGolfer(int nPlayer, int nGolfer, int nController, u32 uBag, int bRightSide) {
    Player* p = &gPlayers[nPlayer];
    s32     i;

    p->nIndex = nPlayer;
    Mem_cpy(&p->golfer, &gGolferTable[nGolfer], sizeof(GolferRecord));
    p->golfer.nIndex = nGolfer;
    if (gSession.uFlags & 0x200) {
        uBag |= BAG_ALL;
    }
    if (uBag != 0) {
        p->golfer.uBagMask = uBag;
    }
    if (p->golfer.uBagMask == 0) {
        p->golfer.uBagMask = BAG_DEFAULT;
    }
    for (i = 0; i < NUM_ATTRS; i++) {
        p->attrMod[i] = 0;
    }
    p->nClub       = 0;
    p->fAim        = 0.0f;
    p->fPower      = 100.0f;
    p->nShotKind   = SHOT_TYPE_DRIVE_e;
    p->nTrajectory = 1;
    p->vBall[0]      = 0.0f;
    p->vBall[1]      = 0.0f;
    p->vBall[2]      = 0.0f;
    p->vBall[3]      = 0.0f;
    p->ball.nLie        = 0;
    p->vTarget[0]    = 0.0f;
    p->vTarget[1]    = 0.0f;
    p->vTarget[2]    = 0.0f;
    p->vTarget[3]    = 0.0f;
    fn_80009710(p->vOrient);
    p->nController = nController;
    p->bMulliganUsed = 0;
    if (gSession.nSplitScreen) {
        if (bRightSide == 0) {
            if (!fn_800170A0(0)) {
                fn_80016D18(0, 0.0f, 0.0f, 0.5f, 1.0f);
            }
            p->nView[0] = 0;
            p->nView[1] = 0;
            fn_8001704C(p->nView[0], nPlayer);
            fn_8001704C(p->nView[1], nPlayer);
        } else if (bRightSide == 1) {
            if (!fn_800170A0(1)) {
                fn_80016D18(1, 0.5f, 0.0f, 0.5f, 1.0f);
            }
            p->nView[0] = 1;
            p->nView[1] = 1;
            fn_8001704C(p->nView[0], nPlayer);
            fn_8001704C(p->nView[1], nPlayer);
        }
    } else {
        if (!fn_800170A0(0)) {
            fn_80016D18(0, 0.0f, 0.0f, 1.0f, 1.0f);
        }
        p->nView[0] = 0;
        fn_8001704C(p->nView[0], nPlayer);
        if (gSession.nGameType == 4) {
            int nView;
            if (!fn_800170A0(2)) {
                fn_80016D18(2, 0.0f, 0.0f, 1.0f, 1.0f);
            }
            p->nView[1] = 2;
            fn_8001704C(p->nView[1], nPlayer);
            nView = p->nView[1];
            CameraController_SetCameraMode(fn_80017028(nView), 0x19, nPlayer, nView);
        } else {
            p->nView[1] = 0;
            fn_8001704C(p->nView[1], nPlayer);
        }
    }
    p->pChar = gViewSlots[nPlayer].pChar;
    if (p->pChar != NULL) {
        p->pChar->nPlayer = nPlayer;
        if (gSession.nSplitScreen == 2) {
            p->pChar->n16DC = gNumPlayersSetUp * 2;
        }
    }
    p->swing.unk630 = 0;
    p->bLowIQPenalty = 0;
    p->nLevel       = 0;
    p->fC20         = 0.0f;
    p->bC2D         = 0;
    p->bPlanReady       = 0;
    p->bRehearsalDone       = 0;
    p->uFlags       = 0;
    gNumPlayersSetUp++;
}

void Players_Reset(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        GOLFERSTATE_Kill(i);
        PLAYER(i)->pChar = NULL;
    }
    gNumPlayersSetUp = 0;
}

u8 Player_OnTee(int nPlayer) {
    return gPlayers[nPlayer].ball.nLie == 0;
}

u8 Player_IsHoled(int nPlayer) {
    return gPlayers[nPlayer].ball.nLie == LIE_INCUP_e;
}

u8 Player_IsHoledNotState23(int nPlayer) {
    int bResult = 0;
    if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e &&
        (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED) {
        bResult = 1;
    }
    return bResult;
}

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// ---- small accessors ------------------------------------------------------------------------

u8 Player_IsCPU(int nPlayer) {
    return gPlayers[nPlayer].nController == CONTROLLER_CPU;
}

// Team 0 is players 0 and 1, team 1 players 2 and 3.
u8 Team_IsAllCPU(int nTeam) {
    s32 i;
    int nFirst = 2;
    if (nTeam == 0) {
        nFirst = 0;
    }
    for (i = 0; i < 2; i++) {
        if (gPlayers[nFirst + i].nController != CONTROLLER_CPU) return 0;
    }
    return 1;
}

u8 Team_IsAllHuman(int nTeam) {
    s32 i;
    int nFirst = 2;
    if (nTeam == 0) {
        nFirst = 0;
    }
    for (i = 0; i < 2; i++) {
        if (gPlayers[nFirst + i].nController == CONTROLLER_CPU) return 0;
    }
    return 1;
}

u8 Controller_IsCPU(int nController) {
    return nController == CONTROLLER_CPU;
}

u8 Player_IsController8(int nPlayer) {
    return gPlayers[nPlayer].nController == 8;
}

// Controllers 0..7 are pads; 8 is something else; 9 is the CPU.
u8 fn_8002E868_HasPad(int nPlayer) {
    if (gPlayers[nPlayer].nController <= 7) return 1;
    return 0;
}

u8 fn_8002E898_IsPad(int nController) {
    return nController <= 7;
}

u8 fn_8002E8B4(int nPlayer) {
    if (gPlayers[nPlayer].nController <= 8) return 1;
    return 0;
}

u8 fn_8002E8E4(int nController) {
    return nController <= 8;
}

// The bag: one bit per club in the player's copy of the golfer record.
u8 Bag_AddClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bAdded = 0;
    if (!(p->golfer.uBagMask & uBit)) {
        p->golfer.uBagMask |= uBit;
        bAdded = 1;
    }
    return bAdded;
}

u8 Bag_RemoveClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bRemoved = 0;
    if (p->golfer.uBagMask & uBit) {
        p->golfer.uBagMask &= ~uBit;
        bRemoved = 1;
    }
    return bRemoved;
}

u8 Bag_HasClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bHas = 0;
    if (p->golfer.uBagMask & uBit) {
        bHas = 1;
    }
    return bHas;
}

int Bag_CountClubs(int nPlayer) {
    Player* p      = &gPlayers[nPlayer];
    int     nCount = 0;
    int     i;
    for (i = 0; i < CLUB_MAX_e; i++) {
        if (p->golfer.uBagMask & (1 << i)) {
            nCount++;
        }
    }
    return nCount;
}

// An attribute's tier -> bonus points: tiers 1..4 give 1..4, anything else 0.
int Golfer_TierBonus(int nTier) {
    int nBonus = 0;
    switch (nTier) {
    case 1: nBonus = 1; break;
    case 2: nBonus = 2; break;
    case 3: nBonus = 3; break;
    case 4: nBonus = 4; break;
    }
    return nBonus;
}

// The table slot whose record carries this id byte, or -1.
int Golfer_FindById(int nId) {
    int i;
    for (i = 0; i < NUM_GOLFERS; i++) {
        if (nId == gGolferTable[i].nModelID) return i;
    }
    return -1;
}

// The 0xA8 bytes at +0x98 of every record are 21 eight-byte values stored little-endian:
// swap them in place.
void Golfer_TableByteSwap(void) {
    u8* pSrc;
    int i;
    for (i = 0; i < NUM_GOLFERS; i++) {
        pSrc = (u8*)&gGolferTable[i] + 0x98;
        // port: 'stat' is little-endian on disc; a little-endian port does not swap here. Only
        // 0x98..0x140 of each record is swapped, in 8-byte units (the u32 at 0x90 is not); the
        // records are then read in place as GolferRecord.
        BYTESWAP_SWAPDATA(&pSrc, (u8*)&gGolferTable[i] + 0x98, 0xA8, 8);
    }
}

// The 'rcrd' handler: the courses' records into the session.
void Session_OnRecordsLoaded(UStreamObject* pObject) {
    // port: the records are big-endian on disc and copied straight over the course-record structs;
    // a little-endian port converts them field by field here.
    Mem_cpy(gSession.aCourseRecord, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}

void Session_RegisterRecordsHandler(void) {
    Stream_RegisterLoadChunkCallback('rcrd', Session_OnRecordsLoaded);
}

void Session_UnregisterRecordsHandler(void) {
    Stream_UnregisterLoadChunkCallback('rcrd');
}

void fn_8002EBA4(u8* pObj, u8 nValue) {
    int i;
    pObj[7] = nValue;
    for (i = 0; i < 4; i++) {
        fn_80013200(i, pObj[7]);
    }
}

// The default game options. With session flag 0x4000 every row flag is cleared but four
// (rows[0][13], [0][15], [0][17] and [1][0]) and fn_8002EBA4 gets 0 instead of 1.
void Options_SetDefaults(GameOptions* pOpt) {
    int i, j;
    pOpt->a0[0]  = 4;
    pOpt->a0[1]  = 4;
    pOpt->a0[2]  = 5;
    pOpt->a0[3]  = 1;
    pOpt->a0[4]  = 4;
    pOpt->a7[1]    = 1;
    pOpt->a7[2]    = 1;
    pOpt->bGimmes  = 1;
    pOpt->bSkipCameras = 0;
    pOpt->nC       = 2;
    pOpt->nWind    = 0;
    pOpt->n14      = 0;
    pOpt->n18      = 1;
    pOpt->n1C      = 1;
    for (i = 0; i < 8; i++) {
        pOpt->a24[i] = 1;
    }
    pOpt->bBoostEnabled = 1;
    pOpt->bSpinEnabled  = 1;
    pOpt->b7E = 0;
    pOpt->n80 = 1;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 19; j++) {
            pOpt->rows[i][j] = 1;
        }
    }
    pOpt->abRowOn[0] = 1;
    pOpt->abRowOn[1] = 0;
    pOpt->abRowOn[2] = 1;
    pOpt->abRowOn[3] = 0;
    pOpt->b84 = 0;
    fn_8002EBA4((u8*)pOpt, 1);
    if (gSession.uFlags & 0x4000) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 19; j++) {
                pOpt->rows[i][j] = 0;
            }
        }
        pOpt->abRowOn[0]  = 1;
        pOpt->rows[0][13] = 1;
        pOpt->rows[0][15] = 1;
        pOpt->rows[0][17] = 1;
        pOpt->rows[1][0]  = 1;
        fn_8002EBA4((u8*)pOpt, 0);
    }
    fn_80055C40(pOpt->n18);
    fn_80055CD0(pOpt->n1C);
}
