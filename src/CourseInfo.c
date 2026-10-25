// CourseInfo.c: the course table ('CRI ': each course's 18 holes with their par, handicap, length
// from four tees, own wind and side-game flags, and each tee set's par) and the compilation courses
// ('CMPS'), with the par, length and hole lookups the round, the scorecard and the wind use.

#include "game_types.h"
#include "game.h"

CourseData gCourseInfo[NUM_COURSE_DATA];
GM_CompilationCourseData_t gCompilationCourses[NUM_COMPILATION_COURSES];

static void GM_CourseInfo_LoadCRIfromStream(TStreamData* pStreamData);
static void GM_CourseInfo_LoadCMPSfromStream(TStreamData* pStreamData);

// Called when a round is torn down (GM_DeInitModule): empty, the tables are static arrays.
void GM_CourseInfo_DeInit(void) {
}

// The global data's stream manager registers the two tables' chunk loaders.
void GM_CourseInfo_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CRI ', GM_CourseInfo_LoadCRIfromStream);
    Stream_RegisterLoadChunkCallback('CMPS', GM_CourseInfo_LoadCMPSfromStream);
}

void GM_CourseInfo_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CRI ');
    Stream_UnregisterLoadChunkCallback('CMPS');
}

static void GM_CourseInfo_LoadCRIfromStream(TStreamData* pStreamData) {
    Stream_StreamLoadFixedSize(pStreamData, sizeof(gCourseInfo), gCourseInfo);
}

static void GM_CourseInfo_LoadCMPSfromStream(TStreamData* pStreamData) {
    Stream_StreamLoadFixedSize(pStreamData, sizeof(gCompilationCourses), gCompilationCourses);
}

int GM_GetHolePar(int course, int hole) {
    return gCourseInfo[course].aHoles[hole].nPar;
}

// The round's hole holeindex (0..17) is a hole of its own course (gpGame->nHoleCourse, nHoleNum).
int GM_GetHoleIndexPar(int holeindex) {
    return gCourseInfo[gpGame->nHoleCourse[holeindex]].aHoles[gpGame->nHoleNum[holeindex]].nPar;
}

int GM_GetCurrentHolePar(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].nPar;
}

// The hole's handicap (stroke index), which the scorecard shows as its rating.
s32 GM_GetHoleIndexHandicap(int holeindex) {
    return gCourseInfo[gpGame->nHoleCourse[holeindex]].aHoles[gpGame->nHoleNum[holeindex]].nHandicap;
}

// The hole's length in yards from tee 0..3; 0 for any other tee.
s32 GM_GetHoleTeeDistance(int course, int hole, int tee) {
    switch (tee) {
    case 0:
        return gCourseInfo[course].aHoles[hole].nYardsTee0;
    case 1:
        return gCourseInfo[course].aHoles[hole].nYardsTee1;
    case 2:
        return gCourseInfo[course].aHoles[hole].nYardsTee2;
    case 3:
        return gCourseInfo[course].aHoles[hole].nYardsTee3;
    }
    return 0;
}

s32 GM_GetHoleIndexTeeDistance(int holeindex, int tee) {
    return GM_GetHoleTeeDistance(gpGame->nHoleCourse[holeindex], gpGame->nHoleNum[holeindex], tee);
}

s32 GM_GetCurrentHoleTeeDistance(int tee) {
    return GM_GetHoleTeeDistance(Game_GetCourse(), Game_GetCurHoleNum(), tee);
}

// The current hole's own wind direction (0..7, gWindDirs); with a speed of 0 as well, the hole has
// none and Wind_InitForHole rolls one.
int GM_GetCurrentHolePrevailingWindDir(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].nWindDir;
}

f32 GM_GetCurrentHolePrevailingWindSpeed(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].fWindSpeed;
}

// The round's length in yards from tee tee: its 18 holes added up.
s32 GM_GetCurrentCourseTotalYardage(int tee) {
    s32 totalyards = 0;
    int i;
    for (i = 0; i < 18; i++) {
        totalyards += GM_GetHoleIndexTeeDistance(i, tee);
    }
    return totalyards;
}

// The front nine: the round's holes 0..8.
s32 GM_GetCurrentCourseFront9Yardage(int tee) {
    s32 totalyards = 0;
    int i;
    for (i = 0; i < 9; i++) {
        totalyards += GM_GetHoleIndexTeeDistance(i, tee);
    }
    return totalyards;
}

// The back nine: the round's holes 9..17.
s32 GM_GetCurrentCourseBack9Yardage(int tee) {
    s32 totalyards = 0;
    int i;
    for (i = 9; i < 18; i++) {
        totalyards += GM_GetHoleIndexTeeDistance(i, tee);
    }
    return totalyards;
}

