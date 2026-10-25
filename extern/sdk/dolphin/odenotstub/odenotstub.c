#include "dolphin/types.h"

// Says the debugger interface is not a stub. Weak: a library that defines its own replaces it.
__declspec(weak) BOOL Hu_IsStub(void) {
    return FALSE;
}
