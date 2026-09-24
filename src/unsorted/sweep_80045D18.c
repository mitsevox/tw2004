// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

extern BufferPoolBlock* lbl_80281D98;
void fn_80045E3C(int a);
void fn_80045E40(BufferPoolBlock* pBlock);
void fn_80045E60(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045E80(void);

void fn_80045D18(void);
void fn_80045D18(void) {
    lbl_80281D98 = fn_80045E80();
    fn_80012F34(1);
    fn_80045E60(lbl_80281D98);
    fn_80045E40(lbl_80281D98);
    fn_80045E3C(0);
}
