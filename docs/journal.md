Project journal
===============

Dated log of what was done and decided, newest last. Facts and lessons belong in
[`tw2004-notes.md`](tw2004-notes.md), [`decomp-notes.md`](decomp-notes.md) and
[`compiler.md`](compiler.md); this file records the order things happened and why.

2026-09-21 - Project start
--------------------------

**Setup**
- Searched GitHub: no existing TW2004 decompilation. Created `mitsevox/tw2004` (private) from
  `encounter/dtk-template`. One repo for both tracks (code and asset formats).
- Both discs are `GW4E69` Rev 0 with a byte-identical `main.dol`, so there is one code target.
  Disc images are hardlinked into `orig/` and ignored by git, along with every asset extension.
- The discs hold `.hog`, `.gcb`, `.sac`, `.fxg`, `.ngc` and `.sab` files, not the `.big` / `.viv`
  archives first assumed. Course folders are split across the two discs (see `formats/README.md`).
- Roadmap changed from a four-step list to a six-level bottom-up ladder. Added an "EA engine
  services" level because asset loaders are the bridge between the two tracks.

**Discovery**
- dtk initial analysis: 7,646 functions, rebuild is byte-identical.
- The game is plain C. About 92 EA source file names leaked through assert strings.
- The SDK build matches Metroid Prime Rev 0's, to the second.
- `main.dol` contains code from two compilers: CodeWarrior for nearly everything, and unoptimized
  GCC for a 38 KiB EA shared file library.

**Compiler identification** (full evidence in `compiler.md`)
- First guess (same as Prime, GC/1.3.2) was wrong.
- An MSL library test pointed at GC/3.0, a compiler built two years after the game shipped. This was
  a false positive. Lesson: an impossible result is a red flag.
- Function-opening style, a linked-list function and a Vec3 copy narrowed it to GC/2.0 - 2.7.
  Build dates rule out 2.7 and make 2.6 unlikely. Baseline set to GC/2.5, with GC/2.0 as fallback.
- Decided not to ask the GC/Wii decomp Discord: unverifiable answers are worse than none. Prefer
  evidence we can reproduce.
- Tried to separate 2.0 from 2.5 with fingerprint scans, switch statements, 64-bit math, a
  bounding-box loop and a bit-field function. None separated them. Decided to stop chasing it:
  the wrong choice will show up as a function that refuses to match, and the symptom is documented.
- Confirmed `-O4`, `-proc gekko`, `-fp hardware`, and no automatic inlining (`-inline auto` is wrong).

**Matching** (28 functions, 2,016 bytes, three files)
- First match: `Vec3Copy` at `0x80008304`. It turned out to be called 596 times.
- Its neighbours are camera getters. Tracing their callers led to the frustum cull test
  `fn_80007D74`, which identified the object as the camera.
