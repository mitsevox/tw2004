#include "game_types.h"

void fn_801020BC(void);
extern u8 gPlayers[];
extern u8 lbl_802124B8[];
void fn_800584DC();
void fn_801020C0(void);
void fn_8010211C();
void fn_8010217C();
void fn_80102134(void);
void fn_80102158(void);
extern s32 lbl_80282438;
s32 fn_801021FC(void);
extern s32 lbl_8028244C;
void fn_80102308(s32 v);
void UStream_UnregisterHandler();
void fn_80102354(void);
void fn_8000E790();
void fn_8010237C(s32 p0);
extern u8 lbl_80282434;
u8 fn_801025F4(void);
s32 fn_80102A44(s32 p0);
void fn_80102AC4(void);

void fn_801020BC(void) {
}

void fn_801020C0(void) {
    fn_800584DC(*(s32*)gPlayers);
}

void fn_80102134(void) {
    fn_801021FC();
    fn_8010211C();
}

void fn_80102158(void) {
    fn_801021FC();
    fn_8010217C();
}

s32 fn_801021FC(void) {
    return lbl_80282438;
}

void fn_80102308(s32 v) {
    lbl_8028244C = v;
}

void fn_80102354(void) {
    UStream_UnregisterHandler(1413696800);
}

void fn_8010237C(s32 p0) {
    fn_8000E790(p0, 1700, lbl_802124B8);
}

u8 fn_801025F4(void) {
    return lbl_80282434;
}

s32 fn_80102A44(s32 p0) {
    return *(s32*)(lbl_802124B8 + (p0 * 68));
}

void fn_80102AC4(void) {
}
