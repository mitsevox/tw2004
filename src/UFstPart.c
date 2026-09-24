// UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree): the particle emitters
// (PsEmitter in include/psmgr.h). Mostly not yet decompiled.

#include "game_types.h"
#include "psmgr.h"

// Skin.c
void fn_800360A0(void* pMesh);
void fn_800360D4(u8* pMesh);

void fn_80098BDC(PsEmitter* pEmitter);

// Free every emitter on the list, then the six fixed ones.
void fn_80098B5C(void) {
    PsEmitter* pEmitter;
    PsEmitter* pDone;
    int i;

    pEmitter = lbl_80281F88;
    while (pEmitter != NULL) {
        pDone = pEmitter;
        pEmitter = pEmitter->p40;
        fn_80098BDC(pDone);
    }
    lbl_80281F88 = NULL;
    for (i = 0; i < 6; i++) {
        fn_800360A0(lbl_801DB888[i]->mesh);
        fn_80009E70(lbl_801DB888[i]);
    }
}

void fn_80098BDC(PsEmitter* pEmitter) {
    pEmitter->b5C = 0;
    if (pEmitter->nB4 < 0) {
        fn_800360A0(pEmitter->mesh);
        fn_80009E70(pEmitter);
    }
}

// Free every emitter on the list.
void fn_80098C28(void) {
    PsEmitter* pEmitter;
    PsEmitter* pNext;

    pEmitter = lbl_80281F88;
    while (pEmitter != NULL) {
        pNext = pEmitter->p40;
        fn_80098BDC(pEmitter);
        pEmitter = pNext;
    }
    lbl_80281F88 = NULL;
}

void fn_80098C70(void) {
    lbl_801DB888[0]->uB8 |= 0x80000000;
    lbl_801DB888[1]->uB8 |= 0x80000000;
    lbl_801DB888[2]->uB8 |= 0x80000000;
    lbl_801DB888[3]->uB8 |= 0x80000000;
    lbl_801DB888[4]->uB8 |= 0x80000000;
    lbl_801DB888[5]->uB8 |= 0x80000000;
}

void fn_80099B74(PsEmitter* pEmitter) {
    pEmitter->n58 = 0;
    fn_800360D4(pEmitter->mesh);
}

void fn_80099EA4(PsEmitter* pEmitter) {
    pEmitter->p40 = lbl_80281F88;
    lbl_80281F88 = pEmitter;
}
