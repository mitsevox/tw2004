"""Check unit sources against docs/style.md (the mechanical rules only).
    python tools/match/lint.py [files...]        default: every src/*.c (sweeps are exempt)
    python tools/match/lint.py --summary [files]  counts per file and rule instead of each line
    python tools/match/lint.py --diff main       only lines added or changed since main (use this
                                                  on a branch: old debt in untouched lines is skipped)
    python tools/match/lint.py --diff main --compile   also compile every unit the branch touches
                                                  (changed .c files and units including a changed
                                                  header) with its real command: the DOL build never
                                                  compiles a NonMatching unit. `ninja all_source`
                                                  compiles every unit.
Exits 1 if anything is found. Rules and the reason for each are in docs/style.md."""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
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
# port-asm-no-fallback: CodeWarrior-only code with no plain-C version for a port. An `asm` function
# must sit under `#ifdef __MWERKS__` with an `#else` holding a C version; a compiler intrinsic
# (__cntlzw, ...) needs a C fallback in game_types.h or platform.h. Exempt with `port: <why>`.
INTRINSIC = re.compile(r'\b(__cntlzw|__rlwimi|__rlwinm|__lwbrx|__lhbrx|__stwbrx|__sthbrx|__fabs|__fnabs|'
                       r'__frsqrte|__fres|__fmadd|__fmsub|__fnmadd|__fnmsub|__fsel|__dcbf|__dcbt|__dcbz|'
                       r'__dcbst|__icbi|__sync|__eieio|__isync)\s*\(')


def intrinsic_fallbacks():
    """Intrinsics that game_types.h or platform.h define as plain C for a port."""
    out = set()
    for h in ('game_types.h', 'platform.h'):
        f = ROOT / 'include' / h
        if f.exists():
            t = f.read_text(encoding='utf-8', errors='replace')
            out |= set(re.findall(r'^(?:static\s+inline\s+[\w \*]+?|#define\s+)(__\w+)\s*\(', t, re.M))
    return out


def asm_fallback_hits(lines):
    """Lines of asm functions and intrinsics with no plain-C version (port-asm-no-fallback)."""
    hits, stack, have = [], [], intrinsic_fallbacks()
    for i, l in enumerate(lines, 1):
        s = l.strip()
        exempt = 'port:' in l or (i > 1 and 'port:' in lines[i - 2])
        if re.match(r'#\s*if', s):
            stack.append({'mw': re.match(r'#\s*(ifdef\s+__MWERKS__|if\s+defined\s*\(?\s*__MWERKS__)', s)
                          is not None, 'else': False, 'asm': []})
        elif re.match(r'#\s*(else|elif)', s) and stack:
            stack[-1]['else'] = True
        elif re.match(r'#\s*endif', s) and stack:
            blk = stack.pop()
            if not blk['else']:
                hits += blk['asm']
        elif re.match(r'^asm\b', l) and not exempt:
            blk = next((b for b in reversed(stack) if b['mw']), None)
            hit = (i, 'port-asm-no-fallback', s)
            if blk is None:
                hits.append(hit)
            elif not blk['else']:
                blk['asm'].append(hit)
        else:
            m = INTRINSIC.search(l)
            if m and m.group(1) not in have and not exempt and not s.startswith('//'):
                hits.append((i, 'port-asm-no-fallback', s))
    return hits


# The statement after the condition is return/break/continue. Checked separately: a regex
# lookahead after `\(.*\)` can backtrack to an inner parenthesis of the condition.
EARLY_EXIT = re.compile(r'\)\s*(return\b[^;]*|break|continue);\s*(//.*)?$')


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
        hits.append((1, 'header-comment', 'first line should be "// <File>.c (our name): ..."'))
    raw_sweep = sweepblock.lines_in_blocks(lines)      # style rules wait until the code is cleaned
    seen = includes.seen_headers(path)
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
        # only the headers this file includes count: one it does not see declares nothing here
        decl = [(sig, h) for sig, h in protos.get(m.group(2), []) if h in seen] if m else []
        if decl:
            same = [h for sig, h in decl if sig == norm(m.group(1), m.group(3))]
            if same:
                hits.append((i, 'dup-prototype', '%s is already declared in %s'
                             % (m.group(2), includes.label(same[0]))))
            elif '//' not in m.group(4):
                hits.append((i, 'proto-mismatch', '%s differs from %s with no comment saying why'
                             % (m.group(2), includes.label(decl[0][1]))))
    hits += [h for h in asm_fallback_hits(lines) if h[0] not in raw_sweep]
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
            if 'fake match' not in src:
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
        if 'fake match' not in src:
            hits.append((i, 'ub-no-prototype', src.strip()))
    for l in (out.stdout + out.stderr).splitlines():
        m = re.match(r'(.*?):(\d+): warning: (.*)', l)
        if not m or reported_name(m.group(1)) != path.name:
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


def touched_units(rev):
    """The src/ units a branch touches since it left rev: .c files it changed, and every unit that
    includes (directly or not) a header it changed. Paths relative to src/, without .c."""
    import subprocess
    base = subprocess.run(['git', 'merge-base', rev, 'HEAD'], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip() or rev
    names = subprocess.run(['git', 'diff', '--name-only', base, '--', 'src', 'include'], cwd=ROOT,
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
    targets, missing = [], []
    for u in units:
        t = NINJA_SEP.join(['build', 'GW4E69', 'src', '%s.o' % u.replace('/', NINJA_SEP)])
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
            hits = [h for h in hits if h[0] in changed[f.name] or h[1] == 'compile-error']
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
        print('%d lines of sweep code not yet cleaned up, in %d files (tools/match/sweepblock.py)'
              % (sum(debt.values()), len(debt)))
    print(total, 'findings')
    sys.exit(1 if total else 0)


if __name__ == '__main__':
    main()
