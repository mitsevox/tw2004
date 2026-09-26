# Codex: Golfer AI_ChooseTarget, from 99.74599% (main has your round-3 form)

Your codex/round3 version is merged on main (with r7-golfer's ledger). Pull main first.

## What mwcc-debugger says is left (pass 2 is the one that counts: pass 1 spills pCourse)

`python tools/match/mwccdbg.py src/Golfer.c AI_ChooseTarget` then read
`build/mwccdbg/AI_ChooseTarget/summary.txt` (pass 2 section) and `frontend-02-ast-final-code.txt`.
Every NAMED local is already in EA's register. Four values differ, all loop-invariants the
frontend hoists above the `for (k ...)` loop (frontend-02, just after `k = 0`):

| value | what it is | ours | EA |
|---|---|---|---|
| @174 | `&gSession.nTeeSet[nPlayer]` (gSession + nPlayer*4 + 0x58) | r21 | r14 |
| @177 | `nZone * 12` (the zone's offset into gAITargets) | r20 | r26 |
| r101 | backend value (the `(s8)nAggr` extsb, @175's feed) | r27 | r21 |
| r86  | backend value (the `(s8)nPower` extsb, @176's feed) | r26 | r27 |

So the miss is the ORDER in which the frontend creates / the allocator colours the hoisted
invariants (@170 k*?, @171 pDef base, @172/@173 pin addresses, @174 tee-set address,
@175/@176 the s8 casts, @177 zone offset), not declarations.

## Tools

- `python tools/match/rasim.py build/mwccdbg/AI_ChooseTarget replay --pass 2` replays pass 2
  exactly; `--key` / `--phantoms` what-ifs show which vreg order / extra neighbours give EA's
  colouring for those four: find the order first, then the C that creates it.
- The frontend hoists in the order the loop body's expressions are met; statement order in the
  loop body, where `gSession.nTeeSet[nPlayer]` and the `(s8)nAggr` / `(s8)nPower` uses appear,
  and an inline helper EA may have had (an inlined function's temps are numbered differently)
  are the levers to try. TW07's version: docs/reference-builds/tw07-ps3/pairs.tsv + cu/.

Rules as always: EA's form first; any fake labelled `// fake match:`; logic unchanged; log every
attempt in agents/tried/AI_ChooseTarget.md; push to codex/round3 (or a new codex/ branch).
