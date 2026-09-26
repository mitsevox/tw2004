"""Declaration-order search for register-only near misses, many functions at once, many cores.

    python tools/match/declsearch.py <Unit>:<fn>[,<Unit>:<fn>...] [--minutes M] [-j N] [--out DIR]

For each function: sets up build/perm/<fn> (perm_setup.py), finds the declarations at the top of
its body, then runs, in parallel worker processes:
  - `all`: every order of the declarations, when there are 7 or fewer (5,040 orders at most);
  - `ils`: iterated local search (a 2-move kick, then a 60-move first-improvement climb), one
    worker per random seed; it found orders that greedy climbs and pair sweeps missed (GoTerrain
    fn_80031154: 24 -> 0 in 22 seconds, 2026-09-26).
The -j workers are shared out over the functions: one `all` job for each small function, the rest
`ils` seeds. A function whose search reaches 0 stops its other jobs.

Scores are quicktrial's aligned count (0 = exact), ties broken by the positional count. Only the
function's declaration lines are reordered: the logic cannot change. Output per function in
DIR/<fn>/ (default build/declsearch): score.txt and diff.txt (the declaration lines, before ->
after). No source file is copied out. Each trial compiles in its own temporary folder, removed
afterwards (quicktrial.score)."""
import difflib, itertools, multiprocessing as mp, os, pathlib, random, re, subprocess, sys, time

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import quicktrial  # noqa: E402

# base.c is preprocessed and unindented: a declaration is `type name[...] [= init];` whose first
# word is not a statement keyword.
DECL = re.compile(r'^\s*(?!(return|goto|break|continue|case|default|else|do)\b)'
                  r'(const\s+|volatile\s+|register\s+|static\s+|unsigned\s+|signed\s+|struct\s+)*'
                  r'[A-Za-z_]\w*[\s\*]+\**\s*[A-Za-z_]\w*(\s*\[[^\]]*\])*(\s*=[^;]*)?;\s*(//.*)?$')


def split(fn, src):
    """(head, declaration lines, rest) of fn's body in src."""
    m = re.search(r'^[A-Za-z_][^\n;]*\b' + re.escape(fn) + r'\s*\([^;{]*\)\s*\{\n', src, re.M)
    if not m:
        return None
    lines = src[m.end():].split('\n')
    n = 0
    while n < len(lines) and DECL.match(lines[n]) and '(' not in lines[n].split('=')[0]:
        n += 1
    return src[:m.end()], lines[:n], '\n'.join(lines[n:])


def scorer(fn, head, rest):
    cache = {}

    def sc(d):
        k = tuple(d)
        if k not in cache:
            s = head + '\n'.join(d) + '\n' + rest
            a = quicktrial.score(fn, s, aligned=True)
            if isinstance(a, tuple):
                cache[k] = (9999, 9999)
            else:
                cache[k] = (a, 0 if a == 0 else quicktrial.score(fn, s))
        return cache[k]
    return sc


def job(args):
    """One worker: (fn, mode, seed, deadline, stop) -> (fn, mode, seed, best score, best order, trials)."""
    fn, mode, seed, deadline, stop = args
    head, decl, rest = split(fn, quicktrial.base(fn))
    sc = scorer(fn, head, rest)
    best_d, best = list(decl), sc(decl)
    n = 0
    if mode == 'all':
        for d in itertools.permutations(decl):
            if time.time() > deadline or stop.is_set() or best[0] == 0:
                break
            n += 1
            s = sc(d)
            if s < best:
                best, best_d = s, list(d)
    else:
        rnd = random.Random(seed)

        def move(d):
            d = list(d)
            a, b = rnd.randrange(len(d)), rnd.randrange(len(d))
            d.insert(b, d.pop(a))
            return d
        cur, cs = best_d, best
        while time.time() < deadline and not stop.is_set() and best[0]:
            d = move(move(cur))
            s = sc(d)
            n += 1
            for _ in range(60):
                e = move(d)
                r = sc(e)
                n += 1
                if r <= s:
                    d, s = e, r
            if s <= cs:
                cur, cs = d, s
            if s < best:
                best, best_d = s, d
    if best[0] == 0:
        stop.set()
    return fn, mode, seed, best, best_d, decl, n


def main():
    a = sys.argv[1:]
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    minutes = float(opt('--minutes', '60'))
    workers = int(opt('-j', str(os.cpu_count() or 4)))
    out = pathlib.Path(opt('--out', str(ROOT / 'build/declsearch')))
    targets = [t.split(':') for t in a[0].split(',')]
    fns = []
    for unit, fn in targets:
        r = subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_setup.py'), unit, fn],
                           capture_output=True, text=True, cwd=ROOT)
        if r.returncode:
            print('%s: perm_setup failed\n%s' % (fn, (r.stdout + r.stderr)[-600:]), flush=True)
            continue
        parts = split(fn, quicktrial.base(fn))
        if not parts or len(parts[1]) < 2:
            print('%s: fewer than 2 declarations found, skipped' % fn, flush=True)
            continue
        fns.append((fn, len(parts[1])))
        print('%s: %d declarations' % (fn, len(parts[1])), flush=True)
    if not fns:
        sys.exit('nothing to search')
    deadline = time.time() + minutes * 60
    mgr = mp.Manager()
    stops = {fn: mgr.Event() for fn, _ in fns}
    jobs = [(fn, 'all', 0, deadline, stops[fn]) for fn, n in fns if n <= 7]
    seed = 1
    while len(jobs) < max(workers, len(fns)):
        for fn, _ in fns:
            jobs.append((fn, 'ils', seed, deadline, stops[fn]))
        seed += 1
    best = {}
    with mp.Pool(workers) as pool:
        for fn, mode, sd, s, d, decl, n in pool.imap_unordered(job, jobs):
            print('%s %s seed %d: best %s after %d trials' % (fn, mode, sd, s, n), flush=True)
            if fn not in best or s < best[fn][0]:
                best[fn] = (s, d, decl)
    total = 0
    for fn, (s, d, decl) in sorted(best.items(), key=lambda kv: kv[1][0]):
        o = out / fn
        o.mkdir(parents=True, exist_ok=True)
        (o / 'score.txt').write_text('%d\n' % s[0], encoding='utf-8')
        diff = difflib.unified_diff(decl, d, 'before', 'after', lineterm='')
        (o / 'diff.txt').write_text('\n'.join(diff) + '\n', encoding='utf-8')
        start = split(fn, quicktrial.base(fn))[1]
        print('%-28s %s -> %d%s' % (fn, 'base' if d == start else 'new order', s[0],
                                    '   EXACT' if s[0] == 0 else ''), flush=True)
        total += s[0] == 0
    print('%d of %d exact' % (total, len(best)))


if __name__ == '__main__':
    main()
