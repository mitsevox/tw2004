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

**Result (2026-09-22, in C):** **a search with a budget, exactly as refined.** The tip is the
CPU's shot rehearsal (`AI_RehearseShot`) run on a copy of you in player slot 4: the real ball
physics, randomness off, 0.2 s of ball time per frame (twelve real ticks), aim moved 45% of the
miss each round until the simulated ball *stops within 1.8 in of the pin*. `Caddie_GetTip` gives up after 600 frames -
ten seconds - and that is "unavailable". Systematic, not random: it solves for stopping at the
hole rather than dropping, assuming a perfect stroke. Details in
[`gameplay.md`](gameplay.md).

**Status:** answered.

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

**Result (2026-09-22, revised):** **confirmed, with the mechanism.** The *target* comes from
a table of authored aim points per hole (`AI_ChooseTarget`), filtered against POWER, AGGRESSION
and the shot's skill. The *shot* to reach it is then solved by rehearsal (`AI_RehearseShot`):
the real physics simulated with randomness off, the aim corrected by 45% of the miss each
round until it lands. Only then does `AI_ApplyError` worsen the aim and distance by
`(100 - skill)` per shot type, with a random blunder roll on approach shots. IQ inflates the
CPU's idea of its own skill when choosing a target. Full detail in [`gameplay.md`](gameplay.md).

**Status:** answered.

3. No rubber-banding
--------------------

**Prediction:** unlike many games of the era, this one does not buff a trailing player or nerf a
leading one. Nothing in the outcome depends on the current score difference.

**What would settle it:** any read of score / hole standing / match state inside the shot,
physics, wind or AI-error code. Absence across all of those files would confirm the prediction.

**Where to look:** the AI error injection from hypothesis 2 (the obvious place to hide it), wind
generation, and the swing meter code in `Swing.c`.

**Result (2026-09-22):** **mostly right, with one exception.** The complete CPU shot pipeline,
the human forgiveness / power / boost / spin functions read only the player's own attributes,
the shot geometry, club tables and the RNG. But `Golfer_IsLucky` (`0x8002D994`) does read the
standing: **in game mode 4, when player 1 is more than 4 holes up on player 0, player 0's
lucky-bounce odds are doubled** (1 in 12 becomes 1 in 6, or 1 in 3 with LUCK 100). CPU golfers
never get lucky events at all, so this only ever helps the human. Mode 4 is almost certainly
World Tour (the one-on-one match-play mode), which is also where CPU pros use the flatter
second attribute block. So: no rubber band on the swing or the AI, one small deliberate one on
luck in match play. Wind generation is still unread.

**Revised (later the same day):** **refuted for the CPU.** `AI_SetShotModifiers` (`0x8002A630`,
exact in C) sets every CPU shot's attribute modifiers from its standing: +10 when it is one over
on the hole, +20 per stroke beyond, and in match play -5 per hole it leads. Plus +5 for every
failed rehearsal of the shot. The human's swing has no such term; the human's only score-linked
effect is the luck doubling above. So: no rubber band on you, a deliberate one on the CPU in
both directions.

**Wind (later the same day):** read. Generated from the hole's authored wind, else the session's
wind setting and the RNG, forced up on two courses; nothing about the score in it. One fixed
CPU leniency: a CPU's ball has each wind axis clamped to +-15 in flight, a human's takes it
all. See [`gameplay.md`](gameplay.md).

**Status:** answered - refuted for the CPU, upheld for the human swing.

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

**Result (2026-09-22):** there are **twelve** attributes, not nine. The front-end shows POWER,
POWER BOOST, DRIVING ACCURACY, BALL STRIKING, APPROACH, PUTTING, RECOVERY, SPIN, LUCK; a debug
menu adds three hidden ones, **AGGRESSION, IQ, SPEED**, and the attribute screen's own reads
confirm the column order. Every golfer record has all twelve, including the created golfers.

All reads go through one accessor, `Golfer_GetAttribute(pPlayer, k, mode)`, and the 87 call
sites are mapped in [`formats/game-data.md`](formats/game-data.md). What that map says so far:

- **AGGRESSION and IQ are read by exactly two functions** (`0x8002AA74`, `0x8002C2DC`), which
  also sample POWER / STRIKING / APPROACH / PUTTING / RECOVERY twice each. `Swing.c` never
  reads them. That is the AI's shot logic or nothing is; reading it settles hypothesis 2 too.
