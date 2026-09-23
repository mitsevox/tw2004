"""Cross-file type consistency audit for src/*.c.
    python tools/match/typeaudit.py            list every problem
    python tools/match/typeaudit.py --count    one line: "<conflicts> <duplicates>" (used by merges)
Reports two kinds of disparity that per-file matching can introduce:
  - a global declared with different types in different files (e.g. `u8*` in one, a struct in
    another): one object must have one declaration, in a header;
  - a struct/union/enum type defined locally in more than one file: two private views of the same
    data drift apart; the type belongs in a header, defined once.
Sweep files (src/unsorted) are skipped. The goal is zero of both; merges may not add any."""
import collections, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in


def audit():
    ext = collections.defaultdict(dict)
    tdefs = collections.defaultdict(list)
    for f in sorted((ROOT / 'src').glob('*.c')):
        s = f.read_text(encoding='utf-8', errors='replace')
        for m in re.finditer(r'^extern\s+([\w \*]+?)\s*\b(\w+)\s*(\[[^\]]*\])?\s*;', s, re.M):
            t = ' '.join((m.group(1) + ('[]' if m.group(3) else '')).replace('*', ' * ').split())
            ext[m.group(2)][f.name] = t
        for m in re.finditer(r'typedef\s+(?:struct|union|enum)\s*\w*\s*\{.*?\}\s*(\w+)\s*;', s, re.S):
            tdefs[m.group(1)].append(f.name)
    conflicts = {n: d for n, d in ext.items() if len(set(d.values())) > 1}
    dups = {n: fs for n, fs in tdefs.items() if len(fs) > 1}
    return conflicts, dups


conflicts, dups = audit()
if '--count' in sys.argv:
    print(len(conflicts), len(dups))
    sys.exit(0)
print('globals declared with different types:', len(conflicts))
for n, d in sorted(conflicts.items()):
    print('  %s: %s' % (n, '; '.join('%s %s' % (k, v) for k, v in sorted(d.items()))))
print('types defined in more than one file:', len(dups))
for n, fs in sorted(dups.items()):
    print('  %s: %s' % (n, ', '.join(fs)))
sys.exit(1 if conflicts or dups else 0)
