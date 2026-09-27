#ifndef FRONTEND_UISTUDIO_H
#define FRONTEND_UISTUDIO_H

// EA's UI Studio library (0x80165528-0x8016C718; EA's file names UISEvent.c and UIStudio.c come
// from its asserts): the runtime the menu screens are built on. It keeps the loaded screens, a
// stack of pending events, and "rate functions" that move a screen variable towards a target
// over a set time. The front end reaches it through FrontEnd.pHandler (game/frontend.h).
//
// Type, field and tag names are EA's, from Madden NFL 2003's STABS of the same library
// (docs/reference-builds/madden2003-ps2: every struct has the same size and offsets). Field types
// are ours where EA's differ only in sign (EA: Uint32 counts), and where noted. The comments in
// the UIS sources call a control (UISControlT) a node.

#include "game_types.h"
#include "platform.h"
#include "frontend/uisvec.h"

// Where the library reports problems: a level (0 warning, 1 error, as the calls use it), the
// source file, the line and the message. Set by UISRegisterRuntimeErrorFnc.
typedef void UISRuntimeErrorFncT(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);

extern UISRuntimeErrorFncT* RuntimeErrorFnc;

// A text buffer the studio formats into and reads from (UISStringFormat, UISSprintf); a screen
// file's string table (Strings) holds them too.
typedef struct UISString_t {
    u32 ID;                         // 0x00
    u32 length;                     // 0x04: the buffer's size
    char* ptr;                      // 0x08
} UISStringT;
LAYOUT_ASSERT(UISStringT, 0xC);

typedef struct UISArray_t {
    u32 nDimensions;                // 0x00
    u32 Size[1];                    // 0x04
} UISArrayT;

// A word of a call's arguments: UISSprintf reads them by its format string.
typedef union UISParam_t {
    f32 fValue;
    f32* pfValue;
    s32 iValue;
    s32* piValue;
    UISStringT* strAddr;
    UISArrayT* pArray;
    u32 u;                          // ours: the word read unsigned
} UISParamT;

typedef struct UISVector_t {
    f32 x;                          // 0x00
    f32 y;                          // 0x04
    f32 z;                          // 0x08
} UISVectorT;

// The callbacks the game hands the studio (uiProcessInterface.c sets them). Parameters come from
// the studio's calls, or where noted from the game's own callback.
typedef void UISMessageFncT(s32 nCmd, s32 n1, s32 n2, s32 n3, s32 n4, s32 n5);  // the game's fn_8008F568
// Returns the screen's UI file, still unfixed (the game's fn_8008F610, which ignores the group).
typedef void* UISResLoadFncT(u16 uGroup, u16 uScreen);
typedef void UISResUnloadFncT(u16 uGroup, u16 uScreen, void* pData);
typedef void UISTransformFncT(int nOp, void* pDesc);            // the game's fn_80093280
// Never set by TW2004; STABS keep no parameter list.
typedef UISVectorT UISLocalizeFncT();
typedef void UISScreenActivatedFncT(u16 uGroup, u16 uScreen);
typedef void UISScreenDrawDebugFncT(u16 uGroup, u16 uScreen, s32 n);
// The table at UISInfoT.Plugins: pVar is the handler's variable in the screen file.
typedef void UISPluginFncT(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);

typedef struct UISPlugin_t {
    UISPluginFncT* pFnc;            // 0x00
} UISPluginT;

// A handler a screen control has for an event (8 bytes; UISScreen.c looks them up).
typedef struct UISMap_t {
    u16 ControlIndex;               // 0x00: 0x8000 and 0x4000 mark two kinds of handler; with
                                    //       neither, the bits under 0x2FFF are an ID
    u16 EventID;                    // 0x02: the event it handles; 0xFFFF: a link to control nNode
    union {
        u8* pFnc;                   //       the script it runs (UISExecuteFnc starts the interpreter on it)
        u32 nNode;                  //       ours: the linked control (EA has pFnc only)
    };                              // 0x04
} UISMapT;

typedef struct UISTransform_t {
    UISVectorT Offset;              // 0x00
    UISVectorT Pivot;               // 0x0C
    UISVectorT Rotation;            // 0x18
    UISVectorT Scale;               // 0x24
    void* pGlobalInfo;              // 0x30
    UISColorVectorT MultiplerFactor;  // 0x34
    UISColorVectorT AdditiveFactor;   // 0x44
} UISTransformT;
LAYOUT_ASSERT(UISTransformT, 0x54);

