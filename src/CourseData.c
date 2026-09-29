// CourseData.c (our name): the course table read from the disc ('CRI ' chunk: every course's 18
// holes with their par, wind and per-tee values) and the built rounds ('CMPS' chunk: 18 holes
// picked from other courses), with the par and hole lookups the round, the tour and the HUD use.

#include "game_types.h"
#include "game.h"

void GM_CourseInfo_RegisterStreamClients(void);
void GM_CourseInfo_UnRegisterStreamClients(void);
void GM_CourseInfo_LoadCRIfromStream(UStreamObject* pObject);
void GM_CourseInfo_LoadCMPSfromStream(UStreamObject* pObject);
s32  GM_GetHoleIndexHandicap(int nHole);
s32  GM_GetHoleTeeDistance(int nCourse, int nHole, int nTee);
s32  GM_GetCurrentCourseTotalYardage(int nTee);
s32  GM_GetCurrentCourseFront9Yardage(int nTee);
s32  GM_GetCurrentCourseBack9Yardage(int nTee);
s32  GM_GetCurrentCourseFront9Par(void);
s32  GM_GetCurrentCourseBack9Par(void);
u8   GM_GetCurrentHoleSplitScreenLowDetail(void);
u8   GM_GetHoleIndexDrivingSideGame(int nHole);
int  GM_CourseInfo_MapCompilationCourse(int nRound);
int  GM_CurrentCourseTotalPar4andPar5Holes(void);

CourseData gCourseInfo[NUM_COURSE_DATA];
BuiltRound gCompilationCourses[NUM_BUILT_ROUNDS];

// The course table's close, called by GM_DeInitModule with the other units' stream frees: empty
// (the 'CRI ' and 'CMPS' tables are static arrays, nothing to free).
void GM_CourseInfo_DeInit(void) {
}

// Registers the loaders of the 'CRI ' (course table) and 'CMPS' (compilation rounds) chunks with
// the stream; the hole stream manager calls it.
void GM_CourseInfo_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CRI ', GM_CourseInfo_LoadCRIfromStream);
    Stream_RegisterLoadChunkCallback('CMPS', GM_CourseInfo_LoadCMPSfromStream);
}

void GM_CourseInfo_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CRI ');
    Stream_UnregisterLoadChunkCallback('CMPS');
}

// The 'CRI ' chunk's loader: copies it into gCourseInfo.
// port: both chunks are copied straight into their tables; they are big-endian on disc, so a
//       little-endian port converts them field by field here (docs/format-byteorder.md)
void GM_CourseInfo_LoadCRIfromStream(UStreamObject* pObject) {
    Stream_StreamLoadFixedSize(pObject, sizeof(gCourseInfo), gCourseInfo);
}

// The 'CMPS' chunk's loader: copies it into gCompilationCourses.
void GM_CourseInfo_LoadCMPSfromStream(UStreamObject* pObject) {
    Stream_StreamLoadFixedSize(pObject, sizeof(gCompilationCourses), gCompilationCourses);
}

// Hole nHole's (0-based) par on course nCourse, from the course table.
int GM_GetHolePar(int nCourse, int nHole) {
    return gCourseInfo[nCourse].aHoles[nHole].nPar;
}

// The par of the round's hole nHole (0..17; the round's own course and hole for that slot).
int GM_GetHoleIndexPar(int nHole) {
    return gCourseInfo[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].nPar;
}

// The current hole's par.
int GM_GetCurrentHolePar(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].nPar;
}

// The handicap (stroke index) of the round's hole nHole (0..17): the course table's field nRating,
// which the scorecard shows (GM_vGetHoleRating).
s32 GM_GetHoleIndexHandicap(int nHole) {
    return gCourseInfo[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].nRating;
}

// Hole nHole's length (yards) on course nCourse from tee nTee (0..3: fields n14, n10, n0C, n08); 0
// for any other tee.
s32 GM_GetHoleTeeDistance(int nCourse, int nHole, int nTee) {
    switch (nTee) {
    case 0:
        return gCourseInfo[nCourse].aHoles[nHole].n14;
    case 1:
        return gCourseInfo[nCourse].aHoles[nHole].n10;
    case 2:
        return gCourseInfo[nCourse].aHoles[nHole].n0C;
    case 3:
        return gCourseInfo[nCourse].aHoles[nHole].n08;
    }
    return 0;
}

// The length (yards) of the round's hole nHole (0..17) from tee nTee.
s32 GM_GetHoleIndexTeeDistance(int nHole, int nTee) {
    return GM_GetHoleTeeDistance(gpGame->nHoleCourse[nHole], gpGame->nHoleNum[nHole], nTee);
}

s32 GM_GetCurrentHoleTeeDistance(int nTee) {
    return GM_GetHoleTeeDistance(Game_GetCourse(), Game_GetCurHoleNum(), nTee);
}

// The current hole's authored (prevailing) wind direction, 0..7 as gWindDirs; Wind_InitForHole
// rolls a wind when both it and the speed are 0.
int GM_GetCurrentHolePrevailingWindDir(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].nWindDir;
}

f32 GM_GetCurrentHolePrevailingWindSpeed(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].fWindSpeed;
}

// The round's length (yards) from tee nTee: its 18 holes added up (GM_vGetTeeYardage).
s32 GM_GetCurrentCourseTotalYardage(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 0; i < 18; i++) {
        nSum += GM_GetHoleIndexTeeDistance(i, nTee);
    }
    return nSum;
}

// The length (yards) of the round's front nine (holes 0..8) from tee nTee.
s32 GM_GetCurrentCourseFront9Yardage(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 0; i < 9; i++) {
        nSum += GM_GetHoleIndexTeeDistance(i, nTee);
    }
    return nSum;
}