- **The pros have a second attribute block**, used instead of the first when the player is
  CPU-controlled (`Player_IsCPU`, `field_A08 == 9`, the same test the AI code gates on) and the
  game mode is 4. It is flatter and mostly lower. So in that mode a CPU pro plays with different
  numbers than the same pro played by a human.
- LUCK is read in three places outside the swing (two in the `UKernel.c` region), consistent
  with its tooltip (lies and bounces). SPEED is read only by skin/animation code.
- `CharSliders.c` turned out to be the create-a-golfer *face* slider loader, not attributes.
- **Human and CPU are different mechanisms, not different numbers.** A human's stick error is
  *reduced* by the governing attribute (`Swing_ApplyForgiveness`: a threshold and a scale from
  a table, e.g. at PUTTING 100 a miss under 0.436 shrinks to an eighth; at 0, no help). A CPU's
  perfect aim is *worsened* by `(100 - skill)` (`AI_ApplyError`); nothing reads an attribute on
  the CPU's swing. AGGRESSION and IQ are used only in target choice. LUCK, SPEED and POWER BOOST
  are read elsewhere (lies/bounces, animation, the human power boost). Tables and formulas in
  [`gameplay.md`](gameplay.md).

**Update (2026-09-22, Swing.c in C):** every human-side reader is now decompiled: forgiveness,
power, power boost, spin and the mis-hit rumble. Per attribute, on the human side: DRIVING
ACCURACY / BALL STRIKING / APPROACH / PUTTING / RECOVERY each shrink the stick error for their
shot type and (driving, recovery) reduce the distance lost to it; POWER BOOST doubles the boost
ceiling; SPIN scales the spin; POWER sets club reach (shared with the CPU). Two non-attribute
assists found on the way: putts over 75% on the meter count as full power, and a driver off the
tee gets up to +10% when the backswing is angled 36..54 degrees off vertical (the same
angle that shapes the shot - `Swing_FaceVector`).

**Update (LUCK):** read all three readers. LUCK is the odds of a lucky event (above, hypothesis
3), the chance of the good rough lie over the bad one (50% at 0, ~70% at 100), the chance of
escaping the worst lie on a bad surface (12.5% to 32%), and a kinder bounce term in the
collision code. Humans only: CPU golfers never roll for luck.

**Update (`Swing_MeterError` in C):** the human's raw error is the angle between the stick's
path back and its path through, with a random +-15 (of +-128) added to both x samples first:
up to ~1.7 degrees of wobble on a full shot, ~0.25 on a putt, before forgiveness. The one
random term in the human swing; not attribute-scaled.

**Update (the lucky event itself, `Luck_TakePerfectShot` in C):** the "lucky event" is not a
bounce. When the roll succeeds the game runs the CPU's shot rehearsal on a copy of you, aimed
at the pin, while you set up; at launch, if your club is within two of the rehearsed one, the
shot kind matches and your aim is within 5 degrees, **your shot is replaced by the rehearsed
one** and your swing error is zeroed. The human and CPU paths are not just different - the
human's best shots *are* the CPU's solver. Full account in [`gameplay.md`](gameplay.md).

**Update (`SwingState15_Update` in C):** the human's *default* shot is the CPU path too. While
the camera flies to the ball the game runs the CPU rehearsal on the human (controller set to
CPU for the call) and the suggested club and aim are its solution. So the human and CPU share
the solver twice: for the suggestion, and for a lucky shot.

**Status:** answered.

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

**Partial result (2026-09-22):** two assists exist *before* the roll, none found yet *during* it.
(1) `Swing_ApplyForgiveness` zeroes the stroke error for any putt under 2 units, before reading
the attribute; (2) above that, PUTTING shrinks a stroke error under 0.436 (at 100) to an eighth.
The CPU gets the mirror image: no error on putts under 1.5 units, angle error halved under 5.
The physics-side question (a pull toward the cup, capture radius) is still open.

**Result (2026-09-22, `Ball_CupPull` in C):** **yes, there is a pull.** Inside 5.5 in of the pin
(the physics is in yards; we first misread it as metres), while the ball is still short of the
hole, a ball heading within 30 degrees of the cup (or within 3.5 in regardless) gets `0.455 x dt x (pin - ball)` added to its velocity each frame - an
acceleration toward the cup - limited so it never speeds the ball up while it is more than
16.8 degrees off line. A ball crossing the cup fast and off line loses up to 67% of its speed
instead (the lip). The hole itself is geometry: holed means "dropped more than 2 in below the
pin height" on a cup surface, no capture radius. The pull applies to every ball, human or CPU,
with no attribute involved. Full numbers in [`gameplay.md`](gameplay.md).

