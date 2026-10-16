"""Check unit sources against docs/style.md (the mechanical rules only).
    python tools/quality/lint.py [files...]        default: every src/*.c (sweeps are exempt) and
                                                  every header's long lines; named files: only them
    python tools/quality/lint.py --summary [files]  counts per file and rule instead of each line
    python tools/quality/lint.py --diff main       only what the branch added since main (use this
                                                  on a branch): findings on lines it added or
                                                  changed that the file did not already have, so
                                                  editing a line with old debt on it adds nothing
    python tools/quality/lint.py --diff main --compile   also compile every unit the branch touches
                                                  (changed .c files and units including a changed
                                                  header) with its real command: the DOL build never
                                                  compiles a NonMatching unit. `ninja all_source`
                                                  compiles every unit.
Exits 1 if anything is found. Rules and the reason for each are in docs/style.md."""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path[:0] = [str(ROOT / 'tools' / d) for d in ('build', 'naming', 'quality', 'matching')]
import sweepblock                                    # noqa: E402
import includes                                      # noqa: E402
from hosttools import NINJA_SEP, mwcc, reported_name  # noqa: E402

SWEEP_DEBT = {}                                      # file name -> lines of uncleaned sweep code
COMPILE_ERRORS = set()                               # (file name, line) the style pass reported

CHECKS = [
    ('m2c-leftover', re.compile(r'\b(temp|var)_[rf]\d+\b|\bM2C_|\bsp[0-9A-F]{1,3}\b|^\s*\?\*? \w+;')),
    ('raw-offset', re.compile(r'\*\s*\(\s*[\w ]+\*\s*\)\s*\(.*\+\s*0x[0-9A-Fa-f]+\s*\)')),
    ('alias-macro', re.compile(r'^#define\s+\w+\s+\(?(lbl|fn)_[0-9A-F]{8}\b')),
    ('tab', re.compile(r'\t')),
    ('trailing-space', re.compile(r'[ \t]+$')),
    ('no-braces', re.compile(r'^\s*(if|for|while)\s*\(.*\)\s*[^{\s;][^{]*;\s*(//.*)?$')),
    ('old-label', re.compile(r'//.*(\bport:|\bEA bug:)')),
    ('placeholder-name', re.compile(r'\b(fn|lbl)_[0-9A-F]{8}')),
    ('comment-other-build', re.compile(r'(//|/\*).*\b(TW0[67]|PDB|DWARF)\b')),
    ('comment-address', re.compile(r'(//|/\*).*\b0x80[0-9A-Fa-f]{6}\b')),
    ('comment-register', re.compile(r"(//|/\*).*(?<!\.)(?<!->)(?<!'s )\b[rf]([0-9]|[12][0-9]|3[01])\b")),
    ('commented-code', re.compile(r'^\s*//\s*([\w\[\]\.>-]+\s*[-+*/|&]?=[^=][^;]*|\w+\([^)]*\)|return\b[^;]*|(if|for|while)\s*\(.*\)\s*\{?)\s*;?\s*$')),
]
CAST_CALL = re.compile(r'\(\s*\([\w\s\*]+\(\s*\*\s*\)\s*\([^()]*\)\s*\)\s*([A-Za-z_]\w*)\s*\)\s*\(')
# The statement after the condition is return/break/continue. Checked separately: a regex
# lookahead after `\(.*\)` can backtrack to an inner parenthesis of the condition.
EARLY_EXIT = re.compile(r'\)\s*(return\b[^;]*|break|continue);\s*(//.*)?$')


_USERS = None


def users(name):
    """The source files (src/ .c, include/ .h) whose code (not comments) uses the word name."""
    global _USERS
    if _USERS is None:
        _USERS = {}
        for f in list((ROOT / 'src').rglob('*.c')) + list((ROOT / 'include').rglob('*.h')):
            code = re.sub(r'//[^\n]*|/\*.*?\*/', '', f.read_text(encoding='utf-8', errors='replace'), flags=re.S)
            for w in set(re.findall(r'[A-Za-z_]\w*', code)):
                _USERS.setdefault(w, set()).add(f.resolve())
    return _USERS.get(name, set())


