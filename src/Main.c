// Main.c: the program's entry point. main brings up the engine's systems, runs GoEntry.c's outer
// loop and shuts the systems down again.

#include "engine.h"
#include "core/goaram.h"
#include "core/audcontainers.h"
#include "ustream.h"
#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void Session_Init();
void fn_80005580(void);
void fn_800071BC();
void fn_8002F1D4();
void Gaud_InitOnce();
void fn_80005520(void);
void fn_800055D4(void);
void fn_8000724C();
void fn_80005590(void);
void fn_800083A4();
s32 main(s32 p0, s32 p1);

void fn_80005520(void) {
    fn_80005580();
    Circular_PreInit();
    fn_800071BC();
    fn_80095108();
    AudMem_InitOnce();
    GoARAM_InitOnce();
    MF_vInitOnce();
    FEtm_AllocBuffers();
    Circular_CreateBuffer();
    File_InitOnce();
    Input_vInitOnce();
    Stream_InitOnce();
    Session_Init();
    fn_8002F1D4();
    Gaud_InitOnce();
    StreamManager_InitOnce();
    Input_iInitModule();
}

void fn_80005580(void) {
    *gpInterruptsOffDepth = 0;
}

void fn_80005590(void) {
    StreamManager_CloseOnce();
    fn_8000724C();
    Stream_CloseOnce();
    File_CloseOnce();
    Input_vCloseOnce();
    Circular_DestroyBuffer();
    MF_vCloseOnce();
    GoARAM_CloseOnce();
    AudMem_CloseOnce();
    fn_800055D4();
}

void fn_800055D4(void) {
}

s32 main(s32 p0, s32 p1) {
    Circular_InitOnce();
    fn_80005520();
    fn_800083A4(p0, p1);
    fn_80005590();
    return 0;
}

// ---- end of sweep code ----
