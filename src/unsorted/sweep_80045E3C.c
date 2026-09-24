// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_80045EC0(BufferPoolBlock* pBlock);
void fn_80045F14(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045F18(void);
void fn_80045F38(BufferPoolBlock* pBlock);

void fn_80045E3C(int a);
void fn_80045E40(BufferPoolBlock* pBlock);
void fn_80045E60(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045E80(void);
void fn_80045EA0(BufferPoolBlock* pBlock);
void fn_80045E3C(int a) {
}

void fn_80045E40(BufferPoolBlock* pBlock) {
    fn_80045EC0(pBlock);
}

void fn_80045E60(BufferPoolBlock* pBlock) {
    fn_80045F14(pBlock);
}

BufferPoolBlock* fn_80045E80(void) {
    return fn_80045F18();
}

void fn_80045EA0(BufferPoolBlock* pBlock) {
    fn_80045F38(pBlock);
}
