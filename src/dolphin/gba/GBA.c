// GBA.c (Nintendo's name, from its assert): the GBA library's core: the four ports' state, the
// setup, and the two short commands (read the GBA's status, reset it).
//
// port: GameCube only (the Game Boy Advance link cable).

#include <dolphin/GBAPriv.h>

static GBASecParam SecParams[4] ATTRIBUTE_ALIGN(32);
GBAControl __GBA[4];
BOOL __GBAReset = FALSE;

static BOOL OnReset4(BOOL final);

static OSResetFunctionInfo ResetFunctionInfo = {OnReset4, 127};

// The answer to a short command: 00 04 then the status byte.
static void ShortCommandProc(s32 chan) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->ret != GBA_READY) {
        return;
    }
    if (gba->input[0] != 0 || gba->input[1] != 4) {
        gba->ret = GBA_NOT_READY;
        return;
    }
    gba->status[0] = gba->input[2] & GBA_JSTAT_MASK;
}

void GBAInit(void) {
    GBAControl* gba;
    s32 chan;

    for (chan = 0; chan < 4; chan++) {
        gba = &__GBA[chan];
        gba->delay = OSMicrosecondsToTicks(60);
        OSInitThreadQueue(&gba->threadQueue);
        gba->param = &SecParams[chan];
// The debug library's assert carries Nintendo's source line and spelling ("(u32) gba").
#line 90
        ASSERT((u32) gba->param % 32 == 0);
    }
    OSInitAlarm();
    DSPInit();
    __GBAReset = FALSE;
    OSRegisterResetFunction(&ResetFunctionInfo);
}

s32 GBAGetStatusAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba = &__GBA[chan];

    if (gba->callback != NULL) {
        return GBA_BUSY;
    }
    gba->output[0] = 0x00;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 3, ShortCommandProc);
}

s32 GBAGetStatus(s32 chan, u8* status) {
    GBAControl* gba = &__GBA[chan];
    s32 ret = GBAGetStatusAsync(chan, status, __GBASyncCallback);

    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}

s32 GBAResetAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->callback != NULL) {
        return GBA_BUSY;
    }
    gba->output[0] = 0xFF;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 3, ShortCommandProc);
}

s32 GBAReset(s32 chan, u8* status) {
    GBAControl* gba;
    s32 ret;

    gba = &__GBA[chan];
    ret = GBAResetAsync(chan, status, __GBASyncCallback);
    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}

static BOOL OnReset4(BOOL final) {
    __GBAReset = TRUE;
    return TRUE;
}
