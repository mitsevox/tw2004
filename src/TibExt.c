// TibExt.c (EA's name, from its asserts; TW06): the glue between EA's Tiburon SDK libraries (the
// EA Sports Bio's EASB.c, the shared file library) and the game: memory, the real-time clock, and
// the memory-card callbacks (lbl_80281970) the shared file library calls.

#include "engine.h"
#include "core/easb.h"
#include "Common/SharedFileIO.h"
#include "core/memcard.h"

void* TibExtMemAlloc(u32 uHeapID, u32 uSize, u32 uAlign) {
    return fn_80009B34(uSize, fn_8000A0B4(), uAlign, "TibExt.c", 42);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80122468(s32 nCardError);

void TibExtMemFree(u32 uHeapID, void* p, u32 uSize, u32 uAlign) {
    fn_80009E70(p);
}

void fn_8012214C(const char* pFile, int uLine, const char* pExpr, int unused) {
}

void fn_80122468(s32 nCardError) {
    lbl_80281970->nError = lbl_80194758[-nCardError];
}

void fn_80122488(s32 n) {
    lbl_80281970->n48 = n;
}

void fn_80122630(s32 nFile) {
    fn_80122468(fn_8009F488(nFile));
}

void fn_80122868(void) {
    fn_80122468(fn_8009F364());
}

s32 fn_8012288C(s32* p0, s32* p1) {
    *p1 = lbl_80281970->n48;
    *p0 = 2;
    return lbl_80281970->nError;
}

// ---- end of sweep code ----