// The length (yards) of the round's back nine (holes 9..17) from tee nTee.
s32 GM_GetCurrentCourseBack9Yardage(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 9; i < 18; i++) {
        nSum += GM_GetHoleIndexTeeDistance(i, nTee);
    }
    return nSum;
}

// The par of the round's front nine (holes 0..8), for the scorecard.
s32 GM_GetCurrentCourseFront9Par(void) {
    s32 nPar = 0;
    int i;
    for (i = 0; i < 9; i++) {
        nPar += GM_GetHoleIndexPar(i);
    }
    return nPar;
}

// The par of the round's back nine (holes 9..17), for the scorecard.
s32 GM_GetCurrentCourseBack9Par(void) {
    s32 nPar = 0;
    int i;
    for (i = 9; i < 18; i++) {
        nPar += GM_GetHoleIndexPar(i);
    }
    return nPar;
}

// Course nCourse's par from tee set nTeeSet, from the course table; 72 for course 23 (Random 18);
// for course 22 (Tiger's Dream 18) its 18 holes' pars added up from the compilation table.
s32 GM_GetTotalPar(int nCourse, int nTeeSet) {
    int nHoleCourse;
    int i;
    s32 nPar;
    if (nCourse == 23) return 72;
    if (nCourse == 22) {
        nPar = 0;
        for (i = 0; i < 18; i++) {
            nHoleCourse = GM_CourseInfo_GetCompilationCourse(22, i);
            nPar += GM_GetHolePar(nHoleCourse, GM_CourseInfo_GetCompilationHole(22, i) - 1);
        }
        return nPar;
    }
    return gCourseInfo[nCourse].aTeeSets[nTeeSet].nPar;
}

// The par of the round's 18 holes (nTeeSet is not used).
s32 GM_GetCurrentCourseTotalPar(s32 nTeeSet) {
    s32 nPar = 0;
    int i;
    for (i = 0; i < 18; i++) {
        nPar += GM_GetHoleIndexPar(i);
    }
    return nPar;
}

// Whether the current hole asks for the cut-down drawing in two-player split screen: its
// course-table byte 0x34 (b34). The round's frame then sets gSession.b11, which skips the terrain
// objects' sort and level fades and the golfers' morph blending.
u8 GM_GetCurrentHoleSplitScreenLowDetail(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].b34;
}

// Whether the round's hole nHole (0..17) can hold the longest-drive contest (course-table byte
// 0x35, b35); HoleContest_DrawHoles also needs a par 4 or 5.
u8 GM_GetHoleIndexDrivingSideGame(int nHole) {
    return gCourseInfo[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].b35;
}

// Whether the round's hole nHole's drive counts for the driving-distance stats (course-table byte
// 0x37, b37): the PGA Tour's end of hole and its simulated players both check it.
u8 GM_GetHoleIndexCountsForDrivingStat(int nHole) {
    return gCourseInfo[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].b37;
}

// Which row of the 'CMPS' compilation table a built round uses: course 22 (Tiger's Dream 18) row 0,
// courses 24..29 (Compilation 1..6) rows 1..6; 0 for any other course.
int GM_CourseInfo_MapCompilationCourse(int nRound) {
    switch (nRound) {
    case 22:
        return 0;
    case 24:
        return 1;
    case 25:
        return 2;
    case 26:
        return 3;
    case 27:
        return 4;
    case 28:
        return 5;
    case 29:
        return 6;
    }
    return 0;
}

// The course that built round nRound's hole nHole (0..17) is taken from.
int GM_CourseInfo_GetCompilationCourse(int nRound, int nHole) {
    return gCompilationCourses[GM_CourseInfo_MapCompilationCourse(nRound)].aHoles[nHole].nCourse;
}

// The hole number (1-based) on its own course of built round nRound's hole nHole (0..17).
int GM_CourseInfo_GetCompilationHole(int nRound, int nHole) {
    int nRow = GM_CourseInfo_MapCompilationCourse(nRound);
    return gCompilationCourses[nRow].aHoles[nHole].nHole;
}

// How many of the round's 18 holes are a par nPar.
int GM_CurrentCourseTotalParXHoles(int nPar) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (nPar == GM_GetHoleIndexPar(i)) {
            nCount++;
        }
    }
    return nCount;
}

// The number of the 18 holes that are a par 4 or 5.
int GM_CurrentCourseTotalPar4andPar5Holes(void) {
    return GM_CurrentCourseTotalParXHoles(4) + GM_CurrentCourseTotalParXHoles(5);
}

// Data order: after GM_CourseInfo_MapCompilationCourse's jump table in .data; "Skillz" and "NA"
// land in .sdata.
char* gCourseNames[NUM_COURSES] = {
    "Pebble Beach",
    "Princeville Resort",
    "TPC at Sawgrass",
    "Black Rock Cove",
    "Penguin Falls",
    "Bethpage Black",
    "Royal Birkdale",
    "Skillz",
    "Bay Hill Club",
    "The Predator",
    "Spyglass Hill",
    "Poppy Hills",
    "The Highlands",
    "TPC of Scottsdale",
    "Torrey Pines",
    "St Andrews",
    "Sahalee CC",
    "Emerald Dragon",
    "Wallaby Creek",
    "Kapalua Plantation",
    "Pinehurst No. 2",
    "NA",
    "Tiger's Dream 18",
    "Random 18",
    "Compilation 1",
    "Compilation 2",
    "Compilation 3",
    "Compilation 4",
    "Compilation 5",
    "Compilation 6",
};
