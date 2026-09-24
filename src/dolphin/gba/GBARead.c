// GBARead.c (Nintendo's name for the library file): reads a word the GBA has put out for us.
//
// port: GameCube only (the Game Boy Advance link cable).

#include <dolphin/GBAPriv.h>
#include <string.h>

// The answer to a read: the four data bytes, then the status byte.
static void ReadProc(s32 chan) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->ret == GBA_READY) {
        memcpy(gba->ptr, gba->input, 4);
        gba->status[0] = gba->input[4] & GBA_JSTAT_MASK;
    }
}

s32 GBAReadAsync(s32 chan, u8* dst, u8* status, GBACallback callback) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->callback != NULL) {
        return GBA_BUSY;
    }
    gba->output[0] = 0x14;
    gba->ptr = dst;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 5, ReadProc);
}

s32 GBARead(s32 chan, u8* dst, u8* status) {
    GBAControl* gba;
    s32 ret;

    gba = &__GBA[chan];
    ret = GBAReadAsync(chan, dst, status, __GBASyncCallback);
    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}
