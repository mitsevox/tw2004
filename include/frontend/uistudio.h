#ifndef FRONTEND_UISTUDIO_H
#define FRONTEND_UISTUDIO_H

// EA's UI Studio library (0x80165528-0x8016C718; EA's file names UISEvent.c and UIStudio.c come
// from its asserts): the runtime the menu screens are built on. It keeps the loaded screens, a
// stack of pending events, and "rate functions" that move a screen variable towards a target
// over a set time. The front end reaches it through FrontEnd.pHandler (game/frontend.h).
//
// Type, field and tag names are EA's, from Madden NFL 2003's STABS of the same library
// (reference/madden2003-ps2: every struct has the same size and offsets). Field types
// are ours where EA's differ only in sign (EA: Uint32 counts), and where noted. The comments in
// the UIS sources call a control (UISControlT) a node.

#include "game_types.h"
#include "platform.h"
#include "frontend/uisvec.h"

// Where the library reports problems: a level (0 warning, 1 error, as the calls use it), the
// source file, the line and the message. Set by UISRegisterRuntimeErrorFnc.
typedef void UISRuntimeErrorFncT(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);

extern UISRuntimeErrorFncT* RuntimeErrorFnc;

// The thread actions: what an entry of the thread action stack does (UISAddThreadAction).
typedef enum {
    UISThreadAction_Load = 0,
    UISThreadAction_Unload = 1,
    UISThreadAction_Update = 2,
    UISThreadAction_ScreenActivate = 3,
    UISThreadAction_ScreenDeactivate = 4,
    UISThreadAction_ControlActivate = 5,
    UISThreadAction_ControlDeactivate = 6,
    UISThreadAction_ProcessEvent = 7,
    UISThreadAction_MoveScreen = 8,
    UISThreadAction_HINT = 9,
} UISThreadActionT;

// A rate function's State.
enum {
    UISRATE_LOAD = 0,
    UISRATE_UNLOAD = 1,
    UISRATE_ACTIVE = 2,
};

// What the transform callback is asked to do (_ParseTransforms).
typedef enum UISTransformAction_t {
    UISTransformInit = 0,
    UISTransformPush = 1,
    UISTransformPop = 2,
    UISTransformShutdown = 3,
} UISTransformAction;

// What UISStackProcess (and UISExecuteFnc) return.
enum {
    UISPROCESS_CONTINUE = 0,
    UIS_PROCESSSUBONLY = 1,
    UISPROCESS_HARDABORT = 2,
    UISPROCESS_DOMODAL = 3,
};

// The control info float a rate function drives (UISGetActionPtrValue).
enum {
    UIS_ACTION_NONE = 0,
    UIS_ACTION_ROTATION_X = 16,
    UIS_ACTION_ROTATION_Y = 17,
    UIS_ACTION_ROTATION_Z = 18,
    UIS_ACTION_TRANSLATE_X = 19,
    UIS_ACTION_TRANSLATE_Y = 20,
    UIS_ACTION_TRANSLATE_Z = 21,
    UIS_ACTION_SCALE_X = 22,
    UIS_ACTION_SCALE_Y = 23,
    UIS_ACTION_SCALE_Z = 24,
    UIS_ACTION_PIVOT_X = 25,
    UIS_ACTION_PIVOT_Y = 26,
    UIS_ACTION_PIVOT_Z = 27,
    UIS_ACTION_ADD_ALPHA = 32,
    UIS_ACTION_ADD_RED = 33,
    UIS_ACTION_ADD_GREEN = 34,
    UIS_ACTION_ADD_BLUE = 35,
    UIS_ACTION_MULTIPLY_ALPHA = 36,
    UIS_ACTION_MULTIPLY_RED = 37,
    UIS_ACTION_MULTIPLY_GREEN = 38,
    UIS_ACTION_MULTIPLY_BLUE = 39,
};

