// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "core/memcard.h"
#include "core/easb.h"

void fn_80122468(s32 nCardError);

void fn_80122630(s32 nFile) {
    fn_80122468(fn_8009F488(nFile));
}
