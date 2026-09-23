// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80035440(TerSettings* pSettings);
void fn_80035440(TerSettings* pSettings) {
    Mem_cpy(lbl_802811E0, pSettings, sizeof(TerSettings));
}
