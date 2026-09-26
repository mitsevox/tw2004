# Dead asserts on GC/2.5 -O4,p (r6-assert, 2026-09-26)

Question (#match-help, owner-approved lane): do EA's compiled-away asserts and debug code still
shape our compiler's output (register neighbours, frame size, what the frontend keeps)?

## What the binary says about EA's macro

- The only assert expression strings in main.dol belong to the save-file library
  (`NULL != pFilename`, `CARD_RESULT_READY == uErr`, ...: SFIO_ASSERT in
  include/Common/SharedFileIO.h, a live assert). No game-code assert text and no `__FILE__` string
  that is not an allocator tag (`"hwsBurn.c"`, `"skalib.c"`, `"char.c"` are all arguments of the
  allocator fn_80009B34). So EA's game asserts left no strings, and no call remains.
- A dead string never reaches the object in any form tested (`if (0) AssertLog(#x, __FILE__, ..)`
  and a `static const` 0 flag emit no string), so the missing strings do not tell the forms apart.
- TW07's cu files show AssertMacroHelper / AssertLog inlined only in 14 C++ files of the later
  engine (Frustum, LLPoolAllocator, twmodelrenderobj ...), none of this lane's targets' files
  (char.c, Swing.c, Earnings.c, skalib.c, Golfer, SkinPart, hwsBurn, Grass): no per-function
  evidence of where EA asserted in them.

## What each form compiles to (measured)

Small test files compiled with the unit flags (scratch `frame/t*.c`), plus every form inserted
after every statement of hwsBurn fn_8011172C and read with mwccdbg (vreg list, neighbour counts):

| form | code | backend vregs / neighbours | frame | notes |
|---|---|---|---|---|
| `((void)0)`, empty macro | nothing | unchanged | unchanged | |
| `((void)(x))` | nothing | unchanged | unchanged | |
| `do { if (0) { if (!(x)) AssertLog(#x, __FILE__, __LINE__); } } while (0)` | nothing | unchanged | unchanged | frontend @-temp counter moves (+8) |
| `static const int kDebug = 0; if (kDebug) {..}`, local `const`/`int` 0 flag | nothing | unchanged | unchanged | |
| `if (!(x)) { }`, `if (x) { }`, `do { if (!(x)) {} } while (0)` | nothing | unchanged | unchanged | @-counter moves |
| `(x) ? (void)0 : (void)0`, `(void)sizeof(x)`, `0 && ...` | nothing | unchanged | unchanged | |
| empty `static inline void A(int c) {}` / with if-else inside | nothing | unchanged | unchanged | @-counter moves |
| **`if (x) { } else { }`, `if (x) ; else ;`, `while (x) { break; }`, `for (; x; ) { break; }`** | **no instructions, but a trace** | changes | unchanged | see below |

- **No form extends a live range**: a variable last used before a call and asserted after it
  stays in a volatile register (t5.c, 9 forms). A dead assert does not add neighbours.
- **No dead buffer takes stack**: an unused `char dbg[64]`, one written but never read, one used
  only in `if (0)` / `while (0)` / a `static const` 0 flag / `goto`-skipped code / an empty inline,
  an unused 32-byte struct: every frame is the same as without it (t.c, t2.c). An address taken
  only in dead code does not force a variable to the stack (t3.c).
- **An empty two-armed branch is the one trace**: the condition's diamond survives the frontend and
  acts as a block boundary. `m = a - 1; e = h(n) + a; if ((a & m) == 0) {} else {}; e = (e-1) & ~m;`
  computes m before the call in a saved register (20 instructions); without it, or with any
  one-armed / `(void)` / `if (0)` form, m is computed after the call (18) (t7.c). The effect does
  not depend on the variable tested (SW_KillVibration: every condition, every position after the
  pointer setup gives the same code, 4 -> 7: pFrames is then kept past the frontend as EA has it,
  but its registers differ). Only a macro that expands to if/else (or a loop with break) would do
  this; nothing shows EA's did.
- Plain `static` helpers are not auto-inlined under `-inline smart` with or without a dead assert
  body, and `inline` ones are inlined either way (t4.c): dead code does not change inlining.

## Applied to the lane's targets

The if/else form at every statement boundary (asweep.py, condition = the first parameter), and for
hwsBurn fn_8011172C and SW_KillVibration every form above with several conditions, declaration
and statement orders: no function improved (fn_8011172C 1, fn_80017DDC 1, fn_8001F110 2,
Character_SetupForShot 6, fn_800CE52C 5, fn_800CE224 19, fn_80019798 28, fn_800D4F14 14,
fn_800D477C 19, Session_SetupProfiles 14, fn_800E8A68 14, fn_801264B8 26, AI_ChooseTarget 91,
SW_KillVibration 4: all unchanged at best, aligned diffs).

Verdict: on this compiler EA's retail asserts are invisible unless the macro left an empty
if/else; the neighbour, frame and "used more" effects described in #match-help do not occur here.
The register-order and frame misses in the ledgers need other causes.
