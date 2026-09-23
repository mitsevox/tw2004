#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// Basic types for the game-side (CodeWarrior) units, which do not see the SDK headers.
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef signed char    s8;
typedef short          s16;
typedef long           s32;
typedef unsigned long long u64;
typedef long long      s64;
typedef float          f32;
typedef double         f64;
#ifndef NULL
#define NULL 0
#endif

#endif
