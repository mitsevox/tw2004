// gba.h (our name): the parts of Nintendo's GBA library (the Game Boy Advance link cable) that
// EA's code calls: gbacable.c (core/gbacable.h).
//
// port: GameCube only. A PC build has no link cable and drops the callers' layer.

#ifndef CORE_GBA_H
#define CORE_GBA_H

#include "game_types.h"

void GBAInit(void);
s32  GBAGetStatus(s32 nChan, u8* pStatus);
s32  GBAReset(s32 nChan, u8* pStatus);
s32  GBAGetProcessStatus(s32 nChan, u8* pPercent);  // 2 (GBA_BUSY) while a transfer runs
s32  GBARead(s32 nChan, u8* pDst, u8* pStatus);
s32  GBAWrite(s32 nChan, u8* pSrc, u8* pStatus);

#endif
