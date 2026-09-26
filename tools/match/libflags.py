"""Library-wide compiler flag search: one flag set for a group of units, scored together.

    python tools/match/libflags.py <Unit>[,<Unit>...] [--minutes M] [-j N] [--out DIR]

Why: real teams set flags per library, not per file (#match-help, 2026-09-26). The UIS units are EA
Tiburon's IStudio library (TW2005's paths: Code/Tiburon/IStudio/), so their build settings may
differ from the Tiger team's; our per-file flags there are four different guesses.

Every trial compiles each unit's current source with the SAME flag set (the unit's own
extra_cflags are dropped; the flags of a plain game unit are the base) and scores it against the
original object with objdiff: per unit, the size-weighted match of its functions and its exact
count. The search: every value of every axis alone (singles), then coordinate descent from the
best starts, then random restarts until the time is up. src/ is never written.
Output DIR (default build/libflags): results.tsv (every trial: flags, per-unit %, exact counts,
total) and best.txt (the best sets, and the best set for each unit alone)."""
import itertools, json, multiprocessing as mp, os, pathlib, random, re, shlex, subprocess, sys, tempfile, time

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from hosttools import OBJDIFF_CLI  # noqa: E402

# Each axis: its alternatives (the first is the base build's own setting).
AXES = [
    ('opt', ['-O4,p', '-O4,s', '-O3,p', '-O3,s', '-O2,p', '-O4,p -opt nopeephole', '-O4,p -opt noschedule',
             '-O4,p -opt nolifetimes', '-O4,p -opt nopropagation', '-O4,p -opt nocse',
             '-O4,p -opt noloop']),
    ('inline', ['-inline smart', '-inline auto', '-inline auto,deferred', '-inline deferred', '-inline off',
                '-inline all']),
    ('pool', ['', '-pragma "pool_data on"']),
    ('str', ['-str reuse', '-str reuse,readonly', '-str readonly', '-str noreuse']),
    ('fpc', ['-fp_contract on', '-fp_contract off']),
    ('lang', ['-lang=c', '-lang=c++']),
    ('cc', ['GC/2.5', 'GC/1.3.2', 'GC/1.2.5', 'GC/1.1', 'GC/3.0a3', 'GC/3.0a5']),
    ('misc', ['', '-pragma "scheduling off"', '-pragma "opt_propagation off"', '-pragma "opt_dead_assignments off"',
              '-pragma "opt_lifetimes off"', '-pragma "opt_common_subs off"', '-pragma "peephole off"']),
]


def base_command(units):
    """(the plain unit's compile command, {unit: source}) from ninja's compile commands."""
    cmds = subprocess.run(['ninja', '-t', 'commands', 'build/GW4E69/report.json'], capture_output=True,
                          text=True, cwd=ROOT).stdout.splitlines()
    # Windows' ninja writes src\\X.c: match on a /-normalised copy, keep the original for running.
    cc = [c.split(' && ')[0] for c in cmds if 'mwcceppc' in c and re.search(r' -c src[/\\]', c)]
    norm = lambda c: c.replace('\\', '/')
    # The base: a plain game unit's flags (no extra_cflags): EASB.c.
    plain = next(c for c in cc if ' -c src/EASB.c ' in norm(c))
    out = {}
    for u in units:
        src = next((re.search(r' -c (\S+) ', c).group(1) for c in cc
                    if re.search(r' -c src/(\S*/)?%s\.c ' % re.escape(u), norm(c))), None)
        if not src:
            sys.exit('no compile command for ' + u)
        out[u] = src
    return plain, out


def make_cmd(base, flags, src, obj):
    """The plain unit's own command line (a string, run through the shell: the same on Windows and
    Linux) with the compiler, the varied settings, the source and the output swapped."""
    c = re.sub(r'GC/[^/\\]+([/\\])mwcceppc', lambda m: flags['cc'] + m.group(1) + 'mwcceppc', base)
    for x in (' -MMD', ' -O4,p', ' -inline smart', ' -str reuse', ' -fp_contract on', ' -lang=c'):
        assert x in c, x
        c = c.replace(x, '', 1)
    extra = ' '.join(flags[k] for k, _ in AXES if k != 'cc' and flags[k])
    c = re.sub(r' -c \S+ -o \S+', lambda m: ' %s -c %s -o %s' % (extra, src, obj), c)
    return c


