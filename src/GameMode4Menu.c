// GameMode4Menu.c (our name): the front end's map of game mode 4's ladder (GameMode4.c). It keeps
// the map's nodes and the cursor (lbl_80260CB8), moves the cursor between the events the player
// may see, and fills the menus' text for the event under it: region and tour stop, opponent,
// course, name and holes.

#include "golfer.h"
#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/ladder.h"

int fn_80121A38(int nNode);
int fn_80121B8C(int nNode);
int fn_80121C08(int nNode);
int fn_80121C44(int nEvent);
void fn_80121C80(int nDir, u8* abCandidate);
int fn_80121E1C(u8* abCandidate);
int fn_80122018(void);
void fn_80122070(u8* abCandidate);

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
    nGolfer = fn_801020EC(lbl_80260CB8.nEvent);
    if (nGolfer <= 29) {
        fn_80121488(nGolfer, szOpponent);
    }
    nCourse = fn_80102104(lbl_80260CB8.nEvent);
    if (nCourse <= NUM_COURSES - 1) {
        strcpy(szCourse, lbl_80191990[nCourse]);
    }
    fn_80102A58(lbl_80260CB8.nEvent, szName);
    strcpy(szPart, lbl_80194730[fn_80121C44(lbl_80260CB8.nEvent)]);
    strcpy(szEmpty, "");
    nHoles = fn_8010211C(lbl_80260CB8.nEvent);
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
    fn_801022BC(fn_80077B08(), lbl_80260CB8.nEvent);
    gSession.nGolfer[0] = FIRST_CREATED_GOLFER;
    fn_80102468();
}

// The angle, in degrees, from one point to another (x0, y0, x1, y1; y grows downwards).
void fn_80121808(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = (180.0f / PI) * fn_8000AD78(pArgs[2].f - pArgs[0].f, -(pArgs[3].f - pArgs[1].f));
}

void fn_8012185C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80121A38(pArgs[0].i);
}

void fn_80121890(MsgArg* pArgs, MsgArg* pResult) {
    fn_80102A58(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Nodes 18 to 23 stand alone.
u8 fn_801218BC(int nNode) {
    int b = 0;

    if (nNode >= 18 && nNode < 24) {
        b = 1;
    }
    return b;
}

// Node 24 is the final.
u8 fn_801218DC(int nNode) {
    return nNode == 24;
}

u8 fn_801218EC(int nRegion) {
    return nRegion == 6;
}

// Has the player won every event of the region?
u8 fn_801218FC(int nRegion) {
    int nProfile = fn_80077B08();

    if (fn_801218EC(nRegion)) {
        return fn_80102204(nProfile, 24);
    }
    return fn_80102204(nProfile, lbl_801946B0[nRegion * 3]) &&
           fn_80102204(nProfile, lbl_801946B0[nRegion * 3 + 1]) &&
           fn_80102204(nProfile, lbl_801946B0[nRegion * 3 + 2]);
}

// Has the player won every event but the final?
u8 fn_801219CC(void) {
    int nProfile;
    int i;
    u8 bWon = 1;

    nProfile = fn_80077B08();
    for (i = 0; i < 24; i++) {
        if (!fn_80102204(nProfile, i)) {
            bWon = 0;
        }
    }
    return bWon;
}

// A node's state: -1 not shown, 0 open, 1 won, 2 locked.
int fn_80121A38(int nNode) {
    int nProfile;
    int nEvent = lbl_801946B0[nNode];

    nProfile = fn_80077B08();
    if (!fn_80121B8C(nNode)) return -1;
    if (fn_801218DC(nNode)) {
        if (fn_80102204(nProfile, nEvent)) return 1;
        return fn_80102228(nProfile, nEvent) ? 0 : 2;
    }
    if (fn_801218BC(nNode)) {
        if (fn_80102204(nProfile, nEvent)) return 1;
        return fn_80102228(nProfile, nEvent) ? 0 : 2;
    }
    if (fn_80102204(nProfile, nEvent)) return 1;
    return fn_80102228(nProfile, nEvent) ? 0 : 2;
}

// Is the node shown? The final once every other event is won, the single nodes once their region
// is won, the regions' own nodes until it is.
int fn_80121B8C(int nNode) {
    u8 bRegionWon = fn_801218FC(fn_80121C08(nNode));

    if (fn_801218DC(nNode)) {
        return fn_801219CC();
    }
    if (fn_801218BC(nNode)) {
        return bRegionWon;
    }
    return !bRegionWon;
}

// A node's region: three nodes each up to node 17, then one each.
int fn_80121C08(int nNode) {
    if (nNode < 18) {
        return nNode / 3;
    }
    return nNode <= 24 ? nNode - 18 : 0;
}

int fn_80121C44(int nEvent) {
    if (nEvent < 24) {
        return nEvent % 4;
    }
    return nEvent == 24 ? 4 : 0;
}

// Marks the nodes that lie in the direction from the cursor's node.
void fn_80121C80(int nDir, u8* abCandidate) {
    f32 fDX;
    f32 fDY;
    f32 fX = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fX;
    f32 fY = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fY;
    int i;
    u8 b;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (i != lbl_80260CB8.nNode) {
            fDX = lbl_80260CB8.aNode[i].fX - fX;
            fDY = lbl_80260CB8.aNode[i].fY - fY;
            switch (nDir) {
            case 0:
                b = 0;
                if (fDY < 0.0f && -fDY > fabsf(fDX)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 1:
                b = 0;
                if (fDY > 0.0f && fDY > fabsf(fDX)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 2:
                b = 0;
                if (fDX < 0.0f && -fDX > fabsf(fDY)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 3:
                b = 0;
                if (fDX > 0.0f && fDX > fabsf(fDY)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            }
        }
    }
}

// The marked node nearest the cursor's node, -1 for none.
int fn_80121E1C(u8* abCandidate) {
    f32 fX = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fX;
    f32 fY = lbl_80260CB8.aNode[lbl_80260CB8.nNode].fY;
    f32 fBest = 3.4028235e38f;
    f32 fDX;
    f32 fDY;
    f32 fDist;
    int i;
    int nBest = -1;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (abCandidate[i]) {
            fDX = fX - lbl_80260CB8.aNode[i].fX;
            fDY = fY - lbl_80260CB8.aNode[i].fY;
            fDX *= fDX;
            fDY *= fDY;
            fDist = fn_80009680(fDX + fDY);
            if (fDist < fBest) {
                nBest = i;
                fBest = fDist;
            }
        }
    }
    return nBest;
}

// The event is open to the player and not won yet.
u8 fn_80121F0C(int nEvent) {
    int b;
    int nProfile = fn_80077B08();

    b = 0;
    if (!fn_80102204(nProfile, nEvent) && fn_80102228(nProfile, nEvent)) {
        b = 1;
    }
    return b;
}

// The event's node, -1 for none.
int fn_80121F7C(int nEvent) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (nEvent == lbl_801946B0[i]) return i;
    }
    return -1;
}

// The node of the first event the player can play, else the final's.
int fn_80122018(void) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (fn_80121F0C(i)) {
            return fn_80121F7C(i);
        }
    }
    return fn_80121F7C(24);
}

// Unmarks the nodes that are not shown.
void fn_80122070(u8* abCandidate) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (!fn_80121B8C(i)) {
            abCandidate[i] = 0;
        }
    }
}
