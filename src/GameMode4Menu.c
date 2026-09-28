// GameMode4Menu.c (our name): the front-end messages of the map screen of game mode 4's ladder
// (GameMode4.c). They place the map's nodes, move the cursor (gLadderMap) with the rules in
// LadderMap.c, fill the panel's text for the event under it (region and tour stop, opponent,
// course, name, stage and holes) and start that event.

#include "golfer.h"
#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/ladder.h"

LadderMap gLadderMap;

// FE message 569: the map screen places node pArgs[0] at x pArgs[1], y pArgs[2] (floats); the
// cursor moves between these positions.
void LadderMenu_SetNodePos(MsgArg* pArgs, MsgArg* pResult) {
    int nNode = pArgs[0].i;
    f32 fX = pArgs[1].f;
    f32 fY = pArgs[2].f;

    gLadderMap.aNode[nNode].fX = fX;
    gLadderMap.aNode[nNode].fY = fY;
}

// FE message 573: node pArgs[0]'s x and y, into the floats pArgs[1] and pArgs[2] point to.
void LadderMenu_GetNodePos(MsgArg* pArgs, MsgArg* pResult) {
    int nNode = pArgs[0].i;
    f32* pX = (f32*)pArgs[1].p;
    f32* pY = (f32*)pArgs[2].p;

    *pX = gLadderMap.aNode[nNode].fX;
    *pY = gLadderMap.aNode[nNode].fY;
}

// The opponent's name as the ladder map shows it: the golfer's nickname in quotes when there is one
// (not "NA", two letters or more), else the last name. Golfer 18 always goes by the last name.
void LadderMenu_GetOpponentName(int nGolfer, char* szOut) {
    GolferRecord* pRecord = fn_80077A80(nGolfer);
    int bNick = strcmp(pRecord->szNick, "NA") != 0 && strlen(pRecord->szNick) > 1 && nGolfer != 18;

    if (bNick) {
        sprintf(szOut, "\"%s\"", pRecord->szNick);
    } else {
        sprintf(szOut, "%s", pRecord->szLast);
    }
}

// FE message 575: the texts of the event under the cursor, into seven strings: "<region> / Tour
// Stop <n>" (GameMode4_GetEventTourStop, the event's tour stop), the opponent
// (LadderMenu_GetOpponentName; left as it was for a golfer id above 29), the course's name (left as
// it was for an id past the last course), the event's name, its stage (gLadderStageNames), an empty
// string, and its hole set's name (gLadderHoleSetNames; left as it was for a preset above 3).
void LadderMenu_GetEventText(MsgArg* pArgs, MsgArg* pResult) {
    char* szStop = ((MsgString*)pArgs[0].p)->pStr;
    char* szOpponent = ((MsgString*)pArgs[1].p)->pStr;
    char* szCourse = ((MsgString*)pArgs[2].p)->pStr;
    char* szName = ((MsgString*)pArgs[3].p)->pStr;
    char* szStage = ((MsgString*)pArgs[4].p)->pStr;
    char* szEmpty = ((MsgString*)pArgs[5].p)->pStr;
    char* szHoles = ((MsgString*)pArgs[6].p)->pStr;
    int nStop = GameMode4_GetEventTourStop(gLadderMap.nEvent);
    int nRegion = LadderMap_GetNodeRegion(gLadderMap.nNode);
    int nGolfer;
    int nCourse;
    int nHoles;

    sprintf(szStop, "%s / Tour Stop %d", gLadderRegionNames[nRegion], nStop);
    nGolfer = GameMode4_GetEventOpponent(gLadderMap.nEvent);
    if (nGolfer <= 29) {
        LadderMenu_GetOpponentName(nGolfer, szOpponent);
    }
    nCourse = GameMode4_GetEventCourse(gLadderMap.nEvent);
    if (nCourse <= NUM_COURSES - 1) {
        strcpy(szCourse, lbl_80191990[nCourse]);
    }
    GameMode4_GetEventName(gLadderMap.nEvent, szName);
    strcpy(szStage, gLadderStageNames[LadderMap_GetEventStage(gLadderMap.nEvent)]);
    strcpy(szEmpty, "");
    nHoles = GameMode4_GetEventHoles(gLadderMap.nEvent);
    if (nHoles <= 3) {
        strcpy(szHoles, gLadderHoleSetNames[nHoles]);
    }
}

