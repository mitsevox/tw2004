// GameMessages.c (our name): the game's messages to the front end (the HUD and menu screens):
// each sends a message id plus up to eight int-or-float values, and the display queues' handlers.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// One value of a message: an int or a float (the mask passed with it says which), or a pointer.
typedef union MsgArg {
    s32   i;
    f32   f;
    void* p;
} MsgArg;

// A string value: the message holds a pointer to this.
typedef struct MsgString {
    s32   n0;
    s32   nLen;
    char* pStr;
} MsgString;

typedef struct FrontEnd {
    u8    unk0[4];
    void* pHandler;             // 0x4
} FrontEnd;

extern FrontEnd* lbl_80281F1C;
extern u8 lbl_80202B88[];
extern u8 lbl_802822E4;
extern s32 lbl_802822E0;
extern u8 lbl_801D87C0[];

void  fn_8016B09C(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);
void  fn_800E508C(void);
void  fn_800E50FC(void);
void  fn_800E5200(int n);
void  fn_800E522C(int n);
void  fn_800E53A4(u8 b);
void  fn_800E53AC(void);
void  fn_800E53C0(void);
void  fn_800E53E8(u8 b);
void  fn_800E5708(void);
void  fn_800E572C(int n);
void  fn_800E573C(void);
void  fn_800E5908(int nMsg);
void  fn_800E5D40(int n);
void  fn_800E5D68(char* pStr);
u8    fn_800E5D90(void);

void fn_800E4FFC(int n) {
    fn_80062D6C(48, n);
    lbl_802822BE = 1;
}

void fn_800E502C(int n) {
    fn_80062D6C(97, n);
    lbl_802822BE = 1;
}

void fn_800E505C(int n) {
    fn_80062D6C(57, n);
    lbl_802822BE = 1;
}

void fn_800E508C(void) {
    lbl_802822BE = 1;
}

// A pending end-of-hole score screen goes up: the HUD comes back and the player's screen is shown.
u8 fn_800E5098(void) {
    if (lbl_802822BD) {
        fn_800E3C0C(1);
        fn_80062C80(gPlayers[lbl_80282278].nC58, 1);
        lbl_802822BD = 0;
        return 1;
    }
    return lbl_802822BE;
}

void fn_800E50FC(void) {
    lbl_802822BE = 0;
    lbl_802822BD = 1;
}

u8 fn_800E5110(void) {
    return lbl_80282282;
}

// Message 0x18 with the view: 0 on one screen, else 2 or 3 by player.
void fn_800E5118(int a, int b, int nPlayer) {
    int nView;
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            nView = 2;
        } else {
            nView = 3;
        }
    } else {
        nView = 0;
    }
    fn_800E53F0(0x18, a, b, nView);
}

// Message 0x19: four floats and the view (0 or 1 in split screen).
void fn_800E5178(int nPlayer, f32 a, f32 b, f32 c, f32 d) {
    int nView;
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            nView = 0;
        } else {
            nView = 1;
        }
    } else {
        nView = 0;
    }
    fn_800E5B0C(0x19, 0xF, &a, &b, &c, &d, &nView);
}

void fn_800E5200(int n) {
    fn_80062D6C(60, n);
}

void fn_800E5228(void) {
}

void fn_800E522C(int n) {
    lbl_80202B88[n] = 1;
}

// A menu screen closes: the next one still open comes up; with none left, the game unpauses
// (state 2) or stays paused (state 1).
void fn_800E5240(int i) {
    int k;
    u8* p;
    if (lbl_80202B88[i]) {
        if (i >= 0) {
            lbl_80202B88[i] = 0;
        }
        for (k = 0, p = lbl_80202B88; k < 9; k++, p++) {
            if (*p) {
                fn_800E522C(k);
                return;
            }
        }
        fn_800E3B04();
        if (gSession.n14 == 2) {
            if (!fn_80095430(1)) {
                fn_800953C8(1);
            }
            lbl_802822DF = 0;
            fn_800DC9D4(0);
            fn_8009EF98();
            gSession.n14 = 0;
            return;
        }
        gSession.n14 = 1;
    }
}

