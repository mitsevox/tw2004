# Codex: new assignment, 2026-09-27 ~04:45 UTC (supersedes the UIStudio fn_80166098 section)

Your UIStudio fn_80166098 work is merged to main as a checkpoint (97.84 -> 98.75%, ~368 -> ~305
diff hunks). PARK it: it becomes the last function standing, worked when everything else is done.
Your ledger in agents/tried/fn_80166098.md keeps your place.

New: one function per sub-agent, each to exact. Pull main first; it has 20 functions solved tonight
and the levers that closed them: read "Levers that closed functions tonight" in
agents/assign/2026-09-27-battle-plan.md, and docs/decomp-notes.md "New from round 7". Two apply
often: the kept-copy shift `x = (s32)((s64)((u64)x << 32) >> 32);` right after a call, and
per-function `#pragma optimization_level 1/2`.

| Sub-agent | Function | Unit | Now |
|---|---|---|---|
| 1 | fn_80168918 | UIStudio | 97.42%, 616 B, ~20 hunks (you know this file) |
| 2 | fn_80165670 | UISEvent | 96.95%, 1116 B, ~36 hunks |
| 3 | fn_8001144C | LLFont | 89.78%, 2112 B, ~59 hunks |

Other lanes: UISEvent fn_80165E9C is a Claude lane's (touch only fn_80165670 there). Nobody else is
in UIStudio.c or LLFont.c. UPDATE 06:20 UTC: LLFont's other open function
(FO_spLoadFontFromStream) is now a Claude lane's (b11): don't take it. A sub-agent that finishes
early goes back to UIStudio fn_80166098.

Rules as before: EA's form first; else a labelled `// fake match:` (+ `// port:` when a 64-bit value
is truncated) that leaves the logic exactly unchanged, checked by hand. Log attempts in
agents/tried/<fn>.md; SOLVED + commit when exact. UIS_CFLAGS unchanged. Push to codex/round3 after
each solve (merge main first) so wins land one at a time. If a unit's last function falls, link it
(tools/match/graduate.py, datamap.py).
