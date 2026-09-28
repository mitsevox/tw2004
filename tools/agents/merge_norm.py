# Applies every function rename logged in name_sources.tsv (old -> new, in order) and the listed
# globals tsvs to one scratch file, so lane versions differ only in their hand edits.
import re, sys
f, *tsvs = sys.argv[1:]
s = open(f).read()
pairs = []
for l in open('config/GW4E69/name_sources.tsv'):
    if l.startswith('#') or not l.strip(): continue
    c = l.rstrip('\n').split('\t')
    if len(c) > 2 and c[2] != c[1]: pairs.append((c[2], c[1]))
for t in tsvs:
    for l in open(t):
        if l.startswith('#') or not l.strip(): continue
        c = l.rstrip('\n').split('\t'); pairs.append((c[1], c[2]))
for o, n in pairs:
    if o in s: s = re.sub(r'\b%s\b' % re.escape(o), n, s)
open(f, 'w').write(s)
