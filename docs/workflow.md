Working on the code
===================

The match is complete, so work on the code is about how it reads: names, types, prototypes and
comments, under [`style.md`](style.md). Every change must keep the build byte-identical. All
commands run from the checkout root.

Build and score
---------------

```
ninja                                              # must end with: build/GW4E69/main.dol: OK
ninja build/GW4E69/report.json                     # objdiff's per-function scores
python tools/quality/unitfns.py main/<Unit> all    # a unit's functions under 100%
python tools/quality/sbs2.py main/<Unit> <fn>      # instruction diff against the original
python tools/quality/sbs2.py main/<Unit> <fn> sbs  # the same, side by side
```

objdiff's GUI works too: open the checkout as a project (`python configure.py` writes
`objdiff.json`).

`main.dol: OK` is the real check. objdiff scores by name and masks relocations, so a function
can score 100% while a constant holds the wrong value or a jump table points at the wrong cases;
the DOL comparison catches both.

Naming
------

```
python tools/naming/callgraph.py <Unit> [--unnamed]   # a unit's functions, leaves first, with
                                                      # callers, callees, globals and strings
python tools/naming/name.py <batch.tsv> --check       # validate a batch of function names
python tools/naming/name.py <batch.tsv> --by "<who>"  # apply it, rebuild, log the evidence
python tools/naming/rename.py <names.tsv>             # rename globals (symbols.txt and all uses)
```

`name.py`'s docstring describes the batch format: address, current name, new name, tier,
evidence codes, evidence, purpose and an optional comment. Every name lands in
`config/GW4E69/name_sources.tsv`. Name leaves first: a caller reads better once its callees have
names.

Before you commit
-----------------

```
ninja                                         # main.dol: OK
python tools/quality/lint.py --diff main      # style findings on the lines you changed
python tools/quality/typeaudit.py --count     # no number higher than on main
python tools/quality/symaudit.py --count      # no number higher than on main
python tools/quality/constcheck.py --count    # no number higher than on main
```

- `lint.py`: the rules in `style.md`, plus a compile with warnings for undefined behaviour.
- `typeaudit.py`: globals declared with different types in different files, types defined twice,
  and `extern`/`typedef` lines in `.c` files.
- `symaudit.py`: each global's declared size against `symbols.txt`. A global of 8 bytes or less
  lives in small data (reached through r13/r2); declaring it larger, or unsized
  (`extern u8 x[];`), changes every access.
- `constcheck.py`: the bytes each function's constants load, ours against the original's, with
  the expression that reproduces a mismatch (`1.0f / 72.0f`, not `0.0138889f`).

If the DOL check fails, `python tools/quality/doldiff.py orig/GW4E69/sys/main.dol
build/GW4E69/main.dol` prints the differing ranges.

Removing a fake match
---------------------

Replacing a `// fake match:` with what EA wrote usually means a function that no longer matches
while you search. The tools in `tools/matching/` score variants without guessing:

```
python tools/matching/perm_setup.py <Unit> <fn>        # snapshot the function into build/perm/<fn>
python tools/matching/declperm.py <Unit> src/<Unit>.c <fn> <N>   # every order of N declarations
python tools/matching/declclimb.py <Unit> <fn> <N>     # the same, one move at a time (large N)
python tools/matching/declsearch.py <Unit>:<fn>       # declaration-order search on all cores
python tools/matching/leversweep.py <Unit> <fn>        # combinations of the known levers
python tools/matching/permute.py <Unit> <fn>           # decomp-permuter, with a time limit
```

From a script, `trial.py` (`Trial(unit).trial(fn, old, [variants])`, about 15 s a variant, keeps
only an exact one) and `quicktrial.py` (`score(fn, src)`, under a second, on the perm snapshot)
try variants in bulk. Prefer an exact form close to the natural one: a reader should not wonder
why the declarations are shuffled.

To see *why* a register differs, `mwccdbg.py src/<Unit>.c <fn>` runs the compiler under
[mwcc-debugger](https://github.com/cadmic/mwcc-debugger) and dumps the PCode after each backend
pass and the register allocator's priority list into `build/mwccdbg/<fn>`. Setup, once per machine
(Linux or macOS; gdb, cmake, Rust), with both clones next to the repo or under `MWCCDBG_HOME`:

```
git clone https://github.com/cadmic/mwcc-debugger
git clone https://github.com/encounter/retrowin32 && cd retrowin32 && git checkout gdb-stub
cargo build -p retrowin32 -F x86-unicorn --profile lto
```

In a container without IPv6, change `localhost:9001` to `127.0.0.1:9001` in `mwcc_debugger.py`.
The debugger supports GC/2.6; `mwccdbg.py` swaps it in for our GC/2.5, which compiles every unit
identically. On a dump, `rasim.py` replays the register allocator and searches declaration orders
offline, and `sched750.py` replays the instruction scheduler when only the order inside a block
differs.

Data built from your disc
-------------------------

A few objects are Nintendo data or game assets whose bytes can't be committed: the AX library's
compressor table and DSP microcode (`AXComp.c`, `DSPCode.c`), DiscError.c's font, startUp.c's
boot sounds and two textures. `tools/build/gendata.py` reads them from your own
`orig/GW4E69/sys/main.dol` into `build/GW4E69/gen/` before compiling, using the addresses and sizes
in `symbols.txt`. To add one, add a line to its `FRAGMENTS` table and `#include` the generated
fragment from the unit that defines the object.

Rules
-----

- Every commit ends with `main.dol: OK`.
- No game files (the DOL, disc images, art, sound) and no official SDK files in git.
- Edit C and headers with an editor or a saved script, never with `sed` or heredocs.
