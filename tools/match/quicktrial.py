"""Score C variants of one function in under a second, without touching the build.

Needs the function's permuter directory first (it holds base.c, the function with every
declaration it uses, and target.o, the original's code):

    python tools/match/perm_setup.py <Unit> <fn>

Then, from a script in your scratch folder:

    import sys; sys.path.insert(0, r'<checkout>/tools/match')
    from quicktrial import base, score
    src = base('fn_800FE3FC')                       # build/perm/<fn>/base.c as a string
    print(score('fn_800FE3FC', src.replace(a, b)))  # number of differing instructions, 0 = exact
    score('fn_800FE3FC', src, show=True)            # also prints both sides, '*' on differences

or from the command line, to score a file (default: base.c):

    python tools/match/quicktrial.py <fn> [file.c] [--show] [--aligned]
    python tools/match/quicktrial.py <fn> --climb <N> [--positional]   declaration-order climb

score(..., aligned=True) (--aligned) lines both sides up with difflib and counts only the differing
stretches, branch targets masked: one extra instruction costs 1, not every instruction after it.
Use it when variants change the function's length. variants(fn, old, {label: new}) scores several
replacements of one snippet (aligned by default); climb(fn, n) is declclimb.py on base.c, aligned
by default, seconds instead of minutes.

score() compiles with the unit's own compiler and flags (build/perm/<fn>/compile.sh) and compares
with perm_objdump.py, so symbol names are masked as in the permuter. It returns ('ERR', message)
when the compile fails. Use it to sweep hundreds of variants (declaration orders, spellings);
confirm the winner in the real unit with trial.py or a build, since base.c is a snapshot taken by
perm_setup.py.
"""
import difflib, os, re, shlex, subprocess, sys, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
HERE = os.path.dirname(os.path.abspath(__file__))


def _perm(fn):
    return os.path.join(ROOT, 'build', 'perm', fn)


def base(fn):
    return open(os.path.join(_perm(fn), 'base.c'), encoding='utf-8').read()


_cmd = {}


def _compiler(fn):
    # compile.sh: "<cc>" '<flag>' ... -c "$1" -o "$3" ...
    if fn not in _cmd:
        text = open(os.path.join(_perm(fn), 'compile.sh'), encoding='utf-8').read()
        line = [l for l in text.splitlines() if 'mwcceppc' in l][0]
        line = re.split(r'\s-c\s', line)[0]
        _cmd[fn] = shlex.split(line)
    return _cmd[fn]


def _dis(fn, obj):
    out = subprocess.run([sys.executable, os.path.join(HERE, 'perm_objdump.py'), fn, obj],
                         capture_output=True, text=True).stdout
    return [l.split('\t', 2)[-1] for l in out.splitlines() if '\t' in l]


_target = {}


def _branchless(lines):
    """Branch targets masked: one inserted instruction moves every later target."""
    return [re.sub(r'0x[0-9a-f]+$', 'OFF', x) if x.split() and x.split()[0].startswith('b') else x
            for x in lines]


def score(fn, src, show=False, aligned=False):
    """Differing instructions between src's fn and the original (0 = exact).

    aligned=True lines the two up with difflib first and counts only the instructions in the
    differing stretches (branch targets masked), so one extra or missing instruction costs 1
    instead of every instruction after it. Better for comparing near-misses whose lengths differ;
    0 still means exact."""
    if fn not in _target:
        _target[fn] = _dis(fn, os.path.join(_perm(fn), 'target.o'))
    d = tempfile.mkdtemp()
    c, o = os.path.join(d, 'v.c'), os.path.join(d, 'v.o')
    open(c, 'w', encoding='utf-8').write(src)
    r = subprocess.run(_compiler(fn) + ['-c', c, '-o', o], capture_output=True, text=True)
    if not os.path.exists(o):
        return ('ERR', r.stdout[-800:])
    a, b = _target[fn], _dis(fn, o)
    if aligned:
        a, b = _branchless(a), _branchless(b)
        ops = difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes()
        if show:
            for t, i1, i2, j1, j2 in ops:
                if t != 'equal':
                    print('@%d %s\n   orig=%s\n   ours=%s' % (i1, t, a[i1:i2], b[j1:j2]))
        return sum(max(i2 - i1, j2 - j1) for t, i1, i2, j1, j2 in ops if t != 'equal')
    if show:
        for i in range(max(len(a), len(b))):
            x = a[i] if i < len(a) else ''
            y = b[i] if i < len(b) else ''
            print(('  ' if x == y else '* ') + x.ljust(36) + y)
    return sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b))


def variants(fn, old, new, show=False, aligned=True):
    """Score base.c with `old` replaced by each of the texts in the dict `new` ({label: text});
    spacing and indentation are ignored when finding `old`. Prints and returns {label: score}."""
    src = base(fn)
    m = re.search(r'\s+'.join(re.escape(w) for w in old.split()), src)
    assert m, 'old text not found in base.c'
    print('base', score(fn, src, aligned=aligned), flush=True)
    out = {}
    for k, v in new.items():
        out[k] = score(fn, src[:m.start()] + v + src[m.end():], show, aligned)
        print(k, out[k], flush=True)
    return out


def climb(fn, n, aligned=True):
    """declclimb.py on base.c: move one of the first n lines after fn's opening line at a time,
    keep the first move that lowers the score, repeat. Returns (score, lines); fast enough for
    long declaration lists, but base.c is a snapshot: apply the result to the unit and check."""
    src = base(fn)
    m = re.search(r'^[A-Za-z_][^\n;]*\b' + re.escape(fn) + r'\([^;{]*\)\s*\{\n', src, re.M)
    start = m.end()
    lines = src[start:].split('\n')
    decl, rest = lines[:n], '\n'.join(lines[n:])

    def sc(d):
        s = score(fn, src[:start] + '\n'.join(d) + '\n' + rest, aligned=aligned)
        return 9999 if isinstance(s, tuple) else s

    cur = sc(decl)
    print('start', cur, flush=True)
    improved = True
    while improved and cur > 0:
        improved = False
        for i in range(n):
            for j in range(n):
                if i == j:
                    continue
                d = decl[:]
                d.insert(j, d.pop(i))
                s = sc(d)
                if s < cur:
                    cur, decl, improved = s, d, True
                    print(cur, [l.strip() for l in decl], flush=True)
                    break
            if improved:
                break
    print('best', cur)
    print('\n'.join(decl))
    return cur, decl


if __name__ == '__main__':
    argv = sys.argv[1:]
    n = None
    if '--climb' in argv:
        k = argv.index('--climb'); n = int(argv[k + 1]); del argv[k:k + 2]
    args = [a for a in argv if not a.startswith('--')]
    fn = args[0]
    if n is not None:
        climb(fn, n, aligned='--positional' not in argv)
    else:
        src = open(args[1], encoding='utf-8').read() if len(args) > 1 else base(fn)
        print(score(fn, src, show='--show' in argv, aligned='--aligned' in argv))
