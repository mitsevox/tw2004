// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"
#include "core/goaram.h"
#include "ustream.h"

void fn_800055D4();
void fn_8000724C();
void AudMem_CloseOnce();

void fn_80005590(void);
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
