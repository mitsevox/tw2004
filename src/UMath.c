// UMath.c: the math module's start-up and shut-down, which make and free a table of base-2
// logarithms of a float's mantissa (nothing reads the table), and the float log.

#include "engine.h"

#define MF_LOG2_TABLE_SIZE 0x400   // one entry per step of the mantissa's top ten bits

static f32* gLog2Table;   // log2(1 + i / 1024) for i = 0..1023

// Fills gLog2Table: entry i is log2(1 + i / 1024), the natural log of the float 1 + i / 1024 (its
// mantissa's top ten bits i, the next bits 0) times 1 / ln 2.
static void MF_vFillLog2Table(void) {
    u32 uMantissa;
    f32* pEntry;
    u32 i;
    f32 f;

    pEntry = gLog2Table;
    i = 0;
    uMantissa = 0;
    do {
        *(u32*)&f = uMantissa | 0x3F800000;
        *pEntry = 1.442695f * logf(f);
        i++;
        uMantissa += (1 << 23) / MF_LOG2_TABLE_SIZE;   // the next value of the top ten bits
        pEntry++;
    } while (i < MF_LOG2_TABLE_SIZE);
}

// Empty: MF_vInitOnce calls it first, before it makes the table.
static void MF_vPreInit(void) {
}

// At start-up: takes the log2 table, 1024 floats, from the system heap and fills it.
void MF_vInitOnce(void) {
    MF_vPreInit();
    gLog2Table = Mem_AllocFunc(MF_LOG2_TABLE_SIZE * sizeof(f32), 16, 1);
    MF_vFillLog2Table();
}

// At shut-down: frees the log2 table.
void MF_vCloseOnce(void) {
    Mem_FreeFunc(gLog2Table);
}

// The float log: the natural logarithm of x, through the double log().
f32 logf(f32 x) {
    return log(x);
}
