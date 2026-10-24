// GoViewport.c: viewports, the part of the frame buffer a render camera draws into (VM_SViewport):
// making, freeing and resetting one, and working out its derived values.

#include "camera.h"

static void VM_vSetDefaultViewport(VM_SViewport* pViewport);

void VM_vUpdateInternalViewportRectData(VM_SViewport* pViewport) {
    pViewport->fWidthTimesRatio = pViewport->fWidth * pViewport->fWidthRatio;
    pViewport->fHeightTimesRatio = pViewport->fHeight * pViewport->fHeightRatio;
    pViewport->fOneOverWidthRatio = 1.0f / pViewport->fWidthRatio;
    pViewport->fOneOverHeightRatio = 1.0f / pViewport->fHeightRatio;
    pViewport->fWidthOverRatio = pViewport->fWidth * pViewport->fOneOverWidthRatio;
    pViewport->fHeightOverRatio = pViewport->fHeight * pViewport->fOneOverHeightRatio;
    pViewport->fHeightOverWidth = pViewport->fHeight / pViewport->fWidth;
}

void VM_vInitModule(void) {
}

void VM_vCloseModule(void) {
}

VM_SViewport* VM_spCreateViewport(void) {
    VM_SViewport* pViewport = Circular_MemAllocFunc(sizeof(VM_SViewport), MEMTYPE_LOW, 16,
                                                    "GoViewport.c", 137);

    VM_vSetDefaultViewport(pViewport);
    return pViewport;
}

void VM_vReleaseViewport(VM_SViewport* pViewport) {
    Circular_MemFreeFunc(pViewport);
}

// All of the frame buffer, on a 4:3 screen.
static void VM_vSetDefaultViewport(VM_SViewport* pViewport) {
    pViewport->fLeft = 0.0f;
    pViewport->fTop = 0.0f;
    pViewport->fWidth = 1.0f;
    pViewport->fHeight = 1.0f;
    pViewport->fWidthRatio = 1.0f;
    pViewport->fHeightRatio = 0.75f;
    VM_vUpdateInternalViewportRectData(pViewport);
}
