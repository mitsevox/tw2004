# Role: matching / link lane

Read `agents/brief.md` first. Your prompt gives your units (in priority order) and a checkpoint.

**Goal: link units.** A unit counts toward "code linked" only when every function is exact and the
unit is linked (Matching). Work your list in order: units one function from linking first. When a
unit reaches 100%, link it before moving on (`python tools/match/graduate.py`, docs/workflow.md
"Finishing a unit"), and attach its orphan `.bss` too (`agents/roles/data.md`).

Priority when the orchestrator orders units: code-bar gain per remaining function (unit code size /
functions left), `python tools/agents/remain.py` lists what is left per unit.

## Per function

1. Grep `agents/notes/` and `docs/decomp-notes.md` for the function's address: skip what was already
   tried unless a new rule applies.
2. TW07 (EA's PS3 debug build, 2006, same engine): look the function up in
   `docs/reference-builds/tw07-ps3/pairs.tsv`, then `docs/reference-builds/tw07-ps3/cu/<File>.txt`
   for EA's parameter order and types, locals and their order, and what it inlines
   (decomp-notes "Read EA's later source first"). Engine drift is real: the code decides.
3. About 10-20 minutes per function: declaration order, statement order, types (u8/s8/int/s32,
   const), the decomp-notes rules. A near-100 function whose only difference is a branch target or a
   compare's signedness may be a real bug in our C: fix the meaning.
4. Permuter only as the brief says. Write every attempt in your scratch notes (tried / result), so
   the next lane can skip it; the orchestrator copies them into `agents/notes/`.

## Linking

- `.sdata2`/`.sdata`/`.data` constants in the wrong order: a static `<Unit>_StrippedFn` stand-in
  (decomp-notes). `.bss`/`.sbss` definitions go in reverse address order.
- A unit whose functions are all exact but whose link fails the DOL on padding between data (e.g.
  4 zero bytes before an 8-aligned start) is probably two of EA's files: report it with the address;
  the split needs evidence (TW07/TW06 file lists, data ownership) and the orchestrator's go.