// A control's drawing values (UISScreen.c). _ParseObjects multiplies Transform.MultiplerFactor
// into the studio's _MultiplerColorFactor and adds Transform.AdditiveFactor to _AdditiveColorFactor
// while it draws the control's children; a rate function drives one of the floats
// (UISGetActionPtrValue picks it).
typedef struct UISControlInfo_t {
    void* IsVisible;                // 0x00: EA: Uint32. The control or layer that owns it
                                    //       (_DetermineVisibility records it); _ParseObjects draws
                                    //       only controls whose info has one
    u32 IsEnabled;                  // 0x04: _ParseMaps hands it back; UISFindSiblingEnableControl looks
                                    //       for a set one
    UISTransformT Transform;        // 0x08: UISInfoT.pTransformFnc gets a pointer to it
    s32 idxFocusChild;              // 0x5C
    u32 CanHandleMessages;          // 0x60
} UISControlInfoT;
LAYOUT_ASSERT(UISControlInfoT, 0x64);

// An object of a control's layer or of a screen file's static list (8 bytes): a call to one of
// the studio's handlers with a variable of the screen file, or a link to another control.
typedef struct UISObj_t {
    u16 PluginIndex;                // 0x00: index into UISInfoT.Plugins; 0xFFFF: a link to control nNode
    s16 bInitialized;               // 0x02: whether the handler has run (_ParseInitialize)
    union {
        u32* pData;                 //       EA: void*. The handler's variable, as an offset into the
                                    //       screen file
        u32 nNode;                  //       ours: the linked control
    };                              // 0x04
} UISObjT;
LAYOUT_ASSERT(UISObjT, 8);

// A layer of objects in a control. It starts like a control: _DetermineVisibility looks for an
// info pointer in both.
typedef struct UISLayer_t {
    UISControlInfoT* pLayerInfo;    // 0x00: EA: UISLayerInfoT* ({ Uint32 IsVisible; }), read here as
                                    //       the control info's first word; a layer whose info has
                                    //       no owner is skipped
    u32 NumObjs;                    // 0x04
    UISObjT* Objs;                  // 0x08
} UISLayerT;

// A control of a loaded screen (0x14 bytes).
typedef struct UISControl_t {
    UISControlInfoT* pControlInfo;  // 0x00
    u32 NumLayers;                  // 0x04
    UISLayerT** Layers;             // 0x08
    u32 NumMaps;                    // 0x0C
    UISMapT* Maps;                  // 0x10: a handler with EventID 0xFFFF links to control nNode
} UISControlT;
LAYOUT_ASSERT(UISControlT, 0x14);

// A loaded screen file (what the load callback returns; also the global script's file): its
// controls, control 0 first, and a list of objects run before the controls. Its pointer fields
// hold offsets from the file's start until PatchScrData fixes them up.
// port: the file is laid out for a 32-bit machine; a port must read it through a loader instead.
typedef struct UISScrData_t {
    u32 NumControls;                // 0x00
    UISControlT* Controls;          // 0x04
    u32 NumStrings;                 // 0x08
    UISStringT* Strings;            // 0x0C
    u32 NumPatchStrings;            // 0x10
    u32* StringPatchTable;          // 0x14: file offsets of words that name a Strings entry
    u32 NumStaticObjects;           // 0x18
    UISObjT* StaticObjects;         // 0x1C
} UISScrDataT;

// A loaded screen (0x14 bytes, in the studio's screen table; the global script is one too).
typedef struct UISScreen_t {
    u32 ControllerDisable;          // 0x00: one bit per event kind the screen has already taken
    u16 GroupID;                    // 0x04: the screen's group ID
    u16 ScreenID;                   // 0x06: its ID within the group
    u16 ParentGroupID;              // 0x08: with ParentScreenID, the screen made current when this
    u16 ParentScreenID;             // 0x0A: one is unloaded (UISInternalUnloadScreen)
    s32 bWaitingToBeUnloaded;       // 0x0C: set while the screen waits to be unloaded
    UISScrDataT* pScrData;          // 0x10: what the load callback returned; the unload callback gets it
} UISScreenT;
LAYOUT_ASSERT(UISScreenT, 0x14);

