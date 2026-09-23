// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C), three
// jump tables in .data and constants in .sdata2 (0x802851A8-0x802851B7).

#include "frontend/uistudio.h"

// Activates (bOn) a screen. With no p60 record open the current screen first gets event -5;
// then, unless every screen is being unloaded, the screen becomes current, its first node is
// switched on, the game hears of it (pfnScreen24) and it gets event -4. A screen waiting to be
// unloaded is refused with a warning.
void fn_801686F8(UIStudio* pStudio, u8 bOn, u16 uGroup, u16 uScreen) {
    char szMsg[512];
    u32 nIndex;
    UISScreen* pScreen;

    if (pStudio->n5C == 0) {
        pStudio->uFlags |= 2;
        UIStudio_Send(pStudio, &pStudio->stack64, 0, -5, 0, NULL, 0);
        pStudio->uFlags &= ~2;
    }
    if (bOn && !(u8)pStudio->bUnloadingAll) {
        nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
        if (nIndex < pStudio->nScreens) {
            pScreen = &pStudio->pScreens[nIndex];
            if (pScreen->bUnloading == 0) {
                pScreen->pData->pNodes[0].pDesc->n4 = 1;
                pScreen->pData->pNodes[0].pDesc->u0 = 1;
                pStudio->nCurScreen = nIndex;
                if (pStudio->pfnScreen24 != NULL) {
                    pStudio->pfnScreen24(pScreen->uGroup, pScreen->uScreen);
                }
                pStudio->uFlags |= 2;
                UIStudio_Send(pStudio, &pStudio->stack64, 0, -4, 0, NULL, 0);
                pStudio->uFlags &= ~2;
            } else {
                sprintf(szMsg,
                        "Attempting to activate screen (Group ID: %d, Screen ID: %d) which is waiting to be "
                        "unloaded.\n",
                        uGroup, uScreen);
                lbl_80282A28(0, "UIStudio.c", 2942, szMsg);
            }
        }
    }
}

// Runs the queued events, then sends event uEvent to every screen.
void fn_80168B80(UIStudio* pStudio, u32 uEvent) {
    s32 i;
    s32 n;
    UISScreen* pScreen;
    u8 bOut;

    fn_80165528(pStudio, 0);
    n = pStudio->nScreens;
    for (i = 0; i < n; i++) {
        pScreen = &pStudio->pScreens[i];
        bOut = 0;
        pStudio->uFlags |= 2;
        fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, uEvent, -10, 0, NULL, &bOut);
        pStudio->uFlags &= ~2;
    }
}