- Matched the two cull-test wrappers and a point-transform helper (needed a declaration-order fix,
  the inlining fix, and claiming the file's `1.0` constant).
- Matched the cull test, 215 instructions, in three builds: 96.88% -> 99.81% -> 100%. The last fix
  was writing `x / 2.0f` instead of `x * 0.5f`.
- Matched the gap between the two files (vertex format setup, function-table dispatch). The matched
  block is now continuous from `0x80007BC4` to `0x800083A4`.

**Naming**
- 423 SDK / MSL functions named by matching against Prime's split objects, with two self-checks
  (180/180 and 1,906/1,906). Tool saved as `tools/research/match_sdk_names.py`.

**Widescreen pivot**
- Used the decompiled cull test to write two Gecko codes (details in `tw2004-notes.md`). Traced the
  frustum setup to `fn_80013950`; the second plane set is the first at double width.
- Play test: full 16:9, no pop-in, no glitches, including flyovers and replays.
- Control test with the code off: obvious pop-in of trees, grass and objects in the outer 12.5% of
  the screen on each side, on any camera movement. The two-instruction code is what fixes it.
- Dolphin only auto-swaps discs when launched from an `.m3u` playlist; we launched Disc 1 directly.

2026-09-22
----------

- Widescreen code credited to `mits` (the user's forum handle). Forum post drafted in
  `docs/publish/dolphin-forum-post.md` and posted to Dolphin forums > Cheats, Hacks & Game Patches;
  awaiting moderator approval. Wiki entry and GameHacking.org submission still to do.
- Repo made public.
- **SDK sprint.** Matched code 6.69% -> 14.82% in one day, almost all of it borrowed source:
  Level 1/2 linked 49 partial SDK/MSL units from Prime as NonMatching text spans; Pikmin 2's
  MetroTRK (CC0) vendored to `extern/trk` (97 functions exact); a later SDK's library objects
  (reference only, outside the repo at `C:\dev\ext\sdk`) named 134 SDK functions and showed the
  April 2003 CARD patch in code (`__CARDIsWritable(card, ent)`; `CARDRead`/`CARDStat` now 100%).
  The 8.7 KB block after `GXClearGPMetric` is MIX, not DTK. Dashboard chart fixed (it counted
  only fully Matching units) and backfilled with a worktree build per commit.
- **Game code: the 90-99% band.** 27 KB of already-written `Swing.c`/`Golfer.c` functions that
  differed by register or statement shape. 27 more exact so far (Golfer 69/91, Swing 55/181).
  Rules in `decomp-notes.md`: the `lwzu` idiom is a repeated field access; 64-bit event ids;
  mask calls inline; chained assignment order; hoisted constants as locals; switch-with-one-case.
- **Swing.c written out** (14.82% -> 15.70%): the inline-mask rule (States 04/05/08/09/10 exact),
  59 small unwritten functions in one batch (58 exact), the swing state machine (push = replace
  top, `fn_8005CF4C` push, `fn_8005CFD4` pop, `fn_8005D05C` reset, `fn_8005CCD8` tick), `Swing_Init`
  (the file really is `Swing.c`: the allocator's file argument), the stick twist, the club trail
  (25 head/grip samples) and its drawing (the "swing trail" option, textures "clubback" /
  "clubdown"), State 11. Swing.c ~150/181 exact; left: `SwingState01_Update` (1.6 KB) and
  register puzzles (`fn_8005CCD8/CD94/D05C`, `SwingStack_Push`, `fn_8005CFD4`, States 20/22 loops,
  `fn_8005A478` 99.4%, `Swing_MeterError` 99.2%, `SwingState12_Update` 98.7%). Compiler rules
  found: locals are stacked in reverse declaration order and declaration order picks the saved
  registers (`tools`: `C:\dev\scratch\tw\permute_decls.py`, `variants.py`); short literals live in
  .sdata; `beq next; b end` in a chain of tests is a one-case `switch` with `default: return`.
- **The pool-cue gimme.** From the user's memory of TW2003: gimmes are within 18 in of the pin
  (`Gimme_Allowed`); animation 11 (`CharAnim_StartTapIn`) picks a random clip from group 9 of the
  golfer's animation library (`skalib.c`: `AnimLib_Find/Pick`, a played-mask so none repeats).
  Libraries decoded (`SAL` objects; `tools/research/sal_dump.py`, `find_sal.py`): `gplptt12`, the
  pool-cue tap-in, is in seven golfers' sets (Pops's only one). A Gecko code
  (`tools/codes/pool_cue_gimme.txt`, hook at `0x80025AD8`) hands every gimme that clip when one of
  the seven is in the round: **confirmed in game** (4 of 4). Gimme path decompiled into new units
  `skalib.c` (EA's name), `CharClip.c`, `CharAnim.c`, `HoleScore.c`, `Gimme.c` (ours): 7/8 exact.
  Golfer -> character file map in `formats/game-data.md` (Donatello's and McGregor's files keep
  EA's working names Capone / McGruff).
- **`skalib.c` written in full.** The unit now spans EA's whole file, 0x80021ADC-0x80026844 (it
  starts at the find-clip-by-name and ends with the `'SAL '` / `'BNK '` stream-handler pair; the
  8 KB before it has no asserts and is too big for skalib's first 500 lines, so it is another file).
  53 functions, 40 exact, the unit 97% fuzzy. It covers the loaders and byte-swapping, three slots
  with double buffering, the ARAM stash of banks, the paired clip-tree walker, the merge passes
  (size, release, trim, keep, drop, build), the per-round budget, and the overlay merge. The last
  two are at 92% and 87%, and the rest of the misses are register numbering. Game facts are in
  `gameplay.md` ("Animation memory"), compiler rules in `decomp-notes.md`: an `s32` loop counter
  keeps the unroll guard, a bound held in a local or written `n > i` folds it, an initialiser
  after early exits is an inner block, and `table[n].x` rather than a pointer produces the
  original's spilled address temps. Also: GC/2.0-2.7 all compile these identically, so it is
  not a compiler-version question (`try_compilers.py`).
- **Three new hypotheses from the user** (`hypotheses.md` 6-8): CPU putt misses come from
  break, not distance; the power boost's full meter needs a non-linear number of Z presses;
  the heartbeat comes from a look-ahead sim right after the strike. Leads are written in each entry.
- **Hypotheses 6-9 answered** (9 added by the user: gimme lip-outs that still count, and
  "straight in" reads that miss). Results in `hypotheses.md`, mechanics in `gameplay.md`:
  6 half right: distance is coded (an angle error, none under 1.5 yd), but every CPU putt is hit
  5% firmer than its rehearsal, so breaking putts miss high more the more they break.
  7 refuted on the count: each Z *tap* is one level (the pad word's low half is "pressed this
  frame"; the old "held, per frame" note was wrong), but taps count only while the club goes back
  and the reward is back-loaded (1 2 4 6 9 12 16 20). 8 confirmed: at the strike the ball is copied
  to `+0xB5C` and run ahead in fast-forward each frame (`fn_800DF824`); predicted holed -> camera 11,
  predicted close -> golfer reaction animation 9 once the real ball is 2-5.5 yd out. 9: a gimme sets
  flag 8 and the ball at rest is forced to lie 12 (holed) wherever it is; the caddie read is real
  physics, and the misses come from the random +-15 stick wobble added to every human stroke.
  `Player.ballBefore` (0xB5C) is really the look-ahead ball (rename pending). Helpers:
  `dolread.py` (read main.dol by address), `grepfn.py` (asm grep with the enclosing function).
- **`SwingState01_Update` in C (96.1%).** State 1 is addressing the ball, not the swing: the
  golfer's animation places or tees the ball (it rides in the hand until event 3), a moving ball
  is stepped, a CPU rehearses, button 0 hurries the camera, state 2 when the camera is done or
  after 10 s. The swing phase table runs in state 10 (and 12); `gameplay.md`'s table is fixed. Same
  instruction count as the original; the rest is CSE: from the ball-state block on, ours keeps
  `&gPlayers[n]` in a register where the original folds the state read into `lwzu` and recomputes
  the base each time. Pointer local, value local and nested-if variants and 24 declaration orders
  did not change it. New fields: `Game.pfn238`, `View.fCamTime` (0x104), `ShotObj.fAnimEnd` (0x184).
- **`Ball.c` checkpoint 1: the small functions (47 of 68 exact).** Wind (`Wind_Get`/`Set`/
  `Generate`, `gWindDirs` named), four course settings with their setters and getters (two
  come from options +0x18/+0x1C and are nudged on courses 6 and 15), the stance slope angles
  (+-44 degrees), spin input (x 15), ball reset / place / drop, `Ball_Stop`, the 20 ms stepper,
  `Ball_SimSeconds`, and the paired-single vector helpers. `Ball` struct filled in to 0x9C.
- **`Ball.c` checkpoint 2 (50 of 68 exact).** Putt power curve (`gPuttDist`, `gPuttSpeedScale`:
  distance = 43.4 yd x power^2 on a medium green, so the CPU's +5% putt pace is +10% of roll),
  the club distance tables (`gClubRows1..7`, `ClubRow`), chip power, `fn_80051A18`, `Ball_Launch`.
  The two interpolations are at 86% / 89% (the int-to-float term's register and schedule).
  `lbl_80281130` is the green speed.
- **`Ball.c` checkpoint 3: the strike, `fn_800512BC`, exact (51 of 68).** Club and kind tables
  named (`gClubSpeed`, `gClubLoft`, `gClubStep`, `gClubSpin`, `gChipSpeed`, `gChipLoft`,
  `gKindSpeed`, `gKindLoft`, `gKindSpin`, `gTrajLoft`). Mechanics in `gameplay.md` ("The
  strike"): putts laid onto the green plane with no spin; uphill lies cost speed; sidehill lies
  tilt the spin axis (not for a perfect shot); rough and sand speed/spin table.
- **`Ball.c` checkpoint 4: flight and tick (55 of 68 exact).** `Ball_Tick`, `fn_8005418C`,
  `fn_80054040`, `fn_80055324` exact; `Ball_FlightStep` 98.3% (two `addi` schedules),
  `fn_80052088` 99.1% (a branch-over-branch). Findings in `gameplay.md` ("The ball in flight"):
  wind scaled down near the ground, quadratic drag/lift with spin terms, gravity x3 within
  2.25 in above the cup for the real ball but not the CPU rehearsal, 4-second stall check, a
  probable `n < 156` typo in the coasting surface.
- **`Ball.c` checkpoint 5a: skid and roll (56 of 68 exact).** `fn_80052268` (skid) exact,
  `Ball_GroundContact` (roll) 99%. Break = slope / spin per tick (slow balls break more), 5/7 g
  along the line, rolling friction x 0.575 on greens. **The cup pull is skipped in sims** unless
  `gSimFullCup` (was `lbl_80281DD1`): the CPU rehearsal and the caddie run without it
  (`gameplay.md` "The cup" corrected, hypotheses 6 and 9 updated). Course settings identified:
  `gTurfSpeed` (was `lbl_80281130`) is the weather - rain sets 1/0, slower turf and a shorter
  putt table; options +0x18 / +0x1C are green friction / rough friction.
- **`Ball.c` checkpoint 5b: landings, lies, trees, water.** `Ball_Collide`, `fn_80053240`,
  `Ball_SetLie`, `fn_800539F8`, `fn_80053E98` all at 99.2-99.9% (register numbering). The
  landing events are a macro (`BALL_LANDING_EVENTS`), not an inline function - the original
  re-reads the stack locals. Findings: lies 3/4 are rough and 6/7/8 sand (the strike table was
  labelled backwards; fixed); sand plugs on hard landings; course 6 has only the bad rough lie;
  tree deflection is biased 31/32 one way (`& 31` sign test); stick spin is applied at the
  first bounce and weakened on faster greens; water skipping on three water surfaces.
- **`Ball.c` checkpoint 6: the bounce, `fn_80052598` (98%). All 68 functions are now in C (unit
  99% fuzzy).** Soft ground bends the normal toward the incoming ball; restitution from surface
  +0x0C; negative restitution = branches the ball passes through, randomised with LUCK pulling
  toward a cleaner pass (the LUCK tooltip's "kinder bounce"); rain deadens bounces; an automatic
  backspin "check" for shots from over 63 yd on dry short grass, cut to a tenth when the spin
  stick was used. The original reuses nine float variables for many jobs; the C now mirrors
  that reuse.
- **Putt test harness: hypothesis 6 measured.** `tools/research/putt_sim.py` (Python) ports
  skid, roll, cup pull, air and the launch from `Ball.c` and the rehearsal from `Golfer.c` onto
  a tilted plane. Validation: level-green roll matches `gPuttDist` within 0.6% at green setting
  2 (so the putt table was built for the fastest greens). Result: a perfect CPU holes every
  straight putt, but the +5% pace misses high once break x length is big enough (15 ft at 3%,
  20 ft at 3%, 30 ft at 2%, 45 ft at 1%; rerun after the cup-pull fix); with skill error, PUTTING 98 makes are decided by break, PUTTING
  80 by length. The +5% is "never up, never in" for average putters. The rim is modelled, not
  the game's (cup geometry not decoded); `--cup 2.25` shows the same pattern. Side find:
  surfaces 98/105 are the cup (the "water" comment in `AI_PlanShot` was wrong).
- **`Ball.c` polish: 56 -> 63 of 68 exact** (unit 99.66%). Set up `decomp-permuter` on Windows
  (`tw2004-notes.md`, "The permuter"). Matched: the putt and club lookups (one result variable;
  a `ClubRow_Dist` accessor), flagstick (squared distance inline), the landing flag lookup
  (`Ball_SpinKeep` inline), rolling (`fTurn = -fTurn` in place), the ground check (`Ball_Owner`
  inline with an early return), the cup pull. Two real fixes found by matching: the club tables
  are power 0.1..1.1 (reach = full power, last column 110%), and the cup pull's "no speed-up"
  limit is on axis speed (> ~1.5 ft/s), not the heading angle; the putt harness and hypothesis 6
  were rerun (same conclusions). Left: `fn_80052598` (bounce, saved-register choice across nine
  reused floats), `Ball_FlightStep` (one hoisted address), `Ball_SetLie` (one `beq; b` branch),
  `Ball_Collide` / `fn_80053240` (the owner chain's base pointer in r5, not r4). Permuter jobs
  for all five were running at the end of the session (`C:\dev\scratch\tw\perm\`).
- **Small-function sweep: +1,138 functions exact, 17.06% -> 18.33% code, 1,236 -> 2,374 functions.**
  `C:\dev\scratch\tw\sweep.py` writes C for mechanical shapes (empty functions, constant returns,
  field and global getters/setters, one-call wrappers whose arguments are parameters, constants,
  globals or fields) into `src/unsorted/sweep_<address>.c` units, builds them as NonMatching,
  keeps only units whose every function is exact (`sweep.py keep`) and links those. 1,138 of
  1,138 generated functions matched. Units are runs of adjacent functions, or single functions;
  they are placeholders until the real file boundaries are known (merging is mechanical).
  Data symbols in literal pools (`.sdata2`, `.rodata`) are left alone. README numbers updated.
- **CI and decomp.dev.** The build runs on GitHub on every push (private `mitsevox/tw2004-build`
  holds only `main.dol`); the project is listed at https://decomp.dev/mitsevox/tw2004 with README
  badges. Setup and pitfalls in `tw2004-notes.md` ("CI and decomp.dev").
- **Sweep continued, then m2c.** The sweep's translators grew: a general straight-line translator
  (any branch-free function: loads, stores, arithmetic, calls with results in temporaries, plain
  stack frames), then m2c (github.com/matt-kempster/m2c, cloned to `C:\dev\tools\m2c`) for
  functions with branches, its output cleaned to plain C (`sweep_m2c.py`, `sweep.py gen N 1 --m2c`).
  A skip list keyed on the generated C (`sweep_skip.json`) stops retrying failures until a
  translator changes. Pitfall: an exact unit can still break the linked DOL if it brings data -
  16 m2c units carried the int-to-float constant in `.sdata2`; `keep` now rejects any unit
  object with a data section. Result: **22.08% code, 3,483 of 7,647 functions**. Split: EA's code
  (below `0x8012E950`, 79% of the executable) **11.43%**, 2,570 of 6,433 functions; Nintendo SDK
  and runtime (the rest, 21%) **62.71%**, 913 of 1,213. Most of the headline number is SDK;
  the EA figure is the one to watch.
- **m2c at 256/512 bytes and a repair pass.** Batch 11 (m2c up to 256 bytes): 43 exact; 512
  bytes: 3 (m2c's limit). `retry.py` compiles each non-exact m2c function alone and scores it
  against `main.elf`, then tries single edits (add a missing call argument first or last, drop
  prototypes, `void*` -> `u8*`, integer widths): 80 more exact. m2c's commonest miss is a call
  argument already sitting in `r3`/`r4`.
- **SDK from other decompilations** (`harvest.py`, `harvest2.py`, `integrate.py`): matching
  CC0 source from Final Fantasy Crystal Chronicles (same Sep 5 2002 SDK), Wind Waker, Twilight
  Princess, then Sonic Heroes (MIT) and Gauntlet: Dark Legacy; Melee's `__exception.s` for the
  TRK vector table (8 KB). Jump tables/reset records assigned to their units fixed the
  `name_ADDRESS` glitch for 5 functions. The 29 KB "unknown SDK" chunk was **EA's UI Studio**
  (`UIStudio.c`/`UISEvent.c`), now counted as EA. SDK 58.9% -> **82.6%**.
- **Symbols from related builds.** No TW2004 symbols exist publicly. From debugging.games:
  Tiger Woods PGA Tour 06 Xbox beta PDB and PS2 `MAPFILE.TXT` + release ELF (same studio, EA
  Redwood Shores, same engine lineage), 007 AUF/EON GameCube ELFs (different engine: no shared
  code). Name transfer by strings, float constants and call graphs (`anchors.py`,
  `callgraph.py`, `ps2side.py`): **123 functions named** (`docs/tw06-names.md`). Types
  (`tpiread.py`): TW06's `PhysicsBall_t` is our `Ball`; field names and the lie/state/club/shot/
  mishit enums applied (`include/physics.h`); lie 16 / state 5 are "out of bounds" in TW06.
  TW2003 Xbox prototypes (Sep 3 / Sep 12 2002): release builds, every file scanned, no symbols;
  they gave EA's 2002 source tree (`Golf\AI\Swing.c`, `Legacy\SPECIF\UStream.c`, ...).
  A checklist for using these references on every new area is in `tw2004-notes.md`,
  "Starting a new area: check the references first".
- Numbers at the end of the day: **26.06% code, 3,714 of 7,647 functions**; EA 14.4%, SDK 82.6%.
- **Next:**
  1. **Golfer and Swing structs from TW06** (next up): follow the checklist; TW06 types for the
     golfer, swing states and shot data should name most of `Golfer.c`/`Swing.c`'s `nXX`/`fXX`
     fields. Then re-run the name matcher.
  2. The plain m2c sweep is exhausted (every size to 512 bytes, plus the repair pass); rerunning
     it unchanged finds nothing. Only two things reopen it: (a) literal-pool functions (float
     constants, strings), never attempted, which need units that own their `.sdata2`/`.rodata`
     (proper file boundaries); (b) better inputs - feed TW06 signatures (argument counts and
     types) to m2c/`retry.py`, since a wrong argument count is m2c's commonest miss.
  3. `Ball.c` last five; the 57 medium TW06 names as their areas come up.
  Scratch tools added this round (`C:\dev\scratch\tw\`): `dolread.py` (read main.dol by
  address), `grepfn.py` (asm grep with enclosing function), `unitfns.py <unit>` (non-exact
  functions of a unit), `insert_fns.py <module.py> <File.c>` (insert CODE dict at address
  positions; now takes the target file), `ball_cp*.py` (the checkpoint code modules). optional register polish in `skalib.c`
  (`AnimLib_MergeOverlay`, `AnimLib_PlanBank`, `AnimLib_WalkPair`). Scratch tools in `C:\dev\scratch\tw\`:
  `sbs2.py` (normalised diff), `fnsrc.py`, `regress.py` (who lost 100%), `insert_fns.py` (add
  functions at address positions), `unwritten.py`, `find_fn.py` (search asm by regexes),
  `alldiffs.py`. The extracted discs are in `C:\dev\scratch\tw\disc\d1`, `d2`; the Dolphin install
  is portable at `C:\Games\Dolphin` (game settings `User\GameSettings\GW4E69.ini`).
- **SDK import (Level 0).** Copied Prime's `extern/sdk`, gave the SDK its own compiler settings, named
  data symbols by aligning references against Prime's objects, and linked 57 SDK units (162 functions).
  Matched code went 2,016 -> 53,392 bytes in one day. Pipeline in `tools/research/sdk/`.
- Local progress dashboard on the LAN (`tools/dashboard/`). Cost an evening: a windowless server that
  spawns console programs opens a Windows Terminal window per call on this PC, and a process-name
  mismatch (`pythonw3.13.exe`) meant the "fixed" server was never actually restarted. Both in
  `decomp-notes.md`.

- **TagFile.c done** (41/41 exact, linked, DOL byte-identical): the save-data container. Format,
  error codes and file boundaries written up in `tw2004-notes.md`. The `llSharedFileIO.c` /
  `TagFile.c` boundary is `0x801730C8`; the 40/50 counts in the file map were assert sites.
- 37 of 41 matched on the first build. The last four took longer than the other 37: a `u32`
  switch (`cmplwi`), operand order in a return expression, a rounding expression GCC
  reassociates unless it goes through a pointer cast, and a folded-away `if (TRUE)`. Then the
  DOL still failed twice: an unreferenced static (4 bytes of `.sbss`) and a *regression in
  SharedFileIO.c* caused by giving `TagFile_SetDescriptor` its real return type (the result
  variable's type must match the callee's for `mr r0, r3`). All six are new rules in
  `decomp-notes.md`, verified against the compiler rather than guessed.
- A clean rebuild caught that `TagFile_AllocBuffer` / `TagFile_FreeBuffer` are called from the
  game side, so they cannot be `static`. Incremental builds did not relink the auto units;
  **do a clean build before calling a file done.**
- Progress: 258 -> 299 functions, 72,832 -> 84,748 bytes.

- **Asset survey and the container cracked.** `tools/research/asset_survey.py` showed every
  `.hog` / `.gcb` starts with `CTRL`; the tag constants led to one switch, `fn_8000D4F0`
  (UStream.c). What I first took for a relocation walker (`Rdat`) is a **decompressor** with an
  unusual mirrored-copy mode. Decompiled it plus its two helpers (`src/UStream.c`, 2 of 3 exact,
  one at 98.8% over a single `addi` CodeWarrior insists on folding), transcribed it to Python
  (`tools/research/ctrl_dump.py`), and it reproduces all 325 containers' objects to their
  declared sizes. Object-type census in `docs/formats/ctrl-container.md`: terrain (`ter`),
  textures (`txf`, `TXG`), grass, per-hole cameras, and small singletons named `BALF` and
  `BIO ` in the front-end file - the first concrete leads for the gameplay hypotheses.
- Lesson: a magic word found by a 30-line survey script located the loader in minutes; the
  decompiled loader then made the extractor a transcription rather than a guess. Track A feeds
  Track B exactly as designed.

- **UStream.c written in full** (31 functions, 18 exact). Two compiler flags came out of it
  that apply to all EA game code: `-common on` and `-use_lmw_stmw on` (in `compiler.md`). Also:
  the DVD-callback-shared globals are `volatile` in the original, and the object allocator
  takes the chunk's flag word as its second argument.
- Texture format (`TXG`) decoded and PNG export working; game data tables located (attribute
  table, bios, tour, records).
- **The `UStream_Decompress` +3 puzzle is solved: the length variable is `long`, not `int`.**
  Found by isolating the compiler behaviour in ten-line test functions instead of re-spelling
  the real one: CodeWarrior folds `+= const` for `int` locals unconditionally, and stops as
  soon as the statement carries an int-to-long conversion. One typedef, 189/189. Rule in
  `decomp-notes.md`; the same lever is worth trying on the other near-miss functions.
- **First answer for hypothesis 4, from data alone:** the attribute names are in the front-end
  `DATS` object, and a debug menu there lists twelve of them - the nine on the player screen
  plus hidden AGGRESSION, IQ and SPEED. The stat record is two blocks of twelve, and the debug
  order fits the data (SPEED a constant 80, the created golfer's LUCK a neutral 50). Column
  names in `formats/game-data.md`, marked inferred until `CharSliders.c` confirms them.
- **Confirmed from code the same day, by a different route.** `CharSliders.c` is the face-slider
  loader (its "20 asserts" were 20 allocator calls). The attribute code was found by building
  the `'stat'` tag constant and the 320-byte stride: the table at `gGolferTable`, one accessor
  `Golfer_GetAttribute(player, k, mode)`, and 87 call sites. The attribute screen reads the nine
  visible indices in debug-menu order, which pins the columns. AGGRESSION and IQ have exactly two
  readers, neither in `Swing.c`. Pros carry a second attribute block used in game mode 4.
- Lesson: an assert-string count says where a *file* is, not what it does. A constant the code
  must build (a four-character tag, a record stride) finds the code that uses the data.
- **Hypotheses 2, 3 and 4 answered in one sitting, from reading rather than matching.** Followed
  the accessor's callers: the AI picks from authored aim points gated by POWER / AGGRESSION /
  skill (IQ inflates the skill), then worsens its aim by `(100 - skill)`; humans get a
  forgiveness window instead. Nothing in the shot pipeline reads the score. EA's RNG (lagged
  Fibonacci) fell out of the error code. All in `gameplay.md`.
- **Started `src/Golfer.c`** for that file: ten functions in, seven exact, the two AI routines
  instruction-for-instruction with only register numbers differing. The structs are laid out from
  the reads (player record copy at +8, its first byte reused as the golfer index). Four new
  compiler rules in `decomp-notes.md`, and one anti-rule: declaration order stops predicting
  callee-saved registers in a large loop function.
- **The whole CPU shot pipeline is in C** (20 functions in `Golfer.c`, 11 exact, the rest
  instruction-identical or off by register numbers). Along the way: `player+0x98` is the club
  bag as a bit mask (not cash), `AI_NearestTarget` is literally the nearest authored point,
  and `AI_MaxDistance` interpolates two per-club tables by POWER with a small jump at exactly
  100. Five more compiler rules recorded.
- **Swing.c started: the human half of hypothesis 4 in C.** The seven attribute readers,
  instruction-count exact. New on the way: the spin input is an amount (0..20), not a flag, so
  spin is linear in SPIN; putts over 75% count as full; the driver has a tee sweet spot; and
  mis-hit rumble scales with the attribute. `golfer.h` now holds the shared structs.
- **LUCK read, and the first real rubber band found.** `Golfer_IsLucky` doubles a human's
  lucky-bounce odds when 5+ holes down in game mode 4 (match play); CPUs never get lucky
  events. Hypothesis 3 is answered as "no, except this". Hypothesis 4 is closed: every
  attribute's readers are now read, most decompiled.
- **Hypothesis 5: found the pull.** Located the cup by scanning float pools for 4.25 in in any
  unit (0.1072 m won), then the one physics function that reads the pin position with angle
  constants. `Ball_CupPull` bends a ball within 15 cm toward the hole; the cup itself is
  geometry. New `Ball.c` unit. One trap on the way: stale split asm files from before the new
  units were shadowing the fresh ones - delete `build/GW4E69/asm` and re-run `dtk dol split`.
- **Hypothesis 1 answered, and the biggest find of the day fell out of it.** Following the
  break line's "no randomness" flag to its other users found `AI_RehearseShot`: the CPU
  simulates its shot with the real physics and corrects the aim by 45% of the miss until it
  lands. The caddie is that rehearsal run on a copy of the human in slot 4, with a 600-frame
  budget. And its per-shot setup, `AI_SetShotModifiers` (exact), is the rubber band hypothesis 3
  was looking for: +10/+20/+40 as the CPU's hole goes wrong, -5 per hole it leads in match play.
  Three hypotheses revised in one sitting; all five now answered.

**Working agreements**
- Show the plain-English logic and the C before building anything non-trivial.
- One batch of work, then report. No silent fix loops.
- Commit only when asked. No AI attribution in commits.
- Write lessons into the docs the same day.
- **`AI_RehearseShot` in C** (instruction-identical; objdiff 99.76 on float-pool labels), with its
  helpers `Shot_Prepare`, `AI_NudgeAim`, `AI_NudgeDistance`, `AI_ClubLonger`/`AI_ClubShorter`
  (both exact) and `AI_SimAbort`. New details: the sim power is capped at 150%, each authored
  aim point carries an "if it goes wrong" nudge code, the club search alternates longer/shorter
  by a growing step from the original club, and a forced stop with nothing landed is +25 and a
  fresh target. `Player.ball` is the player's own Ball at 0xA90 (0xBC bytes; `nLie` is its +0x68).
- **Wind read; hypothesis 3 closed.** Authored per hole, else rolled from the wind setting
  (0-6 / 2-12 / 5-20 / 12-31), forced up on two courses; no score anywhere. The flight step clamps
  the wind to +-15 per axis for CPU balls only. Two corrections while in the physics: the ball
  works in yards (all constants are whole inches - the cup pull is 5.5 in, holed is 2 in below the
  pin), and the rehearsal steps twelve real 1/60 s ticks per frame, not one coarse 0.2 s step.
- **The lucky shot.** `Luck_TakePerfectShot` (`0x8002DE14`): a human's successful luck roll
  starts the CPU rehearsal on a copy of them aimed at the pin, and at launch swaps the rehearsed
  club/kind/power/aim in for theirs if they were playing roughly that shot (club within 2, same
  kind, aim within 5 degrees). The caddie code and the lucky-shot code are the same code. Also:
  luck odds reset to 12 on a taken lucky shot and tighten 12 -> 11 -> 10 over the first hole
  transitions. Golfer.c: 76 functions in C, 45 exact.
- **Golfer.c fully in C: 91 of 91 functions, 56 exact** (the rest differ by register choice or
  loop shape; every one read and its logic verified against the original). The tail: player
  slot setup (`Player_SetGolfer`), session init and defaults, per-player profiles (a pro's ball
  type comes from the SPIN rating: 3 at 100, 2 at 75, 1 at 50), the table setup. Two compiler
  rules on the way: CW fully unrolls small constant loops (10, 12, 19 iterations) and unrolls a
  34-iteration one by 2, so a run of identical stores in the original is usually a loop in the
  source; and the session byte at +0x10 is the split-screen mode, which switches off luck and
  the caddie.
- **Swing.c: the swing state machine found** - 27 states x (enter, update, exit) at
  `gSwingStates`, a per-player stack (`SwingStack_Top/Clear`, exact), all 67 callbacks named
  `SwingStateNN_*`. In C so far: `Swing_Launch` (the hit: replay reseed, lucky-shot swap, error,
  power, face angle, `Ball_Launch`), `Swing_MeterError` (the stick-path error with a random +-15
  wobble on both x samples), `Swing_FaceVector`/`Swing_CurveAngle`/`Swing_ShapeVector` (the
  human draw/fade: the backswing's sideways angle through a response curve and a per-club range;
  the driver's tee bonus is the same angle at 36..54 degrees), and `SwingState02_Update` (the
  CPU's think time: 1..4 s of rehearsal, tolerance 1.8 in). Corrections: the "tempo" was the
  backswing angle; `Ball_Launch`'s fourth argument is the trajectory.
- Swing.c continued: `Swing_WaitForBackswing` (exact), `Swing_Begin`; the session byte at
  +0x13 is `bReplay` (set only when a saved replay loads), not a spin option. The swing starts
  at a quarter pull on either stick; a 25-sample ring of stick positions feeds the sampler
  (`0x8005934C`, read: dead zone 96..160). Swing.c: 20 functions in C.
- **The swing phases in C** (`gSwingPhaseFns`, 7 entries): backswing follows the stick with a
  chasing animation, the top is the furthest-back of the last 25 samples, holding at the top
  waggles the animation and a slow return to centre cancels the swing, impact is the animation's
  event with the stick's forward direction as the sample, spin is added after impact (1 a frame
  to 20). `fn_8006BF60` is the replay recorder (fresh seed at every human launch), not spin.
  Swing.c: 30 functions in C.
- Swing states: 43 of 67 callbacks in C (mostly camera/HUD glue; 28 exact so far in Swing.c).
  Finds: state 6 is the putt preview, done by launching the caddie's putt with the player
  briefly made a CPU; state 15 is where `Shot_Plan` runs for the next shot; lie 12 is "holed";
  `Player.ballBefore` (0xB5C) keeps the ball as it lay and doubles as the preview ghost.
- **The suggested shot is the rehearsal.** Swing state 15 runs `AI_RehearseShot` on every player,
  humans made CPUs for the call, while the camera flies in; the club and aim you are handed are
  its solution. Also: state 12 keeps the ball as it lies; `bRehearsalDone`/`bPlanReady` named.
