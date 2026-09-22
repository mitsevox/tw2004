Gameplay logic, read from the code
==================================

What the shot code actually does, in plain English with the numbers. Read from the disassembly
on 2026-09-22 (functions named in `config/GW4E69/symbols.txt`). The golfer/AI file is being
decompiled as `src/Golfer.c`: the accessor and its helpers match, `AI_ApplyError` and
`AI_ChooseTarget` compile to the same instructions with only register numbers differing, so
the arithmetic below is confirmed, not just read.
Units: distances compare against constants like 1.5, 5, 50, 100, 200, which fit yards for full
shots and feet for putts; the code does not say, so they are called "units" below.

The pieces
----------

| Function | Address | Role |
|----------|---------|------|
| `Golfer_GetAttribute(p, k, mode)` | `0x8002E15C` | every attribute read (see `formats/game-data.md`) |
| `Player_IsCPU(i)` | `0x8002E760` | `gPlayers[i].field_A08 == 9` |
| `Shot_GoverningAttribute(i, club, lie, kind)` | `0x8002C8F4` | putter (club 25) -> PUTTING; rough/sand lie -> RECOVERY; full swing with a club under 13 from a good lie -> BALL STRIKING; else APPROACH |
| `AI_ChooseTarget(i)` | `0x8002C2DC` | picks where the CPU aims |
| `AI_ApplyError(i)` | `0x8002B59C` | perturbs the CPU's aim and distance by its attributes |
| `AI_PlanShot(i, target)` | `0x8002BDEC` | turns a target into club / power |
| `Swing_ApplyForgiveness(i)` | `0x8005C5EC` | shrinks a human's swing error by attribute |
| `Swing_ComputePower(i)` | `0x8005B250` | final power for either kind of player |
| `Rand_Next(stream)` / `Rand_Float(stream)` | `0x8000B130` / `0x8000B428` | EA's RNG (below) |
| `gPlayers` | `0x801C66E8` | 5 x 0xEF8 player structs |
| `gAITargets` | `0x801C65B8` | table of authored aim points, 12 bytes each |
| `gForgivenessTable` | `0x80188168` | 27 rows x 3 floats (value at attribute 0 / 100 / 110) |

Human swing: starting it (`Swing_WaitForBackswing`, `Swing_Begin`, in C)
--------------------------------------------------------------------------

Before the backswing the game polls both sticks every frame and holds their rest positions at
128. **The swing starts the frame either stick is pulled past 160 of 255** - a quarter of its
travel - and that stick's rest position becomes the centre sample; the C-stick works as a
second swing stick (`bUsingCStick`). `Swing_Begin` then starts the animation, reads its three
timing marks, and fills a **25-sample ring of stick positions** with the centre; the top and
impact samples are picked out of that ring as the swing goes on (`0x8005934C`, read, not yet
in C). That update applies a **dead zone of 96..160** (about +-32 around the 128 centre) to
both stick axes every frame: forward of it the reading maps smoothly down to 1, back of it the
reading jumps to ~179 and runs to 255 - so the first frame past the gate already reads a third
of the way back. A CPU or a replay starts the swing immediately.

The swing states (`gSwingStates`, 27 x enter/update/exit; 43 of 67 in C)
--------------------------------------------------------------------------

Around the swing itself sits a state machine on a per-player stack (`SwingStack_*`). What the
states do, from their code:

    1   the swing (the phase table below runs inside it); leaving it puts the ball back
    2   a CPU thinks (its rehearsal); a human goes straight to 10
    3   aiming (camera 1, camera 2 on a putt); the caddie updates here
    4-7 setup sub-states: cameras 3/4/6, button polling; the caddie updates in 4 and 6
    6   the putt preview: the caddie's solved putt is launched as a ghost (below)
    8   camera 7, then back to 12 or 0
    10  shot setup: starts the caddie, HUD, sounds
    11  the swing animation plays; Swing_Launch at its impact frame
    12  the ball is away
    13, 14  the ball has come to rest (a copy is kept as "before the shot")
    15  plan the next shot from where the ball lies (Shot_Plan)
    16  a second setup state: animation 11, flag 8, a distance to the pin stored
    18  holed out: the ball goes to the pin, lie 12 (LIE_HOLED), animation 12
    19-23  camera states (saved-camera restore, flyovers, camera 25)

