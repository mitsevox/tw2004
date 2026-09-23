// hlaudvoice.c (TW06's name, by structure: golf/audio/engine/hl/hlaudvoice.c, the file after
// hlaudtrackstm.c): the sound engine's voices. Each wraps one of startUp.c's hardware voices; a
// track takes one per channel (fn_800AC4A0) and is called back when it ends. Its extent is its
// data: it is the first to use the .bss at 0x801F19B8 and the .sdata2 block 0x80283FF8-0x80284008.
// Not yet decompiled: the functions below are the sweep's.

#include "core/audtrack.h"
#include "core/startup.h"

void fn_800AC330(void);

u8 fn_800AC470(void) {
    fn_800AC330();
    return 1;
}

s32 fn_800AC494(void) {
    return 1;
}

void fn_800AC49C(void) {
}

// Pauses or resumes a voice's hardware voice.
void fn_800ACA5C(AudVoice* pVoice, u8 bPause) {
    if (pVoice != NULL && !pVoice->bB_6) {
        fn_800AFCBC(pVoice->nHwVoice, bPause);
    }
}

// Stops a voice at once: it forgets its track and gives back its ARAM buffer.
void fn_800ACB28(AudVoice* pVoice) {
    fn_800ACA94(pVoice);
    pVoice->pfnCallback = NULL;
    pVoice->pUser = NULL;
    pVoice->nIndex = 0;
    if (pVoice->bA_4) {
        fn_800ACA5C(pVoice, 1);
        if (pVoice->uAram != 0) {
            fn_800B0748(pVoice->uAram);
            pVoice->uAram = 0;
            pVoice->uPlayPos = 0;
        }
    }
}

f32 fn_800ACEC4(f32 fA, f32 fB) {
    return fA * fB;
}
