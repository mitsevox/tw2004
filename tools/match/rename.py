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
src/ and include/ (comments too), except right after "TW06:", "TW06's" or "TW06 " (optionally with
a return type): that records TW06's own name and stays. Line endings and encoding are kept. The build proves the rest:
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
    """(line, address, old, new). A file with a header row naming `previous_name` and `name` columns
    (config/GW4E69/name_sources.tsv, the repo's rename log) is read by those columns."""
    rows, cols = [], None
    for n, l in enumerate(pathlib.Path(tsv).read_text(encoding='utf-8-sig').splitlines(), 1):
        f = [x.strip() for x in l.split('\t')]
        if not l.strip() or l.lstrip().startswith('#'):
            continue
        if f[0].lower() == 'address':
            if 'previous_name' in f and 'name' in f:
                cols = (f.index('previous_name'), f.index('name'))
            continue
        old, new = (f[cols[0]], f[cols[1]]) if cols else (f[1] if len(f) > 1 else '', f[2] if len(f) > 2 else '')
        if not old or not new:
            sys.exit('%s:%d: need address<TAB>current<TAB>new' % (tsv, n))
        rows.append((n, int(f[0].removeprefix('0x'), 16), old, new))
    return rows


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    rows = load(sys.argv[1])
    dry, refs_only = '--dry-run' in sys.argv, '--refs-only' in sys.argv
    sym_lines = read(SYMS).split('\n')
    sym_names, fn_at = {}, {}
    for i, l in enumerate(sym_lines):
        m = re.match(r'^(\S+) = \.(\w+):0x([0-9A-Fa-f]+);', l)
        if m:
            sym_names.setdefault(m.group(1), []).append((i, int(m.group(3), 16)))
            if m.group(2) == 'text':
                fn_at[int(m.group(3), 16)] = m.group(1)
    if refs_only:
        # every old name goes straight to the name the address has NOW (a name renamed twice
        # leaves two log rows: A -> B, B -> C; both A and B become C)
        rows = [(n, a, old, fn_at.get(a, new)) for n, a, old, new in rows if old != fn_at.get(a, new)]
        # an old name that is now ANOTHER function's symbol (two names swapped) is a live reference
        # to that function: rewriting it would send both to one name
        rows = [r for r in rows if r[2] not in sym_names]
        rows = list({old: (n, a, old, new) for n, a, old, new in rows}.values())
    files = {p: read(p) for p in source_files()}
    words = set()
    for t in files.values():
        # code only: a comment or string that already calls the function by the new name is no
        # clash (EA's own messages name their functions: "GbaWriteOnline: ...")
        code = re.sub(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|//[^\n]*|/\*.*?\*/', ' ', t, flags=re.S)
        words.update(re.findall(r'\b[A-Za-z_][A-Za-z0-9_]*\b', code))

    errors, olds, news = [], {}, {}
    for n, addr, old, new in rows:
        where = 'line %d (%s -> %s)' % (n, old, new)
        if old in olds or (new in news and not refs_only):
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
    tw06_ref = re.compile(r"TW06(?:'s|:)?\s+(?:[A-Za-z_]\w*\s*\**\s*)?$")

    def sub_keeping_tw06(regex, new, text):
        # "TW06: GM_Foo" records TW06's own name for the function: history, not a reference to
        # our symbol, so it stays when our name changes
        return regex.sub(lambda m: m.group(0) if tw06_ref.search(text[max(0, m.start() - 40):m.start()])
                         else new, text)
    for p, t in files.items():
        u = t
        for _, _, old, new in rows:
            u = sub_keeping_tw06(pat[old], new, u)
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
