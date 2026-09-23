// AudTable.c (our name): an audio table of 256 entries of 0x7C bytes, allocated from the audio
// memory stack (UAudMemStack.c's fn_800B5BD8) by fn_800A7AF0 and reached through lbl_80282058.
// Its data starts on its own 8-byte boundaries (.sbss 0x80282058, .sdata2 0x80283F88); where its
// code starts before fn_800A7AF0 is not proven. Mostly not yet decompiled.

#include "game_types.h"

s32 fn_800A7C24(void) {
    return 1;
}

void fn_800A7C2C(void) {
}
