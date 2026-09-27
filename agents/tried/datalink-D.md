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
| auto_07_80280DF0_sdata | 8 | GoRenderCtx_Gc.c | lbl_80280DF0 (= &lbl_80281C90) is read by many units, but of the units linked between UFont's and streammanagerhole's .sdata only GoRenderCtx_Gc uses it (fn_80013D5C sets the slot) | linked, main.dol OK |
| auto_08_80281C88_sbss | 24 | GoRenderCtx_Gc.c 0x80281C90-0x80281CA0; 0x80281C88 is LLFont.c's (skipped) | lbl_80281C88 is LLFont.c's `static u8` (LLFont.c, not linked: left). lbl_80281C90 is only referenced by lbl_80280DF0's initialiser, lbl_80281C98 only by GoRenderCtx_Gc; both between LLFont's and ViewController's .sbss. lbl_80281C98 sits at +8, so lbl_80281C90 is an 8-byte object (void* [2], only [0] used): a 4-byte one packs C98 at +4 (unreferenced padding objects are stripped by the linker) | 16 B linked, main.dol OK; 8 B (LLFont) left |
| auto_08_80281CA8_sbss | 8 | char.c | lbl_80281CA8 (count, also read by Skin.c) and lbl_80281CAC used by char.c; char.c is the only unit between ViewController's and mtalib's .sbss | linked, main.dol OK (defined above char.c's .sdata, away from lane C's .bss lines) |
| auto_08_80281CC0_sbss | 24 | ska_shared.c | lbl_80281CC0 used only by ska_shared; the staging-buffer pointers CC4..CD0 are used by ska_shared and skalib (set in Skalib_Init). Contiguity: every unit's .sbss starts 8-aligned, skalib's starts at 0x80281CD8, so CC4..CD7 cannot open skalib's section; CC0..CD8 is one object (ska_shared, the only unit between mtalib and skalib) | linked, main.dol OK |
| auto_08_80281DF0_sbss | 16 | user.c | user.c (fn_800563C4) clears lbl_80281DF0 and allocates gpSaveData and lbl_80281DF4, fn_80056454 frees them; the rest only read them; user.c links between Wind's and StateGolfer's .sbss | linked, main.dol OK |
| auto_08_80281F80_sbss | 8 | GoShaderObject_Glows_Gc.c | lbl_80281F80 (the glow queue in use) is cleared and read by GoShaderObject_Glows_Gc, set by Code8009A928, used by Code8009AA28/Code800BA940; the only unit between GoCamTuningVars's and UFstPart's .sbss besides a text-only sweep is Glows_Gc | linked, main.dol OK |
| auto_08_80281FB0_sbss | 112 | MC_Gc.c 0x80281FB0-0x80282010 (96 B), PsMgr.c 0x80282010-0x80282018 (8 B); 0x80282018 left | MC_Gc alone uses FB0..FD0 and 80282000/80282008, so the MC.c-only (FF0..FF8) and FE_MessageTable-only (FFC) globals between them are MC_Gc's too (one object's .sbss is contiguous; MC.c links right after MC_Gc). lbl_80282010 is PsMgr's (PsMgr's .sdata lbl_802813F8 = &lbl_80282010). lbl_80281FD0 is 8 bytes in the DOL (next object at +8; a u8[2] packs the next pointer at +4): defined u8[8] with a note, memcard.h extern changed from [MC_NUM_PORTS] to [8] | 104 B linked, main.dol OK |

**0x80282018 (lbl_80282018, 8 B) left:** a u32 counter, incremented by hlaudemitter.c and read by
hlaudtrackseq.c. It sits between PsMgr's .sbss (ends 0x80282018) and GameAudio's (starts
0x80282020), so its owner is PsBallFx.c or GameAudio.c; neither references it. GameAudio.c includes
core/audtrack.h (its extern) and drives the emitters, so GameAudio is likelier, but GameAudio.c's
header comment states its .sbss is 0x80282020-0x80282058 ("proven by its data"), and matching lanes
may not change comments. Needs the orchestrator's call.
| auto_08_80282230_sbss | 8 | AnimStream.c | lbl_80282230 (the stream state, allocated by fn_800C937C) used only by AnimStream; between GoBreakLine's and SkinPart's .sbss (AnimStream, TerrainGround in between) | linked, main.dol OK |
| auto_08_80282510_sbss | 8 | GoGrass.c | lbl_80282510 (texture buffer) and lbl_80282514 used only by GoGrass; GoGrass is the only unit between fe_stats's and gbacable's .sbss that uses them (EventInfo, llrtclock, GoShaderObject_Grass_Gc, GameMode4Menu, LadderMap, TibExt in between) | linked, main.dol OK |
