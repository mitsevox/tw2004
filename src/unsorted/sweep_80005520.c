// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"
#include "core/goaram.h"
#include "ustream.h"

void Session_Init();
void UStream_AllocBuffers();
void fn_80005580();
void fn_800071BC();
void fn_8002F1D4();
void Gaud_InitOnce();
void AudMem_InitOnce();

void fn_80005520(void);
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
    UStream_AllocBuffers();
    Session_Init();
    fn_8002F1D4();
    Gaud_InitOnce();
    StreamManager_InitOnce();
    Input_iInitModule();
}
