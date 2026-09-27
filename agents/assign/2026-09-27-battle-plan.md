# Battle plan (owner, 2026-09-27): one lane, one function, to the end

47 functions left. Codex takes the biggest one; 8 Claude lanes each take ONE function from the queue
below (easiest first, ranked by diff hunks from `sbs2.py`, not match %). When a lane's function is
exact, the orchestrator gives that lane the next one in the queue.

Rules on top of agents/assign/2026-09-26-endgame.md (all its hard rules stand):

- **One function.** A lane does not touch any other function and does not move on until its function
  is exact (or the orchestrator stops it). If it stalls, the orchestrator replaces it with a fresh
  lane on the same function that starts from the ledger; so log everything in agents/tried/<fn>.md.
- **No permuter.** It is off for the endgame (noise, no wins today). Use the compiler's own view:
  `mwccdbg.py` (register allocator, frontend temps), `rasim.py` (allocator replay / declaration
  search), `quicktrial.py` (sub-second scoring of hand-written variants, large hand-built sweeps),
  `sbs2.py`, TW07 DWARF, docs/decomp-notes.md "New from round 5/6/7".
- **Fakes close functions**: EA's form first (briefly), then a labelled `// fake match:` that leaves
  the logic exactly unchanged. Check the logic by hand.
- **At most two lanes per source file.** Shared UIS flags (`UIS_CFLAGS` in configure.py) and shared
  headers: don't change what others use; say so in the report if the codegen proves you must.
- **Link on the last function.** If yours is the unit's last non-exact function, link the unit in
  the same branch (`graduate.py`, ranges from `datamap.py`). The build must end `main.dol: OK`.

## Codex: UIStudio fn_80166098 (9,644 B, 97.84%, ~368 diff hunks)

The biggest function left, the UI studio's event/command dispatcher. Your three sub-agents in
parallel fit it. Read agents/tried/fn_80166098.md first. UIS units share one flag set
(`UIS_CFLAGS`, agents/findings/2026-09-26-uis-library-flags.md); keep it. Push to codex/round3
(merge main first). If you close it, UIStudio's other open function (fn_80168918) is a lane's.

## Queue (hunks, match %, size)

| # | Function | Unit | Hunks | Match | Size |
|---|---|---|---|---|---|
| 1 | fn_8001F110 | char | 2 | 99.11 | 540 |
| 2 | fn_8008F820 | uiProcessInterface | 2 | 99.94 | 1344 |
| 3 | Stm_Tick | hlaudtrackstm | 2 | 99.10 | 892 |
| 4 | fn_80169D90 | UISApi | 2 | 66.85 | 52 |
| 5 | fn_80168CD8 | UISApi | 2 | 99.63 | 216 |
| 6 | fn_8016C6C4 | UISScreen | 2 | 90.48 | 84 |
| 7 | fn_8016B188 | UISScreen | 3 | 99.86 | 844 |
| 8 | fn_80165ACC | UISEvent | 6 | 99.08 | 196 |
| 9 | AnimLib_WasLastPlayed | skalib | 6 | 89.07 | 172 |
| 10 | fn_800949D0 | GoShaderObject_Particle_Gc | 6 | 95.69 | 436 |
| 11 | fn_801694A0 | UISApi | 5 | 92.97 | 128 |
| 12 | fn_80168F5C | UISApi | 5 | 88.15 | 108 |
| 13 | fn_8016C614 | UISScreen | 5 | 90.21 | 96 |
| 14 | fn_80165E9C | UISEvent | 7 | 95.59 | 432 |
| 15 | fn_80169858 | UISApi | 7 | 99.34 | 692 |
| 16 | fn_800C7A9C | CamSpline | 8 | 98.48 | 632 |
| 17 | PictInt_Decode | LLPictInt | 9 | 93.26 | 440 |
| 18 | DF_vDrawBufferToScreen | DepthField | 11 | 97.88 | 1040 |
| 19 | fn_800B769C | rcmp_mad_codec | 11 | 98.54 | 1764 |
| 20 | fn_8016A510 | UISScreen | 12 | 94.45 | 800 |
| 21 | fn_8009F8C8 | MC | 16 | 96.04 | 520 |
| 22 | fn_801264B8 | GameMode22 | 16 | 85.90 | 392 |
| 23 | fn_8016AD54 | UISScreen | 17 | 93.04 | 408 |
| 24 | BFX_vRender | goballfx | 20 | 90.13 | 564 |
| 25 | fn_80168918 | UIStudio | 20 | 97.42 | 616 |
| 26 | fn_8016AEEC | UISScreen | 20 | 97.64 | 432 |
| 27 | fn_8016ABBC | UISScreen | 20 | 96.23 | 408 |
| 28 | fn_80169C0C | UISApi | 21 | 69.36 | 388 |
| 29 | fn_8003F2E0 | gocamscripts | 22 | 98.31 | 568 |
| 30 | fn_80019798 | char | 25 | 99.46 | 1156 |
| 31 | fn_80169DC4 | UISApi | 27 | 96.74 | 620 |
| 32 | fn_8010FC3C | LogoTexture | 32 | 78.25 | 800 |
| 33 | fn_8016A2D4 | UISScreen | 33 | 96.59 | 572 |
| 34 | fn_80165670 | UISEvent | 36 | 96.95 | 1116 |
| 35 | fn_80169590 | UISApi | 37 | 98.31 | 712 |
| 36 | fn_8016C270 | UISScreen | 38 | 86.29 | 852 |
| 37 | fn_8016A030 | UISApi | 42 | 94.83 | 676 |
| 38 | fn_80094534 | GoShaderObject_Particle_Gc | 43 | 98.41 | 1180 |
| 39 | TX_spParseTextureGroupFromStream | LLTex | 53 | 84.95 | 1020 |
| 40 | fn_8001144C | LLFont | 59 | 89.78 | 2112 |
| 41 | fn_8016B844 | UISScreen | 61 | 91.61 | 1688 |
| 42 | fn_80102AC8 | uiArc | 67 | 95.90 | 3004 |
| 43 | FO_spLoadFontFromStream | LLFont | 70 | 87.02 | 2112 |
| 44 | SD_vShaderObject_Grass_Static_Init | GoShaderObject_Grass_Gc | 80 | 82.05 | 1852 |
| 45 | AnimLib_MergeOverlay | skalib | 98 | 87.73 | 1824 |
| 46 | AnimLib_PlanBank | skalib | 112 | 91.62 | 2536 |

Order is by hunks, adjusted so no more than two lanes share a file at once.
