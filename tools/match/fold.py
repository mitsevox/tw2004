"""After mkunit.py: fold the sweep files it listed into the unit, and commit.
    python tools/match/fold.py <Name> <sweep1.c> ... [--push]
Sweep names as mkunit printed them (e.g. unsorted/sweep_X.c). Runs configure, merges and dedupes,
builds, and checks that every folded function is exact. Only then does it git rm the sweeps and
commit. Nothing is deleted unless the DOL is OK and every folded function matches. Pushes only
with --push (agents working in a worktree leave pushing to the orchestrator).

For a unit that already has a source (widened with `mkunit.py --extend`), each sweep function is
put in a sweep block where its address falls, before, between or after the unit's functions, and
the rest of the file is kept (merge_sweeps.py); if the build or the check fails, the source is put
back as it was."""
import difflib, json, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from merge_sweeps import defined                     # noqa: E402
args = [a for a in sys.argv[1:] if a != '--push']
push = '--push' in sys.argv
name, sweeps = args[0], args[1:]
path = ROOT / 'src' / f'{name}.c'
before = path.read_text(encoding='utf-8') if path.exists() else None
extending = bool(before and before.strip())


def run(cmd):
    return subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)


MINE = [f'{name}.c'] + sweeps                        # the only units mkunit.py touched


def undo_mkunit():
    """Put configure.py and splits.txt back to HEAD, when every change in them is mkunit.py's for
    this unit and these sweeps; otherwise say what else changed and leave them."""
    files = ['configure.py', 'config/GW4E69/splits.txt']
    head = {}
    for f in files:
        g = subprocess.run(['git', 'show', f'HEAD:{f}'], cwd=ROOT, capture_output=True)
        if g.returncode:
            return print(f'cannot read {f} at HEAD: roll back configure.py and splits.txt by hand')
        head[f] = g.stdout
    other = []
    for ln in difflib.ndiff(head['configure.py'].decode('utf-8').splitlines(),
                            (ROOT / 'configure.py').read_text(encoding='utf-8').splitlines()):
        if ln[:2] in ('+ ', '- ') and ln[2:].strip() \
                and not any(f'"{u}"' in ln for u in MINE):
            other.append('configure.py: ' + ln)

    def blocks(text):
        return {b.split(':\n')[0]: b.strip() for b in re.split(r'\n(?=\S[^\n]*:\n)', text)}
    old = blocks(head['config/GW4E69/splits.txt'].decode('utf-8'))
    new = blocks((ROOT / 'config/GW4E69/splits.txt').read_text(encoding='utf-8'))
    for k in sorted(set(old) | set(new)):
        if old.get(k) != new.get(k) and k not in MINE:
            other.append('splits.txt: block ' + k)
    if other:
        print('NOT rolled back: configure.py / splits.txt have changes besides this unit\'s:')
        print('\n'.join('    ' + o for o in other[:10]))
        return
    for f in files:
        (ROOT / f).write_bytes(head[f])
    run('python configure.py')
    print(f'configure.py and splits.txt put back as at HEAD ({name} is not a unit any more); '
          'run ninja to rebuild')


def fail(msg):
    print(msg)
    if extending:
        path.write_text(before, encoding='utf-8', newline='\n')
        print(f'src/{name}.c put back as it was; its widened range (mkunit.py --extend) is kept, '
              'so fold.py can be run again once the problem is fixed')
    else:
        # a new unit: take the whole half-made unit away (its source, mkunit.py's edits)
        if before is None:
            path.unlink(missing_ok=True)
            print(f'src/{name}.c removed')
        else:
            path.write_text(before, encoding='utf-8', newline='\n')
        undo_mkunit()
    sys.exit(1)


# The sweeps are deleted with `git rm` at the end, which refuses a file with uncommitted edits:
# check that up front, before anything is merged or built.
dirty = []
for s in sweeps:
    f = f'src/{s}'
    if not (ROOT / f).exists():
        dirty.append(f'{f}: no such file')
    elif subprocess.run(['git', 'ls-files', '--error-unmatch', f], cwd=ROOT,
                        capture_output=True).returncode:
        dirty.append(f'{f}: not committed (untracked)')
    elif subprocess.run(['git', 'diff', '--quiet', 'HEAD', '--', f], cwd=ROOT).returncode:
        dirty.append(f'{f}: has uncommitted changes')
if dirty:
    print('\n'.join(dirty))
    sys.exit('Commit the sweep files first (or undo their edits), then run fold.py again. '
             'Nothing was changed.')


# mkunit.py takes every sweep in the unit's range out of splits.txt and configure.py. When only
# some of them are folded here, the others would be left behind as orphans: files that nothing
# builds, whose functions the unit covers without their code. So each sweep file left out is put
# back as its own unit (its splits.txt block and configure.py line as at HEAD), and the unit's
# .text range is cut back to end before it; that works for sweeps at either end of the range
# only. A left-out sweep between folded code, or one with no block at HEAD, stops the fold
# before anything is changed.
TEXT = re.compile(r'\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)')
SPLITS = ROOT / 'config/GW4E69/splits.txt'


