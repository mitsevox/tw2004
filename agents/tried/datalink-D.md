# Data linking, lane D (.sdata / .sbss / .sdata2 blocks), 2026-09-27

Method: symbols in the block (symbols.txt), who references them (build/GW4E69/asm), and which
units sit between the block's neighbours in link order (splits.txt order). Defined in the owner's
.c, non-static (.sdata initialised from the DOL, .sbss in reverse address order); header externs
kept.

| block | bytes | owner | evidence | result |
|---|---|---|---|---|
| auto_07_80281110_sdata | 24 | DepthField.c | lbl_80281110 (= &lbl_801D5188, DepthField's .bss block) and the floats 0x80281114..0x80281127 used only by DepthField; between GoDynamicCam's and GoDynObj's .sdata, DepthField links between them | linked, main.dol OK (engine.h: `extern DFBuffer lbl_801D5188;` added for the initialiser) |
| auto_08_80281D90_sbss | 8 | DepthField.c | lbl_80281D90/D94 used only by DepthField; between GoDynamicCam's and UObject3D's .sbss | linked, main.dol OK |
| auto_07_802814D0_sdata | 24 | DiscError.c 0x802814D0-0x802814D8 only; 0x802814D8-0x802814E8 left (see below) | lbl_802814D0/D1/D2 (u8 = 1) used only by DiscError | 8 B linked, main.dol OK |
| auto_08_80282188_sbss | 32 | DiscError.c 0x80282188-0x802821A0 only; 0x802821A0-0x802821A8 left | lbl_80282188..lbl_80282198 used only by DiscError | 24 B linked, main.dol OK |

**DiscError.c looks like two compile units (finding, left for the orchestrator).** With every
DiscError global defined in DiscError.c the DOL fails: the original has a 4-byte hole in each
section right before the second half. .sdata: flags D0/D1/D2, hole 0x802814D3-D7, then
lbl_802814D8 (= lbl_80190F6C, the message table) and "%s %d" at 0x802814DC. .sbss: 88..98, hole
0x8028219C, then lbl_802821A0/A4. Our object packs them (pointer at D4, A0 at 9C). A padding
object is no fix (the linker strips an unreferenced one; `= 0` goes to .sbss anyway). Every unit's
.sdata/.sbss in this link starts 8-aligned (317 of 320 ranges), and 0x802814D8 / 0x802821A0 are
exactly the next 8-aligned starts, so the second halves are another object's sections. Those
globals (lbl_802814D8, lbl_802814DC, lbl_802821A0, lbl_802821A4) are used by fn_800B7210 (the
message screen, whose switch is jumptable_80190FA8) and fn_800B7490 (the drive-status poll), which
suggests DiscError.c's .text splits at 0x800B7210 into a second file (fn_800B7210..fn_800B7694,
plus the message strings/table 0x80190DB0..0x80190FA8 and the jump table in .data). Not done:
splitting a linked unit's .text is beyond this pass; the 16 + 8 bytes stay in auto units.