s32 GM_GetCurrentCourseFront9Par(void) {
    s32 totalpar = 0;
    int i;
    for (i = 0; i < 9; i++) {
        totalpar += GM_GetHoleIndexPar(i);
    }
    return totalpar;
}

s32 GM_GetCurrentCourseBack9Par(void) {
    s32 totalpar = 0;
    int i;
    for (i = 9; i < 18; i++) {
        totalpar += GM_GetHoleIndexPar(i);
    }
    return totalpar;
}

// A course's par from tee set tee, as the course table gives it. Random 18 counts as par 72, and
// Tiger's Dream 18 adds up its 18 holes' pars from their own courses.
s32 GM_GetTotalPar(int course, int tee) {
    int nHoleCourse;
    int i;
    s32 nPar;
    if (course == COURSE_RANDOM_18) return 72;
    if (course == COURSE_TIGERS_DREAM_18) {
        nPar = 0;
        for (i = 0; i < 18; i++) {
            nHoleCourse = GM_CourseInfo_GetCompilationCourse(COURSE_TIGERS_DREAM_18, i);
            nPar += GM_GetHolePar(nHoleCourse,
                                  GM_CourseInfo_GetCompilationHole(COURSE_TIGERS_DREAM_18, i) - 1);
        }
        return nPar;
    }
    return gCourseInfo[course].aTeeSets[tee].nPar;
}

// The round's 18 holes' pars added up; tee is not read.
s32 GM_GetCurrentCourseTotalPar(s32 tee) {
    s32 totalpar = 0;
    int i;
    for (i = 0; i < 18; i++) {
        totalpar += GM_GetHoleIndexPar(i);
    }
    return totalpar;
}

// Whether the current hole is drawn with less detail when two views share the screen: the round's
// frame then skips the terrain objects' sort and level fades and the golfers' morph blending.
u8 GM_GetCurrentHoleSplitScreenLowDetail(void) {
    return gCourseInfo[Game_GetCourse()].aHoles[Game_GetCurHoleNum()].bSplitScreenLowDetail;
}

// Whether the round's hole can hold the longest-drive contest (HoleContest_DrawHoles also wants a
// par 4 or 5).
u8 GM_GetHoleIndexDrivingSideGame(int holeindex) {
    return gCourseInfo[gpGame->nHoleCourse[holeindex]].aHoles[gpGame->nHoleNum[holeindex]].bDrivingSideGame;
}

// Whether the drive on the round's hole counts for the PGA Tour's driving-distance stat, the
// player's and the simulated pros' alike.
u8 GM_GetHoleIndexCountsForDrivingStat(int holeindex) {
    return gCourseInfo[gpGame->nHoleCourse[holeindex]].aHoles[gpGame->nHoleNum[holeindex]]
        .bCountsForDrivingStat;
}

// A compilation course's row of the compilation table; row 0 for a course that is not one.
static int GM_CourseInfo_MapCompilationCourse(int compilationCourse) {
    switch (compilationCourse) {
    case COURSE_TIGERS_DREAM_18:
        return COMPILATION_TIGERS_DREAM_18;
    case COURSE_COMPILATION_1:
        return COMPILATION_1;
    case COURSE_COMPILATION_2:
        return COMPILATION_2;
    case COURSE_COMPILATION_3:
        return COMPILATION_3;
    case COURSE_COMPILATION_4:
        return COMPILATION_4;
    case COURSE_COMPILATION_5:
        return COMPILATION_5;
    case COURSE_COMPILATION_6:
        return COMPILATION_6;
    }
    return COMPILATION_TIGERS_DREAM_18;
}

// The course a compilation course's hole holeIndex (0..17) comes from.
int GM_CourseInfo_GetCompilationCourse(int compilationCourse, int holeIndex) {
    return gCompilationCourses[GM_CourseInfo_MapCompilationCourse(compilationCourse)]
        .aHoles[holeIndex].nCourse;
}

// The number (1-based) on its own course of a compilation course's hole holeIndex (0..17).
int GM_CourseInfo_GetCompilationHole(int compilationCourse, int holeIndex) {
    int compCourse = GM_CourseInfo_MapCompilationCourse(compilationCourse);
    return gCompilationCourses[compCourse].aHoles[holeIndex].nHole;
}

int GM_CurrentCourseTotalParXHoles(int par) {
    int result = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (par == GM_GetHoleIndexPar(i)) {
            result++;
        }
    }
    return result;
}

int GM_CurrentCourseTotalPar4andPar5Holes(void) {
    return GM_CurrentCourseTotalParXHoles(4) + GM_CurrentCourseTotalParXHoles(5);
}

// Each course's name, by course number.
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
