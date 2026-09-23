// TibExt.c (EA's name, from its asserts; TW06): the glue between EA's Tiburon SDK libraries (the
// EA Sports Bio's EASB.c, the shared file library) and the game: memory, the real-time clock, and
// the memory-card callbacks (lbl_80281970) the shared file library calls.

#include "engine.h"
#include "core/easb.h"

void* TibExtMemAlloc(u32 uHeapID, u32 uSize, u32 uAlign) {
    return fn_80009B34(uSize, fn_8000A0B4(), uAlign, "TibExt.c", 42);
}
