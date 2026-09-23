"""Putt test harness for hypothesis 6 (docs/hypotheses.md).

A host-side port of the putt physics in src/Ball.c, on a flat, tilted plane of green:

- fn_80052268       state 3, skidding
- Ball_GroundContact state 4, rolling (break, 5/7 g, friction, stop)
- Ball_CupPull      the pull toward the cup (real ball only; sims skip it)
- Ball_FlightStep   in the air over the cup (no wind), with the x3 gravity near the cup
- Ball_Tick         the tick order, the 4 s stall check
- fn_800512BC       the putt launch (x 7.2, laid on the green, x 1.8, no spin)
- fn_80050D34       distance -> putt power (gPuttDist)

and the CPU's rehearsal (AI_RehearseShot in src/Golfer.c): aim at the pin, launch at the power
for the aim point's distance, run the ball with no cup pull until it stops, and move the aim
point by 0.45 x the miss until it stops within 1.8 in (0.05 yd). Then the real putt: the skill
error (AI_ApplyError) and x 1.05 power (Swing_ComputePower), with the cup pull on.

Not from the game: the cup. The real cup is course geometry (not decoded yet). Here it is a
hole of radius CUP_R in the plane, and the rim is a hard edge (restitution 0.07, the green's
+0x0C). A ball whose centre passes over the hole leaves the ground (as fn_8005418C does when the
ground drops away) and is holed once it is entirely below the rim. The line (where the ball
passes the cup) is exact; the make/miss call at the rim is this model's.

Units are the game's: yards; velocity in yd per 36 ticks; 60 ticks a second.
Run: python tools/research/putt_sim.py [--green 0..2] [--cup RADIUS_IN] [--mc N]
"""
import math
import random
import sys

G = 0.10717                 # gravity per tick
BALL_R = 0.0256667          # BALL_RADIUS
CLEAR = 0.0013888889        # ball rests this far above the ground
CUP_R = 4.25 / 2 / 36       # regulation cup radius in yd (assumed)
RIM_E = 0.07                # rim restitution (green surface +0x0C)

# Green surface 16 (class 3), from gSurfaceTypes (0x8017E9B8 + 16 x 0x44).
S_F14, S_F18, S_F1C, S_F20 = 0.0, 0.975, 0.375, 0.25
GREEN_MUL = [1.0, 1.1, 1.2]           # gGreenSpeedMul, setting default 0
PUTT_DIST = [0, 0.11, 0.43, 0.97, 1.73, 2.71, 3.91, 5.32, 6.96, 8.81, 10.88, 13.09, 15.59,
             18.3, 21.24, 24.39, 27.77, 31.35, 35.16, 39.19, 43.43, 47.75, 52.42]
PUTT_SPEED = 0.0471 * 1.0 * 8.333333 * 7.2    # gClubSpeed[25] x gKindSpeed[0] x 8.33 x 7.2

GREEN_SETTING = 0


# ---- small vector helpers (the game's fn_8000C5D4 etc.) -----------------------------------------
def add_s(a, b, s):
    return [a[0] + s * b[0], a[1] + s * b[1], a[2] + s * b[2]]


def dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def scale(a, s):
    return [a[0] * s, a[1] * s, a[2] * s]


def length(a):
    return math.sqrt(dot(a, a))


def norm(a):
    l = length(a)
    return scale(a, 1.0 / l) if l else [0.0, 0.0, 0.0]


def cross(a, b):
    return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]]


def rot(a, b, s, c):        # fn_80055D70
    return a * c - b * s, b * c + a * s


# ---- the green -------------------------------------------------------------------------------
class Green:
    """A plane y = gx * x + gz * z through the pin at the origin."""

    def __init__(self, gx, gz):
        self.gx, self.gz = gx, gz
        self.n = norm([-gx, 1.0, -gz])

    def height(self, x, z):
        return self.gx * x + self.gz * z


def putt_power(dist):       # fn_80050D34, dry weather (scale 1.0)
    for i in range(1, 23):
        hi = PUTT_DIST[i]
        if dist <= hi:
            lo = PUTT_DIST[i - 1]
            return 0.05 * ((dist - lo) / (hi - lo)) + 0.05 * (i - 1)
    return 1.1


