#ifndef FRONTEND_UISTUDIO_TIBURON_H
#define FRONTEND_UISTUDIO_TIBURON_H

// EA Tiburon UIStudio / IStudio Library definitions
// Recovered directly from STABS debug info in EA Tiburon builds (Madden NFL 2003 / NCAA Football 2003)

#include "game_types.h"
#include "platform.h"

#ifndef _TIBURON_TYPES_
#define _TIBURON_TYPES_
typedef unsigned char   Uint8;
typedef signed char     Int8;
typedef short           Int16;
typedef unsigned short  Uint16;
typedef int             Int32;
typedef unsigned int    Uint32;
typedef float           Float32;
typedef char*           String;
typedef int             Bool;
#endif

// ---- Forward Declarations ---------------------------------------------------

typedef struct UISInfo_t UISInfo_t;
typedef UISInfo_t UISInfoT;

typedef struct UISScreen_t UISScreen_t;
typedef UISScreen_t UISScreenT;

typedef struct UISScrData_t UISScrData_t;
typedef UISScrData_t UISScrDataT;

typedef struct UISControl_t UISControl_t;
typedef UISControl_t UISControlT;

typedef struct UISControlInfo_t UISControlInfo_t;
typedef UISControlInfo_t UISControlInfoT;

typedef struct UISLayer_t UISLayer_t;
typedef UISLayer_t UISLayerT;

typedef struct UISLayerInfo_t UISLayerInfo_t;
typedef UISLayerInfo_t UISLayerInfoT;

typedef struct UISObj_t UISObj_t;
typedef UISObj_t UISObjT;

typedef struct UISMap_t UISMap_t;
typedef UISMap_t UISMapT;

typedef struct UISControlList_t UISControlList_t;
typedef UISControlList_t UISControlListT;

typedef struct UISRateFnc_t UISRateFnc_t;
typedef UISRateFnc_t UISRateFncT;

typedef struct UISPlugin_t UISPlugin_t;
typedef UISPlugin_t UISPluginT;

typedef struct UISStackInfo_t UISStackInfo_t;
typedef UISStackInfo_t UISStackInfoT;

typedef struct UISModalStack_t UISModalStack_t;
typedef UISModalStack_t UISModalStackT;

typedef struct UISLocalThreadInfo_t UISLocalThreadInfo_t;
typedef UISLocalThreadInfo_t UISLocalThreadInfoT;

typedef union UISParam_t UISParam_t;
typedef UISParam_t UISParamT;

typedef struct UISString_t UISString_t;
typedef UISString_t UISStringT;

typedef struct UISArray_t UISArray_t;
typedef UISArray_t UISArrayT;

typedef struct UISVector_t UISVector_t;
typedef UISVector_t UISVectorT;

typedef struct UISColorVector_t UISColorVector_t;
typedef UISColorVector_t UISColorVectorT;

typedef struct UISTransform_t UISTransform_t;
typedef UISTransform_t UISTransformT;

typedef struct UISAnimateData_t UISAnimateData_t;
typedef UISAnimateData_t UISAnimateDataT;

typedef struct UISThreadGroupInfoScreen_t UISThreadGroupInfoScreen_t;
typedef UISThreadGroupInfoScreen_t UISThreadGroupInfoScreenT;

typedef struct UISThreadGroupInfoActivate_t UISThreadGroupInfoActivate_t;
typedef UISThreadGroupInfoActivate_t UISThreadGroupInfoActivateT;

typedef struct UISThreadGroupInfoMessage_t UISThreadGroupInfoMessage_t;
typedef UISThreadGroupInfoMessage_t UISThreadGroupInfoMessageT;

typedef struct UISThreadGroupInfoGeneric_t UISThreadGroupInfoGeneric_t;
typedef UISThreadGroupInfoGeneric_t UISThreadGroupInfoGenericT;

typedef union UISThreadGroupInfoT UISThreadGroupInfoT;

// ---- Basic Structs & Types --------------------------------------------------

typedef enum UISTransformAction_t {
    UISTransformInit = 0,
    UISTransformPush = 1,
    UISTransformPop = 2,
    UISTransformShutdown = 3
} UISTransformAction_t;
typedef UISTransformAction_t UISTransformAction;

