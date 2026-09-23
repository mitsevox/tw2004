// GoShaderObject_Glows_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet
// decompiled; the sweep code below is the matched small functions.

#include "glows.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8009809C();
void fn_800981D0();
void fn_80098350(void);

void fn_80098350(void) {
    fn_8009809C(8, 3);
    fn_8009809C(9, 4);
    fn_8009809C(10, 5);
    fn_8009809C(11, 6);
    fn_8009809C(12, 8);
    fn_8009809C(0, 10);
    fn_8009809C(1, 16);
    fn_8009809C(2, 24);
    fn_8009809C(3, 32);
    fn_800981D0(4, 1);
    fn_800981D0(5, 2);
    fn_800981D0(6, 3);
    fn_800981D0(7, 4);
}

// ---- end of sweep code ----

void fn_80098740(void) {
    s32 i;
    lbl_801D99D0.nCount = 0;
    for (i = 0; i < NUM_GLOWS; i++) {
        lbl_801D99D0.a[i].p4 = NULL;
    }
    fn_80098350();
}

// Free the glows' data. The pointers are left as they were.
void fn_800987D4(void) {
    int i;
    for (i = 0; i < NUM_GLOWS; i++) {
        if (lbl_801D99D0.a[i].p4 != NULL) {
            fn_80009E70(lbl_801D99D0.a[i].p4);
            lbl_801D99D0.nCount--;
        }
    }
}
