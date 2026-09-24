// Code8009B340.c (our name; what it draws is not known yet): a list of nodes (FadeNode,
// lbl_80281FA0) drawn as queued glows (fn_8009B260) that fade out, each freed once it has faded. Its
// data is its own: the .sbss 0x80281FA0-0x80281FA8 and the .sdata2 0x80283EC0-0x80283EC8.
// fn_8009B320 before it (it frees a node) is probably this file's too.

#include "glows.h"

void fn_8009B320(FadeNode* pNode);

FadeNode* lbl_80281FA0;

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 255.0f (0x80283EC0), before the 0.0f fn_8009B340 uses first; its body is unknown.
static f32 Code8009B340_StrippedFn(f32 x) {
    return x * 255.0f;
}

// Advances a node by fTime seconds; one that has faded out is marked for freeing.
void fn_8009B340(FadeNode* pNode, f32 fTime) {
    pNode->f18 = pNode->f30 * fTime + pNode->f18;
    pNode->f1C = pNode->f18 + pNode->f20;
    if (pNode->f1C < 0.0f) {
        pNode->f1C = 0.0f;
    }
    pNode->fAlpha = pNode->fAlphaSpeed * fTime + pNode->fAlpha;
    if (pNode->fAlpha < 0.0f) {
        pNode->fAlpha = 0.0f;
        pNode->uFlags |= 0x80000000;
        return;
    }
    pNode->uColor = (pNode->uColor & 0xFFFFFF) | ((u32)(s32)(255.0f * pNode->fAlpha) << 24);
}

void fn_8009B3D8(FadeNode* pNode) {
    f32* pPos;

    if (pNode->p34 != NULL) {
        pPos = pNode->p34->a30;
    } else {
        pPos = pNode->a4;
    }
    if (pNode->uFlags & 1) {
        fn_8009B260(pPos, 0, pNode->uColor, 2, 0, 1, pNode->f1C, pNode->f18, 0.0f);
        return;
    }
    if (pNode->uFlags & 2) {
        fn_8009B260(pPos, 0, pNode->uColor, 2, 0, 1, pNode->f1C, pNode->f18, 0.0f);
        fn_8009B260(pPos, pNode->uColor, 0, 2, 0, 1, pNode->f18, pNode->f18 - pNode->f20, 0.0f);
    }
}

// Frees the nodes that have faded out.
void fn_8009B4B4(void) {
    FadeNode* pNode;
    FadeNode* pNext;
    FadeNode* pPrev;

    pPrev = NULL;
    pNode = lbl_80281FA0;
    while (pNode != NULL) {
        pNext = pNode->pNext;
        if (pNode->uFlags & 0x80000000) {
            if (pPrev != NULL) {
                pPrev->pNext = pNext;
            } else {
                lbl_80281FA0 = pNext;
            }
            fn_8009B320(pNode);
        } else {
            pPrev = pNode;
        }
        pNode = pNext;
    }
}

void fn_8009B528(f32 fTime) {
    FadeNode* pNode;

    for (pNode = lbl_80281FA0; pNode != NULL; pNode = pNode->pNext) {
        fn_8009B340(pNode, fTime);
    }
    fn_8009B4B4();
}

void fn_8009B57C(void) {
    FadeNode* pNode;

    for (pNode = lbl_80281FA0; pNode != NULL; pNode = pNode->pNext) {
        fn_8009B3D8(pNode);
    }
}

// Frees every node.
void fn_8009B5BC(void) {
    FadeNode* pNode;
    FadeNode* pNext;

    pNode = lbl_80281FA0;
    while (pNode != NULL) {
        pNext = pNode->pNext;
        fn_8009B320(pNode);
        pNode = pNext;
    }
    lbl_80281FA0 = NULL;
}

void fn_8009B604(void) {
    lbl_80281FA0 = NULL;
}

void fn_8009B610(void) {
    fn_8009B5BC();
}
