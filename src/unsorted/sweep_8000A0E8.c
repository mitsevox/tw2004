// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Vec_Copy();

void fn_8000A0E8(u8* p0, u8* p1);
void fn_8000A144(u8* p0, u8* p1);
void fn_8000A0E8(u8* p0, u8* p1) {
    Vec_Copy();
    Vec_Copy((p0 + 0x10), (p1 + 0x10));
    Vec_Copy((p0 + 0x20), (p1 + 0x20));
    Vec_Copy((p0 + 0x30), (p1 + 0x30));
}

void fn_8000A144(u8* p0, u8* p1) {
    Vec_Copy();
    Vec_Copy((p0 + 0x10), (p1 + 0x10));
    Vec_Copy((p0 + 0x20), (p1 + 0x20));
}