// The info words of a thread action (16 bytes): which of the four views depends on the action.
typedef struct UISThreadGroupInfoMessage_t {
    u32 Message;                    // 0x00
    u32 Controller;                 // 0x04
    u32 pad1;                       // 0x08
    u32 pad2;                       // 0x0C
} UISThreadGroupInfoMessageT;

// Actions 5 and 6.
// port: the thread action stack holds pointers in 32-bit words
typedef struct UISThreadGroupInfoActivate_t {
    s16 iDir;                       // 0x00: an ID
    s16 iProcessed;                 // 0x02: a done flag
    s32* pTableEntry;               // 0x04: EA: UISControlListT* (a count, a word, then file offsets)
    UISControlInfoT* pControlInfo;  // 0x08
    u16 ScreenID;                   // 0x0C
    u16 GroupID;                    // 0x0E
} UISThreadGroupInfoActivateT;

// A screen action fills only GroupID and ScreenID.
typedef struct UISThreadGroupInfoScreen_t {
    u16 GroupID;                    // 0x00
    u16 ScreenID;                   // 0x02
    u16 ParentGroupID;              // 0x04
    u16 ParentScreenID;             // 0x06
    u32 iRetVal;                    // 0x08
    s32 iDir;                       // 0x0C
} UISThreadGroupInfoScreenT;

typedef struct UISThreadGroupInfoGeneric_t {
    u32 Data[4];                    // 0x00
} UISThreadGroupInfoGenericT;

// UISAddThreadAction copies all of it.
typedef union {
    UISThreadGroupInfoMessageT MessageInfo;
    UISThreadGroupInfoActivateT ActivateInfo;
    UISThreadGroupInfoScreenT ScreenInfo;
    UISThreadGroupInfoGenericT GenericInfo;
} UISThreadGroupInfoT;

// A thread action on the studio's stack (0x24 bytes; ours: EA writes it word by word). Its type
// is the stack's top word; its arguments lie below it, the first one lowest.
typedef struct UISEvent {
    s32 nArgs;                      // 0x00
    UISThreadGroupInfoT data;       // 0x04
    u8 unk14[4];
    s32 nB;                         // 0x18
    s32 nA;                         // 0x1C
    s32 nType;                      // 0x20
} UISEvent;
LAYOUT_ASSERT(UISEvent, 0x24);

typedef struct UISAnimateData_t {
    u32 iType;                      // 0x00 (0x20): which of pInfo's floats it drives
                                    //       (UISGetActionPtrValue); 0 for none
    f32 fEndValue;                  // 0x04 (0x24): the value it moves towards
    f32 fStepValue;                 // 0x08 (0x28): the change per tick
    u8* pEndFnc;                    // 0x0C (0x2C): a script run when the target is reached (NULL:
                                    //       the first control's event -14 handler)
    union {
        UISControlInfoT* pSubControlInfo;  // 0x10 (0x30): the control the variable belongs to
        s32 n30;                    //       ours: as UISLoadAdvRateFnc stores it
    };
} UISAnimateDataT;

// A rate function: moves one variable of a screen towards a target, a step every tick.
typedef struct UISRateFnc_t {
    u32 RateFncID;                  // 0x00: the rate function's ID
    u8* pFnc;                       // 0x04: a script run each step (_ParseRateFncs); it can scale the
                                    //       step. NULL for none
    s32 MSCount;                    // 0x08: ms run so far
    s32 MSLastCount;                // 0x0C: ms stepped so far
    u32 MSRate;                     // 0x10: ms per step (the studio's MSPerTick when loaded; 0: never steps)
    u32 State;                      // 0x14: 0 new, 1 finished (removed by UISRemoveUnNessaryRateFncs),
                                    //       2 running
    UISControlInfoT* pControlInfo;  // 0x18: the control info its scripts run with (UISExecuteFnc); with
                                    //       RateFncID, what a rate function is looked up by
    UISScreenT* pScreen;            // 0x1C: the screen it belongs to
    UISAnimateDataT AnimationData;  // 0x20
} UISRateFncT;
LAYOUT_ASSERT(UISRateFncT, 0x34);

