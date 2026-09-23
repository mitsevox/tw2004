// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Session_Init();
void UStream_AllocBuffers();
void fn_80005580();
void fn_80005EC0();
void fn_800071BC();
void fn_800097C8();
void fn_800097CC();
void fn_8000AF20();
void fn_8000B984();
void fn_80012FA0();
void fn_80012FA4();
void fn_80014524();
void fn_8002F1D4();
void fn_80095108();
void fn_800A4BAC();
void fn_800B5C30();
void fn_800B62DC();

void fn_80005520(void);
void fn_80005520(void) {
    fn_80005580();
    fn_800097C8();
    fn_800071BC();
    fn_80095108();
    fn_800B5C30();
    fn_800B62DC();
    fn_8000AF20();
    fn_8000B984();
    fn_800097CC();
    fn_80005EC0();
    fn_80012FA0();
    UStream_AllocBuffers();
    Session_Init();
    fn_8002F1D4();
    fn_800A4BAC();
    fn_80014524();
    fn_80012FA4();
}
