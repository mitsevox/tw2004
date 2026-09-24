// LoadData.c (our name): the stream handlers for loading data. 'txf2' objects become texture
// banks, up to 200, found by a texture's name (the front end's logos use them); the 'load'
// object is copied into a fixed buffer, and fn_8000BAE0 loads the loading-screen file of the
// current game mode or course. Also the 64-bit name hash of textures and Game_GetMode.

#include "game.h"
#include "ustream.h"

// The globals, in reverse address order (CodeWarrior lays them out last-defined-first).
UStreamObject* lbl_80281C0C;            // a copy of the 'txf2' object with id 10000
u8* lbl_80281C08;                       // } and of its data (TXF2_KEPT_SIZE bytes)
u8* lbl_80281C04;
int lbl_80281C00;                       // how many 'txf2' banks came in
TexBank* lbl_801A26DC[NUM_TXF2_BANKS];
u8 lbl_801A2614[NUM_TXF2_BANKS];        // the bank has its own data to free (fn_8000FFAC)
LoadObjInfo lbl_801A25F0;

void fn_8000BA3C(void);
void fn_8000BA6C(void);
void fn_8000BA94(UStreamObject* pObject);
void fn_8000BCA0(UStreamObject* pObject);
void fn_8000BEC8(void);
int  fn_8001005C(TexBank* pBank, u64 uHash);   // LLTex.c: the texture's index, or 0x80000000

void fn_8000B884(void) {
    int i;

    lbl_80281C00 = 0;
    for (i = 0; i < NUM_TXF2_BANKS; i++) {
        lbl_801A26DC[i] = NULL;
        lbl_801A2614[i] = 0;
    }
}

void fn_8000B8F4(void) {
    int i;

    for (i = 0; i < lbl_80281C00; i++) {
        if (lbl_801A2614[i]) {
            fn_8000FFAC(lbl_801A26DC[i]);
        }
        fn_80009E70(lbl_801A26DC[i]);
        lbl_801A26DC[i] = NULL;
    }
}

void fn_8000B984(void) {
    lbl_80281C04 = fn_800951A0(147700, 16, 1);
    lbl_80281C0C = fn_800951A0(sizeof(UStreamObject), 16, 1);
    lbl_80281C08 = fn_800951A0(TXF2_KEPT_SIZE, 16, 1);
}

void fn_8000B9E4(void) {
    UStream_RegisterHandler(TAG('t', 'x', 'f', '2'), fn_8000BCA0);
}

void fn_8000BA14(void) {
    UStream_UnregisterHandler(TAG('t', 'x', 'f', '2'));
}

void fn_8000BA3C(void) {
    UStream_RegisterHandler(TAG('l', 'o', 'a', 'd'), fn_8000BA94);
}

void fn_8000BA6C(void) {
    UStream_UnregisterHandler(TAG('l', 'o', 'a', 'd'));
}

void fn_8000BA94(UStreamObject* pObject) {
    lbl_801A25F0.uSize = pObject->uSize;
    Mem_cpy(lbl_80281C04, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}

void fn_8000BAE0(void) {
    int nFile;

    if (fn_800ED6F0()) {
        nFile = 2;
    } else if (Game_GetMode() == 11 || Game_GetMode() == 5 || fn_800EC550()) {
        nFile = 0;
    } else if (Game_GetMode() == 13 || Game_GetMode() == 14 || Game_GetMode() == 16 ||
               Game_GetMode() == 17) {
        nFile = 0;
    } else if (gpGame->b137 || gpGame->b136 || gpGame->b139) {
        nFile = 0;
    } else if (gpGame->b138) {
        nFile = 21;
    } else {
        switch (Game_GetCourse()) {
        case 0:  nFile = 1;  break;
        case 1:  nFile = 3;  break;
        case 2:  nFile = 4;  break;
        case 3:  nFile = 5;  break;
        case 6:  nFile = 6;  break;
        case 10: nFile = 7;  break;
        case 11: nFile = 8;  break;
        case 12: nFile = 9;  break;
        case 13: nFile = 10; break;
        case 15: nFile = 11; break;
        case 14: nFile = 12; break;
        case 9:  nFile = 13; break;
        case 4:  nFile = 20; break;
        case 5:  nFile = 22; break;
        case 8:  nFile = 14; break;
        case 16: nFile = 16; break;
        case 17: nFile = 19; break;
        case 18: nFile = 15; break;
        case 19: nFile = 18; break;
        case 20: nFile = 17; break;
        default: nFile = 0;  break;
        }
    }
    fn_8000BA3C();
    fn_80014544(nFile);
    fn_800147A4();
    fn_8000BEC8();
    fn_8000BA6C();
}

// The 'txf2' handler. Id 10000 (modulo 100000) is kept as a copy; the others become banks.
void fn_8000BCA0(UStreamObject* pObject) {
    int n = (int)pObject->uId % 100000;

    if (n == 10000) {
        Mem_cpy(lbl_80281C0C, pObject, sizeof(UStreamObject));
        Mem_cpy(lbl_80281C08, pObject->pData, TXF2_KEPT_SIZE);
        lbl_80281C0C->pData = lbl_80281C08;
        fn_80009E70(pObject);
        return;
    }
    lbl_801A26DC[n] = fn_8000FB88(pObject, NULL, 0);
    lbl_801A2614[n] = 1;
    fn_80009E70(pObject);
    lbl_80281C00++;
}

u8 fn_8000BD80(char* pName, u8** ppPixels) {
    TexBank* pBank;
    TexEntry* pTex;

    if (pName == NULL || ppPixels == NULL) {
        return 0;
    }
    fn_8000BDF8(pName, &pBank, &pTex);
    if (pTex == NULL) {
        return 0;
    }
    *ppPixels = pBank->p18 + pTex->uPixels;
    return 1;
}

int fn_8000BDF8(char* pName, TexBank** ppBank, TexEntry** ppTex) {
    int i;
    u64 uHash = fn_8000BEE4(pName);
    u64 nTex;
    TexBank* pBank;

    for (i = 0; i < NUM_TXF2_BANKS; i++) {
        if (lbl_801A2614[i]) {
            pBank = lbl_801A26DC[i];
            if (pBank != NULL) {
                // fake match: the original compares the index as a 64-bit value, zero-extended
                nTex = (u32)fn_8001005C(pBank, uHash);
                if (nTex != 0x80000000) {
                    *ppBank = pBank;
                    *ppTex = &pBank->p8[nTex];
                    return i;
                }
            }
        }
    }
    *ppBank = NULL;
    *ppTex = NULL;
    return -1;
}

void fn_8000BEC8(void) {
    lbl_80280DF8->aParams[2].nNumFiles = 0;
}

int Game_GetMode(void) {
    return gpGame->nMode;
}

u64 fn_8000BEE4(char* pName) {
    u64 uHash = 0;

    fn_800CB700(&uHash, pName);
    return uHash;
}
