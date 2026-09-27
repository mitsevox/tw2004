#ifndef FRONTEND_UISTUDIO_H
#define FRONTEND_UISTUDIO_H

// EA's UI Studio library (0x80165528-0x8016C718; EA's file names UISEvent.c and UIStudio.c come
// from its asserts): the runtime the menu screens are built on. It keeps the loaded screens, a
// stack of pending events, and "rate functions" that move a screen variable towards a target
// over a set time. The front end reaches it through FrontEnd.pHandler (game/frontend.h).

#include "game_types.h"
#include "platform.h"
#include "frontend/uisvec.h"

// Where the library reports problems: a level (0 warning, 1 error, as the calls use it), the
// source file, the line and the message. Set by UISRegisterRuntimeErrorFnc.
typedef void (*UISReportFn)(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);

extern UISReportFn RuntimeErrorFnc;

// A text buffer the studio formats into and reads from (UISStringFormat, UISSprintf).
typedef struct UISText {
    u32 u0;                         // 0x00
    s32 nSize;                      // 0x04: the buffer's size
    char* szText;                   // 0x08
} UISText;

// A word of a call's arguments: UISSprintf reads them by its format string.
typedef union UISWord {
    s32 n;
    u32 u;
    f32 f;
    UISText* pText;
} UISWord;

// The callbacks the game hands the studio (uiProcessInterface.c sets them). Parameters come from
// the studio's calls, or where noted from the game's own callback.
typedef void (*UISCommandFn)(s32 nCmd, s32 n1, s32 n2, s32 n3, s32 n4, s32 n5);  // the game's fn_8008F568
// Returns the screen's UI file, still unfixed (the game's fn_8008F610, which ignores the group).
typedef void* (*UISLoadFn)(u16 uGroup, u16 uScreen);
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
    u16 uEvent;                     // 0x02: the event it handles; 0xFFFF: a link to node u4.nNode
    union {
        u8* pScript;                //       the script it runs (UISExecuteFnc starts the interpreter on it)
        u32 nNode;                  //       the linked node
    } u4;                           // 0x04
} UISHandler;

// A node's drawing values (UISScreen.c). _ParseObjects multiplies afMul into the studio's
// _MultiplerColorFactor and adds afAdd to _AdditiveColorFactor while it draws the node's children; a rate
// function drives one of the floats (UISGetActionPtrValue picks it).
typedef struct UISNodeInfo {
    void* p0;                       // 0x00: the node or group that owns it (_DetermineVisibility records it);
                                    //       _ParseObjects draws only nodes whose info has one
    u32 u4;                         // 0x04: _ParseMaps hands it back; UISFindSiblingEnableControl looks for a set one
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
    s16 n2;                         // 0x02: whether the handler has run (_ParseInitialize)
    union {
        u32 nNode;                  //       the linked node
        u32* pnOffset;              //       the handler's variable, as an offset into the screen file
    } u4;                           // 0x04
} UISEntry;
LAYOUT_ASSERT(UISEntry, 8);

// A group of entries in a node. It starts like a node: _DetermineVisibility looks for an info pointer
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

// A 12-byte entry of a screen file's third table.
typedef struct UISFileEntryC {
    u32 u0;                         // 0x00
    u32 u4;                         // 0x04
    void* p8;                       // 0x08
} UISFileEntryC;
LAYOUT_ASSERT(UISFileEntryC, 0xC);

// A loaded screen file (what the load callback returns; also the studio's current file): its
// nodes, node 0 first, and a list of entries run before the nodes. Its pointer fields hold offsets
// from the file's start until PatchScrData fixes them up.
// port: the file is laid out for a 32-bit machine; a port must read it through a loader instead.
typedef struct UISScreenFile {
    u32 nNodes;                     // 0x00
    UISNode* pNodes;                // 0x04
    u32 nEntriesC;                  // 0x08
    UISFileEntryC* pEntriesC;       // 0x0C
    u32 nLinks;                     // 0x10
    u32* pLinks;                    // 0x14: file offsets of words that name a pEntriesC entry
    u32 nStart;                     // 0x18
    UISEntry* pStart;               // 0x1C
} UISScreenFile;

// A loaded screen (0x14 bytes, in the studio's screen table).
typedef struct UISScreen {
    u32 uMask;                      // 0x00: one bit per event kind the screen has already taken
    u16 uGroup;                     // 0x04: the screen's group ID
    u16 uScreen;                    // 0x06: its ID within the group
    u16 uPrevGroup;                 // 0x08: with uPrevScreen, the screen made current when this
    u16 uPrevScreen;                // 0x0A: one is unloaded (UISInternalUnloadScreen)
    s32 bUnloading;                 // 0x0C: set while the screen waits to be unloaded
    UISScreenFile* pData;           // 0x10: what the load callback returned; the unload callback gets it
} UISScreen;
LAYOUT_ASSERT(UISScreen, 0x14);

