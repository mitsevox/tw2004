# Codebase health recon (2026-09-27, post-100%)

Sampled review, the way a critic on a decomp Discord would read it. Sample: Ball.c, Swing.c,
GoTerrain.c, LLFileIO_Gc.c, GameModeReplay.c (a style.md model file), GameMode22.c, MC.c,
CamSpline.c, LLFont.c, uiArc.c, UISStack.c, UISApi.c, GoDynamicCam.c, GoRenderCtx_Gc.c,
FE_MessageTable.c, Code80012ED0.c, unsorted/sweep_800055D8.c, sweep_80095780.c, char.c,
PGATourSimulation.c, SitDevMisc.c; headers golfer.h, engine.h, unsorted/cull.h; README.md,
docs/getting_started.md, docs/gameplay.md, src/README.md. Counts are tree-wide greps over the
270 game `.c` files (not `src/dolphin`), so treat them as close, not exact.

## Executive summary

| Dimension | Grade | Why, in one line |
|---|---|---|
| Naming | **poor** | 80% of game function definitions are `fn_`; the core game-mode interface, the main globals and half of all struct fields are placeholders, while EA names for many of them sit parked in `docs/reference-builds/`. |
| Readability | **needs work** | The C is mostly natural and struct-based, but magic numbers are everywhere (no game-mode, game-type or surface-class enum), and the worst fake matches (64-bit products, one-element arrays, `nBase | (long)p`) are what a critic will screenshot. |
| Comments | **needs work** | Header comments are consistently good and intent-level; ~40% of functions have no comment; some comments read as "offset soup" (`surface +0x24`), carry stale match percentages, or are detached from their function. |
| Structure | **needs work** | `main()` lives in `unsorted/sweep_800055D8.c`; 23 `Code800*.c` + 29 `unsorted/` placeholder files; `Vec3`/`Vec4` defined in `include/unsorted/cull.h`; 568 functions prototyped in 2+ `.c` files against style.md's own rule. |
| Fidelity | **good** (with caveats) | Byte match, `port:`/`EA bug:` discipline (335 / 123 notes), EA asserts reproduced, paired-single `asm` kept as EA's; caveats: 486 fake-match notes, 64 invented "stripped stand-in" bodies, 39 per-function pragmas. |
| Newcomer docs | **poor** | README.md still says "26.1% matched" and lists sweep files as the main content; getting_started.md is the untouched dtk-template walkthrough; gameplay.md contradicts the code's own comments. src/README.md is good. |

The honest one-liner: the *engineering* is strong (100% byte match, disciplined notes, audited
function comments); the *surface a reviewer sees first* (README, names, magic numbers, `main()`)
still reads like a fresh machine pass.

## 1. Naming

- **Functions:** 5,420 of 6,761 function definitions in game `.c` files are `fn_XXXXXXXX`
  (80%); `symbols.txt` has 5,351 `fn_` of 7,647. Whole files are 99-100% `fn_`:
  FE_MessageTable.c (463/464), GameUICommands.c (227/228), startUp.c (105/105), GameAudio.c
  (105/106), GoTerrain.c (99/103), EASBStorage.c (91/91). Only 46 provisional `fn_<addr>_<Guess>`.
- **The most-called functions are unnamed although their meaning is known:** `fn_80009E70`
  (340 calls; the free), `fn_80009B34` (261; the allocator, called with `"Swing.c", 555`),
  `fn_80009680` (207; engine.h:138 says `// sqrt`), `fn_800BAF04` (157; normalize),
  `fn_80009744` (126; dot with itself), `fn_8001EF34` (107; engine.h:1406 "scale a vector").
  Ball.c:1184 `Physics_HandleCollision` is unreadable largely because of these.
- **EA names in hand, not applied:** LLFileIO_Gc.c:63 says "EA's name, from its lock:
  file_RequestDaemon" above `fn_80005D10`; UISStack.c's interpreter `UISStackProcess` is Madden 2003
  STABS `UISStackProcess` (docs/reference-builds/madden2003-ps2/pairing.md pairs ~68 UIS
  functions plus every UIS struct and field); golfer.h:566 `pfn200 // TW06: GetCurrentLead`;
  TW07 `pairs.tsv` has 60 med/high-confidence pairs on `fn_` functions; the MAD decoder (14) and
  ska names are parked in CLAUDE.md. A critic will say "you *have* the symbols and didn't use them".
