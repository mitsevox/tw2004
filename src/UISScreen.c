// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's nodes to draw them with a scale and offset, finds and runs the handlers
// nodes have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus.

#include "frontend/uistudio.h"

void fn_8016B0F8(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs);
u16 fn_8016B188(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent,
                s32 nArgs, const s32* pArgs);

// Send an event to every screen. While the studio is already sending one (flag 2 or 4), it is
// queued on the event stack instead.
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs) {
    UISEventData data;
    if ((pStudio->uFlags & 2) || (pStudio->uFlags & 4)) {
        data.au[0] = uEvent;
        fn_80165B90(-1, -1, pStudio, 9, &data, nArgs, pArgs);
    } else {
        fn_8016B0F8(pStudio, uEvent, nArgs, pArgs);
    }
}

void fn_8016B0F8(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs) {
    u32 i;
    u32 nScreens = pStudio->nScreens;
    for (i = 0; i < nScreens; i++) {
        pStudio->uFlags |= 2;
        fn_8016B188(pStudio, &pStudio->pScreens[i], &pStudio->stack64, 0, uEvent, nArgs, pArgs);
        pStudio->uFlags &= ~2;
    }
}

// The offset and scale every node is drawn with. fn_8016A510 multiplies a node's four values at
// 0x3C by the scale and adds its four at 0x4C to the offset for the node's children.
void fn_8016C15C(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280628[0] = f1;
    lbl_80280628[3] = f4;
    lbl_80280628[1] = f2;
    lbl_80280628[2] = f3;
}

void fn_8016C174(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280638[0] = f1;
    lbl_80280638[3] = f4;
    lbl_80280638[1] = f2;
    lbl_80280638[2] = f3;
}

f32* fn_8016C18C(void) {
    return lbl_80280628;
}

f32* fn_8016C198(void) {
    return lbl_80280638;
}

// A node's handler of the kind marked 0x4000 for an event.
u32 fn_8016C5C4(UISNode* pNode, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x4000) && pHandler->uEvent == uEvent) {
            return pHandler->u4;
        }
    }
    return 0;
}

// A node's plain handler (neither kind bit) with the given ID for an event.
u32 fn_8016C614(UISNode* pNode, u16 uId, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if (!(pHandler->uFlags & 0xC000) && pHandler->uEvent == uEvent &&
            (pHandler->uFlags & 0x2FFF) == uId) {
            return pHandler->u4;
        }
    }
    return 0;
}

// A node's handler of the kind marked 0x8000 for an event.
u32 fn_8016C674(UISNode* pNode, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x8000) && pHandler->uEvent == uEvent) {
            return pHandler->u4;
        }
    }
    return 0;
}

// The index of a loaded screen, or the number of screens when it is not loaded.
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    u16 i;
    for (i = 0; i < pStudio->nScreens; i++) {
        if (pStudio->pScreens[i].uGroup == uGroup && pStudio->pScreens[i].uScreen == uScreen) break;
    }
    return i;
}
