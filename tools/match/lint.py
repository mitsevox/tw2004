"""Check unit sources against docs/style.md (the mechanical rules only).
    python tools/match/lint.py [files...]        default: every src/*.c (sweeps are exempt)
    python tools/match/lint.py --summary [files]  counts per file and rule instead of each line
    python tools/match/lint.py --diff main       only lines added or changed since main (use this
                                                  on a branch: old debt in untouched lines is skipped)
Exits 1 if anything is found. Rules and the reason for each are in docs/style.md."""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402

SWEEP_DEBT = {}                                      # file name -> lines of uncleaned sweep code

CHECKS = [
    ('m2c-leftover', re.compile(r'\b(temp|var)_[rf]\d+\b|\bM2C_|\bsp[0-9A-F]{1,3}\b|^\s*\?\*? \w+;')),
    ('raw-offset', re.compile(r'\*\s*\(\s*[\w ]+\*\s*\)\s*\(.*\+\s*0x[0-9A-Fa-f]+\s*\)')),
    ('alias-macro', re.compile(r'^#define\s+\w+\s+\(?(lbl|fn)_[0-9A-F]{8}\b')),
    ('tab', re.compile(r'\t')),
    ('trailing-space', re.compile(r'[ \t]+$')),
    ('no-braces', re.compile(r'^\s*(if|for|while)\s*\(.*\)\s*[^{\s;][^{]*;\s*(//.*)?$')),
    ('commented-code', re.compile(r'^\s*//\s*([\w\[\]\.>-]+\s*[-+*/|&]?=[^=][^;]*|\w+\([^)]*\)|return\b[^;]*|(if|for|while)\s*\(.*\)\s*\{?)\s*;?\s*$')),
]
# Portability (the goal is source a PC port can build, 64-bit and little-endian): a pointer squeezed
# into an integer, a copy whose size is a literal instead of sizeof, the frame rate as a bare number.
# A line that must stay this way says why with `port: <why>` on the line or the line before.
PORT = [
    ('port-ptr-int', re.compile(r'\(\s*(int|s32|u32|long)\s*\)\s*(&\s*\w|p[A-Z]\w*\b(?!\s*(\[|->|\.)))')),
    ('port-literal-size', re.compile(r'\b(Mem_cpy|memcpy|memset|fn_80005AE8)\s*\([^;]*,\s*(0x[0-9A-Fa-f]{2,}|\d{3,})\s*\)')),
    ('port-frame-rate', re.compile(r'\b59\.94|\b0\.01668')),
]
# The statement after the condition is return/break/continue. Checked separately: a regex
# lookahead after `\(.*\)` can backtrack to an inner parenthesis of the condition.
EARLY_EXIT = re.compile(r'\)\s*(return\b[^;]*|break|continue);\s*(//.*)?$')


def header_protos():
    """name -> normalized signature, from the top-level headers."""
    out = {}
    for h in (ROOT / 'include').glob('*.h'):
        for m in re.finditer(r'^(?!typedef|#|\s)([\w \*]+?)\b(\w+)\s*\(([^;{}]*)\)\s*;',
                             h.read_text(encoding='utf-8', errors='replace'), re.M):
            out[m.group(2)] = (norm(m.group(1), m.group(3)), h.name)
    return out


def norm(ret, params):
    ps = []
    for p in params.split(','):
        p = ' '.join(p.replace('*', ' * ').split())
        toks = p.split(' ')
        if len(toks) > 1 and re.match(r'^\w+$', toks[-1]) and toks[-1] not in ('void', 'int'):
            toks = toks[:-1]           # drop the parameter name
        ps.append(' '.join(toks))
    return ' '.join(ret.replace('*', ' * ').split()) + '(' + ','.join(ps) + ')'


