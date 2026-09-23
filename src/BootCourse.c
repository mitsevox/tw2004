// BootCourse.c (our name): picks a course and hole at random when the game starts (fn_80099ED8,
// called from GoEntry.c), and a 120-second timer that runs while fn_8008AC40 is false and raises a
// flag when it runs out (fn_8009A180; GoEntry.c tests the flag with fn_8009A1EC). Its .bss starts on
// its own 8-byte boundary (0x801DB8A0, shared by fn_80099ED8 and fn_8009A1F4).

#include "golfer.h"

u8 fn_8008AC40(void);

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
