// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Session_Init();
void UStream_AllocBuffers();
void fn_80005580();
void fn_80005EC0();
void fn_800071BC();
void StaticMem_PreInit();
void StaticMem_Init();
void Math_InitLog2Table();
void fn_8000B984();
void DS_vInitOnce();
void Input_iInitModule();
void StreamManager_InitOnce();
void fn_8002F1D4();
void fn_80095108();
void Gaud_InitOnce();
void AudMem_InitOnce();
void GoARAM_Init();

void fn_80005520(void);
void fn_80005520(void) {
    fn_80005580();
    StaticMem_PreInit();
    fn_800071BC();
    fn_80095108();
    AudMem_InitOnce();
    GoARAM_Init();
    Math_InitLog2Table();
    fn_8000B984();
    StaticMem_Init();
    fn_80005EC0();
    DS_vInitOnce();
    UStream_AllocBuffers();
    Session_Init();
    fn_8002F1D4();
    Gaud_InitOnce();
    StreamManager_InitOnce();
    Input_iInitModule();
}
