# Data linking, lane D (.sdata / .sbss / .sdata2 blocks), 2026-09-27

Method: symbols in the block (symbols.txt), who references them (build/GW4E69/asm), and which
units sit between the block's neighbours in link order (splits.txt order). Defined in the owner's
.c, non-static (.sdata initialised from the DOL, .sbss in reverse address order); header externs
kept.

| block | bytes | owner | evidence | result |
|---|---|---|---|---|
| auto_07_80281110_sdata | 24 | DepthField.c | lbl_80281110 (= &lbl_801D5188, DepthField's .bss block) and the floats 0x80281114..0x80281127 used only by DepthField; between GoDynamicCam's and GoDynObj's .sdata, DepthField links between them | linked, main.dol OK (engine.h: `extern DFBuffer lbl_801D5188;` added for the initialiser) |
| auto_08_80281D90_sbss | 8 | DepthField.c | lbl_80281D90/D94 used only by DepthField; between GoDynamicCam's and UObject3D's .sbss | linked, main.dol OK |
