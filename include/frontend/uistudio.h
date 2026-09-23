#ifndef FRONTEND_UISTUDIO_H
#define FRONTEND_UISTUDIO_H

// EA's UI Studio library (0x80165528-0x8016C718; EA's file names UISEvent.c and UIStudio.c come
// from its asserts): the runtime the menu screens are built on. It keeps the loaded screens, a
// stack of pending events, and "rate functions" that move a screen variable towards a target
// over a set time. The front end reaches it through FrontEnd.pHandler (game/frontend.h).

#include "game_types.h"
#include "platform.h"

// Where the library reports problems: a level (0 warning, 1 error, as the calls use it), the
// source file, the line and the message. Set by fn_80165C6C.
typedef void (*UISReportFn)(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);

extern UISReportFn lbl_80282A28;

// Four floats, copied as one (UISScreen.c).
typedef struct UISVec4 {
    f32 a[4];
} UISVec4;

// The values screen nodes are drawn with (UISScreen.c): each node adds its UISNodeInfo.afAdd to
// the first and multiplies its afMul into the second for its children.
extern UISVec4 lbl_80280628;
extern UISVec4 lbl_80280638;

// A text buffer the studio formats into and reads from (fn_8016B808, fn_8016B844).
typedef struct UISText {
    u32 u0;                         // 0x00
    s32 nSize;                      // 0x04: the buffer's size
    char* szText;                   // 0x08
} UISText;

// A word of a call's arguments: fn_8016B844 reads them by its format string.
typedef union UISWord {
    s32 n;
    u32 u;
    f32 f;
    UISText* pText;
} UISWord;

// The callbacks the game hands the studio (uiProcessInterface.c sets them). Parameters come from
// the studio's calls, or where noted from the game's own callback.
typedef void (*UISCommandFn)(s32 nCmd, s32 n1, s32 n2, s32 n3, s32 n4, s32 n5);  // the game's fn_8008F568
typedef u32 (*UISLoadFn)(void* p, u16 u);                       // the game's fn_8008F610
typedef void (*UISUnloadFn)(u16 uGroup, u16 uScreen, void* pData);
typedef void (*UISTransformFn)(int nOp, void* pDesc);           // the game's fn_80093280
typedef void (*UISScreenFn)(u16 uGroup, u16 uScreen);
typedef void (*UISScreenDataFn)(u16 uGroup, u16 uScreen, s32 n);
// The table at UIStudio.ppfnHandlers: pVar is the handler's variable in the screen file.
typedef void (*UISHandlerFn)(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);

// A handler a screen node has for an event (8 bytes; UISScreen.c looks them up).
typedef struct UISHandler {
    u16 uFlags;                     // 0x00: 0x8000 and 0x4000 mark two kinds of handler; with
                                    //       neither, the bits under 0x2FFF are an ID
    u16 uEvent;                     // 0x02: the event it handles
    u32 u4;                         // 0x04: what a lookup returns (0: none)
} UISHandler;

// A node's drawing values (UISScreen.c). fn_8016A510 multiplies afMul into the studio's
// lbl_80280638 and adds afAdd to lbl_80280628 while it draws the node's children; a rate
// function drives one of the floats (fn_8016C1A4 picks it).
typedef struct UISNodeInfo {
    void* p0;                       // 0x00: the node or group that owns it (fn_8016AD54 records it);
                                    //       fn_8016A510 draws only nodes whose info has one
    u32 u4;                         // 0x04: fn_8016A2D4 hands it back; fn_8016B6BC looks for a set one
    f32 af8[12];                    // 0x08: UIStudio.pfnTransform gets a pointer to these
    u8 unk38[4];
    f32 afMul[4];                   // 0x3C
    f32 afAdd[4];                   // 0x4C
    u8 unk5C[4];
    u32 u60;                        // 0x60
} UISNodeInfo;

// An entry of a node's group or of a screen file's start list (8 bytes): a call to one of the
// studio's handlers with a variable of the screen file, or a link to another node.
typedef struct UISEntry {
    u16 uHandler;                   // 0x00: index into UIStudio.ppfnHandlers; 0xFFFF: a link to node u4.nNode
    s16 n2;                         // 0x02: whether the handler has run (fn_8016AEEC)
    union {
        u32 nNode;                  //       the linked node
        u32* pnOffset;              //       the handler's variable, as an offset into the screen file
    } u4;                           // 0x04
} UISEntry;
LAYOUT_ASSERT(UISEntry, 8);