**Your suggested shot is the CPU's solution** (state 15, `SwingState15_Update`, in C). When
the camera flies to your ball for the next shot, `Shot_Plan` has just picked an authored aim
point for you (the same table the CPU uses), and every frame of the flight the game **runs
`AI_RehearseShot` on you - your controller set to the CPU for the call, in fast mode (six ticks
a frame)** - until it settles. The rehearsal writes its solved aim and the CPU's club choice
straight into your player. So the club and aim marker you are handed when you arrive at the
ball are not a rule of thumb: they are the CPU's simulated answer for that aim point, which is
why "just hit what it gives you" works as well as it does. If the camera arrives before the
rehearsal settles, a "still working" call is made and you get whatever it had.

**The putt preview** (state 6, `SwingState06_Enter`, in C) is the one with a trick in it: it
takes the caddie's solved putt (`Caddie_ApplyTip`), **sets the player's controller to the CPU
for one call of `Swing_Launch`** - so the launch has no swing error and no luck swap - keeps the
launched ball as a ghost in the "ball before the shot" slot with no owner, and then restores the
player's own club, power, aim, ball and controller. The preview you watch is a real shot played
by the CPU code with your caddie's answer.

Human swing: the phases (`gSwingPhaseFns`, all in C)
------------------------------------------------------

`SwingData.nPhase` drives a table of seven per-frame functions:

    0  Swing_WaitForBackswing  poll both sticks; start at a quarter pull
    1  Swing_UpdateBackswing   the backswing follows the stick
    2  Swing_UpdateAtTop       holding at the top
    3  Swing_UpdateDownswing   the stick's forward reading becomes the impact sample
    4  (idle)
    5  Swing_UpdateAfterImpact rumble countdown and spin input while the ball flies
    6  (idle)

**Backswing (1).** Each frame the stick's dead-zoned magnitude (0..100) says how far along
the backswing animation should be, and the animation chases that at a rate
`min(1, (1 + |gap| / range)^2 - 1)`, scaled per shot kind (0.85 full, 0.5 chip, 0.8 pitch); a
pull past 93 runs the animation at full rate. Every frame's sample goes into the 25-sample
ring and is provisionally the top. When the animation has caught up (gap under 0.05 of
animation time) it is phase 2; if the stick backs down first, a flag makes the animation
back down with it. The moment the stick comes forward of 96 (with the backswing at least 0.1
along) **the top is the furthest-back sample in the ring** - not the last one - the downswing
animation starts, and the impact sample is seeded from that frame. A CPU plays the backswing
to 98% of the way to the top (65% in lesson 5) and swings down.

