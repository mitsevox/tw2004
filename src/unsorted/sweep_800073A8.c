// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_800073A8(void);

// Resets the FIFO high-water mark (fn_800124CC raises it).
void fn_800073A8(void) {
    lbl_80281B9C = 0;
}
