// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Session_Init();
void UStream_AllocBuffers();
void fn_80005580();
void fn_80005EC0();
void fn_800071BC();
void fn_800097C8();
void StaticMem_Init();
void Math_InitLog2Table();
void fn_8000B984();
void fn_80012FA0();
void Input_iInitModule();
void fn_80014524();
void fn_8002F1D4();
void fn_80095108();
void Gaud_InitOnce();
void fn_800B5C30();
void GoARAM_Init();

void fn_80005520(void);
void fn_80005520(void) {
    fn_80005580();
    fn_800097C8();
    fn_800071BC();
    fn_80095108();
    fn_800B5C30();
    GoARAM_Init();
    Math_InitLog2Table();
    fn_8000B984();
    StaticMem_Init();
    fn_80005EC0();
    fn_80012FA0();
    UStream_AllocBuffers();
    Session_Init();
    fn_8002F1D4();
    Gaud_InitOnce();
    fn_80014524();
    Input_iInitModule();
}