struct UISString_t {
    Uint32 ID;                      // 0x00
    Uint32 length;                  // 0x04
    String ptr;                     // 0x08
};
LAYOUT_ASSERT(UISString_t, 0x0C);

struct UISArray_t {
    Uint32 nDimensions;             // 0x00
    Uint32 Size[1];                 // 0x04
};
LAYOUT_ASSERT(UISArray_t, 0x08);

struct UISVector_t {
    Float32 x;                      // 0x00
    Float32 y;                      // 0x04
    Float32 z;                      // 0x08
};
LAYOUT_ASSERT(UISVector_t, 0x0C);

struct UISColorVector_t {
    Float32 r;                      // 0x00
    Float32 g;                      // 0x04
    Float32 b;                      // 0x08
    Float32 a;                      // 0x0C
};
LAYOUT_ASSERT(UISColorVector_t, 0x10);

struct UISTransform_t {
    UISVectorT Offset;              // 0x00
    UISVectorT Pivot;               // 0x0C
    UISVectorT Rotation;            // 0x18
    UISVectorT Scale;               // 0x24
    void *pGlobalInfo;              // 0x30
    UISColorVectorT MultiplerFactor;// 0x34
    UISColorVectorT AdditiveFactor; // 0x44
};
LAYOUT_ASSERT(UISTransform_t, 0x54);

union UISParam_t {
    Float32 fValue;                 // 0x00
    Float32 *pfValue;               // 0x00
    Int32 iValue;                   // 0x00
    Int32 *piValue;                 // 0x00
    UISStringT *strAddr;            // 0x00
    UISArrayT *pArray;              // 0x00
};
LAYOUT_ASSERT(UISParam_t, 0x04);

// ---- Callback Types ---------------------------------------------------------

typedef void (*UISRuntimeErrorFncT)(Int32 nLevel, const char* szFile, Int32 nLine, const char* szMsg);
typedef void* (*UISResLoadFncT)(Uint16 GroupID, Uint16 ScreenID);
typedef void (*UISResUnloadFncT)(Uint16 GroupID, Uint16 ScreenID, void* pData);
typedef void (*UISMessageFncT)(Int32 nCmd, Int32 n1, Int32 n2, Int32 n3, Int32 n4, Int32 n5);
typedef void (*UISTransformFncT)(Int32 nOp, void* pDesc);
typedef UISVectorT (*UISLocalizeFncT)(void);
typedef void (*UISPluginFncT)(void* pVar, Int32 nMsg, Int32 n2, Int32* pn3, Int32 n4);
typedef void (*UISScreenActivatedFncT)(Uint16 GroupID, Uint16 ScreenID);
typedef void (*UISScreenDrawDebugFncT)(Uint16 GroupID, Uint16 ScreenID, Int32 n);

// ---- Screen Object Model ----------------------------------------------------

struct UISObj_t {
    Uint16 PluginIndex;             // 0x00
    Int16 bInitialized;             // 0x02
    void *pData;                    // 0x04
};
LAYOUT_ASSERT(UISObj_t, 0x08);

struct UISControlList_t {
    Int32 nList;                    // 0x00
    Int32 ThisObject;               // 0x04
    Int32 Offsets[1];               // 0x08
};

struct UISLayerInfo_t {
    Uint32 IsVisible;               // 0x00
};
LAYOUT_ASSERT(UISLayerInfo_t, 0x04);

struct UISLayer_t {
    UISLayerInfoT *pLayerInfo;      // 0x00
    Uint32 NumObjs;                 // 0x04
    UISObjT *Objs;                  // 0x08
};
LAYOUT_ASSERT(UISLayer_t, 0x0C);

struct UISMap_t {
    Uint16 ControlIndex;            // 0x00
    Uint16 EventID;                 // 0x02
    Uint8 *pFnc;                    // 0x04
};
LAYOUT_ASSERT(UISMap_t, 0x08);

struct UISControlInfo_t {
    Uint32 IsVisible;               // 0x00
    Uint32 IsEnabled;               // 0x04
    UISTransformT Transform;        // 0x08
    Int32 idxFocusChild;            // 0x5C
    Uint32 CanHandleMessages;       // 0x60
};
LAYOUT_ASSERT(UISControlInfo_t, 0x64);

