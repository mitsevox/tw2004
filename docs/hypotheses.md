Gameplay hypotheses
===================

Predictions from thousands of hours of play, written down before the relevant code is decompiled
so the source can confirm or refute them. Each entry says what evidence would settle it and where
in the binary to look (addresses from [`filemap.md`](filemap.md)). Update the **Status** line when
something is learned; keep the original prediction as written.

1. Caddie putt tips are unreliable
----------------------------------

**Observation (2026-09-22):** the caddie's read for putts is genuinely not great in some situations,
including "straight in" reads that still lip out. Suspected: RNG noise scaled by the putting stat,
but it happened with putting maxed too. An open question rather than a prediction.

**What would settle it:** the code that produces the tip (aim point / break) versus the code that
runs the ball. If the tip is a simplified model (fewer green samples, coarser integration, no
green-speed or slope term the physics uses), the misreads are systematic, not random. If a random
term is added to the aim, it is deliberate. If the tip is exact but the ball sim adds noise on the
strike, the lip-outs come from the swing, not the read.

**Where to look:** `GoBreakLine.c` (`0x800C8094`; the on-green break line is the caddie read
rendered), `GoGreenGrid.c` (`0x8009B68C`; green elevation grid), `PsBallFx.c` / `PsMgr.c`
(`0x800A2C6C`; ball physics), `Swing.c` (`0x80058D18`), `CharSliders.c` (`0x8010D614`; golfer
attribute sliders, 20 assert references - likely where stats scale things).

**Refinement (2026-09-22):** the guess is a *simplified* physics model so the tip populates quickly.
Supporting observation: on long putts with a lot of break the caddie visibly takes time, and
sometimes reports the tip as **unavailable**. That means the tip is a search, not a closed-form
formula. Two ways a search says "unavailable": it exhausted its budget (an iteration cap or a
frame/time limit) before converging, or it found no aim at its coarsest step that reaches the hole.
The code will show which: look for a loop counter compared against a constant, or a time check,
versus a "no candidate passed" exit. Either way the search granularity (step size in aim angle and
power) is what makes the tip miss.

**Status:** open.

2. AI shots are solved backwards from the pin, then nerfed by skill
-------------------------------------------------------------------

**Prediction:** the AI computes its shot by working back from the target (the same solver as the
caddie tip), and then error is injected according to the golfer's attributes so the AI is not
perfect.

**What would settle it:** an AI shot routine that calls the same target-to-swing solver as the tip
code, followed by a perturbation of aim/power/spin that reads the AI golfer's attributes.
Alternative outcome: a forward search (try candidate swings, keep the best), or a table of canned
shots per club.

**Where to look:** whichever file owns the AI turn; candidates are not named in the leaked file
list, so start from callers of the tip solver once `GoBreakLine.c` / `Swing.c` are understood, and
from `CharSliders.c` / `EASportsBio.c` (`0x80124B5C`) for attribute reads.

**Status:** open.

3. No rubber-banding
--------------------

**Prediction:** unlike many games of the era, this one does not buff a trailing player or nerf a
leading one. Nothing in the outcome depends on the current score difference.

**What would settle it:** any read of score / hole standing / match state inside the shot,
physics, wind or AI-error code. Absence across all of those files would confirm the prediction.

**Where to look:** the AI error injection from hypothesis 2 (the obvious place to hide it), wind
generation, and the swing meter code in `Swing.c`.

**Status:** open.

4. Which attributes touch which math, and whether human and CPU are treated alike
---------------------------------------------------------------------------------

**Question:** the game's tooltips say what each attribute generally does (e.g. Luck: more
favourable lies). How does that turn into code, and is it the same code path for the human golfer
and for CPU golfers? A favourable lie is naturally golfer-agnostic; things like swing error,
putting read, or wind resistance are the grey area where the human and CPU paths might diverge.

**What would settle it:** for each attribute, the list of functions that read it and what they
multiply or threshold with it. Then, for each of those functions, whether it is reached from the
human input path (controller -> swing meter), the CPU path (AI shot solver), or both.

**Where to look:** `CharSliders.c` (`0x8010D614`, 20 assert references: the attribute sliders),
`EASportsBio.c` (`0x80124B5C`, golfer bio / stats), then the callers of whatever getters they
expose. Expect attributes to be read through a small set of accessor functions; naming those
accessors makes every use site searchable.

**Lead (2026-09-22):** the per-golfer attribute values are in `loadonce.gcb` as the `stat`
object (EA's `DATA\STATS_GC.BIN`): 34 golfers x two blocks of 12 values 0..100, plus the
created golfers' starting rows. See [`formats/game-data.md`](formats/game-data.md).

**Result (2026-09-22, from data, to be confirmed in code):** there are **twelve** attributes,
not nine. The front-end shows POWER, POWER BOOST, DRIVING ACCURACY, BALL STRIKING, APPROACH,
PUTTING, RECOVERY, SPIN, LUCK; a debug menu in the same file adds three hidden ones:
**AGGRESSION, IQ, SPEED**. Those are the CPU-golfer personality knobs this hypothesis was
looking for - and every record has them, including the created golfers (10, 10, 80), so the
human and CPU golfers share one record layout. Whether the game *reads* the hidden three for
the human golfer is the code question that remains. Each record also carries a second block of
twelve with a flatter profile (meaning unknown - the next thing to settle).

**Status:** open.

5. Is there hidden putting assistance (a "pull" toward the cup)?
-----------------------------------------------------------------

**Question (2026-09-22):** does the ball roll exactly where the player aimed and struck it, or is
there any assistance: a force or nudge toward the cup when the ball passes near it, a capture
radius larger than the physical hole, or an aim correction applied before the roll?

**What would settle it:** in the putt physics, any term that depends on the ball-to-cup vector
besides the geometric hole test (rim collision / drop-in). A steering force, a distance-scaled
velocity correction, or a capture check with a radius or speed threshold more generous than the
real hole would all count as assistance. A clean sim has only gravity, slope from the green grid,
rolling friction, and the hole collision. Also check whether any such term is gated on human vs
CPU or on an attribute.

**Where to look:** `PsBallFx.c` / `PsMgr.c` (`0x800A2C6C`, ball physics), `GoGreenGrid.c`
(`0x8009B68C`, slope), the hole/cup collision code (unnamed; find it from the constant for the
cup radius, 0.054 m or 2.125 in, or from what writes the "holed" state).

**Status:** open.

Facts already established that bear on these
---------------------------------------------

- **The game never calls the C library `rand()` / `srand()`.** Neither function is referenced by any
  of the 7,646 functions (only the Nintendo CARD library has its own LCG, for memory-card
  serials). So any randomness in gameplay is EA's own generator. Finding it (look for an LCG
  multiply, an `xorshift`, or a seeded table) and listing its callers will locate every random
  decision in the game at once: swing error, wind, AI error, crowd noise. That is the single most
  useful lead for all three hypotheses.
- The leaked file names contain no "AI", "CPU" or "opponent" file, so the AI logic lives in a file
  without asserts or under a name that does not say so (`user.c`? `PsMgr.c`?).
