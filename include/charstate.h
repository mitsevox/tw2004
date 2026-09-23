// charstate.h (our name): what char.c keeps beyond the character object in character.h: its
// globals and the small helpers it defines that other code also calls (the bit-array helpers, the
// vector copies).

#ifndef CHARSTATE_H
#define CHARSTATE_H

#include "character.h"

// A skin (Skin.c): a character's body or one of its attachments; only what the code reads.
typedef struct Skin {
    u8   unk0[0x10B4];
    void* p10B4;                // 0x10B4  } fn_800CCA40's and fn_800CCEA0's number of 8-byte
    u8   unk10B8[0x10C4 - 0x10B8];
    void* p10C4;                // 0x10C4  } entries each (the created golfer's are saved)
    u8   unk10C8[0x10D4 - 0x10C8];
    u32  u10D4;                 // 0x10D4  bit 1 set by fn_80019CEC; bit 2 tested by fn_80037708
} Skin;

s32  fn_800CCA40(Skin* pSkin);          // SkinPart.c: how many 8-byte entries p10B4 holds
s32  fn_800CCEA0(Skin* pSkin);          // SkinPart.c: and p10C4

// A pool of seven entries characters take (fn_8001A418) and give back (fn_8001A3B0).
typedef struct CharPoolEntry {
    void* p;                    // 0x0
    u8    bUsed;                // 0x4
    u8    pad5[3];
} CharPoolEntry;

typedef struct CharPool {
    CharPoolEntry a[7];         // 0x00
    s32   nEntries;             // 0x38
} CharPool;
LAYOUT_ASSERT(CharPool, 0x3C);

extern CharPool   lbl_801B95E8;
extern Character* lbl_801B9624[5];     // the characters made so far (fn_8001C21C)
extern s32        lbl_80281CA8;         // how many
extern s32        lbl_80281CAC;         // the player fn_8001D8DC last marked (-1 at start)
extern s32        lbl_80187164[8];      // the clip key for each shot kind (fn_8001C724)
extern s32        lbl_80280E20;         // set to 6 (4 in split screen) by fn_8001C254, 3 by fn_8001C304
extern void*      lbl_80280E24[2];      // what fn_8001B208 makes of the 'CLB ' object: one, or one per view
                                        // in split screen
extern Character* lbl_80281EE8;   // a character fn_8001C37C clears and fn_8001C518 frees

// Bit n of a bit array of 32-bit words: test, set, clear.
u8    fn_8001E9CC(u32* aBits, u32 n);
void  fn_8001EA34(u32* aBits, u32 n);
void  fn_8001EB6C(u32* aBits, u32 n);
u8    fn_8001E9F4(u32* aA, u32* aB, u32 nBits);  // two bit arrays share a set bit

// 4-vectors (the quaternion copy, fn_8001E85C, is in character.h).
void  fn_8001E880(f32* pSrc, f32* pDst);  // a point to a 4-vector with w = 1
f32   fn_8001EEA4(f32* pA, f32* pB);      // dot product

#endif
