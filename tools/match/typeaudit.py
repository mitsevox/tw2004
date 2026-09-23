"""Cross-file type consistency audit for src/*.c.
    python tools/match/typeaudit.py            list every problem
    python tools/match/typeaudit.py --count    one line: "<conflicts> <duplicates> <externs> <typedefs>"
                                               (used by merges: none of the four may rise)
Reports two kinds of disparity that per-file matching can introduce:
  - a global declared with different types in different files (e.g. `u8*` in one, a struct in
    another): one object must have one declaration, in a header;
  - a struct/union/enum type defined locally in more than one file: two private views of the same
    data drift apart; the type belongs in a header, defined once.
It also counts `extern` declarations and `typedef`s written in .c files at all: both belong in a
header (docs/style.md), so those two counts only go down.
Sweep files (src/unsorted) are skipped. The goal is zero of all four; merges may not add any."""
import collections, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402


def audit():
    ext = collections.defaultdict(dict)
    tdefs = collections.defaultdict(list)
    raw = collections.Counter()      # 'extern' / 'typedef' lines in .c files
    for f in sorted((ROOT / 'src').glob('*.c')):
        # a marked block of uncleaned sweep code is counted by lint.py, not here (sweepblock.py)
        s = sweepblock.strip_blocks(f.read_text(encoding='utf-8', errors='replace'))
        for m in re.finditer(r'^extern\s+([\w \*]+?)\s*\b(\w+)\s*(\[[^\]]*\])?\s*;', s, re.M):
            t = ' '.join((m.group(1) + ('[]' if m.group(3) else '')).replace('*', ' * ').split())
            ext[m.group(2)][f.name] = t
        for m in re.finditer(r'typedef\s+(?:struct|union|enum)\s*\w*\s*\{.*?\}\s*(\w+)\s*;', s, re.S):
            tdefs[m.group(1)].append(f.name)
        raw['extern'] += len(re.findall(r'^\s*extern\b', s, re.M))
        raw['typedef'] += len(re.findall(r'^\s*typedef\b', s, re.M))
    conflicts = {n: d for n, d in ext.items() if len(set(d.values())) > 1}
    dups = {n: fs for n, fs in tdefs.items() if len(fs) > 1}
    return conflicts, dups, raw


conflicts, dups, raw = audit()
if '--count' in sys.argv:
    print(len(conflicts), len(dups), raw['extern'], raw['typedef'])
    sys.exit(0)
print('globals declared with different types:', len(conflicts))
for n, d in sorted(conflicts.items()):
    print('  %s: %s' % (n, '; '.join('%s %s' % (k, v) for k, v in sorted(d.items()))))
print('types defined in more than one file:', len(dups))
for n, fs in sorted(dups.items()):
    print('  %s: %s' % (n, ', '.join(fs)))
print('extern declarations in .c files: %d, typedefs in .c files: %d (both belong in headers)'
      % (raw['extern'], raw['typedef']))
sys.exit(1 if conflicts or dups else 0)
