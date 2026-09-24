// frontend.h (our name): the front end (the HUD and menu screens) as the game code sees it.
// GameMessages.c sends its messages to the front end's handler; startUp.c uses it too. Also the
// menu UI's transform stack (uiTransform.c).

#ifndef GAME_FRONTEND_H
#define GAME_FRONTEND_H

#include "game_types.h"
#include "platform.h"

// An entry of the front end's colour table: p8 points at four bytes, alpha first (uiText.c).
// Every table in the UI file's second list holds entries of this shape; u0 is the entry's kind,
// 0x10 in the colour table (fn_8008FDDC).
// Movie entries (fe_movies.c) use the same shape: flags 1 a texture (p4 its data), 2 a movie
// (p8 its LLPict).
typedef struct UIColorEntry {
    u32  u0;                    // 0x0
    void* p4;                   // 0x4
    u8*  p8;                    // 0x8
    char szC[4];                // 0xC  its name (fn_8008FFF0 reads it); the length is not known
} UIColorEntry;

typedef struct UIColorTable {
    s32  nCount;                // 0x0  read as an s16
    UIColorEntry* apEntries[1]; // 0x4  nCount of them
} UIColorTable;

// A pair in the UI file's first list. fn_8008F610 hands p4 to the studio as a screen's data.
typedef struct UIFilePair {
    void* p0;                   // 0x0
    void* p4;                   // 0x4
} UIFilePair;

typedef struct UIFilePairs {
    u32  nCount;                // 0x0
    UIFilePair aPairs[1];      // 0x4  nCount of them
} UIFilePairs;

// The UI file's second list: its tables, one of them the colour table (fn_8008FDDC).
typedef struct UIFileTables {
    s32  nCount;                // 0x0
    UIColorTable* apTables[1];  // 0x4  nCount of them
} UIFileTables;

// The menu UI's file (the 'DATS' object uiLoadFile.c keeps). Its lists hold offsets from the
// file's start until fn_8008F488 adds the file's address to them.
// port: the file stores 32-bit offsets in these pointer fields and in the lists' pointers, and
// fn_8008F488 turns them into pointers in place; a 64-bit port must load the file into structs.
typedef struct UIFile {
    u32  u0;                    // 0x0
    UIFilePairs*  p4;           // 0x4  its pairs
    UIFileTables* p8;           // 0x8  its tables
} UIFile;

// The block at FrontEnd.pC: a count, then pointers to records that each start with a name;
// fn_8008FE88 gives a UI file entry of kind 2 the record of its name.
typedef struct UINamedList {
    u32   nCount;               // 0x0
    char* apNames[1];           // 0x4  nCount of them
} UINamedList;

typedef struct FrontEnd {
    UIFile* pFile;              // 0x0
    void* pHandler;             // 0x4  where GameMessages.c sends its messages (fn_8016B09C)
    struct UILoaded* p8;        // 0x8  the texture banks fn_8008F0FC frees (fn_80090400)
    UINamedList* pC;            // 0xC  a block uiLoadFile.c frees (fn_8008F24C)
    u32*  p10;                  // 0x10  the fonts table fn_8008F194 frees (fn_80090400)
    UIColorTable* p14;          // 0x14  the colours UIText.n8 picks from (uiText.c), NULL: none
    f32   f18;                  // 0x18  set to 1 when a round starts (gomainloop fn_8006DC20)
} FrontEnd;

extern FrontEnd* lbl_80281F1C;

// uiProcessInterface.c's controller input (fn_8008F820): which UI event each button sends.
typedef struct UIButtonEvent {
    u32 uMask;                  // 0x0  the button's bit in fn_800136DC's pressed-this-frame half
    s32 nEvent;                 // 0x4  the event fn_80168DB0 sends the UI
} UIButtonEvent;
#define UI_NUM_BUTTON_EVENTS 16
extern UIButtonEvent lbl_80189B58[UI_NUM_BUTTON_EVENTS];
extern s32 lbl_80189B38[8];     // per controller (0..3): frames fn_800142AC(0x20, 1)'s button is held
                                // in game type 6; past 10 fn_800E4F88 runs
extern s8 lbl_80281368;         // CrAPState.b86 put aside while fn_8008F820's lone-player UI is up (-1: none)