void fn_800E5314(void) {
    lbl_80202B88[0] = 0;
    lbl_80202B88[1] = 0;
    lbl_80202B88[2] = 0;
    lbl_80202B88[3] = 0;
    lbl_80202B88[4] = 0;
    lbl_80202B88[5] = 0;
    lbl_80202B88[6] = 0;
    lbl_80202B88[7] = 0;
    lbl_80202B88[8] = 0;
}

// Whether a big message is waiting: queue 2 or 6, or a queue-1 item below 10.
u8 fn_800E5344(void) {
    int i;
    if (lbl_802822B0 > 0 || lbl_8028229C > 0) {
        return 1;
    }
    if (lbl_802822B8 > 0) {
        for (i = 0; i < lbl_802822B8; i++) {
            if (lbl_802030BC[i].n0 < 10) {
                return 1;
            }
        }
    }
    return 0;
}

void fn_800E53A4(u8 b) {
    lbl_802822BC = b;
}

void fn_800E53AC(void) {
    lbl_802822DA = 1;
}

u8 fn_800E53B8(void) {
    return lbl_802822DA;
}

void fn_800E53C0(void) {
    fn_800A72EC(1, 0);
}

void fn_800E53E8(u8 b) {
    lbl_80282280 = b;
}

// Sends a message with three ints.
void fn_800E53F0(int nMsg, int a, int b, int c) {
    fn_800E5A4C(nMsg, 0, &a, &b, &c);
}

void fn_800E542C(void) {
    fn_800E58B4(6);
}

void fn_800E5450(void) {
    fn_800E58B4(4);
}

void fn_800E5474(int n) {
    fn_80062D6C(47, (n & 0xFF));
}

void fn_800E54A0(int nA, int nB, int nC) {
    fn_800E53F0(96, nA, nB, nC);
}

void fn_800E54D8(int nA, int nB, int nC) {
    fn_800E53F0(95, nA, nB, nC);
}

void fn_800E5510(int nA, int nB, int nC) {
    fn_800E53F0(94, nA, nB, nC);
}

void fn_800E5548(int nA, int nB, int nC) {
    fn_800E53F0(93, nA, nB, nC);
}

void fn_800E5580(int nA, int nB, int nC) {
    fn_800E53F0(92, nA, nB, nC);
}

void fn_800E55B8(int nA, int nB, int nC) {
    fn_800E53F0(91, nA, nB, nC);
}

void fn_800E55F0(int nA, int nB, int nC) {
    fn_800E53F0(27, nA, nB, nC);
}

void fn_800E5628(int nA, int nB, int nC) {
    fn_800E53F0(12, nA, nB, nC);
}

void fn_800E5660(int nA, int nB, int nC) {
    fn_800E53F0(11, nA, nB, nC);
}

void fn_800E5698(int nA, int nB, int nC) {
    fn_800E53F0(13, nA, nB, nC);
}

void fn_800E56D0(int nA, int nB, int nC) {
    fn_800E53F0(10, nA, nB, nC);
}

void fn_800E5708(void) {
    lbl_802822E4 = 0;
}

void fn_800E5714(int n) {
    lbl_802822E4 = (lbl_802822E4 | n);
}

void fn_800E5724(int n) {
    lbl_802822E0 = n;
}

void fn_800E572C(int n) {
    lbl_802822E4 = (lbl_802822E4 ^ n);
}

void fn_800E573C(void) {
    if ((s8) lbl_802822E4 != 0) {
        if (lbl_802822E4 & 1) {
            fn_800E58B4(0x8D);
            fn_800E572C(1);
        }
        if (lbl_802822E4 & 2) {
            fn_800E58B4(0x54);
            fn_800E572C(2);
        }
    }
}

// The other pending-message flags, sent once each.
void fn_800E5798(void) {
    MsgArg args[3];
    if ((s8)lbl_802822E4 != 0) {
        if (lbl_802822E4 & 1) {
            fn_80005AE8(args, 0, sizeof(args));
            args[0].i = 15;
            args[1].f = 0.0f;
            args[2].i = lbl_802822E0;
            fn_8016B09C(lbl_80281F1C->pHandler, 5, 3, args);
            fn_800E572C(1);
        }
        if (lbl_802822E4 & 2) {
            fn_800E58B4(0x3D);
            fn_800E572C(2);
            fn_80062D6C(1, 1);
        }
        if (lbl_802822E4 & 4) {
            fn_800E58B4(0x31);
            fn_800E572C(4);
        }
        if (lbl_802822E4 & 8) {
            fn_800E58B4(0x27);
            fn_800E572C(8);
        }
        if (lbl_802822E4 & 0x10) {
            fn_80062D38(0x62, 0, lbl_802822E0);
            fn_800E572C(0x10);
        }
        if (lbl_802822E4 & 0x20) {
            fn_80062D6C(0x21, lbl_802822E0);
            fn_800E572C(0x20);
        }
    }
}