struct UISControl_t {
    UISControlInfoT *pControlInfo;  // 0x00
    Uint32 NumLayers;               // 0x04
    UISLayerT **Layers;             // 0x08
    Uint32 NumMaps;                 // 0x0C
    UISMapT *Maps;                  // 0x10
};
LAYOUT_ASSERT(UISControl_t, 0x14);

struct UISScrData_t {
    Uint32 NumControls;             // 0x00
    UISControlT *Controls;          // 0x04
    Uint32 NumStrings;              // 0x08
    UISStringT *Strings;            // 0x0C
    Uint32 NumPatchStrings;         // 0x10
    Uint32 *StringPatchTable;       // 0x14
    Uint32 NumStaticObjects;        // 0x18
    UISObjT *StaticObjects;         // 0x1C
};
LAYOUT_ASSERT(UISScrData_t, 0x20);

struct UISScreen_t {
    Uint32 ControllerDisable;       // 0x00
    Uint16 GroupID;                 // 0x04
    Uint16 ScreenID;                // 0x06
    Uint16 ParentGroupID;           // 0x08
    Uint16 ParentScreenID;          // 0x0A
    Int32 bWaitingToBeUnloaded;     // 0x0C
    UISScrDataT *pScrData;          // 0x10
};
LAYOUT_ASSERT(UISScreen_t, 0x14);

// ---- Rate Functions & Animation ---------------------------------------------

enum {
    UISRATE_LOAD = 0,
    UISRATE_UNLOAD = 1,
    UISRATE_ACTIVE = 2
};

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
    UIS_ACTION_MULTIPLY_BLUE = 39
};

struct UISAnimateData_t {
    Uint32 iType;                   // 0x00
    Float32 fEndValue;              // 0x04
    Float32 fStepValue;             // 0x08
    Uint8 *pEndFnc;                 // 0x0C
    UISControlInfoT *pSubControlInfo; // 0x10
};
LAYOUT_ASSERT(UISAnimateData_t, 0x14);

struct UISRateFnc_t {
    Uint32 RateFncID;               // 0x00
    Uint8 *pFnc;                    // 0x04
    Uint32 MSCount;                 // 0x08
    Uint32 MSLastCount;             // 0x0C
    Uint32 MSRate;                  // 0x10
    Uint32 State;                   // 0x14
    UISControlInfoT *pControlInfo;  // 0x18
    UISScreenT *pScreen;            // 0x1C
    UISAnimateDataT AnimationData;  // 0x20
};
LAYOUT_ASSERT(UISRateFnc_t, 0x34);

struct UISPlugin_t {
    UISPluginFncT *pFnc;            // 0x00
};
LAYOUT_ASSERT(UISPlugin_t, 0x04);

// ---- Stack & Thread Execution -----------------------------------------------

struct UISStackInfo_t {
    UISParamT *pStackStart;         // 0x00
    UISParamT *pStackCurrent;       // 0x04
    UISParamT *pStackEnd;           // 0x08
    UISParamT *pStack;              // 0x0C
    Uint8 *pPC;                     // 0x10
};
LAYOUT_ASSERT(UISStackInfo_t, 0x14);

struct UISModalStack_t {
    UISStackInfoT StackState;       // 0x00
    UISControlInfoT *pControlInfo;  // 0x14
    UISScreenT *pScreen;            // 0x18
    UISParamT *pRestoreStack;       // 0x1C
    UISStackInfoT *pRestoreState;   // 0x20
    Uint16 ScreenID;                // 0x24
    Uint16 GroupID;                 // 0x26
};
LAYOUT_ASSERT(UISModalStack_t, 0x28);

struct UISLocalThreadInfo_t {
    UISParamT *pBeginParams;        // 0x00
    UISParamT *pCurrentParams;      // 0x04
    UISParamT **ppEndParams;        // 0x08
};
LAYOUT_ASSERT(UISLocalThreadInfo_t, 0x0C);

