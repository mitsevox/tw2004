// ShaderRow19.c (our name): the four hooks of row 19 of the shader-object table lbl_80188E88
// (engine.h's ModuleHooks, +0x34..+0x40): make and free the object's dynamic rendering buffer
// (0x8007018C..0x80070764), fill it and draw it. The same code as row 0's hooks at 0x8006FF80.
// Which shader object row 19 is, is not known yet.

#include "game_types.h"
#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

void fn_801247C0(void* arg0, void* arg1);
void fn_80124814(u8* p);

void fn_801247C0(void* arg0, void* arg1) {
    if (arg1 != NULL) {
        (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C((*(s32*)((u8*)(arg1) + 0)), (*(s32*)((u8*)(arg1) + 4)));
        return;
    }
    (*(DynRenderBuffer**)((u8*)(arg0) + 4)) = fn_8007018C(0x32, 1);
}

void fn_80124814(u8* p) {
    fn_80070348(*(DynRenderBuffer**)(p + 0x4));
}

// ---- end of sweep code ----
