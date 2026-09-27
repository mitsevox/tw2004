# Data linking, lane C (.bss blocks), 2026-09-27

Method: symbols in the block (symbols.txt), who references them (build/GW4E69/asm), and which
units sit between the block's .bss neighbours in link order (splits.txt order). Defined in the
owner's .c, non-static, reverse address order; header externs kept.

| block | bytes | owner | evidence | result |
|---|---|---|---|---|
| auto_06_801B95C8_bss | 160 | char.c | 95C8/95D8/95E8/9624 used by char.c (9624 also Skin.c, 9638 also FEgolferanim.c); between ViewController.c and mtalib.c .bss, char.c is the only unit between them | linked, main.dol OK |
| auto_06_801D4F68_bss | 24 | Code80037AB8.c | one ScreenCopy (0x14 + pad); no code uses it, only the .sdata pointer lbl_80281100 (lane D's block auto_07_80281100, right after Skin.c's .sdata); the only unit between Skin.c's and GoPostFx.c's .bss. Skin.c's tail fits the addresses equally; Code80037AB8 chosen because an initialised global of Skin.c would sit before its .sdata strings. lbl_80281100 should go to the same unit | linked, main.dol OK; engine.h: extern added |
| auto_06_801D5110_bss | 280 | DepthField.c | 5110/5198/51C8 used only by DepthField.c; 5188 (the DFBuffer) is between them and pointed at by lbl_80281110 (lane D block auto_07_80281110, DepthField's .sdata); units between GoPostFx.c and UKernel.c .bss have no other .bss | linked, main.dol OK; engine.h: extern lbl_801D5188 added |
| auto_06_801D5888_bss | 128 | Ball.c | both f32[4][4] written by Ball.c (Physics_*), read by GoTerrainCollision.c and GameMode8.c; right after GoTerrainCollision.c's .bss, Ball.c next in link order (its .sbss likewise follows GoTerrainCollision.c's) | linked, main.dol OK |
| auto_06_801D5AB0_bss | 320 | event.c | the SitDevData block; no code names it, only the .sdata pointer lbl_802811B8 (lane D block auto_07_802811B8, between stateFunc.c's and target.c's .sdata). event.c's fn_80067608 / fn_8006765C clear and free it; Code80067710.c (next unit) reads its queue. Both lie between StateGolfer.c's and target.c's .bss; event.c chosen as the unit that sets it up. lbl_802811B8 should go to the same unit | linked, main.dol OK; sitdev.h: extern added |
| auto_06_801D87C0_bss | 88 | uiProcessInterface.c | FEScreen 87C0 (many users) and 880C (uiProcessInterface.c, GameUICommands.c, FE_MessageTable.c); right after uiLoadFile.c's .bss; of the units before fe_movies.c's .bss only uiProcessInterface.c and Code80090940.c are candidates, and Code80090940.c only reads 87C0 | linked, main.dol OK |
| auto_06_801D8890_bss | 2400 | fe_movies.c | 8890[200] used by fe_movies.c (also FE_Manager.c, uiProcessInterface.c), 8ED0 by uiProcessInterface.c; contiguous with fe_movies.c's own .bss (8818-8890); uiText.c (the only other unit before uiTransform.c's .bss) uses neither. uiProcessInterface.c's .bss lies before fe_movies.c's, so it cannot own these | linked (range extended to 801D91F0), main.dol OK |
