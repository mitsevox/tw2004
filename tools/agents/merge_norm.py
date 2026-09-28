# merge_norm.py <file> [globals.tsv ...]: rewrites a scratch copy of a source file so every old
# function or global name reads as its current one (config/GW4E69/name_sources.tsv plus the lane's
# globals tsvs), in ONE pass: a lane's version and main's then differ only in their hand edits.
# A name some function or global still has today is never mapped (history holds swaps, such as
# ComputePurseForBracket <-> ComputeFirstPrizeForBracket, that a step-by-step replay would corrupt).
import re, sys
f, *tsvs = sys.argv[1:]
current = {}                                    # address -> current name
for l in open('config/GW4E69/symbols.txt'):
    m = re.match(r'^(\S+) = \.\w+:0x([0-9A-Fa-f]+);', l)
    if m:
        current[m.group(2).upper()] = m.group(1)
live = set(current.values())
old2new = {}
for l in open('config/GW4E69/name_sources.tsv'):
    if l.startswith('#') or not l.strip():
        continue
    c = l.rstrip('\n').split('\t')
    if len(c) > 2 and c[0].upper() in current:
        now = current[c[0].upper()]
        for old in (c[1], c[2]):
            if old != now and old not in live:
                old2new[old] = now
for t in tsvs:
    for l in open(t):
        if l.startswith('#') or not l.strip():
            continue
        c = l.rstrip('\n').split('\t')
        if len(c) > 2 and c[1] not in live:
            old2new[c[1]] = c[2]
s = open(f, encoding='utf-8', errors='surrogateescape').read()
if old2new:
    rx = re.compile(r'\b(' + '|'.join(map(re.escape, sorted(old2new, key=len, reverse=True))) + r')\b')
    s = rx.sub(lambda m: old2new[m.group(1)], s)
open(f, 'w', encoding='utf-8', errors='surrogateescape').write(s)
