// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void EXI2_Init(void);
void EXI2_EnableInterrupts(void);
s32 EXI2_Poll(void);
s32 EXI2_ReadN(void);
s32 EXI2_WriteN(void);
void EXI2_Reserve(void);
void EXI2_Unreserve(void);
s32 AMC_IsStub(void);
s32 Hu_IsStub(void);
void EXI2_Init(void) {
}

void EXI2_EnableInterrupts(void) {
}

s32 EXI2_Poll(void) {
    return 0;
}

s32 EXI2_ReadN(void) {
    return 0;
}

s32 EXI2_WriteN(void) {
    return 0;
}

void EXI2_Reserve(void) {
}

void EXI2_Unreserve(void) {
}

s32 AMC_IsStub(void) {
    return 1;
}

s32 Hu_IsStub(void) {
    return 0;
}
