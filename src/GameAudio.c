// GameAudio.c (our name): the game's side of the sound engine. fn_800A3E3C starts the engine
// (memory stack, ARAM, sound table, movie sound, banks) one step after another; the rest drives
// the sound emitters (hlaudemitter.c's fn_800AD*) from the game: the course, the game mode and the
// pin set. Its extent is proven by its data: every section starts and ends on 8-byte boundaries
// shared with no other file (.rodata 0x80183AD8-0x80183B08, .data 0x8018E988-0x8018EB30,
// .bss 0x801F1708-0x801F17D0, .sdata 0x80281418-0x80281460, .sbss 0x80282010-0x80282058,
// .sdata2 0x80283F48-0x80283F88), and all its functions share those globals.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

u8 fn_800A3FF4(void);
u8 fn_800A7AF0();
u8 fn_800A8604();
s32 fn_800A86BC(s32);
u8 fn_800A8754();
u8 fn_800A8824();
u8 fn_800A8D2C();
u8 fn_800A98B4();
u8 fn_800AAD18();
u8 fn_800ABBC8();
u8 fn_800AC470();
u8 fn_800ACECC();
u8 fn_800AF224();
u8 fn_800AFAB0();
u8 fn_800B0438();
u8 fn_800B0568();
u8 fn_800B0798();
s32 fn_800B07A0();
s32 fn_800B5B80();
u8 fn_800A3E3C(s32 arg0);
void fn_800A8F68();
void fn_800A3F38(void);
void fn_800A8700();
void fn_800A8708();
void fn_800A871C();
void fn_800A3F94(void);
void fn_800A3FB4(void);
void fn_800A3FD4(void);
s32 fn_80005AE8(s32*, s32, s32);
extern u8 lbl_80282020;
extern s32 lbl_80282024;
s32 fn_800A402C(s32 p0);
void fn_800A4038(void);
void fn_800A4080(void);
s32 fn_800AD698(u8, s32, s32);
extern u8 lbl_8028141C;
extern u8 lbl_8028141D;
extern u8 lbl_80282040;
void fn_800A4374(void);
extern u8 lbl_8028141A;
extern s32 lbl_8028203C;
void fn_800A49A4(u8 arg0);
s32 fn_800A4A88();
s32 fn_800A4BAC(void);
s32 fn_800A7968(u8, s32, u8, u16, s32);
s32 fn_800AD1C8();
extern u8 lbl_80281419;
extern u8 lbl_80282038;
extern s32 lbl_8028204C;
extern u16 lbl_80282050;
extern u8 lbl_80282052;
extern u32 lbl_80282054;
void fn_800A4BDC(void);
s32 fn_800A4084();
s32 fn_800A41A4();
s32 fn_800A43DC();
void fn_800A44A0();
s32 fn_800A4928();
extern u8 lbl_80282029;
extern u8 lbl_8028202A;
extern u8 lbl_8028202C;
void fn_800A4C54(void);
extern u8 lbl_80281418;
extern u8 lbl_8028141B;
void fn_800A75B4(void);
void fn_800A4FD8(void);
extern u8 lbl_8028202D;
void fn_800A5620(void);
void fn_8006BAA8();
void fn_800A707C(void);
void fn_800A5E94(s32 p0);
s32 fn_800ADB4C(s32, s32, s32);
void fn_800A624C(void);
void fn_800A6278(void);
s32 fn_800ADC44(s32, s32, s32);
void fn_800A62A4(void);
void fn_800A62E0(void);
void fn_800A631C(void);
void fn_800A6358(void);
void fn_800A6394(void);
void fn_800A63D0(void);
void fn_800A640C(void);
void fn_800A6448(void);
void fn_800A644C(void);
void fn_800A6EC8(void);
s32 fn_800AD9AC(u8, s32, s32);
void fn_800A6F38(void);
extern void* gpGame;
void fn_800A70E4(u8 arg0);
void fn_800A714C(void);
void fn_800A7198(u8 arg0);
void fn_800A71E4(void);
void fn_800A7294(void);
void fn_800A47A0();
s32 fn_800A6660(s32);
extern u8 lbl_8028202E;
void fn_800A72EC(u8 arg0, u8 arg1);
s32 fn_800ADA28(u8, s32, u8, s32);
void fn_800A73C0(s32 p0, s32 p1);
void fn_800A73F0(s32 arg0);
u8 fn_800A7720(void);
s32 fn_800A7528(void);
u8 fn_800AD618(u8, s32);
s32 fn_800A75F4(void);
void fn_800A7644(void);
void fn_800A76E4(void);
void fn_800A7748(void);
void fn_800A7924(void);
void fn_800A7944(void);
void Mov_Init();
void Mov_Exit();
void Mov_Start();
void Mov_Tick();
void fn_800B0858();
void fn_800A7994(void);
void fn_800A79B4(void);
void fn_800A79D4(void);
void fn_800A79F4(void);
void fn_800A7A14(void);
void fn_800A8D88();
void fn_800A8DC8();
void fn_800AD0C4();
void fn_800AD1C4();
s32 fn_800A7A34(s32 p0, s32 p1, s32 p2);
void fn_800A7A98(s32 p0);
void fn_800A94F4();
void fn_800A7AD0(void);