struct UISInfo_t {
    Uint32 InfoID;                  // 0x00
    Uint32 CriticalRegions;         // 0x04
    Uint32 MSPerTick;               // 0x08
    UISMessageFncT *pMessageFnc;    // 0x0C
    UISResUnloadFncT *pShutdownScreenFnc; // 0x10
    UISResLoadFncT *pLoadFnc;       // 0x14
    UISResUnloadFncT *pUnloadFnc;   // 0x18
    UISTransformFncT *pTransformFnc;// 0x1C
    UISLocalizeFncT *pLocalizeFnc;  // 0x20
    UISScreenActivatedFncT *pScreenActivatedFnc; // 0x24
    UISScreenDrawDebugFncT *pScreenDrawDebugFnc; // 0x28
    Uint32 ActiveScreenIdx;         // 0x2C
    Uint32 MaxScreens;              // 0x30
    Uint32 NumScreens;              // 0x34
    UISScreenT *Screens;            // 0x38
    UISScreenT *pGlobalScript;      // 0x3C
    Uint32 MaxPlugins;              // 0x40
    Uint32 NumPlugins;              // 0x44
    UISPluginT *Plugins;            // 0x48
    Uint32 MaxRateFncs;             // 0x4C
    Uint32 NumRateFncs;             // 0x50
    UISRateFncT *RateFncs;          // 0x54
    Uint32 MaxModals;               // 0x58
    Uint32 NumModals;               // 0x5C
    UISModalStackT *ModalStack;     // 0x60
    UISStackInfoT EventStack;       // 0x64
    UISStackInfoT RateStack;        // 0x78
    UISColorVectorT MultiplerFactor;// 0x8C
    UISColorVectorT AdditiveFactor; // 0x9C
    UISLocalThreadInfoT ThreadInfo; // 0xAC
    Uint32 bShuttingDown;           // 0xB8
};
LAYOUT_ASSERT(UISInfo_t, 0xBC);

// ---- Symbol Enums -----------------------------------------------------------

enum {
    OBJTYPE_SYMBOL_LAYER = 7,
    OBJTYPE_SYMBOL_CONTROL = 8,
    OBJTYPE_SYMBOL_SCREEN = 10
};

typedef enum UISStackOpCode {
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
    UIS_GET_ACTIVE_SCREEN = 120
} UISStackOpCode;

enum {
    UISPROCESS_CONTINUE = 0,
    UIS_PROCESSSUBONLY = 1,
    UISPROCESS_HARDABORT = 2,
    UISPROCESS_DOMODAL = 3
};

typedef enum UISThreadActionT {
    UISThreadAction_Load = 0,
    UISThreadAction_Unload = 1,
    UISThreadAction_Update = 2,
    UISThreadAction_ScreenActivate = 3,
    UISThreadAction_ScreenDeactivate = 4,
    UISThreadAction_ControlActivate = 5,
    UISThreadAction_ControlDeactivate = 6,
    UISThreadAction_ProcessEvent = 7,
    UISThreadAction_MoveScreen = 8,
    UISThreadAction_HINT = 9
} UISThreadActionT;

struct UISThreadGroupInfoScreen_t {
    Uint16 GroupID;                 // 0x00
    Uint16 ScreenID;                // 0x02
    Uint16 ParentGroupID;           // 0x04
    Uint16 ParentScreenID;          // 0x06
    Uint32 iRetVal;                 // 0x08
    Int32 iDir;                     // 0x0C
};
LAYOUT_ASSERT(UISThreadGroupInfoScreen_t, 0x10);
typedef UISThreadGroupInfoScreen_t UISThreadGroupInfoScreenT;

struct UISThreadGroupInfoActivate_t {
    Int16 iDir;                     // 0x00
    Int16 iProcessed;               // 0x02
    UISControlListT *pTableEntry;   // 0x04
    UISControlInfoT *pControlInfo;  // 0x08
    Uint16 ScreenID;                // 0x0C
    Uint16 GroupID;                 // 0x0E
};
LAYOUT_ASSERT(UISThreadGroupInfoActivate_t, 0x10);
typedef UISThreadGroupInfoActivate_t UISThreadGroupInfoActivateT;

struct UISThreadGroupInfoMessage_t {
    Uint32 Message;                 // 0x00
    Uint32 Controller;              // 0x04
    Uint32 pad1;                    // 0x08
    Uint32 pad2;                    // 0x0C
};
LAYOUT_ASSERT(UISThreadGroupInfoMessage_t, 0x10);
typedef UISThreadGroupInfoMessage_t UISThreadGroupInfoMessageT;

