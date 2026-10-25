// Main.c: the program's entry point. main brings up the engine's systems, runs GoEntry.c's outer
// loop (MA_vMain) and shuts the systems down again.

#include "engine.h"
#include "game.h"
#include "core/goaram.h"
#include "core/audcontainers.h"
#include "ustream.h"

static void MA_vOnceInit(void);
static void MA_ClearInterruptsOffDepth(void);
static void MA_vOnceClose(void);
static void MA_CloseEmpty(void);

// Brings up the systems at start-up, in this order.
static void MA_vOnceInit(void) {
    MA_ClearInterruptsOffDepth();
    Circular_PreInit();
    Displ_OpenOnce();
    Mem_InitOnce();
    AudMem_InitOnce();
    GoARAM_InitOnce();
    MF_vInitOnce();
    FEtm_AllocBuffers();
    Circular_CreateBuffer();
    File_InitOnce();
    Input_vInitOnce();
    Stream_InitOnce();
    Session_Init();
    RenderSurface_InitOnce();
    Gaud_InitOnce();
    StreamManager_InitOnce();
    Input_iInitModule();
}

// No level of Displ_DisableInterrupts is open: its next call turns interrupts off.
static void MA_ClearInterruptsOffDepth(void) {
    *gpInterruptsOffDepth = 0;
}

// Shuts the systems down when the outer loop returns.
static void MA_vOnceClose(void) {
    StreamManager_CloseOnce();
    Displ_CloseOnce();
    Stream_CloseOnce();
    File_CloseOnce();
    Input_vCloseOnce();
    Circular_DestroyBuffer();
    MF_vCloseOnce();
    GoARAM_CloseOnce();
    AudMem_CloseOnce();
    MA_CloseEmpty();
}

static void MA_CloseEmpty(void) {
}

int main(s32 iArgc_p, char** chppArgv_p) {
    Circular_InitOnce();
    MA_vOnceInit();
    MA_vMain(iArgc_p, chppArgv_p);
    MA_vOnceClose();
    return 0;
}