class Ball:
    def __init__(self, green, pos):
        self.g = green
        self.pos = list(pos)
        self.prev = list(pos)
        self.start = list(pos)
        self.vel = [0.0, 0.0, 0.0]
        self.spin = [0.0, 0.0, 0.0]
        self.state = 0
        self.holed = False
        self.speed = 0.0
        self.fb4 = 0.0
        self.fb8 = 0.0
        self.closest = 1e9
        self.cross = None       # (offset across the line, speed) when it first reaches the cup

    # fn_800512BC (putt part) + Ball_Launch
    def launch(self, power, aim):
        s, c = math.sin(aim), math.cos(aim)
        v = [-s * power * PUTT_SPEED, 0.0, c * power * PUTT_SPEED]
        n = self.g.n
        v = add_s(v, n, -dot(v, n))
        self.vel = scale(v, 1.8)
        self.spin = [0.0, 0.0, 0.0]
        self.start = list(self.pos)
        self.state = 3

    # fn_80052268
    def skid(self, t):
        n = self.g.n
        pull = 1.0 - S_F14
        pull *= 2.0 - GREEN_MUL[GREEN_SETTING]
        fdot = -G * n[1]
        acc = [-(pull * n[0] * fdot), pull * (-G - n[1] * fdot), -(pull * n[2] * fdot)]
        d = norm(add_s(self.vel, n, -dot(self.vel, n)))
        self.vel = scale(d, length(self.vel))
        fric = 1.5 * (S_F18 * (-G * n[1]))
        fric *= 2.0 - GREEN_MUL[GREEN_SETTING]
        fric *= 2.97619057
        spin_add = scale(cross(d, n), fric)
        self.vel = add_s(self.vel, acc, t)
        self.spin = add_s(self.spin, spin_add, t)
        if 0.84 * length(self.spin) >= length(self.vel):
            self.state = 4

    # Ball_GroundContact (without the cup-surface holed check: the cup is modelled below)
    def roll(self, t, cup_pull):
        n = self.g.n
        d = norm(add_s(self.vel, n, -dot(self.vel, n)))
        self.vel = scale(d, length(self.vel))
        down = scale(n, -0.839999974)
        fz = -0.173615396 * down[2]
        fx = 0.173615396 * down[0]
        flen = math.sqrt(fz * fz + fx * fx)
        if flen != 0.0:
            k = abs(fz * d[0] + fx * d[2]) / flen
            fx *= k
            fz *= k
            flen = S_F1C * math.sqrt(fz * fz + fx * fx) * 0.6
            self.spin = scale(cross(self.vel, down), 1.41723347)
            fang = 0.45722881 * length(self.spin)
            turn = flen / fang if fang != 0.0 else 0.0
            ax = [self.spin[1] * fx, self.spin[2] * fz - self.spin[0] * fx, -(self.spin[1] * fz)]
            if ax[1] < 0.0:
                ax = scale(ax, -1.0)
                flip = True
            else:
                flip = False
            ang_x = math.atan2(ax[0], ax[1])
            s, c = math.sin(ang_x), math.cos(ang_x)
            fa, fb = rot(ax[0], ax[1], s, c)
            v = self.vel
            v[0], v[1] = rot(v[0], v[1], s, c)
            ang = -math.atan2(ax[2], fb)
            s, c = math.sin(ang), math.cos(ang)
            v[1], v[2] = rot(v[1], v[2], s, c)
            if not flip:
                turn = -turn
            s, c = math.sin(turn), math.cos(turn)
            v[0], v[2] = rot(v[0], v[2], s, c)
            s, c = math.sin(-ang), math.cos(-ang)
            v[1], v[2] = rot(v[1], v[2], s, c)
            s, c = math.sin(-ang_x), math.cos(-ang_x)
            v[0], v[1] = rot(v[0], v[1], s, c)
        acc = scale(d, -(0.714285731 * (G * d[1])))
        self.vel = add_s(self.vel, acc, t)
        if cup_pull:
            self.cup_pull(t)
        rough = S_F20
        if n[1] < 0.866:
            rough = min(rough * 0.5 * n[1], 0.14)
        fric = n[1] * (0.0765499994 * rough) * 0.575
        fric *= 2.0 - GREEN_MUL[GREEN_SETTING]
        fric *= t
        if length(self.vel) < fric:
            self.stop()
            return
        self.vel = add_s(self.vel, d, -fric)
        self.spin = [-1.41723347 * self.vel[2] * down[1], 0.0, 1.41723347 * self.vel[0] * down[1]]

    # Ball_CupPull (the pin is the origin)
    def cup_pull(self, dt):
        p = self.pos
        dist = length(p)
        if dist >= 0.152778:
            return
        pin = [0.0, BALL_R, 0.0]
        sd = length(add_s(self.start, pin, -1))
        if sd < 0.166667:
            return
        if length(add_s(self.start, p, -1)) > sd - 0.0416667:
            return
        a = math.atan2(p[0] - self.prev[0], p[2] - self.prev[2])
        a = abs(a - math.atan2(pin[0] - p[0], pin[2] - p[2]))
        while a > math.pi:
            a -= math.pi
        if dist < 0.0625:
            if a > 0.523599 and self.speed > 0.366667:
                slow = 1.0 - 16.0 * (0.67 * dist)
                self.vel[0] *= slow
                self.vel[2] *= slow
            return
        if a >= 0.523599 and dist >= 0.0972222:
            return
        k = 0.455472 * dt
        for i in (0, 2):
            pull = k * (pin[i] - p[i])
            if (self.vel[i] < 0 and pull < 0) or (self.vel[i] > 0 and pull > 0):
                if abs(a) > 0.293333:
                    pull = 0.0
            self.vel[i] += pull

    # Ball_FlightStep with no wind; spin decays
    def fly(self, t, full_cup):
        rel = self.vel
        sp2 = dot(rel, rel)
        sp = math.sqrt(sp2)
        spin = length(self.spin)
        if sp != 0.0:
            drag = -(sp2 * (0.000474568689 * (0.225790471 + (spin * (-0.000348685688 * sp + 0.0168940704)
                                                               + 0.000780952396 * sp))) / sp)
        else:
            drag = 0.0
        acc = scale(rel, drag)
        lift = sp2 * (0.000474568689 * (0.0847342834 + (spin * (-0.000628289126 * sp + 0.0407094695)
                                                        - 0.000201047602 * sp)))
        lv = cross(self.spin, rel)
        ll = length(lv)
        acc = add_s(acc, lv, lift / ll if ll else 0.0)
        acc[1] -= G
        if full_cup and length(add_s(self.pos, [0.0, BALL_R, 0.0], -1)) < 0.0625:
            acc[1] -= 0.21434
        self.vel = add_s(self.vel, acc, t)
        self.spin = scale(self.spin, 1.0 - t * 0.003)

    def stop(self):
        self.state = 1
        self.vel = [0.0, 0.0, 0.0]

    def hole(self):     # Ball_Holed: parked at pin + (0.5 in, -3 in, 0.5 in)
        self.pos = [0.0138889, -0.0833333, 0.0138889]
        self.holed = True
        self.stop()

    # The modelled cup: after the move, the ground under a rolling ball; the rim for one in the air.
    def cup(self):
        x, y, z = self.pos
        rho = math.hypot(x, z)
        if self.cross is None and (z >= 0.0 or rho < CUP_R):
            self.cross = (x, self.speed)
        if self.state in (3, 4):
            if rho < CUP_R:
                self.state = 2          # the ground has dropped away (fn_8005418C)
            else:
                self.pos[1] = self.g.height(x, z) + BALL_R + CLEAR
            return
        # In the air over or beside the hole: the rim is a hard circular edge.
        if rho > 0.0:
            px, pz = x / rho * CUP_R, z / rho * CUP_R
            rim = [px, self.g.height(px, pz), pz]
            off = add_s(self.pos, rim, -1)
            d = length(off)
            if d < BALL_R:
                nrm = scale(off, 1.0 / d)
                vn = dot(self.vel, nrm)
                if vn < 0.0:
                    self.vel = add_s(self.vel, nrm, -(1.0 + RIM_E) * vn)
                self.pos = add_s(rim, nrm, BALL_R)
        x, y, z = self.pos
        rho = math.hypot(x, z)
        ground = self.g.height(x, z)
        if rho < CUP_R and y < ground - BALL_R:
            self.hole()
        elif rho >= CUP_R and y <= ground + BALL_R + CLEAR:
            self.pos[1] = ground + BALL_R + CLEAR       # back on the green: rolling again
            n = self.g.n
            self.vel = add_s(self.vel, n, -dot(self.vel, n))
            self.state = 4

    # Ball_Tick
    def tick(self, t, real):
        if self.state == 3:
            self.skid(t)
        elif self.state == 4:
            self.roll(t, cup_pull=real)
        elif self.state == 2:
            self.fly(t, full_cup=real)
        else:
            return
        self.prev = list(self.pos)
        self.pos = add_s(self.pos, self.vel, t / 36.0)
        if self.state not in (1, 5) and not self.holed:
            self.cup()
            self.fb8 += 0.0166666675 * t
            if self.fb8 > 4.0:
                d = length(add_s(self.start, self.pos, -1))
                if abs(d - self.fb4) < 0.111111112:
                    self.stop()
                else:
                    self.fb4 = d
                    self.fb8 = 0.0
        if self.holed or self.state in (1, 5):
            self.speed = 0.0
        else:
            self.speed = 60.0 * (length(self.vel) / 36.0)
            self.closest = min(self.closest, length(self.pos))

    def run(self, real, max_ticks=20000):
        n = 0
        while self.state in (2, 3, 4) and n < max_ticks:
            self.tick(1.0, real)
            n += 1
        return n