struct UISThreadGroupInfoGeneric_t {
    Uint32 Data[4];                 // 0x00
};
LAYOUT_ASSERT(UISThreadGroupInfoGeneric_t, 0x10);
typedef UISThreadGroupInfoGeneric_t UISThreadGroupInfoGenericT;

typedef union UISThreadGroupInfoT {
    UISThreadGroupInfoMessageT MessageInfo;
    UISThreadGroupInfoActivateT ActivateInfo;
    UISThreadGroupInfoScreenT ScreenInfo;
    UISThreadGroupInfoGenericT GenericInfo;
} UISThreadGroupInfoT;
LAYOUT_ASSERT(UISThreadGroupInfoT, 0x10);

// ---- Prototypes -------------------------------------------------------------

// UIStudio.c
Uint32 UISGetMemSize(Uint32 MaxScreens, Uint32 MaxPlugins, Uint32 MaxRateFncs, Uint32 MaxModals, Uint32 StackSize, Uint32 RateStackSize);
void UISInit(UISInfoT *pInfo, Uint32 MaxScreens, Uint32 MaxPlugins, Uint32 MaxRateFncs, Uint32 MaxModals, Uint32 StackSize, Uint32 RateStackSize, Uint32 MSPerTick);
void UISShutdown(UISInfoT *pInfo);
void UISRegisterMessageFnc(UISInfoT *pInfo, UISMessageFncT *pMessageFnc);
void UISRegisterResourceFncs(UISInfoT *pInfo, UISResLoadFncT *pLoadFnc, UISResUnloadFncT *pUnloadFnc);
void UISRegisterShutdownFnc(UISInfoT *pInfo, UISResUnloadFncT *pShutdownScreenFnc);
void UISRegisterTransformFncs(UISInfoT *pInfo, UISTransformFncT *pTransformFnc);
void UISRegisterLocalizeFnc(UISInfoT *pInfo, UISLocalizeFncT *pLocalizeFnc);
void UISRegisterScreenActivatedFnc(UISInfoT *pInfo, UISScreenActivatedFncT *pScreenActivatedFnc);
void UISRegisterScreenDrawDebugFnc(UISInfoT *pInfo, UISScreenDrawDebugFncT *pScreenDrawDebugFnc);
void UISRegisterPluginFnc(UISInfoT *pInfo, Uint32 PluginIndex, UISPluginFncT *pPluginFnc);
Bool UISInternalLoadScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Uint16 ParentGroupID, Uint16 ParentScreenID, Uint8 nParams, UISParamT *pParams);
Bool UISSetGlobalScript(UISInfoT *pInfo, void *pGlobalScriptData);
Bool UISLoadScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Uint8 nParams, UISParamT *pParams);
Bool UISPopupScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Uint8 nParams, UISParamT *pParams);
Bool UISInternalUnloadScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Int32 iRetVal);
void UISUnloadScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Bool bImmediately);
void UISSetScreenActive(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID);
void UISGetActiveScreen(UISInfoT *pInfo, Uint16 *pGroupID, Uint16 *pScreenID);
void UISProcessEvent(UISInfoT *pInfo, Int32 Channel, Uint32 Message, Uint32 nParam, UISParamT *pParam, Bool AllScreens);
void UISProcessInternalEvents(UISInfoT *pInfo, UISStackInfoT *pStackInfo, Int32 Channel, Uint32 Message, Uint32 nParam, UISParamT *pParam, Bool AllScreens);
void UISDrawObjects(UISInfoT *pInfo, Uint32 NumTicks);
void UISIdleProcess(UISInfoT *pInfo, Uint32 NumTicks);
void* UISFindStaticObject(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Uint32 PluginIndex);
void UISInternalActivateControl(UISInfoT *pInfo, Bool bActivate, Uint32 iDir, UISControlInfoT *pControlInfo, UISControlListT *pTableEntry, Uint16 GroupID, Uint16 ScreenID);
void UISInternalActivateScreen(UISInfoT *pInfo, Bool bActivate, Uint16 GroupID, Uint16 ScreenID);
void UISUpdateVisibility(UISInfoT *pInfo, UISScreenT *pScreen, Uint32 targType, void *pTarget, Uint32 uNewVisibility);
Bool UISIsActiveScreenEnabled(UISInfoT *pInfo);
Bool UISIsShuttingDown(UISInfoT *pInfo);
Bool UISSetScreenParent(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Uint16 GroupParentID, Uint16 ScreenParentID);
Int32 PatchScrData(UISScrDataT *pNewBase);