def lint(path, protos):
    raw = path.read_bytes()
    text = raw.decode('utf-8', errors='replace')
    lines = text.split('\n')
    hits = []
    if b'\r\n' in raw:
        hits.append((1, 'crlf', 'CRLF line endings'))
    if not text.endswith('\n'):
        hits.append((len(lines), 'final-newline', 'no newline at end of file'))
    if not re.match(r'// \w+\.c\b', lines[0]):
        hits.append((1, 'header-comment', 'first line should be "// <File>.c (our name): ..."'))
    raw_sweep = sweepblock.lines_in_blocks(lines)      # style rules wait until the code is cleaned
    SWEEP_DEBT[path.name] = len(raw_sweep)
    for i, l in enumerate(lines, 1):
        l = l.rstrip('\r')
        if i in raw_sweep:
            continue
        for name, rx in CHECKS:
            if name == 'no-braces' and EARLY_EXIT.search(l):
                continue            # a one-line early exit is allowed (style.md, Formatting)
            if rx.search(l):
                hits.append((i, name, l.strip()))
        for name, rx in PORT:
            if rx.search(l) and 'port:' not in l and 'port:' not in lines[i - 2]:
                hits.append((i, name, l.strip()))
        if len(l) > 110:
            hits.append((i, 'long-line', '%d columns' % len(l)))
        if re.search(r'\bgoto\b', l) and 'fake match' not in l and 'fake match' not in lines[i - 2]:
            hits.append((i, 'goto-unmarked', l.strip()))
        m = re.match(r'^(?!typedef|return|#|static)([A-Za-z_][\w \*]*?[\s\*])(\w+)\s*\(([^;{}]*)\)\s*;(.*)$', l)
        if m and m.group(2) in protos:
            sig, hname = protos[m.group(2)]
            if norm(m.group(1), m.group(3)) == sig:
                hits.append((i, 'dup-prototype', '%s is already declared in %s' % (m.group(2), hname)))
            elif '//' not in m.group(4):
                hits.append((i, 'proto-mismatch', '%s differs from %s with no comment saying why'
                             % (m.group(2), hname)))
    return hits + ub_check(path, lines)


# Undefined behaviour the game's own compiler can see. Unlike the style rules, these are bugs in
# the C even when the function matches; one is accepted only with a "fake match:" comment on the
# line or the line before, saying why the original must have had it.
UB = [('ub-missing-return', 'return value expected'),
      ('ub-uninitialized', 'is not initialized before being used')]
CFLAGS = ['-nodefaults', '-proc', 'gekko', '-align', 'powerpc', '-enum', 'int', '-fp', 'hardware',
          '-Cpp_exceptions', 'off', '-O4,p', '-inline', 'smart', '-nosyspath', '-RTTI', 'off',
          '-fp_contract', 'on', '-str', 'reuse', '-common', 'on', '-multibyte', '-lang=c',
          '-i', 'include', '-i', 'build/GW4E69/include', '-DBUILD_VERSION=0', '-DVERSION_GW4E69',
          '-DVERSION=0', '-DNDEBUG=1', '-w', 'all', '-msgstyle', 'gcc']


def is_definition(lines, i):
    """Whether line i (1-based) ends a function definition's header. The compiler reports the line
    of the opening brace, and a header can wrap: walk back over the brace line and indented
    continuation lines to the header's first line (column 0; statements are indented)."""
    j = i
    if lines[j - 1].strip() == '{' and j > 1:
        j -= 1
    while j > 1 and lines[j - 1][:1].isspace():
        j -= 1
    head = ' '.join(l.strip() for l in lines[j - 1:i])
    return re.match(r'^[A-Za-z_][\w \*]*\b\w+\s*\([^;{}]*\)\s*\{?\s*$', head) is not None


