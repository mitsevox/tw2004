// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8000AD34(void* arg0, void* arg1);
void fn_8000AD34(void* arg0, void* arg1) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;

    temp_f1 = (*(f32*)((u8*)(arg0) + 0));
    (*(f32*)((u8*)(arg0) + 0)) = (f32) (*(f32*)((u8*)(arg1) + 0));
    (*(f32*)((u8*)(arg1) + 0)) = temp_f1;
    temp_f1_2 = (*(f32*)((u8*)(arg0) + 4));
    (*(f32*)((u8*)(arg0) + 4)) = (f32) (*(f32*)((u8*)(arg1) + 4));
    (*(f32*)((u8*)(arg1) + 4)) = temp_f1_2;
    temp_f1_3 = (*(f32*)((u8*)(arg0) + 8));
    (*(f32*)((u8*)(arg0) + 8)) = (f32) (*(f32*)((u8*)(arg1) + 8));
    (*(f32*)((u8*)(arg1) + 8)) = temp_f1_3;
    temp_f1_4 = (*(f32*)((u8*)(arg0) + 0xC));
    (*(f32*)((u8*)(arg0) + 0xC)) = (f32) (*(f32*)((u8*)(arg1) + 0xC));
    (*(f32*)((u8*)(arg1) + 0xC)) = temp_f1_4;
}