// The studio's "current" record, set up by UISInit (0x14 bytes, after the tables).
typedef struct UISCurrent {
    s32 n0;                         // 0x00: -1 when empty
    u16 u4;                         // 0x04: 0xFFFF when empty
    u16 u6;                         // 0x06
    u16 u8;                         // 0x08
    u16 uA;                         // 0x0A
    s32 nC;                         // 0x0C
    UISScreenFile* p10;             // 0x10: a loaded UI file (UISSetGlobalScript)
} UISCurrent;
LAYOUT_ASSERT(UISCurrent, 0x14);

// The data words of an event (16 bytes). A screen event fills only aw[0] (group) and aw[1]
// (screen); UISAddThreadAction copies all of it.
typedef union UISEventData {
    u32 au[4];
    u16 aw[8];
    s16 as[8];                      // events 5 and 6: an ID in as[0], a done flag in as[1]
    void* ap[4];                    // events 5 and 6 carry pointers in words 1 and 2
                                    // port: the event stack holds pointers in 32-bit words
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
    u8* pStepScript;                // 0x04: a script run each step (_ParseRateFncs); it can scale the
                                    //       step. NULL for none
    s32 n8;                         // 0x08: ms run so far
    s32 nC;                         // 0x0C: ms stepped so far
    u32 u10;                        // 0x10: ms per step (the studio's uMsPerTick when loaded; 0: never steps)
    u32 uState;                     // 0x14: 0 new, 1 finished (removed by UISRemoveUnNessaryRateFncs), 2 running
    UISNodeInfo* pNodeInfo;         // 0x18: the node info its scripts run with (UISExecuteFnc); with
                                    //       uId, what a rate function is looked up by
    UISScreen* pScreen;             // 0x1C: the screen it belongs to
    u32 u20;                        // 0x20: which of pInfo's floats it drives (UISGetActionPtrValue); 0 for none
    f32 fTarget;                    // 0x24: the value it moves towards
    f32 fStep;                      // 0x28: the change per tick
    u8* pDoneScript;                // 0x2C: a script run when the target is reached (NULL: the first
                                    //       node's event -14 handler)
    union {
        s32 n30;                    // 0x30: as UISLoadAdvRateFnc stores it
        UISNodeInfo* pInfo;         //       the node the variable belongs to
    };
} UISRateFn;
LAYOUT_ASSERT(UISRateFn, 0x34);

// A block of words the studio hands out (UISInit sets p0, p4 and pC to its start). It is also
// where the script interpreter (UISStackProcess) is in a screen's script: UISExecuteFnc pushes a call's
// words on it and hands it to the interpreter as its frame.
typedef struct UISWordStack {
    s32* p0;                        // 0x00: the start
    s32* p4;                        // 0x04
    s32* p8;                        // 0x08: the end
    s32* pC;                        // 0x0C: the value stack's top, the next free word
    u8* p10;                        // 0x10: the next opcode byte
} UISWordStack;
LAYOUT_ASSERT(UISWordStack, 0x14);

// The interpreter's view of a word stack.
typedef UISWordStack UISFrame;

// A 0x28-byte record in UIStudio.p60; the last one in use names the current screen. It keeps a
// paused script: UISInternalUnloadModal restores the frame and runs it on when the screen it names is done.
typedef struct UISRecord60 {
    UISFrame frame;                 // 0x00: a copy of *pFrame, taken when the script paused
    UISNodeInfo* pInfo;             // 0x14: the screen's first node's; UISStackProcess's last argument
    UISScreen* pScreen;             // 0x18: the screen whose script paused; UISMoveScreenDrawPosition keeps it
                                    //       pointing at the same screen when screens move
    s32* p1C;                       // 0x1C: UISStackProcess's second argument; also a stack top
    UISFrame* pFrame;               // 0x20: the live frame
    u16 u24;                        // 0x24: a screen ID (UISFindScreen's third argument)
    u16 u26;                        // 0x26: a group ID (its second)
} UISRecord60;
LAYOUT_ASSERT(UISRecord60, 0x28);

#define UIS_MAGIC 0x5549535F        // "UIS_", while the studio is set up