- **Globals:** 4,315 of 4,971 objects are `lbl_` (87%), including the obvious singletons:
  `lbl_801D3CB0` (the terrain renderer manager, GoTerrain.c), `lbl_80281D88` (dynamic cameras),
  `lbl_80281FE8`/`lbl_80281FDC` (memory-card save buffers, MC.c), `lbl_80281ED4` (FE state).
- **Struct fields:** of 4,574 header fields with offset comments, 2,341 (51%) are placeholders
  (`n38`, `b27F`, `unk1F4`); camera.h 369/432, character.h 264/460, engine.h 270/511. The
  game-mode callback table in `GameState` (golfer.h:559-591) is 32 `pfnXXX` slots: every
  `GameMode*.c` Init function is then a wall of `gpGame->pfn1EC = ...; gpGame->b27B = 0;`
  (GameModeReplay.c:26-49, GameMode22.c:217-260).
- **Locals/params:** 193 single-letter float locals (`fE`, `fF`, `fG`... in Ball.c:1185-1216),
  71 functions with `p0`/`arg0` parameters, 138 lines of m2c residue (`arg0`, `var_`, `temp_`),
  worst in GoRenderCtx_Gc.c (19), EASportsBio.c, GoDynObj.c, GameTargets.c (a *model file*,
  GameTargets.c:263 `s32 fn_800F2408(s32 arg0)`).
- **Accuracy of existing names:** spot checks held up (CamSpline's Catmull-Rom matrix is the
  standard basis; Swing's `SW_vInitSwing` comment matches its calls; MC's mount/unmount pattern
  matches). The audit process is doing its job; the problem is coverage, not wrong names.

## 2. Readability

- **Good:** most code uses structs and named fields (`gPlayers[n].swing.nRestX`), tables are
  typed and commented row by row (Ball.c:22-135 club tables with `DEG()`/`INCHES()` macros is
  exemplary), MC.c uses `MC_ERR_*`/`MC_SAVE_*` constants, LLFont.c reads like real code.
- **Magic numbers:** no game-mode enum: 215 `Game_GetMode() == N`; 151 `nClass == N` surface
  classes (Ball.c:1220-1245: `nClass == 7 || nClass == 16` is water); 115 `nGameType == N`. Only
  18 enums in all of `include/`. TW06/07 have `GameMode_Stroke`, `GameMode_Skins`... names.
- **Raw offsets:** rare (36 `*(T*)((u8*)p + 0x..)`), but concentrated where a reader lands:
  GoRenderCtx_Gc.c:189-199 `fn_80013CCC` is raw m2c output two functions below code that uses
  the same `Camera` struct properly; sweep files (`sweep_80095780.c`: `*(s32*)((u8*)(arg0)+0x20)`).
