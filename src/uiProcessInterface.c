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
void fn_80090400(FrontEnd* pFE);
void fn_80090664(void);
void fn_8008FE88(FrontEnd* pFE);
TexEntry* fn_80090904(TexBank* pBank, u64 uHash);
void fn_8008F820(void);
void fn_800E573C(void);         // GameMessages.c
void fn_800E5798(void);         // GameMessages.c
void fn_800E5708(void);         // GameMessages.c
void fn_80092BA0(void);         // uiText.c
void fn_8008EC30(void);         // uiLoadFile.c
void fn_800B9FF0(void);
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void fn_80099ED8(void);                 // BootCourse.c
void fn_80077340(void);                 // FE_Manager.c
void fn_80077344(void);                 // FE_Manager.c
void fn_80077348(void);                 // FE_Manager.c
// UIStudio.c, with the front end's view of the handler (uistudio.h cannot be included with
// game/frontend.h; it takes a UIStudio*).
void fn_80168B80(void* pHandler, u32 uEvent);
void fn_80169B4C(void* pHandler);      // UISApi.c: unload every screen
void fn_80168C24(void* pHandler, s32 nTicks);     // UIStudio.c: run the UI
void fn_8016B09C(void* pHandler, u32 uEvent, s32 nArgs, const s32* pArgs);
void fn_80016B6C(f32 x, f32 y);
void fn_80012898(s32 nMode);
void fn_80012C54(s32 v);
void fn_8001273C(void);
void fn_800908D4(f32 x0);

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

    pFE->pFile->p4 = (UIFilePairs*)((uptr)pFE->pFile->p4 + (uptr)pFile);
    pFE->pFile->p8 = (UIFileTables*)((uptr)pFE->pFile->p8 + (uptr)pFile);
    fn_8008F3A4((u32*)pFE->pFile->p4->aPairs, (uptr)pFile, 1, pFE->pFile->p4->nCount * 2);
    fn_8008F3A4((u32*)pFE->pFile->p8->apTables, (uptr)pFile, 1, pFE->pFile->p8->nCount);
    for (i = 0; i < pFE->pFile->p8->nCount; i++) {
        fn_8008F3A4((u32*)pFE->pFile->p8->apTables[i]->apEntries, (uptr)pFile, 1,
                    pFE->pFile->p8->apTables[i]->nCount);
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

// The studio's UISLoadFn: screen uScreen's data from the UI file's pairs (0 past the end). The
// group is ignored.
u32 fn_8008F610(u16 uGroup, u16 uScreen) {
    UIFilePairs* pPairs = lbl_80281F1C->pFile->p4;

    if (uScreen >= pPairs->nCount) return 0;
    return (u32)pPairs->aPairs[uScreen].p4;
}

void fn_8008F644(void) {
}

// Run and draw the UI for nTicks (while lbl_80281F1A is set), then step lbl_801D880C: counting
// 0..2 up, and past 2 it is reset and its n4 sent to the menus (0x23) or the round (0x24).
void fn_8008F648(s32 nTicks) {
    s32 aArgs[1];

    if (lbl_80281F1A) {
        fn_8001425C(0);
        fn_80035118(4, 5);
        fn_80012F34(0);
        fn_80012F50(1, 6, 1);
        fn_80012EF8();
        if (lbl_80281370 && gSession.nGameType == 3) {
            fn_80091454();
        }
        fn_80014194(NULL);
        fn_80016B6C(1.0f / 512.0f, 1.0f / 448.0f);
        fn_80012898(1);
        fn_80012C54(1);
        fn_800908D4(0.85f);
        if (lbl_80281F1C != NULL) {
            fn_80168C24(lbl_80281F1C->pHandler, nTicks);
        }
        fn_800908D4(1.0f);
        fn_80012C54(0);
        fn_80016B6C(1.0f, 1.0f);
        fn_80012F34(1);
        fn_80012F50(1, 6, 0x80);
        fn_80012F18(3);
        fn_80012EF8();
        if (lbl_801D880C.n0 <= 2 && lbl_801D880C.n0 >= 0) {
            lbl_801D880C.n0++;
        } else if (lbl_801D880C.n0 > 2) {
            lbl_801D880C.n0 = -1;
            fn_80005AE8(aArgs, 0, sizeof(aArgs));
            aArgs[0] = lbl_801D880C.n4;
            if (gSession.nGameType == 3) {
                fn_8016B09C(lbl_80281F1C->pHandler, 0x23, 1, aArgs);
            }
            if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
                fn_8016B09C(lbl_80281F1C->pHandler, 0x24, 1, aArgs);
            }
        }
        fn_8001273C();
    }
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

// Find the colour table among the UI file's tables (the one whose first entry is of kind 0x10)
// and turn its entries' colour offsets into pointers; with none, p14 is NULL.
void fn_8008FDDC(FrontEnd* pFE) {
    UIFile* pFile = pFE->pFile;
    u8 bFound = 0;
    int nTables;
    int i;

    nTables = pFile->p8->nCount;
    for (i = 0; i < nTables; i++) {
        UIColorTable* pTable = pFE->pFile->p8->apTables[i];

        // fake match: the original tests the count unsigned here (cmplwi), signed below
        if ((u32)pTable->nCount != 0 && pTable->apEntries[0]->u0 == 0x10) {
            pFE->p14 = pTable;
            bFound = 1;
        }
    }
    if (bFound) {
        for (i = 0; i < pFE->p14->nCount; i++) {
            pFE->p14->apEntries[i]->p8 = (u8*)((uptr)pFE->p14->apEntries[i]->p8 + (uptr)pFile);
        }
    } else {
        pFE->p14 = NULL;
    }
}

// Resolve the UI file's entries by their names: kind 1 (except in game type 3) to the texture of
// that name in the texture bank fn_8008FFF0 picks, kind 2 to the record of that name in pC (its
// table noted in lbl_801D87C0.n3C).
void fn_8008FE88(FrontEnd* pFE) {
    UIColorTable* pTable;
    UIColorEntry* pEntry;
    u64 uHash;
    char* szName;
    int nBank;
    u32 i;
    u32 j;
    u32 k;

    for (i = 0; i < pFE->pFile->p8->nCount; i++) {
        pTable = pFE->pFile->p8->apTables[i];
        for (j = 0; j < pTable->nCount; j++) {
            pEntry = pTable->apEntries[j];
            szName = pEntry->szC;
            if (pEntry->u0 == 1) {
                if (gSession.nGameType != 3) {
                    uHash = fn_8000BEE4(szName);
                    nBank = fn_8008FFF0(szName);
                    if (nBank != -1) {
                        pEntry->p4 = fn_80090904(lbl_80281F1C->p8->ap4[nBank], uHash);
                    }
                }
            } else if (pEntry->u0 == 2 && pFE->pC != NULL) {
                lbl_801D87C0.n3C = i;
                for (k = 0; k < pFE->pC->nCount; k++) {
                    if (strcmp(pEntry->szC, pFE->pC->apNames[k]) == 0) {
                        pEntry->p4 = pFE->pC->apNames[k];
                        pEntry->p8 = NULL;
                        break;
                    }
                }
            }
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

// Shut the front end down: in game type 1 with no nC, fe_movies.c's fn_80091EE8; in game type 3,
// every lbl_801D8890 entry whose lbl_801D8ED0 word is set gets b0 set and b1 cleared, and the
// menus' data is brought back and freed (fn_8008F294, fn_8008F24C). Then everything the front
// end loaded is freed, and the front end itself.
void fn_80090400(FrontEnd* pFE) {
    int i;

    fn_80090B10();
    if (gSession.nGameType == 1 && gSession.nC == 0) {
        fn_80091EE8();
    } else if (gSession.nGameType == 3) {
        for (i = 0; i < FE_NUM_801D8890; i++) {
            if (lbl_801D8ED0[i] != 0) {
                lbl_801D8890[i].b0 = 1;
                lbl_801D8890[i].b1 = 0;
            }
        }
        lbl_80281370 = 1;
        fn_80091454();
        fn_8008F294();
        fn_8008F24C();
    }
    fn_80169B4C(lbl_80281F1C->pHandler);
    fn_8008F194(pFE->p10);
    fn_8008F164(pFE->pC);
    fn_8008F0FC(pFE->p8);
    fn_8008F0C8(pFE->pFile);
    fn_80009E70(lbl_80281F1C->pHandler);
    fn_80009E70(lbl_80281F1C);
    lbl_80281F1C = NULL;
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

// The fade to black before a movie: draw it, 0.05 darker each frame. Once it is black, unpause,
// and in start-up (1) or the menus (3) shut the UI down (fn_8008FD60 sees lbl_80281F19); the menus
// then leave for the mode's next step.
void fn_8009069C(void) {
    f32 aColor[4];

    if (lbl_801D87C0.b0 == 0) return;
    aColor[0] = 0.0f;
    aColor[1] = 0.0f;
    aColor[2] = 0.0f;
    aColor[3] = lbl_801D87C0.fFade;
    fn_80037FB4(1, aColor);
    lbl_801D87C0.fFade += 0.05f;
    if (lbl_801D87C0.fFade >= 1.0f) {
        lbl_801D87C0.fFade = 1.0f;
        gSession.nPaused = 0;
        if (gSession.nGameType == 1) {
            lbl_80281F19 = 1;
            gSession.b12 = 1;
        } else if (gSession.nGameType == 3) {
            lbl_80281F19 = 1;
            if (gSession.a8[0] != 0) {
                fn_80099ED8();
            }
            if (Game_GetMode() == 0x17) {
                fn_80077340();
            } else if (Game_GetMode() == 0x18) {
                fn_80077344();
            } else if (Game_GetMode() == 4) {
                fn_80077348();
            }
        } else {
            gSession.b12 = 1;
        }
    }
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

int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000
TexEntry* fn_800107E4(TexBank* pBank, int nTex);  // LLTexGrp.c

void fn_800908D4(f32 x0) {
    UFontContext* pCtx;
    pCtx = fn_80012EC4();
    pCtx->fB4 = x0;
}

// The texture in pBank whose name hashes to uHash.
TexEntry* fn_80090904(TexBank* pBank, u64 uHash) {
    int nTex = fn_8001005C(pBank, uHash);

    return fn_800107E4(pBank, nTex);
}

// ---- end of sweep code ----
