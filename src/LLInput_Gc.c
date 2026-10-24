// LLInput_Gc.c: the four GameCube controllers. Once a frame Input_vUpdate reads the pads into
// gControllers (buttons held and pressed this frame, sticks and triggers rescaled to 0-255); the
// rumble is started, stopped and switched on or off per pad.

#include "engine.h"
#include "pad.h"

static int  Input_ScaleStickAxis(int nValue, int nDeadZone, int nMax);
static int  Input_ScaleTrigger(int nValue, int nDeadZone, int nMax);
static void Input_ScaleAnalog(PADStatus* pStatus, Controller_StickInfo* pStickInfo);

Controllers gControllers;   // the four pads: what Input_vUpdate read, and their rumble state

// The controllers' once-only start-up: empty on this machine. main's start-up list calls it just
// after setting up the file reads.
void Input_vInitOnce(void) {
}

// Starts the pad library and clears the four pads' state: D-pad emulation off, rumble switched on
// and allowed on every pad. Returns 0. Called once, at the end of main's start-up list.
int Input_iInitModule(void) {
    int i;

    PADInit();
    Mem_set(&gControllers, 0, sizeof(Controllers));
    gControllers.uRead = 0;
    gControllers.uRead = 0;
    gControllers.uRead = 0;
    gControllers.bStickAsDpad = 0;
    for (i = 0; i < INPUT_NUM_PADS; i++) {
        gControllers.aRumble[i].bOn = 1;
        gControllers.aRumble[i].bAllowed = 1;
        gControllers.auHeld[i] = 0;
    }
    gControllers.uRead = 0;
    return 0;
}

// Stops every pad's rumble at shut-down (main's shut-down list).
void Input_vCloseOnce(void) {
    Input_vStopAllVibration();
}

// What is plugged into port joyPad, as SIProbe reports it (SI_GC_CONTROLLER, SI_GC_WAVEBIRD...).
u32 Input_iGetPadType(int joyPad) {
    return SIProbe(joyPad);
}

// Whether the port holds a controller the game takes: a standard pad or a WaveBird, or a WaveBird
// receiver whose controller is off.
u8 Input_bDoesPadExist(int joyPad) {
    s32 bResult;

    if (Input_iGetPadType(joyPad) == SI_GC_RECEIVER) {
        return 1;
    }
    bResult = 0;
    if (Input_iGetPadType(joyPad) == SI_GC_CONTROLLER || Input_iGetPadType(joyPad) == SI_GC_WAVEBIRD) {
        bResult = 1;
    }
    return bResult;
}

// Turns the D-pad emulation on (1) or off: while it is on, Input_vUpdate also sets a pad's D-pad
// bits when its main stick is pushed past a third of its travel. The UI turns it on at the start-up
// screens, in the front end and in the pause menu (UI_ReadControllers), and off everywhere else.
void Input_vEmulateDPad(u8 emulate) {
    gControllers.bStickAsDpad = emulate;
}

// Rumble on at full strength or off for pad joyPad (Input_vVibrateWave with 0xFF or 0).
void Input_vVibrateBuzz(int joyPad, int vibOn) {
    if (vibOn) {
        Input_vVibrateWave(joyPad, 0xFF);
    } else {
        Input_vVibrateWave(joyPad, 0);
    }
}

// Runs pad joyPad's rumble at vibRate (0-255): above 0x20 it starts the motor unless it is already
// running (Input_vUpdate stops it 60 frames later); 0x20 or less stops it. Does nothing while the
// pad's rumble is switched off (Input_vSetVibrationStatus) or not allowed.
void Input_vVibrateWave(int joyPad, int vibRate) {
    if (gControllers.aRumble[joyPad].bOn && gControllers.aRumble[joyPad].bAllowed) {
        if (vibRate > 0x20) {
            if (gControllers.aRumble[joyPad].nFrames == 0) {
                PADControlMotor(joyPad, PAD_MOTOR_RUMBLE);
                gControllers.aRumble[joyPad].nFrames = 1;
            }
        } else {
            PADControlMotor(joyPad, PAD_MOTOR_STOP);
            gControllers.aRumble[joyPad].nFrames = 0;
        }
    }
}

void Input_vStopVibration(int joyPad) {
    Input_vVibrateBuzz(joyPad, 0);
    Input_vVibrateWave(joyPad, 0);
}

// Switches pad joyPad's rumble on or off (the game options' vibration setting); switching it off
// also stops the motor.
void Input_vSetVibrationStatus(int joyPad, int vibOn) {
    if (vibOn) {
        gControllers.aRumble[joyPad].bOn = 1;
    } else {
        Input_vStopVibration(joyPad);
        gControllers.aRumble[joyPad].bOn = 0;
    }
}