**Power** (`Swing_UpdatePower`, every backswing frame): a human's power is the **square root**
of how far along the backswing is - half a backswing is 71% power - snapping to 100% within
3% of the top (a CPU's is linear). **Holding at the top of a full backswing costs power**: on
anything but a putt, `(hold - 0.05)^2` comes off, at most 0.3 - so a 0.6 s pause is -30%.
`Swing_ComputePower` then applies boost, the error's cost and the tee bonus.

**Power boost** (`Swing_BoostInput`, every backswing frame): with the boost option on, a boost
button held while the stick is pulled past 93 of its range adds one level **per frame**, to a
maximum of 8 - about an eighth of a second of holding at the top. Backing the stick down for
1/12 s clears it. `Swing_ApplyPowerBoost` turns the level into power through the POWER BOOST
attribute.

**At the top (2).** The animation waggles +-0.0076 either side of the top while the stick is
steady; any change in the stick's y drops back to phase 1. If the stick sits near centre
(y at or below 160, x within 64..192) for over 0.1 s the swing is **abandoned**: the address
animation plays again and sound 9 fires. That is the "let it back down slowly to cancel".

**Downswing (3).** Every frame the stick is forward of 96 and more than ~17 units from the
centre sample, that reading becomes the impact sample. The ball goes when the *animation*
reports impact - so what counts on the way through is the stick's direction, not its timing.
Then `Swing_Launch`, the mis-hit rumble on a pad, the putt sound.

**After impact (5).** With the spin option on and the spin button held after a real shot, the
spin amount grows by one a frame up to 20 (a third of a second for full spin) and its
direction is the stick whenever it leaves the dead zone; the first press starts at straight
back (255). `Swing_ApplySpin` then scales it by the SPIN attribute every frame.

**Replays.** At every human launch (`0x8006BF60`, read) the game draws a fresh RNG seed,
reseeds with it and snapshots the player, profile and a block of globals into the replay
buffer; a replay reseeds from that and skips the luck swap and spin. That is why a replay
reproduces the shot exactly, bounces and all.

Human swing: the miss itself (`Swing_MeterError`, `0x8005BA94`, in C)
-------------------------------------------------------------------------

The raw error is the analog stick's path. Three stick samples are kept (each -128..127): where
the swing started (centre), the top of the backswing, and impact. The path back is
`(centre - top)` and the path through is `(impact - centre)`, each with its x scaled by **0.2
(0.03 on a putt)** and then normalised; the error is `atan` of the sideways deviation between
the two unit directions, clamped to the meter's maximum (`gpSwing + 0x114`). Straight back and
straight through is zero.

**Both x samples get a random +-15 added first** (`Rand_Float x 30 - 15`) - on a +-128 stick
that is up to ~1.7 degrees of wobble on a full shot after the 0.2 scale, ~0.25 degrees on a putt.
That is the only random term in the human swing, and forgiveness (next) then shrinks whatever
is left. A CPU or a perfect shot never gets here: `Swing_Launch` stores an error of 0 instead.

At launch the ball's direction is the aim plus the angle of the clubface vector (spin input)
plus this error.

Human swing: forgiveness (`Swing_ApplyForgiveness`)
----------------------------------------------------

Returns at once for a CPU player. For a human, the raw swing error `e` (from the stick, at
`player+0x410`) is compared against a threshold `T`; if `|e| < T` it becomes `e * S`. `T` and
`S` come from `gForgivenessTable`, linearly interpolated by the governing attribute: rows 0/1
for a 0..100 value, and a second segment between the 100 and 110 columns for 101..110.

    governing attribute (rows)               T @0    @100   @110    S @0    @100   @110
    DRIVING ACCURACY  (clubs 0-8)            0.100  0.415  0.435   0.850  0.180  0.165
    BALL STRIKING     (clubs 9-12)           0.125  0.420  0.445   0.825  0.175  0.145
    BALL STRIKING     (clubs 13-16)          0.130  0.425  0.465   0.800  0.170  0.125
    BALL STRIKING     (clubs 17-24)          0.135  0.430  0.485   0.775  0.165  0.115
    RECOVERY          (rough/sand, kinds 5-7) 0.100 0.415  0.524   0.850  0.200  0.150
    APPROACH          (kinds 2 and 3)        0.100  0.415  0.524   0.850  0.180  0.100
    PUTTING           (kind 0)               0.000  0.436  0.524   1.000  0.125  0.100

So an attribute of 0 gives almost nothing (only errors under 0.10 are touched, and only by 15%),
100 turns any miss under ~0.42 into an 82% smaller miss, and 110 goes a little further. Putting
at 0 has no forgiveness at all; at 100 a miss under 0.436 shrinks to an eighth.

**Putts shorter than 2 units have their error set to zero, unconditionally** (the check is on
`player+0xA54`, the putt distance, before any attribute is read).

Human swing: power (`Swing_ComputePower`, human branch)
-------------------------------------------------------

Power boost is applied (below), then the swing error also costs distance: rows 8/9
(DRIVING ACCURACY) and 12/13 (RECOVERY) of the same table give a threshold and a scale, and the
power loses `S * |e|` below the threshold and the full `|e|` above it. At attribute 100 the scale
is 0.125. Putting and approach shots read their attribute and discard it: no distance penalty
there. Result clamped to 0.05..1.5.

Two more things in the same function: **a putt meter over 75% is treated as 100%**
(`gpSwing->fPuttFullPower`), and a **driver from the tee gets up to +10% power**
(`Swing_TeeSweetSpot`) when the *backswing's sideways angle* is between 0.4 and 0.6 of a
quarter turn - **36 to 54 degrees off vertical, peaking at 45** - on the side that reads
negative. We first took the value for a tempo; it is the angle the face code below stores.
Both are tuning values set in `Swing_Init`, not attributes.

Human swing: draw and fade (`Swing_FaceVector`, `Swing_CurveAngle`, in C)
--------------------------------------------------------------------------

The shot's curve comes from the same three stick samples. On a full shot the angle of the
backswing off vertical, `atan((top.x - centre.x) / (top.y - centre.y))`, is kept
(`SwingData.fBackAngle`) and turned into a clubface angle: the angle as a fraction of a quarter
turn goes through a three-piece response curve (knots at 0.4 and 0.6, tuning values), is scaled
by **the club's shaping range** (`gClubCurve[club] / 26` between two tuning values - some clubs
curve more than others) and by a quarter turn, and keeps the backswing's sign. The face vector
`(-sin, 0, cos)` is launch block A; `Swing_Launch` adds its angle to the aim. On a putt there is
no curve: the face is the stick's sideways offset as a plain proportion (x 0.03).