u8 fn_800A3E3C(s32 arg0) {
    u8 var_r3;

    fn_800B5B80();
    var_r3 = fn_800AFAB0();
    if (var_r3 != 0) {
        var_r3 = fn_800B0438();
        if (var_r3 != 0) {
            var_r3 = fn_800B0568();
            if (var_r3 != 0) {
                var_r3 = fn_800AF224();
                if (var_r3 != 0) {
                    var_r3 = fn_800B0798();
                    if (var_r3 != 0) {
                        var_r3 = fn_800A8604();
                        if (var_r3 != 0) {
                            var_r3 = fn_800A8D2C();
                            if (var_r3 != 0) {
                                var_r3 = fn_800A7AF0();
                                if (var_r3 != 0) {
                                    var_r3 = fn_800A98B4();
                                    if (var_r3 != 0) {
                                        var_r3 = fn_800AAD18();
                                        if (var_r3 != 0) {
                                            var_r3 = fn_800ABBC8();
                                            if (var_r3 != 0) {
                                                var_r3 = fn_800A8754();
                                                if (var_r3 != 0) {
                                                    var_r3 = fn_800AC470();
                                                    if (var_r3 != 0) {
                                                        var_r3 = fn_800A8824();
                                                        if (var_r3 != 0) {
                                                            var_r3 = fn_800ACECC();
                                                            if (var_r3 != 0) {
                                                                var_r3 = fn_800A3FF4();
                                                                if (var_r3 != 0) {
                                                                    fn_800B07A0();
                                                                    fn_800A86BC(arg0);
                                                                    var_r3 = 1;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return var_r3;
}

void fn_800A3F38(void) {
    fn_800A8F68();
}

void fn_800A3F94(void) {
    fn_800A8700();
}

void fn_800A3FB4(void) {
    fn_800A8708();
}

void fn_800A3FD4(void) {
    fn_800A871C();
}

u8 fn_800A3FF4(void) {
    fn_80005AE8(&lbl_80282024, 0, 2);
    lbl_80282020 = 0;
    return 1;
}

s32 fn_800A402C(s32 p0) {
    lbl_80282020 = p0;
    return 1;
}

void fn_800A4038(void) {
    lbl_80282020 = 0;
}

void fn_800A4080(void) {
}

void fn_800A4374(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800AD698(lbl_8028141C, 2, 0);
        fn_800AD698(lbl_8028141D, 2, 0);
        fn_800AD698(lbl_8028141C, 3, 0);
        fn_800AD698(lbl_8028141D, 3, 0);
    }
}

void fn_800A49A4(u8 arg0) {
    if ((s32) lbl_8028203C == 2) {
        if (arg0 == 0) {
            fn_800AD698(lbl_8028141A, 0, 0);
        }
        fn_800AD698(lbl_8028141A, 1, 0);
        fn_800AD698(lbl_8028141A, 2, 0);
        fn_800AD698(lbl_8028141A, 3, 0);
        fn_800AD698(lbl_8028141A, 4, 0);
    }
}

s32 fn_800A4BAC(void) {
    s32 t0;
    t0 = fn_800A4A88();
    return ((u32)((-(t0 & 0xFF)) | (t0 & 0xFF)) >> 31);
}

void fn_800A4BDC(void) {
    u32 temp_r0;

    fn_800AD1C8();
    fn_800A4080();
    if ((u32) lbl_80282054 != 0U) {
        temp_r0 = lbl_80282054 - 1;
        lbl_80282054 = temp_r0;
        if ((temp_r0 == 0U) && ((u8) lbl_80282038 != 0)) {
            fn_800A7968(lbl_80281419, 0, lbl_80282052, lbl_80282050, lbl_8028204C);
            fn_800AD698(lbl_80281419, 0, 1);
            lbl_80282038 = 0;
        }
    }
}

void fn_800A4C54(void) {
    if ((u8) lbl_8028202A != 0) {
        lbl_8028202A = 0;
        fn_800A4084();
    }
    if ((u8) lbl_80282029 != 0) {
        if ((u8) lbl_8028202C != 0) {
            fn_800A44A0();
            lbl_8028202C = 0;
        }
        fn_800A43DC();
        fn_800A41A4();
        fn_800A4928();
    }
}

void fn_800A4FD8(void) {
    fn_800A75B4();
    lbl_8028141B = 255;
    lbl_80281418 = 255;
    lbl_80282029 = 0;
}

void fn_800A5620(void) {
    lbl_8028202D = 1;
}

void fn_800A5E94(s32 p0) {
    fn_8006BAA8((p0 & 0xFF));
    fn_800A4374();
    fn_800A707C();
}

void fn_800A624C(void) {
    fn_800ADB4C(3, 1, 1);
}

void fn_800A6278(void) {
    fn_800ADB4C(3, 1, 0);
}

void fn_800A62A4(void) {
    fn_800ADC44(3, 0, 0);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A62E0(void) {
    fn_800ADC44(3, 0, 1);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A631C(void) {
    fn_800ADC44(3, 0, 2);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6358(void) {
    fn_800ADC44(3, 0, 3);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6394(void) {
    fn_800ADC44(3, 0, 4);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A63D0(void) {
    fn_800ADC44(3, 0, 6);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A640C(void) {
    fn_800ADC44(3, 0, 7);
    fn_800ADB4C(3, 0, 1);
}

void fn_800A6448(void) {
}

void fn_800A644C(void) {
}

void fn_800A6EC8(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800A4374();
        fn_800A707C();
        fn_800AD698(lbl_8028141C, 0, 0);
        fn_800AD698(lbl_8028141C, 1, 0);
        fn_800AD698(lbl_8028141D, 0, 0);
        fn_800AD698(lbl_8028141D, 1, 0);
    }
}

void fn_800A6F38(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800AD9AC(lbl_8028141C, 0, 2);
        fn_800AD9AC(lbl_8028141C, 1, 2);
        fn_800AD9AC(lbl_8028141D, 0, 2);
        fn_800AD9AC(lbl_8028141D, 1, 2);
        fn_800AD698(lbl_8028141C, 0, 1);
        fn_800AD698(lbl_8028141C, 1, 1);
        fn_800AD698(lbl_8028141D, 0, 1);
        fn_800AD698(lbl_8028141D, 1, 1);
    }
}

void fn_800A707C(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800AD698(lbl_8028141C, 4, 0);
        fn_800AD698(lbl_8028141D, 4, 0);
        fn_800AD698(lbl_8028141C, 5, 0);
        fn_800AD698(lbl_8028141D, 5, 0);
    }
}

void fn_800A70E4(u8 arg0) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(0, 0, 1);
        fn_800ADC44(0, 1, arg0);
        fn_800ADB4C(0, 1, 1);
    }
}

void fn_800A714C(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(0, 0, 0);
        fn_800ADB4C(0, 1, 0);
    }
}

void fn_800A7198(u8 arg0) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADC44(1, 0, arg0);
        fn_800ADB4C(1, 1, 1);
    }
}

void fn_800A71E4(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(1, 1, 0);
    }
}

void fn_800A7294(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(0, 2, 0);
        if ((s32) lbl_8028203C == 2) {
            fn_800AD698(lbl_8028141A, 2, 0);
        }
    }
}

void fn_800A72EC(u8 arg0, u8 arg1) {
    if ((lbl_8028202E ^ arg0) != 0) {
        lbl_8028202E = arg0;
        if (arg0 != 0) {
            fn_800A49A4(0);
            fn_800A75B4();
            fn_800A6EC8();
            fn_800A6660(0);
            return;
        }
        if (arg1 != 0) {
            fn_800A44A0();
            fn_800A47A0();
        }
    }
}

void fn_800A73C0(s32 p0, s32 p1) {
    fn_800ADA28(lbl_8028141B, 0, (p1 & 0xFF), 0);
}

void fn_800A73F0(s32 arg0) {
    switch (arg0) {
    case 1:
        fn_800ADA28(lbl_8028141B, 1, 0U, 0);
        return;
    case 6:
        fn_800ADA28(lbl_8028141B, 1, 1U, 0);
        return;
    default:
        fn_800ADA28(lbl_8028141B, 0, (u8) arg0, 0);
        return;
    }
}

s32 fn_800A7528(void) {
    s32 t0;
    t0 = fn_800A7720();
    return (t0 & 0xFF);
}

void fn_800A75B4(void) {
    if ((s32) lbl_8028203C == 1) {
        fn_800AD698(lbl_80281418, 0, 0);
        lbl_8028203C = 0;
    }
}

s32 fn_800A75F4(void) {
    u8 var_r31;

    var_r31 = 0;
    if (((s32) lbl_8028203C == 1) && (fn_800AD618(lbl_80281418, 0) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}

void fn_800A7644(void) {
    fn_800A44A0();
}

void fn_800A76E4(void) {
    fn_800AD698(lbl_80281419, 0, 0);
    lbl_80282054 = 15;
    lbl_80282038 = 0;
}

u8 fn_800A7720(void) {
    return fn_800AD618(lbl_80281419, 0);
}

void fn_800A7748(void) {
    fn_800AD618(lbl_8028141A, 0);
}

void fn_800A7924(void) {
    fn_800A44A0();
}

void fn_800A7944(void) {
    fn_800A44A0();
    fn_800A47A0();
}

void fn_800A7994(void) {
    Mov_Init();
}

void fn_800A79B4(void) {
    Mov_Exit();
}

void fn_800A79D4(void) {
    Mov_Start();
}

void fn_800A79F4(void) {
    Mov_Tick();
}

void fn_800A7A14(void) {
    fn_800B0858();
}

s32 fn_800A7A34(s32 p0, s32 p1, s32 p2) {
    fn_800AD0C4();
    fn_800A402C(p2);
    fn_800A8DC8(p0, p1, p2, 0);
    return 1;
}

void fn_800A7A98(s32 p0) {
    fn_800AD1C4();
    fn_800A4038();
    fn_800A8D88(p0);
}

void fn_800A7AD0(void) {
    fn_800A94F4();
}

// ---- end of sweep code ----
