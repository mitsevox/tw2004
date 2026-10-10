// holeloader.h (TW07: golf/HoleLoader/Headers/GoHoleLoader.h): loading a hole (GoHoleLoader.c).

#ifndef HOLELOADER_H
#define HOLELOADER_H

#include "game_types.h"

typedef struct HoleLoaderState {
    u8   bHoleLoaded;           // 0x00  a hole's files are in and set up
    u8   bHoleQueued;           // 0x01  the main loop is to load a hole
} HoleLoaderState;

extern HoleLoaderState* gpHoleLoader;      // gHoleLoader
extern HoleLoaderState  gHoleLoader;

void HoleLoader_vLoadQueuedHole(void);
void HoleLoader_QueueNextHole(void);
void HoleLoader_ResetQueueNextHole(void);
void HoleLoader_PreHoleInit(void);
void HoleLoader_PostHoleInit(void);
void HoleLoader_CloseCurrentHole(void);
void HoleLoader_OnLoadQueuedHole(void);
void HoleLoader_OnQueueNextHole(void);
void HoleLoader_OnPreHoleInit(void);
void HoleLoader_OnPostHoleInit(void);

#endif
