// madidct.c (EA's name: TW06's PDB puts IdctColumn, IdctRow and idctcompute in Golf\rcmp\madidct.c;
// NFSMW's copy of EA's rcmp package has them, in this order, in madidct.cpp): the MAD decoder's
// inverse DCT, 16.16 fixed point: a pass over the columns of idctinput (the block madvlcdecode,
// maddeca.c, decoded), then one over the rows into the macroblock (maddec.c).
// Its data (.bss 0x801F8258..0x801F8458) is idctinput and the column pass's output.

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// .bss: defined in reverse address order (CodeWarrior lays it out last-defined-first)
s32 idctinput[64];              // a block's coefficients, dequantized (madvlcdecode)
s32 gMadIdctColumns[64];        // the inverse DCT's first pass (IdctColumn), read back as rows

s32 MAD_FixedMul(s32 a, s32 b);

// The inverse DCT's first pass: eight coefficients in, a column of dest (8 apart) out.
void IdctColumn(s32* src, s32* dest) {
    s32 t10;
    s32 z11;
    s32 z13;
    s32 z5;
    s32 t11;
    s32 o2;
    s32 z10;
    s32 e0;
    s32 e1;
    s32 z12;
    s32 e3;
    s32 e2;
    s32 t;
    s32 s;

    if ((src[1] | src[2] | src[3] | src[4] | src[5] | src[6] | src[7]) == 0) {
        // only the DC coefficient: the column is flat
        dest[0] = src[0];
        dest[8] = src[0];
        dest[16] = src[0];
        dest[24] = src[0];
        dest[32] = src[0];
        dest[40] = src[0];
        dest[48] = src[0];
        dest[56] = src[0];
        return;
    }
    z10 = src[5] - src[3];
    z12 = src[1] - src[7];
    z11 = src[1] + src[7];
    z13 = src[5] + src[3];
    t11 = z11 - z13;
    // register note: z13 and z5 are reused for the later sums (z13: the odd sum, then o0; z5: t12,
    // then o1), which gives EA's register allocation
    z13 = z13 + z11;
    z5 = MAD_FixedMul(z10 + z12, 0x61F8);
    t10 = z5 + MAD_FixedMul(z10, 0x8A8C);
    t11 = MAD_FixedMul(t11, 0xB505);
    z5 = MAD_FixedMul(z12, 0x14E7B) - z5;
    z13 = z13 + z5;
    z5 = z5 + t11;
    o2 = t11 + t10;
    e0 = src[0] + src[4];
    e1 = src[0] - src[4];
    t = MAD_FixedMul(src[2] - src[6], 0xB505);
    e2 = e1 - t;
    e1 = e1 + t;
    s = src[2] + src[6] + t;
    e3 = e0 - s;
    e0 = e0 + s;
    dest[0] = e0 + z13;
    dest[8] = e1 + z5;
    dest[16] = e2 + o2;
    dest[24] = e3 + t10;
    dest[32] = e3 - t10;
    dest[40] = e2 - o2;
    dest[48] = e1 - z5;
    dest[56] = e0 - z13;
}

// The second pass: a row of the first pass's output into eight 16.16 values.
void IdctRow(s32* src, s32* dest) {
    s32 t10;
    s32 z11;
    s32 z13;
    s32 z5;
    s32 t11;
    s32 o2;
    s32 z10;
    s32 e0;
    s32 e1;
    s32 z12;
    s32 e3;
    s32 e2;
    s32 t;
    s32 s;

    z10 = src[5] - src[3];
    z11 = src[1] + src[7];
    z12 = src[1] - src[7];
    z13 = src[5] + src[3];
    t11 = z11 - z13;
    // register note: z13 and z5 reused as in IdctColumn
    z13 = z13 + z11;
    z5 = MAD_FixedMul(z10 + z12, 0x61F8);
    t10 = z5 + MAD_FixedMul(z10, 0x8A8C);
    t11 = MAD_FixedMul(t11, 0xB505);
    z5 = MAD_FixedMul(z12, 0x14E7B) - z5;
    z13 = z13 + z5;
    z5 = z5 + t11;
    o2 = t11 + t10;
    e0 = src[0] + src[4];
    e1 = src[0] - src[4];
    t = MAD_FixedMul(src[2] - src[6], 0xB505);
    e2 = e1 - t;
    e1 = e1 + t;
    s = src[2] + src[6] + t;
    e3 = e0 - s;
    e0 = e0 + s;
    dest[0] = e0 + z13;
    dest[1] = e1 + z5;
    dest[2] = e2 + o2;
    dest[3] = e3 + t10;
    dest[4] = e3 - t10;
    dest[5] = e2 - o2;
    dest[6] = e1 - z5;
    dest[7] = e0 - z13;
}

// The inverse DCT of idctinput into an 8x8 block (rows stride words apart).
void idctcompute(s32* dest, int stride) {
    IdctColumn(&idctinput[0], &gMadIdctColumns[0]);
    IdctColumn(&idctinput[8], &gMadIdctColumns[1]);
    IdctColumn(&idctinput[16], &gMadIdctColumns[2]);
    IdctColumn(&idctinput[24], &gMadIdctColumns[3]);
    IdctColumn(&idctinput[32], &gMadIdctColumns[4]);
    IdctColumn(&idctinput[40], &gMadIdctColumns[5]);
    IdctColumn(&idctinput[48], &gMadIdctColumns[6]);
    IdctColumn(&idctinput[56], &gMadIdctColumns[7]);
    IdctRow(&gMadIdctColumns[0], dest);
    IdctRow(&gMadIdctColumns[8], dest + stride);
    IdctRow(&gMadIdctColumns[16], dest + stride * 2);
    IdctRow(&gMadIdctColumns[24], dest + stride * 3);
    IdctRow(&gMadIdctColumns[32], dest + stride * 4);
    IdctRow(&gMadIdctColumns[40], dest + stride * 5);
    IdctRow(&gMadIdctColumns[48], dest + stride * 6);
    IdctRow(&gMadIdctColumns[56], dest + stride * 7);
}
