// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_80039550(void);

void fn_80039520(void);
void fn_80039520(void) {
    UStream_UnregisterHandler('CAMV');
    fn_80039550();
}
