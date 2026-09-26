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
TOOLS = pathlib.Path(os.environ.get('MWCCDBG_HOME', ROOT.parent))  # both clones sit next to the repo


def main():
    a = sys.argv[1:]
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
    env = os.environ.copy()
    if '/opt/homebrew/bin' not in env.get('PATH', ''):
        env['PATH'] = '/opt/homebrew/bin:' + env.get('PATH', '')
    r = subprocess.run([sys.executable, str(script), '-e', str(emu), '-a', shlex.join(args + ['-sym', 'on']),
                        fn, str(out)], cwd=ROOT, capture_output=True, text=True, env=env)
    if not list(out.glob('*before-regalloc.txt')):
        sys.exit((r.stdout + r.stderr)[-1500:])
    # Summary: each variable's register in priority order (highest first), with its neighbour count.
    for f in sorted(out.glob('regalloc-*-assigned.txt')):
        print('== ' + f.name)
        txt = f.read_text()
        for m in re.finditer(r'^(r\d+|f\d+) -> (\S+)( [^\n]*)?\n(?:.*\n){4}\s*neighbors: (\d+)', txt, re.M):
            if m.group(2) not in ('r0', 'f0') or m.group(3):
                print('  %-5s -> %-4s %3s nb  %s' % (m.group(1), m.group(2), m.group(4), (m.group(3) or '').strip()))
    print('dumps in', out)


if __name__ == '__main__':
    main()
