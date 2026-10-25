// GlowMgr.c: the glows' module hooks for gomainloop.c: set-up and shut-down, and each view's
// update and drawing, of the glow list (GoShaderObject_Glows_Gc.c), the sun flare and the fading
// glow nodes.

#include "glows.h"
#include "camera.h"

static void ColGlow_SetCurrentList(int theListIndex);

// Sets up the glow list (glows queue on list 0), the fading nodes and the sun flare for
// iNumberOfViewports views.
void GLW_vInitModule(int iNumberOfViewports) {
    ColGlow_InitModule();
    ColGlow_SetCurrentList(0);
    FadeGlow_InitModule();
    SF_vInitModule(iNumberOfViewports);
}

void GLW_vCloseModule(void) {
    SF_vCloseModule();
    FadeGlow_CloseModule();
    ColGlow_CloseModule();
}

// View iViewportID's update: empties the glow list, updates the view's sun flare and advances the
// fading nodes by a 60th of a second.
void GLW_vUpdateGlows(int iViewportID) {
    ColGlow_ResetCurrentList();
    SF_vUpdateSunFlare(iViewportID);
    FadeGlow_UpdateAll(1.0f / 60.0f);
}

// Queues the fading nodes' glows, draws the glow list, then gives the current render camera the
// identity model matrix. iViewportID is not read.
void GLW_vRenderGlows(int iViewportID) {
    FadeGlow_RenderAll();
    RC_spGetCurrentRenderCtx();
    ColGlow_RenderAllGlowInCurrentList();
    RC_vSetRenderCtxTransformationMatrix(RC_spGetCurrentRenderCtx(), NULL);
}

// Makes list theListIndex the one glows are queued on and drawn from.
static void ColGlow_SetCurrentList(int theListIndex) {
    gpCurrentGlowList = &gaGlowLists[theListIndex];
}

void ColGlow_ResetCurrentList(void) {
    gpCurrentGlowList->nCount = 0;
}
