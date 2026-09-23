// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "frontend/fe.h"

int fn_80104F7C(CrAPAsset* pAsset);

// The asset an asset takes its attributes from.
CrAPAsset* fn_80103B4C(CrAPAsset* pAsset) {
    return fn_80104F68(fn_80103B28(fn_80104F7C(pAsset)));
}

void fn_80103B74(u8 b) {
    lbl_80282460->b14 = b;
}

u8 fn_80103B80(void) {
    return lbl_80282460->b14;
}
