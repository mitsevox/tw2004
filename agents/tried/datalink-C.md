# Data linking, lane C (.bss blocks), 2026-09-27

Method: symbols in the block (symbols.txt), who references them (build/GW4E69/asm), and which
units sit between the block's .bss neighbours in link order (splits.txt order). Defined in the
owner's .c, non-static, reverse address order; header externs kept.

| block | bytes | owner | evidence | result |
|---|---|---|---|---|
| auto_06_801B95C8_bss | 160 | char.c | 95C8/95D8/95E8/9624 used by char.c (9624 also Skin.c, 9638 also FEgolferanim.c); between ViewController.c and mtalib.c .bss, char.c is the only unit between them | linked, main.dol OK |
| auto_06_801D4F68_bss | 24 | Code80037AB8.c | one ScreenCopy (0x14 + pad); no code uses it, only the .sdata pointer lbl_80281100 (lane D's block auto_07_80281100, right after Skin.c's .sdata); the only unit between Skin.c's and GoPostFx.c's .bss. Skin.c's tail fits the addresses equally; Code80037AB8 chosen because an initialised global of Skin.c would sit before its .sdata strings. lbl_80281100 should go to the same unit | linked, main.dol OK; engine.h: extern added |
| auto_06_801D5110_bss | 280 | DepthField.c | 5110/5198/51C8 used only by DepthField.c; 5188 (the DFBuffer) is between them and pointed at by lbl_80281110 (lane D block auto_07_80281110, DepthField's .sdata); units between GoPostFx.c and UKernel.c .bss have no other .bss | linked, main.dol OK; engine.h: extern lbl_801D5188 added |
