# TX_spParseTextureGroupFromStream (LLTex.c, 0x8000FBB0)

Status: SOLVED 2026-09-27 (lane b3), commit 1c9b8ca "LLTex.c: TX_spParseTextureGroupFromStream
exact (16/16)": EA-style forms, no fake in the function (b3 entries below).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-27 b3 (quicktrial aligned, base 111; "struct" = aligned count with registers masked):
  - The cursor: `pSection = ((TexSection*)p)++;` (a CodeWarrior cast-lvalue post-increment) for
    every 8-byte step, `pHead = ((TexBankHeader*)p)++`, keeps EA's moving cursor (`addi r25,r3,0x10;
    mr r28,r25; addi r25,r25,8`, and the last section's `mr r28,r25; addi r25,r25,8`): 111 -> 106
    (struct 34). The IRO does not fold a step written this way. Tried first, no effect:
    `((char*)p) += n` (111), `*(u32*)&p += n` (139), inline `Step(u8** pp, n)` / `Adv()` helpers
    and a `u8** pp = &p` block (all 111), every GC compiler 1.3.2-2.7 (111), per-function pragmas
    (opt_propagation off 109, opt_lifetimes off 108, optimization_level 2 115 / 1 173,
    global_optimizer off 137: the fold is the IRO's).
  - Header copy as one 8-byte struct copy through a new TexBankHeader type (TW07's
    TX_STextureGroupHeader): 106 -> 101.
  - nSize in two statements: `nSize = sizeof(TexBank) + n2 * sizeof(TexEntry) + n4 *
    sizeof(TexPalette); nSize += n2 * sizeof(TexGXObj) + n4 * sizeof(TexGXTlut);` (a sweep of all
    term orders, `+=` splits and a two-local form; 3 forms give EA's block): 101 -> 95.
  - GXInitTexObj(CI) wrap flags as `(pTex->b46 & 1) ? 0 : 1` / `(pTex->b46 & 2) ? 0 : 1`: 88
    (`!x`, `x ^ 1`, `~b46 & 1`, `>> 1` forms no better); mipmap flag `pTex->n41 > 1 ? 1 : 0`: 65,
    struct 1.
  - Texture loop `pTex = &pBank->p8[i]` (no nOffset; the strength-reduced offset is its own vreg
    and takes r31): struct 0; palette loop `&pBank->pC[i]`: 1 (EA `li r28,0; li r25,0`, ours
    `mr r25,r28`: the backend's second CSE copies the loop-split i's 0 into the offset).
  - Registers (mwccdbg + rasim search 35..46): pBank declared after i: 9 -> 1; a separate palette
    counter `k` (declared before pColors, or first) instead of reusing i: 0, exact.
  - Linking LLTex: its .sbss (0x80281C60-0x80281C88) has 4 zero bytes at 0x80281C64 and 0x80281C7C,
    before the two u8s lbl_80281C68 and lbl_80281C80 (both 8-aligned); one object packs them
    (unreferenced gap globals are dead-stripped by the link). `__attribute__((aligned(8)))` on the
    two u8s links it (main.dol: OK): labelled fake. Likely LLTex.c is several of EA's files
    (object boundaries at 0x80281C68 and 0x80281C80).

- 2026-09-26 e-render (aligned, base 111; with `#pragma opt_propagation off` 109): the mip loop
  through a `TexMip*` pointer (`for (j = 0, pMip = pTex->aMips; ..; j++, pMip++)`, set before
  the loop and stepped in the body, `pMip[j]`, `pMip++->n8`) to give back the folded j*12 start
  under the pragma: 111-113 with the pragma, 114-118 without. Under the pragma the rest differs
  too (pBank r31 vs EA r27, the nSize adds' order), so the pragma is far from closing it.

- 2026-09-26 r6-misc (quicktrial aligned, base 111): `#pragma opt_propagation off` around the
  function gives EA's whole head (`addi r25,r3,0x10; mr r28,r25; ...; addi r25,r25,8`, the
  frontend keeps every `p +=`): 109, but it breaks the texture loop (`j * 12` no longer folds to
  0) and, file-wide, 3 other LLTex functions (fn_8000EA1C, fn_8000F0EC, fn_8001005C): not EA's
  file setting. opt_lifetimes off 108, opt_common_subs off 135, others 111. Other pragma-free
  forms, all still folded (111-171): void* parameter with `(u8*)p + n` / `(char*)` /
  `(u32)`/`(int)` steps (153), `pHead = (TexBank*)(void*)p`, `p = &p[0x10]`, comma statement,
  `p -= -0x10`, an inline step helper (116), `(u32*)` cursor (113), `register` parameter,
  `p = p + 0x10`. Small-file probes (scratch r6-misc/m1-m6.c): the fold survives 600 extra
  statements, 300 block locals, a later loop or conditional redefinition of p, a local whose
  address is taken, a switch. nSize term orders under the pragma: best 107. Still open: what
  EA wrote that stops the frontend's propagation of the first two constant steps.