// A block of words the studio hands out (UISInit sets pStackStart, pStackCurrent and pStack to its
// start). It is also where the script interpreter (UISStackProcess) is in a screen's script:
// UISExecuteFnc pushes a call's words on it and hands it to the interpreter as its frame.
typedef struct UISStackInfo_t {
    s32* pStackStart;               // 0x00: EA: UISParamT*, as the other four. The start
    s32* pStackCurrent;             // 0x04
    s32* pStackEnd;                 // 0x08: the end
    s32* pStack;                    // 0x0C: the value stack's top, the next free word
    u8* pPC;                        // 0x10: the next opcode byte
} UISStackInfoT;
LAYOUT_ASSERT(UISStackInfoT, 0x14);

// A 0x28-byte entry of UISInfoT.ModalStack; the last one in use names the current screen. It keeps
// a paused script: UISInternalUnloadModal restores the frame and runs it on when the screen it
// names is done.
typedef struct UISModalStack_t {
    UISStackInfoT StackState;       // 0x00: a copy of *pRestoreState, taken when the script paused
    UISControlInfoT* pControlInfo;  // 0x14: the screen's first control's; UISStackProcess's last argument
    UISScreenT* pScreen;            // 0x18: the screen whose script paused; UISMoveScreenDrawPosition
                                    //       keeps it pointing at the same screen when screens move
    s32* pRestoreStack;             // 0x1C: UISStackProcess's second argument; also a stack top
    UISStackInfoT* pRestoreState;   // 0x20: the live frame
    u16 ScreenID;                   // 0x24: a screen ID (UISFindScreen's third argument)
    u16 GroupID;                    // 0x26: a group ID (its second)
} UISModalStackT;
LAYOUT_ASSERT(UISModalStackT, 0x28);

typedef struct UISLocalThreadInfo_t {
    s32* pBeginParams;              // 0x00: the bottom of the stack; it grows down from here
    s32* pCurrentParams;            // 0x04: the stack's current top (UISAddThreadAction)
    s32** ppEndParams;              // 0x08: points at EventStack.pStackCurrent
} UISLocalThreadInfoT;

#define UIS_MAGIC 0x5549535F        // "UIS_", while the studio is set up

// The studio (0xBC bytes). UISInit builds it in one block: this header, then the screen
// table, the plugin table, the rate functions, the modal stack, the global script's screen and
// the two word stacks. UISGetMemSize gives the block's size.
typedef struct UISInfo_t {
    u32 InfoID;                     // 0x00: UIS_MAGIC, cleared when every screen is unloaded
    u32 CriticalRegions;            // 0x04: bit 0 while the thread actions run, bit 1 while an event
                                    //       is being sent (new actions are queued instead)
    u32 MSPerTick;                  // 0x08: a tick's length in ms (the game passes 16)
    UISMessageFncT* pMessageFnc;    // 0x0C: UISRegisterMessageFnc
    UISResUnloadFncT* pShutdownScreenFnc;  // 0x10
    UISResLoadFncT* pLoadFnc;       // 0x14: UISRegisterResourceFncs
    UISResUnloadFncT* pUnloadFnc;   // 0x18: UISRegisterResourceFncs
    UISTransformFncT* pTransformFnc;  // 0x1C: UISRegisterTransformFncs
    UISLocalizeFncT* pLocalizeFnc;  // 0x20
    UISScreenActivatedFncT* pScreenActivatedFnc;  // 0x24
    UISScreenDrawDebugFncT* pScreenDrawDebugFnc;  // 0x28: fn_80169B3C
    u32 ActiveScreenIdx;            // 0x2C: index into Screens, -1 for none
    u32 MaxScreens;                 // 0x30
    u32 NumScreens;                 // 0x34
    UISScreenT* Screens;            // 0x38
    UISScreenT* pGlobalScript;      // 0x3C: set up by UISInit after the tables (GroupID etc.
                                    //       0xFFFF when empty); pScrData is a loaded UI file
                                    //       (UISSetGlobalScript)
    u32 MaxPlugins;                 // 0x40
    u32 NumPlugins;                 // 0x44
    UISPluginT* Plugins;            // 0x48: filled by UISRegisterPluginFnc
    u32 MaxRateFncs;                // 0x4C
    s32 NumRateFncs;                // 0x50: rate functions in use
    UISRateFncT* RateFncs;          // 0x54
    u32 MaxModals;                  // 0x58
    u32 NumModals;                  // 0x5C: entries in use in ModalStack
    UISModalStackT* ModalStack;     // 0x60
    UISStackInfoT EventStack;       // 0x64: the thread action words; the stack grows down from its end
    UISStackInfoT RateStack;        // 0x78: a second block of words
    UISColorVectorT MultiplerFactor;  // 0x8C
    UISColorVectorT AdditiveFactor;   // 0x9C
    UISLocalThreadInfoT ThreadInfo;   // 0xAC
    s32 bShuttingDown;              // 0xB8: set while UISShutdown unloads every screen
} UISInfoT;
LAYOUT_ASSERT(UISInfoT, 0xBC);

