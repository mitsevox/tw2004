# Role: matching / link lane

Read `agents/brief.md` first. Your prompt gives your units (in priority order) and a checkpoint.

**Goal: link units.** A unit counts toward "code linked" only when every function is exact and the
unit is linked (Matching). Work your list in order: units one function from linking first. When a
unit reaches 100%, link it before moving on (`python tools/match/graduate.py`, docs/workflow.md
"Finishing a unit"), and attach its orphan `.bss` too (`agents/roles/data.md`).

Priority when the orchestrator orders units: code-bar gain per remaining function (unit code size /
functions left), `python tools/agents/remain.py` lists what is left per unit.

## Per function

1. **Read `agents/tried/<fn>.md` first, all of it** (one file per function not yet exact: every
   attempt from every lane, the PC's sweeps and the notes, with scores). Do not repeat an attempt
   listed there unless you combine it with something new. Then `docs/decomp-notes.md` for the rules.
2. TW07 (EA's PS3 debug build, 2006, same engine): look the function up in
   `docs/reference-builds/tw07-ps3/pairs.tsv`, then `docs/reference-builds/tw07-ps3/cu/<File>.txt`
   for EA's parameter order and types, locals and their order, and what it inlines
   (decomp-notes "Read EA's later source first"). Engine drift is real: the code decides.
3. **Read the compiler's own view before guessing**: `build/mwccdbg/<fn>/summary.txt` (the
   orchestrator's batch; rerun it with `python tools/match/mwccdbg.py src/<Unit>.c <fn>`, and with
   `--src <scratch copy>` for a variant, about 10 s; runs from all lanes queue on one lock, so a
   run can wait a minute). Each variable in the allocator's priority
   order: its register, EA's register ('!' = differs), its neighbour count. Find the first '!' from
   the top and ask why the compiler ordered it there (docs/workflow.md "mwcc-debugger": more than 28
   neighbours jumps a variable to the top; `backend-00-initial-code.txt` shows every temp the C
   made). Then change the C so the compiler's view matches EA's, the EA way (a temp fewer or more,
   a dead assert from TW07, a different statement form), and confirm with a rerun. Put the key
   line of what the dump showed in the ledger (e.g. "pBurn 29 nb with nAlign+*pOffset: r31").
4. About 10-20 minutes per function: declaration order, statement order, types (u8/s8/int/s32,
   const), the decomp-notes rules. A near-100 function whose only difference is a branch target or a
   compare's signedness may be a real bug in our C: fix the meaning.
5. Permuter only as the brief says. **Before you stop, add every attempt to `agents/tried/<fn>.md`**
   under "Attempts" (date, lane, what, score before -> after) and commit it with your work, whether
   the function matched or not. When it is exact: set its Status line to SOLVED with the fix and
   the commit. (Old per-lane notes stay in `agents/notes/` as history; the ledger is the record.)

## Linking

- `.sdata2`/`.sdata`/`.data` constants in the wrong order: a static `<Unit>_StrippedFn` stand-in
  (decomp-notes). `.bss`/`.sbss` definitions go in reverse address order.
- A unit whose functions are all exact but whose link fails the DOL on padding between data (e.g.
  4 zero bytes before an 8-aligned start) is probably two of EA's files: report it with the address;
  the split needs evidence (TW07/TW06 file lists, data ownership) and the orchestrator's go.
