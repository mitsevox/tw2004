// uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree): runs the menu
// UI. Prepares the loaded UI file, passes the UI's commands to whichever part of the game is
// running (start-up, the menus or a round), and formats numbers for it.

#include "golfer.h"
#include "game/frontend.h"

u8 lbl_80281F1B;

void fn_800B1D3C(s32 nCmd, s32 a, s32 b);
void fn_80079E6C(s32 nCmd, s32 a, s32 b);
void fn_8008F80C(s32 p0, s32 p1);
s32 fn_80092BC4();
s32 fn_800934F8();
s32 fn_800BA038();
s32 fn_80090400();
void fn_80090664(void);
void fn_8008FE88(FrontEnd* pFE);

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
    if (gSession.nGameType == 3) {
        fn_80079E6C(nCmd, a, b);
    } else if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        // port: the studio passes the command's values and answer as 32-bit words
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

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801D87C0[];

void fn_8008F80C(s32 p0, s32 p1) {
    *(u8*)((lbl_801D87C0 + p0) + 0x30) = p1;
}

// ---- end of sweep code ----

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
