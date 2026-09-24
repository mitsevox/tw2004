// LadderMap.c (our name): the rules of game mode 4's ladder map (GameMode4.c), split from
// GameMode4Menu.c, which holds the front end's messages. It knows the map's regions and nodes,
// which nodes are shown, won or locked, and finds the node the cursor moves to.

#include "engine.h"
#include "frontend/fe.h"
#include "game/modes/ladder.h"

int fn_80121B8C(int nNode);

char* lbl_80194694[7] = {
    "US Northwest", "US Southwest", "US East", "Europe", "Pacific", "Southern Hemisphere", "World",
};

s32 lbl_801946B0[NUM_LADDER_EVENTS] = {
    1, 0, 2, 5, 6, 4, 8, 10, 9, 13, 12, 14, 18, 16, 17, 20, 21, 22, 3, 7, 11, 15, 19, 23, 24,
};

char* lbl_80194714[4] = { "None", "All", "Front 9", "Back 9" };

char* lbl_80194730[6] = { "1/4", "2/4", "3/4", "Dominated", "World", NULL };

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
