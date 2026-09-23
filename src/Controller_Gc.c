// Controller_Gc.c (our name): the four GameCube controllers. Each frame it reads the pads into
// Controllers (buttons held and pressed this frame, sticks and triggers rescaled to 0-255), resets
// pads that stopped answering, and runs the rumble motor, which stops by itself after 60 frames.

#include "engine.h"
#include "pad.h"

void fn_8001437C(void);
void fn_80013200(int nController, int bEnable);
int  fn_80013268(int nValue, int nDeadZone, int nMax);
int  fn_800132F8(int nValue, int nDeadZone, int nMax);
void fn_8001333C(PadStatus* pStatus, PadAnalog* pAnalog);

Controllers lbl_801A36A0;

int fn_80012FA4(void) {
    int i;

    PADInit();
    fn_80005AE8(&lbl_801A36A0, 0, sizeof(Controllers));
    lbl_801A36A0.nRead = 0;
    lbl_801A36A0.nRead = 0;
    lbl_801A36A0.nRead = 0;
    lbl_801A36A0.bStickAsDpad = 0;
    for (i = 0; i < 4; i++) {
        lbl_801A36A0.aRumble[i].bOn = 1;
        lbl_801A36A0.aRumble[i].bAllowed = 1;
        lbl_801A36A0.auHeld[i] = 0;
    }
    lbl_801A36A0.nRead = 0;
    return 0;
}

void fn_80013030(void) {
    fn_8001437C();
}

u32 fn_80013050(int nChan) {
    return SIProbe(nChan);
}

// Whether the port holds a controller the game takes: a standard pad or a WaveBird, or a WaveBird
// receiver whose controller is off.
u8 fn_80013070(int nChan) {
    s32 bResult;

    if (fn_80013050(nChan) == 0x88000000) {
        return 1;
    }
    bResult = 0;
    if (fn_80013050(nChan) == 0x09000000 || fn_80013050(nChan) == 0x8B100000) {
        bResult = 1;
    }
    return bResult;
}

void fn_800130EC(u8 bOn) {
    lbl_801A36A0.bStickAsDpad = bOn;
}

void fn_800130F8(int nController, int bOn) {
    if (bOn) {
        fn_80013130(nController, 0xFF);
    } else {
        fn_80013130(nController, 0);
    }
}

void fn_80013130(int nController, int nStrength) {
    if (lbl_801A36A0.aRumble[nController].bOn && lbl_801A36A0.aRumble[nController].bAllowed) {
        if (nStrength > 0x20) {
            if (lbl_801A36A0.aRumble[nController].nFrames == 0) {
                PADControlMotor(nController, 1);
                lbl_801A36A0.aRumble[nController].nFrames = 1;
            }
        } else {
            PADControlMotor(nController, 0);
            lbl_801A36A0.aRumble[nController].nFrames = 0;
        }
    }
}

void fn_800131C4(int nController) {
    fn_800130F8(nController, 0);
    fn_80013130(nController, 0);
}

void fn_80013200(int nController, int bEnable) {
    if (bEnable) {
        lbl_801A36A0.aRumble[nController].bOn = 1;
    } else {
        fn_800131C4(nController);
        lbl_801A36A0.aRumble[nController].bOn = 0;
    }
}

// A stick axis (-128..127) to 0-255: 0x80 inside the dead zone, then linear up to nMax.
int fn_80013268(int nValue, int nDeadZone, int nMax) {
    int n;

    if (nValue > nMax) {
        n = 0xFF;
    } else if (nValue > nDeadZone) {
        n = ((nValue - nDeadZone) << 7) / (nMax - nDeadZone) + 0x80;
    } else if (nValue < -nMax) {
        n = 0;
    } else if (nValue < -nDeadZone) {
        n = ((nValue + nDeadZone) << 7) / (nMax - nDeadZone) + 0x80;
    } else {
        n = 0x80;
    }
    if (n < 0) {
        return 0;
    }
    if (n <= 0xFF) {
        return n;
    }
    return 0xFF;
}

// A trigger (0..255) to 0-255: 0 inside the dead zone, then linear up to nMax.
int fn_800132F8(int nValue, int nDeadZone, int nMax) {
    int n;

    if (nValue > nMax) {
        n = 0xFF;
    } else if (nValue > nDeadZone) {
        n = ((nValue - nDeadZone) << 8) / (nMax - nDeadZone);
    } else {
        n = 0;
    }
    if (n < 0x100) {
        return n;
    }
    return 0xFF;
}

