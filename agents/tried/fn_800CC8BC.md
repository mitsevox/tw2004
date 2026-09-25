# fn_800CC8BC (SkinPart.c, 0x800CC8BC)

Status: SOLVED (100.0% exact match).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25 (Gemini): Swapped `int i; int j;` declaration order and assigned `b = fn_800CEE90(); nCopy = 3; if (b) nCopy = 0;` to prevent `nCopy` from being live across `fn_800CEE90()` and avoid non-volatile `r23` allocation. Score 32 -> 0 (100.0% exact match).