# ---- a putt from L yards short of the pin along +z --------------------------------------------
def start_pos(green, L):
    return [0.0, green.height(0.0, -L) + BALL_R + CLEAR, -L]


def shoot(green, L, target, power_mul=1.0, real=True, aim_err=0.0, dist_err=0.0):
    """Launch at an aim point the way the CPU does: power from the point's distance."""
    b = Ball(green, start_pos(green, L))
    dx, dz = target[0] - b.pos[0], target[1] - b.pos[2]
    dist = math.hypot(dx, dz)
    aim = math.atan2(-dx, dz) + aim_err
    dist *= (100.0 + dist_err) / 100.0
    b.launch(putt_power(dist) * power_mul, aim)
    ticks = b.run(real)
    return b, ticks


def rehearse(green, L, tol2=0.0025, budget_frames=240, ticks_per_frame=12):
    """AI_RehearseShot: aim point starts at the pin, moves by -0.45 x the miss. Returns
    (aim point, converged, rounds, frames used)."""
    tx, tz = 0.0, 0.0
    best, best_d2 = (tx, tz), 1e9
    frames, rounds = 0, 0
    while frames < budget_frames:
        b, ticks = shoot(green, L, (tx, tz), real=False)
        frames += 1 + (ticks + ticks_per_frame - 1) // ticks_per_frame   # launch frame + steps
        rounds += 1
        mx, mz = b.pos[0], b.pos[2]
        d2 = mx * mx + mz * mz
        if d2 < best_d2:
            best, best_d2 = (tx, tz), d2
        if d2 <= tol2:
            return (tx, tz), True, rounds, frames
        tx -= 0.45 * mx
        tz -= 0.45 * mz
    return best, False, rounds, frames


