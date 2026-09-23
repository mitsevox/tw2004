#include "game_types.h"

void fn_800FA570();
void fn_800F9824(void);
extern u8 lbl_802823C9;
void fn_800F9AB0();
void fn_800F9844(void);

void fn_800F9824(void) {
    fn_800FA570();
}

void fn_800F9844(void) {
    lbl_802823C9 = 1;
    fn_800F9AB0();
}
