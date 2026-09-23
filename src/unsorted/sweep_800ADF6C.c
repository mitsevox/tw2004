// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800ADF6C(void* arg0, void* arg1);
void fn_800ADF6C(void* arg0, void* arg1) {
    void* temp_r0;
    void** temp_r0_2;

    if (arg1 == (void* ) (*(void**)((u8*)(arg0) + 4))) {
        temp_r0 = (*(void**)((u8*)(arg1) + 0));
        if (temp_r0 != NULL) {
            (*(void**)((u8*)(arg0) + 4)) = temp_r0;
            (*(void***)((u8*)((*(void**)((u8*)(arg1) + 0))) + 4)) = NULL;
        } else {
            (*(void**)((u8*)(arg0) + 4)) = NULL;
            (*(void***)((u8*)(arg0) + 0)) = NULL;
        }
    } else if (arg1 == (void** ) (*(void***)((u8*)(arg0) + 0))) {
        temp_r0_2 = (*(void***)((u8*)(arg1) + 4));
        if (temp_r0_2 != NULL) {
            (*(void***)((u8*)(arg0) + 0)) = temp_r0_2;
            *(*(void***)((u8*)(arg1) + 4)) = NULL;
        } else {
            (*(void**)((u8*)(arg0) + 4)) = NULL;
            (*(void***)((u8*)(arg0) + 0)) = NULL;
        }
    } else {
        (*(void***)((u8*)((*(void**)((u8*)(arg1) + 0))) + 4)) = (void** ) (*(void***)((u8*)(arg1) + 4));
        *(*(void***)((u8*)(arg1) + 4)) = (*(void**)((u8*)(arg1) + 0));
    }
    (*(u16*)((u8*)(arg0) + 0xA)) = (u16) ((*(u16*)((u8*)(arg0) + 0xA)) - 1);
}