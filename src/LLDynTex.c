// LLDynTex.c (EA's name, from its asserts): textures whose pixels the game rewrites while they are
// shown; the menu golfer (FEgolferanim.c) drives them. Not yet decompiled beyond the sweep code.

#include "engine.h"
#include "lldyntex.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8001052C(s16 n);
void fn_8010A668(DynTex* pTex);
void* fn_8010A780(DynTex* pTex);
s32 fn_8010AD10(DynTex* pTex);
void fn_8010B098(void* arg0);
s32 fn_8010C458(s16);
s32 fn_8010B664(void* arg0);
void fn_8000FBAC();
void fn_8010BC64(u8* p);

void fn_8010A4E8(void) {
    if (lbl_80282488->p0 != NULL) {
        fn_80009E70(lbl_80282488->p0);
    }
    fn_80009E70(lbl_80282488);
}

// Free a dynamic texture.
void fn_8010A668(DynTex* pTex) {
    fn_8001052C(pTex->n1C);
    fn_80009E70(pTex->p18);
    fn_80009E70(pTex);
}

void* fn_8010A780(DynTex* pTex) {
    return pTex->p4;
}

s32 fn_8010AD10(DynTex* pTex) {
    return pTex->n8;
}

void fn_8010B098(void* arg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 8)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 2)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 4)) = 0;
        (*(s32*)((u8*)(arg0) + 0x14)) = 0;
    }
}

s32 fn_8010B664(void* arg0) {
    s32 var_r3;

    if ((s16) (*(s16*)((u8*)(arg0) + 8)) > 0x10) {
        var_r3 = 0x40;
        if (fn_8010C458((*(s16*)((u8*)(arg0) + 0xA))) == 0x10) {
            return 0x20;
        }
        /* Duplicate return node #4. Try simplifying control flow for better match */
        return var_r3;
    }
    var_r3 = 4;
    return var_r3;
}

void fn_8010BC64(u8* p) {
    fn_8000FBAC(*(s32*)(p + 0x4));
}

void fn_8010BC88(s32 n) {
    lbl_80282488->n8 = n;
}

void fn_8010BEC4(void) {
    lbl_80282488->n96C = 0;
}

s32 fn_8010BF3C(void) {
    u8 b = 0;
    if (lbl_80282488->nA84 == 0 && lbl_80282488->n980 == 0) {
        b = 1;
    }
    return b;
}

void fn_8010BFA0(s32 n) {
    lbl_80282488->n97C = n;
    lbl_80282488->n978 = lbl_80282488->n978 + n;
    lbl_80282488->b974 = 1;
    if (lbl_80282488->n978 >= lbl_80282488->n984) {
        lbl_80282488->b975 = 1;
    }
}

// ---- end of sweep code ----
