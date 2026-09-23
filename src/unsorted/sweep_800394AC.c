// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_8003954C(void);

void fn_800394AC(void);
void fn_800394AC(void) {
    UStream_UnregisterHandler('CAMS');
    UStream_UnregisterHandler('CAMV');
    UStream_UnregisterHandler('CAMA');
    fn_8003954C();
}