void fn_8001333C(PadStatus* pStatus, PadAnalog* pAnalog) {
    pAnalog->nStickX = fn_80013268(pStatus->nStickX, 15, 90);
    pAnalog->nStickY = fn_80013268(-pStatus->nStickY, 15, 90);
    pAnalog->nSubStickX = fn_80013268(pStatus->nSubStickX, 15, 70);
    pAnalog->nSubStickY = fn_80013268(-pStatus->nSubStickY, 15, 70);
    pAnalog->nTriggerL = fn_800132F8(pStatus->nTriggerL, 15, 192);
    pAnalog->nTriggerR = fn_800132F8(pStatus->nTriggerR, 15, 192);
}

void fn_80013400(void) {
    int i;
    u32 uReset;
    u32 uBit;
    u32 uHeld;
    u32 uPressed;

    PADRead(lbl_801A36A0.aStatus);
    lbl_801A36A0.nRead = 0;
    uReset = 0;
    for (i = 0; i < 4; i++) {
        uBit = 0x80000000 >> i;
        switch (lbl_801A36A0.aStatus[i].nError) {
        case 0:
            lbl_801A36A0.uConnected |= uBit;
            lbl_801A36A0.nRead |= 1 << i;
            break;
        case -1:
            uReset |= uBit;
            lbl_801A36A0.nRead |= 1 << i;
            break;
        }
    }
    if (uReset != 0) {
        lbl_801A36A0.uConnected &= ~uReset;
        PADReset(uReset);
    }

    for (i = 0; i < 4; i++) {
        if (!(lbl_801A36A0.nRead & (1 << i))) continue;
        if (lbl_801A36A0.uConnected & (0x80000000 >> i)) {
            fn_8001333C(&lbl_801A36A0.aStatus[i], &lbl_801A36A0.aAnalog[i]);
            lbl_801A36A0.auButtons[i] = lbl_801A36A0.aStatus[i].uButtons;
            if (lbl_801A36A0.bStickAsDpad) {
                // D-pad bits: 1 left, 2 right, 4 down, 8 up
                if (lbl_801A36A0.aAnalog[i].nStickX > 0xAA) {
                    lbl_801A36A0.auButtons[i] |= 2;
                    lbl_801A36A0.auButtons[i] &= ~1;
                } else if (lbl_801A36A0.aAnalog[i].nStickX < 0x56) {
                    lbl_801A36A0.auButtons[i] |= 1;
                    lbl_801A36A0.auButtons[i] &= ~2;
                }
                if (lbl_801A36A0.aAnalog[i].nStickY > 0xAA) {
                    lbl_801A36A0.auButtons[i] |= 4;
                    lbl_801A36A0.auButtons[i] &= ~8;
                } else if (lbl_801A36A0.aAnalog[i].nStickY < 0x56) {
                    lbl_801A36A0.auButtons[i] |= 8;
                    lbl_801A36A0.auButtons[i] &= ~4;
                }
            }
            if (lbl_801A36A0.aRumble[i].nFrames > 0) {
                lbl_801A36A0.aRumble[i].nFrames++;
                if (lbl_801A36A0.aRumble[i].nFrames > 60) {
                    fn_800131C4(i);
                }
            }
            uHeld = lbl_801A36A0.auButtons[i];
            uPressed = uHeld & ~lbl_801A36A0.auHeld[i];
            lbl_801A36A0.auHeld[i] = uHeld;
            lbl_801A36A0.auButtons[i] = (uHeld << 16) | uPressed;
        } else {
            lbl_801A36A0.aAnalog[i].nStickX = 0x80;
            lbl_801A36A0.aAnalog[i].nStickY = 0x80;
            lbl_801A36A0.aAnalog[i].nSubStickX = 0x80;
            lbl_801A36A0.aAnalog[i].nSubStickY = 0x80;
            lbl_801A36A0.aAnalog[i].nTriggerL = 0;
            lbl_801A36A0.aAnalog[i].nTriggerR = 0;
            lbl_801A36A0.auButtons[i] = 0;
            lbl_801A36A0.auHeld[i] = 0;
        }
    }
}

u8* fn_800136C4(int nController) {
    return (u8*)&lbl_801A36A0.aAnalog[nController];
}

u32 fn_800136DC(int nController) {
    return lbl_801A36A0.auButtons[nController];
}
