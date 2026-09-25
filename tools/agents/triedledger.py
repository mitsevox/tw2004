"""Build agents/tried/<fn>.md, one file per non-exact function: everything the notes, findings and
docs say was tried on it, with the source of each line. Run once to start the ledger; after that the
files are kept by hand (lanes add their attempts, the orchestrator marks SOLVED). An existing file
is never overwritten; new non-exact functions get a new file.

    python tools/agents/triedledger.py            (needs build/GW4E69/report.json)"""
import json, pathlib, re

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / 'agents' / 'tried'
SOURCES = sorted((ROOT / 'agents' / 'notes').glob('*')) + [
    ROOT / 'agents/findings/cleanup-audit-2026-09-23.md', ROOT / 'agents/findings/fill-cp1.md',
    ROOT / 'docs/decomp-notes.md', ROOT / 'docs/journal.md', ROOT / 'agents/state.md']
SWEEP = ROOT / 'agents/findings/leversweep-2026-09-24'


def addresses():
    out = {}
    for l in open(ROOT / 'config/GW4E69/symbols.txt', encoding='utf-8'):
        m = re.match(r'(\S+) = \.text:0x([0-9A-F]{8});', l)
        if m:
            out[m.group(1)] = m.group(2)
    return out


def excerpts(path, pats):
    """The lines that mention the function, each with the more-indented lines under it."""
    try:
        lines = path.read_text(encoding='utf-8', errors='replace').split('\n')
    except (IsADirectoryError, FileNotFoundError):
        return []
    out, i = [], 0
    while i < len(lines):
        if any(p.search(lines[i]) for p in pats):
            ind = len(lines[i]) - len(lines[i].lstrip())
            block = [lines[i]]
            j = i + 1
            while j < len(lines) and lines[j].strip() and \
                    len(lines[j]) - len(lines[j].lstrip()) > ind and len(block) < 30:
                block.append(lines[j])
                j += 1
            out.append('\n'.join(block))
            i = j
        else:
            i += 1
    return out


def main():
    rep = json.load(open(ROOT / 'build/GW4E69/report.json'))
    addr = addresses()
    OUT.mkdir(parents=True, exist_ok=True)
    index, made = [], 0
    for u in rep['units']:
        unit = u['name'].split('/', 1)[1]
        for f in u.get('functions', []):
            pct = f.get('fuzzy_match_percent') or 0
            if pct == 100:
                continue
            fn = f['name']
            a = addr.get(fn)
            names = {fn} | ({'fn_' + a, a, a.lower(), '0x' + a, '0x' + a.lower()} if a else set())
            pats = [re.compile(r'(?<![0-9A-Za-z_])%s(?![0-9A-Za-z_])' % re.escape(n)) for n in names]
            path = OUT / ('%s.md' % fn)
            hits = [(s, excerpts(s, pats)) for s in SOURCES]
            hits = [(s, e) for s, e in hits if e]
            sweep = SWEEP / ('%s.txt' % fn)
            index.append((unit, fn, pct, len(hits) + sweep.exists()))
            if path.exists():
                continue
            doc = ['# %s (%s.c%s)' % (fn, unit, ', 0x' + a if a else ''),
                   '',
                   'Status: OPEN, %.2f%% on 2026-09-25.' % pct,
                   '',
                   'Read all of this before working on the function. Do not repeat an attempt listed here',
                   'unless you combine it with something new. Before you stop, add every attempt under',
                   '"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.',
                   '',
                   '## Attempts',
                   '',
                   '(add yours here: date, lane, what, score)',
                   '']
            if sweep.exists():
                doc += ['## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)', '', '```']
                doc += sweep.read_text(encoding='utf-8', errors='replace').split('\n')[:14]
                doc += ['```', '']
            doc += ['## Collected from the notes and docs (2026-09-25)', '']
            if not hits:
                doc.append('Nothing recorded.')
            for s, ex in hits:
                doc += ['### %s' % s.relative_to(ROOT).as_posix(), '', '```']
                doc += ex
                doc += ['```', '']
            path.write_text('\n'.join(doc).rstrip() + '\n', encoding='utf-8', newline='\n')
            made += 1
    rows = ['# Tried-ledger: one file per function that is not exact yet', '',
            'Read agents/tried/<fn>.md before working on a function; add your attempts to it before you',
            'stop. Built by tools/agents/triedledger.py (never overwrites a file).', '',
            '| unit | function | % | sources |', '|---|---|---|---|']
    rows += ['| %s | [%s](%s.md) | %.2f | %d |' % (u, f, f, p, n) for u, f, p, n in sorted(index)]
    (OUT / 'README.md').write_text('\n'.join(rows) + '\n', encoding='utf-8', newline='\n')
    print('%d functions, %d new files' % (len(index), made))


if __name__ == '__main__':
    main()
