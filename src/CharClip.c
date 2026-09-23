// CharClip.c (our name): choosing the clip a character plays.

#include "golfer.h"
#include "engine.h"

// Pick the character's clip for an animation group and style from its animation library, keyed
// also by the character's club class (+0x169C; class 1 looks up as 0) and +0x16D4. The lookup's
// fallback flags go to bits 0x200 / 0x400 of +0x168; the clip is kept at +0x1788.
void* Char_SetClip(u8* pChar, int nGroup, int nStyle, const char* pName) {
    u32   uFlags = 0;
    int   nClub  = *(s32*)(pChar + 0x169C);
    void* pClip;
    if (nClub == 1) nClub = 0;
    pClip = AnimLib_Pick(*(s32*)(pChar + 4), *(void**)(pChar + 0x3D8), nGroup, nStyle, nClub,
                         *(s32*)(pChar + 0x16D4), &uFlags, pName);
    if (uFlags & 1) {
        *(u32*)(pChar + 0x168) |= 0x200;
    } else {
        *(u32*)(pChar + 0x168) &= ~0x200;
    }
    if (uFlags & 2) {
        *(u32*)(pChar + 0x168) |= 0x400;
    } else {
        *(u32*)(pChar + 0x168) &= ~0x400;
    }
    *(void**)(pChar + 0x1788) = pClip;
    return pClip;
}
