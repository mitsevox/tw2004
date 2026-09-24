// GameMode4Menu.c (our name): the front end's messages for the map of game mode 4's ladder
// (GameMode4.c). They place the map's nodes, move the cursor (lbl_80260CB8) with the rules in
// LadderMap.c, and fill the menus' text for the event under it: region and tour stop, opponent,
// course, name and holes.

#include "golfer.h"
#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/ladder.h"

// Places a node: node, x, y.
void fn_80121430(MsgArg* pArgs, MsgArg* pResult) {
    int nNode = pArgs[0].i;
    f32 fX = pArgs[1].f;
    f32 fY = pArgs[2].f;

    lbl_80260CB8.aNode[nNode].fX = fX;
    lbl_80260CB8.aNode[nNode].fY = fY;
}

// Where a node is.
void fn_80121458(MsgArg* pArgs, MsgArg* pResult) {
    int nNode = pArgs[0].i;
    f32* pX = (f32*)pArgs[1].p;
    f32* pY = (f32*)pArgs[2].p;

    *pX = lbl_80260CB8.aNode[nNode].fX;
    *pY = lbl_80260CB8.aNode[nNode].fY;
}

// The opponent's nickname in quotes, or the last name when there is none (golfer 18 always
// goes by the last name).
void fn_80121488(int nGolfer, char* szOut) {
    GolferRecord* pRecord = fn_80077A80(nGolfer);
    int bNick = strcmp(pRecord->szNick, "NA") != 0 && strlen(pRecord->szNick) > 1 && nGolfer != 18;

    if (bNick) {
        sprintf(szOut, "\"%s\"", pRecord->szNick);
    } else {
        sprintf(szOut, "%s", pRecord->szLast);
    }
}

// The text for the event under the cursor.
void fn_8012153C(MsgArg* pArgs, MsgArg* pResult) {
    char* szStop = ((MsgString*)pArgs[0].p)->pStr;
    char* szOpponent = ((MsgString*)pArgs[1].p)->pStr;
    char* szCourse = ((MsgString*)pArgs[2].p)->pStr;
    char* szName = ((MsgString*)pArgs[3].p)->pStr;
    char* szPart = ((MsgString*)pArgs[4].p)->pStr;
    char* szEmpty = ((MsgString*)pArgs[5].p)->pStr;
    char* szHoles = ((MsgString*)pArgs[6].p)->pStr;
    int nStop = fn_80102AAC(lbl_80260CB8.nEvent);
    int nRegion = fn_80121C08(lbl_80260CB8.nNode);
    int nGolfer;
    int nCourse;
    int nHoles;

    sprintf(szStop, "%s / Tour Stop %d", lbl_80194694[nRegion], nStop);
    nGolfer = GameMode4_GetEventOpponent(lbl_80260CB8.nEvent);
    if (nGolfer <= 29) {
        fn_80121488(nGolfer, szOpponent);
    }
    nCourse = GameMode4_GetEventCourse_80102104(lbl_80260CB8.nEvent);
    if (nCourse <= NUM_COURSES - 1) {
        strcpy(szCourse, lbl_80191990[nCourse]);
    }
    GameMode4_GetEventName(lbl_80260CB8.nEvent, szName);
    strcpy(szPart, lbl_80194730[fn_80121C44(lbl_80260CB8.nEvent)]);
    strcpy(szEmpty, "");
    nHoles = GameMode4_GetEventHoles_8010211C(lbl_80260CB8.nEvent);
    if (nHoles <= 3) {
        strcpy(szHoles, lbl_80194714[nHoles]);
    }
}

// Moves the cursor: direction 0 up, 1 down, 2 left, 3 right, or -1 to the first event the player
// can play. Gives the new node, -1 when there is none that way.
void fn_8012168C(MsgArg* pArgs, MsgArg* pResult) {
    int nDir = pArgs[0].i;
    u8 abCandidate[NUM_LADDER_EVENTS];
    int nNode;

    memset(abCandidate, 0, sizeof(abCandidate));
    if (nDir == -1) {
        nNode = fn_80122018();
    } else {
        fn_80121C80(nDir, abCandidate);
        fn_80122070(abCandidate);
        nNode = fn_80121E1C(abCandidate);
    }
    if (nNode != -1) {
        lbl_80260CB8.nNode = nNode;
    }
    lbl_80260CB8.nEvent = lbl_801946B0[lbl_80260CB8.nNode];
    pResult->i = nNode;
}

void fn_8012172C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80102A44(lbl_801946B0[pArgs[0].i]);
}

// Where the first node and the cursor's node are.
void fn_80121770(MsgArg* pArgs, MsgArg* pResult) {
    f32* pFirstX = (f32*)pArgs[0].p;
    f32* pFirstY = (f32*)pArgs[1].p;
    f32* pX = (f32*)pArgs[2].p;
    f32* pY = (f32*)pArgs[3].p;

    *pFirstX = lbl_80260CB8.aNode[0].fX;
    *pFirstY = lbl_80260CB8.aNode[0].fY;
    *pX = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fX;
    *pY = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fY;
}

// Starts the event under the cursor, with the player on the created golfer.
void fn_801217C4(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_SelectEvent(fn_80077B08(), lbl_80260CB8.nEvent);
    gSession.nGolfer[0] = FIRST_CREATED_GOLFER;
    GameMode4_StartEvent();
}

// The angle, in degrees, from one point to another (x0, y0, x1, y1; y grows downwards).
void fn_80121808(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = (180.0f / PI) * fn_8000AD78(pArgs[2].f - pArgs[0].f, -(pArgs[3].f - pArgs[1].f));
}

void fn_8012185C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80121A38(pArgs[0].i);
}

void fn_80121890(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_GetEventName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}