// A group of entries in a node. It starts like a node: fn_8016AD54 looks for an info pointer
// in both.
typedef struct UISGroup {
    UISNodeInfo* pInfo;             // 0x00: a group whose info has no owner is skipped
    u32 nEntries;                   // 0x04
    UISEntry* pEntries;             // 0x08
} UISGroup;

// A node of a loaded screen (0x14 bytes).
typedef struct UISNode {
    UISNodeInfo* pInfo;             // 0x00
    u32 nGroups;                    // 0x04
    UISGroup** ppGroups;            // 0x08
    u32 nHandlers;                  // 0x0C
    UISHandler* pHandlers;          // 0x10: a handler with uEvent 0xFFFF links to node u4
} UISNode;
LAYOUT_ASSERT(UISNode, 0x14);

// A loaded screen file (what the load callback returns): its nodes, node 0 first, and a list of
// entries run before the nodes.
typedef struct UISScreenFile {
    u32 nNodes;                     // 0x00
    UISNode* pNodes;                // 0x04
    u8 unk8[0x10];
    u32 nStart;                     // 0x18
    UISEntry* pStart;               // 0x1C
} UISScreenFile;

// A loaded screen (0x14 bytes, in the studio's screen table).
typedef struct UISScreen {
    u32 uMask;                      // 0x00: one bit per event kind the screen has already taken
    u16 uGroup;                     // 0x04: the screen's group ID
    u16 uScreen;                    // 0x06: its ID within the group
    u8 unk8[4];
    s32 bUnloading;                 // 0x0C: set while the screen waits to be unloaded
    UISScreenFile* pData;           // 0x10: what the load callback returned; the unload callback gets it
} UISScreen;
LAYOUT_ASSERT(UISScreen, 0x14);

// The studio's "current" record, set up by fn_80169C0C (0x14 bytes, after the tables).
typedef struct UISCurrent {
    s32 n0;                         // 0x00: -1 when empty
    u16 u4;                         // 0x04: 0xFFFF when empty
    u16 u6;                         // 0x06
    u16 u8;                         // 0x08
    u16 uA;                         // 0x0A
    s32 nC;                         // 0x0C
    void* p10;                      // 0x10: a loaded UI file (fn_80169520)
} UISCurrent;
LAYOUT_ASSERT(UISCurrent, 0x14);

// The data words of an event (16 bytes). A screen event fills only aw[0] (group) and aw[1]
// (screen); fn_80165B90 copies all of it.
typedef union UISEventData {
    u32 au[4];
    u16 aw[8];
} UISEventData;

// An event on the studio's event stack (0x24 bytes). Its type is the stack's top word; its
// arguments lie below it, the first one lowest.
typedef struct UISEvent {
    s32 nArgs;                      // 0x00
    UISEventData data;              // 0x04
    u8 unk14[4];
    s32 nB;                         // 0x18
    s32 nA;                         // 0x1C
    s32 nType;                      // 0x20
} UISEvent;
LAYOUT_ASSERT(UISEvent, 0x24);

// A rate function: moves one variable of a screen towards a target, a step every tick.
typedef struct UISRateFn {
    u32 uId;                        // 0x00: the rate function's ID
    s32 n4;                         // 0x04
    s32 n8;                         // 0x08
    s32 nC;                         // 0x0C
    u32 u10;                        // 0x10: the studio's uMsPerTick when the function was loaded
    u32 uState;                     // 0x14: 0 new, 1 finished (removed by fn_80165C74), 2 running
    u32 u18;                        // 0x18: with uId, what a rate function is looked up by
    UISScreen* pScreen;             // 0x1C: the screen it belongs to
    s32 n20;                        // 0x20: which of pInfo's floats it drives (fn_8016C1A4)
    f32 fTarget;                    // 0x24: the value it moves towards
    f32 fStep;                      // 0x28: the change per tick
    s32 n2C;                        // 0x2C
    union {
        s32 n30;                    // 0x30: as fn_80165E9C stores it
        UISNodeInfo* pInfo;         //       the node the variable belongs to
    };
} UISRateFn;
LAYOUT_ASSERT(UISRateFn, 0x34);

