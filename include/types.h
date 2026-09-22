#ifndef _TYPES
#define _TYPES

// Project-wide basic types. The SDK source under extern/sdk includes "types.h" and expects
// this file to pull in dolphin/types.h (u8, u32, f32, BOOL, ...).

#include <dolphin/types.h>

/* Short names used by the borrowed MSL source (extern/sdk/runtime). */
typedef unsigned int uint;
typedef signed short sshort;
typedef unsigned short ushort;
typedef signed char schar;
typedef unsigned char uchar;

#endif
