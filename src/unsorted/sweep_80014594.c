// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Skalib_Register();
void Skalib_Unregister();
void fn_80010284();
void fn_800102B4();
void fn_8001CDD4();
void fn_8001CE34();
void fn_8001CFF0();
void fn_8001D268();
void fn_8001D44C();
void fn_8001D47C();
void fn_8001FAA8();
void fn_8001FAD8();
void fn_80039454();
void fn_800394AC();
void fn_80046130();
void fn_80046174();
void fn_800676B8();
void fn_800676E8();
void fn_8008EC68();
void fn_8008ED28();
void fn_8009EA98();
void fn_8009EAF0();
void fn_800A295C();
void fn_800A298C();

void fn_80014594(void);
void fn_800145E0(void);
void fn_80014594(void) {
    fn_80010284();
    fn_80039454();
    fn_8008EC68();
    fn_80046130();
    fn_8001CDD4();
    Skalib_Register();
    fn_8001FAA8();
    fn_8001CFF0();
    fn_8001D44C();
    fn_8009EA98();
    fn_800A295C();
    fn_800676B8();
}

void fn_800145E0(void) {
    fn_800102B4();
    fn_800394AC();
    fn_8008ED28();
    fn_80046174();
    fn_8001CE34();
    Skalib_Unregister();
    fn_8001FAD8();
    fn_8001D268();
    fn_8001D47C();
    fn_8009EAF0();
    fn_800A298C();
    fn_800676E8();
}
