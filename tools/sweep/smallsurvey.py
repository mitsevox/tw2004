"""Survey the shapes of small unmatched functions in the auto_ asm files.
    python smallsurvey.py [maxbytes]
Prints the most common normalised instruction sequences."""
import collections, glob, json, re, sys

MAX = int(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1].isdigit() else 64
ASM = 'C:/dev/tw2004/build/GW4E69/asm/'
rep = json.load(open('C:/dev/tw2004/build/GW4E69/report.json'))
done = {f['name'] for u in rep['units'] for f in u.get('functions', []) if f.get('fuzzy_match_percent', 0) >= 100}


def parse(path):
    fns, cur = [], None
    for line in open(path, encoding='utf-8'):
        m = re.match(r'^\.fn (\S+),', line)
        if m:
            cur = {'name': m.group(1), 'ins': [], 'file': path}
            continue
        if line.startswith('.endfn'):
            fns.append(cur); cur = None; continue
        if cur is not None:
            m = re.match(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  ([0-9A-F ]{11}) \*/\t(.*)$', line.rstrip('\n'))
            if m:
                cur.setdefault('addr', int(m.group(1), 16))
                cur['ins'].append(m.group(3).strip())
            elif line.strip().startswith('.L_'):
                cur['ins'].append('LABEL')
    return fns


def norm(i):
    i = re.sub(r'\b(lbl_[0-9A-F]+|@\d+|[A-Za-z_]\w*)@(sda21|ha|l)', r'SYM@\2', i)
    i = re.sub(r'0x[0-9a-f]+|\b\d+\b', 'N', i)
    i = re.sub(r'^(b\w*) \S+$', r'\1 T', i)
    return i


if __name__ == '__main__':
    shapes = collections.Counter()
    ex = {}
    total = 0
    for path in sorted(glob.glob(ASM + 'auto_*_text.s')):
        for f in parse(path):
            n = len([i for i in f['ins'] if i != 'LABEL'])
            if f['name'] in done or n * 4 > MAX:
                continue
            total += 1
            key = ' ; '.join(norm(i) for i in f['ins'])
            shapes[key] += 1
            ex.setdefault(key, f['name'])
    print(total, 'functions <=', MAX, 'bytes')
    for k, c in shapes.most_common(40):
        print('%5d  %-80s %s' % (c, k[:80], ex[k]))
