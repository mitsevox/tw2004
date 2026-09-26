# AnimLib_WalkPair (skalib.c, 0x80021F50)

Status: SOLVED 2026-09-25 (n-terrain): SKA_CHILD as a static inline (fn_80021F50_Read) and the
first call's two default nodes through an inline (fn_80021F50_Get) into pLeafB, then pLeafA,
before the call. A call-like argument is worked out before r3/r4 are loaded; a macro's is not.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-terrain (quicktrial aligned, base 36): args inline at the calls instead of locals
  45-91; pLeafA/pLeafB swapped 91; leaf locals void* 36, ternary void* 131-230; (AnimLib*) casts
  36; GC 2.0/2.0p1/2.6/2.7/1.3.2 all 36. SKA_CHILD as a static inline: 9 (only the first call
  left); both macros inline 62, SKA_NODE inline only 89; first call through locals: macro 31,
  default-node inline 30, SKA_NODE2(pLib, nOff) inline 33-41; reversed locals (pLeafB first) with
  the macro 9, with the default-node inline 0 (exact).

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-04-notes_w10.txt

```
- skalib AnimLib_WalkPair (36 qt): (*pfn)(), &pTree[n], `pA &&`, nested ternary (50), void* cast (230).
  Orig order at the calls: arg4, arg3, mr r12 (pfn), mr r3/r4; ours moves pA/pB before arg3.
```

### agents/notes/map-09-notes_w6.txt

```
skalib AnimLib_WalkPair 95.0: orig computes the SKA_NODE/SKA_CHILD argument before moving pA/pB into r3/r4.
  Static inline helper instead of the macro (worse), locals before the call (worse).
```

### docs/journal.md

```
  (`AnimLib_MergeOverlay`, `AnimLib_PlanBank`, `AnimLib_WalkPair`). Scratch tools in `C:\dev\scratch\tw\`:
```
