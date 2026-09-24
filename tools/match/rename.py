"""Rename functions or globals everywhere at once: symbols.txt, src/ and include/.
    python tools/match/rename.py <names.tsv> [--dry-run]
    python tools/match/rename.py <names.tsv> --refs-only   fix leftover references only (after a merge)
names.tsv: one rename per line, tab-separated: address, current name, new name[, anything else].
Lines starting with # are skipped.

Nothing is written unless every line passes:
  - the current name is in symbols.txt exactly once, at that address;
  - the new name is a C identifier, not a keyword, and not in use: not a symbol in symbols.txt and
    not a word anywhere in src/ or include/ (a type, macro, local or field of the same name would
    capture or shadow it);
  - no two lines share a current name or a new name.
Then the name is replaced on its symbols.txt line and as a whole word in every .c/.h/.inc file under
src/ and include/ (comments too). Line endings and encoding are kept. The build proves the rest:
run configure + ninja (main.dol: OK) and compare the exact functions by address.

--refs-only is for a branch that was started before a rename: symbols.txt already has the new name,
so only leftover whole-word uses of the old name in src/ and include/ are replaced."""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
SYMS = ROOT / 'config/GW4E69/symbols.txt'
IDENT = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')
KEYWORDS = set('''auto break case char const continue default do double else enum extern float for goto
if inline int long register restrict return short signed sizeof static struct switch typedef union
unsigned void volatile while asm'''.split())


def source_files():
    for top in ('src', 'include'):
        for p in sorted((ROOT / top).rglob('*')):
            if p.suffix in ('.c', '.h', '.inc') and p.is_file():
                yield p


def read(p):
    return p.read_bytes().decode('utf-8', errors='surrogateescape')


def write(p, text):
    p.write_bytes(text.encode('utf-8', errors='surrogateescape'))


def load(tsv):
    rows = []
    for n, l in enumerate(pathlib.Path(tsv).read_text(encoding='utf-8-sig').splitlines(), 1):
        if not l.strip() or l.lstrip().startswith('#') or l.split('\t')[0].strip().lower() == 'address':
            continue
        f = l.split('\t')
        if len(f) < 3:
            sys.exit('%s:%d: need address<TAB>current<TAB>new' % (tsv, n))
        rows.append((n, int(f[0].strip().removeprefix('0x'), 16), f[1].strip(), f[2].strip()))
    return rows


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    rows = load(sys.argv[1])
    dry, refs_only = '--dry-run' in sys.argv, '--refs-only' in sys.argv
    sym_lines = read(SYMS).split('\n')
    sym_names = {}
    for i, l in enumerate(sym_lines):
        m = re.match(r'^(\S+) = \.\w+:0x([0-9A-Fa-f]+);', l)
        if m:
            sym_names.setdefault(m.group(1), []).append((i, int(m.group(2), 16)))
    files = {p: read(p) for p in source_files()}
    words = set()
    for t in files.values():
        words.update(re.findall(r'\b[A-Za-z_][A-Za-z0-9_]*\b', t))

    errors, olds, news = [], {}, {}
    for n, addr, old, new in rows:
        where = 'line %d (%s -> %s)' % (n, old, new)
        if old in olds or new in news:
            errors.append('%s: %s or %s appears on two lines' % (where, old, new))
        olds[old], news[new] = n, n
        if refs_only:
            if [a for _, a in sym_names.get(new, [])] != [addr]:
                errors.append('%s: --refs-only needs %s at 0x%08X in symbols.txt' % (where, new, addr))
            continue
        hits = sym_names.get(old, [])
        if len(hits) != 1 or hits[0][1] != addr:
            errors.append('%s: %s is not in symbols.txt exactly once at 0x%08X (%s)' % (
                where, old, addr, ', '.join('0x%08X' % a for _, a in hits) or 'absent'))
        if not IDENT.match(new) or new in KEYWORDS:
            errors.append('%s: %s is not a usable C name' % (where, new))
        elif new in sym_names:
            errors.append('%s: %s is already a symbol (0x%08X)' % (where, new, sym_names[new][0][1]))
        elif new in words:
            errors.append('%s: %s is already a word in src/ or include/' % (where, new))
    if errors:
        sys.exit('nothing renamed:\n  ' + '\n  '.join(errors))

    pat = {old: re.compile(r'\b%s\b' % re.escape(old)) for _, _, old, _ in rows}
    changed = 0
    if not refs_only:
        for _, _, old, new in rows:
            i = sym_names[old][0][0]
            sym_lines[i] = new + sym_lines[i][len(old):]
        if not dry:
            write(SYMS, '\n'.join(sym_lines))
    for p, t in files.items():
        u = t
        for _, _, old, new in rows:
            u = pat[old].sub(new, u)
        if u != t:
            changed += 1
            if not dry:
                write(p, u)
            elif '-v' in sys.argv:
                print('would change', p.relative_to(ROOT).as_posix())
    print('%s %d name(s); %d source file(s) %s' % (
        'would rename' if dry else 'renamed', len(rows), changed, 'to change' if dry else 'changed'))


if __name__ == '__main__':
    main()