def split_blocks(text):
    return {m.group(1): b for b in re.split(r'\n(?=\S[^\n]*:\n)', text)
            for m in [re.match(r'(\S[^\n]*):\n', b)] if m}


def text_range(block):
    t = TEXT.search(block or '')
    return (int(t.group(1), 16), int(t.group(2), 16)) if t else None


def restore_left_out():
    head_splits = run('git show HEAD:config/GW4E69/splits.txt').stdout
    head_cfg = run('git show HEAD:configure.py').stdout
    now = split_blocks(SPLITS.read_text(encoding='utf-8'))
    old = split_blocks(head_splits)
    unit = f'{name}.c'
    rng = text_range(now.get(unit))
    if not rng:
        return
    lo, hi = rng
    cfg = (ROOT / 'configure.py').read_text(encoding='utf-8')
    passed = set(sweeps)
    left, stuck = [], []
    for f in sorted((ROOT / 'src/unsorted').glob('sweep_*.c')):
        s = f'unsorted/{f.name}'
        if s in passed or s in now or f'"{s}"' in cfg:
            continue                                 # folded now, or still its own unit
        r = text_range(old.get(s))
        if r is None:
            a = int(f.stem.split('_')[1], 16)
            if lo <= a < hi:
                stuck.append(f'{s}: in {name}.c\'s range, no block in splits.txt at HEAD either '
                             '(an orphan already: fold it too)')
            continue
        if r[0] < hi and r[1] > lo:
            left.append((s, r))
    # what the unit keeps: the folded sweeps, and its own range at HEAD when it is being widened
    keep = [text_range(old.get(s)) for s in sweeps] + [text_range(old.get(unit))]
    keep = [r for r in keep if r]
    k_lo = min((r[0] for r in keep), default=hi)
    k_hi = max((r[1] for r in keep), default=lo)
    below = [(s, r) for s, r in left if r[1] <= k_lo]
    above = [(s, r) for s, r in left if r[0] >= k_hi]
    for s, r in left:
        if (s, r) not in below and (s, r) not in above:
            stuck.append(f'{s} (0x{r[0]:08X}-0x{r[1]:08X}): left out, but it lies between '
                         'code that is folded; fold it too, or narrow the range with mkunit.py')
        elif not re.search(r'\n[ \t]*Object\(\w+, "%s"\),[^\n]*' % re.escape(s), head_cfg):
            stuck.append(f'{s}: left out, but configure.py has no line for it at HEAD')
    if stuck:
        print('\n'.join(stuck))
        sys.exit('Nothing was changed.')
    if not left:
        return
    new_lo = max([lo] + [r[1] for _, r in below])
    new_hi = min([hi] + [r[0] for _, r in above])
    text = SPLITS.read_text(encoding='utf-8')
    parts = re.split(r'\n(?=\S[^\n]*:\n)', text)
    out = []
    for b in parts:
        if b.startswith(unit + ':\n'):
            out += [old[s].rstrip('\n') + '\n' for s, _ in sorted(below, key=lambda x: x[1])]
            out.append(TEXT.sub('.text       start:0x%08X end:0x%08X' % (new_lo, new_hi), b,
                                count=1))
            out += [old[s].rstrip('\n') + '\n' for s, _ in sorted(above, key=lambda x: x[1])]
        else:
            out.append(b)
    for s, r in below + above[::-1]:
        # where it was at HEAD: after the line that came before it, or before the line that came
        # after it; failing both, just before / after the unit's own line
        h = head_cfg.split('\n')
        i = next(j for j, ln in enumerate(h) if re.match(r'[ \t]*Object\(\w+, "%s"\),' % re.escape(s), ln))
        now_lines = cfg.split('\n')
        obj = re.compile(r'[ \t]*Object\(')             # only a unit's line is unique
        if obj.match(h[i - 1]) and h[i - 1] in now_lines:
            now_lines.insert(now_lines.index(h[i - 1]) + 1, h[i])
        elif i + 1 < len(h) and obj.match(h[i + 1]) and h[i + 1] in now_lines:
            now_lines.insert(now_lines.index(h[i + 1]), h[i])
        else:
            j = next((j for j, ln in enumerate(now_lines) if re.match(
                r'[ \t]*Object\(\w+, "%s"' % re.escape(unit), ln)), None)
            if j is None:
                sys.exit(f'{unit} has no line in configure.py: nothing was changed')
            now_lines.insert(j if (s, r) in below else j + 1, h[i])
        cfg = '\n'.join(now_lines)
    SPLITS.write_text('\n'.join(out), encoding='utf-8', newline='\n')
    (ROOT / 'configure.py').write_text(cfg, encoding='utf-8', newline='\n')
    print(f'left out, put back as their own units: {", ".join(s for s, _ in below + above)}; '
          f'{unit} .text cut to 0x{new_lo:08X}-0x{new_hi:08X} (was 0x{lo:08X}-0x{hi:08X})')