// The script interpreter's opcodes (EA's file: UISStack.c). TW2004 adds 0x79-0x7E, which have no
// names here.
typedef enum {
    UIS_NOOP = 1,
    UIS_LOAD_SCREEN = 2,
    UIS_UNLOAD_SCREEN = 3,
    UIS_LOAD_RATEFNC = 6,
    UIS_UNLOAD_RATEFNC = 7,
    UIS_SET_SCREEN_ACTIVE = 8,
    UIS_SET_CONTROL_ACTIVE = 9,
    UIS_PROCESS_OBJECT = 10,
    UIS_SEND_MESSAGE = 11,
    UIS_PROCESS_EVENT = 12,
    UIS_BASE_ADDR = 13,
    UIS_STRNCPY = 14,
    UIS_PUSH_STR = 15,
    UIS_PUSH_INT = 16,
    UIS_PUSH_FLT = 17,
    UIS_CAST_INT = 18,
    UIS_CAST_FLT = 19,
    UIS_CAST_INT_1 = 20,
    UIS_CAST_FLT_1 = 21,
    UIS_GET_D = 24,
    UIS_PUT_D = 25,
    UIS_GET_S = 26,
    UIS_PUT_S = 27,
    UIS_BAND = 28,
    UIS_BOR = 29,
    UIS_BNEG = 30,
    UIS_LAND = 31,
    UIS_LOR = 32,
    UIS_LNOT = 33,
    UIS_ABS = 34,
    UIS_ABS_F = 35,
    UIS_NEG = 36,
    UIS_ADD = 37,
    UIS_SUB = 38,
    UIS_MUL = 39,
    UIS_DIV = 40,
    UIS_NEG_F = 41,
    UIS_ADD_F = 42,
    UIS_SUB_F = 43,
    UIS_MUL_F = 44,
    UIS_DIV_F = 45,
    UIS_INC = 46,
    UIS_DEC = 47,
    UIS_INC_F = 48,
    UIS_DEC_F = 49,
    UIS_GE = 50,
    UIS_LE = 51,
    UIS_GT = 52,
    UIS_LT = 53,
    UIS_EQ = 54,
    UIS_NE = 55,
    UIS_GE_F = 56,
    UIS_LE_F = 57,
    UIS_GT_F = 58,
    UIS_LT_F = 59,
    UIS_EQ_F = 60,
    UIS_NE_F = 61,
    UIS_BRA_TRUE = 62,
    UIS_BRA_FALSE = 63,
    UIS_JUMP = 64,
    UIS_PUSH = 65,
    UIS_POP = 66,
    UIS_CALL = 67,
    UIS_RET = 68,
    UIS_GOTONEXTSCREEN = 69,
    UIS_DEACTIVECONTROL = 70,
    UIS_ACTIVECONTROL = 71,
    UIS_ACTIVEPARENTSCREEN = 72,
    UIS_STR_GETLENTGH = 73,
    UIS_STR_GETCHAR = 74,
    UIS_STR_SETCHAR = 75,
    UIS_STR_FORMAT = 76,
    UIS_UPDATESCREENS = 77,
    UIS_MAPLINE_DEBUGONLY = 78,
    UIS_EVENTSON = 79,
    UIS_EVENTSOFF = 80,
    UIS_DOMODAL = 81,
    UIS_SWAPSTACK = 82,
    UIS_PRINT_DEBUGONLY = 83,
    UIS_SET_SCREEN_OBJECT = 84,
    UIS_GET_D_THIS = 85,
    UIS_PUT_D_THIS = 86,
    UIS_CALOFFSET_THIS = 87,
    UIS_ADVRATEFNC = 88,
    UIS_VISIBILITY_CHANGE = 89,
    UIS_MOD = 90,
    UIS_GET_ELEMENT_S = 91,
    UIS_PUT_ELEMENT_S = 92,
    UIS_GET_ELEMENT_D = 93,
    UIS_PUT_ELEMENT_D = 94,
    UIS_GET_ELEMENT_D_THIS = 95,
    UIS_PUT_ELEMENT_D_THIS = 96,
    UIS_GET_ELEMENT_ADDR = 97,
    UIS_PUT_ELEMENT_ADDR = 98,
    UIS_PUSH_MULTIPLE = 99,
    UIS_POP_MULTIPLE = 100,
    UIS_COPY_MULTIPLE = 101,
    UIS_FILL_ARRAY_S = 102,
    UIS_FILL_ARRAY_D = 103,
    UIS_FILL_ARRAY_D_THIS = 104,
    UIS_ADDR_S = 105,
    UIS_ADDR_D = 106,
    UIS_ADDR_D_THIS = 107,
    UIS_ADDR_ELEMENT_S = 108,
    UIS_ADDR_ELEMENT_D = 109,
    UIS_ADDR_ELEMENT_D_THIS = 110,
    UIS_PATCH_STRING = 111,
    UIS_DOMODAL_PARAMS = 112,
    UIS_LOAD_SCREEN_PARAMS = 113,
    UIS_MOVE_SCREEN = 114,
    UIS_IS_TIMER_LOADED = 115,
    UIS_ENABLE_CONTROLLER = 116,
    UIS_IS_CONTROLLER_ENABLED = 117,
    UIS_HINT = 118,
    UIS_ISINGROUP = 119,
    UIS_GET_ACTIVE_SCREEN = 120,
} UISStackOpCode;

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
// The game's UI_RunGameMessage (Madden 2003's UISCallbackMessageFnc): command nCmd of screen
// (nGroup, nScreen) with nParams arguments; the arguments' and the answer's addresses as words.
typedef void UISMessageFncT(s32 nCmd, s32 nGroup, s32 nScreen, s32 nParams, s32 nArgsAddr, s32 nResultAddr);
// Returns the screen's UI file, still unfixed (the game's UI_ResLoad, which ignores the group).
typedef void* UISResLoadFncT(u16 uGroup, u16 uScreen);
typedef void UISResUnloadFncT(u16 uGroup, u16 uScreen, void* pData);
typedef void UISTransformFncT(int nOp, void* pDesc);            // the game's UITransform_HandleOp
// Never set by TW2004; STABS keep no parameter list.
typedef UISVectorT UISLocalizeFncT();
typedef void UISScreenActivatedFncT(u16 uGroup, u16 uScreen);
typedef void UISScreenDrawDebugFncT(u16 uGroup, u16 uScreen, s32 n);
// The table at UISInfoT.Plugins (Madden 2003's _BlankProcess(pObjData, ProcessID, nParam, pParam,
// pReturn)): pVar is the handler's variable in the screen file, nMsg the message, then nParams
// arguments at pParams and the answer's address as a word.
typedef void UISPluginFncT(void* pVar, s32 nMsg, s32 nParams, s32* pParams, s32 nReturnAddr);

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
    UISControlInfoT* pSubControlInfo;  // 0x10 (0x30): the control the variable belongs to
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
    UISScreenDrawDebugFncT* pScreenDrawDebugFnc;  // 0x28: UISRegisterScreenDrawDebugFnc
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
void UISProcessThreadAction(UISInfoT* pInfo, u8 bControlEventsOnly);
s32* _UISDoThreadAction(UISInfoT* pInfo, s32* pLocalThreadInfo, s32** pNextFrameThreadInfo);
s32 UISThreadProcessHints(UISInfoT* pInfo, u16 GroupID, u16 ScreenID);
void UISAddThreadAction(s16 GroupID, s16 ScreenID, UISInfoT* pInfo, UISThreadActionT Action,
                        UISThreadGroupInfoT* pInputThreadInfo, s32 nParms, const s32* pParms);
