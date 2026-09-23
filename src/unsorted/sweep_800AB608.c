// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8018EB90[];
extern u8 lbl_801F18B8[];
void fn_800AE084();
void fn_800B59BC();
void fn_800B59EC();

void RemoveFromAudStreamQueue(void);
void RemoveFromAudStreamQueue(void) {
    fn_800B59BC(lbl_8018EB90);
    fn_800AE084(lbl_801F18B8);
    *(u8*)(lbl_801F18B8 + 0x18) = 0;
    fn_800B59EC(lbl_8018EB90, lbl_801F18B8);
}
