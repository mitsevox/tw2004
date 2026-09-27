# Codex and Gemini: quick wins (2026-09-26 night)

Pull main first (tonight's merges). Endgame rules (agents/assign/2026-09-26-endgame.md): EA's form
first, briefly; then a labelled `// fake match:` that leaves the logic exactly unchanged is fine.
Log every attempt in agents/tried/<fn>.md; push to your own branch (codex/..., gemini/...).
Tools: `python tools/match/mwccdbg.py src/<Unit>.c <fn>` (register-allocator view; read the LAST
regalloc pass in summary.txt), `python tools/match/rasim.py build/mwccdbg/<fn> replay|search`,
docs/decomp-notes.md "New from round 5/6/7" (tonight's levers: empty `if (x) {} else {}` tests to
reorder frontend temps, `(s64)` round trips to add an allocator neighbour, a second local re-assigned
inside the loop to keep a copy, locals for sub-expressions declared after a crowded variable, field
and local types int/s32/s8/u8).

## Codex

1. **SkinPart fn_800CE224** (99.36%, 660 B): the unit's LAST function: exact links SkinPart
   (12 KB). Ledger: rasim shows EA needs `i` ordered after the pSet induction temp @823 and the
   nVariant conversion temp between j and i. An inline for the i loop plus `int nVariant` after j got
   every register right but left 3 diffs (the inline's second optimisation round turns the induction
   temp's `li 0` into a copy of i). Try tonight's levers on that 3-diff form.
2. Backup: **skalib fn_80026844** (99.18%, 416 B), little attention so far. Ledger: the C form of
   `pTree + nGroupOff + 2` decides the shape; `(pTree + nGroupOff) + 2` gives every register right
   with 4 diffs.

## Gemini

1. **UISApi fn_80168CD8** (99.63%, 216 B): small and close, only a permuter run so far. What's left
   is the order of the entry copies (scheduling). Tonight's UISApi closer was a `(s64)` round trip
   (fn_80169308); the UIS "kept copy" lever is a second local re-assigned after its use.
2. Backup: **UISApi fn_801694A0** (92.97%, 128 B), tiny, little attention so far.

Don't take: Golfer.c and GoGreenGrid.c (being split / linked tonight).
