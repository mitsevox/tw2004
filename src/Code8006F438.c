// Code8006F438.c (our name): the hole loader. Its extent is where its .sdata starts and ends
// (lbl_802811E8 alone, padded to 8): a hole load is asked for with lbl_802811E8[1] and done here,
// the hole's files streamed in and the game's systems set up for it.

#include "game_types.h"
#include "engine.h"
#include "game.h"

void fn_8006F438(void);
void fn_800106A0(int n);                    // LLTexGrp.c
void fn_8001491C(void);                     // streammanagerhole.c
void fn_8001494C(void);                     // streammanagerhole.c
void StreamManagerHole_StreamFiles(void);   // streammanagerhole.c
void fn_8006F4F0(void);
void fn_8006F518(void);
void fn_8006F5F8(void);

// The hole loader's flags: [0] a hole has just been loaded (fn_8006F568 finishes it and clears
// it), [1] a hole load is asked for (fn_8006F4B4 / fn_8006F4E0). The size is not known (2 to 8).
u8 lbl_80281E68[2];
u8* lbl_802811E8 = lbl_80281E68;

// Load the hole if one is asked for: stream its files in (with fn_800106A0 and fn_8000B4B0 set to 1
// around it), mark it loaded (lbl_802811E8[0]) and set everything up for it.
void fn_8006F438(void) {
    fn_8006F5F8();
    if (lbl_802811E8[1] != 0) {
        fn_8006F4F0();
        fn_800106A0(1);
        fn_8000B4B0(1);
        fn_8001491C();
        StreamManagerHole_StreamFiles();
        fn_8001494C();
        fn_800106A0(0);
        fn_8000B4B0(0);
        lbl_802811E8[0] = 1;
        fn_8006F518();
        lbl_802811E8[1] = 0;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006F5FC(void);
void fn_8006F4B4(void);
void fn_8006F4E0(void);
void fn_800676AC();
void fn_8006A89C();
void fn_8006F600(void);
void AI_TargetsHook();
void fn_80019648();
void fn_80037E50();
void fn_80046288();
void fn_8006F604(void);
void fn_8006F650();
void fn_80091708();
void fn_8009B894();
void fn_800A2E68();
void fn_800B26DC();
void fn_800C72F0();
void fn_800C8134();
void GM_InitForHole();
s32 fn_80010608(s32);
s32 fn_8003467C();
s32 fn_80046664();
s32 fn_80048EF4();
s32 fn_80058E40();
s32 fn_80064EA4();
s32 fn_8006FBF8();
s32 fn_80098C28();
s32 fn_800A2B34(s32);
s32 fn_800C830C();
s32 fn_800C9CA0();
s32 fn_8011E3B4();
void fn_8006F568(void);

void fn_8006F4B4(void) {
    fn_8006F5FC();
    lbl_802811E8[1] = 1;
}

void fn_8006F4E0(void) {
    lbl_802811E8[1] = 0;
}

void fn_8006F4F0(void) {
    fn_8006F600();
    fn_800676AC();
    fn_8006A89C();
}

void fn_8006F518(void) {
    fn_8009B894();
    fn_800C8134();
    AI_TargetsHook();
    fn_80019648();
    fn_80046288();
    fn_8006F650();
    GM_InitForHole();
    fn_800A2E68();
    fn_80091708();
    fn_80037E50();
    fn_800B26DC();
    fn_800C72F0();
    fn_8006F604();
}

void fn_8006F568(void) {
    s32 var_r31;

    fn_8006FBF8();
    fn_800C830C();
    fn_80058E40();
    fn_800C9CA0();
    if ((u8) *lbl_802811E8 != 0) {
        fn_8011E3B4();
        fn_8003467C();
        fn_80046664();
        fn_80064EA4();
        fn_80098C28();
        var_r31 = 0;
        do {
            fn_800A2B34(var_r31);
            var_r31 += 1;
        } while (var_r31 < 4);
        fn_80048EF4();
        fn_80010608(1);
        fn_8000B68C(1);
        *lbl_802811E8 = 0;
    }
}

void fn_8006F5F8(void) {
}

void fn_8006F5FC(void) {
}

void fn_8006F600(void) {
}

void fn_8006F604(void) {
}

// ---- end of sweep code ----
