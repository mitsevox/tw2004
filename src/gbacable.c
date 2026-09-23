// gbacable.c (EA's name, from its asserts): the Game Boy Advance link cable: the four ports' link
// state (lbl_80260E18), the commands sent over the cable and the getters and setters the front end
// uses. Only partly decompiled.

#include "game_types.h"
#include "platform.h"
#include "core/gbacable.h"

s32  fn_80176200(s32 nChan, u8* pStatus);  // the GBA library: read a port's status
s32  fn_80122CFC(s32 nChan, u32* pCmd);
s32  fn_80122FD8(s32 nChan);
void fn_8012311C(s32 nChan);
void fn_8012408C(s32 v);

// The check byte of a port's key: a CRC-style sum over its two bytes, with the polynomial 0xCD.
u32 fn_801228E0(u32 uValue) {
    u32 uSum = 0;
    u32 uBit;
    int i;

    for (uBit = 0x80; uBit != 0; uBit >>= 1) {
        uSum <<= 1;
        if (uValue & uBit) {
            if (uSum & 0x100) {
                uSum ^= 0xCC;
            } else {
                uSum += 1;
            }
        } else if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    uValue >>= 8;
    for (uBit = 0x80; uBit != 0; uBit >>= 1) {
        uSum <<= 1;
        if (uValue & uBit) {
            if (uSum & 0x100) {
                uSum ^= 0xCC;
            } else {
                uSum += 1;
            }
        } else if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    for (i = 0; i < 8; i++) {
        uSum <<= 1;
        if (uSum & 0x100) {
            uSum ^= 0xCD;
        }
    }
    return uSum & 0xFF;
}

// Every port starts unlinked, with its key: 0x40 + the port, then 0xDF for an odd port and 0x8F
// for ports 2 and 3, then the check byte of those two.
void fn_801229F8(void) {
    int i;

    for (i = 0; i < GBA_NUM_CHANNELS; i++) {
        lbl_80260E18[i].n0 = 0;
        fn_8012408C(0x12);
        lbl_80260E18[i].n5C = 0x40;
        lbl_80260E18[i].n64 = 0;
        lbl_80260E18[i].n4C = 0;
        lbl_80260E18[i].n50 = 0;
        lbl_80260E18[i].uKey = (i + 0x40) << 24;
        lbl_80260E18[i].uKey |= (i % 2 ? 0xDF : 0) << 16;
        lbl_80260E18[i].uKey |= (i / 2 ? 0x8F : 0) << 8;
        lbl_80260E18[i].uKey |= fn_801228E0(((lbl_80260E18[i].uKey >> 16) & 0xFF) |
                                            (lbl_80260E18[i].uKey & 0xFF00));
    }
}

void fn_8012332C(s32 nChan) {
    if (fn_80176200(nChan, &lbl_80260E18[nChan].uStatus) == 0) {
        if (fn_80122FD8(nChan)) {
            fn_8012311C(nChan);
        } else {
            fn_8012408C(3);
        }
    }
}

// Sends the port the "context differs" command and unlinks it.
void fn_80123C2C(s32 nChan) {
    u32 uCmd = 0x50000000;

    if (fn_80122CFC(nChan, &uCmd) == 0) {
        OSReport("GbaSetport: An error occurred to command 'FROMGC_CONTEXT_DIFFER' (chan=%d).\n", nChan);
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x10);
    } else {
        lbl_80260E18[nChan].n0 = 0;
        fn_8012408C(0x10);
    }
}

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_8028255C;
extern s32 lbl_80282560;
void fn_801229F8();
void fn_80175FB8();
void fn_80123FF8(void);
s32 OSGetResetButtonState();
s32 OSResetSystem(s32, s32, s32);
s32 fn_80123CBC(s32, s32);
s32 fn_80123E34();
extern s32 lbl_80282540;
void fn_8012402C(void);
extern s32 lbl_80281980;
extern s32 lbl_80282518;
void fn_8012408C(s32 v);
s32 fn_80124094(void);
void fn_8012409C(void);
extern s32 lbl_8028251C;
s32 fn_80077ACC();
void fn_801240A8(void);
extern s32 lbl_80282520;
extern s32 lbl_80282524;
extern s32 lbl_80282528;
extern s32 lbl_8028252C;
extern s32 lbl_80282530;
extern s32 lbl_80282534;
extern s32 lbl_80282538;
extern s32 lbl_8028253C;
void fn_801241AC(s32 v);
s32 fn_801241B4(void);
void fn_801241BC(s32 v);
s32 fn_801241C4(void);
s32 fn_801241CC(void);
void fn_801241D4(s32 v);
void fn_801241DC(s32 v);
s32 fn_801241E4(void);
void fn_801241EC(s32 v);
s32 fn_801241F4(void);
void fn_801241FC(s32 v);
s32 fn_80124204(void);
void fn_8012420C(s32 v);
s32 fn_80124214(void);
void fn_8012421C(s32 v);
s32 fn_80124224(void);
void fn_8012422C(void);
extern s32 lbl_8028254C;
extern s32 lbl_80282550;
extern s32 lbl_80282554;
extern s32 lbl_80282558;
void fn_80124238(s32 arg0, s32 arg1);
s32 fn_80124280(s32 arg0);

void fn_80123FF8(void) {
    s32 t0;
    s32 t1;
    t0 = (s32)DVDGetCurrentDiskID();
    lbl_8028255C = t0;
    t1 = OSGetTick();
    lbl_80282560 = t1;
    fn_801229F8();
    fn_80175FB8();
}

void fn_8012402C(void) {
    fn_80123E34();
    fn_80123CBC(0, 0);
    if (OSGetResetButtonState() != 0) {
        lbl_80282540 = 1;
        return;
    }
    if ((s32) lbl_80282540 != 0) {
        OSResetSystem(0, 1, 0);
    }
}

void fn_8012408C(s32 v) {
    lbl_80281980 = v;
}

s32 fn_80124094(void) {
    return lbl_80281980;
}

void fn_8012409C(void) {
    lbl_80282518 = 1;
}

void fn_801240A8(void) {
    s32 t0;
    t0 = fn_80077ACC();
    *(u8*)(((u8*)t0) + 0x50) = 1;
    *(u8*)(((u8*)t0) + 0x51) = 1;
    *(u8*)(((u8*)t0) + 0x52) = 1;
    *(u8*)(((u8*)t0) + 0x53) = 1;
    *(u8*)(((u8*)t0) + 0x54) = 1;
    *(u8*)(((u8*)t0) + 0x55) = 1;
    *(u8*)(((u8*)t0) + 0x56) = 1;
    *(u8*)(((u8*)t0) + 0x57) = 1;
    *(u8*)(((u8*)t0) + 0x58) = 1;
    *(u8*)(((u8*)t0) + 0x59) = 1;
    *(u8*)(((u8*)t0) + 0x5A) = 1;
    *(u8*)(((u8*)t0) + 0x5B) = 1;
    *(u8*)(((u8*)t0) + 0x5C) = 1;
    *(u8*)(((u8*)t0) + 0x5D) = 1;
    *(u8*)(((u8*)t0) + 0x5E) = 1;
    *(u8*)(((u8*)t0) + 0x5F) = 1;
    *(u8*)(((u8*)t0) + 0x60) = 1;
    *(u8*)(((u8*)t0) + 0x61) = 1;
    *(u8*)(((u8*)t0) + 0x62) = 1;
    lbl_8028251C = 1;
}

s32 fn_8012411C(void) {
    return lbl_80260E18[lbl_80281984].n6C;
}

void fn_80124138(s32 n) {
    lbl_80260E18[lbl_80281984].n6C = n;
}

void fn_80124154(void) {
    lbl_80260E18[lbl_80281984].n6C = 0;
}

s32 fn_80124174(void) {
    return lbl_80260E18[lbl_80281984].n68;
}

s32 fn_80124190(void) {
    return lbl_80260E18[lbl_80281984].n70;
}

void fn_801241AC(s32 v) {
    lbl_80282520 = v;
}

s32 fn_801241B4(void) {
    return lbl_80282520;
}

void fn_801241BC(s32 v) {
    lbl_80282524 = v;
}

s32 fn_801241C4(void) {
    return lbl_80282524;
}

s32 fn_801241CC(void) {
    return lbl_80282528;
}

void fn_801241D4(s32 v) {
    lbl_80282528 = v;
}

void fn_801241DC(s32 v) {
    lbl_8028252C = v;
}

s32 fn_801241E4(void) {
    return lbl_8028252C;
}

void fn_801241EC(s32 v) {
    lbl_80282530 = v;
}

s32 fn_801241F4(void) {
    return lbl_80282530;
}

void fn_801241FC(s32 v) {
    lbl_80282538 = v;
}

s32 fn_80124204(void) {
    return lbl_80282538;
}

void fn_8012420C(s32 v) {
    lbl_8028253C = v;
}

s32 fn_80124214(void) {
    return lbl_8028253C;
}

void fn_8012421C(s32 v) {
    lbl_80282534 = v;
}

s32 fn_80124224(void) {
    return lbl_80282534;
}

void fn_8012422C(void) {
    lbl_80281984 = -1;
}

void fn_80124238(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        lbl_80282558 = arg1;
        return;
    case 1:
        lbl_80282554 = arg1;
        return;
    case 2:
        lbl_80282550 = arg1;
        return;
    case 3:
        lbl_8028254C = arg1;
        return;
    }
}

s32 fn_80124280(s32 arg0) {
    switch (arg0) {
    case 0:
        return lbl_80282558;
    case 1:
        return lbl_80282554;
    case 2:
        return lbl_80282550;
    case 3:
        return lbl_8028254C;
    default:
        return 0;
    }
}

// ---- end of sweep code ----
