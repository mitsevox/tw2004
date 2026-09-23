// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's objects to draw them with a colour scale and offset, finds and runs the handlers
// objects have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus.

#include "frontend/uistudio.h"

extern u8 lbl_80280628[];
extern u8 lbl_80280638[];
u8* fn_8016C18C(void);
u8* fn_8016C198(void);

u8* fn_8016C18C(void) {
    return lbl_80280628;
}

u8* fn_8016C198(void) {
    return lbl_80280638;
}