// What uiLoadFile.c's stream handlers loaded (lbl_801D87A8): up to five objects, freed together
// by fn_8008F0FC.
#define UI_NUM_LOADED 5
typedef struct UILoaded {
    s32   nCount;               // 0x0  how many ap4 holds
    void* ap4[UI_NUM_LOADED];   // 0x4  from fn_8000FB88 (fn_8008EE1C)
} UILoaded;
LAYOUT_ASSERT(UILoaded, 0x18);

extern UILoaded lbl_801D87A8;

// uiLoadFile.c: what the front end's shutdown (uiProcessInterface.c fn_80090400) frees.
void fn_8008F0C8(void* p);              // free p unless it is NULL
void fn_8008F0FC(UILoaded* pLoaded);    // free the texture banks the 'TXFS' handler kept
void fn_8008F164(void* p);              // free p unless it is NULL
void fn_8008F194(u32* pTable);          // free the fonts' slots and the 'FONS' data
void fn_8008F24C(void);
void fn_8008F294(void);

// One font in the 'FONS' object (uiLoadFile.c fn_8008EFFC): the font's data starts at 0x20; once
// it is loaded into a font slot, its first word holds the slot.
typedef struct UIFont {
    u8  unk0[0x20];
    s32 nSlot;                  // 0x20
} UIFont;

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

void fn_8007BCA4(MsgArg* pArgs, MsgArg* pResult);  // FE_MessageTable.c: fn_80079AD4 (DiscCheck.c
                                        // calls it directly, with no values)

// Messages to a front-end handler (FrontEnd.pHandler) go through fn_8016B0F8 (UISScreen.c; its
// prototype is in frontend/uistudio.h). The values are passed as an s32 array.

// A menu UI arc (uiArc.c): nSegments pieces from fStart to fEnd degrees, shaded from colour
// aColorA to aColorB. Only what the code reads so far.
typedef struct UIArc {
    s16  n0;                    // 0x00  } passed to fe_movies.c's fn_800913EC and fn_80091460
    s16  n2;                    // 0x02  } (-1: none)
    u8   unk4[4];
    u16  u8;                    // 0x08
    u16  uFlags;                // 0x0A
    s16  nSegments;             // 0x0C  message 8
    s16  nQuarterTurns;         // 0x0E  message 11: a rotation in degrees, as quarter turns 0..3
    u8   aColorA[4];            // 0x10  message 6: red, green, blue, alpha
    u8   aColorB[4];            // 0x14  message 7
    f32  v18[2];                // 0x18  message 5
    f32  v20[2];                // 0x20  message 1
    f32  v28[2];                // 0x28  message 2
    f32  fStart;                // 0x30  message 3: degrees
    f32  fEnd;                  // 0x34  message 4: degrees
} UIArc;

// A menu UI text element (uiText.c): a string drawn in a font, in a colour of its own or of the
// front end's colour table, optionally with a shadow. Only what the code reads so far.
typedef struct UIText {
    s32  nText;                 // 0x00  messages 3 and 4: its string (a MsgString), as an offset
                                //       from the element
    s16  n4;                    // 0x04  its font (fn_80012868)
    u8   unk6[2];
    s16  n8;                    // 0x08  message 8, low half: an entry of the colour table, -1: aColor
    s16  nA;                    // 0x0A  message 8, high half
    s16  nFlags;                // 0x0C  bits 0/1: message 5; 0x10: a shadow; 0x100/0x200: f30/f34
    s16  nE;                    // 0x0E  messages 16 and 17
    u8   aColor[4];             // 0x10  message 0: red, green, blue, alpha
    u8   aShadowColor[4];       // 0x14  message 1
    f32  v18[3];                // 0x18  message 6
    f32  f24;                   // 0x24  } passed to fn_80092C38 when it has a shadow
    f32  f28;                   // 0x28  }
    u8   unk2C[4];
    f32  f30;                   // 0x30  messages 18 and 20, as an int
    f32  f34;                   // 0x34  messages 19 and 21, as an int
} UIText;

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
extern u8 lbl_80281F19;         // (uiProcessInterface.c) FEgolferanim.c's fn_8008EB10 tests it

// Four floats a UI element passes down its transform stack, copied as one struct; fe_movies.c's
// fn_80090D28 scales its quad's colours by them / 511.
typedef struct UIWords4 {
    f32 a[4];
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

UITransform* fn_80093274(void);         // the current level (uiTransform.c)

// uiProcessInterface.c: for a UI name starting "tu", 1 in a lesson and -1 otherwise; else 0.
int fn_8008FFF0(const char* szName);

#endif