// UISEvent.c
void UISProcessThreadAction(UISInfoT* pStudio, u8 b);
s32* _UISDoThreadAction(UISInfoT* pStudio, s32* pTop, s32** ppKeep);
s32 UISThreadProcessHints(UISInfoT* pStudio, u16 uGroup, u16 uScreen);
void UISAddThreadAction(s16 nA, s16 nB, UISInfoT* pStudio, s32 nType, UISThreadGroupInfoT* pData, s32 nArgs,
                        const s32* pArgs);
void UISRegisterRuntimeErrorFnc(UISRuntimeErrorFncT* pfnReport);
void UISRemoveUnNessaryRateFncs(UISInfoT* pStudio);
void UISUnloadRateFnc(UISInfoT* pStudio, UISControlInfoT* pNodeInfo, u32 uId);
void UISLoadRateFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pNodeInfo, u32 uId,
                    u8* pStepScript, u32 u10);
void UISLoadAdvRateFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pNodeInfo, s32 n30, u32 uId,
                       u8* pDoneScript, u8* pStepScript, u32 uTime, f32 fTarget, u32 u20);
u32 UISFindRateFnc(UISInfoT* pStudio, UISControlInfoT* pNodeInfo, u32 uId);

// UISStack.c
// Runs a screen's script from pFrame (a bytecode interpreter).
s8 UISStackProcess(UISInfoT* pStudio, s32* p, UISStackInfoT* pFrame, UISScreenT* pScreen,
                   UISControlInfoT* pInfo);

// UIStudio.c
void UISUpdateVisibility(UISInfoT* pStudio, UISScreenT* pScreen, s32 nKind, void* p, s32 bOn);
void UISInternalActivateScreen(UISInfoT* pStudio, u8 bOn, u16 uGroup, u16 uScreen);
void UISInternalActivateControl(UISInfoT* pStudio, u8 bOn, s32 nId, UISControlInfoT* pInfo, s32* p,
                                u16 uScreen, u16 uGroup);
void UISIdleProcess(UISInfoT* pStudio, u32 uEvent);

// UISApi.c
void UISDrawObjects(UISInfoT* pStudio, s32 nTicks);
void UISProcessInternalEvents(UISInfoT* pStudio, UISStackInfoT* pStack, int uEvent, u32 n, s32 b, void* p,
                              u8 bAll);
void UISProcessEvent(UISInfoT* pStudio, u32 uEvent, s32 n, s32 b, void* p, u8 bAll);
void UISGetActiveScreen(UISInfoT* pStudio, u16* puGroup, u16* puScreen);
void UISSetScreenActive(UISInfoT* pStudio, u16 uGroup, u16 uScreen);
u8 UISInternalUnloadScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 n);
u8 UISInternalUnloadModal(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 n);
s32 UISLoadScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u8 nArgs, s32* pArgs);
u8 UISSetGlobalScript(UISInfoT* pStudio, UISScrDataT* pFile);
s32 _UISInternalLoad(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u8 bPush, u8 nArgs, s32* pArgs);
s32 UISInternalLoadScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u16 uPrevGroup, u16 uPrevScreen,
                          u8 nArgs, s32* pArgs);
void UISRegisterPluginFnc(UISInfoT* pStudio, s32 nIndex, UISPluginFncT* pfnHandler);
void UISRegisterTransformFncs(UISInfoT* pStudio, UISTransformFncT* pfnTransform);
void UISRegisterResourceFncs(UISInfoT* pStudio, UISResLoadFncT* pfnLoad, UISResUnloadFncT* pfnUnload);
void fn_80169B3C(UISInfoT* pStudio, UISScreenDrawDebugFncT* pfnScreen28);
void UISRegisterMessageFnc(UISInfoT* pStudio, UISMessageFncT* pfnCommand);
void UISShutdown(UISInfoT* pStudio);
void UISInit(UISInfoT* pStudio, u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords,
             u32 nWords2, u32 uMsPerTick);
