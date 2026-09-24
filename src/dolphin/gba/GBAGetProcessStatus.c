// GBAGetProcessStatus.c (Nintendo's name for the library file): how far a port's program download
// has got.
//
// port: GameCube only (the Game Boy Advance link cable).

#include <dolphin/GBAPriv.h>

// Returns GBA_BUSY while a download or another command runs on the port, and the download's
// progress in percent in *percentp: the share of bytes sent, scaled down during the first 5.5
// seconds so the bar does not jump ahead.
s32 GBAGetProcessStatus(s32 chan, u8* percentp) {
    BOOL enabled;
    s32 ret;
    GBAControl* gba;
    GBABootInfo* bootInfo;
    u8 percent;
    OSTime t;

    gba = &__GBA[chan];
    bootInfo = &__GBA[chan].bootInfo;
    enabled = OSDisableInterrupts();
    if (bootInfo->callback != NULL) {
        ret = GBA_BUSY;
        percent = (bootInfo->curOffset * 100) / bootInfo->realLength;
        if (bootInfo->begin != 0) {
            t = OSGetTime() - bootInfo->begin;
            if (OSTicksToMilliseconds(t) < 5500) {
                percent = (percent * t) / OSMillisecondsToTicks(5500ll);
            }
            if (percent >= 100) {
                percent = 100;
            }
        }
        if (percentp != NULL) {
            *percentp = percent;
        }
    } else if (gba->callback != NULL) {
        ret = GBA_BUSY;
    } else {
        ret = GBA_READY;
    }
    OSRestoreInterrupts(enabled);
    return ret;
}
