// pad.h (our name): the GameCube controller library (PAD, and SI for what is plugged in) calls the
// game code makes, declared once for every file. Parameter types are from how the game calls them.
// Add a call here when a file needs it, never a local declaration.

#ifndef PAD_H
#define PAD_H

#include "engine.h"

// What PADRead leaves in PADStatus.err for a pad.
#define PAD_ERR_NONE            0       // read
#define PAD_ERR_NO_CONTROLLER   -1      // nothing answered: the pad is reset (PADReset)
#define PAD_ERR_NOT_READY       -2
#define PAD_ERR_TRANSFER        -3

// PADStatus.button's bits.
#define PAD_BUTTON_LEFT         0x0001
#define PAD_BUTTON_RIGHT        0x0002
#define PAD_BUTTON_DOWN         0x0004
#define PAD_BUTTON_UP           0x0008
#define PAD_TRIGGER_Z           0x0010
#define PAD_TRIGGER_R           0x0020
#define PAD_TRIGGER_L           0x0040
#define PAD_BUTTON_A            0x0100
#define PAD_BUTTON_B            0x0200
#define PAD_BUTTON_X            0x0400
#define PAD_BUTTON_Y            0x0800
#define PAD_BUTTON_START        0x1000

// Pad n's bit for PADReset: PAD_CHAN0_BIT >> n.
#define PAD_CHAN0_BIT           0x80000000
#define PAD_CHAN1_BIT           0x40000000
#define PAD_CHAN2_BIT           0x20000000
#define PAD_CHAN3_BIT           0x10000000

// PADControlMotor's commands.
#define PAD_MOTOR_STOP          0
#define PAD_MOTOR_RUMBLE        1

// The device types SIProbe reports.
#define SI_GC_CONTROLLER        0x09000000      // a standard pad
#define SI_GC_RECEIVER          0x88000000      // a WaveBird receiver whose controller is off
#define SI_GC_WAVEBIRD          0x8B100000      // a WaveBird

int  PADInit(void);
u32  PADRead(PADStatus* pStatus);       // all four pads; returns the pads that answered
void PADClamp(PADStatus* pStatus);      // all four pads' sticks and triggers into range
int  PADReset(u32 uMask);               // uMask: PAD_CHAN0_BIT >> n for pad n
void PADControlMotor(int nChan, u32 uCommand);  // PAD_MOTOR_STOP or PAD_MOTOR_RUMBLE
u32  SIProbe(int nChan);                // the device type on a port

#endif