def skill_error(L, skill, rng):
    """AI_ApplyError for a putt: returns (aim error in radians, distance error in %)."""
    if L < 1.5:
        return 0.0, 0.0
    skill = max(15.0, min(98.0, skill))
    max_ang = math.radians(8.0) * (0.5 if L < 5.0 else 1.0)
    miss = 100.0 - skill
    a = max_ang * (miss * rng.random()) / 100.0
    if a < math.radians(0.25):
        a += math.radians(0.25)
    if rng.getrandbits(1):
        a = -a
    de = 20.0 * (miss * rng.random()) / 100.0 + 10.0 * (miss * rng.random()) / 100.0
    if rng.getrandbits(1):
        de = -de
    return a, de


def sidehill(grade_pct):
    """Green falling to the -x side (a putt along +z breaks toward -x)."""
    return Green(grade_pct / 100.0, 0.0)


def inches(yd):
    return yd * 36.0


def main():
    global GREEN_SETTING, CUP_R
    if "--cup" in sys.argv:
        CUP_R = float(sys.argv[sys.argv.index("--cup") + 1]) / 36.0
    if "--green" in sys.argv:
        GREEN_SETTING = int(sys.argv[sys.argv.index("--green") + 1])
    print("Green speed setting %d (friction x %.1f), cup radius %.3f in" % (
        GREEN_SETTING, 2.0 - GREEN_MUL[GREEN_SETTING], CUP_R * 36.0))
    print("Calibration: flat green, power -> roll (yd); the game's putt table says 43.4 x p^2")
    flat = Green(0.0, 0.0)
    for p in (0.2, 0.4, 0.6, 0.8, 1.0):
        b = Ball(flat, [0.0, BALL_R + CLEAR, -200.0])
        b.launch(p, 0.0)
        b.run(real=False)
        print("  p=%.1f  rolled %6.2f   table %6.2f" % (p, b.pos[2] + 200.0, 43.43 * p * p))

    lengths_ft = [6, 10, 15, 20, 30, 45]
    grades = [0, 1, 2, 3, 4]
    print()
    print("Perfect CPU (no skill error): rehearsed aim, then the real putt at x1.00 and x1.05.")
    print("Offset = where the ball crosses the pin's line, inches; + is the high side.")
    print("%5s %5s | %8s %6s | %22s | %22s" % ("ft", "grade", "aim(in)", "conv", "x1.00: offset  result", "x1.05: offset  result"))
    table = {}
    for L_ft in lengths_ft:
        L = L_ft / 3.0
        for g in grades:
            green = sidehill(g)
            aim, conv, rounds, frames = rehearse(green, L)
            row = []
            for mul in (1.0, 1.05):
                b, _ = shoot(green, L, aim, power_mul=mul)
                off = inches(b.cross[0]) if b.cross else float('nan')
                res = "holed" if b.holed else "miss %4.1f in" % inches(length([b.pos[0], 0, b.pos[2]]))
                row.append((off, res, b.holed))
            table[(L_ft, g)] = (aim, conv, row)
            print("%5d %4d%% | %8.1f %6s | %+7.2f  %-13s | %+7.2f  %-13s" % (
                L_ft, g, inches(aim[0]), "yes" if conv else "no(%d)" % rounds,
                row[0][0], row[0][1], row[1][0], row[1][1]))

    if "--mc" not in sys.argv:
        return
    n = int(sys.argv[sys.argv.index("--mc") + 1]) if len(sys.argv) > sys.argv.index("--mc") + 1 else 200
    print()
    print("Make rate with skill error, %d putts each (x1.05 as the game does / x1.00 for comparison)" % n)
    for skill in (98, 80):
        print("PUTTING %d" % skill)
        print("%5s " % "ft" + " ".join("%12s" % ("%d%%" % g) for g in grades))
        for L_ft in lengths_ft:
            L = L_ft / 3.0
            cells = []
            for g in grades:
                green = sidehill(g)
                aim = table[(L_ft, g)][0]
                made = [0, 0]
                rng = random.Random(1000 * L_ft + g)
                for _ in range(n):
                    ae, de = skill_error(L, skill, rng)
                    for j, mul in enumerate((1.05, 1.0)):
                        b, _ = shoot(green, L, aim, power_mul=mul, aim_err=ae, dist_err=de)
                        made[j] += b.holed
                cells.append("%3d%% /%3d%%" % (100 * made[0] // n, 100 * made[1] // n))
            print("%5d " % L_ft + " ".join("%12s" % c for c in cells))


if __name__ == "__main__":
    main()
