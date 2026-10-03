"""Give a unit its data ranges, switch it to Matching and rebuild; put both files back if the DOL fails.
    python tools/matching/graduate.py <Unit>.c ".sdata2 0x80284700 0x80284708" [".data 0x.. 0x.."]
Ranges go under the unit's code lines (.init and/or .text) in the order given.
Exits 0 only when the DOL is OK; any failure exits 1 with both files unchanged."""
import pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2].as_posix()   # the checkout this script lives in
SP, CF = ROOT + '/config/GW4E69/splits.txt', ROOT + '/configure.py'
if len(sys.argv) < 2: sys.exit(__doc__)
unit, ranges = sys.argv[1], sys.argv[2:]
sp0 = open(SP, encoding='utf-8', newline='').read(); cf0 = open(CF, encoding='utf-8', newline='').read()
m = re.search(r'\n' + re.escape(unit) + r':\n((?:\t\.(?:init|text)[^\n]*\n)+)', sp0)
if not m: sys.exit('%s: no .init/.text line for this unit in splits.txt' % unit)
try:
    add = ''.join('\t%-11s start:%s end:%s\n' % tuple(r.split()) for r in ranges)
except TypeError:
    sys.exit('each range must be "<section> <start> <end>"')
sp = sp0[:m.end()] + add + sp0[m.end():]
a = 'Object(NonMatching, "%s"' % unit      # may go on with extra arguments (extra_cflags=...)
if cf0.count(a) != 1: sys.exit('%s: expected exactly one %s...) in configure.py' % (unit, a))
open(SP, 'w', encoding='utf-8', newline='').write(sp)
open(CF, 'w', encoding='utf-8', newline='').write(cf0.replace(a, 'Object(Matching, "%s"' % unit))
configured = subprocess.run([sys.executable, 'configure.py'], cwd=ROOT, capture_output=True, text=True)
r = (subprocess.run(['ninja'], cwd=ROOT, capture_output=True, text=True)
     if configured.returncode == 0 else configured)
ok = r.returncode == 0 and 'main.dol: OK' in r.stdout
if r.returncode == 0 and not ok:
    # The hash target can be up to date; verify it without deleting its build marker.
    dtk = ROOT + '/build/tools/dtk'
    if not pathlib.Path(dtk).exists():
        dtk += '.exe'
    check = subprocess.run([dtk, 'shasum', '-c',
                            'config/GW4E69/build.sha1'], cwd=ROOT, capture_output=True, text=True)
    ok = check.returncode == 0 and 'main.dol: OK' in check.stdout
print(unit, 'OK' if ok else 'FAILED')
if not ok:
    if configured.returncode != 0:
        print(configured.stderr[-3000:])
    print('\n'.join(l for l in r.stdout.splitlines()
                    if 'WARN' not in l and ('rror' in l or 'FAIL' in l or l.startswith('#   ')))[:3000])
    open(SP, 'w', encoding='utf-8', newline='').write(sp0)
    open(CF, 'w', encoding='utf-8', newline='').write(cf0)
    subprocess.run([sys.executable, 'configure.py'], cwd=ROOT, capture_output=True)
    sys.exit(1)
