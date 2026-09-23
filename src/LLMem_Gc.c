// LLMem_Gc.c (our name, after TW06's legacy/ll/xbox/llmem_xbox.c, which holds Mem_cpy): the
// engine's memory copy, fill and compare. Only the compare is decompiled so far.

#include "engine.h"

int fn_80005BC8(const void* pA, const void* pB, u32 uLen) {
    return memcmp(pA, pB, uLen);
}
