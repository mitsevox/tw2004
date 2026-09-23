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

// The colour offset and scale screen nodes are drawn with (UISScreen.c).
extern f32 lbl_80280628[4];
extern f32 lbl_80280638[4];

// A handler a screen node has for an event (8 bytes; UISScreen.c looks them up).
typedef struct UISHandler {
    u16 uFlags;                     // 0x00: 0x8000 and 0x4000 mark two kinds of handler; with
                                    //       neither, the bits under 0x2FFF are an ID
    u16 uEvent;                     // 0x02: the event it handles
    u32 u4;                         // 0x04: what a lookup returns (0: none)
} UISHandler;

// A node of a loaded screen (0x14 bytes).
typedef struct UISNode {
    u8 unk0[0xC];
    u32 nHandlers;                  // 0x0C
    UISHandler* pHandlers;          // 0x10
} UISNode;
LAYOUT_ASSERT(UISNode, 0x14);

// A loaded screen, as the rate functions see it.
typedef struct UISScreen {
    u8 unk0[4];
    u16 uGroup;                     // 0x04: the screen's group ID
    u16 uScreen;                    // 0x06: its ID within the group
    u8 unk8[4];
    s32 bUnloading;                 // 0x0C: set while the screen waits to be unloaded
    void* p10;                      // 0x10: the screen's loaded data
} UISScreen;
LAYOUT_ASSERT(UISScreen, 0x14);

// A rate function: moves one variable of a screen towards a target, a step every tick.
typedef struct UISRateFn {
    u32 uId;                        // 0x00: the rate function's ID
    s32 n4;                         // 0x04
    s32 n8;                         // 0x08
    s32 nC;                         // 0x0C
    u32 u10;                        // 0x10: the studio's u8 when the function was loaded
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

// A stack of words the studio keeps (0x14 bytes). fn_8016C270 pushes a handler's arguments at pTop.
typedef struct UISStack {
    s32* pBase;                     // 0x00
    s32* p4;                        // 0x04
    s32* pEnd;                      // 0x08
    s32* pTop;                      // 0x0C
    u32 u10;                        // 0x10
} UISStack;
LAYOUT_ASSERT(UISStack, 0x14);

// The studio itself (only the fields its code has shown so far).
typedef struct UIStudio {
    u8 unk0[4];
    u32 uFlags;                     // 0x04: bit 0 is set while the event stack is being run
    u32 u8;                         // 0x08: a rate function's time divided by this is its tick count
    u8 unkC[0x28];
    u32 nScreens;                   // 0x34: screens loaded
    UISScreen* pScreens;            // 0x38
    u8 unk3C[0x14];
    s32 nRateFns;                   // 0x50: rate functions in use
    UISRateFn* pRateFns;            // 0x54
    u8 unk58[0xC];
    UISStack stack64;               // 0x64: the script stack handlers run on (fn_8016C270)
    UISStack stack78;               // 0x78
    u8 unk8C[0x20];
    s32* pEventBase;                // 0xAC: the bottom of the event stack; it grows down from here
    s32* pEventTop;                 // 0xB0: the stack's current top (fn_80165B90)
} UIStudio;

// UISScreen.c
// Returns a pointer to the variable a rate function drives.
f32* fn_8016C1A4(s32 n20, s32 n30);
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs);
u32 fn_8016C5C4(UISNode* pNode, u16 uEvent);
u32 fn_8016C614(UISNode* pNode, u16 uId, u16 uEvent);
u32 fn_8016C674(UISNode* pNode, u16 uEvent);
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen);
void fn_8016C15C(f32 fR, f32 fG, f32 fB, f32 fA);
void fn_8016C174(f32 fR, f32 fG, f32 fB, f32 fA);
f32* fn_8016C18C(void);
f32* fn_8016C198(void);

// UISEvent.c
void fn_80165B90(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, const u32* pData, s32 nArgs, const s32* pArgs);
void fn_80165C6C(UISReportFn pfnReport);
void fn_80165C74(UIStudio* pStudio);
void fn_80165D2C(UIStudio* pStudio, u32 u18, u32 uId);
void fn_80165D90(UIStudio* pStudio, UISScreen* pScreen, u32 u18, u32 uId, s32 n4, u32 u10);
void fn_80165E9C(UIStudio* pStudio, UISScreen* pScreen, u32 u18, s32 n30, u32 uId, s32 n2C, s32 n4,
                 u32 uTime, f32 fTarget, s32 n20);
u32 fn_8016604C(UIStudio* pStudio, u32 u18, u32 uId);

#endif
