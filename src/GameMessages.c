// GameMessages.c (our name): the game's messages to the front end (the HUD and menu screens):
// each sends a message id plus up to eight int-or-float values, and the display queues' handlers.

#include "golfer.h"

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

void  fn_80005AE8(void* p, int c, int n);   // memset
void  fn_8016B09C(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);
u32   strlen(const char* s);
void  fn_800E5A4C(int nMsg, u32 uFloats, void* p0, void* p1, void* p2);
void  fn_800E5B0C(int nMsg, u32 uFloats, void* p0, void* p1, void* p2, void* p3, void* p4);
void  fn_800E3C0C(int a);
void  fn_80062C80(int a, int b);
extern s32 lbl_80282278;
void  fn_800E3B04(void);
u8    fn_80095430(int a);
void  fn_800953C8(int a);
extern u8 lbl_802822DF;
void  fn_800DC9D4(int a);
void  fn_8009EF98(void);
extern s32 lbl_802822B0;
extern s32 lbl_8028229C;
extern s32 lbl_802822B8;
typedef struct UIQueueItem {
    s32 a;
    s32 b;
    s32 c;
} UIQueueItem;
extern UIQueueItem lbl_802030BC[10];
void  fn_80062D38(int a, int b, int c);
extern u8 lbl_80203138[14];

extern u8 lbl_802822BE;
void fn_80062D6C();
void fn_800E4FFC(int p0);
void fn_800E502C(int p0);
void fn_800E505C(int p0);
void fn_800E508C(void);
extern u8 lbl_802822BD;
void fn_800E50FC(void);
extern u8 lbl_80282282;
u8 fn_800E5110(void);
void fn_800E5200(int p0);
void fn_800E5228(void);
extern u8 lbl_80202B88[];
void fn_800E522C(int p0);
extern u8 lbl_802822BC;
extern u8 lbl_802822DA;
void fn_800E53A4(u8 v);
void fn_800E53AC(void);
u8 fn_800E53B8(void);
void fn_800A72EC();
void fn_800E53C0(void);
extern u8 lbl_80282280;
void fn_800E53E8(u8 v);
void fn_800E542C(void);
void fn_800E5450(void);
void fn_800E5474(int p0);
void fn_800E53F0(int nMsg, int a, int b, int c);
void fn_800E54A0(int p0, int p1, int p2);
void fn_800E54D8(int p0, int p1, int p2);
void fn_800E5510(int p0, int p1, int p2);
void fn_800E5548(int p0, int p1, int p2);
void fn_800E5580(int p0, int p1, int p2);
void fn_800E55B8(int p0, int p1, int p2);
void fn_800E55F0(int p0, int p1, int p2);
void fn_800E5628(int p0, int p1, int p2);
void fn_800E5660(int p0, int p1, int p2);
void fn_800E5698(int p0, int p1, int p2);
void fn_800E56D0(int p0, int p1, int p2);
extern u8 lbl_802822E4;
void fn_800E5708(void);
void fn_800E5714(int p0);
extern s32 lbl_802822E0;
void fn_800E5724(int v);
void fn_800E572C(int p0);
void fn_800E58B4(int arg0);
void fn_800E573C(void);
void fn_800E5908(int nMsg);
void fn_800E5C84(void);
void fn_800E5C08(int nMsg, char* pStr);
void fn_800E5D40(int p0);
void fn_800E5D68(char* p0);
extern u8 lbl_801D87C0[];
s32 fn_800E5D90(void);

void fn_800E4FFC(int p0) {
    fn_80062D6C(48, p0);
    lbl_802822BE = 1;
}

void fn_800E502C(int p0) {
    fn_80062D6C(97, p0);
    lbl_802822BE = 1;
}

void fn_800E505C(int p0) {
    fn_80062D6C(57, p0);
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

void fn_800E5200(int p0) {
    fn_80062D6C(60, p0);
}

void fn_800E5228(void) {
}

void fn_800E522C(int p0) {
    *(u8*)(lbl_80202B88 + p0) = 1;
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
        if (gSession.unk14 == 2) {
            if (!fn_80095430(1)) {
                fn_800953C8(1);
            }
            lbl_802822DF = 0;
            fn_800DC9D4(0);
            fn_8009EF98();
            gSession.unk14 = 0;
            return;
        }
        gSession.unk14 = 1;
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
            if (lbl_802030BC[i].a < 10) {
                return 1;
            }
        }
    }
    return 0;
}

