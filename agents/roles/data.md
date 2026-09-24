# Role: data lane (orphan data -> its owning unit)

Read `agents/brief.md` first. The "data linked" bar counts data bytes in linked units. Data the
splitter could not assign sits in orphan units named `auto_NN_<ADDR>_<section>` in
`build/GW4E69/report.json`; matching functions never links it.

The map: `agents/findings/orphan-data/` (datamap.tsv: one row per orphan block with its owner runs;
symbols.tsv: per symbol, the resolved owner and every unit referencing it; summary.txt; the data
lane's notes). Owners were found from the units whose asm references each symbol, one owner per
symbol by link order (every data section follows .text link order in this DOL; no common BSS).

## Recipe (proven 2026-09-24 on 45 units: data linked 30.8% -> 65.0%)

1. Only for an owner unit that is already linked (or that you are linking now).
2. In the owner's .c, turn its `extern` globals in the range into definitions (non-static), in the
   order the original needs: `.bss`/`.sbss` in REVERSE address order. The type/size must equal the
   symbol's size in `config/GW4E69/symbols.txt`; one declaration per global (a header keeps its
   `extern`). Moving a declaration keeps its comment text unchanged.
3. Add `.bss start:0x... end:0x...` (or the section's range) under the unit in
   `config/GW4E69/splits.txt` by hand (graduate.py does not take `.bss`).
4. `python configure.py && ninja` -> `main.dol: OK`; `ninja build/GW4E69/report.json`; note
   complete_data before -> after; commit per owner.
5. Where the original's slot is bigger than the value (padding), shrinking the symbol's size in
   symbols.txt to the real value is fine if the DOL stays OK (matched_data rises).

## Not yet solved (ask the orchestrator)

- Initialised data whose values are raw blobs (e.g. textures): no convention yet for data files.
- A symbol used by several units with no clear definer; a type whose size disagrees with the slot
  (fix the type with proof first); alignment the C cannot express without a fake match.