def ub_check(path, lines):
    import subprocess, tempfile
    cc = ROOT / 'build/compilers/GC/2.5/mwcceppc.exe'
    with tempfile.TemporaryDirectory() as tmp:
        out = subprocess.run([str(cc)] + CFLAGS + ['-c', str(path.resolve()), '-o', tmp + '/x.o'],
                             cwd=ROOT, capture_output=True, text=True)
    hits = []
    if out.returncode:
        # The checks below read compiler warnings; a file that does not compile here gives none,
        # so a failure must be a finding, not a silent pass.
        err = [l for l in (out.stdout + out.stderr).splitlines()
               if re.match(r'.*?:\d+: ', l) and ': warning:' not in l]
        m = re.match(r'(.*?):(\d+): (.*)', err[0]) if err else None
        hits.append((int(m.group(2)) if m and pathlib.Path(m.group(1)).name == path.name else 1,
                     'compile-error', m.group(3) if m else 'the compiler failed on this file'))
        return hits
    # Calls with no prototype in scope: the compiler assumes `int` (a float result is then read as
    # an int). -requireprotos also flags definitions without an earlier declaration; skip those.
    with tempfile.TemporaryDirectory() as tmp:
        rp = subprocess.run([str(cc)] + CFLAGS + ['-requireprotos', '-maxerrors', '1000', '-c',
                            str(path.resolve()), '-o', tmp + '/x.o'], cwd=ROOT, capture_output=True, text=True)
    seen = set()
    for l in (rp.stdout + rp.stderr).splitlines():
        m = re.match(r'(.*?):(\d+):(?: warning:)? function has no prototype', l)
        if not m or pathlib.Path(m.group(1)).name != path.name:
            continue
        i = int(m.group(2))
        src = lines[i - 1] if i <= len(lines) else ''
        if i in seen or is_definition(lines, i):
            continue            # a definition, not a call
        seen.add(i)
        if 'fake match' not in src:
            hits.append((i, 'ub-no-prototype', src.strip()))
    for l in (out.stdout + out.stderr).splitlines():
        m = re.match(r'(.*?):(\d+): warning: (.*)', l)
        if not m or pathlib.Path(m.group(1)).name != path.name:
            continue
        for name, text in UB:
            if text in m.group(3):
                i = int(m.group(2))
                near = lines[i - 1] + (lines[i - 2] if i > 1 else '')
                if 'fake match' not in near:
                    hits.append((i, name, lines[i - 1].strip()))
    return hits


def changed_lines(rev):
    """file name -> set of line numbers added or changed since rev (working tree included)."""
    import subprocess
    # Diff against the merge base, so lines that changed on `rev` after this branch left it are
    # not counted as this branch's.
    base = subprocess.run(['git', 'merge-base', rev, 'HEAD'], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip() or rev
    out = subprocess.run(['git', 'diff', '-U0', base, '--', 'src'], cwd=ROOT,
                         capture_output=True, text=True).stdout
    res, cur = {}, None
    for l in out.splitlines():
        if l.startswith('+++ '):
            cur = pathlib.Path(l[6:]).name if l != '+++ /dev/null' else None
            res.setdefault(cur, set())
        m = re.match(r'@@ -\S+ \+(\d+)(?:,(\d+))? @@', l)
        if m and cur:
            a, n = int(m.group(1)), int(m.group(2) or 1)
            res[cur].update(range(a, a + n))
    # untracked files (a new unit before `git add`) are all new lines
    new = subprocess.run(['git', 'ls-files', '--others', '--exclude-standard', '--', 'src'], cwd=ROOT,
                         capture_output=True, text=True).stdout.split()
    for f in new:
        n = len((ROOT / f).read_text(encoding='utf-8', errors='replace').split('\n'))
        res[pathlib.Path(f).name] = set(range(1, n + 1))
    return res


def main():
    argv = sys.argv[1:]
    rev = None
    if '--diff' in argv:
        k = argv.index('--diff'); rev = argv[k + 1]; del argv[k:k + 2]
    args = [a for a in argv if a != '--summary']
    files = [pathlib.Path(a) for a in args] or sorted((ROOT / 'src').glob('*.c'))
    protos = header_protos()
    changed = changed_lines(rev) if rev else None
    total = 0
    for f in files:
        if 'unsorted' in f.parts:
            continue
        if changed is not None and f.name not in changed:
            continue
        hits = lint(f, protos)
        if changed is not None:     # file-level checks (line 1) count only if line 1 changed too
            hits = [h for h in hits if h[0] in changed[f.name]]
        total += len(hits)
        if '--summary' in sys.argv:
            if hits:
                counts = {}
                for _, n, _ in hits:
                    counts[n] = counts.get(n, 0) + 1
                print('%-28s %s' % (f.name, ', '.join('%s %d' % kv for kv in sorted(counts.items()))))
        else:
            for ln, n, msg in hits:
                print('%s:%d: %s: %s' % (f.name, ln, n, msg[:100]))
    debt = {f: n for f, n in SWEEP_DEBT.items() if n}
    if debt and changed is None:
        print('%d lines of sweep code not yet cleaned up, in %d files (tools/match/sweepblock.py)'
              % (sum(debt.values()), len(debt)))
    print(total, 'findings')
    sys.exit(1 if total else 0)


if __name__ == '__main__':
    main()
