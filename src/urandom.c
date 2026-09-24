// urandom.c (TW06's legacy/lib/urandom.c): the game's random numbers. Three independent streams,
// each an additive lagged-Fibonacci generator over a table of 20 words, seeded from the
// Park-Miller "minimal standard" generator; on top of them uniform floats and normally distributed
// floats.

#include "engine.h"
#include "golfer.h"                     // PI

// Written in reverse address order (CodeWarrior lays out .bss last-defined-first).
u32 lbl_801A24C4[3][20];        // each stream's table
s32 lbl_801A24B8[3];            // each stream's position in its table, counting down
u32 lbl_80281BE8;               // the Park-Miller seed the tables are filled from
f32 lbl_80281BE4;               // the second normal value of the last Box-Muller pair
u8  lbl_80281BE0;               // lbl_80281BE4 holds a value not yet handed out

u32 Misc_PMRand(void);
u32 Misc_CreateRandomSeed_Internal(void);
u32 fn_8000B408(void);

// The next number of a stream: the table entry 3 places on is added into the current one.
u32 Misc_RandFunc(int nStream) {
    int nLag;
    int nIndex;

    nIndex = lbl_801A24B8[nStream] - 1;
    nLag = nIndex + 3;
    if (nIndex < 0) {
        nIndex = 19;
    } else if (nLag > 19) {
        nLag -= 20;
    }
    lbl_801A24B8[nStream] = nIndex;
    lbl_801A24C4[nStream][nIndex] += lbl_801A24C4[nStream][nLag];
    return lbl_801A24C4[nStream][nIndex];
}

// Park-Miller: seed = seed * 16807 mod (2^31 - 1), in 32-bit arithmetic (Carta's method).
u32 Misc_PMRand(void) {
    u32 uLo;
    u32 uHi;

    uHi = (lbl_80281BE8 >> 16) * 16807;
    uLo = (lbl_80281BE8 & 0xFFFF) * 16807;
    lbl_80281BE8 = ((uHi & 0x7FFF) << 16) + uLo + (uHi >> 15);
    if ((s32)lbl_80281BE8 < 0) {
        lbl_80281BE8 -= 0x7FFFFFFF;
    }
    return lbl_80281BE8;
}

// Seeds a stream: its table is filled from the Park-Miller generator started at uSeed.
void Misc_SetSeedFunc(int nStream, u32 uSeed) {
    int i;

    lbl_80281BE8 = uSeed;
    lbl_801A24B8[nStream] = 19;
    for (i = 0; i < 20; i++) {
        lbl_801A24C4[nStream][i] = Misc_PMRand();
    }
}

// A seed from the clock (the front end seeds a new session with it).
u32 Misc_CreateRandomSeed(void) {
    return Misc_CreateRandomSeed_Internal();
}

// The seconds, minutes and hours of the date, plus 10, mixed with the CPU's tick counter.
u32 Misc_CreateRandomSeed_Internal(void) {
    OSCalendarTime time;
    u32 uSeed;

    OSTicksToCalendarTime(OSGetTime(), &time);
    uSeed = 10 + time.nSec + time.nMin + time.nHour;
    uSeed ^= fn_8000B3E8();
    return uSeed;
}

// Seeds all three streams with one seed.
void Misc_InitModule(u32 uSeed) {
    int i;

    i = 0;
    do {
        Misc_SetSeedFunc(i, uSeed);
        i++;
    } while (i < 3);
    lbl_80281BE0 = 0;
}

void Misc_CloseModule(void) {
    lbl_80281BE0 = 0;
}

// Box-Muller: two uniform numbers make two independent normal ones; the second is kept for the
// next call.
f32 Misc_RandFuncg(int nStream) {
    f32 fU;
    f32 fAngle;
    f32 fSin;
    f32 fCos;
    f32 fRadius;

    if (lbl_80281BE0) {
        lbl_80281BE0 = 0;
        return lbl_80281BE4;
    }
    fU = Misc_RandFuncf(nStream);
    fAngle = 2.0f * PI * Misc_RandFuncf(nStream);
    fSin = fn_800095F0(fAngle);
    fCos = fn_80009638(fAngle);
    fRadius = fn_80009680(-2.0f * logf(fU));
    lbl_80281BE0 = 1;
    lbl_80281BE4 = fRadius * fSin;
    return fRadius * fCos;
}

u32 fn_8000B3E8(void) {
    return fn_8000B408();
}

u32 fn_8000B408(void) {
    return OSGetTick();
}

// A float in [0, 1): 23 random bits as the mantissa of a number in [1, 2), less 1.
f32 Misc_RandFuncf(int nStream) {
    f32 f;

    f = 0.0f;
    // port: writes the float's bits through a u32 pointer (a union does not keep the store of
    // 0.0f above, which the original has); build with -fno-strict-aliasing or use memcpy.
    *(u32*)&f = (Misc_RandFunc(nStream) & 0x7FFFFF) | 0x3F800000;
    f -= 1.0f;
    return f;
}
