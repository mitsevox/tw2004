// pad.h (our name): the GameCube controller library (PAD, and SI for what is plugged in) calls the
// game code makes, declared once for every file. Parameter types are from how the game calls them.
// Add a call here when a file needs it, never a local declaration.

#ifndef PAD_H
#define PAD_H

#include "engine.h"

int  PADInit(void);
u32  PADRead(PadStatus* pStatus);       // all four pads; returns the pads that answered
int  PADReset(u32 uMask);               // uMask: 0x80000000 >> n for pad n
void PADControlMotor(int nChan, u32 uCommand);   // 0: stop, 1: rumble
u32  SIProbe(int nChan);                // the device type on a port

#endif