// A 0x28-byte record in UIStudio.p60; the last one in use names the current screen.
typedef struct UISRecord60 {
    u8 unk0[0x18];
    UISScreen* pScreen;             // 0x18: fn_8016B4D4 keeps it pointing at the same screen
    u8 unk1C[8];
    u16 u24;                        // 0x24: a screen ID (fn_8016C6C4's third argument)
    u16 u26;                        // 0x26: a group ID (its second)
} UISRecord60;
LAYOUT_ASSERT(UISRecord60, 0x28);

// A block of words the studio hands out (fn_80169C0C sets p0, p4 and pC to its start).
typedef struct UISWordStack {
    s32* p0;                        // 0x00: the start
    s32* p4;                        // 0x04
    s32* p8;                        // 0x08: the end
    s32* pC;                        // 0x0C: the next free word (fn_8016C270 pushes a call's words here)
    u32 u10;                        // 0x10: the handler of the call fn_8016C270 runs
} UISWordStack;
LAYOUT_ASSERT(UISWordStack, 0x14);

#define UIS_MAGIC 0x5549535F        // "UIS_", while the studio is set up

// The studio (0xBC bytes). fn_80169C0C builds it in one block: this header, then the screen
// table, the handler table, the rate functions, the 0x28-byte records, the current record and
// the two word stacks. fn_80169D90 gives the block's size.
typedef struct UIStudio {
    u32 uMagic;                     // 0x00: UIS_MAGIC, cleared when every screen is unloaded
    u32 uFlags;                     // 0x04: bit 0 while the event stack runs, bit 1 while an event
                                    //       is being sent (new events are queued instead)
    u32 uMsPerTick;                 // 0x08: a tick's length in ms (the game passes 16)
    UISCommandFn pfnCommand;        // 0x0C: fn_80169B44
    s32 n10;                        // 0x10
    UISLoadFn pfnLoad;              // 0x14: fn_80169B30
    UISUnloadFn pfnUnload;          // 0x18: fn_80169B30
    UISTransformFn pfnTransform;    // 0x1C: fn_80169B28
    s32 n20;                        // 0x20
    UISScreenFn pfnScreen24;        // 0x24
    UISScreenDataFn pfnScreen28;    // 0x28: fn_80169B3C
    s32 nCurScreen;                 // 0x2C: index into pScreens, -1 for none
    u32 nMaxScreens;                // 0x30
    u32 nScreens;                   // 0x34
    UISScreen* pScreens;            // 0x38
    UISCurrent* pCurrent;           // 0x3C
    u32 nMaxHandlers;               // 0x40
    u32 nHandlers;                  // 0x44
    UISHandlerFn* ppfnHandlers;     // 0x48: filled by fn_80169B0C
    u32 nMaxRateFns;                // 0x4C
    s32 nRateFns;                   // 0x50: rate functions in use
    UISRateFn* pRateFns;            // 0x54
    u32 nMax60;                     // 0x58
    s32 n5C;                        // 0x5C: records in use in p60
    UISRecord60* p60;               // 0x60
    UISWordStack stack64;           // 0x64: the event words; the event stack grows down from its end
    UISWordStack stack78;           // 0x78: a second block of words
    u8 unk8C[0x20];
    s32* pEventBase;                // 0xAC: the bottom of the event stack; it grows down from here
    s32* pEventTop;                 // 0xB0: the stack's current top (fn_80165B90)
    s32** pp68;                     // 0xB4: points at stack64.p4
    s32 bUnloadingAll;              // 0xB8: set while fn_80169B4C unloads every screen
} UIStudio;
LAYOUT_ASSERT(UIStudio, 0xBC);

// UISEvent.c
void fn_80165528(UIStudio* pStudio, u8 b);
void fn_80165B90(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, const UISEventData* pData, s32 nArgs,
                 const s32* pArgs);
void fn_80165C6C(UISReportFn pfnReport);
void fn_80165C74(UIStudio* pStudio);
void fn_80165D2C(UIStudio* pStudio, u32 u18, u32 uId);
void fn_80165D90(UIStudio* pStudio, UISScreen* pScreen, u32 u18, u32 uId, s32 n4, u32 u10);
void fn_80165E9C(UIStudio* pStudio, UISScreen* pScreen, u32 u18, s32 n30, u32 uId, s32 n2C, s32 n4,
                 u32 uTime, f32 fTarget, s32 n20);