So the driver's tee bonus and the biggest curve live on the same input: a backswing at 45
degrees is both the +10% power and a strong draw or fade. A CPU or a perfect shot gets a square
face, and its shape (draw/fade/punch/lob from the aim point) rides in launch block B instead
(`Swing_ShapeVector` -> `AI_FaceVector`).

The launch itself (`Swing_Launch`, in C) runs from swing state 11 once the swing animation
reaches its impact frame. In a replay (session byte 0x13, set when a saved shot is loaded) the
RNG is reseeded from the replay, player 0's swing state is restored, and the lucky-shot swap and
spin are skipped - which is how a replay reproduces the shot exactly.

Human swing: power boost, spin, rumble
--------------------------------------

- **Power boost** (`Swing_ApplyPowerBoost`): `power += scale * step[level]`, steps 1 2 4 6 9 12
  16 20 for levels 1..8, `scale` = table row 24 by POWER BOOST: 0.005 at 0, 0.010 at 100, 0.011
  at 110. So a full boost is +10% at POWER BOOST 0 and +20% at 100. It reads the *base* value
  (record plus equipment); the per-player modifiers do not count here.
- **Spin** (`Swing_ApplySpin`): stick deflection (-1..1) x (amount asked for / 20) x row 26 by
  SPIN: 0.15 at 0, 0.6 at 100, 1.0 at 110. Linear in the attribute: SPIN 0 gives a quarter of
  SPIN 100, and 110 gives two thirds more than 100.
- **Rumble** (`Swing_MisHitRumble`): frames = row 25 (135 at attribute 0, 35 at 100, 30 at
  110) x |error|, capped at 30, then the pad rumbles. A weak golfer's mistakes shake the pad
  about four times longer than a pro's.

CPU: choosing a target (`AI_ChooseTarget`)
------------------------------------------

The CPU does **not** solve backwards from the pin. Each hole has authored aim points
(`gAITargets`: position, enable flag, tee-set and hole filters, a "priority" flag, a type, and
three requirement bytes). The ball's current zone lists up to ten candidates. For each one:

1. skip if disabled, wrong tee set, wrong hole;
2. if nothing is chosen yet and the point is flagged priority, take it;
3. **humans stop here** - the same function gives a human player only the priority point
   (that is the default aim you get when you walk up to the ball);
4. for a CPU: distance to the point -> club -> shot kind -> governing attribute -> `skill`;
5. **IQ makes the CPU overconfident**: `skill += (100 - IQ)^2 * 40 / 10000` (or `* 10` when a
   per-player flag is set), capped at 100. IQ 10 adds 32 points of imagined skill, IQ 50 adds 10,
   IQ 100 adds nothing;
6. the point's three requirement bytes are checked against `skill`, AGGRESSION and POWER
   (a negative requirement means "at most");
7. the point must be within the club's maximum carry for this golfer (reads POWER);
8. among survivors prefer: priority flag, then the highest POWER requirement, then the highest
   AGGRESSION requirement, then the highest skill requirement, then the one closest to the pin.

Afterwards, if the ball is already closer to the pin than the chosen point, fall back to the
default aim. So AGGRESSION and IQ do exactly one thing each: AGGRESSION unlocks more demanding
aim points, IQ (low) makes the golfer qualify for points it should not.

CPU: applying error (`AI_ApplyError`)
-------------------------------------

After the target is chosen. Skipped when the shot is under 1 unit. Per governing attribute the
maximum aim error (degrees), and two distance-error percentages `D1`, `D2`:

    PUTTING    8 deg / 20% / 10%;  under 1.5 units: no error at all;  under 5: angle halved
    RECOVERY   over 200 units: 2.5 / 6.25 / 2.5;  over 100: 5 / 12.5 / 5;  else 10 / 25 / 10
    STRIKING   2.75 / 4.5 / 1;  under 150 units the angle and D2 are doubled
    APPROACH   over 100: 3.75 / 7.5 / 5.25;  over 50: 4.25 / 8.5 / 5.95;  else 5 / 10 / 7

