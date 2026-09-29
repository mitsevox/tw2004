"""Remove build outputs that no build rule makes any more (a renamed, split or folded unit's old object).
    python tools/build/prune_stale.py [--dry-run]        (after configure.py, before ninja)
Why: wibo runs the Windows compiler, whose file lookup ignores case. After uiobject.c became uiObject.c
(2026-09-29) it kept writing into the old build/GW4E69/src/uiobject.o and uiObject.o never appeared, so
objdiff's report failed; CI restores the previous run's build folder from its cache, so every CI build
failed from then on. The check: `ninja -t targets all` lists every output the build makes; an object
(.o, and its .d / .i / .s) under build/*/src that is not one of them is stale and is removed. Nothing
outside build/*/src is touched."""
import pathlib, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
dry = '--dry-run' in sys.argv[1:]
out = subprocess.run(['ninja', '-t', 'targets', 'all'], cwd=ROOT, capture_output=True, text=True, check=True).stdout
targets = {line.rsplit(':', 1)[0].strip() for line in out.splitlines()}
made = {t[:-2] for t in targets if t.endswith('.o')}          # objects the build makes, without .o
stale = []
for d in sorted(ROOT.glob('build/*/src')):
    for p in sorted(d.rglob('*')):
        rel = p.relative_to(ROOT).as_posix()
        if p.is_file() and p.suffix in ('.o', '.d', '.i', '.s') and rel not in targets \
                and rel[:-len(p.suffix)] not in made:
            stale.append(p)
for p in stale:
    print('%s %s' % ('would remove' if dry else 'removed', p.relative_to(ROOT).as_posix()))
    if not dry:
        p.unlink()
print('%d stale build output(s)' % len(stale))