// Sends a message with no values.
void fn_800E58B4(int nMsg) {
    MsgArg arg;
    fn_800E5908(nMsg);
    fn_80005AE8(&arg, 0, sizeof(arg));
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 0, &arg);
}

void fn_800E5908(int nMsg) {
}

// Sends a message with one value (bit 0 of uFloats: a float).
void fn_800E590C(int nMsg, u32 uFloats, void* pA) {
    MsgArg args[1];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 1, args);
}

// Two values.
void fn_800E5998(int nMsg, u32 uFloats, void* pA, void* pB) {
    MsgArg args[2];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 2, args);
}

// Three values.
void fn_800E5A4C(int nMsg, u32 uFloats, void* pA, void* pB, void* pC) {
    MsgArg args[3];
    fn_80005AE8(args, 0, sizeof(args));
    fn_800E5908(nMsg);
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)pC;
    } else {
        args[2].i = *(s32*)pC;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 3, args);
}

// Five values.
void fn_800E5B0C(int nMsg, u32 uFloats, void* pA, void* pB, void* pC, void* pD, void* pE) {
    MsgArg args[5];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)pC;
    } else {
        args[2].i = *(s32*)pC;
    }
    if (uFloats & 8) {
        args[3].f = *(f32*)pD;
    } else {
        args[3].i = *(s32*)pD;
    }
    if (uFloats & 0x10) {
        args[4].f = *(f32*)pE;
    } else {
        args[4].i = *(s32*)pE;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 5, args);
}

// Sends a message with a string.
void fn_800E5C08(int nMsg, char* pStr) {
    MsgString str;
    MsgArg arg;
    fn_800E5908(nMsg);
    fn_80005AE8(&arg, 0, sizeof(arg));
    str.pStr = pStr;
    arg.p = &str;
    ((MsgString*)arg.p)->nLen = strlen(pStr);
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 1, &arg);
}

u8 fn_800E5C84(void) {
    return fn_800E5D90();
}

// Message 0x42: seven ints and a float (the sixth value).
void fn_800E5CA4(int a, int b, int c, int d, int e, int g, int h, f32 f) {
    MsgArg args[8];
    fn_800E5908(0x42);
    fn_80005AE8(args, 0, sizeof(args));
    args[0].i = a;
    args[1].i = b;
    args[2].i = c;
    args[3].i = d;
    args[4].i = e;
    args[5].f = f;
    args[6].i = g;
    args[7].i = h;
    fn_8016B09C(lbl_80281F1C->pHandler, 0x42, 8, args);
}

void fn_800E5D40(int n) {
    fn_80062D6C(89, n);
}

void fn_800E5D68(char* pStr) {
    fn_800E5C08(90, pStr);
}

u8 fn_800E5D90(void) {
    return lbl_801D87C0[0];
}

void fn_800E5DA0(void) {
    lbl_80203138[0] = 0;
    lbl_80203138[1] = 0;
    lbl_80203138[2] = 0;
    lbl_80203138[3] = 0;
    lbl_80203138[4] = 0;
    lbl_80203138[5] = 0;
    lbl_80203138[6] = 0;
    lbl_80203138[7] = 0;
    lbl_80203138[8] = 0;
    lbl_80203138[9] = 0;
    lbl_80203138[10] = 0;
    lbl_80203138[11] = 0;
    lbl_80203138[12] = 0;
    lbl_80203138[13] = 0;
}

// A tutorial tip (below 15; tip 12 can repeat, the others show once), not when bD4 is set.
void fn_800E5DE4(int n) {
    if (!gpGame->bD4 || n >= 15) {
        if (n < 15 && n != 12) {
            lbl_80203138[n] = 1;
        }
        fn_800E5714(0x20);
        fn_800E5724(n);
    }
}
