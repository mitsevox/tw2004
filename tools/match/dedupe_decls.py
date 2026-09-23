"""Drop duplicate top-level declarations in a merged sweep file: when a name is declared more than
once, keep the first typed one (with parameters) and drop the rest. Only column-0 lines count."""
import re, sys
p = sys.argv[1]
s = open(p, encoding='utf-8').read()
lines = s.split('\n')
decl = {}
for i, l in enumerate(lines):
    if l.startswith((' ', '\t')):
        continue
    m = re.match(r'^(?:extern\s+)?[A-Za-z_][\w \*]*?\b(\w+)\((.*)\);$', l)
    if m:
        decl.setdefault(m.group(1), []).append((i, m.group(2)))
drop = set()
for name, lst in decl.items():
    if len(lst) > 1:
        typed = [x for x in lst if x[1].strip() != '']
        untyped = [x for x in lst if x[1].strip() == '']
        if typed:
            for x in untyped:
                drop.add(x[0])
            for x in typed[1:]:
                drop.add(x[0])
        else:
            for x in lst[1:]:
                drop.add(x[0])
print('dropping', [lines[i] for i in sorted(drop)])
lines = [l for i, l in enumerate(lines) if i not in drop]
open(p, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines))