u32 UISGetMemSize(u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords, u32 nWords2);
s32 PatchScrData(UISScrDataT* pFile);
void _ParseRateFncs(UISInfoT* pStudio, u32 uMs);

// UISScreen.c (0x8016A2D4-0x8016C718)
// Sends event uEvent to node nNode of a screen and the nodes it links to; *pbOut gets the node's
// UISControlInfoT.IsEnabled.
s32 _ParseMaps(UISInfoT* pStudio, UISScreenT* pScreen, UISStackInfoT* pStack, u32 nNode, u32 uEvent, u32 n5,
               s32 nArgs, s32* pArgs, u8* pbOut);
void _ParseObjects(UISInfoT* pStudio, UISScreenT* pScreen, u32 nNode, s32 nMsg);
void _ParseTransforms(UISInfoT* pStudio, int nOp, UISScreenT* pScreen, u32 nNode);
void _ParseVisibility(UISInfoT* pStudio, UISScreenT* pScreen, s32 n, s32 nKind, void* p, u8 bAll);
s32 _DetermineVisibility(UISScreenT* pScreen, UISControlInfoT* pInfo, s32 nKind, void* p);
void _ParseInitialize(UISInfoT* pStudio, UISScreenT* pScreen, u32 nNode, s32 nMsg);
void fn_8016B09C(UISInfoT* pStudio, u32 uEvent, s32 nArgs, s32* pArgs);
void UISDoHint(UISInfoT* pStudio, u32 uEvent, s32 nArgs, s32* pArgs);
void UISMoveScreenDrawPosition(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 nMove);
UISControlInfoT* UISFindSiblingEnableControl(UISScreenT* pScreen, UISControlInfoT* pInfo);
void UISStringFormat(u32 u0, UISStringT* pOut, UISStringT* pFormat, s32 nArgs, const UISParamT* pArgs);
// The studio's printf: %c %s %d %i %u %f %x %X %p, with '-', '0', a width and a precision.
s32 UISSprintf(char* pOut, s32 nSize, const char* szFormat, s32 nArgs, const UISParamT* pArgs);
void UISSetColorAdditive(f32 f1, f32 f2, f32 f3, f32 f4);
void UISSetColorMultipler(f32 f1, f32 f2, f32 f3, f32 f4);
// Returns a pointer to the float of pInfo that a rate function's n20 names.
f32* UISGetActionPtrValue(s32 n20, UISControlInfoT* pInfo);
// Runs pScript for node info pInfo with a call frame pushed on pStack.
s32 UISExecuteFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pInfo, UISStackInfoT* pStack,
                  u8* pScript, s32 nArgs, s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra,
                  s32* pnSaved);
// A node's handler scripts for an event, by kind (0x4000, plain with an ID, 0x8000); NULL for none.
// The event is a u32 (callers pass it unmasked; each function masks it to 16 bits).
u8* _UISFindHintPC(UISControlT* pNode, u32 uEvent);
u8* UISFindSubControlEventPC(UISControlT* pNode, u16 uId, u32 uEvent);
u8* UISFindEventPC(UISControlT* pNode, u32 uEvent);
u16 UISFindScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen);

// Sends event uEvent to the current screen, or to every screen when bAll is set; n -8 skips a
// screen being unloaded. UISProcessInternalEvents's body, which UIStudio.c has pasted in twice (the pasted
// copies keep this block layout, with the loop set-up after the loop).
static inline void UIStudio_Send(UISInfoT* pStudio, UISStackInfoT* pStack, u32 uEvent, s32 n, s32 b, void* p,
                                 u8 bAll) {
    u32 i;
    u32 nEnd;
    UISScreenT* pScreen;
    u8 bOut;

    if (bAll) {
        nEnd = pStudio->NumScreens;
        i = 0;
    } else {
        i = pStudio->ActiveScreenIdx;
        nEnd = i + 1;
        if (i == -1) return;
    }
    for (; i < nEnd; i++) {
        pScreen = &pStudio->Screens[i];
        // fake match: the original compares unsigned
        if ((u32)n != -8 || pScreen->bWaitingToBeUnloaded != 1) {
            bOut = 0;
            _ParseMaps(pStudio, pScreen, pStack, 0, uEvent, n, b, p, &bOut);
        }
    }
}

#endif
