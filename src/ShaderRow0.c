// ShaderRow0.c (our name): the four shader-object hooks of row 0 of lbl_80188E88 (engine.h's
// ModuleHooks, +0x34..+0x40): make and free the object's dynamic rendering buffer
// (DynamicRenderingBuffer.c), draw it and fill it. The same code as ShaderRow19.c, and like that
// file nothing else: row 0's other hooks (+0x24..+0x2C) end GoShaderObjectContainer_OBFData_Gc.c.

#include "game_types.h"
#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8006FF80(void* arg0, void* arg1);
void fn_8006FFD4(u8* p);

void fn_8006FF80(void* arg0, void* arg1) {
    if (arg1 != NULL) {
        (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C((*(s32*)((u8*)(arg1) + 0)), (*(s32*)((u8*)(arg1) + 4)));
        return;
    }
    (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C(0x32, 1);
}

void fn_8006FFD4(u8* p) {
    fn_80070348(*(DynRenderBuffer**)(p + 0x4));
}

// ---- end of sweep code ----
