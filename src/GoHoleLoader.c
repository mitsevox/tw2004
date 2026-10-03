// GoHoleLoader.c (EA's name, from TW07's golf/HoleLoader/GoHoleLoader.c): loading a hole. The game
// queues a hole load (a new hole, a playoff, a disc swap); the main loop then closes the current
// hole, streams the new hole's files in and runs every system's per-hole set-up.

#include "game.h"
#include "engine.h"
#include "golfer.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "dynobj.h"
#include "breakline.h"
#include "greengrid.h"
#include "grassshader.h"
#include "shadow.h"
#include "sitdev.h"
#include "psmgr.h"
#include "ustream.h"
#include "holeloader.h"

HoleLoaderState  gHoleLoader;
HoleLoaderState* gpHoleLoader = &gHoleLoader;

void HoleLoader_vLoadQueuedHole(void) {
    HoleLoader_OnLoadQueuedHole();
    if (gpHoleLoader->bHoleQueued != 0) {
        HoleLoader_PreHoleInit();
        fn_800106A0(1);
        fn_8000B4B0(1);
        StreamManagerHole_RegisterStreamClients();
        StreamManagerHole_StreamFiles();
        StreamManagerHole_UnregisterStreamClients();
        fn_800106A0(0);
        fn_8000B4B0(0);
        gpHoleLoader->bHoleLoaded = 1;
        HoleLoader_PostHoleInit();
        gpHoleLoader->bHoleQueued = 0;
    }
}

void HoleLoader_QueueNextHole(void) {
    HoleLoader_OnQueueNextHole();
    gpHoleLoader->bHoleQueued = 1;
}

void HoleLoader_ResetQueueNextHole(void) {
    gpHoleLoader->bHoleQueued = 0;
}

void HoleLoader_PreHoleInit(void) {
    HoleLoader_OnPreHoleInit();
    SitDev_vInitBeforeHole();
    fn_8006A89C();
}

void HoleLoader_PostHoleInit(void) {
    GR_vInitForHole();
    BreakLine_InitForHole();
    fn_8002BC6C();
    Character_PreHoleInit();
    DynObj_InitForHole();
    fn_8006F650();
    GM_InitForHole();
    fn_800A2E68();
    UI_InitForHole();
    fn_80037E50();
    fn_800B26DC();
    GolfCamera_ResetSpecialCameraStates();
    HoleLoader_OnPostHoleInit();
}

// Closes the hole: stops what runs on it, and when one is loaded, frees its terrain, objects and
// cameras and stops every weather effect.
void HoleLoader_CloseCurrentHole(void) {
    s32 i;

    fn_8006FBF8();
    BreakLine_CloseAfterHole();
    SW_vDeInitForHole();
    AnimStream_WaitForRead();
    if (gpHoleLoader->bHoleLoaded != 0) {
        Grass_DeInitForHole();
        Ter_UnloadHole();
        DynObj_DeInitForHole();
        StaticCam_Reset();
        fn_80098C28();
        for (i = 0; i < PS_NUM_KINDS; i++) {
            fn_800A2B34(i);
        }
        Kernel_RemoveAllObjects();
        fn_80010608(1);
        fn_8000B68C(1);
        gpHoleLoader->bHoleLoaded = 0;
    }
}

void HoleLoader_OnLoadQueuedHole(void) {
}

void HoleLoader_OnQueueNextHole(void) {
}

void HoleLoader_OnPreHoleInit(void) {
}

void HoleLoader_OnPostHoleInit(void) {
}
