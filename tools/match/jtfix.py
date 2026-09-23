"""Make a unit's asm m2c-ready for switch statements: add .L_ labels at every jump-table target
and emit the tables as `.data` objects that point at those labels.

    python tools/match/jtfix.py <unit asm> <data asm> <out asm>
"""
import re, sys
text_path, data_path, out_path = sys.argv[1:4]
text = open(text_path, encoding='utf-8').read()
data = open(data_path, encoding='utf-8').read()

# function start addresses in the unit
starts = {}
for m in re.finditer(r'^\.fn (\S+),.*\n(?:#.*\n)*/\* ([0-9A-F]{8}) ', text, re.M):
    starts[m.group(1).rstrip(',')] = int(m.group(2), 16)
for m in re.finditer(r'# \.text:0x[0-9A-F]+ \| 0x([0-9A-F]+) \| size: 0x[0-9A-F]+\n\.fn (\S+),', text):
    starts[m.group(2)] = int(m.group(1), 16)

tables = sorted(set(re.findall(r'(jumptable_[0-9A-F]{8})@ha', text)))
targets = set()
out_tables = []
for t in tables:
    m = re.search(r'\.obj %s, \w+\n(.*?)\.endobj %s' % (t, t), data, re.S)
    ents = []
    for e in re.findall(r'\.4byte (\S+)', m.group(1)):
        fn, _, off = e.partition('+')
        a = starts[fn] + (int(off, 16) if off else 0)
        targets.add(a)
        ents.append('.L_%08X' % a)
    out_tables.append('.obj %s, global\n' % t + ''.join('\t.4byte %s\n' % x for x in ents) + '.endobj %s\n' % t)

lines = text.split('\n')
have = set(int(x, 16) for x in re.findall(r'^\.L_([0-9A-F]{8}):', text, re.M))
out = []
for l in lines:
    m = re.match(r'/\* ([0-9A-F]{8}) ', l)
    if m:
        a = int(m.group(1), 16)
        if a in targets and a not in have:
            out.append('.L_%08X:' % a)
    out.append(l)
res = '\n'.join(out) + '\n\n.section .data\n' + '\n'.join(out_tables)
open(out_path, 'w', encoding='utf-8').write(res)
print(len(tables), 'tables,', len(targets), 'targets')
