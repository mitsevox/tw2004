"""Fetch a reference build's debug archive ON THE PC, dump its DWARF, keep only derived evidence.

    python tools/ref/fetchdwarf.py <name> --store <dir outside the repo> [--out build/refdwarf]

<name> is a key of REFS (an allowlist: the workflow never takes a free-form URL). The archive and
everything extracted from it stay in --store (game data never enters git or the results). What is
written to --out, for committing: cus.txt (every compilation unit's source path and code range),
hits.txt (the units and lines naming the KEYWORDS), and cu/<file>.txt: dtk's DWARF dump of each unit
whose path names a keyword or whose file name is one of our src/ units (function signatures, locals, types: the same kind of evidence as
docs/reference-builds/tw07-ps3/cu/). Needs py7zr (pip) for .7z archives."""
import pathlib, re, subprocess, sys, urllib.parse, urllib.request

ROOT = pathlib.Path(__file__).resolve().parents[2]
REFS = {
    # EA Tiburon, GameCube, 2004: the likeliest build with the IStudio (UIS) library and DWARF
    # (agents/findings/2026-09-26-tiburon-symbols.md).
    'nascar2005gc': 'https://debugging.games/_files/GameCube/'
                    '[GC] NASCAR 2005 - Chase for the Cup (USA) [GN4E69] (DWARF).7z',
}
KEYWORDS = ['IStudio', 'iStudio', 'UIS', 'UIStudio', 'Tiburon', 'TibExt', 'Eassdk']
# Also kept: every unit whose file name (without extension, any case) is one of our src/ units, and
# these (the font code around our LLFont.c's FO_ functions).
EXTRA_STEMS = {'font'}
OUR_STEMS = {p.stem.lower() for p in (ROOT / 'src').rglob('*.c')}


def main():
    a = sys.argv[1:]
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    name = a[0]
    if name not in REFS:
        sys.exit('unknown reference %s (known: %s)' % (name, ', '.join(REFS)))
    store = pathlib.Path(opt('--store', '')) / name
    if not str(opt('--store', '')):
        sys.exit('--store <dir outside the repo> is required')
    if ROOT in store.resolve().parents:
        sys.exit('--store must be outside the repository')
    out = pathlib.Path(opt('--out', str(ROOT / 'build/refdwarf'))) / name
    store.mkdir(parents=True, exist_ok=True)
    (out / 'cu').mkdir(parents=True, exist_ok=True)

    url = REFS[name]
    arc = store / urllib.parse.unquote(url.rsplit('/', 1)[1])
    if not arc.exists():
        print('downloading', url, flush=True)
        q = urllib.parse.quote(url, safe=':/')
        with urllib.request.urlopen(urllib.request.Request(q, headers={'User-Agent': 'Mozilla/5.0'})) as r:
            arc.write_bytes(r.read())
    print('archive %s: %d bytes' % (arc.name, arc.stat().st_size), flush=True)
    ext = store / 'extracted'
    if not ext.exists():
        import py7zr
        with py7zr.SevenZipFile(arc, 'r') as z:
            z.extractall(ext)
    files = [p for p in ext.rglob('*') if p.is_file()]
    listing = ['%10d  %s' % (p.stat().st_size, p.relative_to(ext).as_posix()) for p in files]

    dtk = ROOT / 'build/tools' / ('dtk.exe' if (ROOT / 'build/tools/dtk.exe').exists() else 'dtk')
    elfs = [p for p in files if p.read_bytes()[:4] == b'\x7fELF']
    cus, hits = [], []
    for elf in elfs:
        dump = store / (elf.name + '.dwarf.txt')
        r = subprocess.run([str(dtk), 'dwarf', 'dump', '--no-color', '-o', str(dump), str(elf)],
                           capture_output=True, text=True)
        if r.returncode or not dump.exists():
            hits.append('%s: dtk dwarf dump failed: %s' % (elf.name, (r.stdout + r.stderr)[-300:]))
            continue
        txt = dump.read_text(encoding='utf-8', errors='replace')
        # Split into compilation units at each "Compile unit:" header (with its opening comment).
        starts = [m.start() for m in re.finditer(r'^/\*\s*\n\s*Compile unit: ', txt, re.M)]
        starts.append(len(txt))
        for s, e in zip(starts, starts[1:]):
            unit = txt[s:e]
            path = re.search(r'Compile unit: (.+)', unit).group(1).strip()
            rng = re.search(r'Code range: (.+)', unit)
            cus.append('%s\t%s\t%s' % (elf.name, rng.group(1).strip() if rng else '', path))
            stem = re.split(r'[/\\]', path)[-1].rsplit('.', 1)[0].lower()
            if any(k in path for k in KEYWORDS) or stem in OUR_STEMS | EXTRA_STEMS:
                fn = re.sub(r'[^A-Za-z0-9_.-]', '_', path.replace('\\', '/').rsplit('/', 1)[-1])
                (out / 'cu' / (fn + '.txt')).write_text(unit, encoding='utf-8')
                hits.append('unit %s' % path)
        for ln in txt.splitlines():
            if any(k in ln for k in ('IStudio', 'iStudio', 'UIStudio', 'TibExt', 'Eassdk')):
                hits.append('line: ' + ln.strip()[:200])
    (out / 'files.txt').write_text('\n'.join(listing) + '\n', encoding='utf-8')
    (out / 'cus.txt').write_text('\n'.join(cus) + '\n', encoding='utf-8')
    (out / 'hits.txt').write_text('\n'.join(hits[:2000]) + '\n', encoding='utf-8')
    print('%d files, %d ELF, %d compilation units, %d keyword units' % (
        len(files), len(elfs), len(cus), sum(h.startswith('unit ') for h in hits)))


if __name__ == '__main__':
    main()