// The studio (0xBC bytes). UISInit builds it in one block: this header, then the screen
// table, the handler table, the rate functions, the 0x28-byte records, the current record and
// the two word stacks. UISGetMemSize gives the block's size.
typedef struct UIStudio {
    u32 uMagic;                     // 0x00: UIS_MAGIC, cleared when every screen is unloaded
    u32 uFlags;                     // 0x04: bit 0 while the event stack runs, bit 1 while an event
                                    //       is being sent (new events are queued instead)
    u32 uMsPerTick;                 // 0x08: a tick's length in ms (the game passes 16)
    UISCommandFn pfnCommand;        // 0x0C: UISRegisterMessageFnc
    s32 n10;                        // 0x10
    UISLoadFn pfnLoad;              // 0x14: UISRegisterResourceFncs
    UISUnloadFn pfnUnload;          // 0x18: UISRegisterResourceFncs
    UISTransformFn pfnTransform;    // 0x1C: UISRegisterTransformFncs
    s32 n20;                        // 0x20
    UISScreenFn pfnScreen24;        // 0x24
    UISScreenDataFn pfnScreen28;    // 0x28: fn_80169B3C
    u32 nCurScreen;                 // 0x2C: index into pScreens, -1 for none
    u32 nMaxScreens;                // 0x30
    u32 nScreens;                   // 0x34
    UISScreen* pScreens;            // 0x38
    UISCurrent* pCurrent;           // 0x3C
    u32 nMaxHandlers;               // 0x40
    u32 nHandlers;                  // 0x44
    UISHandlerFn* ppfnHandlers;     // 0x48: filled by UISRegisterPluginFnc
    u32 nMaxRateFns;                // 0x4C
    s32 nRateFns;                   // 0x50: rate functions in use
    UISRateFn* pRateFns;            // 0x54
    u32 nMax60;                     // 0x58
    u32 n5C;                        // 0x5C: records in use in p60
    UISRecord60* p60;               // 0x60
    UISWordStack stack64;           // 0x64: the event words; the event stack grows down from its end
    UISWordStack stack78;           // 0x78: a second block of words
    u8 unk8C[0x20];
    s32* pEventBase;                // 0xAC: the bottom of the event stack; it grows down from here
    s32* pEventTop;                 // 0xB0: the stack's current top (UISAddThreadAction)
    s32** pp68;                     // 0xB4: points at stack64.p4
    s32 bUnloadingAll;              // 0xB8: set while UISShutdown unloads every screen
} UIStudio;
LAYOUT_ASSERT(UIStudio, 0xBC);

// UISEvent.c
void UISProcessThreadAction(UIStudio* pStudio, u8 b);
s32* _UISDoThreadAction(UIStudio* pStudio, s32* pTop, s32** ppKeep);
s32 UISThreadProcessHints(UIStudio* pStudio, u16 uGroup, u16 uScreen);
void UISAddThreadAction(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, UISEventData* pData, s32 nArgs,
                        const s32* pArgs);
void UISRegisterRuntimeErrorFnc(UISReportFn pfnReport);
void UISRemoveUnNessaryRateFncs(UIStudio* pStudio);
void UISUnloadRateFnc(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId);
void UISLoadRateFnc(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pNodeInfo, u32 uId, u8* pStepScript,
                    u32 u10);
void UISLoadAdvRateFnc(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pNodeInfo, s32 n30, u32 uId,
                       u8* pDoneScript, u8* pStepScript, u32 uTime, f32 fTarget, u32 u20);
u32 UISFindRateFnc(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId);

// UISStack.c
// Runs a screen's script from pFrame (a bytecode interpreter).
s8 UISStackProcess(UIStudio* pStudio, s32* p, UISFrame* pFrame, UISScreen* pScreen, UISNodeInfo* pInfo);

// UIStudio.c
void UISUpdateVisibility(UIStudio* pStudio, UISScreen* pScreen, s32 nKind, void* p, s32 bOn);
void UISInternalActivateScreen(UIStudio* pStudio, u8 bOn, u16 uGroup, u16 uScreen);
void UISInternalActivateControl(UIStudio* pStudio, u8 bOn, s32 nId, UISNodeInfo* pInfo, s32* p, u16 uScreen,
                                u16 uGroup);
void UISIdleProcess(UIStudio* pStudio, u32 uEvent);

// UISApi.c
void UISDrawObjects(UIStudio* pStudio, s32 nTicks);
void UISProcessInternalEvents(UIStudio* pStudio, UISWordStack* pStack, int uEvent, u32 n, s32 b, void* p,
                              u8 bAll);
void UISProcessEvent(UIStudio* pStudio, u32 uEvent, s32 n, s32 b, void* p, u8 bAll);
void UISGetActiveScreen(UIStudio* pStudio, u16* puGroup, u16* puScreen);
void UISSetScreenActive(UIStudio* pStudio, u16 uGroup, u16 uScreen);
u8 UISInternalUnloadScreen(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 n);
u8 UISInternalUnloadModal(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 n);
s32 UISLoadScreen(UIStudio* pStudio, u16 uGroup, u16 uScreen, u8 nArgs, s32* pArgs);
u8 UISSetGlobalScript(UIStudio* pStudio, UISScreenFile* pFile);
s32 _UISInternalLoad(UIStudio* pStudio, u16 uGroup, u16 uScreen, u8 bPush, u8 nArgs, s32* pArgs);
s32 UISInternalLoadScreen(UIStudio* pStudio, u16 uGroup, u16 uScreen, u16 uPrevGroup, u16 uPrevScreen,
                          u8 nArgs, s32* pArgs);
