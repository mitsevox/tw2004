// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"
#include "lldyntex.h"

// Free the state and the block allocated with it.
void fn_8010A4E8(void) {
    if (lbl_80282488->p0 != NULL) {
        fn_80009E70(lbl_80282488->p0);
    }
    fn_80009E70(lbl_80282488);
}