Then `miss = 100 - skill` (skill capped at 98; 100 means no error; floor 15), and:

    aim   += maxAngle * rand() * miss / 100          (minimum 0.25 deg, random sign)
    dist  *= 1 + (D1 * rand() + D2 * rand()) * miss / 100 / 100    (random sign, one
                                                                    shot kind always short)

**Approach-shot blunders:** with skill over 80, a 1-in-20 roll multiplies all three error
limits by 4 and quarters the skill for this shot, otherwise a 1-in-10 roll doubles them and
halves the skill; with skill over 60 the 1-in-20 roll doubles them. Only approach shots roll
this. This is the "even pros shank one" mechanic.

Finally the CPU sets its spin input in proportion to the error it just gave itself (scaled by
its SPIN attribute and clamped to +-1), re-plans the shot for the perturbed target, and the CPU
branch of `Swing_ComputePower` just takes that power (putts +5%). **No attribute is read on the
CPU's swing itself.** Human and CPU are therefore different mechanisms: the human's stick error
is *reduced* by attributes; the CPU's perfect aim is *worsened* by them.

Luck
----

`Golfer_IsLucky(player)` (`0x8002D994`) decides whether a shot gets a lucky event. **CPU golfers
never do** (early exit), and split-screen play turns it off. The odds start at **1 in 12** for every
player (`gLuckOdds`, reset by `Luck_ResetAllOdds` at the start of a round) and **tighten by one
at each hole transition until 1 in 10** (`Luck_TightenOdds`, from the game state machine): 1 in
12 on the first hole, 1 in 11 on the second, 1 in 10 from the third. Then:

- LUCK cuts the "12" by LUCK/2 percent: LUCK 100 makes it 1 in 6 (110 rounds to the same);
- **in game mode 4, when player 1 has won more than 4 holes more than player 0, the odds are
  halved again** (1 in 3 at LUCK 100); the same halving applies on one flagged hole;
- it only fires off the green, never on a putt, and only for a pitch, a shot from lie 1 or 2
  under 250 yards, or on a par 3; then `Rand_Next(0) % odds == 0` is the roll.

The roll is made once, when the shot is planned (`Shot_Plan`), and its result is
`Player.bPerfect`. What that flag does turned out to be the best-hidden mechanic in the game:

**A lucky shot is the CPU's rehearsal, played for you** (`Luck_TakePerfectShot`,
`0x8002DE14`, in C). The moment the roll succeeds, `Caddie_Start` - the same code that runs the
putt tip - copies you into player slot 4, marks the copy a CPU, **aims it at the pin**, and
starts rehearsing (`AI_RehearseShot`, twelve physics ticks a frame) while you line up your
shot. The rehearsal converges when its ball stops within 9 inches of the pin (0.0625 yd
squared in `Caddie_Update`'s perfect branch). Then, at the instant your swing launches
(`Swing_Launch`, `0x8005AEE0`), the game checks that you were playing roughly the shot it
solved:

- your club within **two** of the rehearsed club;
- the **same shot kind** (full, chip, pitch...);
- your aim within **5 degrees** of the rehearsed aim.

If so, and the rehearsal had converged, **your club, trajectory, shot kind, power and aim are
silently replaced by the rehearsed ones** - a shot the physics has already shown lands at the
pin - your swing error is zeroed and forgiveness skipped (`Swing.c`), the tree-deflection roll
is skipped, and your luck odds go back to 1 in 12. If you were playing something else (a
different club, a lay-up, a punch), the flag is cleared and the shot is ordinary. If the
rehearsal had not converged in time, the flag stays but nothing is swapped - you just get the
error-free swing.

So "Luck: increases % of favorable lies and bounces" understates it. The favourable lie and the
kind bounce are real (below), but the main event is a whole shot handed to you, disguised as a
great swing. It cannot happen on the green, on a putt, or from the deep stuff - only on par 3s,
pitches, and shots from lie 1 or 2 inside 250 yards - and **split-screen play turns the whole
thing off** (the session byte we first took for a "no luck" option is the split-screen mode;
the caddie is off in split screen too). A CPU never gets one.

What the event does is in the lie code (`0x80053594`, read, not decompiled): landing in the
rough is a coin flip between the good rough lie and the bad one, and `(roll & 127) < LUCK/2`
forces the good one - 50% good at LUCK 0, ~70% at 100. On a worse surface, `(LUCK/4 + 16)/128`
is the chance of getting the rough treatment instead of the worst lie (12.5% at 0, 32% at 100).
And in the collision code (`0x80052598`, read) a bounce-direction term has `0.005 x LUCK`
subtracted, floored at -1: the kinder deflection off trees the tooltip promises.

The cup
-------

The ball physics (`src/Ball.c`, our name; `0x80050C2C`-`0x8005620C`) works in **yards**, like
the rest of the game - we first read it as metres, but every constant in it is a whole number of
inches (1/36). The ball it rolls has radius 0.92 in (a real one is 0.84); the cup constant is
3.86 in (a real cup is 4.25). The cup is **real geometry**: it is a depression in the green mesh
with its own surface kinds (12 and 18, type 90), and `Ball_GroundContact` (`0x80054D28`)
declares the ball holed when it is on one of those and more than **2 inches** below the pin's
height. No speed test, no capture radius: the ball has to physically fall in. `Ball_Holed` then
parks it 3 in down in the cup.

**There is a pull, though.** `Ball_CupPull` (`0x80054AB0`, in C at the original instruction
count) runs from the ground-contact step every frame the ball is on the ground, for every
ball, human or CPU, gated only by a debug flag pair:

- inside **5.5 inches** of the pin (under three cup radii), on a putt that started at least 6 in
  away, and only while the ball is still short of the hole along its path from where it started;
- take the angle between the ball's heading and the direction to the cup;
- **within 2.25 in** (over the cup): if heading more than 30 degrees off and faster than
  1.1 ft/s, the horizontal velocity is scaled by `1 - 16 x 0.67 x distance` - up to a 67% loss.
  That is the lip; a slow or straight ball is left alone to drop;
- otherwise, if heading **within 30 degrees** of the cup, or **within 3.5 in** regardless:
  `velocity += 0.455 x dt x (pin - ball)` on x and z - an acceleration toward the cup
  proportional to the offset - except on any axis where that would speed the ball up while it
  is more than 16.8 degrees off line.

How much is it? At 4 in off the line and rolling at a foot per second the ball spends about
half a second in the zone and picks up under an inch per second sideways: a few degrees of
bend, an inch or so at the cup. Enough to turn a lip-out into a drop, not enough to save a putt that
was never close. No attribute and no human/CPU test anywhere in it.

Two other things in the same file, read but not decompiled: hitting a tree (surface kind 17)
deflects the ball by a random 12..19 degrees in two axes unless the player is flagged perfect
(`0x800539F8`), and out of bounds is 600 m from the shot's start (`0x80054450`).

Gimmes and the pool-cue tap-in
------------------------------

**When** (`fn_800E2810`, from swing state 14 once the ball has stopped): the gimme option is on
(options byte 5, default on), not a replay, two mode flags clear, the ball within **0.5 yd (18 in)**
of the pin (`fn_800D0478`), and either the club is the putter or it is a one-player game. Then
state 15 (the CPU rehearsal runs on the player until it settles) and state 16: animation 11,
camera 12, and `Swing_Launch` with the controller set to the CPU for the call.

**Which animation** (animation 11 -> `fn_800965DC`): a style from the score the tap-in will give
(`fn_800D0AA0` = strokes + 1 - par: under par 6, par 5, over 2) is stored on the golfer, then
clip group 9 is looked up in the golfer's animation library (`fn_800176E8` -> `fn_800258B4` ->
`fn_80025640`: group, style, club class, a fourth key; each level falls back to a default). The
leaf's clips are picked **at random** (`Rand_Next(1) % n`, up to three tries) with a used-mask so
none repeats until all have played. No other condition applies to group 9.

The libraries (`SAL` objects in `glbchar.gcb` for male/female, and one embedded in each
character's `Data/Chars/NNchar.gcb` `CHR` object; `tools/research/find_sal.py`,
`tools/research/sal_dump.py`) give, for the putter, the same clips for every style:

    shared male / female       gplptt10, gplptt01   /  fplptt10, fplptt01
    03 Cedric "Ace" Andrews, 09 G. Capone, 16 S. McGruff, 18 Takeharu "Tsunami" Moto,
    25 Moa "Big Mo" Ta'a Vatu, 26 Melvin "Yosh" Tanigawa
                               gplptt10, gplptt01, gplptt12
    15 Edwin B. "Pops" Masterson IV   gplptt12 only

(Capone and McGruff have no bio card; their names survive only as the tail of their files'
resource paths, `...ICAPONE\EXPORTED/GC.CHG` and `MCGRUFF\EXPORTED/SM.CHG`, so the initials come
from the file names. They are probably unlockable characters.)