**Status:** answered.

6. The CPU's putt misses come from break, not distance
-------------------------------------------------------

**Prediction (2026-09-22):** how likely a CPU golfer is to make a putt has nothing to do with the
distance to the cup. The break decides it: more break, more misses; distance is not factored in
at all. Probably not coded that way on purpose. It could fall out of how the AI "pulls back"
from its perfect rehearsed putt: a breaking putt leaves more room for the error to push the
ball off line.

**What would settle it:** the CPU putt pipeline end to end. What the error is applied to (the aim
angle, the strike power, or the target point), whether any term reads the distance or the slope,
and how that error becomes a miss once the ball rolls over a sloped green. A test harness could
then measure it: the same error on a flat 10-footer and on a flat 30-footer, then on breaking
putts of the same length, run through the real rehearsal and ball code.

**Leads already known:** `AI_ApplyError` (in C) does read the distance for putts: no error at all
under 1.5 units, the angle error halved under 5, above that up to 8 degrees of aim and 20% / 10%
of distance, scaled by `(100 - PUTTING)`. The shot itself comes from `AI_RehearseShot`, which
solves for the ball *stopping* within 1.8 in of the pin, and `Ball_CupPull` pulls in any ball
within 5.5 in of the pin that is heading within 30 degrees of the cup. So the question is how
those combine on a real green: does the pull swallow the error on straight putts, and does an
aim or pace error on a breaking putt move the ball's line more than the same error does on a
straight one?

**Where to look:** `AI_ApplyError` (`Golfer.c`), `AI_RehearseShot`, `Ball_CupPull`, the green slope
code (`GoGreenGrid.c`, `0x8009B68C`) and the putt part of the ball physics.

**Status:** open.

7. Power boost's full meter needs a non-linear number of Z presses
-------------------------------------------------------------------

**Prediction (2026-09-22):** a full power boost (the whole ball on the meter filled red) is very
hard to reach because the number of Z presses needed grows faster than linearly: exponential, or
some other curve that is not a straight line.

**What would settle it:** the code that turns Z presses into a boost level: whether each press adds
a fixed amount, adds less as the level rises, decays over time, or needs faster presses at higher
levels. Also the timing window (is there one?) and how the level shown on the ball maps to the
level used.

**Leads already known:** `Swing_ApplyPowerBoost` (in C) applies the *result*: levels 1..8 add
steps 1 2 4 6 9 12 16 20, times 0.005 to 0.011 by the POWER BOOST attribute. So the *effect* of
the levels already grows faster than linearly. The open part is the *input* side, which turns
presses into a level.

**Where to look:** `Swing.c` (the swing phases and state machine; search for the Z button mask and
for writes of the boost level), and the meter drawing code that fills the ball.

**Status:** open.

8. The heartbeat comes from a quick look-ahead simulation after the strike
-----------------------------------------------------------------------------

**Prediction (2026-09-22):** the heartbeat effect (the tense "this could go in" moment) is decided
by a separate, sped-up physics simulation run right after the ball is struck. If the simulated
ball finishes close enough to the pin, the heartbeat starts shortly after the ball takes off.

**What would settle it:** what starts the heartbeat sound or effect, and what that code checks.
Look for a call into the ball physics on a copy of the ball (like the rehearsal's `gSimBall`)
made at launch, and a distance-to-pin threshold. The alternative is a check on the *live*
ball during flight or roll (its predicted landing point, or its distance each frame).

**Leads already known:** the game already has a look-ahead sim, the CPU rehearsal
(`AI_RehearseShot`, on `gSimBall` with sounds and effects off). Whether the heartbeat reuses it,
runs another, or just watches the real ball is the question.

**Where to look:** the sound or effect trigger for the heartbeat (find the sample name in the audio
banks or a string in `main.dol`), then its callers. Also check the swing states after launch
(`Swing.c`) and `Ball.c`.

**Status:** open.

Facts already established that bear on these
---------------------------------------------

- **The game never calls the C library `rand()` / `srand()`.** EA's own generator is
  `Rand_Next(stream)` (`0x8000B130`), an additive lagged-Fibonacci generator with 20 words of
  state per stream, and `Rand_Float` (`0x8000B428`) for [0, 1). Found 2026-09-22 through the AI
  error code; listing its callers locates every random decision in the game.
- The leaked file names contain no "AI", "CPU" or "opponent" file, so the AI logic lives in a file
  without asserts or under a name that does not say so (`user.c`? `PsMgr.c`?).
