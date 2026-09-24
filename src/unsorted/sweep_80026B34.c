// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "character.h"

AnimLib* fn_80026B34(Character* pChar);
AnimLib* fn_80026B34(Character* pChar) {
    return lbl_801C6068[pChar->nSlot].pLib;
}
