// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"
#include "core/startup.h"

void fn_80045EC0(BufferPoolBlock* pBlock);
void fn_80045EC0(BufferPoolBlock* pBlock) {
    DCInvalidateRange(pBlock->unk0, sizeof(pBlock->unk0));
    GXBeginDisplayList(pBlock->unk0, sizeof(pBlock->unk0));
    fn_80015624();
    pBlock->u1000 = GXEndDisplayList();
    DCFlushRange(pBlock->unk0, pBlock->u1000);
}