`gplptt12` is the **pool-cue tap-in** (the golfer drops to the ground, turns the putter round
and knocks it in with the grip like a cue). **Confirmed in game (2026-09-22)**: with the Gecko code
in `tools/codes/pool_cue_gimme.txt`, which hands every group-9 lookup the `gplptt12` clip from any
loaded golfer's library, a created golfer playing alongside Cedric Andrews tapped in with the pool
cue on the first gimme. A created golfer's own library (character 08) has no group 9, and the
creator's 111-animation playlist (group 20) has no tap-ins, so without the code they never get it. So it is a one-in-three
gimme for six golfers, every gimme for Pops, and never for anyone else - including Tiger.

The CPU's shot rehearsal (`AI_RehearseShot`, `0x8002B030`, in C)
-------------------------------------------------------------------

After choosing an aim point the CPU does not trust its plan: it rehearses it. A private copy of
the ball (`gSimBall`) is launched with the planned club and aim at **`power x AI_PowerScale`,
capped at 150%** (the sim may overswing past anything a human can), and the **real ball
physics** are stepped with the "simulating" flag up (`Ball_SetSimulating(1)`, which silences sounds,
effects and the tree-deflection roll), **0.2 s of ball time per frame - twelve real 1/60 s ticks,
not a coarse step** (six in "fast" mode). It is a small state machine on
`Player.nRehearseState`: 2 reset, 0 launch, 1 step, 3 "stop now", 4 settled.

Each frame in state 1, after the step:

