// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void UStream_FreeBuffers();
void fn_800055D4();
void fn_800060DC();
void fn_8000724C();
void fn_8000977C();
void fn_8000AF58();
void fn_80013030();
void fn_80014590();
void fn_800B5C34();
void fn_800B64D8();

void fn_80005590(void);
void fn_80005590(void) {
    fn_80014590();
    fn_8000724C();
    UStream_FreeBuffers();
    fn_800060DC();
    fn_80013030();
    fn_8000977C();
    fn_8000AF58();
    fn_800B64D8();
    fn_800B5C34();
    fn_800055D4();
}