void UISRegisterRuntimeErrorFnc(UISRuntimeErrorFncT* pRuntimeErrorFnc);
void UISRemoveUnNessaryRateFncs(UISInfoT* pInfo);
void UISUnloadRateFnc(UISInfoT* pInfo, UISControlInfoT* pControlInfo, u32 RateFncID);
void UISLoadRateFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo, u32 RateFncID,
                    u8* pFnc, u32 MSRate);
void UISLoadAdvRateFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo,
                       UISControlInfoT* pSubControlInfo, u32 RateFncID, u8* pEndFnc, u8* pAcelFnc, u32 MSDur,
                       f32 targValue, u32 animType);
u32 UISFindRateFnc(UISInfoT* pInfo, UISControlInfoT* pControlInfo, u32 RateFncID);

// UISStack.c
// Runs a screen's script from pStackState (a bytecode interpreter).
s8 UISStackProcess(UISInfoT* pInfo, s32* pBeginStack, UISStackInfoT* pStackState, UISScreenT* pScreen,
                   UISControlInfoT* pControlInfo);

// UIStudio.c
void UISUpdateVisibility(UISInfoT* pInfo, UISScreenT* pScreen, s32 targType, void* pTarget,
                         s32 uNewVisibility);
void UISInternalActivateScreen(UISInfoT* pInfo, u8 bActivate, u16 GroupID, u16 ScreenID);
void UISInternalActivateControl(UISInfoT* pInfo, u8 bActivate, s32 iDir, UISControlInfoT* pControlInfo,
                                s32* pTableEntry, u16 uScreen, u16 uGroup);