// UISEvent.c
Uint32 UISFindRateFnc(UISInfoT *pInfo, UISControlInfoT *pControlInfo, Uint32 RateFncID);
void UISLoadAdvRateFnc(UISInfoT *pInfo, UISScreenT *pScreen, UISControlInfoT *pControlInfo, UISControlInfoT *pSubControlInfo, Uint32 RateFncID, Uint8 *pEndFnc, Uint8 *pAcelFnc, Uint32 MSDur, Float32 targValue, Uint32 animType);
void UISLoadRateFnc(UISInfoT *pInfo, UISScreenT *pScreen, UISControlInfoT *pControlInfo, Uint32 RateFncID, Uint8 *pFnc, Uint32 MSRate);
void UISUnloadRateFnc(UISInfoT *pInfo, UISControlInfoT *pControlInfo, Uint32 RateFncID);
void UISRemoveUnNessaryRateFncs(UISInfoT *pInfo);

// UISStack.c
Int8 UISStackProcess(UISInfoT *pInfo, UISParamT *pBeginStack, UISStackInfoT *pStackState, UISScreenT *pScreen, UISControlInfoT *pControlInfo);

// UISUtils.c
Uint32 UISFindScreen(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID);
void* UISFindEventPC(UISControlT *pControl, Uint32 EventID);
void* UISFindSubControlEventPC(UISControlT *pControl, Uint16 idxControl, Uint32 EventID);
Int32 UISExecuteFnc(UISInfoT *pInfo, UISScreenT *pScreen, UISControlInfoT *pControlInfo, UISStackInfoT *pStackInfo, void *pcEvent, Uint32 nParam, UISParamT *pParam, Uint32 nAppend, UISParamT *pAppend, int bUseChannel, Int32 Channel, UISParamT *pReturn);
Float32* UISGetActionPtrValue(Uint32 Action, UISControlInfoT *pControlInfo);
Uint32 UISIsScreenLoaded(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID);
UISColorVectorT* UISGetColorMultipler(void);
UISColorVectorT* UISGetColorAdditive(void);
void UISSetColorMultipler(Float32 r, Float32 g, Float32 b, Float32 a);
void UISSetColorAdditive(Float32 r, Float32 g, Float32 b, Float32 a);
int UISSprintf(char *buf, int destSize, char *fmt, Uint32 nParam, UISParamT *pParam);
void UISStringFormat(UISScrDataT *pScrData, UISStringT *pString, UISStringT *pFormatStr, Uint32 nParam, UISParamT *pParam);
UISControlInfoT* UISFindSiblingEnableControl(UISScreenT *pScreen, UISControlInfoT *pControlInfo);
void UISMoveScreenDrawPosition(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID, Int32 iDir);
void UISDoHint(UISInfoT *pInfo, Uint32 Hint, Uint32 nParms, UISParamT *pParam);
void UISProcessHint(UISInfoT *pInfo, Uint32 Hint, Uint32 nParms, UISParamT *pParam);
Bool UISAreEventsEnabled(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID);

// UISError.c
void UISRegisterRuntimeErrorFnc(UISRuntimeErrorFncT *pRuntimeErrorFnc);

// UISActionProcess.c
void UISAddThreadAction(Int16 GroupID, Int16 ScreenID, UISInfoT *pInfo, UISThreadActionT Action, UISThreadGroupInfoT *pInputThreadInfo, Int32 nParms, UISParamT *pParms);
Bool UISThreadProcessHints(UISInfoT *pInfo, Uint16 GroupID, Uint16 ScreenID);
void UISProcessThreadAction(UISInfoT *pInfo, Bool bControlEventsOnly);

#endif // FRONTEND_UISTUDIO_TIBURON_H
