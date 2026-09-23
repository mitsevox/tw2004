// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C), three
// jump tables in .data and constants in .sdata2 (0x802851A8-0x802851B7).

#include "frontend/uistudio.h"