// A stick axis (-128..127) to 0-255: 0x80 inside the dead zone, then linear up to nMax.
static int Input_ScaleStickAxis(int nValue, int nDeadZone, int nMax) {
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
static int Input_ScaleTrigger(int nValue, int nDeadZone, int nMax) {
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

// A pad's sticks and triggers from the pad library's values (pStatus) into pStickInfo, each 0-255
// with a dead zone of 15: full travel at 90 for the main stick, 70 for the C stick and 192 for the
// triggers. The Y axes are flipped so they grow downwards.
static void Input_ScaleAnalog(PADStatus* pStatus, Controller_StickInfo* pStickInfo) {
    pStickInfo->nStickX = Input_ScaleStickAxis(pStatus->stickX, 15, 90);
    pStickInfo->nStickY = Input_ScaleStickAxis(-pStatus->stickY, 15, 90);
    pStickInfo->nSubStickX = Input_ScaleStickAxis(pStatus->substickX, 15, 70);
    pStickInfo->nSubStickY = Input_ScaleStickAxis(-pStatus->substickY, 15, 70);
    pStickInfo->nTriggerL = Input_ScaleTrigger(pStatus->triggerLeft, 15, 192);
    pStickInfo->nTriggerR = Input_ScaleTrigger(pStatus->triggerRight, 15, 192);
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
    Controller_StickInfo* pStickInfo;

    PADRead(gControllers.aStatus);
    gControllers.uRead = 0;
    uReset = 0;
    for (i = 0; i < INPUT_NUM_PADS; i++) {
        uBit = PAD_CHAN0_BIT >> i;
        switch (gControllers.aStatus[i].err) {
        case PAD_ERR_NONE:
            gControllers.uConnected |= uBit;
            gControllers.uRead |= 1 << i;
            break;
        case PAD_ERR_NO_CONTROLLER:
            uReset |= uBit;
            gControllers.uRead |= 1 << i;
            break;
        case PAD_ERR_NOT_READY:
        case PAD_ERR_TRANSFER:
            break;
        }
    }
    if (uReset != 0) {
        gControllers.uConnected &= ~uReset;
        PADReset(uReset);
    }

    for (i = 0; i < INPUT_NUM_PADS; i++) {
        if (!(gControllers.uRead & (1 << i))) continue;
        if (gControllers.uConnected & (PAD_CHAN0_BIT >> i)) {
            Input_ScaleAnalog(&gControllers.aStatus[i], &gControllers.aStickInfo[i]);
            gControllers.auButtons[i] = gControllers.aStatus[i].button;
            if (gControllers.bStickAsDpad) {
                pStickInfo = &gControllers.aStickInfo[i];
                if (pStickInfo->nStickX > 0xAA) {
                    gControllers.auButtons[i] |= PAD_BUTTON_RIGHT;
                    gControllers.auButtons[i] &= ~PAD_BUTTON_LEFT;
                } else if (pStickInfo->nStickX < 0x56) {
                    gControllers.auButtons[i] |= PAD_BUTTON_LEFT;
                    gControllers.auButtons[i] &= ~PAD_BUTTON_RIGHT;
                }
                if (pStickInfo->nStickY > 0xAA) {
                    gControllers.auButtons[i] |= PAD_BUTTON_DOWN;
                    gControllers.auButtons[i] &= ~PAD_BUTTON_UP;
                } else if (pStickInfo->nStickY < 0x56) {
                    gControllers.auButtons[i] |= PAD_BUTTON_UP;
                    gControllers.auButtons[i] &= ~PAD_BUTTON_DOWN;
                }
            }
            if (gControllers.aRumble[i].nFrames > 0) {
                gControllers.aRumble[i].nFrames++;
                if (gControllers.aRumble[i].nFrames > 60) {
                    Input_vStopVibration(i);
                }
            }
            uHeld = gControllers.auButtons[i];
            uLastHeld = gControllers.auHeld[i];
            gControllers.auHeld[i] = uHeld;
            // pressed this frame in the low half, held in the high half
            gControllers.auButtons[i] = (uHeld & ~uLastHeld) | (uHeld << 16);
        } else {
            gControllers.aStickInfo[i].nStickX = 0x80;
            gControllers.aStickInfo[i].nStickY = 0x80;
            gControllers.aStickInfo[i].nSubStickX = 0x80;
            gControllers.aStickInfo[i].nSubStickY = 0x80;
            gControllers.aStickInfo[i].nTriggerL = 0;
            gControllers.aStickInfo[i].nTriggerR = 0;
            gControllers.auButtons[i] = 0;
            gControllers.auHeld[i] = 0;
        }
    }
}

// Pad joyPad's sticks and triggers, as Input_vUpdate last rescaled them.
Controller_StickInfo* Input_sGetStickInfo(int joyPad) {
    return &gControllers.aStickInfo[joyPad];
}

// A controller's buttons (PAD_BUTTON_*): the held ones in the high 16 bits, the ones pressed this
// frame in the low 16 (Input_uiMap gives a button's mask in either half).
u32 Input_ReadControlPad(int joyPad) {
    return gControllers.auButtons[joyPad];
}