restore_left_out()

folded = [n for s in sweeps for _, n in defined((ROOT / 'src' / s).read_text(encoding='utf-8'))]
run('python configure.py')
# Sweeps that declare one name with two types (with each other, or with the unit and its headers)
# cannot be merged: find them before anything is written, and let the lane fix them first.
d = run(f'python "{HERE / "declcheck.py"}" {name} ' + ' '.join(sweeps))
print(d.stdout.strip() or d.stderr.strip()[-1000:])
if d.returncode:
    fail('declarations conflict: nothing merged')
m = run(f'python "{HERE / "merge_sweeps.py"}" {name}.c ' + ' '.join(sweeps))
print(m.stdout.strip())
if m.returncode:
    fail('merge failed: ' + (m.stderr.strip() or m.stdout.strip())[-500:])
dd = run(f'python "{HERE / "dedupe_decls.py"}" src/{name}.c')
print(dd.stdout.strip())
if dd.returncode:
    fail('conflicting prototypes after the merge (see CONFLICT above)')


def compile_errors(out):
    """The compiler's messages in ninja's output (from each FAILED: line to the next [n/m] line)."""
    keep, on = [], False
    for ln in out.splitlines():
        if ln.startswith('FAILED:'):
            on = True
        elif re.match(r'^\[\d+/\d+\]', ln) or ln.startswith('ninja:'):
            on = False
        if on and not ln.startswith('FAILED:') and len(ln) < 400:
            keep.append(ln)
    return '\n'.join(keep).strip()


# A NonMatching unit is not linked, so main.dol can be OK while the unit does not even compile:
# build its object first, on its own, and stop on a compile error.
obj = ROOT / 'build/GW4E69/src' / f'{name}.o'
c = run(f'ninja build/GW4E69/src/{name}.o')
if c.returncode or not obj.exists() or obj.stat().st_mtime < path.stat().st_mtime:
    print(compile_errors(c.stdout) or c.stdout[-2000:] or c.stderr[-2000:])
    fail(f'src/{name}.c DOES NOT COMPILE (ninja exit {c.returncode})')
(ROOT / 'build/GW4E69/ok').unlink(missing_ok=True)
b = run('ninja')
if b.returncode or 'main.dol: OK' not in b.stdout:
    print(compile_errors(b.stdout) or b.stdout[-2000:])
    fail('BUILD NOT OK')
# Judge only on a report made now: ninja does not rebuild report.json when a target object (from
# the split) changes, and leaves the old one in place when any source fails to build.
report = ROOT / 'build/GW4E69/report.json'
report.unlink(missing_ok=True)
rp = run('ninja build/GW4E69/report.json')
if rp.returncode or not report.exists():
    print(compile_errors(rp.stdout) or rp.stdout[-2000:] or rp.stderr[-2000:])
    fail('REPORT NOT GENERATED (ninja exit %d)' % rp.returncode)
r = json.load(open(report))
us = [u for u in r['units'] if u['name'] == 'main/' + name]
if not us:
    fail(f'main/{name} is not in the new report.json')
u = us[0]
have = {f['name'] for f in u.get('functions', [])}
ex = {f['name'] for f in u.get('functions', []) if f.get('fuzzy_match_percent') == 100}
absent = [d for d in folded if d not in have]
miss = [d for d in folded if d in have and d not in ex]
print(len(folded), 'folded, not exact:', miss, 'total fns', len(have))
if absent:
    print('not in the unit at all (outside its range, or renamed?):', absent)
if miss or absent or not folded:
    fail('NOT deleting sweeps')
rm = run('git rm -q ' + ' '.join('src/' + s for s in sweeps))
if rm.returncode != 0:
    # e.g. a sweep file has local changes: someone edited it, so it is not deleted unchecked
    print('git rm failed, nothing committed:', rm.stderr.strip()); sys.exit(1)
print(run(f'git add src/{name}.c config/GW4E69/splits.txt configure.py').stderr.strip())
lo = re.search(r'start:(0x\w+) end:(0x\w+)',
               (ROOT / 'config/GW4E69/splits.txt').read_text().split(name + '.c:')[1])
what = 'widened to' if extending else 'new unit'
msg = f'{name}.c: {what} ({lo.group(1)}-{lo.group(2)}), {len(sweeps)} sweep files folded in'
print(run(f'git commit -q -m "{msg}"').stderr.strip())
if push:
    print(run('git push -q').stderr.strip()[-200:])
print(run('git log --oneline -1').stdout.strip())
