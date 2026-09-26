"""Dump MWCC's internal state for one function (cadmic/mwcc-debugger): the PCode before and after
each backend pass and the register allocator's priority list, to see WHY a register differs.

    python tools/match/mwccdbg.py <src/Unit.c> <fn> [--src other.c] [--out DIR]

Runs the unit's own compile command (from ninja) with GC/2.6 in place of GC/2.5: the debugger
supports 2.6, and 2.6 compiles all 262 of our units byte-identically to 2.5 (checked 2026-09-26).
--src compiles a scratch copy of the unit instead (same flags). Output: DIR (default
build/mwccdbg/<fn>): backend-NN-*.txt, frontend-*.txt, regalloc-gpr/fpr-pass-N-assigned.txt.
Setup (once per machine) and how to read the dumps: docs/workflow.md "mwcc-debugger"."""
import os, pathlib, re, shlex, shutil, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
# Both clones sit next to the main checkout (a lane's worktree is one level further down).
_HOMES = ([pathlib.Path(os.environ['MWCCDBG_HOME'])] if 'MWCCDBG_HOME' in os.environ else []) + [
    ROOT.parent, ROOT.parent.parent, pathlib.Path.home()]
TOOLS = next((p for p in _HOMES if (p / 'mwcc-debugger').exists()), ROOT.parent)


def batch():
    """--all: every non-exact game function in report.json, one after another (the emulator's gdb
    port is fixed, so no parallel runs); summaries in build/mwccdbg/<fn>/summary.txt."""
    import json
    rep = json.loads((ROOT / 'build/GW4E69/report.json').read_text())
    todo = [(u['metadata']['source_path'], f['name']) for u in rep['units']
            if 'game' in u['metadata'].get('progress_categories', [])
            for f in u.get('functions', []) if f.get('fuzzy_match_percent', 0) != 100]
    for i, (src, fn) in enumerate(todo):
        r = subprocess.run([sys.executable, __file__, src, fn], cwd=ROOT, capture_output=True, text=True)
        bad = r.stdout.count('!EA')
        print('%2d/%d %-34s %s' % (i + 1, len(todo), fn, 'FAILED ' + (r.stdout + r.stderr)[-200:].replace('\n', ' ')
                                  if r.returncode else '%d variables differ' % bad), flush=True)


def main():
    a = sys.argv[1:]
    if a == ['--all']:
        return batch()
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    unit, fn = a[0], a[1]
    out = pathlib.Path(opt('--out', str(ROOT / 'build/mwccdbg' / fn)))
    script = TOOLS / 'mwcc-debugger/mwcc_debugger.py'
    emu = TOOLS / 'retrowin32/target/lto/retrowin32'
    for p in (script, emu):
        if not p.exists():
            sys.exit('%s missing: see docs/workflow.md "mwcc-debugger"' % p)
    cmds = subprocess.run(['ninja', '-t', 'commands', 'build/GW4E69/report.json'],
                          capture_output=True, text=True, cwd=ROOT).stdout.splitlines()
    cmd = next((c for c in cmds if ' -c %s ' % unit in c and 'mwcceppc' in c), None)
    if not cmd:
        sys.exit('no compile command for ' + unit)
    args = shlex.split(cmd.split(' && ')[0])
    args = args[args.index(next(x for x in args if x.endswith('mwcceppc.exe'))):]
    args[0] = args[0].replace('GC/2.5/', 'GC/2.6/')
    args = [x for x in args if x != '-MMD']
    args[args.index('-o') + 1] = str(out / 'obj.o')
    if '--src' in a:
        args[args.index('-c') + 1] = opt('--src', unit)
    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True)
    # The emulator's gdb port (9001) is fixed: one run at a time per machine, others wait here.
    import fcntl, tempfile
    env = os.environ.copy()
    if '/opt/homebrew/bin' not in env.get('PATH', ''):     # macOS: gdb from Homebrew (Gemini, round 4)
        env['PATH'] = '/opt/homebrew/bin:' + env.get('PATH', '')
    with open(os.path.join(tempfile.gettempdir(), 'mwccdbg.lock'), 'w') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        r = subprocess.run([sys.executable, str(script), '-e', str(emu), '-a', shlex.join(args + ['-sym', 'on']),
                            fn, str(out)], cwd=ROOT, capture_output=True, text=True, env=env)
    if not list(out.glob('*before-regalloc*')):
        sys.exit((r.stdout + r.stderr)[-1500:])
    # The emulated run writes no object: compile ours with the build's own command (GC/2.5, wibo).
    real = shlex.split(cmd.split(' && ')[0])
    real = [x for x in real if x != '-MMD']
    real[real.index('-o') + 1] = str(out / 'obj.o')
    if '--src' in a:
        real[real.index('-c') + 1] = opt('--src', unit)
    subprocess.run(real, cwd=ROOT, capture_output=True)
    ea = regmap(unit, fn, out / 'obj.o')
    # Summary: each variable's register in priority order (highest first), with its neighbour count
    # and the register EA has where ours has that one ('!' = differs).
    lines = []
    for f in sorted(out.glob('regalloc-*-assigned.txt')):
        lines.append('== ' + f.name)
        txt = f.read_text()
        for m in re.finditer(r'^(r\d+|f\d+) -> (\S+)( [^\n]*)?\n(?:.*\n){4}\s*neighbors: (\d+)', txt, re.M):
            if m.group(2) not in ('r0', 'f0') or m.group(3):
                want = ea.get(m.group(2), '')
                flag = '!' if want and want != m.group(2) else ' '
                lines.append('  %-5s -> %-4s %sEA %-4s %3s nb  %s' % (
                    m.group(1), m.group(2), flag, want or '?', m.group(4), (m.group(3) or '').strip()))
    lines.append('dumps in %s' % out)
    (out / 'summary.txt').write_text('\n'.join(lines) + '\n')
    print('\n'.join(lines))


def dis(fn, obj):
    """[(mnemonic, args)] of fn in obj (perm_objdump layout)."""
    r = subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_objdump.py'), fn, str(obj)],
                       capture_output=True, text=True)
    return [tuple(l.split('\t')[2:4]) for l in r.stdout.splitlines() if l.count('\t') >= 3]


def regmap(unit, fn, obj):
    """{our register: EA's register}, the most common pairing over instructions that line up."""
    import collections, difflib
    tgt = ROOT / 'build/perm' / fn / 'target.o'
    if not tgt.exists():
        subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_setup.py'),
                        pathlib.Path(unit).stem, fn], cwd=ROOT, capture_output=True)
    if not tgt.exists() or not obj.exists():
        return {}
    a, b = dis(fn, tgt), dis(fn, obj)
    reg = re.compile(r'\b([rf]\d+)\b')
    pairs = collections.defaultdict(collections.Counter)
    sm = difflib.SequenceMatcher(None, [x[0] for x in a], [x[0] for x in b], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'equal':
            continue
        for x, y in zip(a[i1:i2], b[j1:j2]):
            ra, rb = reg.findall(x[1]), reg.findall(y[1])
            if len(ra) == len(rb):
                for p, q in zip(rb, ra):
                    if p not in ('r1', 'r2', 'r13'):
                        pairs[p][q] += 1
    return {k: c.most_common(1)[0][0] for k, c in pairs.items()}


if __name__ == '__main__':
    main()
