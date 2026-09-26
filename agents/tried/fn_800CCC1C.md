# fn_800CCC1C (SkinPart.c, 0x800CCC1C)

Status: SOLVED 2026-09-26 (r4-charskin, d1336d3): the four copies stored in a loop `for (i = 0; i != 4; i++)` (the unrolled loop gives EA's addi r0,ptr,off; stwx; `i < 4`, `i <= 3` and `4U` unroll to the add + stw form: 12).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-charskin (12 diffs: the four copies in the fn_800CEE90 branch store with
  `addi r0,ptr,4; stwx v,idx,r0`; ours `add r3,ptr,idx; stw v,4(r3)`; the single store after
  the branch is the normal form in both; fn_800CCF90 has the same pattern, fn_800CCB08 its
  offset-0 form): cast `((SkinChoice*)pSkin->aParts[k])[nPart]`: 10 (the +4 goes onto the index
  instead); + (u32)/`& 0xFFFFFFFFu` on nPart: 10; `(&p->nOption)[nPart*2]`, `(p + nPart)->`,
  `nPart[p]`, `(&p[nPart])->`, `(u32)`, `(u16)`, `&0xFFFFFFFFu` on nPart, `(u32)`/`u` on k,
  `*(s32*)((u8*)p + nPart*8 + 4)` and `+ 4 + nPart*8`: 10-14; `s32 (*)[2]`, `s32*` with
  `[nPart*2+1]` (four spellings), a struct `{ s32 a[2]; }`: 10-14; the aParts field retyped
  `s32 (*aParts[4])[2]` in the scratch copy: 12; a k loop (int/u32, before or after i), `p = aParts[k];
  p[nPart]`, `SkinChoice** pp` walk: 12; `p = &aParts[k][nPart]; p->nOption`: 9 (idx+4 CSE'd); an
  `n = nPart*8` local with `&->nOption + n`: 12; inline helpers (set(a,n,v), set(p,k,n,v),
  get(a,n) 9, get(p,k,n) 10, field pointer 10); GC/2.0: 12 (same code). PC permuter candidate.
  Also: the pointer cast through a struct with an embedded array (`((T*)p)->a[nPart]`, a[1] or
  a[40]): 10, same code as the plain cast; `((SkinChoice*)&p->nOption)[nPart].nVariant`,
  `((SkinChoice*)((s32*)p + 1))[..]`, `((SkinChoice*)((u8*)p + 4))[..]`, `(*(SkinChoice**)&..)`:
  12. Note: the exact fn_800CC8BC has the same `addi r0,base,off; lwzx` shape from an embedded
  array field (`p16D8->apSkins[i]`), so EA's shape here looks like an array starting at +4.

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
- 2026-09-26 round 3 (r3-charskin; written by the orchestrator from the lane report, the disk was full): same forms as fn_800CCB08: best 10; EA's addi r0,ptr,4; stwx never came out.
- 2026-09-26 r4-charskin: the copies as a loop: `i < 4` 12, `i <= 3` 12, `i < 4U` 12, counting down `i = 3; i >= 0` 8 (EA's form, stores reversed), `aParts[3 - i]` 25, `i = 4; i > 0; aParts[i - 1]` 8, do/while 17, `i != 4`: 0 (exact).
