// frontend.h (our name): the front end (the HUD and menu screens) as the game code sees it.
// GameMessages.c sends its messages to the front end's handler; startUp.c uses it too. Also the
// menu UI's transform stack (uiTransform.c).

#ifndef GAME_FRONTEND_H
#define GAME_FRONTEND_H

#include "game_types.h"
#include "platform.h"

// The menu UI's file (the 'DATS' object uiLoadFile.c keeps). Its tables hold offsets from the
// file's start until fn_8008F488 adds the file's address to them.
typedef struct UIFile {
    u32  u0;                    // 0x0
    u32* p4;                    // 0x4  a count, then that many pairs of words
    u32* p8;                    // 0x8  a count, then that many tables (each a count and its words)
} UIFile;

typedef struct FrontEnd {
    UIFile* pFile;              // 0x0
    void* pHandler;             // 0x4  where GameMessages.c sends its messages (fn_8016B09C)
    u8    unk8[4];
    void* pC;                   // 0xC  a block uiLoadFile.c frees (fn_8008F24C)
    u8    unk10[8];
    f32   f18;                  // 0x18  set to 1 when a round starts (gomainloop fn_8006DC20)
} FrontEnd;

extern FrontEnd* lbl_80281F1C;

// What uiLoadFile.c's stream handlers loaded (lbl_801D87A8): up to five objects, freed together
// by fn_8008F0FC.
#define UI_NUM_LOADED 5
typedef struct UILoaded {
    s32   nCount;               // 0x0  how many ap4 holds
    void* ap4[UI_NUM_LOADED];   // 0x4  from fn_8000FB88 (fn_8008EE1C)
} UILoaded;
LAYOUT_ASSERT(UILoaded, 0x18);

extern UILoaded lbl_801D87A8;

// Print nValue into szOut with a comma between every three digits ("1,234,567").
void fn_800907AC(int nValue, char* szOut);

// One value of a message: an int or a float (the mask passed with it says which), or a pointer.
typedef union MsgArg {
    s32   i;
    f32   f;
    void* p;
} MsgArg;

// A string value: the message holds a pointer to this.
typedef struct MsgString {
    s32   n0;
    s32   nLen;
    char* pStr;
} MsgString;

// A handler in the menus' message tables (FE_MessageTable.c, GameUICommands.c): the message's
// values, and where its answers go (an int or float, or a string to fill in).
typedef void (*MsgHandler)(MsgArg* pArgs, MsgArg* pResult);

// Send message nMsg with nArgs values to a front-end handler (fn_8016B09C also sends through it).
void fn_8016B0F8(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

// The menu UI's commands go to one of these, by the session's game type (uiProcessInterface.c's
// fn_8008F568): each runs the handler for message nMsg of its table.
void fn_80079E6C(int nMsg, MsgArg* pArgs, MsgArg* pResult);    // the menus (FE_MessageTable.c)
void fn_800850E4(int nMsg, MsgArg* pArgs, MsgArg* pResult);    // a round (GameUICommands.c)

// The round's handlers (GameUICommands.c): fn_80085120 fills the table. Entries 0 and 119 stay
// empty.
#define UI_NUM_ROUND_COMMANDS 214
extern MsgHandler lbl_801D83B0[UI_NUM_ROUND_COMMANDS];

// Menu handlers (FE_MessageTable.c) that the round's table also runs, or that round handlers
// pass on to.
void fn_8007E9BC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008299C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082DBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082E10(MsgArg* pArgs, MsgArg* pResult);
void fn_800834A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800834E8(MsgArg* pArgs, MsgArg* pResult);

extern u8 lbl_80281F18;         // set by the pause handler (GameUICommands.c fn_8008633C)

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
