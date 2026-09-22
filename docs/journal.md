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

**Working agreements**
- Show the plain-English logic and the C before building anything non-trivial.
- One batch of work, then report. No silent fix loops.
- Commit only when asked. No AI attribution in commits.
- Write lessons into the docs the same day.
