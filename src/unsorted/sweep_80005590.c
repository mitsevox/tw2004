// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void UStream_FreeBuffers();
void fn_800055D4();
void fn_800060DC();
void fn_8000724C();
void StaticMem_Shutdown();
void Math_FreeLog2Table();
void Input_vCloseOnce();
void fn_80014590();
void AudMem_CloseOnce();
void GoARAM_Shutdown();

void fn_80005590(void);
void fn_80005590(void) {
    fn_80014590();
    fn_8000724C();
    UStream_FreeBuffers();
    fn_800060DC();
    Input_vCloseOnce();
    StaticMem_Shutdown();
    Math_FreeLog2Table();
    GoARAM_Shutdown();
    AudMem_CloseOnce();
    fn_800055D4();
}
