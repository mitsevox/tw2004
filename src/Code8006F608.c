// Code8006F608.c (our name): split from GoShaderObjectContainer_OBFData_Gc.c. Its extent is its
// data: lbl_802811F0 (.sdata), lbl_801D7100 (.bss), the table lbl_80188900 (.data, then 4 bytes of
// padding before GoShaderObjectContainer_OBFData_Gc.c's assert string) and its .sdata2 floats.

#include "game.h"
#include "ball.h"
#include "psmgr.h"
#include "core/startup.h"

void fn_80055C24(int n);        // Ball.c
void fn_800B24E0(f32 f);        // shadow.c

Unk802811F0 lbl_801D7100;
Unk802811F0* lbl_802811F0 = &lbl_801D7100;

Unk80188900 lbl_80188900[NUM_COURSE_DATA][5] = {
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.88f, 5, 8}, {0.12f, 3, 5}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.92f, 5, 8}, {0.08f, 3, 7}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.88f, 5, 8}, {0.12f, 3, 5}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.85f, 4, 8}, {0.15f, 3, 6}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.82f, 3, 7}, {0.18f, 3, 7}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.82f, 3, 7}, {0.18f, 3, 7}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
    {{0.9f, 5, 8}, {0.1f, 5, 10}, {0.0f, 0, 0}, {0.0f, 0, 0}, {0.0f, 0, 0}},
};

void fn_8006F608(void);
void fn_8006F64C(void);
void fn_8006FB10(void);
void fn_8006FCC4(u32 uSeed);
void fn_8006FCC8(void);
u8   fn_8006FCCC(void);         // a flag (fn_8006F650 tests it as a byte)
void fn_8006FCD4(void);
void fn_8006FCD8(void);

// ---- sweep code (not yet cleaned up) ----

void fn_8006F608(void) {
    lbl_802811F0->uFlags = 0;
    lbl_802811F0->u04 = 0;
    lbl_802811F0->b14 = 1;
    lbl_802811F0->n10 = 0;
    lbl_802811F0->n0C = 0;
    lbl_802811F0->n08 = 0;
    lbl_802811F0->b1C = 0;
}

void fn_8006F64C(void) {
}

// ---- end of sweep code ----

// Picks lbl_802811F0's flags for the hole, then reseeds random stream 1 and applies them
// (fn_8006FB10). Split screen (or session flag 0x4000) gets bit 0 only; else a wind over 6 (wind
// option on) on courses 0, 1, 2 and 6 sets bit 4, then b1C sets bit 1, or the game option nC
// picks (not on course 4's hole 18): a roll over the course's lbl_80188900 choices with f18 rolled
// 0.25..1 (mode 1; mode 2 keeps a choice for a rolled number of calls while f18 rises then falls
// by 0.3), bit 1 with f18 rolled (mode 3) or bit 0 (modes 0 and 4).
void fn_8006F650(void) {
    u32 uSeed;

    if (gSession.nSplitScreen || (gSession.uFlags & 0x4000)) {
        lbl_802811F0->uFlags |= 1;
    } else {
        if (Wind_Get(NULL) > 6.0f && gSession.options.nWind != 0) {
            if (Game_GetCourse() == 0 || Game_GetCourse() == 1 || Game_GetCourse() == 2 ||
                Game_GetCourse() == 6) {
                lbl_802811F0->uFlags |= 0x10;
            }
        }
        if (lbl_802811F0->b1C) {
            lbl_802811F0->uFlags |= 2;
        } else if (Game_GetCourse() != 4 || fn_80015464() != 17) {
            switch (gSession.options.nC) {
            case 0:
            case 4:
                lbl_802811F0->uFlags |= 1;
                break;
            case 2: {
                f32 fSum;
                f32 fRoll;
                int i;

                if (lbl_802811F0->n10 >= lbl_802811F0->n0C || lbl_802811F0->b14) {
                    fSum = 0.0f;
                    fRoll = Misc_RandFuncf(1);
                    lbl_802811F0->n08 = 0;
                    for (i = 0; fRoll > fSum; i++) {
                        fSum += lbl_80188900[Game_GetCourse()][i].fShare;
                        if (fRoll <= fSum) {
                            lbl_802811F0->uFlags |= 1 << i;
                            lbl_802811F0->n08 = i;
                            break;
                        }
                    }
                    lbl_802811F0->n0C = Misc_RandFuncf(1) * (f32)(lbl_80188900[Game_GetCourse()][i].nMax -
                                                                  lbl_80188900[Game_GetCourse()][i].nMin) +
                                        (f32)lbl_80188900[Game_GetCourse()][i].nMin;
                    lbl_802811F0->b14 = 0;
                    lbl_802811F0->n10 = 1;
                    lbl_802811F0->f18 = 0.3f;
                } else {
                    lbl_802811F0->n10++;
                    lbl_802811F0->uFlags |= 1 << lbl_802811F0->n08;
                    if (lbl_802811F0->n0C % 2 == 0) {
                        if (lbl_802811F0->n10 <= lbl_802811F0->n0C / 2) {
                            lbl_802811F0->f18 += 0.3f;
                        } else if (lbl_802811F0->n10 != lbl_802811F0->n0C / 2 + 1) {
                            lbl_802811F0->f18 -= 0.3f;
                        }
                    } else if (lbl_802811F0->n10 <= lbl_802811F0->n0C / 2 + 1) {
                        lbl_802811F0->f18 += 0.3f;
                    } else {
                        lbl_802811F0->f18 -= 0.3f;
                    }
                }
                break;
            }
            case 1: {
                f32 fSum;
                f32 fRoll;
                int i;

                fSum = 0.0f;
                fRoll = Misc_RandFuncf(1);
                for (i = 0; fRoll > fSum; i++) {
                    fSum += lbl_80188900[Game_GetCourse()][i].fShare;
                    if (fRoll <= fSum) {
                        lbl_802811F0->uFlags |= 1 << i;
                        lbl_802811F0->n08 = i;
                        break;
                    }
                }
                lbl_802811F0->f18 = 0.75f * Misc_RandFuncf(1) + 0.25f;
                break;
            }
            case 3:
                lbl_802811F0->uFlags |= 2;
                lbl_802811F0->f18 = 0.75f * Misc_RandFuncf(1) + 0.25f;
                break;
            case 5:
            case 6:
                break;
            }
            lbl_802811F0->b14 = 0;
        }
    }
    if (fn_8006FCCC()) {
        fn_8006FCC8();
    } else {
        uSeed = Misc_RandFunc(1);
        Misc_SetSeedFunc(1, uSeed);
        fn_8006FCC4(uSeed);
    }
    fn_8006FB10();
}