// FE message 576: moves the cursor to the nearest shown node in direction pArgs[0] (0 up, 1 down, 2
// left, 3 right), or with -1 to the node of the first event the player may play. The cursor stays
// put when no node lies that way; either way gLadderMap.nEvent becomes its node's event. Gives the
// new node, -1 for none.
void LadderMenu_MoveCursor(MsgArg* pArgs, MsgArg* pResult) {
    int nDir = pArgs[0].i;
    u8 abCandidate[NUM_LADDER_EVENTS];
    int nNode;

    memset(abCandidate, 0, sizeof(abCandidate));
    if (nDir == -1) {
        nNode = LadderMap_GetFirstPlayableNode();
    } else {
        LadderMap_MarkNodesInDirection(nDir, abCandidate);
        LadderMap_UnmarkHiddenNodes(abCandidate);
        nNode = LadderMap_FindNearestMarkedNode(abCandidate);
    }
    if (nNode != -1) {
        gLadderMap.nNode = nNode;
    }
    gLadderMap.nEvent = gLadderNodeEvents[gLadderMap.nNode];
    pResult->i = nNode;
}

// FE message 579: field n0 of the event at node pArgs[0] (GameMode4_GetEventN0: the first word of the
// event's 'TCM ' record, read nowhere else; what it holds is not known).
void LadderMenu_GetNodeEventN0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameMode4_GetEventN0(gLadderNodeEvents[pArgs[0].i]);
}

// FE message 580: node 0's x and y and the cursor node's x and y, into the four floats pArgs[0] to
// pArgs[3] point to.
void LadderMenu_GetFirstAndCursorNodePos(MsgArg* pArgs, MsgArg* pResult) {
    f32* pFirstX = (f32*)pArgs[0].p;
    f32* pFirstY = (f32*)pArgs[1].p;
    f32* pX = (f32*)pArgs[2].p;
    f32* pY = (f32*)pArgs[3].p;

    *pFirstX = gLadderMap.aNode[0].fX;
    *pFirstY = gLadderMap.aNode[0].fY;
    *pX = gLadderMap.aNode[gLadderMap.nNode].fX;
    *pY = gLadderMap.aNode[gLadderMap.nNode].fY;
}

// FE message 46: plays the event under the cursor. It becomes GameMode4's current event when the
// profile may play it (otherwise the current event stays as it was), player 0 plays the created
// golfer, and GameMode4_StartEvent sets the session up.
void LadderMenu_StartEvent(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_SelectEvent(fn_80077B08(), gLadderMap.nEvent);
    gSession.nGolfer[0] = FIRST_CREATED_GOLFER;
    GameMode4_StartEvent();
}

// FE message 596: the direction from point (pArgs[0], pArgs[1]) to point (pArgs[2], pArgs[3]) in
// degrees, -180 to 180: 0 straight up, growing clockwise (90 right; y grows downwards).
void LadderMenu_GetAngleBetween(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = (180.0f / PI) * atan2f(pArgs[2].f - pArgs[0].f, -(pArgs[3].f - pArgs[1].f));
}

// FE message 691: node pArgs[0]'s state (LadderMap_GetNodeState: -1 not shown, 0 open, 1 won, 2
// locked).
void LadderMenu_GetNodeState(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = LadderMap_GetNodeState(pArgs[0].i);
}

// FE message 720: ladder event pArgs[0]'s name (an event, not a node), into the string pArgs[1].
void LadderMenu_GetEventName(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_GetEventName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}
