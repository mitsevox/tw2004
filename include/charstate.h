// charstate.h (our name): what char.c keeps beyond the character object in character.h: its
// globals and the small helpers it defines that other code also calls (the bit-array helpers, the
// vector copies).

#ifndef CHARSTATE_H
#define CHARSTATE_H

#include "character.h"

extern Character* lbl_801B9624[5];     // the characters made so far (fn_8001C21C)
extern s32        lbl_80281CA8;         // how many
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
