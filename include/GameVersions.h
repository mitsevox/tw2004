#ifndef _GAME_VERSIONS_H
#define _GAME_VERSIONS_H

// The SDK source under extern/sdk comes from the Metroid Prime decompilation and selects SDK
// revisions with these macros. TW2004 links the same SDK build as Prime Rev 0 (GM8E_00), so
// configure.py passes -DVERSION=0. Kept verbatim so extern/sdk stays identical to upstream.

#define VERSION_GM8E_00 0
#define VERSION_GM8E_01 1
#define VERSION_GM8E_48 2
#define VERSION_GM8P_00 3
#define VERSION_GM8J_00 4
#define VERSION_GM8E_02 5
#define VERSION_R3IJ_00 6
#define VERSION_R3ME_00 7
#define VERSION_R3MP_00 8

#endif // _GAME_VERSIONS_H