def score_unit(cmd, obj, unit):
    r = subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)
    if r.returncode or not os.path.exists(obj):
        return None
    tgt = ROOT / 'build/GW4E69/obj' / (unit + '.o')
    d = subprocess.run([OBJDIFF_CLI, 'diff', '-1', str(tgt), '-2', obj, '-c', 'functionRelocDiffs=none',   # as the report
                        '-o', '-', '--format', 'json'],
                       capture_output=True, text=True).stdout
    try:
        syms = json.loads(d)['left']['symbols']
    except Exception:
        return None
    fns = [s for s in syms if s.get('instructions')]
    tot = sum(int(s.get('size', 0)) for s in fns) or 1
    pct = sum(int(s.get('size', 0)) * s.get('match_percent', 0) for s in fns) / tot
    exact = sum(1 for s in fns if s.get('match_percent', 0) == 100)
    return pct, exact, len(fns)


def trial(args):
    flags, base, srcs = args
    res = {}
    with tempfile.TemporaryDirectory() as d:
        for u, src in srcs.items():
            obj = os.path.join(d, u + '.o').replace('\\', '/')
            res[u] = score_unit(make_cmd(base, flags, src, obj), obj, u)
    return flags, res


def total(res):
    if any(v is None for v in res.values()):
        return (-1, -1)
    return (sum(v[1] for v in res.values()), sum(v[0] for v in res.values()))   # exact first, then %


def main():
    a = sys.argv[1:]
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    units = a[0].split(',')
    minutes = float(opt('--minutes', '60'))
    workers = int(opt('-j', str(os.cpu_count() or 4)))
    out = pathlib.Path(opt('--out', str(ROOT / 'build/libflags')))
    out.mkdir(parents=True, exist_ok=True)
    base, srcs = base_command(units)
    end = time.time() + minutes * 60
    seen = {}
    log = open(out / 'results.tsv', 'w', encoding='utf-8')
    log.write('total_exact\ttotal_pct\t' + '\t'.join('%s_pct\t%s_exact' % (u, u) for u in units) + '\tflags\n')
    start = {k: v[0] for k, v in AXES}

    def key(f):
        return tuple(f[k] for k, _ in AXES)

    def run(batch, pool):
        batch = [f for f in batch if key(f) not in seen]
        for flags, res in pool.imap_unordered(trial, [(f, base, srcs) for f in batch]):
            seen[key(flags)] = (flags, res)
            t = total(res)
            log.write('%s\t%.3f\t%s\t%s\n' % (t[0], t[1], '\t'.join(
                ('%.3f\t%d' % res[u][:2]) if res[u] else 'ERR\tERR' for u in units),
                ' | '.join('%s=%s' % (k, flags[k]) for k, _ in AXES)))
            log.flush()
            if time.time() > end:
                break

    def best(n=1):
        return sorted(seen.values(), key=lambda fr: total(fr[1]), reverse=True)[:n]

    with mp.Pool(workers) as pool:
        # 1. the base, then every single change
        batch = [dict(start)]
        for k, vals in AXES:
            for v in vals[1:]:
                f = dict(start); f[k] = v; batch.append(f)
        run(batch, pool)
        print('singles done: %d trials, best %s' % (len(seen), total(best()[0][1])), flush=True)
        # 2. coordinate descent from the 4 best, then random restarts
        rnd = random.Random(1)
        starts = [f for f, _ in best(4)]
        while time.time() < end:
            cur = starts.pop(0) if starts else {k: rnd.choice(v) for k, v in AXES}
            improved = True
            while improved and time.time() < end:
                improved = False
                for k, vals in AXES:
                    batch = []
                    for v in vals:
                        f = dict(cur); f[k] = v; batch.append(f)
                    run(batch, pool)
                    cand = max((seen[key(f)] for f in batch if key(f) in seen), key=lambda fr: total(fr[1]))
                    if key(cur) not in seen or total(cand[1]) > total(seen[key(cur)][1]):
                        cur, improved = cand[0], cand[0] != cur
            print('descent end: %s  %s' % (total(seen[key(cur)][1]) if key(cur) in seen else '?',
                                            ' | '.join('%s=%s' % (k, cur[k]) for k, _ in AXES)), flush=True)
    log.close()
    lines = ['base: %s' % str(total(seen[key(start)][1])), '']
    for f, res in best(10):
        lines.append('%s  %s' % (total(res), ' | '.join('%s=%s' % (k, f[k]) for k, _ in AXES)))
        lines.append('    ' + '  '.join('%s %.2f%% %d/%d' % (u, *res[u]) for u in units))
    lines.append('')
    for u in units:
        ok = [fr for fr in seen.values() if fr[1].get(u)]
        f, res = max(ok, key=lambda fr: (fr[1][u][1], fr[1][u][0]))
        lines.append('best for %s alone: %.2f%% %d/%d  %s' % (u, *res[u], ' | '.join('%s=%s' % (k, f[k]) for k, _ in AXES)))
    (out / 'best.txt').write_text('\n'.join(lines) + '\n', encoding='utf-8')
    print('\n'.join(lines))


if __name__ == '__main__':
    main()
