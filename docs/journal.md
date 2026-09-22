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
