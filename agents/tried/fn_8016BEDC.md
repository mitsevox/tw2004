# fn_8016BEDC (UISScreen.c, 0x8016BEDC)

Status: SOLVED 2026-09-25 (n-uisscreen): add order, pre-decrement while, (int) digit cast.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-uisscreen: SOLVED (17 -> 0). Three changes: `nWidth - (nPrec + nDigits + (nPrec != 0))` (the add order; 17 -> 14 with the while), `while (--nDigits >= 0 && pOut < pEnd)` (subic./blt; 17 -> 14), `*pOut++ = (int)fFrac + '0'` (int, not s32: the digit and the whole part get separate stfd/lwz; 12 -> 0; (char)/(u8)/(s8)/(s16) casts also 0). Also tried: 6 other add orders, digit store split, `n` temp: 12-15. `-opt nocse` on the unit gave 98.9 before this.

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