- **The hazard hook fired** (`AI_SimAbort`, called from the water/OB code when the ball it is
  handling is the rehearsal's): if an earlier rehearsal landed, snap the aim back to the best
  one found and relaunch. Otherwise **+5 on the modifiers** (aggression -5, capped by
  `Golfer_ClampModifiers`) and a nudge that the *authored aim point itself* prescribes. The
  byte we had called the target's "type" is really its **if-it-goes-wrong code**: 1/2 turn the
  aim 1 degree left/right, 3/4 make the shot 5 yards shorter/longer, 5/6 turn 2 degrees.
  Then re-plan and relaunch.
- **Ball still moving** (ball state 2, 3 or 4): wait for the next frame.
- **Ball stopped in a hazard** (ball state 5), and nothing has landed yet: put the original
  club back and swap **longer by 1, shorter by 1, longer by 2, shorter by 2...** on successive
  tries (`AI_ClubLonger` / `AI_ClubShorter` walk on until a club usable for the shot kind),
  +5 on the modifiers (uncapped this time), re-plan, relaunch.
- **Ball stopped** anywhere else: measure the miss from the intended landing point (x/z only);
  remember this aim if it is the best so far; if the miss squared is over the tolerance,
  **aim -= 0.45 x miss** and relaunch. Within tolerance: state 4, done - the rehearsed aim is
  the shot, and only then `AI_ApplyError` worsens it by skill (the CPU path in `Swing.c`,
  `0x8005E0BC`).

State 3 is the caller's "enough": use the best aim found, or, if nothing ever landed, **+25 on
the modifiers** and `AI_ChooseTarget` again from scratch.

**The caller is swing state 2, "thinking"** (`SwingState02_Update`, in C). The CPU rehearses
one frame at a time with the same tolerance as the caddie - **land within 1.8 inches** of the
chosen point - and moves on once the rehearsal has settled, at least **1 second** has passed and
the camera has settled; or when its time is up: **4 seconds**, 1.5 s for a tee shot and 3.5 s
after it in modes 6 and 7, 3..4 s in mode 11. Four seconds is 240 frames x 0.2 s = 48 s of
simulated ball time, enough for a few full rehearsals. Time up with no landing is what forces
state 3. Then `AI_ApplyError` and on to state 10 like a human. So the CPU's "pause to think"
before a shot is exactly its simulation running.

So hypothesis 2 was right after all in its first half too: the CPU **does** solve its shot -
not by inverting a formula, but by simulating it until it lands.

The caddie (`Caddie_Start` / `Caddie_Update` / `Caddie_GetTip`, `0x8002DB80`..)
-----------------------------------------------------------------------------------

The putt tip is that same rehearsal, run for you. On a putt, `Caddie_Start` copies your whole
player struct into **slot 4**, marks the copy a CPU, aims it at the pin and resets the
rehearsal. `Caddie_Update` steps it once per frame with a tolerance of 0.05 (0.0025 squared)
and counts frames. `Caddie_GetTip` hands back slot 4's solved aim point, or **gives up after
600 frames** (ten seconds) and reports "unavailable" (return value 2).

Why it misreads: the search accepts an aim whose simulated ball *stops within 1.8 in of the
pin* - stopping at the hole, not dropping in - and it assumes a perfect stroke at the power the
game computes for the distance. The physics itself is the real thing at full resolution, twelve
ticks a frame. A long, breaking putt needs many rehearsal rounds of several seconds of simulated
roll each (a ten-second putt is 50 frames per round), and hits the ten-second budget: that is
the "tip unavailable". No random term anywhere in it. The `+5` modifiers from failed rehearsals land on
slot 4, not on you.

The drawn break line (`BreakLine_Start` / `BreakLine_Step`, `GoBreakLine.c`) is honest in the
same way: it launches a putt at your *current* aim with the computed power, runs the real
physics without randomness, and draws 450 samples of the trail, recomputing only once your aim
has been still for less than an inch of movement.

The CPU's per-shot modifiers (`AI_SetShotModifiers`, `0x8002A630`, exact)
--------------------------------------------------------------------------

Before every CPU shot, `Player_IsCPU` gated, its eight modifiers (POWER, IQ, AGGRESSION,
STRIKING, APPROACH, PUTTING, RECOVERY, LUCK; aggression always the opposite sign) are set:

    game mode 11 (scenarios)          all 0
    per-player level (0xC2A) != 0     25 x level
    strokes on this hole >= par + 2   20 x (strokes - par - 1)
    strokes on this hole == par + 1   10
    game mode 4, CPU leads by N holes -5 x N (POWER: random -5..+4)
    otherwise                         each a random -5..+4

So a CPU that is having a bad hole gets **better** as it goes - +10 at bogey pace, +20 at
double, +40 at triple - and in match play it gets **worse** by 5 per hole it leads. The base
attributes never change; these are the modifiers `Golfer_GetAttribute` adds in mode 2, capped
at 100 (110 for POWER, IQ, AGGRESSION).

Wind (`fn_80055FC8`, `fn_80055F88`, `fn_80055F1C`; the flight step `fn_80051CD0`)
----------------------------------------------------------------------------------

Each hole carries an authored wind (direction and speed in the course table at `0x801FA2F4`,
`0x430` bytes per course, `0x38` per hole). A global "wind off" flag (`gpGame + 0x28B`) zeroes
it. When the authored wind is zero the hole's wind is rolled from the session's wind setting
(`gSession + 0xE88`), direction one of eight at random:

    setting 0    0 .. 6
    setting 1    2 .. 12
    setting 2    5 .. 20
    setting 3    12 .. 31
    4 and up     none

On two courses (indices 6 and 15 - Royal Birkdale and Sahalee if the index follows the course
name table, which we have not verified) the setting is forced up: below 2 becomes 2, otherwise
3 - except in game mode 4 with no challenge selected. A speed of exactly 0 is stored as 0.1.
That is the whole generator: the RNG, the course, the setting. **No score, no standing, no
player in it.**

The flight step reads the wind vector every tick - and this is the one place the wind knows
who is hitting: **if the ball's player is a CPU, each axis of the wind is clamped to +-15**
before it is applied. A human's ball takes the full 31. Not a rubber band - it does not move
with the score - but a fixed leniency for the CPU in strong wind, presumably so its
rehearsal-tuned shots stay on the course.

What none of this reads
-----------------------

The swing and physics code reads only the player's own attributes, the shot geometry, club
tables, the wind and the RNG. The score reaches the attributes through two doors:
`Golfer_IsLucky` (holes-won, mode 4, humans) and `AI_SetShotModifiers` (strokes vs par on the
hole, and holes-won in mode 4, CPU only). Wind is generated from the course, the setting and the
RNG, and is softened for CPU balls regardless of the score.

EA's random number generator
----------------------------

`Rand_Next(stream)` is an additive lagged-Fibonacci generator: 20 words of state per stream at
`0x801A24C4 + stream*0x50`, an index per stream at `0x801A24B8`. Each call moves the index back
one (wrapping at 20) and does `s[i] = s[i] + s[(i + 3) mod 20]`, returning `s[i]`.
`Rand_Float(stream)` puts the low 23 bits into a float mantissa with exponent 0 and subtracts 1,
giving [0, 1). The AI uses stream 0. The C library `rand()` is never called anywhere.
