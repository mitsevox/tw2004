// uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree): runs the menu
// UI. Prepares the loaded UI file, passes the UI's commands to whichever part of the game is
// running (start-up, the menus or a round), and formats numbers for it.

#include "golfer.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"

u8 lbl_80281F19;
u8 lbl_80281F1A;                // set: fn_8008FD60 passes events to the UI
u8 lbl_80281F1B;

void fn_800B1D3C(s32 nCmd, s32 a, s32 b);
void fn_8008F80C(s32 p0, s32 p1);
s32 fn_80092BC4();
s32 fn_800934F8();
s32 fn_800BA038();
s32 fn_80090400();
void fn_80090664(void);
void fn_8008FE88(FrontEnd* pFE);
void fn_8008F820(void);
void fn_800E573C(void);         // GameMessages.c
void fn_800E5798(void);         // GameMessages.c
void fn_800E5708(void);         // GameMessages.c
void fn_80092BA0(void);         // uiText.c
void fn_8008EC30(void);         // uiLoadFile.c
void fn_800B9FF0(void);
// UIStudio.c, with the front end's view of the handler (uistudio.h cannot be included with
// game/frontend.h; it takes a UIStudio*).
void fn_80168B80(void* pHandler, u32 uEvent);

u8 fn_8008F39C(void) {
    return lbl_80281F1B;
}

// Add uBase to n words, nStride words apart.
void fn_8008F3A4(u32* p, uptr uBase, int nStride, u32 n) {
    u32 i;

    for (i = 0; i < n; i++) {
        *p += uBase;
        p += nStride;
    }
}

// Turn the UI file's offsets into pointers.
// port: the file stores 32-bit offsets in its pointer fields, as the GameCube's pointers are.
void fn_8008F488(FrontEnd* pFE) {
    UIFile* pFile = pFE->pFile;
    u32 i;

    pFE->pFile->p4 = (u32*)((uptr)pFE->pFile->p4 + (uptr)pFile);
    pFE->pFile->p8 = (u32*)((uptr)pFE->pFile->p8 + (uptr)pFile);
    fn_8008F3A4(pFE->pFile->p4 + 1, (uptr)pFile, 1, pFE->pFile->p4[0] * 2);
    fn_8008F3A4(pFE->pFile->p8 + 1, (uptr)pFile, 1, pFE->pFile->p8[0]);
    for (i = 0; i < pFE->pFile->p8[0]; i++) {
        fn_8008F3A4((u32*)pFE->pFile->p8[1 + i] + 1, (uptr)pFile, 1, *(u32*)pFE->pFile->p8[1 + i]);
    }
}

// Pass a UI command to the part of the game that is running: by the session's game type,
// start-up (1), the menus (3) or a round (4 to 8).
void fn_8008F568(s32 nCmd, s32 unused1, s32 unused2, s32 unused3, s32 a, s32 b) {
    if (gSession.nGameType == 1) {
        fn_800B1D3C(nCmd, a, b);
    }
    // port: the studio passes the addresses of the command's values and answer as 32-bit words
    if (gSession.nGameType == 3) {
        fn_80079E6C(nCmd, (MsgArg*)a, (MsgArg*)b);
    } else if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        fn_800850E4(nCmd, (MsgArg*)a, (MsgArg*)b);
    }
}

// Entry n of the UI file's first table: the second word of its pair (0 past the end).
u32 fn_8008F610(void* unused, u16 n) {
    u32* pTable = lbl_80281F1C->pFile->p4;

    if (n >= pTable[0]) return 0;
    return pTable[2 + n * 2];
}

void fn_8008F644(void) {
}

void fn_8008F80C(s32 n, s32 b) {
    lbl_801D87C0.a30[n] = b;
}

// Passes an event to the UI (while lbl_80281F1A is set). With lbl_80281F19 set it then shuts the UI
// down (fn_80090400) and sets lbl_80281F1B; otherwise fn_8008F820 runs.
void fn_8008FD60(u32 uEvent) {
    if (lbl_80281F1A) {
        if (lbl_80281F1C != NULL) {
            fn_80168B80(lbl_80281F1C->pHandler, uEvent);
        }
        if (lbl_80281F19) {
            fn_80090400(lbl_80281F1C);
            lbl_80281F19 = 0;
            lbl_80281F1A = 0;
            lbl_80281F1B = 1;
        } else if (lbl_80281F1C != NULL) {
            fn_8008F820();
        }
    }
}

// A UI name starting "tu": 1 in a lesson, -1 otherwise; 0 for any other name, and
// always 0 when the game type is 1.
int fn_8008FFF0(const char* szName) {
    if (gSession.nGameType == 1) return 0;
    if (szName[0] == 't' && szName[1] == 'u') {
        if (fn_80100294()) return 1;
        return -1;
    }
    return 0;
}

// Resets the front end's screen state.
void fn_800905A8(void) {
    s32 i;

    fn_80092BA0();
    fn_8008EC30();
    lbl_801D87C0.n34 = 0;
    lbl_801D87C0.n38 = 0;
    lbl_801D87C0.b49 = 0;
    lbl_801D87C0.b40 = 0;
    for (i = 0; i < 4; i++) {
        lbl_801D87C0.a18[i] = 0;
        lbl_801D87C0.a2C[i] = 0;
        lbl_801D87C0.a30[i] = 1;
    }
    lbl_801D87C0.b48 = 0;
    fn_800B9FF0();
    fn_800E5708();
}

// Sends the pending messages (GameMessages.c): game type 3 has its own set.
void fn_80090628(void) {
    if (gSession.nGameType == 3) {
        fn_800E573C();
    } else {
        fn_800E5798();
    }
}

void fn_80090664(void) {
    if (lbl_80281F1C != NULL) {
        fn_80090400(lbl_80281F1C);
    }
    fn_800934F8();
    fn_80092BC4();
    fn_800BA038();
}

void fn_800907AC(int nValue, char* szOut) {
    char aBuf[128];
    int nDigit = 1;
    u32 nLen;
    u32 nCommas;
    int i;

    sprintf(szOut, "%d", nValue);
    nLen = strlen(szOut);
    nCommas = nLen / 3;
    if (nLen % 3 == 0) {
        nCommas--;
    }
    aBuf[nLen + nCommas] = '\0';
    for (i = nLen + nCommas - 1; i >= 0; i--) {
        if (nDigit % 4 == 0) {
            aBuf[i] = ',';
        } else {
            aBuf[i] = szOut[nLen - 1];
            nLen--;
        }
        nDigit++;
    }
    sprintf(szOut, aBuf);       // EA: the result is used as a format; it holds only digits, '-' and ','
}

void fn_80090890(void) {
}

void fn_80090894(void) {
}

void fn_80090898(void) {
    fn_8008FE88(lbl_80281F1C);
}

void fn_800908BC(void) {
}

void fn_800908C0(void) {
}

void fn_800908C4(void) {
}

void fn_800908C8(void) {
}

void fn_800908CC(void) {
}

void fn_800908D0(void) {
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_8001005C();
void fn_800107E4();
s32 fn_80012EC4();

void fn_800908D4(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xB4) = x0;
}

void fn_80090904(s32 p0) {
    s32 t0;
    t0 = fn_8001005C();
    fn_800107E4(p0, t0);
}

// ---- end of sweep code ----