- 2026-09-26 r6-misc: the bank's first 8 bytes copied as one 8-byte struct (`*(Head*)pBank =
  *(Head*)pHead`, a scratch typedef of TexBank's first 8 bytes) gives EA's order (both lwz, then
  both stw): 111 -> 109. Not applied (needs a new header type; the propagation diff dominates).
  A void* cursor initialised in its declaration from the void* parameter (TW07's vpGenPtr):
  the frontend then materialises p + 0x18, not EA's p + 0x10 / + 8 steps (small-file probe m7.c).

- 2026-09-26 r5-render (aligned, base 111): `pHead = (TexBank*)(p += 0x10); p += 8;` 111,
  with `p = (u8*)(pHead + 0) + 8` 111, `p = p + 8` 111, `p = (u8*)((u32*)pHead + 2)` 111,
  `(u32*)` steps 171: the frontend folds them all.

- 2026-09-26 r5-render: mwccdbg frontend-02 shows the AST optimizer drops `p += 0x10` and
  `p += 8` entirely (pHead becomes `p + 0x10`, the first pSection `p + 0x18`, then one
  `p = p + 0x20`), so the cursor starts life as the parameter itself; EA materialises
  `addi r25,r3,0x10` + `mr r28,r25` (pHead) + `addi r25,r25,8` before the call. The fix must stop
  the frontend's constant-increment folding for the first two steps (later steps, after a
  variable `+= nSize`, already match). No new variant tried this round.

- 2026-09-26, r2-ll (quicktrial aligned, base 111). TW07's LLTex.c locals for this function
  (docs/reference-builds/tw07-ps3/cu/LLTex.c.txt): `void* vpGenPtr` (the cursor), vpTexturesData,
  spTexture, iTexture, spTextureGroup, spTextureGroupHeader, spChunkDesc, spTextureGroupFileDesc,
  iTextureDescsSize, iTextureGroupSize. A separate cursor local (param renamed pData): u8* 171,
  void* with `(u8*)p + n` steps 153 (CW still folds every step into offsets). nSize term orders
  (sizeof(TexBank) third / last, grouped, split over `+=` statements, `x + nSize` form, `(int)`
  casts on the sizeofs): 111-118; the target adds 0x30 after the first two products.

- 2026-09-25, n-ll (quicktrial aligned, base 111). Reading: the target keeps the stream cursor as a
  moving register (`addi r25,r3,0x10; mr r28,r25` for the header; `mr r29,r25; addi r25,r25,8` for
  each section) while ours folds every field into offsets from the parameter. `pHead = p + 0x10;
  p += 0x18`, `p = (u8*)pHead + 8`, `&p[0x10]`, header copy through `((u32*)p)[-2]`: all 111 (CW
  canonicalises); through `(u32)p + n` integer arithmetic: 171. No change kept.

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w6.txt

```
- LLTex TX_spParseTextureGroupFromStream: parameter copy / p = p + n / pHead-based advance: no (111+).
```
