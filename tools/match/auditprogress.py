"""How much of the existing game code the name and comment audit has covered (SDK excluded).
    python tools/match/auditprogress.py [--units]      --units: per-unit table of what is left
Reads config/GW4E69/audit.tsv (one row per audited function) and build/GW4E69/report.json.
Phases: 1 = functions in linked game units, 2 = functions in game units not yet linked,
3 = game headers (struct and field comments; counted by file)."""
import collections, json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
AUDIT = ROOT / 'config/GW4E69/audit.tsv'
HEADER = 'address\tunit\tname_before\tname_after\ttier\tdecision\tcomments\tlane1\tlane2\treconciler\tdate\tnotes'


def ledger():
    rows = {}
    if AUDIT.exists():
        cols = None
        for l in AUDIT.read_text(encoding='utf-8').splitlines():
            if not l.strip() or l.startswith('#'):
                continue
            f = l.split('\t')
            if f[0] == 'address':
                cols = f
                continue
            r = dict(zip(cols, f))
            rows[r['address']] = r                   # a later row for the same function wins
    return rows


def main():
    rep = json.loads((ROOT / 'build/GW4E69/report.json').read_text())
    led = ledger()
    phase = {1: [0, 0, 0, 0], 2: [0, 0, 0, 0]}        # functions, audited, named, named audited
    left = collections.Counter()
    for u in rep['units']:
        md = u.get('metadata', {})
        if 'game' not in md.get('progress_categories', []):
            continue
        ph = 1 if md.get('complete') else 2
        for f in u.get('functions', []):
            a = '%08X' % int(f.get('metadata', {}).get('virtual_address', 0))
            named = not re.fullmatch(r'fn_[0-9A-F]{8}', f['name'])
            done = a in led
            p = phase[ph]
            p[0] += 1
            p[1] += done
            p[2] += named
            p[3] += named and done
            if not done:
                left[(ph, u['name'].removeprefix('main/'))] += 1
    tot = [sum(x) for x in zip(phase[1], phase[2])]
    pct = lambda a, b: 100.0 * a / b if b else 0.0
    print('NAME AND COMMENT AUDIT (EA game code; SDK excluded)')
    for ph, label in ((1, 'Phase 1, linked units'), (2, 'Phase 2, units not yet linked')):
        p = phase[ph]
        print('  %-30s %5d / %5d functions  %5.1f%%   (named: %d / %d)' % (label, p[1], p[0], pct(p[1], p[0]), p[3], p[2]))
    print('  %-30s %5d / %5d functions  %5.1f%%   (named: %d / %d)' % ('ALL GAME CODE', tot[1], tot[0], pct(tot[1], tot[0]), tot[3], tot[2]))
    hdrs = [h for h in (ROOT / 'include').rglob('*.h') if not {'dolphin', 'MSL_C', 'libc'} & set(h.parts)]
    hdone = {r['unit'] for r in led.values() if r.get('unit', '').startswith('include/')}
    print('  %-30s %5d / %5d header files' % ('Phase 3, game headers', len(hdone), len(hdrs)))
    if led:
        dec = collections.Counter(r['decision'] for r in led.values())
        tier = collections.Counter(r['tier'] for r in led.values())
        fixed = sum(int(m.group(1)) for r in led.values() for m in [re.search(r'fixed:(\d+)', r['comments'])] if m)
        stripped = sum(int(m.group(1)) for r in led.values() for m in [re.search(r'stripped:(\d+)', r['comments'])] if m)
        print('  decisions: %s' % ', '.join('%s %d' % kv for kv in sorted(dec.items())))
        print('  tiers:     %s' % ', '.join('%s %d' % kv for kv in sorted(tier.items())))
        print('  comments:  %d rewritten, %d stripped' % (fixed, stripped))
    if '--units' in sys.argv:
        for (ph, u), n in sorted(left.items(), key=lambda x: (x[0][0], -x[1])):
            print('  phase %d  %-40s %4d left' % (ph, u, n))


if __name__ == '__main__':
    main()
