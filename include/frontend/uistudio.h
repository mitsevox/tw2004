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

// A loaded screen, as the rate functions see it.
typedef struct UISScreen {
    u8 unk0[4];
    u16 uGroup;                     // 0x04: the screen's group ID
    u16 uScreen;                    // 0x06: its ID within the group
    u8 unk8[4];
    s32 bUnloading;                 // 0x0C: set while the screen waits to be unloaded
} UISScreen;

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

// The studio itself (only the fields its code has shown so far).
typedef struct UIStudio {
    u8 unk0[4];
    u32 uFlags;                     // 0x04: bit 0 is set while the event stack is being run
    u32 u8;                         // 0x08: a rate function's time divided by this is its tick count
    u8 unkC[0x28];
    u32 u34;                        // 0x34
    u8 unk38[0x18];
    s32 nRateFns;                   // 0x50: rate functions in use
    UISRateFn* pRateFns;            // 0x54
    u8 unk58[0x54];
    s32* pEventBase;                // 0xAC: the bottom of the event stack; it grows down from here
    s32* pEventTop;                 // 0xB0: the stack's current top (fn_80165B90)
} UIStudio;

// Returns a pointer to the variable a rate function drives.
f32* fn_8016C1A4(s32 n20, s32 n30);

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
