// Controller_Gc.c (our name): the four GameCube controllers. Each frame it reads the pads into
// Controllers (buttons held and pressed this frame, sticks and triggers rescaled to 0-255), resets
// pads that stopped answering, and runs the rumble motor, which stops by itself after 60 frames.

#include "engine.h"
#include "pad.h"

void fn_8001437C(void);
void Input_vSetVibrationStatus(int nController, int bEnable);
int  Input_ScaleStickAxis(int nValue, int nDeadZone, int nMax);
int  Input_ScaleTrigger(int nValue, int nDeadZone, int nMax);
void Input_ScaleAnalog(PadStatus* pStatus, PadAnalog* pAnalog);

Controllers lbl_801A36A0;

// Starts the pad library and clears the four pads' state: D-pad emulation off, rumble switched on
// and allowed on every pad. Returns 0. Called once, by the start-up list fn_80005520.
int Input_iInitModule(void) {
    int i;

    PADInit();
    Mem_set(&lbl_801A36A0, 0, sizeof(Controllers));
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

// Stops every pad's rumble (fn_8001437C) at shut-down; called by the shut-down list fn_80005590.
void Input_vCloseOnce(void) {
    fn_8001437C();
}

// What is plugged into port nChan, as SIProbe reports it: 0x09000000 a standard pad, 0x8B100000 a
// WaveBird, 0x88000000 a WaveBird receiver whose controller is off.
u32 Input_iGetPadType(int nChan) {
    return SIProbe(nChan);
}

// Whether the port holds a controller the game takes: a standard pad or a WaveBird, or a WaveBird
// receiver whose controller is off.
u8 Input_bDoesPadExist(int nChan) {
    s32 bResult;

    if (Input_iGetPadType(nChan) == 0x88000000) {
        return 1;
    }
    bResult = 0;
    if (Input_iGetPadType(nChan) == 0x09000000 || Input_iGetPadType(nChan) == 0x8B100000) {
        bResult = 1;
    }
    return bResult;
}

// Turns the D-pad emulation on (1) or off: while it is on, Input_vUpdate also sets a pad's D-pad
// bits when its main stick is pushed past a third of its travel. The UI turns it on in some game
// types and in the pause menu (uiProcessInterface).
void Input_vEmulateDPad(u8 bOn) {
    lbl_801A36A0.bStickAsDpad = bOn;
}

// Rumble on at full strength or off for pad nController (Input_vVibrateWave with 0xFF or 0).
void Input_vVibrateBuzz(int nController, int bOn) {
    if (bOn) {
        Input_vVibrateWave(nController, 0xFF);
    } else {
        Input_vVibrateWave(nController, 0);
    }
}

// Runs pad nController's rumble at nStrength (0-255): above 0x20 it starts the motor unless it is
// already running (Input_vUpdate stops it 60 frames later); 0x20 or less stops it. Does nothing
// while the pad's rumble is switched off (Input_vSetVibrationStatus) or not allowed.
void Input_vVibrateWave(int nController, int nStrength) {
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

void Input_vStopVibration(int nController) {
    Input_vVibrateBuzz(nController, 0);
    Input_vVibrateWave(nController, 0);
}

// Switches pad nController's rumble on or off (the game options' vibration setting, fn_8002EBA4);
// switching it off also stops the motor.
void Input_vSetVibrationStatus(int nController, int bEnable) {
    if (bEnable) {
        lbl_801A36A0.aRumble[nController].bOn = 1;
    } else {
        Input_vStopVibration(nController);
        lbl_801A36A0.aRumble[nController].bOn = 0;
    }
}

// A stick axis (-128..127) to 0-255: 0x80 inside the dead zone, then linear up to nMax.
int Input_ScaleStickAxis(int nValue, int nDeadZone, int nMax) {
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
int Input_ScaleTrigger(int nValue, int nDeadZone, int nMax) {
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

// A pad's sticks and triggers from the pad library's values (pStatus) into pAnalog, each 0-255 with
// a dead zone of 15: full travel at 90 for the main stick, 70 for the C stick and 192 for the
// triggers. The Y axes are flipped so they grow downwards.
void Input_ScaleAnalog(PadStatus* pStatus, PadAnalog* pAnalog) {
    pAnalog->nStickX = Input_ScaleStickAxis(pStatus->nStickX, 15, 90);
    pAnalog->nStickY = Input_ScaleStickAxis(-pStatus->nStickY, 15, 90);
    pAnalog->nSubStickX = Input_ScaleStickAxis(pStatus->nSubStickX, 15, 70);
    pAnalog->nSubStickY = Input_ScaleStickAxis(-pStatus->nSubStickY, 15, 70);
    pAnalog->nTriggerL = Input_ScaleTrigger(pStatus->nTriggerL, 15, 192);
    pAnalog->nTriggerR = Input_ScaleTrigger(pStatus->nTriggerR, 15, 192);
}

// Reads the four pads, once a frame: a pad that stopped answering is reset (PADReset) and reads as
// unplugged; one that did not answer this frame keeps last frame's values. For each connected pad:
// its sticks and triggers rescaled (Input_ScaleAnalog), the D-pad emulated from the main stick
// while Input_vEmulateDPad is on, a rumble past 60 frames stopped, and its buttons stored as held
// << 16 | pressed this frame. An unplugged pad reads centred sticks and no buttons.
void Input_vUpdate(void) {
    u32 uBit;
    int i;
    u32 uReset;
    u32 uHeld;
    u32 uLastHeld;
    PadAnalog* pAnalog;

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
        case -2:        // not ready yet, or a transfer error: try again next frame
        case -3:
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
            Input_ScaleAnalog(&lbl_801A36A0.aStatus[i], &lbl_801A36A0.aAnalog[i]);
            lbl_801A36A0.auButtons[i] = lbl_801A36A0.aStatus[i].uButtons;
            if (lbl_801A36A0.bStickAsDpad) {
                // D-pad bits: 1 left, 2 right, 4 down, 8 up
                pAnalog = &lbl_801A36A0.aAnalog[i];
                if (pAnalog->nStickX > 0xAA) {
                    lbl_801A36A0.auButtons[i] |= 2;
                    lbl_801A36A0.auButtons[i] &= ~1;
                } else if (pAnalog->nStickX < 0x56) {
                    lbl_801A36A0.auButtons[i] |= 1;
                    lbl_801A36A0.auButtons[i] &= ~2;
                }
                if (pAnalog->nStickY > 0xAA) {
                    lbl_801A36A0.auButtons[i] |= 4;
                    lbl_801A36A0.auButtons[i] &= ~8;
                } else if (pAnalog->nStickY < 0x56) {
                    lbl_801A36A0.auButtons[i] |= 8;
                    lbl_801A36A0.auButtons[i] &= ~4;
                }
            }
            if (lbl_801A36A0.aRumble[i].nFrames > 0) {
                lbl_801A36A0.aRumble[i].nFrames++;
                if (lbl_801A36A0.aRumble[i].nFrames > 60) {
                    Input_vStopVibration(i);
                }
            }
            uHeld = lbl_801A36A0.auButtons[i];
            uLastHeld = lbl_801A36A0.auHeld[i];
            lbl_801A36A0.auHeld[i] = uHeld;
            // pressed this frame in the low half, held in the high half
            lbl_801A36A0.auButtons[i] = (uHeld & ~uLastHeld) | (uHeld << 16);
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

// Pad nController's sticks and triggers (its PadAnalog, as bytes), as Input_vUpdate last rescaled
// them.
u8* Input_sGetStickInfo(int nController) {
    return (u8*)&lbl_801A36A0.aAnalog[nController];
}

// A controller's buttons: the held ones in the high 16 bits, the ones pressed this frame in the low
// 16 (see Controller_GetButtonMask).
u32 Input_ReadControlPad(int nController) {
    return lbl_801A36A0.auButtons[nController];
}
