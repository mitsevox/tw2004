// pgatoursim.h (our name): the PGA TOUR simulation (PGATourSimulation.c, TW06's
// pgatoursimulation.c): the season statistics kept for every tour golfer.

#ifndef GAME_MODES_PGATOURSIM_H
#define GAME_MODES_PGATOURSIM_H

#include "platform.h"

// One golfer's season counts, from which each tour statistic is worked out (0x58 bytes; the
// save profile holds one per tour golfer, 0x58 apart). TW06: GM_Pga_StatCounts, which has
// three more counts (water saves, water hits, long putts) between nNonGIRPars and nEagles.
typedef struct PgaStatCounts {
    u8   unk0[2];
    u16  nRounds;               // 0x02  TW06: nRounds
    u16  nLongestDrive;         // 0x04  TW06: longestDrive
    u16  nDrives;               // 0x06  TW06: nDrives
    u32  nDriveDistance;        // 0x08  all drives together. TW06: totalDriveDistance
    u16  nLongestPutt;          // 0x0C  TW06: longestPutt
    u16  nFairwaysHit;          // 0x0E  TW06: nFairwaysHit
    u16  nFairways;             // 0x10  TW06: nFairwaysPossible
    u16  nGreensHit;            // 0x12  greens in regulation. TW06: nGreensHit
    u16  nHoles;                // 0x14  TW06: nHoles
    u16  nPutts;                // 0x16  TW06: nPutts
    u16  nGIRPutts;             // 0x18  putts on greens hit in regulation. TW06: nGIRPutts
    u16  nBunkerSaves;          // 0x1A  TW06: nBunkerSaves
    u16  nBunkers;              // 0x1C  TW06: nBunkers
    u16  nNonGIRPars;           // 0x1E  pars on greens missed in regulation. TW06: nNonGIRPars
    u16  nBirdiesAfterBogey;    // 0x20  TW06: nBirdiesAfterBogey
    u16  nBogeys;               // 0x22  bogeys or worse. TW06: nBogeysOrWorse
    u16  nEagles;               // 0x24  TW06: nEagles
    u16  nBirdies;              // 0x26  TW06: nBirdies
    u16  nPar3Birdies;          // 0x28  TW06: nPar3Birdies
    u16  nPar3Holes;            // 0x2A  TW06: nPar3Holes
    u16  nPar4Birdies;          // 0x2C  TW06: nPar4Birdies
    u16  nPar4Holes;            // 0x2E  TW06: nPar4Holes
    u16  nPar5Birdies;          // 0x30  TW06: nPar5Birdies
    u16  nPar5Holes;            // 0x32  TW06: nPar5Holes
    u16  nGIRBirdies;           // 0x34  birdies on greens hit in regulation. TW06: nGIRBirdies
    u16  nStrokes;              // 0x36  TW06: nStrokes
    u16  nPar3Strokes;          // 0x38  TW06: nPar3Strokes
    u16  nPar4Strokes;          // 0x3A  TW06: nPar4Strokes
    u16  nPar5Strokes;          // 0x3C  TW06: nPar5Strokes
    u8   unk3E[2];
    u32  nSeasonWinnings;       // 0x40  TW06: seasonWinnings
    u8   unk44[0x4A - 0x44];
    u8   nPlayerOfYearPoints;   // 0x4A  TW06: playerOfYearPoints
    u8   unk4B;
    u16  nConsecutiveCuts;      // 0x4C  TW06: nConsecutiveCuts
    u8   unk4E[2];
    u32  nCareerWinnings;       // 0x50  TW06: careerWinnings
    u8   unk54[0x58 - 0x54];
} PgaStatCounts;
LAYOUT_ASSERT(PgaStatCounts, 0x58);

#endif
