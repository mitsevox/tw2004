#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// Basic types for the game-side units, which do not see the SDK headers. For the GameCube build
// (CodeWarrior, and ProDG for the Common library) s32 and u32 are `long`, as the original was
// built: CodeWarrior treats int and long as different types, and some functions only match with
// long. A port defines TW_PORT and gets the fixed-size <stdint.h> types instead, so a 64-bit build
// keeps 32-bit fields 32 bits wide. uptr is an unsigned integer as wide as a pointer, for address
// arithmetic (alignment, offsets stored in pointer fields).
#ifndef TW_PORT
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef signed char        s8;
typedef short              s16;
typedef long               s32;
typedef unsigned long long u64;
typedef long long          s64;
typedef u32                uptr;
#else
#include <stdint.h>
typedef uint8_t            u8;
typedef uint16_t           u16;
typedef uint32_t           u32;
typedef int8_t             s8;
typedef int16_t            s16;
typedef int32_t            s32;
typedef uint64_t           u64;
typedef int64_t            s64;
typedef uintptr_t          uptr;

// CodeWarrior's intrinsics, as plain C (port: untested). __cntlzw counts the leading zero bits.
static inline u32 __cntlzw(u32 x) {
    u32 n = 0;
    if (x == 0) {
        return 32;
    }
    while (!(x & 0x80000000u)) {
        x <<= 1;
        n++;
    }
    return n;
}
#endif
typedef float              f32;
typedef double             f64;
#ifndef NULL
#define NULL 0
#endif

#endif
