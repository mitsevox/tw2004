"""Check leversweep.py against functions already solved on main: each is swept from its source just
before its fix (git), and the sweep should find an exact variant, or at least beat the old score.

    python tools/match/leversweep_selftest.py [--minutes 6] [-j N] [--only fn,fn]

Checks out the old src/<Unit>.c for each case, sweeps it, and puts the current file back
(`git checkout HEAD -- src/<Unit>.c`). Run it in a worktree with no uncommitted work in src/.
Results: build/leversweep/<fn>.txt and summary.tsv, and one line per case here."""
import pathlib, subprocess, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]

# (unit, function, commit whose src/<unit>.c is the version before the fix, the fix's levers)
CASES = [
    ('GoBreakLine', 'BreakLine_Reset', '69fc1e4^1', 'identity inline on a pointer'),
    ('GoShaderObjectCommon_ShaderObjectsData_Gc', 'fn_80074628', '6d79760^1', 'identity inline on a u16'),
    ('GoDynObj', 'fn_8004731C', 'a2d0373^1', 'type int -> s32 + a declaration move (a pair)'),
    ('GoTerrain', 'fn_80032518', 'a66182b^1', '(u32) index cast'),
    ('target', 'PlaceBall_UpdateMomentums', '9a375fa^1', '*= splits + a declaration move'),
    ('GoShaderObject_Glows_Gc', 'fn_800981D0', '1745a20^1', 'do/while as a loop'),
    ('UObject', 'fn_800488B4', '653997b^1', 'ternary temp + a field address taken early (decomp.me SOh7Q)'),
    ('GameHoleContests', 'fn_800DA6D0', '854e8e9^1', '(u32) index cast + a new loop variable (partly out of reach)'),
    ('Golfer', 'AI_ChooseTarget', '2afeac0^1', 'three adjacent assignments swapped (partial: registers stay)'),
    ('target', 'TARGET_Init', '9a375fa^1', 'a u64 local for a call result (out of reach)'),
    ('GameMode26', 'fn_8010CA2C', 'a2d0373^1', 'parameter type + declaration move (out of reach)'),
    ('uiLoadFile', 'fn_8008EFC0', 'a9e160c^1', 'add through a new local (out of reach)'),
]


def main():
    args = sys.argv[1:]
    minutes = args[args.index('--minutes') + 1] if '--minutes' in args else '6'
    jobs = args[args.index('-j') + 1] if '-j' in args else '4'
    only = set(args[args.index('--only') + 1].split(',')) if '--only' in args else None
    for unit, fn, rev, fix in CASES:
        if only and fn not in only:
            continue
        path = 'src/%s.c' % unit
        subprocess.run(['git', 'checkout', rev, '--', path], cwd=ROOT, check=True)
        try:
            r = subprocess.run([sys.executable, str(HERE / 'leversweep.py'), unit, fn, '--minutes', minutes,
                                '-j', jobs], cwd=ROOT, capture_output=True, text=True)
            row = (r.stdout.strip().splitlines() or ['(no output) ' + r.stderr[-200:]])[-1]
        finally:
            subprocess.run(['git', 'checkout', 'HEAD', '--', path], cwd=ROOT, check=True)
        print('%-26s EA fix: %s\n    %s' % (fn, fix, row), flush=True)


if __name__ == '__main__':
    main()
