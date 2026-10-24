// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void Session_Init();
void UStream_AllocBuffers();
void fn_80005580();
void fn_800071BC();
void StreamManager_InitOnce();
void fn_8002F1D4();
void Gaud_InitOnce();
void AudMem_InitOnce();
void GoARAM_Init();

void fn_80005520(void);
void fn_80005520(void) {
    fn_80005580();
    Circular_PreInit();
    fn_800071BC();
    fn_80095108();
    AudMem_InitOnce();
    GoARAM_Init();
    Math_InitLog2Table();
    fn_8000B984();
    Circular_CreateBuffer();
    File_InitOnce();
    Input_vInitOnce();
    UStream_AllocBuffers();
    Session_Init();
    fn_8002F1D4();
    Gaud_InitOnce();
    StreamManager_InitOnce();
    Input_iInitModule();
}