void UISIdleProcess(UISInfoT* pInfo, u32 uEvent);

// UISApi.c
void UISDrawObjects(UISInfoT* pInfo, s32 NumTicks);
void UISProcessInternalEvents(UISInfoT* pInfo, UISStackInfoT* pStackInfo, int Channel, u32 Message,
                              s32 nParam, void* pParam, u8 AllScreens);
void UISProcessEvent(UISInfoT* pInfo, u32 Channel, s32 Message, s32 nParam, void* pParam, u8 AllScreens);
void UISGetActiveScreen(UISInfoT* pInfo, u16* pGroupID, u16* pScreenID);
void UISSetScreenActive(UISInfoT* pInfo, u16 GroupID, u16 ScreenID);
u8 UISInternalUnloadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iRetVal);
u8 UISInternalUnloadModal(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iRetVal);
s32 UISLoadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u8 nParams, s32* pParams);
u8 UISSetGlobalScript(UISInfoT* pInfo, UISScrDataT* pGlobalScriptData);
s32 _UISInternalLoad(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u8 bModal, u8 nParams, s32* pParams);
s32 UISInternalLoadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u16 ParentGroupID, u16 ParentScreenID,
                          u8 nParams, s32* pParams);
void UISRegisterPluginFnc(UISInfoT* pInfo, s32 PluginIndex, UISPluginFncT* pPluginFnc);
void UISRegisterTransformFncs(UISInfoT* pInfo, UISTransformFncT* pTransformFnc);
void UISRegisterResourceFncs(UISInfoT* pInfo, UISResLoadFncT* pLoadFnc, UISResUnloadFncT* pUnloadFnc);
void UISRegisterScreenDrawDebugFnc(UISInfoT* pInfo, UISScreenDrawDebugFncT* pScreenDrawDebugFnc);
void UISRegisterMessageFnc(UISInfoT* pInfo, UISMessageFncT* pMessageFnc);
void UISShutdown(UISInfoT* pInfo);
void UISInit(UISInfoT* pInfo, u32 MaxScreens, u32 MaxPlugins, u32 MaxRateFncs, u32 MaxModals, u32 StackSize,
             u32 RateStackSize, u32 MSPerTick);
u32 UISGetMemSize(u32 MaxScreens, u32 MaxPlugins, u32 MaxRateFncs, u32 MaxModals, u32 StackSize,
                  u32 RateStackSize);
s32 PatchScrData(UISScrDataT* pNewBase);
void _ParseRateFncs(UISInfoT* pInfo, u32 MSElapsed);

// UISScreen.c (0x8016A2D4-0x8016C718)
// Sends event Channel to node idxControl of a screen and the nodes it links to; *bIsControlActive
// gets the node's UISControlInfoT.IsEnabled.
s32 _ParseMaps(UISInfoT* pInfo, UISScreenT* pScreen, UISStackInfoT* pStackInfo, u32 idxControl, u32 Channel,
               u32 EventID, s32 nParam, s32* pParam, u8* bIsControlActive);