def header_protos():
    """name -> [(normalized signature, header path)], from every header under include/. lint()
    compares a file only with the headers it includes, directly or not (includes.py)."""
    out = {}
    for h in sorted((ROOT / 'include').rglob('*.h')):
        for m in re.finditer(r'^(?!typedef|#|\s)([\w \*]+?)\b(\w+)\s*\(([^;{}]*)\)\s*;',
                             h.read_text(encoding='utf-8', errors='replace'), re.M):
            out.setdefault(m.group(2), []).append((norm(m.group(1), m.group(3)), h.resolve()))
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
        hits.append((1, 'header-comment', 'first line should be "// <File>.c: what the file does in the game"'))
    raw_sweep = sweepblock.lines_in_blocks(lines)      # style rules wait until the code is cleaned
    seen = includes.seen_headers(path)
    defined = {m.group(1) for m in re.finditer(
        r'^(?!typedef|return|#)[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;]*$', text, re.M)}
    SWEEP_DEBT[path.name] = len(raw_sweep)
    for i, l in enumerate(lines, 1):
        l = l.rstrip('\r')
        if i in raw_sweep:
            continue
        for name, rx in CHECKS:
            if name == 'no-braces' and EARLY_EXIT.search(l):
                continue            # a one-line early exit is allowed (style.md, Formatting)
            if name == 'comment-register' and 'fake match' in l:
                continue            # a fake-match tag may name the register its trick pins
            if rx.search(l):
                hits.append((i, name, l.strip()))
        if len(l) > 110:
            hits.append((i, 'long-line', '%d columns' % len(l)))
        c = CAST_CALL.search(l)
        if c:
            hits.append((i, 'cast-call', '%s is called through a cast: make its definition and prototype '
                         'take what the callers pass' % c.group(1)))
        m = re.match(r'^(?!typedef|return|#|static)([A-Za-z_][\w \*]*?[\s\*])(\w+)\s*\(([^;{}]*)\)\s*;(.*)$', l)
        # only the headers this file includes count: one it does not see declares nothing here
        decl = [(sig, h) for sig, h in protos.get(m.group(2), []) if h in seen] if m else []
        if decl:
            same = [h for sig, h in decl if sig == norm(m.group(1), m.group(3))]
            if same:
                hits.append((i, 'dup-prototype', '%s is already declared in %s'
                             % (m.group(2), includes.label(same[0]))))
            else:
                hits.append((i, 'proto-mismatch', '%s differs from %s: one prototype, with the '
                             'definition\'s types' % (m.group(2), includes.label(decl[0][1]))))
        elif m and m.group(2) not in defined:
            hits.append((i, 'foreign-prototype', '%s belongs in the header of the file that defines it'
                         % m.group(2)))
        elif m and users(m.group(2)) - {path.resolve()}:
            hits.append((i, 'foreign-prototype', '%s is used by other files: declare it in this file\'s '
                         'header' % m.group(2)))
        elif m:
            hits.append((i, 'should-be-static', 'only this file uses %s: make it static (scope:local '
                         'in symbols.txt), not a header prototype' % m.group(2)))
    return hits + ub_check(path, lines)


# Undefined behaviour the game's own compiler can see. Unlike the style rules, these are bugs in
# the C even when the function matches. One is accepted only when EA wrote it (the bytes require
# it) and it is listed in docs/game/ea-bugs.md.
UB = [('ub-missing-return', 'return value expected'),
      ('ub-uninitialized', 'is not initialized before being used')]
