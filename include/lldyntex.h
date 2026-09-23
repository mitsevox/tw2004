// lldyntex.h (our name): LLDynTex.c (EA's name, from its asserts), textures whose pixels the game
// rewrites while they are shown (GXInitTexObj on buffers it flushes with DCFlushRange). The menu
// golfer (FEgolferanim.c) drives it.

#ifndef LLDYNTEX_H
#define LLDYNTEX_H

#include "game_types.h"
#include "platform.h"

// Its state (0xA9C bytes, allocated by fn_8010A448); only what the code reads so far.
typedef struct DynTexState {
    void* p0;                   // 0x000  a 0x40-byte block allocated with it
    u8    unk4[4];
    s32   n8;                   // 0x008
    u8    unkC[0x96C - 0xC];
    s32   n96C;                 // 0x96C
    u8    unk970[4];
    u8    b974;                 // 0x974  set by fn_8010BFA0
    u8    b975;                 // 0x975  set once n978 reaches n984
    u8    unk976[2];
    s32   n978;                 // 0x978  fn_8010BFA0 adds its argument
    s32   n97C;                 // 0x97C  and keeps the last one here
    s32   n980;                 // 0x980
    s32   n984;                 // 0x984
    u8    unk988[0xA84 - 0x988];
    s32   nA84;                 // 0xA84
    u8    unkA88[0xA98 - 0xA88];
    s32   nA98;                 // 0xA98  fn_8010A448's argument
} DynTexState;
LAYOUT_ASSERT(DynTexState, 0xA9C);

extern DynTexState* lbl_80282488;

#endif
