// maddeca.c (EA's name: TW06's PDB puts madvlcdecode in Golf\rcmp\maddeca.c; NFSMW's copy of EA's
// rcmp package has maddeca.cpp with a static discardbits of its own, our MAD_DiscardBitsVlc, then
// madvlcdecode): the MAD decoder's coefficient decoder. It reads one block's run and level codes
// from maddec.c's bit buffer through that file's code tables (madvlctbl1..3, built by madinit) and
// writes the block, dequantized (madquant), into idctinput for madidct.c's inverse DCT.
// Its own data is the scan order (.rodata 0x80184B68..0x80184C68); the rest is maddec.c's, and
// idctinput is madidct.c's.

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// the scan order of a block's coefficients (the zigzag, transposed): scan position -> index
const s32 gMadScanOrder[64] = {
    0,  8,  1,  2,  9,  16, 24, 17, 10, 3,  4,  11, 18, 25, 32, 40,
    33, 26, 19, 12, 5,  6,  13, 20, 27, 34, 41, 48, 56, 49, 42, 35,
    28, 21, 14, 7,  15, 22, 29, 36, 43, 50, 57, 58, 51, 44, 37, 30,
    23, 31, 38, 45, 52, 59, 60, 53, 46, 39, 47, 54, 61, 62, 55, 63,
};

u32 MAD_ReadLittleEndian(u8* pData, int nBytes);

// The same as discardbits: madvlcdecode's own copy. NFSMW's maddeca.cpp (EA's rcmp package) keeps a
// static discardbits beside madvlcdecode, so this is this file's own discardbits (EA's name; ours
// keeps it apart from maddec.c's).
void MAD_DiscardBitsVlc(int nBits) {
    madshiftreg <<= nBits;
    madbitcount -= nBits;
    if (madbitcount < 16) {
        madshiftreg |= MAD_ReadLittleEndian(maddataptr, 2) << (16 - madbitcount);
        madbitcount += 16;
        maddataptr += 2;
    }
}

// Decode one block's coefficients into idctinput, dequantized, in natural order. The result
// is one past the last coefficient's scan position: 1 when there is only the DC one.
int madvlcdecode(void) {
    u32 uCode;
    int nLen;
    int n;
    int i;
    int nDC;
    s32* p;

    nDC = (s32)madshiftreg >> 24;
    idctinput[0] = nDC * madquant[0];
    MAD_DiscardBitsVlc(8);
    // fake match: the 63 words cleared three per pass; a loop of single stores unrolls 9-way, not
    // EA's 21 stores x 3
    p = &idctinput[1];
    for (i = 0; i < 21; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p += 3;
    }
    n = 1;
    while (1) {
        uCode = madvlctbl1[madshiftreg >> 23];
        nLen = uCode & 0xFF;
        if (nLen > 9) {
            if (!(nLen & 0x20)) {
                if (!(nLen & 0x10)) {
                    MAD_DiscardBitsVlc(9);
                    uCode = madvlctbl2[madshiftreg >> 24];
                    nLen = uCode & 0xFF;
                } else {
                    MAD_DiscardBitsVlc(6);
                    uCode = madvlctbl3[madshiftreg >> 24];
                    nLen = uCode & 0xFF;
                }
            } else if (!(nLen & 0x10)) {
                // escape: the run and level follow as they are
                MAD_DiscardBitsVlc(6);
                uCode = madshiftreg;
                nLen = 16;
            } else {
                // end of block
                MAD_DiscardBitsVlc(2);
                return n;
            }
        }
        MAD_DiscardBitsVlc(nLen);
        n += (uCode >> 16) & 0x3F;
        i = gMadScanOrder[n++];
        idctinput[i] = ((s32)uCode >> 22) * madquant[i];
    }
}
