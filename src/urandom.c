// urandom.c: the game's random numbers. Three independent streams, each an additive
// lagged-Fibonacci generator over a table of 20 words seeded from the Park-Miller "minimal standard"
// generator; on top of them uniform floats in [0, 1) and normally distributed floats.

#include "engine.h"
#include "golfer.h"                     // PI

u32 gRandomTable[RANDOM_NUM_TYPES][RANDOM_TABLE_SIZE];  // each stream's table
s32 gRandomIndex[RANDOM_NUM_TYPES];     // each stream's place in its table, counting down
u32 gPMRandSeed;                        // Misc_PMRand's state, set to the seed a table is filled from
f32 gRandomKeptNormal;                  // the second value of Misc_RandFuncg's last pair
u8  gbRandomNormalKept;                 // gRandomKeptNormal is still to be handed out

#pragma explicit_zero_data on           // fake match: keeps the all-zero vector in .data
f32 gVec4Zero[4] = {0.0f, 0.0f, 0.0f, 0.0f};
#pragma explicit_zero_data reset

static s32 Misc_PMRand(void);
static s32 Misc_CreateRandomSeed_Internal(void);
static u32 TI_uiReadCP0Count_Internal(void);

// The next number of a stream: it steps back one place in its table (from the first to the last)
// and adds the entry 3 places on into that one, which it returns.
u32 Misc_RandFunc(eRandomType type) {
    int j;
    int k;
    u32* table;

    k = gRandomIndex[type] - 1;
    j = k + 3;
    if (k < 0) {
        k = RANDOM_TABLE_SIZE - 1;
    } else if (j > RANDOM_TABLE_SIZE - 1) {
        j -= RANDOM_TABLE_SIZE;
    }
    gRandomIndex[type] = k;
    table = gRandomTable[type];
    table[k] += table[j];
    return table[k];
}

// Park-Miller: seed = seed * 16807 mod (2^31 - 1), in 32-bit arithmetic (Carta's method).
static s32 Misc_PMRand(void) {
    u32 tmp;

    tmp = (gPMRandSeed >> 16) * 16807;
    gPMRandSeed = ((tmp & 0x7FFF) << 16) + (gPMRandSeed & 0xFFFF) * 16807 + (tmp >> 15);
    if ((s32)gPMRandSeed < 0) {
        gPMRandSeed -= 0x7FFFFFFF;
    }
    return gPMRandSeed;
}

// Seeds a stream: its table is filled from the Park-Miller generator started at seed.
void Misc_SetSeedFunc(eRandomType type, s32 seed) {
    int i;

    gPMRandSeed = seed;
    gRandomIndex[type] = RANDOM_TABLE_SIZE - 1;
    for (i = 0; i < RANDOM_TABLE_SIZE; i++) {
        gRandomTable[type][i] = Misc_PMRand();
    }
}

s32 Misc_CreateRandomSeed(void) {
    return Misc_CreateRandomSeed_Internal();
}

// A seed from the clock: 10 plus the seconds, minutes and hours of the time of day, XORed with the
// hardware's tick.
static s32 Misc_CreateRandomSeed_Internal(void) {
    OSCalendarTime time;
    s32 returnValue;

    OSTicksToCalendarTime(OSGetTime(), &time);
    returnValue = 10 + time.nSec + time.nMin + time.nHour;
    returnValue ^= TI_uiReadCP0Count();
    return returnValue;
}

// Seeds every stream with the same seed.
void Misc_InitModule(s32 seed) {
    int i;

    i = 0;
    do {
        Misc_SetSeedFunc(i, seed);
        i++;
    } while (i < RANDOM_NUM_TYPES);
    gbRandomNormalKept = 0;
}

void Misc_CloseModule(void) {
    gbRandomNormalKept = 0;
}

// Box-Muller: two uniform numbers make two independent normal ones (mean 0, deviation 1); the
// second is kept for the next call, whichever stream that call asks for.
f32 Misc_RandFuncg(eRandomType type) {
    f32 x1;
    f32 x2;
    f32 s;
    f32 c;
    f32 z;

    if (gbRandomNormalKept) {
        gbRandomNormalKept = 0;
        return gRandomKeptNormal;
    }
    x1 = Misc_RandFuncf(type);
    x2 = 2.0f * PI * Misc_RandFuncf(type);
    s = sinf(x2);
    c = cosf(x2);
    z = Math_Sqrt(-2.0f * logf(x1));
    gbRandomNormalKept = 1;
    gRandomKeptNormal = z * s;
    return z * c;
}

// The hardware's tick (the clock, LLTime.c, reads it too).
u32 TI_uiReadCP0Count(void) {
    return TI_uiReadCP0Count_Internal();
}

static u32 TI_uiReadCP0Count_Internal(void) {
    return OSGetTick();
}

// A float in [0, 1): 23 random bits as the mantissa of a number in [1, 2), less 1.
f32 Misc_RandFuncf(eRandomType type) {
    u32 ri;
    f32 rf;

    rf = 0.0f;
    ri = (Misc_RandFunc(type) & 0x7FFFFF) | 0x3F800000;
    *(u32*)&rf = ri;
    rf -= 1.0f;
    return rf;
}
