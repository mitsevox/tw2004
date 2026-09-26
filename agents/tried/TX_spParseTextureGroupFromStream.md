# TX_spParseTextureGroupFromStream (LLTex.c, 0x8000FBB0)

Status: OPEN, 84.95% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

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
