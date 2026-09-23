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

// The callbacks the game hands the studio (uiProcessInterface.c sets them). Parameters come from
// the studio's calls, or where noted from the game's own callback.
typedef void (*UISCommandFn)(s32 nCmd, s32 n1, s32 n2, s32 n3, s32 n4, s32 n5);  // the game's fn_8008F568
typedef u32 (*UISLoadFn)(void* p, u16 u);                       // the game's fn_8008F610
typedef void (*UISUnloadFn)(u16 uGroup, u16 uScreen, void* pData);
typedef void (*UISTransformFn)(int nOp, void* pDesc);           // the game's fn_80093280
typedef void (*UISScreenFn)(u16 uGroup, u16 uScreen);
typedef void (*UISScreenDataFn)(u16 uGroup, u16 uScreen, s32 n);
typedef void (*UISHandlerFn)(void);                             // the table at UIStudio.ppfnHandlers

// A loaded screen (0x14 bytes, in the studio's screen table).
typedef struct UISScreen {
    u32 uMask;                      // 0x00: one bit per event kind the screen has already taken
    u16 uGroup;                     // 0x04: the screen's group ID
    u16 uScreen;                    // 0x06: its ID within the group
    u8 unk8[4];
    s32 bUnloading;                 // 0x0C: set while the screen waits to be unloaded
    void* pData;                    // 0x10: what the load callback returned; the unload callback gets it
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
    s32 n20;                        // 0x20: with n30, names the variable (fn_8016C1A4)
    f32 fTarget;                    // 0x24: the value it moves towards
    f32 fStep;                      // 0x28: the change per tick
    s32 n2C;                        // 0x2C
    s32 n30;                        // 0x30
} UISRateFn;
LAYOUT_ASSERT(UISRateFn, 0x34);

// A 0x28-byte record in UIStudio.p60; the last one in use names the current screen.
typedef struct UISRecord60 {
    u8 unk0[0x24];
    u16 u24;                        // 0x24: a screen ID (fn_8016C6C4's third argument)
    u16 u26;                        // 0x26: a group ID (its second)
} UISRecord60;
LAYOUT_ASSERT(UISRecord60, 0x28);

// A block of words the studio hands out (fn_80169C0C sets p0, p4 and pC to its start).
typedef struct UISWordStack {
    s32* p0;                        // 0x00: the start
    s32* p4;                        // 0x04
    s32* p8;                        // 0x08: the end
    s32* pC;                        // 0x0C
} UISWordStack;
LAYOUT_ASSERT(UISWordStack, 0x10);

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
    s32 nScreens;                   // 0x34
    UISScreen* pScreens;            // 0x38
    UISCurrent* pCurrent;           // 0x3C
    u32 nMaxHandlers;               // 0x40
    s32 nHandlers;                  // 0x44
    UISHandlerFn* ppfnHandlers;     // 0x48: filled by fn_80169B0C
    u32 nMaxRateFns;                // 0x4C
    s32 nRateFns;                   // 0x50: rate functions in use
    UISRateFn* pRateFns;            // 0x54
    u32 nMax60;                     // 0x58
    s32 n5C;                        // 0x5C: records in use in p60
    UISRecord60* p60;               // 0x60
    UISWordStack stack64;           // 0x64: the event words; the event stack grows down from its end
    u8 unk74[4];
    UISWordStack stack78;           // 0x78: a second block of words
    u8 unk88[0x24];
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

// The file after UISApi.c (0x8016A2D4-0x8016C718).
// Sends event uEvent to a screen; *pbOut is set by it.
void fn_8016A2D4(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, s32 n3, u32 uEvent, s32 n5, u8 b6,
                 void* p7, u8* pbOut);
void fn_8016A510(UIStudio* pStudio, UISScreen* pScreen, void* p, s32 n);
void fn_8016C15C(f32 f1, f32 f2, f32 f3, f32 f4);
void fn_8016C174(f32 f1, f32 f2, f32 f3, f32 f4);
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen);
// Returns a pointer to the variable a rate function drives.
f32* fn_8016C1A4(s32 n20, s32 n30);

#endif
