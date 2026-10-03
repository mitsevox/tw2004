// Code8006F438.c (our name): the hole loader. Its extent is where its .sdata starts and ends
// (lbl_802811E8 alone, padded to 8): a hole load is asked for with lbl_802811E8[1] and done here,
// the hole's files streamed in and the game's systems set up for it.

#include "game_types.h"
#include "engine.h"
#include "game.h"

void HoleLoader_vLoadQueuedHole(void);
void fn_800106A0(int n);                    // LLTexGrp.c
void StreamManagerHole_RegisterStreamClients(void); // streammanagerhole.c
void StreamManagerHole_UnregisterStreamClients(void); // streammanagerhole.c
void StreamManagerHole_StreamFiles(void);   // streammanagerhole.c
void HoleLoader_PreHoleInit(void);
void HoleLoader_PostHoleInit(void);
void HoleLoader_OnLoadQueuedHole(void);

// The hole loader's flags: [0] a hole is loaded (HoleLoader_CloseCurrentHole unloads it and clears the flag),
// [1] a hole load is asked for (HoleLoader_QueueNextHole / HoleLoader_ResetQueueNextHole). The size
// is not known (2 to 8).
u8 lbl_80281E68[2];
u8* lbl_802811E8 = lbl_80281E68;

// Load the hole if one is asked for: stream its files in (with fn_800106A0 and fn_8000B4B0 set to 1
// around it), mark it loaded (lbl_802811E8[0]) and set everything up for it.
void HoleLoader_vLoadQueuedHole(void) {
    HoleLoader_OnLoadQueuedHole();
    if (lbl_802811E8[1] != 0) {
        HoleLoader_PreHoleInit();
        fn_800106A0(1);
        fn_8000B4B0(1);
        StreamManagerHole_RegisterStreamClients();
        StreamManagerHole_StreamFiles();
        StreamManagerHole_UnregisterStreamClients();
        fn_800106A0(0);
        fn_8000B4B0(0);
        lbl_802811E8[0] = 1;
        HoleLoader_PostHoleInit();
        lbl_802811E8[1] = 0;
    }
}

// ---- sweep code (not yet cleaned up) ----

void HoleLoader_OnQueueNextHole(void);
void HoleLoader_QueueNextHole(void);
void HoleLoader_ResetQueueNextHole(void);
void SitDev_vInitBeforeHole();
void fn_8006A89C();
void HoleLoader_OnPreHoleInit(void);
void fn_8002BC6C();
void Character_PreHoleInit();
void fn_80037E50();
void DynObj_InitForHole();
void HoleLoader_OnPostHoleInit(void);
void fn_8006F650();
void UI_InitForHole();
void GR_vInitForHole();
void fn_800A2E68();
void fn_800B26DC();
void GolfCamera_ResetSpecialCameraStates();
void BreakLine_InitForHole();
void GM_InitForHole();
s32 fn_80010608(s32);
s32 Ter_UnloadHole();
s32 DynObj_DeInitForHole();
s32 Kernel_RemoveAllObjects();
void SW_vDeInitForHole(void);
s32 StaticCam_Reset();
s32 fn_8006FBF8();
s32 fn_80098C28();
s32 fn_800A2B34(s32);
s32 BreakLine_CloseAfterHole();
void AnimStream_WaitForRead(void);
s32 Grass_DeInitForHole();
void HoleLoader_CloseCurrentHole(void);

void HoleLoader_QueueNextHole(void) {
    HoleLoader_OnQueueNextHole();
    lbl_802811E8[1] = 1;
}

void HoleLoader_ResetQueueNextHole(void) {
    lbl_802811E8[1] = 0;
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

void HoleLoader_CloseCurrentHole(void) {
    s32 var_r31;

    fn_8006FBF8();
    BreakLine_CloseAfterHole();
    SW_vDeInitForHole();
    AnimStream_WaitForRead();
    if ((u8) *lbl_802811E8 != 0) {
        Grass_DeInitForHole();
        Ter_UnloadHole();
        DynObj_DeInitForHole();
        StaticCam_Reset();
        fn_80098C28();
        var_r31 = 0;
        do {
            fn_800A2B34(var_r31);
            var_r31 += 1;
        } while (var_r31 < 4);
        Kernel_RemoveAllObjects();
        fn_80010608(1);
        fn_8000B68C(1);
        *lbl_802811E8 = 0;
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

// ---- end of sweep code ----
