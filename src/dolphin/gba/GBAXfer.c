// GBAXfer.c (Nintendo's name, from its assert): one command over a port: check that a GBA is on
// the port, send the command bytes through the serial interface, then finish the command and call
// the caller back.
//
// port: GameCube only (the Game Boy Advance link cable).

#include <dolphin/GBAPriv.h>
#include <dolphin/sipriv.h>

// The serial transfer is done: note whether it failed, let the command read its answer, then
// call the caller back in a fresh context.
static void __GBAHandler(s32 chan, u32 error, OSContext* context) {
    GBAControl* gba;
    GBATransferCallback proc;
    GBACallback callback;
    OSContext exceptionContext;

    gba = &__GBA[chan];
    if (__GBAReset != FALSE) {
        return;
    }
    if (error & 0xF) {
        gba->ret = GBA_NOT_READY;
    } else {
        gba->ret = GBA_READY;
    }
    if (gba->proc != NULL) {
        proc = gba->proc;
        gba->proc = NULL;
        proc(chan);
    }
    if (gba->callback != NULL) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback = gba->callback;
        gba->callback = NULL;
        callback(chan, gba->ret);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

// Wakes the thread waiting in __GBASync.
void __GBASyncCallback(s32 chan, s32 ret) {
    GBAControl* gba;

    gba = &__GBA[chan];
    OSWakeupThread(&gba->threadQueue);
}

// Waits for the port's command to finish and returns its result.
s32 __GBASync(s32 chan) {
    GBAControl* gba;
    s32 ret;
    BOOL enabled;

    gba = &__GBA[chan];
    enabled = OSDisableInterrupts();
    while (gba->callback != NULL) {
        OSSleepThread(&gba->threadQueue);
    }
    ret = gba->ret;
    OSRestoreInterrupts(enabled);
    return ret;
}

// The port's device type is known: only a GBA (type 0x00040000, no error bits) gets the command;
// anything else fails it at once.
void TypeAndStatusCallback(s32 chan, u32 type) {
    GBAControl* gba;
    GBATransferCallback proc;
    GBACallback callback;
    OSContext exceptionContext;
    OSContext* context;

    gba = &__GBA[chan];
    if (__GBAReset != FALSE) {
        return;
    }
// The assert carries Nintendo's source line.
#line 121
    ASSERT(!(type & SI_ERROR_BUSY));
    if ((type & 0xFF) != 0 || (type & 0xFFFF0000) != SI_GBA) {
        gba->ret = GBA_NOT_READY;
    } else {
        if (SITransfer(chan, gba->output, gba->outputBytes, gba->input, gba->inputBytes,
                       __GBAHandler, gba->delay)) {
            return;
        }
        gba->ret = GBA_BUSY;
    }
    if (gba->proc != NULL) {
        proc = gba->proc;
        gba->proc = NULL;
        proc(chan);
    }
    if (gba->callback != NULL) {
        context = OSGetCurrentContext();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback = gba->callback;
        gba->callback = NULL;
        callback(chan, gba->ret);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}

// Starts a command: w1 bytes out, w2 bytes back, then callback reads the answer.
s32 __GBATransfer(s32 chan, s32 w1, s32 w2, GBATransferCallback callback) {
    BOOL enabled;
    GBAControl* gba;

    gba = &__GBA[chan];
    enabled = OSDisableInterrupts();
    gba->proc = callback;
    gba->outputBytes = w1;
    gba->inputBytes = w2;
    SIGetTypeAsync(chan, TypeAndStatusCallback);
    OSRestoreInterrupts(enabled);
    return GBA_READY;
}
