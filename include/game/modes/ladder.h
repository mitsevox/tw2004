// ladder.h (our name): game mode 4's ladder of 25 events (GameMode4.c) and the map screen the
// player picks them on (GameMode4Menu.c, LadderMap.c).

#ifndef GAME_MODES_LADDER_H
#define GAME_MODES_LADDER_H

#include "game_types.h"
#include "platform.h"

#define NUM_LADDER_EVENTS 25

// GameMode4.c
int  fn_801020EC(int nEvent);                   // the event's opponent (a golfer id)
int  fn_80102104(int nEvent);                   // its course
int  fn_8010211C(int nEvent);                   // its hole-selection preset
u8   fn_80102204(int nProfile, int nEvent);     // the profile has won it
u8   fn_80102228(int nProfile, int nEvent);     // the profile has won every event it needs
u8   fn_801022BC(int nProfile, int nEvent);     // makes it the current event if it may be played
void fn_80102468(void);                         // sets the session up for the current event
int  fn_80102A44(int nEvent);
void fn_80102A58(int nEvent, char* szOut);      // its name
int  fn_80102AAC(int nEvent);                   // its tour stop number

// GameMode4Menu.c and LadderMap.c: the map. Each event has a node on it; the menus place the
// nodes, and the cursor jumps to the nearest node in the direction pressed. The first 18 nodes are
// six regions of three, then six single nodes and the final one.
typedef struct LadderMapNode {
    f32 fX;                     // 0x0
    f32 fY;                     // 0x4
} LadderMapNode;

typedef struct LadderMap {
    s32 nNode;                  // 0x00  the node under the cursor
    s32 nEvent;                 // 0x04  its event
    LadderMapNode aNode[NUM_LADDER_EVENTS];     // 0x08
} LadderMap;
LAYOUT_ASSERT(LadderMap, 0xD0);

extern LadderMap lbl_80260CB8;
extern s32 lbl_801946B0[NUM_LADDER_EVENTS];     // the event at each node
extern char* lbl_80194694[7];   // each region's name
extern char* lbl_80194714[4];   // each hole-selection preset's name ("All", "Front 9", ...)
extern char* lbl_80194730[6];   // what fn_80121C44 picks: "1/4", "2/4", "3/4", "Dominated", "World"

// LadderMap.c
int  fn_80121A38(int nNode);                    // the node's state: -1 not shown, 0 open, 1 won, 2 locked
int  fn_80121C08(int nNode);                    // the node's region
int  fn_80121C44(int nEvent);
void fn_80121C80(int nDir, u8* abCandidate);    // marks the nodes in that direction
int  fn_80121E1C(u8* abCandidate);              // the marked node nearest the cursor's
int  fn_80122018(void);                         // the first playable event's node
void fn_80122070(u8* abCandidate);              // unmarks the nodes not shown

#endif
