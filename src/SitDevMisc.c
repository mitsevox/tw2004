// SitDevMisc.c (EA's name: TW06's and TW07's Golf\SitDev\SitDevMisc.c; TW07 has these functions
// in this order): the commentary scripts' odds and ends. The struck ball they watch
// (SitDev_SetBallHitTime at the hit; each frame SitDev_ThrowBallHitDelayedEvent raises event 75
// 48 frames in and notes the cup bevel), the emotion flags response kinds 12 and 13 set, drawing
// from a response list without repeats, and two values the scripts test (the tour event place
// band, the game mode bit). Own unit; its .sbss is padded to 8 at 0x80282201..0x80282208.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

// Per game mode: the number of the scripts' mode bit (SitDev_TranslateGameMode), -1 for none.
s32 gSitDevGameModeBits[28] = {
    0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1, 7, 8, 9, 10, 11, 12, 13, 14,
    -1, 0,
};

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    gSitDevCupBevelFlag;      // 0x80282200  the watched ball has lain on surface 105 (the cup
                                //             bevel) this shot
Ball* gSitDevBallThatWasHit;    // 0x802821FC  the watched ball, NULL for none
u32   gSitDevBallHitTime;       // 0x802821F8  gSession.nFrameCount when it was struck

s32 gSitDevPredictedEmotionSet[5];  // per player: 1 when response kind 13 set the predicted emotion
s32 gSitDevEmotionSet[5];           // per player: 1 when response kind 12 set the shot's emotion

// ---- the watched ball ----------------------------------------------------------------------

void SitDev_ClearCupBevelFlag(void);

// A mulligan (GM_PlayerTakeMulligan): clears the cup bevel flag (SitDev_ClearCupBevelFlag), so the
// retaken shot can queue situation event 34 again. TW07's is empty.
void SitDev_OnMulligan(void) {
    SitDev_ClearCupBevelFlag();
}

// Stops watching the struck ball (SitDev_SetBallHitTime starts): at round start
// (SitDev_vInitModule) and at every shot set-up (situation event 3).
void SitDev_ClearBallThatWasHit(void) {
    gSitDevBallThatWasHit = NULL;
}

// Whether the watched ball has lain on surface 105, the cup bevel, this shot
// (SitDev_ThrowBallHitDelayedEvent sets the flag): state value 95.
u8 SitDev_GetCupBevelFlag(void) {
    return gSitDevCupBevelFlag;
}

// Clears the cup bevel flag: at every shot set-up (situation event 3) and for a mulligan.
void SitDev_ClearCupBevelFlag(void) {
    gSitDevCupBevelFlag = 0;
}

// Every frame from gomainloop.c (fn_8006D8E8), for the watched ball (nothing when none): while it
// flies or rolls (nState 2..4), exactly 48 frames after it was hit it triggers event 75
// (EVENT_BallHitDelayed: situation event 31); the first time it lies on surface 105 (the cup bevel)
// it queues situation event 34 and sets the cup bevel flag.
void SitDev_ThrowBallHitDelayedEvent(void) {
    if (gSitDevBallThatWasHit == NULL) return;
    switch (gSitDevBallThatWasHit->nState) {
    case 2:
    case 3:
    case 4:
        if (48.0f == (f32)(gSession.nFrameCount - gSitDevBallHitTime)) {
            EVENT_Trigger(gSitDevBallThatWasHit->nPlayer, 0x4B, NULL, -1);
        }
        break;
    }
    if (gSitDevBallThatWasHit->nSurface == 105 && !gSitDevCupBevelFlag) {
        SitDev_QueueEvent(gSitDevBallThatWasHit->nPlayer, 0, 0x22);
        gSitDevCupBevelFlag = 1;
    }
}

// Starts watching a ball as it is struck (EVENT_HitBall): remembers it and the frame
// (gSession.nFrameCount) for SitDev_ThrowBallHitDelayedEvent.
void SitDev_SetBallHitTime(Ball* pBall) {
    gSitDevBallThatWasHit = pBall;
    gSitDevBallHitTime = gSession.nFrameCount;
}

// Forgets, for all five players, that the scripts set the shot's emotion (gSitDevEmotionSet,
// response kind 12) or its predicted emotion (gSitDevPredictedEmotionSet, kind 13): situation
// events 2, 3 and 25.
void SitDev_ClearEmotionStates(void) {
    int i;
    for (i = 0; i < 5; i++) {
        gSitDevEmotionSet[i] = 0;
        gSitDevPredictedEmotionSet[i] = 0;
    }
}

// Whether the scripts set the player's predicted emotion this shot (response kind 13):
// GM_SimulateBallMovement starts a post-shot reaction only then.
u8 SitDev_PredictedEmotionAvailable(int nPlayer) {
    return gSitDevPredictedEmotionSet[nPlayer] == 1;
}

// ---- picking without repeats ---------------------------------------------------------------

// A list of nCount u16 values used as a deck: the top bit marks one already drawn, 0xFFF0 is an
// empty slot.

// The entries of the first nCount of a list that are in use (not 0xFFF0, the empty mark); an
// action's sound list (SitDev_InvokeCommentaryBank).
int SitDev_NumEntries(u16* pList, int nCount) {
    int i;
    int nUsed = 0;
    for (i = 0; i < nCount; i++) {
        if (pList[i] != 0xFFF0) {
            nUsed++;
        }
    }
    return nUsed;
}

// The entries of the list not drawn yet (bit 15 clear). When every one is drawn, the marks of all
// nCount are cleared and nCount is returned.
int SitDev_NumEntriesUnused(u16* pList, int nCount) {
    int nLeft = 0;
    int i;
    u16* p = pList;
    for (i = 0; i < nCount; i++) {
        if ((*pList & 0x8000) != 0x8000) {
            nLeft++;
        }
        pList++;
    }
    if (nLeft == 0) {
        i = nCount;
        while (i-- > 0) {
            *p &= 0x7FFF;
            p++;
        }
        return nCount;
    }
    return nLeft;
}

// Draws the nPick'th entry not drawn yet (bit 15 clear), marks it drawn and returns it; nPick
// itself when there are not that many. nLeft (SitDev_NumEntriesUnused's count) is not read.
u32 SitDev_ChooseRandomResponseNoRepeat(u16* pList, int nCount, int nLeft, u32 nPick) {
    int i;
    u32 n = 0;
    for (i = 0; i < nCount; i++) {
        if (!(*pList & 0x8000)) {
            if (nPick == n) {
                nPick = *pList;
                *pList |= 0x8000;
                break;
            }
            n++;
        }
        pList++;
    }
    return nPick;
}

// The player's place in the tour event as a band for the scripts (state value 94): 0 the top 3, 1
// the top 10, 2 the top 25, 3 below. The place comes from GM_PgaTourSim_GetScoreRankFromEntrantID
// in game mode 23 and is 1 in any other mode.
int SitDev_GetPGARank(int nPlayer) {
    int nRank;
    if (Game_GetMode() == 23) {
        nRank = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, 0);
    } else {
        nRank = 1;
    }
    if (nRank <= 3) return 0;
    if (nRank <= 10) return 1;
    return nRank <= 25 ? 2 : 3;
}

// The game mode as the scripts' mode bit (state value 54): 1 << gSitDevGameModeBits[nMode], 0 for
// a mode the table gives -1.
u16 SitDev_TranslateGameMode(int nMode) {
    s32 nBit = gSitDevGameModeBits[nMode];
    return nBit == -1 ? 0 : 1 << nBit;
}
