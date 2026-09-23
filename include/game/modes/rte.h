// rte.h (our name): the real-time events of game mode 24 (GameModeDriverRTE.c): a calendar of
// dated entries, each starting one of the challenges (challenge.h) on its date.

#ifndef GAME_MODES_RTE_H
#define GAME_MODES_RTE_H

#include "game/modes/challenge.h"

// One calendar event (0x30 bytes). TW06: CalendarEntry_t, which also starts with the name and
// description and ends with the start dates.
typedef struct RTEvent {
    s32 nName;                  // 0x00  offset into the names block. TW06: nameIdx
    s32 nDesc;                  // 0x04  the same for its description. TW06: descIdx
    s32 bOff;                   // 0x08  nonzero: not playable
    s32 nChallenge;             // 0x0C  1-based, in aChallenge
    s32 nId;                    // 0x10  the event's id (its award in the save profile)
    s32 n14;                    // 0x14
    u8  unk18[4];
    u16 aDate[10];              // 0x1C  the start date per season (from 2003; 0 = not held). TW06: startDate
} RTEvent;

// The calendar's data, loaded from the 'RTE' stream objects. TW06: RTEvents
// (GameModeDriverRTE::m_RTEs), which holds pointers to the same three blocks.
typedef struct RTEData {
    RTEvent   aEvent[118];      // 0x0000  'RTEc'. TW06: pCalendar
    Challenge aChallenge[111];  // 0x1620  'RTEs'. TW06: pScenarios
    char*     pNames;           // 0x4DA0  'RTEn'. TW06: pStrTable
    u8        unk4DA4[4];
} RTEData;
extern RTEData gRTEs;

#endif