- **Fake matches:** 486 `fake match` notes in 133 files; all labelled, which is to the project's
  credit. The ugly ones: 76 `(u64)`/`(s64)` casts ("64-bit product whose dead high word fills the
  pre-allocation schedule", UISApi.c:227-270); uiArc.c:54-86 declares 14 floats as one-element
  arrays and six as `f64 fU0[1]` to steer the allocator; UISApi.c:204 `(nBase5 | (long)pFile)`;
  `#pragma optimization_level 2` on one function; 64 `static f32 X_StrippedFn(f32 x) { return x +
  1.0f; }` invented bodies (Swing.c:202, GoDynamicCam.c:231) that are not EA code at all; 57
  `goto`s. The Madden STABS source for UIS may show EA's real form for some of the UIS fakes.
- **Vector style drift:** `f32 v[4]` arrays, `Vec4` structs and `Vec3Copy(&v.x)` mix in the same
  file (CamSpline.c:60-110), because `Vec3`/`Vec4` came late via `unsorted/cull.h`.

## 3. Comments

- **File header comments: good.** Every sampled unit opens with what it does and where its name
  comes from (GoTerrain.c, MC.c, LLFileIO_Gc.c are exactly what a newcomer needs).
- **Coverage:** ~2,690 of ~6,650 functions (40%) have no comment; whole runs of FE_MessageTable.c
  handlers (8007D25C-8007D3D8) are bare `fn_` bodies with neither name nor comment.
- **Offset-speak instead of intent:** Ball.c:1163-1183 explains the bounce in terms of "surface
  +0x24", "+0x0C", "class 11"; GoTerrain.c:486-495 "word 3 bits 0x4, 0x10, 0x20". Accurate, but it
  documents placeholders instead of naming them; once fields are named these comments shrink.
- **Detached comments:** Swing.c:206-216 stacks four functions' comments above `SW_vInitModule`
  ("The swing is under way..." belongs to `SW_vStateInitBackSwing` at :341, "a hair early" to
  `Swing_TopTime` at :284, "Free what..." to `fn_80058DB4` at :265). A scan found this pattern
  only in Swing.c; others were legitimate group intros.
- **Broken comment:** GameModeReplay.c:107-108 (the model file!) "Sized as the distance / port:
  between the two fields the copy scores 96.4%" is two notes spliced mid-sentence.
- **Stale match percentages:** 56 comment lines quote percentages; most are fake-match
  justification ("without the gotos: 83.9%, not 84.9%", Swing.c:1182 x4; "(96.9%)",
  GameModeReplay.c:82). Legitimate evidence, but it reads as work-in-progress noise in finished
  code; it belongs in the commit message or a single terse form.
- **Process noise in source:** low. 0 hits for lane/ledger/agent/TODO-style; 8 "found by the
  permuter" (fine). Process noise lives in git history instead: 106 of 1,422 commit subjects say
  "ledger", many carry "98.08 -> 98.26" style progress.
- **Compiler talk in headers:** Swing.c:4 "CodeWarrior GC/2.5, -O4,p" and "section note:"
  paragraphs (LLFileIO_Gc.c:8-12) are fine for matching but dense for a newcomer.

## 4. Structure

- **Entry point in a sweep file:** `main()` is in `unsorted/sweep_800055D8.c`, header "Original
  file and meanings unknown", with unprototyped `void fn_80005520();` and `s32 main(s32 p0, s32 p1)`.
  First thing a reader opens from src/README.md's "Start here".
- **Placeholders:** 23 `Code800*.c`, 29 `unsorted/*.c` (69 functions). Their headers are honest;
  src/README.md marks them `ph`.
- **Headers:** core math types (`Vec3`, `Vec4`, `Sphere`) live in `include/unsorted/cull.h`,
  included by 33 files. engine.h is 1,462 lines of mixed services. 568 functions are prototyped
  in 2+ `.c` files (`fn_8000ADC0` in 21), 16 `extern` and 16 `typedef` lines in `.c` files, 176
  empty-paren `()` prototypes: all against style.md's "Prototypes" section.
- **Unity include:** char.c:64 `#include "../src/char_tex_manager.c"`, justified in a comment by
  TW07's `golf2_unity.cpp`; style.md says "No includes of other units' .c files". Either the rule
  gets an exception note or the include gets its reason in style.md.
- **Library file split:** our `UISEvent.c` = EA's UISActionProcess.c + UISError.c + UISEvent.c
  (pairing.md); EA's split is known from STABS but not applied.
- **Consistency:** early units (Ball.c, Swing.c) and endgame units (UISApi.c, uiArc.c) differ
  most in fake-match density, not in style; drift is smaller than expected. The model files
  named in style.md are not models (see GameModeReplay.c above, GameTargets.c `arg0`).

## 5. Fidelity signals

- Good: allocator calls keep EA's `"Swing.c", 555` file/line; `port:` notes mark EA's 32-bit
  habits instead of rewriting them (LLFileIO_Gc.c:80 callback with extra args; GameMode22.c:230);
  `EA bug:` notes are specific (MC.c:340 leaves a card mounted); discarded-result calls kept
  (Swing.c:1197 `Golfer_GetAttribute(...)` unused), which is EA's form, not ours.
- Types: `u8` returns for EA's 8-bit `Bool` match Madden's typedefs, but we spell them `u8`, not
  `Bool`; EA's `Uint32`/`Int32` family is known from STABS for the UIS library.
- Risk: the 64 invented stripped-function bodies and the one-element-array/64-bit tricks are the
  places where the source says something EA never wrote. They are labelled; they still count
  against "EA's code exactly as EA wrote it".
- Struct evidence exists but is unused: Madden gives UIS struct and field names (`UISInfoT`
  `CriticalRegions`, `pGlobalScript`, `bShuttingDown`) that "correct our guesses"; TW06-Xbox
  key-types has `GameModeDriverPGATour.hpp`.

## 6. Docs a newcomer meets

- **README.md is badly stale** ("Last updated 2026-09-23", "Matched code: 26.1%", "about 2,370
  small functions ... in unsorted/sweep_*", "Ball.c, 63 of 68 functions exact", "About 6,170 are
  still fn_"). The badges say 100%; the text says 26%. This is the single worst first impression.
- **docs/getting_started.md** after a good first paragraph is the dtk-template guide ("Create a
  new repository from this template", "Rename orig/GAMEID"): wrong for this repo, and it does
  not mention `tools/cloud/`, the private `main.dol`, or `ninja` ending in `main.dol: OK`.
- **docs/gameplay.md** is stale and unaudited (flagged in CLAUDE.md): it still says Golfer.c "is
  being decompiled", uses `field_A08`, and says distance units are unknown while Ball.c:1 says
  yards and seconds.
- **src/README.md: good.** Start-here table, glossary, subsystem map. It is the doc to lead with.
- **Separation:** `agents/` (2.5 MB, roles, findings, state), `CLAUDE.md` and `docs/journal.md`
  sit at top level next to the project docs, and docs/ mixes reader docs (style, gameplay,
  formats) with internal process (journal, splits, symbols, tw2004-notes, 1,717-line
  decomp-notes). A visitor cannot tell which of 20 `docs/*.md` to read.

## 7. Top 10 for credibility (what a critic singles out first)

1. README says 26.1% while the badge says 100% (README.md:25-45).
2. 80% of functions are `fn_`, including `sqrt`, `malloc`/`free`, `normalize` (fn_80009680,
   fn_80009B34, fn_80009E70, fn_800BAF04), which makes even the flagship Ball.c unreadable.
3. EA symbols available and unused: Madden STABS for all of UI Studio, TW07/TW06 pairs, EA's
   own lock/assert strings (LLFileIO_Gc.c:63 `file_RequestDaemon`).
4. `main()` in `unsorted/sweep_800055D8.c` with `p0, p1` and K&R prototypes.
5. No enums for game mode / game type / surface class: 480+ bare number comparisons.
6. The game-mode interface `GameState` is `pfn1E4`...`pfn264` + `b271`...`b28A`, so all ~30
   `GameMode*.c` files read as walls of offsets.
7. Fake-match showpieces: uiArc.c:54-86 one-element float arrays, UISApi.c:227-270 64-bit
   products with "dead high words", `nBase5 | (long)pFile`.
8. 64 invented `*_StrippedFn` bodies (`return x + 1.0f;`): code EA never wrote.
9. Raw m2c output left in linked units (GoRenderCtx_Gc.c:189-199, sweep files, `arg0` in
   GameTargets.c, which style.md calls a model file).
10. getting_started.md is the template's; docs/ has 20 files with no reader/internal split;
    `agents/` + `CLAUDE.md` in the repo root advertise the AI-lane process.

## Procedural feedback loop

### Per-file quality bar (a lane applies it to one unit, then the next)

A unit is **"reviewed"** when all hold:

1. Header comment present and still true; placement is a real file (not `unsorted/`) or the
   header says why not.
2. Every function has a name with evidence (T1/T2) or is `fn_` with a one-line comment; no
   function over ~15 lines without a comment.
3. No `lbl_` for a global the file owns and the header comment describes.
4. Every struct field the unit *reads or writes in logic* is named or has an offset comment with
   a one-line meaning; comments speak in field names, not `+0x24`.
5. No bare numbers for game mode, game type, surface class, shot kind, club (enums from TW06/07
   where values are confirmed).
6. No `arg0`/`var_`/`temp_`/`p0`, no letter locals, no raw `*(T*)((u8*)p+off)`.
7. Prototypes: nothing prototyped here that another `.c` also prototypes; no `()` prototypes; no
   `extern`/`typedef` in the `.c`.
8. Fake matches: each note in one terse form (`// fake match: <what>; <why>`), no progress
   percentages; each one checked against the reference builds for a truer EA form first.
9. Comments sit on their function; no stale status ("being decompiled", "not matched", "%").
10. Build still ends `main.dol: OK`; `auditbaseline.py` shows no unexpected *changed* functions.

### Verification

- The byte match proves renames/local renames/enum substitutions change no code (run `ninja`).
- Names and behaviour comments still go through the audit (two blind readers + reconciler);
  add a `reviewed` column to a per-unit ledger so progress is measurable.
- A reviewer lane (not the author) samples 3 functions per reviewed unit and signs off.

### Lint checks to automate (extend tools/match/lint.py; report per file, gate merges on "no new")

| Check | Measure now |
|---|---|
| `fn_` / `lbl_` defined per file, and share | 80% / 87% |
| placeholder fields referenced per file (`\b[nfbpu][0-9A-F]{2,3}\b`, `unk\w+`) | 51% of header fields |
| m2c residue (`arg\d`, `var_`, `temp_`, `p\d` params) | 138 lines, 71 functions |
| single-letter locals (`f[A-Z]`) | 193 |
| raw offset derefs `*(T*)((u8*)x + ...)` | 36 |
| `Game_GetMode() == N`, `nClass == N`, `nGameType == N` | 215 / 151 / 115 |
| percentages in comments (`\d+\.\d+%`) | ~56 lines |
| stale-status phrases (`being decompiled`, `not matched`, `WIP`, `TODO`) | few in src; docs |
| stacked comment blocks with no code between (detached comments) | 11 hits, Swing.c real |
| same function prototyped in 2+ `.c` files; `()` prototypes | 568 / 176 |
| functions without a preceding comment | ~2,690 |
| README/docs numbers vs report.json (fail if they differ) | README 26.1% vs 100% |

### Order of attack (impact per effort)

1. **Front door (hours):** rewrite README.md status from report.json (and make CI check it);
   replace getting_started.md with this project's real setup; one "Docs index" paragraph that
   separates reader docs from `agents/`/journal/internal notes; banner gameplay.md as unaudited.
2. **Core utility names (a day, huge leverage):** name the ~50 most-called engine functions
   (math, memory, vectors, strings) whose meaning is already in engine.h comments or TW07; each
   rename improves hundreds of call sites. Move `Vec3`/`Vec4` to a core header.
3. **Apply parked EA evidence:** Madden STABS for UIS (functions, structs, fields, the EA file
   split), TW07 med/high pairs, MAD/ska names, assert/lock strings. Pure `rename.py` work,
   verified by the match.
4. **Enums:** game mode, game type, surface class (values proven from the code, names from
   TW06/07). Mechanical substitution, match-verified.
5. **`GameState` callbacks and flags:** name the 32 `pfn` slots and the `b27x` flags from TW06's
   GameMode classes; ~30 game-mode files become readable at once.
6. **Entry point and m2c residue:** give `main()` a real home (GoEntry area) and clean the
   GoRenderCtx_Gc.c / sweep / `arg0` functions into struct form.
7. **Fake-match pass:** for each of the ~135 "ugly" kinds (64-bit, arrays, OR, identity, stand-in
   bodies) search once for a truer EA form (Madden source for UIS first); normalise note wording,
   drop percentages.
8. **Prototype/header hygiene:** move the 568 multi-file prototypes to headers (typeaudit/
   dedupe_decls tooling exists).
9. **Long tail per unit:** apply the checklist file by file, biggest readers' files first (Ball,
   Swing, Golfer, GameManager, GoEntry, gomainloop, the GameMode family), then FE/UI tables.
