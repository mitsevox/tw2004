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

// GameModeDriverRTE.c, as the career calendar (GameModeDriver.c) uses it
u8   GameModeDriverRTE_GetEventByDate(u16 nDate, s32* pId, s32* pRound);
s32  fn_800F0E3C(void);
RTEvent* GameModeDriverRTE_GetCalData(s32 i);
char* GameModeDriverRTE_GetName(s32 i);
s32  fn_800F1008(s32 i);
u8   fn_800F102C(void);                 // always 0 (the calendar's cell look for today)

// GameModeDriverRTE.c, as the calendar's event details (EventInfo.c) use it
s32  fn_800F0F30(s32 i);
u16  fn_800F0FBC(s32 i);
s32  GameModeDriverRTE_GetNextEvent(void);
s32  fn_800F1154(s32 nId);
void fn_800F11A0(s32 nId, char* pDst);
s32  fn_800F120C(s32 i);
u8   GameModeDriverRTE_IsEventComplete(s32 nProfile, s32 i);

#endif
