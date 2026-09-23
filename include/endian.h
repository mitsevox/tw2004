// endian.h (our name): reading the disc's and the memory card's data, which is big-endian (see
// docs/format-byteorder.md for every format and the ones that are little-endian and swapped).
//
// BE16(p), BE32(p) and BEF32(p) read a big-endian value at p. Under CodeWarrior they are the plain
// loads the game's code does (*(u32*)p and so on), so the code is the same byte for byte; anywhere
// else they assemble the value from its bytes, which works on a machine of either byte order and
// needs no alignment. Those portable versions are written for a port and are not tested yet.

#ifndef ENDIAN_H
#define ENDIAN_H

#include "game_types.h"

#ifdef __MWERKS__

#define BE16(p)  (*(u16*)(p))
#define BE32(p)  (*(u32*)(p))
#define BEF32(p) (*(f32*)(p))

#else

static u16 Endian_ReadBE16(const void* p) {
    const u8* b = (const u8*)p;
    return (u16)((b[0] << 8) | b[1]);
}

static u32 Endian_ReadBE32(const void* p) {
    const u8* b = (const u8*)p;
    return ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | (u32)b[3];
}

static f32 Endian_ReadBEF32(const void* p) {
    union {
        u32 u;
        f32 f;
    } v;
    v.u = Endian_ReadBE32(p);
    return v.f;
}

#define BE16(p)  Endian_ReadBE16(p)
#define BE32(p)  Endian_ReadBE32(p)
#define BEF32(p) Endian_ReadBEF32(p)

#endif

#endif