// Applies lbl_802811F0's flags: bit 0 calls fn_800B24E0 with 1; bit 1 starts effects 0 (with f18
// kept to 0.1..1), 2 and 1 and calls fn_80055C24 with whether f18 is under 0.5; bit 4 starts
// effect 3.
void fn_8006FB10(void) {
    f32 fAmount;

    if (lbl_802811F0->uFlags & 1) {
        fn_800B24E0(1.0f);
    }
    if (lbl_802811F0->uFlags & 2) {
        fAmount = lbl_802811F0->f18 < 0.1f ? 0.1f : (lbl_802811F0->f18 > 1.0f ? 1.0f : lbl_802811F0->f18);
        fn_800A2A80(0, &fAmount, 2);
        fn_800A2A80(2, NULL, 2);
        fn_800A2A80(1, NULL, 2);
        if (lbl_802811F0->f18 < 0.5f) {
            fn_80055C24(1);
        } else {
            fn_80055C24(0);
        }
    }
    if (lbl_802811F0->uFlags & 0x10) {
        fn_800A2A80(3, NULL, 2);
    }
}

// Ends fn_8006FB10's flags: keeps them in u04; bit 1 stops effects 0, 2 and 1 and calls
// fn_80055C24 with 2, bit 4 stops effect 3; clears bits 0, 1, 2 and 4 (bit 0's fn_800B24E0 call
// is not undone).
void fn_8006FBF8(void) {
    lbl_802811F0->b1C = 0;
    lbl_802811F0->u04 = lbl_802811F0->uFlags;
    if (lbl_802811F0->uFlags & 1) {
        lbl_802811F0->uFlags &= ~1;
    }
    if (lbl_802811F0->uFlags & 2) {
        fn_800A2B34(0);
        fn_800A2B34(2);
        fn_800A2B34(1);
        fn_80055C24(2);
        lbl_802811F0->uFlags &= ~2;
    }
    if (lbl_802811F0->uFlags & 4) {
        lbl_802811F0->uFlags &= ~4;
    }
    if (lbl_802811F0->uFlags & 0x10) {
        fn_800A2B34(3);
        lbl_802811F0->uFlags &= ~0x10;
    }
}

// ---- sweep code (not yet cleaned up) ----

// uSeed: unused (the stub takes the seed fn_8006F650 hands it)
void fn_8006FCC4(u32 uSeed) {
}

void fn_8006FCC8(void) {
}

u8 fn_8006FCCC(void) {
    return 0;
}

void fn_8006FCD4(void) {
}

void fn_8006FCD8(void) {
}

// ---- end of sweep code ----
