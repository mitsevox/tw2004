"""Check a round's name batches and globals files before any replay starts.
    python tools/agents/check_batches.py <file.tsv>...
Every file the round's lanes handed in, together: name.py batches (address, current, new, tier, codes,
evidence, purpose[, comment]) and rename.py globals files (address, current, new, evidence). Reports:
- a malformed row (wrong column count, a new name that is not a C identifier, a bad address);
- one new name given to two different addresses (in one lane or across lanes);
- a new name some other symbol already has on main (config/GW4E69/symbols.txt).
Exit status 1 on any finding: fix the files (or ask the lane) before replaying anything.
"""
import re, sys

ident = re.compile(r'^[A-Za-z_]\w*$')
addr = re.compile(r'^[0-9A-Fa-f]{8}$')
owner = {}                                      # current symbol name -> address, on main
for l in open('config/GW4E69/symbols.txt'):
    m = re.match(r'^(\S+) = \.\w+:0x([0-9A-Fa-f]+);', l)
    if m:
        owner[m.group(1)] = m.group(2).upper()
errors, given, rows = [], {}, 0
for f in sys.argv[1:]:
    for n, l in enumerate(open(f, encoding='utf-8'), 1):
        l = l.rstrip('\n')
        if not l.strip() or l.startswith('#'):
            continue
        c = l.split('\t')
        rows += 1
        where = '%s:%d' % (f, n)
        is_batch = len(c) >= 4 and re.match(r'^T\d$', c[3] or '')
        if not addr.match(c[0]):
            errors.append('%s: bad address %r' % (where, c[0]))
            continue
        if is_batch and len(c) < 7:
            errors.append('%s: a name.py row needs at least 7 columns, has %d' % (where, len(c)))
        if not is_batch and len(c) != 4:
            errors.append('%s: a globals row needs 4 columns (address, current, new, evidence), has %d'
                          % (where, len(c)))
            continue
        if len(c) < 3:
            continue
        new, a = c[2], c[0].upper()
        if new in ('KEEP', 'NONE') or new == c[1]:
            continue
        if not ident.match(new):
            errors.append('%s: %r is not a C identifier' % (where, new[:60]))
            continue
        if new in given and given[new][0] != a:
            errors.append('%s: %s also given to %s at %s' % (where, new, given[new][0], given[new][1]))
        given.setdefault(new, (a, where))
        if new in owner and owner[new] != a:
            errors.append('%s: %s is already the name of %s on main' % (where, new, owner[new]))
for e in errors:
    print(e)
print('%d rows checked (%d new names), %d problems' % (rows, len(given), len(errors)))
sys.exit(1 if errors else 0)
