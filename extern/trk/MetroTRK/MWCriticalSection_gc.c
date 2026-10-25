#include "Dolphin/os.h"

// TRK's critical sections on the GameCube: entering one turns interrupts off and keeps the old
// state in the section; leaving puts it back.

void MWInitializeCriticalSection(u32* section) {
}

void MWEnterCriticalSection(u32* section) {
    *section = OSDisableInterrupts();
}

void MWExitCriticalSection(u32* section) {
    OSRestoreInterrupts(*section);
}
