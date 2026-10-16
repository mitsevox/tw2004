// GoFrameBuf.c: frame buffers (FB_SFrameBuffer, camera.h), the screen area in pixels a render
// camera draws into: making, setting and freeing one, and working out its derived sizes. The main
// loop's, the grass's and the shadow's render cameras each draw into one.

#include "camera.h"

static void FB_vSetDefaultFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p);

static void FB_vUpdateInternalFrameBufferData(FB_SFrameBuffer* spFrameBuffer_p) {
    spFrameBuffer_p->fWidthTimesRatio = spFrameBuffer_p->fWidth * spFrameBuffer_p->fWidthRatio;
    spFrameBuffer_p->fHeightTimesRatio = spFrameBuffer_p->fHeight * spFrameBuffer_p->fHeightRatio;
    spFrameBuffer_p->fOneOverWidthRatio = 1.0f / spFrameBuffer_p->fWidthRatio;
    spFrameBuffer_p->fOneOverHeightRatio = 1.0f / spFrameBuffer_p->fHeightRatio;
    spFrameBuffer_p->fWidthOverRatio = spFrameBuffer_p->fWidth * spFrameBuffer_p->fOneOverWidthRatio;
    spFrameBuffer_p->fHeightOverRatio = spFrameBuffer_p->fHeight * spFrameBuffer_p->fOneOverHeightRatio;
    spFrameBuffer_p->fHeightOverWidth = spFrameBuffer_p->fHeight / spFrameBuffer_p->fWidth;
}

void FB_vInitModule(void) {
}

void FB_vCloseModule(void) {
}

FB_SFrameBuffer* FB_spCreateFrameBuffer(void) {
    FB_SFrameBuffer* spFrameBuffer = StaticMem_Alloc(sizeof(FB_SFrameBuffer), 2, 16, "GoFrameBuf.c", 137);

    FB_vSetDefaultFrameBuffer(spFrameBuffer);
    return spFrameBuffer;
}

void FB_vReleaseFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p) {
    StaticMem_Free(spFrameBuffer_p);
}

// The whole 512 x 448 screen.
static void FB_vSetDefaultFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p) {
    FB_vSetFrameBuffer(spFrameBuffer_p, 0.0f, 0.0f, 512.0f, 448.0f, 1.0f, 1.0f);
}

void FB_vSetFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p, f32 fOffsetX_p, f32 fOffsetY_p, f32 fWidth_p,
                        f32 fHeight_p, f32 fWidthRatio_p, f32 fHeightRatio_p) {
    spFrameBuffer_p->fOffsetX = fOffsetX_p;
    spFrameBuffer_p->fOffsetY = fOffsetY_p;
    spFrameBuffer_p->fWidth = fWidth_p;
    spFrameBuffer_p->fHeight = fHeight_p;
    spFrameBuffer_p->fWidthRatio = fWidthRatio_p;
    spFrameBuffer_p->fHeightRatio = fHeightRatio_p;
    FB_vUpdateInternalFrameBufferData(spFrameBuffer_p);
}
