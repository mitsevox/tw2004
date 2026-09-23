// BootCourse.c (our name): picks a course and hole at random when the game starts (fn_80099ED8,
// called from GoEntry.c), and a 120-second timer that runs while fn_8008AC40 is false and raises a
// flag when it runs out (fn_8009A180; GoEntry.c tests the flag with fn_8009A1EC). Its .bss starts on
// its own 8-byte boundary (0x801DB8A0, shared by fn_80099ED8 and fn_8009A1F4).

#include "golfer.h"

u8 fn_8008AC40(void);

Session lbl_801DB8A0;           // the session as it was before fn_80099ED8 changed it
u8  lbl_80281F94;               // the timer ran out
f32 lbl_80281F98;               // seconds counted

void fn_8009A16C(void) {
    lbl_80281F94 = 0;
    lbl_80281F98 = 0.0f;
}

u8 fn_8009A180(void) {
    if (fn_8008AC40()) {
        return 0;
    }
    lbl_80281F98 += gSession.fFrameTime;
    if (lbl_80281F98 >= 120.0f) {
        lbl_80281F94 = 1;
        return 1;
    }
    return 0;
}

u8 fn_8009A1EC(void) {
    return lbl_80281F94;
}

// Put the session back as fn_80099ED8 found it, all but n28.
void fn_8009A1F4(void) {
    s32 n28;
    if (gSession.a8[0] != 0) {
        n28 = gSession.n28;
        Mem_cpy(&gSession, &lbl_801DB8A0, sizeof(Session));
        gSession.n28 = n28;
    }
}
