// challenge.h (our name): the challenges of game mode 5 (PlayNowMode.c), which game mode 24
// (GameModeDriverRTE.c) and the ladder (LadderedMode.c) also run. A challenge is a round set up in
// another game mode, with a target score and three medals.

#ifndef GAME_MODES_CHALLENGE_H
#define GAME_MODES_CHALLENGE_H

#include "game_types.h"
#include "platform.h"

// A medal of a challenge: the rule it is judged by (0 none; see PlayNow_GetMedal), the mark the rule
// compares with, and the reward.
typedef struct ChallengeMedal {
    s32 nRule;                  // 0x0
    s32 nMark;                  // 0x4
    s32 nReward;                // 0x8
} ChallengeMedal;

// One challenge (0x80 bytes); gChallengeList points to the list being played.
typedef struct Challenge {
    s32 nGroupName;             // 0x00  its group's name: an offset into the 'PLYs' text
                                //       (PlayNow_GetGroupName)
    s32 nGroupDesc;             // 0x04  the same for the group's description
    s32 nGroup;                 // 0x08  challenges with the same group are played together
    s32 nMode;                  // 0x0C  the game mode it is played as (GM_SetModeType)
    s32 nCourse;                // 0x10
    s32 nType;                  // 0x14  0 one hole, 1 all 18, 2/3 a nine, 4/5/6 the par 5s/4s/3s
    s32 nHole;                  // 0x18  1-based
    s32 nTeeSet;                // 0x1C  for every player
    s32 nPins;                  // 0x20  the pin set plus 1 (gSession.nPinSet; 0 = leave it)
    s32 nClubBits;              // 0x24  the bag, one bit per club (see PlayNow_ApplyChallengeSetup);
                                //       0 = the golfer's own. Signed: the original tests it with cmpwi
    s32 nOpponents;             // 0x28  CPU players, 0..3
    s32 aOpponent[3];           // 0x2C  their golfers
    u8  unk38[0x3C - 0x38];
    s32 nTargetKind;            // 0x3C  how the target is built (see PlayNow_GetChallengeTarget)
    s32 nTargetBase;            // 0x40
    s32 nHoleKind;              // 0x44  what the challenge hole adds
    s32 nHoleExtra;             // 0x48
    u8  bPlaceBall;             // 0x4C  the ball starts at the spot in gPlayNowBallSpots
    u8  bForceWeather;          // 0x4D  the weather option becomes 3, at fWeatherAmount
                                //       (PlayNow_ForceWeather); clear: option 0
    u8  unk4E[2];
    s32 nWind;                  // 0x50  the wind option while it is played
    f32 fWeatherAmount;         // 0x54
    s32 nScoring;               // 0x58  0 the round's totals, 1 this hole
    ChallengeMedal aMedal[3];   // 0x5C  0 the best
} Challenge;
LAYOUT_ASSERT(Challenge, 0x80);

// Where a challenge places the ball (12 bytes), when its bPlaceBall is set.
typedef struct ChallengeSpot {
    f32 fX;                     // 0x0
    f32 fY;                     // 0x4
    f32 fZ;                     // 0x8
} ChallengeSpot;

// A course object of type 10, handed to PlayNow_LoadBallSpot by the course loader (fn_80048BDC):
// the spot where challenge nChallenge (1-based) places the ball.
typedef struct ChallengeSpotRecord {
    u8  unk0[0x10];
    f32 v[3];                   // 0x10
    u8  nChallenge;             // 0x1C
} ChallengeSpotRecord;

extern Challenge  gPlayNowChallenges[83];    // mode 5's own challenges ('PLY ')
extern ChallengeSpot gPlayNowBallSpots[83];  // their ball spots
extern Challenge* gChallengeList;            // the list being played
extern s32        gNumChallenges;            // its length
extern s32        gCurChallenge;             // the current one

void PlayNow_SetChallengeList(Challenge* pList, s32 nCount);     // play from another list (mode 24's)

#endif
