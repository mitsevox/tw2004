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
(`Swing_TeeSweetSpot`) when the tempo value lands in a 0.4..0.6 window, peaking at the centre.
Both are tuning values set in `Swing_Init`, not attributes.

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
never do** (early exit), and a session flag turns it off. The odds start at **1 in 12** for every
player (`gLuckOdds`), and:

- LUCK cuts the "12" by LUCK/2 percent: LUCK 100 makes it 1 in 6 (110 rounds to the same);
- **in game mode 4, when player 1 has won more than 4 holes more than player 0, the odds are
  halved again** (1 in 3 at LUCK 100); the same halving applies on one flagged hole;
- it only fires off the green, never on a putt, and only for a pitch, a shot from lie 1 or 2
  under 250 units, or one special shot mode; then `Rand_Next(0) % odds == 0` is the roll.

What the event does is in the lie code (`0x80053594`, read, not decompiled): landing in the
rough is a coin flip between the good rough lie and the bad one, and `(roll & 127) < LUCK/2`
forces the good one - 50% good at LUCK 0, ~70% at 100. On a worse surface, `(LUCK/4 + 16)/128`
is the chance of getting the rough treatment instead of the worst lie (12.5% at 0, 32% at 100).
And in the collision code (`0x80052598`, read) a bounce-direction term has `0.005 x LUCK`
subtracted, floored at -1: the kinder deflection off trees the tooltip promises.

What none of this reads
-----------------------

`AI_ChooseTarget`, `AI_ApplyError`, `AI_PlanShot`, both power functions, the forgiveness
function, boost, spin and rumble read: the player's own attributes, the shot geometry (ball,
target, pin, distance), lie, club tables, and the RNG. None of them reads a score, a hole
standing, or another player. **The one exception found is in `Golfer_IsLucky`**: the match-play
holes-won comparison above, in game mode 4 only. (Wind generation is still unread.)

EA's random number generator
----------------------------

`Rand_Next(stream)` is an additive lagged-Fibonacci generator: 20 words of state per stream at
`0x801A24C4 + stream*0x50`, an index per stream at `0x801A24B8`. Each call moves the index back
one (wrapping at 20) and does `s[i] = s[i] + s[(i + 3) mod 20]`, returning `s[i]`.
`Rand_Float(stream)` puts the low 23 bits into a float mantissa with exponent 0 and subtracts 1,
giving [0, 1). The AI uses stream 0. The C library `rand()` is never called anywhere.