CFLAGS = ['-nodefaults', '-proc', 'gekko', '-align', 'powerpc', '-enum', 'int', '-fp', 'hardware',
          '-Cpp_exceptions', 'off', '-O4,p', '-inline', 'smart', '-nosyspath', '-RTTI', 'off',
          '-fp_contract', 'on', '-str', 'reuse', '-common', 'on', '-multibyte', '-lang=c',
          '-i', 'include', '-i', 'build/GW4E69/include', '-i', 'build/GW4E69/gen',
          '-DBUILD_VERSION=0', '-DVERSION_GW4E69',
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


def enclosing_function(lines, i):
    """The name of the function whose body holds line i (1-based), or None."""
    for j in range(i - 1, -1, -1):
        m = re.match(r'^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;]*$', lines[j])
        if m and not lines[j].startswith(('typedef', 'return')):
            return m.group(1)
    return None


_EA_BUGS = None


def ea_bugs():
    """{(file name, function)} from the Where column of docs/game/ea-bugs.md."""
    global _EA_BUGS
    if _EA_BUGS is None:
        f = ROOT / 'docs/game/ea-bugs.md'
        text = f.read_text(encoding='utf-8') if f.exists() else ''
        _EA_BUGS = set(re.findall(r'^\| (\S+\.c) (\w+)', text, re.M))
    return _EA_BUGS


def ub_check(path, lines):
    import subprocess, tempfile
    cc = mwcc()
    with tempfile.TemporaryDirectory() as tmp:
        out = subprocess.run(cc + CFLAGS + ['-maxerrors', '1000', '-c', str(path.resolve()),
                             '-o', tmp + '/x.o'], cwd=ROOT, capture_output=True, text=True)
    hits = []
    # With -w all, a call to a function with no declaration in scope (an implicit declaration: the
    # compiler assumes `int f()`) is an error, not a warning. It is reported as ub-no-prototype at
    # the call, once per function (later calls see the implicit declaration), and the file is still
    # read to the end, so the warning checks below keep working.
    err, implicit = [], set()
    for l in (out.stdout + out.stderr).splitlines():
        m = re.match(r'(.*?):(\d+): (?!warning:)(.*)', l)
        if not m:
            continue
        if m.group(3).startswith('function has no prototype') and reported_name(m.group(1)) == path.name:
            i = int(m.group(2))
            src = lines[i - 1] if i <= len(lines) else ''
            implicit.add(i)
            hits.append((i, 'ub-no-prototype', 'implicit declaration: ' + src.strip()))
        else:
            err.append(m)
    if out.returncode and (err or not implicit):
        # The checks below read compiler warnings; a file that does not compile here gives none,
        # so a failure must be a finding, not a silent pass.
        m = err[0] if err else None
        hits.append((int(m.group(2)) if m and reported_name(m.group(1)) == path.name else 1,
                     'compile-error', m.group(3) if m else 'the compiler failed on this file'))
        return hits
    # Calls with no prototype in scope: the compiler assumes `int` (a float result is then read as
    # an int). -requireprotos also flags definitions without an earlier declaration; skip those.
    with tempfile.TemporaryDirectory() as tmp:
        rp = subprocess.run(cc + CFLAGS + ['-requireprotos', '-maxerrors', '1000', '-c',
                            str(path.resolve()), '-o', tmp + '/x.o'], cwd=ROOT, capture_output=True, text=True)
    seen = set(implicit)
    for l in (rp.stdout + rp.stderr).splitlines():
        m = re.match(r'(.*?):(\d+):(?: warning:)? function has no prototype', l)
        if not m or reported_name(m.group(1)) != path.name:
            continue
        i = int(m.group(2))
        src = lines[i - 1] if i <= len(lines) else ''
        if i in seen or is_definition(lines, i):
            continue            # a definition, not a call
        seen.add(i)
        hits.append((i, 'ub-no-prototype', src.strip()))
    for l in (out.stdout + out.stderr).splitlines():
        m = re.match(r'(.*?):(\d+): warning: (.*)', l)
        if not m or reported_name(m.group(1)) != path.name:
            continue
        for name, text in UB:
            if text in m.group(3):
                i = int(m.group(2))
                # EA's own bug, kept because the bytes require it, is listed in the register
                ea = (path.name, enclosing_function(lines, i)) in ea_bugs()
                if not ea:
                    hits.append((i, name, lines[i - 1].strip()))
    return hits


def header_long_lines(rev):
    """(include path, line, columns) for every header line past 110 columns: all of them without
    rev, else only those the branch added since rev's merge base (working tree included). A long
    line that replaces a long line in the same change (an edit to a line that was already long)
    is old debt, not the branch's."""
    import subprocess
    if rev is None:
        return [(h.relative_to(ROOT).as_posix(), i, len(l.rstrip('\r')))
                for h in sorted((ROOT / 'include').rglob('*.h'))
                for i, l in enumerate(h.read_text(encoding='utf-8', errors='replace').split('\n'), 1)
                if len(l.rstrip('\r')) > 110]
    out = subprocess.run(['git', 'diff', '-B', '-M', '-U0', merge_base(rev), '--', 'include'], cwd=ROOT,
                         capture_output=True, text=True).stdout
    res, cur, ln, old_long = [], None, 0, 0
    for l in out.splitlines():
        if l.startswith(('+++ ', '--- ')):
            if l.startswith('+++ '):
                cur = l[6:] if l != '+++ /dev/null' else None
            continue
        m = re.match(r'@@ -\S+ \+(\d+)', l)
        if m:
            ln, old_long = int(m.group(1)), 0
            continue
        if l.startswith('-') and len(l) - 1 > 110:
            old_long += 1                           # -U0: a change's old lines come before its new
        elif l.startswith('+') and cur:
            if len(l) - 1 > 110:
                if old_long:
                    old_long -= 1
                else:
                    res.append((cur, ln, len(l) - 1))
            ln += 1
    return res


def changed_lines(rev):
    """file name -> set of line numbers added or changed since rev (working tree included)."""
    import subprocess
    # Diff against the merge base, so lines that changed on `rev` after this branch left it are
    # not counted as this branch's.
    base = subprocess.run(['git', 'merge-base', rev, 'HEAD'], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip() or rev
    out = subprocess.run(['git', 'diff', '-B', '-M', '-U0', base, '--', 'src'], cwd=ROOT,
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


def merge_base(rev):
    import subprocess
    return subprocess.run(['git', 'merge-base', rev, 'HEAD'], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip() or rev


def base_paths(rev, folder):
    """path now (relative to ROOT) -> its path at rev's merge base, for each file under folder the
    branch changed or renamed. A file the branch added has no entry."""
    import subprocess
    out = subprocess.run(['git', 'diff', '-B', '-M', '--name-status', merge_base(rev), '--', folder],
                         cwd=ROOT, capture_output=True, text=True).stdout
    res = {}
    for l in out.splitlines():
        f = l.split('\t')
        if f[0].startswith('R'):
            res[f[2]] = f[1]
        elif f[0][:1] in 'MT':
            res[f[1]] = f[1]
    return res


def base_text(rev, path):
    """The bytes of path (relative to ROOT) at rev's merge base."""
    import subprocess
    return subprocess.run(['git', 'show', '%s:%s' % (merge_base(rev), path)], cwd=ROOT,
                          capture_output=True).stdout


# Rules whose findings are told apart only by how many a file has: the line's text is no help,
# because editing the line changes it (and the compiler wraps its messages at a width that
# counts the file's path).
COUNTED = {'long-line', 'trailing-space', 'tab', 'crlf', 'final-newline', 'header-comment',
           'compile-error'}


def finding_key(rule, msg):
    """What identifies a finding in two versions of a file, apart from its line number: for a
    pattern rule the text it matched (a placeholder name, a register), else the message."""
    if rule in COUNTED:
        return rule, ''
    rx = dict(CHECKS).get(rule)
    m = rx.search(msg) if rx else None
    return rule, m.group(0) if m else msg


def new_hits(f, hits, changed, rev, old, protos):
    """The findings on changed lines the file did not have at rev's merge base. Each finding of
    the old file pays for one finding on a changed line with the same key; findings on untouched
    lines pay first, since they are the same ones."""
    import collections, tempfile
    budget = collections.Counter()
    if old:
        with tempfile.TemporaryDirectory() as tmp:
            p = pathlib.Path(tmp) / f.name           # same name: compiler messages are matched by it
            p.write_bytes(base_text(rev, old))
            budget.update(finding_key(r, m) for _, r, m in lint(p, protos))
    for ln, r, m in hits:
        if ln not in changed and r != 'compile-error':
            budget[finding_key(r, m)] -= 1
    out = []
    for h in hits:
        # a compile error counts wherever it is: an edit anywhere can cause it
        if h[0] in changed or h[1] == 'compile-error':
            k = finding_key(h[1], h[2])
            if budget[k] > 0:
                budget[k] -= 1
            else:
                out.append(h)
    return out


def touched_units(rev):
    """The src/ units a branch touches since it left rev: .c files it changed, and every unit that
    includes (directly or not) a header it changed. Paths relative to src/, without .c."""
    import subprocess
    base = subprocess.run(['git', 'merge-base', rev, 'HEAD'], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip() or rev
    names = subprocess.run(['git', 'diff', '-B', '-M', '--name-only', base, '--', 'src', 'include'], cwd=ROOT,
                           capture_output=True, text=True).stdout.split()
    names += subprocess.run(['git', 'ls-files', '--others', '--exclude-standard', '--', 'src',
                             'include'], cwd=ROOT, capture_output=True, text=True).stdout.split()
    units = {n[4:-2] for n in names if n.startswith('src/') and n.endswith('.c')
             and (ROOT / n).exists()}
    headers = {(ROOT / n).resolve() for n in names if n.endswith('.h') and (ROOT / n).exists()}
    if headers:
        for c in (ROOT / 'src').rglob('*.c'):
            if includes.seen_headers(c) & headers:
                units.add(c.relative_to(ROOT / 'src').with_suffix('').as_posix())
    return sorted(units)


def compile_units(units):
    """Build each unit's object with its own build.ninja command (its compiler version and flags),
    through ninja, so a NonMatching unit, which the DOL build never compiles, is checked too.
    -> [(file name, line, 'compile-error', message)]"""
    import subprocess
    ninja = (ROOT / 'build.ninja').read_text(encoding='utf-8', errors='replace')

    def target(u):
        return NINJA_SEP.join(['build', 'GW4E69', 'src', '%s.o' % u.replace('/', NINJA_SEP)])
    # A .c file #included into another unit (char_tex_manager.c into char.c) is checked by
    # compiling the unit that includes it.
    units = set(units)
    for u in [u for u in units if ('build %s:' % target(u)) not in ninja]:
        f = (ROOT / 'src' / (u + '.c')).resolve()
        hosts = {c.relative_to(ROOT / 'src').with_suffix('').as_posix()
                 for c in (ROOT / 'src').rglob('*.c') if f in includes.seen_headers(c)}
        if hosts:
            units = (units - {u}) | hosts
    targets, missing = [], []
    for u in sorted(units):
        t = target(u)
        (targets if ('build %s:' % t) in ninja else missing).append((u, t))
    out = []
    for u, _ in missing:
        out.append((u + '.c', 1, 'compile-error', 'not in build.ninja: not a unit in configure.py '
                    '(or run python configure.py)'))
    if not targets:
        return out
    r = subprocess.run(['ninja', '-k', '0'] + [t for _, t in targets], cwd=ROOT,
                       capture_output=True, text=True)
    failed, cur, msg = {}, None, []
    for l in r.stdout.splitlines() + ['[end]']:
        m = re.match(r'^FAILED: (?:\[code=\d+\] )?(\S+)', l)
        if m or re.match(r'^\[\d+/\d+\]|^\[end\]|^ninja:', l):
            if cur:
                failed[cur] = msg
            cur, msg = (m.group(1).replace('/', NINJA_SEP) if m else None), []
        elif cur:
            msg.append(l)
    for u, t in targets:
        if t in failed:
            text = '\n'.join(failed[t])
            ln = re.search(r'^#\s+(\d+): ', text, re.M)
            err = [x.lstrip('#').strip() for x in text.split('Error:', 1)[-1].splitlines()[1:]]
            err = [x for x in err if x and not x.startswith(('Too many errors', 'User break'))]
            msg = ' / '.join(err[:3]) or 'the unit does not compile'
            where = re.search(r'^#\s+File: (\S+)', text, re.M)
            at = int(ln.group(1)) if ln else 1
            if where and reported_name(where.group(1)) != pathlib.Path(u).name + '.c':
                msg = 'in %s:%d: %s' % (where.group(1).replace('\\', '/'), at, msg)
                at = 1                              # the error is in a header it includes
            out.append((u + '.c', at, 'compile-error', msg))
    if r.returncode and not failed and not out:
        out.append(('(ninja)', 1, 'compile-error', (r.stdout + r.stderr).strip()[-200:]))
    return out


def main():
    argv = sys.argv[1:]
    rev = None
    if '--diff' in argv:
        k = argv.index('--diff'); rev = argv[k + 1]; del argv[k:k + 2]
    args = [a for a in argv if a not in ('--summary', '--compile')]
    files = [pathlib.Path(a) for a in args] or sorted((ROOT / 'src').rglob('*.c'))
    protos = header_protos()
    changed = changed_lines(rev) if rev else None
    olds = base_paths(rev, 'src') if rev else {}
    total = 0
    for f in files:
        if 'unsorted' in f.parts:
            continue
        if changed is not None and f.name not in changed:
            continue
        hits = lint(f, protos)
        if changed is not None:
            rel = f.resolve().relative_to(ROOT).as_posix()
            hits = new_hits(f, hits, changed[f.name], rev, olds.get(rel), protos)
        total += len(hits)
        COMPILE_ERRORS.update((f.name, h[0]) for h in hits if h[1] == 'compile-error')
        if '--summary' in sys.argv:
            if hits:
                counts = {}
                for _, n, _ in hits:
                    counts[n] = counts.get(n, 0) + 1
                print('%-28s %s' % (f.name, ', '.join('%s %d' % kv for kv in sorted(counts.items()))))
        else:
            for ln, n, msg in hits:
                print('%s:%d: %s: %s' % (f.name, ln, n, msg[:100]))
    # headers: long lines only (the C style rules above are for .c files); with --diff, only the
    # ones the branch added; with named files and no --diff, none (they are not the files asked for).
    for f, ln, n in (header_long_lines(rev) if rev or not args else []):
        print('%s:%d: long-line: %d columns' % (f, ln, n))
        total += 1
    if '--compile' in sys.argv:
        units = touched_units(rev) if rev else None
        hits = compile_units(units) if units else []
        for f, ln, n, msg in list(hits):         # already reported by the style pass above
            if (pathlib.Path(f).name, ln) in COMPILE_ERRORS:
                hits.remove((f, ln, n, msg))
        for f, ln, n, msg in hits:
            print('%s:%d: %s: %s' % (f, ln, n, msg[:200]))
        total += len(hits)
        print('compiled %s with their own build commands, %d failed'
              % ('%d touched units' % len(units) if units is not None else 'nothing (no --diff)',
                 len(hits)))
    if rev and '--compile' not in sys.argv:
        print('(--compile also builds every unit this branch touches, NonMatching ones included; '
              '`ninja all_source` builds all of them)')
    debt = {f: n for f, n in SWEEP_DEBT.items() if n}
    if debt and changed is None:
        print('%d lines of sweep code not yet cleaned up, in %d files (tools/quality/sweepblock.py)'
              % (sum(debt.values()), len(debt)))
    print(total, 'findings')
    sys.exit(1 if total else 0)


if __name__ == '__main__':
    main()
