#ifndef COMMON_TIBEXTMEM_H
#define COMMON_TIBEXTMEM_H

// The game's allocator as EA's shared file library calls it: with the library's allocator and the
// calling file and line. TibExt.c defines the pair (core/easb.h declares them for the game's own
// calls), takes the allocator as a heap id it never reads and ignores the file and line. Only the
// library's files include this header.

#include "game_types.h"

void* TibExtMemAlloc(void* pAllocator, u32 uSize, u32 uAlign, const char* pFile, int nLine);
void  TibExtMemFree(void* pAllocator, void* pBlock, u32 uSize, u32 uAlign);

#endif
