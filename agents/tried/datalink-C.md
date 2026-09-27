# Data linking, lane C (.bss blocks), 2026-09-27

Method: symbols in the block (symbols.txt), who references them (build/GW4E69/asm), and which
units sit between the block's .bss neighbours in link order (splits.txt order). Defined in the
owner's .c, non-static, reverse address order; header externs kept.

| block | bytes | owner | evidence | result |
|---|---|---|---|---|
| auto_06_801B95C8_bss | 160 | char.c | 95C8/95D8/95E8/9624 used by char.c (9624 also Skin.c, 9638 also FEgolferanim.c); between ViewController.c and mtalib.c .bss, char.c is the only unit between them | linked, main.dol OK |