void _ParseObjects(UISInfoT* pInfo, UISScreenT* pScreen, u32 idxControl, s32 FncID);
void _ParseTransforms(UISInfoT* pInfo, int action, UISScreenT* pScreen, u32 idxControl);
void _ParseVisibility(UISInfoT* pInfo, UISScreenT* pScreen, s32 uNewVisibility, s32 uChangeType,
                      void* pChange, u8 bFirstPass);
s32 _DetermineVisibility(UISScreenT* pScreen, UISControlInfoT* pTarget, s32 contextType, void* pContext);
void _ParseInitialize(UISInfoT* pInfo, UISScreenT* pScreen, u32 idxControl, s32 FncID);
void UISProcessHint(UISInfoT* pInfo, u32 Hint, s32 nParms, s32* pParam);
void UISDoHint(UISInfoT* pInfo, u32 Hint, s32 nParms, s32* pParam);
void UISMoveScreenDrawPosition(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iDir);
UISControlInfoT* UISFindSiblingEnableControl(UISScreenT* pScreen, UISControlInfoT* pControlInfo);
void UISStringFormat(u32 pScrData, UISStringT* pString, UISStringT* pFormatStr, s32 nParam,
                     const UISParamT* pParam);
// The studio's printf: %c %s %d %i %u %f %x %X %p, with '-', '0', a width and a precision.
s32 UISSprintf(char* buf, s32 destSize, const char* fmt, s32 nParam, const UISParamT* pParam);
void UISSetColorAdditive(f32 r, f32 g, f32 b, f32 a);
void UISSetColorMultipler(f32 r, f32 g, f32 b, f32 a);
// Returns a pointer to the float of pControlInfo that Action (a rate function's
// AnimationData.iType) names.
f32* UISGetActionPtrValue(s32 Action, UISControlInfoT* pControlInfo);
// Runs pcEvent for node info pControlInfo with a call frame pushed on pStackInfo.
s32 UISExecuteFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo,
                  UISStackInfoT* pStackInfo, u8* pcEvent, s32 nParam, s32* pParam, u32 nAppend,
                  const s32* pAppend, u8 bUseChannel, s32 Channel, s32* pReturn);
// A node's handler scripts for an event, by kind (0x4000, plain with an ID, 0x8000); NULL for none.
// The event is a u32 (callers pass it unmasked; each function masks it to 16 bits).
u8* _UISFindHintPC(UISControlT* pControl, u32 HintID);
u8* UISFindSubControlEventPC(UISControlT* pControl, u16 idxControl, u32 EventID);
u8* UISFindEventPC(UISControlT* pControl, u32 EventID);
u16 UISFindScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID);

// Sends event Channel to the current screen, or to every screen when AllScreens is set; Message -8
// skips a screen being unloaded. UISProcessInternalEvents's body, which UIStudio.c has pasted in
// twice (the pasted copies keep this block layout, with the loop set-up after the loop).
static inline void UIStudio_Send(UISInfoT* pInfo, UISStackInfoT* pStackInfo, u32 Channel, s32 Message,
                                 s32 nParam, void* pParam, u8 AllScreens) {
    u32 idxScreen;
    u32 numScreens;
    UISScreenT* pScreen;
    u8 bProcess;

    if (AllScreens) {
        numScreens = pInfo->NumScreens;
        idxScreen = 0;
    } else {
        idxScreen = pInfo->ActiveScreenIdx;
        numScreens = idxScreen + 1;
        if (idxScreen == -1) return;
    }
    for (; idxScreen < numScreens; idxScreen++) {
        pScreen = &pInfo->Screens[idxScreen];
        // fake match: the original compares unsigned
        if ((u32)Message != -8 || pScreen->bWaitingToBeUnloaded != 1) {
            bProcess = 0;
            _ParseMaps(pInfo, pScreen, pStackInfo, 0, Channel, Message, nParam, pParam, &bProcess);
        }
    }
}

#endif