void UISRegisterPluginFnc(UIStudio* pStudio, s32 nIndex, UISHandlerFn pfnHandler);
void UISRegisterTransformFncs(UIStudio* pStudio, UISTransformFn pfnTransform);
void UISRegisterResourceFncs(UIStudio* pStudio, UISLoadFn pfnLoad, UISUnloadFn pfnUnload);
void fn_80169B3C(UIStudio* pStudio, UISScreenDataFn pfnScreen28);
void UISRegisterMessageFnc(UIStudio* pStudio, UISCommandFn pfnCommand);
void UISShutdown(UIStudio* pStudio);
void UISInit(UIStudio* pStudio, u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords,
             u32 nWords2, u32 uMsPerTick);
u32 UISGetMemSize(u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords, u32 nWords2);
s32 PatchScrData(UISScreenFile* pFile);
void _ParseRateFncs(UIStudio* pStudio, u32 uMs);

// UISScreen.c (0x8016A2D4-0x8016C718)
// Sends event uEvent to node nNode of a screen and the nodes it links to; *pbOut gets the node's
// UISNodeInfo.u4.
s32 _ParseMaps(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent, u32 n5,
               s32 nArgs, s32* pArgs, u8* pbOut);
void _ParseObjects(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg);
void _ParseTransforms(UIStudio* pStudio, int nOp, UISScreen* pScreen, u32 nNode);
void _ParseVisibility(UIStudio* pStudio, UISScreen* pScreen, s32 n, s32 nKind, void* p, u8 bAll);
s32 _DetermineVisibility(UISScreen* pScreen, UISNodeInfo* pInfo, s32 nKind, void* p);
void _ParseInitialize(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg);
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, s32* pArgs);
void UISDoHint(UIStudio* pStudio, u32 uEvent, s32 nArgs, s32* pArgs);
void UISMoveScreenDrawPosition(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 nMove);
UISNodeInfo* UISFindSiblingEnableControl(UISScreen* pScreen, UISNodeInfo* pInfo);
void UISStringFormat(u32 u0, UISText* pOut, UISText* pFormat, s32 nArgs, const UISWord* pArgs);
// The studio's printf: %c %s %d %i %u %f %x %X %p, with '-', '0', a width and a precision.
s32 UISSprintf(char* pOut, s32 nSize, const char* szFormat, s32 nArgs, const UISWord* pArgs);
void UISSetColorAdditive(f32 f1, f32 f2, f32 f3, f32 f4);
void UISSetColorMultipler(f32 f1, f32 f2, f32 f3, f32 f4);
// Returns a pointer to the float of pInfo that a rate function's n20 names.
f32* UISGetActionPtrValue(s32 n20, UISNodeInfo* pInfo);
// Runs pScript for node info pInfo with a call frame pushed on pStack.
s32 UISExecuteFnc(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pInfo, UISWordStack* pStack,
                  u8* pScript, s32 nArgs, s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra,
                  s32* pnSaved);
// A node's handler scripts for an event, by kind (0x4000, plain with an ID, 0x8000); NULL for none.
// The event is a u32 (callers pass it unmasked; each function masks it to 16 bits).
u8* _UISFindHintPC(UISNode* pNode, u32 uEvent);
u8* UISFindSubControlEventPC(UISNode* pNode, u16 uId, u32 uEvent);
u8* UISFindEventPC(UISNode* pNode, u32 uEvent);
u16 UISFindScreen(UIStudio* pStudio, u16 uGroup, u16 uScreen);

// Sends event uEvent to the current screen, or to every screen when bAll is set; n -8 skips a
// screen being unloaded. UISProcessInternalEvents's body, which UIStudio.c has pasted in twice (the pasted
// copies keep this block layout, with the loop set-up after the loop).
static inline void UIStudio_Send(UIStudio* pStudio, UISWordStack* pStack, u32 uEvent, s32 n, s32 b, void* p,
                                 u8 bAll) {
    u32 i;
    u32 nEnd;
    UISScreen* pScreen;
    u8 bOut;

    if (bAll) {
        nEnd = pStudio->nScreens;
        i = 0;
    } else {
        i = pStudio->nCurScreen;
        nEnd = i + 1;
        if (i == -1) return;
    }
    for (; i < nEnd; i++) {
        pScreen = &pStudio->pScreens[i];
        if ((u32)n != -8 || pScreen->bUnloading != 1) {  // fake match: the original compares unsigned
            bOut = 0;
            _ParseMaps(pStudio, pScreen, pStack, 0, uEvent, n, b, p, &bOut);
        }
    }
}

#endif
