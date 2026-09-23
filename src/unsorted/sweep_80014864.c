// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Golfer_RegisterStatsHandler();
void Golfer_UnregisterStatsHandler();
void Session_RegisterRecordsHandler();
void Session_UnregisterRecordsHandler();
void fn_800BA0A0();
void fn_800BA0E4();
void fn_800D29EC();
void fn_800D2A30();
void fn_800D33F4();
void fn_800D3424();
void fn_800EAE74();
void fn_800EAEB8();
void fn_800EDE7C();
void fn_800EDEE8();
void fn_800F0518();
void fn_800F0570();
void fn_80102310();
void fn_80102354();
void fn_8011763C();
void fn_8011766C();

void fn_80014864(void);
void fn_800148A8(void);
void fn_80014864(void) {
    Golfer_RegisterStatsHandler();
    Session_RegisterRecordsHandler();
    fn_800BA0A0();
    fn_800D29EC();
    fn_800EAE74();
    fn_800D33F4();
    fn_80102310();
    fn_8011763C();
    fn_800EDE7C();
    fn_800F0518();
}

void fn_800148A8(void) {
    Golfer_UnregisterStatsHandler();
    Session_UnregisterRecordsHandler();
    fn_800BA0E4();
    fn_800D2A30();
    fn_800EAEB8();
    fn_800D3424();
    fn_80102354();
    fn_8011766C();
    fn_800EDEE8();
    fn_800F0570();
}
