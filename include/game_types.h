#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// Basic types for the game-side units, which do not see the SDK headers. Under CodeWarrior s32 and
// u32 are `long`, as the original was built: the compiler treats int and long as different types,
// and some functions only match with long. Elsewhere they are the fixed-size <stdint.h> types, so a
// 64-bit port keeps 32-bit fields 32 bits wide. uptr is an unsigned integer as wide as a pointer, for
// address arithmetic (alignment, offsets stored in pointer fields).
#ifdef __MWERKS__
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
#endif
typedef float              f32;
typedef double             f64;
#ifndef NULL
#define NULL 0
#endif

#endif
