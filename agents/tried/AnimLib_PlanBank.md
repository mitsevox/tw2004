# AnimLib_PlanBank (skalib.c, 0x80023F7C)

Status: SOLVED 2026-09-27 (lane b8), 91.62 -> 100%. Fix: EA's array order, constant-first
sums (`0x20 + ...`), two byte-count locals (labelled fake match), own locals for loop 1 (`pWork`)
and the ppClips loop (`n`), declaration order for EA's spill slots and colours, the pEnd store
before the bank-pointer store, and `nClips += nLibClips; nClipsAll = nClips;`. Commit: 3ad8935
on agent/b8.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

### 2026-09-27, lane b8 (quicktrial aligned diffs; objdiff %)

Kept (91.62 -> 98.26%, aligned 205 -> 110):
- Local array stack order: EA has apIndex@0x44, apTree@0x6c, apRecords@0x94; declaring
  apRecords before apTree/apIndex gives it (score unchanged alone, but it is EA's layout).
- `nTotal = 0x20 + ctx.nBytes + nIndexSize + nRecSize` and `nHdr = 0x20 + nIndexSize + nRecSize`
  (constant first): EA's exact blocks (`add ctx.nBytes+nIndexSize; add nRecSize; addi 0x20`, and
  nHdr's double store `stw r0,0xec; ... stw r3,0xec`). With the constant last, the code generator
  emits nIndexSize+nRecSize+0x20 first; `sizeof(ClipBank)` last changes nothing. 205 -> 192.
- Spill slots are handed out in vreg order and locals get vregs in reverse declaration order, so
  EA's slot map (0xc8 pIndexCopy, 0xcc pTreeCopy, 0xd0 pRecordsCopy, 0xd4 B, 0xd8 nRecSize, 0xdc
  bBoth, 0xe0 nRet, 0xe4 pSlot, 0xe8 nIndexSize, 0xec nHdr, then temps &entry.nBytes,
  &entry.nTrimmed, nSlot*0x18, the MergeReleaseCb address) gives the declaration order
  nHdr, nIndexSize, pSlot, nRet, bBoth, nRecSize, ..., B, A, pRecordsCopy, pTreeCopy, pIndexCopy.
- EA keeps `ctx.nBytes + nHdr` in r20 until the restore and stores it to a spilled slot (0xd4)
  in BOTH the setup and the restore (`stw r20,0x34; stw r20,0xd4` twice): two locals.
  `ctx.nBytes += nHdr; nBytesBefore2 = ctx.nBytes; nBytesBefore = ctx.nBytes;`, restore
  `ctx.nBytes = nBytesBefore; nBytesBefore2 = ctx.nBytes;`, end `nTrimmed = nBytesBefore2 - ...`.
  Order matters: B copied before A makes A's copy the temp's last use (coalesced, cost 9, kept
  in a register) and B interferes with A (spilled). 192 -> 110.
- The frontend splits later live ranges of a reused local into its own temps (@1117 = loop 3's
  use of pOvLib, @1118-@1121 = the second name search's k/pOther/m/pRecO), numbered above the
  locals. EA's loop 3 pointer is coloured right after nOvs (r27): loop 1 gets its own local
  (`pWork`), so loop 3 keeps pOvLib's local vreg; declaration order nOvs, pOvLib, pRec, j, nLeft,
  i, pOther, k, pRecO, m, pWork (EA's colour order; pass-2 replay by rasim). 110 -> 62,
  98.26 -> 98.77%.
- Tail: `pSlot->pEnd = ...` before `lbl_801C6050[nSlot] = pBank` (EA loads pBank->pRecords
  before the stwx) 62 -> 59; the ppClips loop with its own counter `n` (EA `li r7,0`, not a CSE
  `mr` from the zero register: the loop's counter is not a split web of i) 59 -> 57. 99.42%.
Left (57 aligned, all in the second half): one register shift. EA colours nClipsAll first
(r15), then nTotal r16, copy-loop walkers r16-r19, nBytesBefore r20, &entry r21, arrays
r22-r24, copy-loop i r25. Ours removes nClipsAll in simplify round 3 at 27 neighbours; rasim
what-if: 2 permanent (coalesced) neighbours on nClipsAll, shaped like the temps of the
nIndexSize/nRecSize block (r189-r194: live set nSlot, nOvs, pOvs, pLib, nClipsAll, ctx.nBytes),
give EA's registers exactly; one extra temp alone does not; no declaration move of nClipsAll /
nBudget / nTotal does. Not found yet: nIndexSize/nRecSize written in two steps (69), as
assignment expressions inside nTotal (69), `* 16` (62), `/ 16` (67), an inline align helper
(s32: 80-98, int: 62), any int/s32/u32/short type for nIndexSize/nRecSize/nClipsAll (>= 62),
dead `(s64)` round trips on the setup's call arguments (62).
- SOLVED by `nClips += nLibClips; nClipsAll = nClips;` in place of `nClipsAll = nClips +
  nLibClips; nClips = nClipsAll;` (57 -> 0, 100%): the same values, but nClips' reload becomes
  a copy of the stored sum (load deletion), which changes nClipsAll's place in the allocator
  (EA's r15). Also exact: `nClipsAll = nClips += nLibClips` and reading nClips instead of
  nClipsAll in the check / the size lines; `nClips = nClipsAll = nClips + nLibClips` is not (57).
- Still needed: the two `goto done` shared exits (`return 0`/`return nRet` instead: 5 and 39
  aligned differences; their comments' percentages are from before this round).
Tried, worse or no change: nHdr split `nHdr = a + b; nHdr += 0x20` (214), nTotal split (208),
single local with `nBytesBefore = ctx.nBytes + nHdr; ctx.nBytes = nBytesBefore;` (153 with the
new declaration order), two locals with A the add's destination (v1/v2/v3: the frontend or the
colouring folds B into A, 180), B assigned after A (185), restore `nBytesBefore = ctx.nBytes`
after `ctx.nBytes = nBytesBefore` (folded, no change).

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-03-notes_w7.txt

```
- skalib AnimLib_PlanBank (91.6, 0x9d0 bytes): register colouring over the whole function; not tried.
```

### docs/decomp-notes.md

```
  `lbl_801C6008[nSlot].x` (`AnimLib_PlanBank`, frame 0x140 -> 0x150 like the original).
```

### docs/journal.md

```
  (`AnimLib_MergeOverlay`, `AnimLib_PlanBank`, `AnimLib_WalkPair`). Scratch tools in `C:\dev\scratch\tw\`:
```
