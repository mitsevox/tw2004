// uiLoadFile.c (EA's name, from its asserts; also in EA's 2002 source tree): loads the menu UI's
// files. Registers the stream handlers for the UI's data, keeps the loaded file's data (and can
// park it in ARAM while a round is played), and frees what was loaded.

#include "game_types.h"
#include "engine.h"
#include "frontend/fe.h"
#include "game/frontend.h"

u32   lbl_80281EF8;             // the UI file's ARAM address while it is parked there
u32   lbl_80281EFC;             // its size, rounded up to 32 bytes
u8    lbl_80281F00;             // it is in ARAM (fn_8008F310), not in main memory
s32   lbl_80281F04;
s32   lbl_80281F08;
void* lbl_80281F0C;             // the UI file's data, copied out of its stream object (fn_8008ED80)
char* lbl_80281F10;             // the name of the UI set: "frontend", "ingame" or "startup"

void fn_8008ED28(void);
void* fn_8008F0C0(void);
void fn_8008F0C8(void* p);
UILoaded* fn_8008F0F0(void);
s32 fn_8008F15C(void);
void fn_8008F164(void* p);
s32 fn_8008F18C(void);
s32 fn_80090B10();
void fn_8008F24C(void);

// Start with nothing loaded.
void fn_8008EC30(void) {
    int i;

    lbl_801D87A8.nCount = 0;
    lbl_80281F0C = NULL;
    for (i = 0; i < UI_NUM_LOADED; i++) {
        lbl_801D87A8.ap4[i] = NULL;
    }
    lbl_80281F08 = 0;
    lbl_80281F04 = 0;
}

void fn_8008EC60(char* szSet) {
    lbl_80281F10 = szSet;
}

void fn_8008ED28(void) {
    UStream_UnregisterHandler('DATS');
    UStream_UnregisterHandler('TXFS');
    UStream_UnregisterHandler('FONS');
    UStream_UnregisterHandler('GRPS');
    UStream_UnregisterHandler('MPCS');
}

void* fn_8008F0C0(void) {
    return lbl_80281F0C;
}

void fn_8008F0C8(void* p) {
    if (p != NULL) {
        fn_80009E70(p);
    }
}

UILoaded* fn_8008F0F0(void) {
    return &lbl_801D87A8;
}

s32 fn_8008F15C(void) {
    return lbl_80281F04;
}

void fn_8008F164(void* p) {
    if (p != NULL) {
        fn_80009E70(p);
    }
}

s32 fn_8008F18C(void) {
    return lbl_80281F08;
}

void fn_8008F24C(void) {
    if (lbl_80281F1C->pC != NULL) {
        fn_80090B10();
        fn_8008F164(lbl_80281F1C->pC);
        lbl_80281F1C->pC = NULL;
    }
}

// Park the UI file's data in ARAM (the main memory copy stays allocated).
void fn_8008F310(void) {
    lbl_80281EF8 = fn_800B6564(lbl_80281EFC);
    fn_800B6844(lbl_80281F0C, lbl_80281EF8, lbl_80281EFC);
    fn_800B67EC();
    lbl_80281F00 = 1;
}

void* fn_8008F354(void) {
    return lbl_80281F0C;
}

// Bring the UI file's data back from ARAM and free the ARAM.
void fn_8008F35C(void) {
    lbl_80281F00 = 0;
    fn_800B68B4(lbl_80281F0C, lbl_80281EF8, lbl_80281EFC);
    fn_800B67EC();
    fn_800B6594(lbl_80281EF8);
}