void fn_800E53A4(u8 v) {
    lbl_802822BC = v;
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

void fn_800E53E8(u8 v) {
    lbl_80282280 = v;
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

void fn_800E5474(int p0) {
    fn_80062D6C(47, (p0 & 0xFF));
}

void fn_800E54A0(int p0, int p1, int p2) {
    fn_800E53F0(96, p0, p1, p2);
}

void fn_800E54D8(int p0, int p1, int p2) {
    fn_800E53F0(95, p0, p1, p2);
}

void fn_800E5510(int p0, int p1, int p2) {
    fn_800E53F0(94, p0, p1, p2);
}

void fn_800E5548(int p0, int p1, int p2) {
    fn_800E53F0(93, p0, p1, p2);
}

void fn_800E5580(int p0, int p1, int p2) {
    fn_800E53F0(92, p0, p1, p2);
}

void fn_800E55B8(int p0, int p1, int p2) {
    fn_800E53F0(91, p0, p1, p2);
}

void fn_800E55F0(int p0, int p1, int p2) {
    fn_800E53F0(27, p0, p1, p2);
}

void fn_800E5628(int p0, int p1, int p2) {
    fn_800E53F0(12, p0, p1, p2);
}

void fn_800E5660(int p0, int p1, int p2) {
    fn_800E53F0(11, p0, p1, p2);
}

void fn_800E5698(int p0, int p1, int p2) {
    fn_800E53F0(13, p0, p1, p2);
}

void fn_800E56D0(int p0, int p1, int p2) {
    fn_800E53F0(10, p0, p1, p2);
}

void fn_800E5708(void) {
    lbl_802822E4 = 0;
}

void fn_800E5714(int p0) {
    lbl_802822E4 = (lbl_802822E4 | p0);
}

void fn_800E5724(int v) {
    lbl_802822E0 = v;
}

void fn_800E572C(int p0) {
    lbl_802822E4 = (lbl_802822E4 ^ p0);
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
void fn_800E590C(int nMsg, u32 uFloats, void* p0) {
    MsgArg args[1];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)p0;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)p0;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 1, args);
}

// Two values.
void fn_800E5998(int nMsg, u32 uFloats, void* p0, void* p1) {
    MsgArg args[2];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)p0;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)p0;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)p1;
    } else {
        args[1].i = *(s32*)p1;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 2, args);
}

// Three values.
void fn_800E5A4C(int nMsg, u32 uFloats, void* p0, void* p1, void* p2) {
    MsgArg args[3];
    fn_80005AE8(args, 0, sizeof(args));
    fn_800E5908(nMsg);
    if (uFloats & 1) {
        args[0].f = *(f32*)p0;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)p0;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)p1;
    } else {
        args[1].i = *(s32*)p1;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)p2;
    } else {
        args[2].i = *(s32*)p2;
    }
    fn_8016B09C(lbl_80281F1C->pHandler, nMsg, 3, args);
}

// Five values.
void fn_800E5B0C(int nMsg, u32 uFloats, void* p0, void* p1, void* p2, void* p3, void* p4) {
    MsgArg args[5];
    fn_800E5908(nMsg);
    fn_80005AE8(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)p0;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)p0;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)p1;
    } else {
        args[1].i = *(s32*)p1;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)p2;
    } else {
        args[2].i = *(s32*)p2;
    }
    if (uFloats & 8) {
        args[3].f = *(f32*)p3;
    } else {
        args[3].i = *(s32*)p3;
    }
    if (uFloats & 0x10) {
        args[4].f = *(f32*)p4;
    } else {
        args[4].i = *(s32*)p4;
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

void fn_800E5C84(void) {
    fn_800E5D90();
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

void fn_800E5D40(int p0) {
    fn_80062D6C(89, p0);
}

void fn_800E5D68(char* p0) {
    fn_800E5C08(90, p0);
}

s32 fn_800E5D90(void) {
    return *(u8*)lbl_801D87C0;
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
