"""Resolve configure.py merge conflicts between lanes, mid-merge, in the checkout it lives in.
    python tools/match/resolve_configure.py [--dry-run] [--root DIR]

Every conflict in configure.py's unit lists comes from two lanes adding units (mkunit.py) or
removing folded sweeps (fold.py), so a conflict block is resolved by keeping every line from both
sides, once, and dropping the Object line of a sweep that is gone after the merge (its file is
deleted, or its block is no longer in splits.txt). When both sides carry an Object line for the
same source and only one of them has args (extra_cflags=, mw_version=, ...), the one with args is
kept; when they differ in any other way (two different args, Matching against NonMatching), the
block is left for a person. The kept lines are put in address order (their
.text start in splits.txt; a line without one stays after the line it followed).

Then it checks, and prints, without failing:
  - units whose order in their list does not follow their addresses, next to a resolved block;
  - units in splits.txt with a source file but no Object line (dtk: "Missing configuration");
  - Object lines of sweeps that splits.txt no longer has, outside the conflicts.
A conflict block with anything but Object lines (or comments) in it is left for a person; the exit
code is 1 when any conflict is left. Resolve splits.txt first: the order and checks read it.
Afterwards: `git add configure.py`, then `python configure.py` and `ninja`."""
import os, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
BLOCK = re.compile(r'^<<<<<<< [^\n]*\n(.*?)^(?:\|\|\|\|\|\|\| [^\n]*\n.*?)?=======\n(.*?)'
                   r'^>>>>>>> [^\n]*\n', re.S | re.M)
OBJ = re.compile(r'^\s*Object\(\w+(?:\([^)]*\))?, "([^"]+)"')


def split_units(root):
    """unit name -> .text start, from splits.txt (None when it still has conflict markers)."""
    text = (root / 'config/GW4E69/splits.txt').read_text(encoding='utf-8')
    if '<<<<<<<' in text:
        return None
    out = {}
    for b in re.split(r'\n(?=\S[^\n]*:\n)', text):
        m = re.match(r'(\S[^\n]*):\n', b)
        t = re.search(r'\.text\s+start:0x([0-9A-Fa-f]+)', b)
        if m:
            out[m.group(1)] = int(t.group(1), 16) if t else None
    return out


def gone(unit, root, units):
    return unit.startswith('unsorted/sweep_') and (
        not (root / 'src' / unit).exists() or (units is not None and unit not in units))


def obj_args(line):
    """The arguments after the source path in an Object line ('' for none), e.g.
    'extra_cflags=["-inline auto"]'."""
    rest = line[OBJ.match(line).end():]
    return re.sub(r'\)\s*,?\s*(#.*)?$', '', rest.strip()).strip(' ,')


def one_line_per_unit(lines, why):
    """Both sides may carry an Object line for the same source (one side added args to it with
    mkunit.py, e.g. extra_cflags or mw_version). Keep one line per source: the one with args.
    -> the lines to keep, or None when a person must choose (both sides differ in some other way,
    e.g. two different args, or Matching against NonMatching); `why` gets the reason."""
    by_unit = {}
    for l in lines:
        o = OBJ.match(l)
        if o and l not in by_unit.setdefault(o.group(1), []):
            by_unit[o.group(1)].append(l)
    drop = set()
    for unit, variants in by_unit.items():
        if len(variants) < 2:
            continue
        with_args = [l for l in variants if obj_args(l)]
        if len(with_args) != 1:
            why.append('%s: %d different Object lines (%s)' % (
                unit, len(variants), ' / '.join(obj_args(l) or 'no args' for l in variants)))
            return None
        drop.update(l for l in variants if l != with_args[0])
    return [l for l in lines if l not in drop]


def resolve(text, root, units, why=None):
    """-> (text, blocks resolved, blocks left, resolved line ranges as (first, last) unit names).
    Reasons a block was left for a person are appended to `why`."""
    done, left, spans = 0, 0, []
    why = [] if why is None else why

    def one(m):
        nonlocal done, left
        theirs, ours = m.group(2).splitlines(True), m.group(1).splitlines(True)
        if not all(OBJ.match(l) or not l.strip() or l.strip().startswith('#')
                   for l in theirs + ours):
            left += 1
            why.append('a block holds something besides Object lines and comments')
            return m.group(0)
        both = one_line_per_unit(theirs + ours, why)   # main's side first
        if both is None:
            left += 1
            return m.group(0)
        keep = []
        for l in both:
            o = OBJ.match(l)
            if l in keep or (o and gone(o.group(1), root, units)):
                continue
            keep.append(l)
        if units:                                    # address order; unknown lines ride along
            key, last = [], -1
            for l in keep:
                o = OBJ.match(l)
                a = units.get(o.group(1)) if o else None
                last = a if a is not None else last
                key.append(last)
            keep = [l for _, l in sorted(zip(key, keep), key=lambda x: x[0])]
        names = [OBJ.match(l).group(1) for l in keep if OBJ.match(l)]
        if names:
            spans.append(names)
        done += 1
        return ''.join(keep)

    return BLOCK.sub(one, text), done, left, spans


def lists(text):
    """[[unit names in one "objects" list, in order]]"""
    return [re.findall(r'Object\(\w+(?:\([^)]*\))?, "([^"]+)"', m.group(1))
            for m in re.finditer(r'"objects": \[(.*?)\n\s*\]', text, re.S)]


def check(text, root, units, spans):
    notes = []
    if units is None:
        return ['splits.txt still has conflict markers: resolve it, then run this again to check']
    touched = {n for s in spans for n in s}
    for lst in lists(text):
        for i in range(1, len(lst)):
            a, b = units.get(lst[i - 1]), units.get(lst[i])
            if a is not None and b is not None and b < a and (lst[i] in touched
                                                               or lst[i - 1] in touched):
                notes.append('out of address order: %s (0x%08X) after %s (0x%08X)'
                             % (lst[i], b, lst[i - 1], a))
    listed = {n for lst in lists(text) for n in lst}
    for u in sorted(units):
        if u not in listed and (root / 'src' / u).exists():
            notes.append('%s is in splits.txt and has a source, but no Object line' % u)
    for u in sorted(listed):
        if u.startswith('unsorted/sweep_') and u not in units:
            notes.append('%s has an Object line but no block in splits.txt (folded?)' % u)
    return notes


def main():
    argv = sys.argv[1:]
    root = ROOT
    if '--root' in argv:
        k = argv.index('--root'); root = pathlib.Path(argv[k + 1]).resolve(); del argv[k:k + 2]
    path = root / 'configure.py'
    text = path.read_text(encoding='utf-8')
    units = split_units(root)
    why = []
    out, done, left, spans = resolve(text, root, units, why)
    print('configure.py: %d conflict blocks resolved, %d left for a person' % (done, left))
    for w in why:
        print('  left:', w)
    for note in check(out, root, units, spans):
        print('  check:', note)
    if '--dry-run' not in argv and out != text:
        path.write_text(out, encoding='utf-8', newline='\n')
    return 1 if left else 0


if __name__ == '__main__':
    sys.exit(main())
