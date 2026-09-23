// frontend.h (our name): the front end (the HUD and menu screens) as the game code sees it.
// GameMessages.c sends its messages to the front end's handler; startUp.c uses it too. Also the
// menu UI's transform stack (uiTransform.c).

#ifndef GAME_FRONTEND_H
#define GAME_FRONTEND_H

#include "game_types.h"
#include "platform.h"

typedef struct FrontEnd {
    u8    unk0[4];
    void* pHandler;             // 0x4  where GameMessages.c sends its messages (fn_8016B09C)
    u8    unk8[4];
    void* pC;                   // 0xC  a block uiLoadFile.c frees (fn_8008F24C)
} FrontEnd;

extern FrontEnd* lbl_80281F1C;

// One value of a message: an int or a float (the mask passed with it says which), or a pointer.
typedef union MsgArg {
    s32   i;
    f32   f;
    void* p;
} MsgArg;

// Send message nMsg with nArgs values to a front-end handler (fn_8016B09C also sends through it).
void fn_8016B0F8(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

// Four words a UI element passes down its transform stack, copied as one struct (what they hold is
// not known yet).
typedef struct UIWords4 {
    u32 a[4];
} UIWords4;

// One level of the menu UI's transform stack (uiTransform.c). A pushed element's transform is
// multiplied into the matrix; the other fields keep running totals of what was applied.
typedef struct UITransform {
    f32      m[4][4];           // 0x00  row 3 is the translation
    UIWords4 w40;               // 0x40  copied from the element (UITransformDesc.w34)
    f32      f50[3];            // 0x50  the element's f44[0..2] / 511
    f32      f5C;               // 0x5C  sum of the elements' f44[3] / 511
    f32      f60;               // 0x60  sum of the third rotations, radians
    f32      f64;               // 0x64  sum of the pivots' x
    f32      f68;               // 0x68  sum of the pivots' y
    f32      f6C;               // 0x6C  product of the scales' x
    f32      f70;               // 0x70  product of the scales' y
    f32      f74;               // 0x74  sum of the moves' x
    f32      f78;               // 0x78  sum of the moves' y
} UITransform;
LAYOUT_ASSERT(UITransform, 0x7C);

// The stack itself: fn_8009349C allocates it (0x3E4 bytes: eight levels), fn_800934F8 frees it.
typedef struct UITransformStack {
    s32         nTop;           // 0x0  the current level
    UITransform aLevel[8];      // 0x4
} UITransformStack;
LAYOUT_ASSERT(UITransformStack, 0x3E4);

// The current level of the stack.
#define UI_TRANSFORM_TOP (lbl_80281F38->aLevel[lbl_80281F38->nTop])

extern UITransformStack* lbl_80281F38;
extern UITransform       lbl_801D91FC;      // a copy of the current level
extern f32               lbl_801D91F0[3];   // the UI view: field of view, tan of half of it, distance

// A UI element's transform, as pushed onto the stack (fn_80093280, op 1): moved, then rotated and
// scaled about a pivot.
typedef struct UITransformDesc {
    f32      vMove[3];          // 0x00
    f32      vPivot[3];         // 0x0C  moved to before rotating and scaling, and back after
    f32      vRotate[3];        // 0x18  degrees, one per axis
    f32      vScale[3];         // 0x24
    s32      n30;               // 0x30  cleared by op 3
    UIWords4 w34;               // 0x34
    f32      f44[4];            // 0x44  divided by 511 when pushed
} UITransformDesc;

#endif
