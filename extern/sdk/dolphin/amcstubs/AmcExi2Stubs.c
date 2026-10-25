#include "dolphin/os.h"

// The AMC debugger's EXI2 interface, as the retail SDK's stub library has it: nothing is attached,
// so every call does nothing and AMC_IsStub says so.

typedef enum {
    AMC_EXI_NO_ERROR = 0,
    AMC_EXI_UNSELECTED
} AmcExiError;

void EXI2_Init(vu8** inputPendingPtrRef, __OSInterruptHandler monitorCallback) {
}

void EXI2_EnableInterrupts(void) {
}

int EXI2_Poll(void) {
    return 0;
}

AmcExiError EXI2_ReadN(void* bytes, u32 length) {
    return AMC_EXI_NO_ERROR;
}

AmcExiError EXI2_WriteN(const void* bytes, u32 length) {
    return AMC_EXI_NO_ERROR;
}

void EXI2_Reserve(void) {
}

void EXI2_Unreserve(void) {
}

BOOL AMC_IsStub(void) {
    return TRUE;
}
