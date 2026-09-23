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

    python tools/match/quicktrial.py <fn> [file.c] [--show]

score() compiles with the unit's own compiler and flags (build/perm/<fn>/compile.sh) and compares
with perm_objdump.py, so symbol names are masked as in the permuter. It returns ('ERR', message)
when the compile fails. Use it to sweep hundreds of variants (declaration orders, spellings);
confirm the winner in the real unit with trial.py or a build, since base.c is a snapshot taken by
perm_setup.py.
"""
import os, re, shlex, subprocess, sys, tempfile

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


def score(fn, src, show=False):
    """Differing instructions between src's fn and the original (0 = exact)."""
    if fn not in _target:
        _target[fn] = _dis(fn, os.path.join(_perm(fn), 'target.o'))
    d = tempfile.mkdtemp()
    c, o = os.path.join(d, 'v.c'), os.path.join(d, 'v.o')
    open(c, 'w', encoding='utf-8').write(src)
    r = subprocess.run(_compiler(fn) + ['-c', c, '-o', o], capture_output=True, text=True)
    if not os.path.exists(o):
        return ('ERR', r.stdout[-800:])
    a, b = _target[fn], _dis(fn, o)
    if show:
        for i in range(max(len(a), len(b))):
            x = a[i] if i < len(a) else ''
            y = b[i] if i < len(b) else ''
            print(('  ' if x == y else '* ') + x.ljust(36) + y)
    return sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b))


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if a != '--show']
    fn = args[0]
    src = open(args[1], encoding='utf-8').read() if len(args) > 1 else base(fn)
    print(score(fn, src, show='--show' in sys.argv))
