# AnimLib_WalkPair (skalib.c, 0x80021F50)

Status: OPEN, 94.99% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

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