u32 fn_8016604C(UIStudio* pStudio, u32 u18, u32 uId);

// UIStudio.c
void fn_80168B80(UIStudio* pStudio, u32 uEvent);

// UISApi.c
void fn_80168C24(UIStudio* pStudio, s32 nTicks);
void fn_80168CD8(UIStudio* pStudio, UISWordStack* pStack, u32 uEvent, s32 n, u8 b, void* p, u8 bAll);
void fn_80168DB0(UIStudio* pStudio, u32 uEvent, s32 n, u8 b, void* p, u8 bAll);
void fn_80168EE8(UIStudio* pStudio, u16* puGroup, u16* puScreen);
void fn_80168F5C(UIStudio* pStudio, s16 nGroup, s16 nScreen);
s32 fn_801694A0(UIStudio* pStudio, s16 nGroup, s16 nScreen, u8 nArgs, s32* pArgs);
u8 fn_80169520(UIStudio* pStudio, void* pFile);
s32 fn_80169590(UIStudio* pStudio, s16 nGroup, s16 nScreen, s32 n, u8 nArgs, s32* pArgs);
void fn_80169B0C(UIStudio* pStudio, s32 nIndex, UISHandlerFn pfnHandler);
void fn_80169B28(UIStudio* pStudio, UISTransformFn pfnTransform);
void fn_80169B30(UIStudio* pStudio, UISLoadFn pfnLoad, UISUnloadFn pfnUnload);
void fn_80169B3C(UIStudio* pStudio, UISScreenDataFn pfnScreen28);
void fn_80169B44(UIStudio* pStudio, UISCommandFn pfnCommand);
void fn_80169B4C(UIStudio* pStudio);
void fn_80169C0C(UIStudio* pStudio, u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords,
                 u32 nWords2, u32 uMsPerTick);
u32 fn_80169D90(u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords, u32 nWords2);
u8 fn_80169DC4(void* pFile);
void fn_8016A030(UIStudio* pStudio, u32 uMs);

// UIStudio.c: runs a handler's call frame (fn_8016C270 pushes it).
s8 fn_80166098(UIStudio* pStudio, s32* pFrame, UISWordStack* pStack, UISScreen* pScreen, UISNodeInfo* pInfo);

// UISScreen.c (0x8016A2D4-0x8016C718)
// Sends event uEvent to node nNode of a screen and the nodes it links to; *pbOut gets the node's
// UISNodeInfo.u4.
s32 fn_8016A2D4(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent, u32 n5,
                u8 nArgs, const s32* pArgs, u8* pbOut);
void fn_8016A510(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg);
void fn_8016A830(UIStudio* pStudio, s32 nOp, UISScreen* pScreen, u32 nNode);
void fn_8016ABBC(UIStudio* pStudio, UISScreen* pScreen, s32 n, s32 nKind, void* p, u8 bAll);
s32 fn_8016AD54(UISScreen* pScreen, UISNodeInfo* pInfo, s32 nKind, void* p);
void fn_8016AEEC(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg);
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs);
void fn_8016B4D4(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 nMove);
UISNodeInfo* fn_8016B6BC(UISScreen* pScreen, UISNodeInfo* pInfo);
void fn_8016B808(u32 u0, UISText* pOut, UISText* pFormat, const UISWord* pArgs);
s32 fn_8016B844(char* pOut, s32 nSize, const char* szFormat, const UISWord* pArgs);
void fn_8016C15C(f32 f1, f32 f2, f32 f3, f32 f4);
void fn_8016C174(f32 f1, f32 f2, f32 f3, f32 f4);
UISVec4* fn_8016C18C(void);
UISVec4* fn_8016C198(void);
// Returns a pointer to the float of pInfo that a rate function's n20 names.
f32* fn_8016C1A4(s32 n20, UISNodeInfo* pInfo);
s8 fn_8016C270(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pInfo, UISWordStack* pStack, u32 uHandler,
               u32 nArgs, const s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra, s32* pnSaved);
u32 fn_8016C5C4(UISNode* pNode, u16 uEvent);
u32 fn_8016C614(UISNode* pNode, u16 uId, u16 uEvent);
u32 fn_8016C674(UISNode* pNode, u16 uEvent);
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen);

#endif
