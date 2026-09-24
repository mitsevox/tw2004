"""The audit baseline: a fingerprint of what the name and comment audit checked, so that later we can
tell exactly which names and comments are still the audited ones and which changed since.
    python tools/match/auditbaseline.py                  status: every game function and file, by class
    python tools/match/auditbaseline.py --list <class>   the functions/files in one class
    python tools/match/auditbaseline.py --write <draft list> <tag>
        (once, at an audit milestone) write config/GW4E69/audit_baseline.tsv from the current tree;
        <draft list> = a file whose lines start with the addresses audited from non-exact draft C
    python tools/match/auditbaseline.py --rehash
        only when this script's fingerprint method changes: recompute the hashes from the source at
        the baseline tag (what was audited stays the same)
Classes:
  audited        passed the audit; name, comments and code unchanged since the baseline
  draft          passed the audit, but the audit read our non-exact draft C: re-check its comments
                 against the final code once it matches
  changed        passed the audit, but its name, comments or code changed since (the reason column
                 says which; `git diff <tag> -- <file>` shows the text). Code: renaming a callee does
                 not count; matching work on a draft function does not either (it stays draft)
  new            a game function the baseline does not have
  headers        game headers: struct field comments were never audited (phase 3)
Never audited anywhere (the baseline covers function names and comments only): names of globals,
types, struct fields and parameters; they are claims earlier lanes wrote."""
import hashlib, json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BASE = ROOT / 'config/GW4E69/audit_baseline.tsv'
AUDIT = ROOT / 'config/GW4E69/audit.tsv'
COLS = ['kind', 'address', 'unit', 'file', 'name', 'audited_on', 'tier', 'decision', 'comments', 'comments_sha', 'code_sha']


def mask(t):
    """(comments, code): code has comments and string/char contents blanked (same length, line
    breaks kept); comments is a list of (start, text)."""
    code, comments, i, n = list(t), [], 0, len(t)
    while i < n:
        c = t[i]
        if c in '"\'':
            j = i + 1
            while j < n and t[j] != c and t[j] != '\n':
                j += 2 if t[j] == '\\' else 1
            for k in range(i + 1, min(j, n)):
                code[k] = ' '
            i = j + 1
        elif t.startswith('//', i) or t.startswith('/*', i):
            j = (t.find('\n', i) if t[i + 1] == '/' else t.find('*/', i + 2))
            j = n if j < 0 else (j if t[i + 1] == '/' else j + 2)
            comments.append((i, t[i:j]))
            for k in range(i, j):
                if code[k] != '\n':
                    code[k] = ' '
            i = j
        else:
            i += 1
    return comments, ''.join(code)


def line_end(code, i):
    """After a top-level ';' or '}' at i: the end of its line when the rest of the line is blank
    (a trailing comment there belongs to that item, not to the next one), else i + 1."""
    j = code.find('\n', i + 1)
    j = len(code) if j < 0 else j
    return j if not code[i + 1:j].strip() else i + 1


def definitions(t):
    """name -> [(start, end)] spans of each top-level function definition, where start is just after
    the previous top-level item (so the comment above the function belongs to it); plus the source."""
    comments, code = mask(t)
    out, depth, prev, head = {}, 0, 0, 0
    i, n = 0, len(code)
    while i < n:
        c = code[i]
        if c == '#' and (i == 0 or code[i - 1] == '\n') and depth == 0:
            j = code.find('\n', i)
            while j > 0 and code[j - 1] == '\\':
                j = code.find('\n', j + 1)
            i = n if j < 0 else j
            prev = head = i
            continue
        if c == '{':
            if depth == 0:
                head = i
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                hdr = code[prev:head]
                m = (re.search(r'\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*$', hdr) or    # T name(args)
                     re.search(r'\(\s*\*\s*([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\)\s*[\[(][^;{}]*[\])]\s*$', hdr))  # T (*name(args))[4]
                end = line_end(code, i)
                if m and not re.search(r'=\s*$', hdr) and not re.search(r'\b(struct|union|enum)\b[^()]*$', hdr[:m.start()]):
                    out.setdefault(m.group(1), []).append((prev, end))
                prev = end
        elif c == ';' and depth == 0:
            prev = line_end(code, i)
        i += 1
    return out, t, comments, code


def sha(parts):
    return hashlib.sha1('\n'.join(parts).encode('utf-8')).hexdigest()[:12]


def worktree(rel):
    return (ROOT / rel).read_text(encoding='utf-8', errors='replace')


def fingerprint(read=worktree):
    """{('fn', address): row, ('file', path): row} for every game function and file, from the source
    text read(path) gives (default: the working tree). The comments outside functions are hashed as
    a sorted set: moving a global's declaration with its comment is not a change."""
    rep = json.loads((ROOT / 'build/GW4E69/report.json').read_text())
    rows = {}
    game = [u for u in rep['units'] if 'game' in u.get('metadata', {}).get('progress_categories', [])]
    addr = {f['name']: 'fn_%08X' % int(f['metadata']['virtual_address']) for u in game for f in u.get('functions', [])
            if f.get('metadata', {}).get('virtual_address')}
    ident = lambda m: addr.get(m.group(0), m.group(0))   # a later rename of a callee is not a code change
    for u in rep['units']:
        md = u.get('metadata', {})
        if 'game' not in md.get('progress_categories', []) or not md.get('source_path'):
            continue
        rel = md['source_path'].replace('\\', '/')
        defs, t, comments, code = definitions(read(rel))
        owned = set()
        for f in u.get('functions', []):
            a = int(f.get('metadata', {}).get('virtual_address', 0))
            spans = defs.get(f['name'], [])
            cm = [txt.strip() for s, e in spans for (o, txt) in comments if s <= o < e]
            owned |= {o for s, e in spans for (o, _) in comments if s <= o < e}
            body = [re.sub(r'\b[A-Za-z_]\w*\b', ident, re.sub(r'\s+', ' ', code[s:e]).strip()) for s, e in spans]
            rows[('fn', '%08X' % a)] = {'kind': 'fn', 'address': '%08X' % a, 'unit': u['name'].split('/', 1)[1],
                                        'file': rel, 'name': f['name'], 'comments': str(len(cm)),
                                        'comments_sha': sha(cm) if spans else 'missing', 'code_sha': sha(body) if spans else 'missing'}
        rest = [txt.strip() for o, txt in comments if o not in owned]
        rows[('file', rel)] = {'kind': 'file', 'address': '-', 'unit': u['name'].split('/', 1)[1], 'file': rel,
                               'name': '-', 'comments': str(len(rest)), 'comments_sha': sha(sorted(rest)), 'code_sha': '-'}
    return rows


def ledger():
    led, cols = {}, None
    for l in AUDIT.read_text(encoding='utf-8').splitlines():
        if not l.strip() or l.startswith('#'):
            continue
        f = l.split('\t')
        if f[0] == 'address':
            cols = f
            continue
        led[f[0]] = dict(zip(cols, f))
    return led


def load_base():
    tag, rows = '?', {}
    for l in BASE.read_text(encoding='utf-8').splitlines():
        m = re.match(r'# baseline tag: (\S+)', l)
        if m:
            tag = m.group(1)
        if not l.strip() or l.startswith('#') or l.startswith('kind\t'):
            continue
        r = dict(zip(COLS, l.split('\t')))
        rows[(r['kind'], r['address'] if r['kind'] == 'fn' else r['file'])] = r
    return tag, rows


def write(draft_file, tag):
    draft = {m.group(1).upper() for m in re.finditer(r'^([0-9A-Fa-f]{8})\b', pathlib.Path(draft_file).read_text(encoding='utf-8'), re.M)}
    led, cur = ledger(), fingerprint(worktree)
    missing = [k[1] for k in cur if k[0] == 'fn' and k[1] not in led]
    if missing:
        sys.exit('not every game function is audited (%d missing, e.g. %s): no baseline' % (len(missing), missing[:5]))
    bad = [r['name'] for r in cur.values() if r['comments_sha'] == 'missing']
    if bad:
        sys.exit('definition not found for %d functions, e.g. %s' % (len(bad), bad[:5]))
    lines = ['# The audit baseline (tools/match/auditbaseline.py): what the name and comment audit of EA',
             '# game code checked, fingerprinted at git tag %s (every game function audited, 100%%).' % tag,
             '# baseline tag: %s' % tag,
             '# One row per game function (kind fn) and per game source file (kind file: the comments outside',
             '# functions, e.g. the unit header). audited_on: exact = the audit read byte-matching C; draft = it',
             '# read our non-exact draft C (re-check the comments once the function matches). comments_sha /',
             '# code_sha: hashes of the comments and of the comment-free code as audited. Not covered: struct',
             '# field comments in headers (phase 3) and the names of globals, types, fields and parameters.',
             '\t'.join(COLS)]
    for k in sorted(cur, key=lambda k: (k[0] != 'file', cur[k]['file'], k[1])):
        r = dict(cur[k])
        if k[0] == 'fn':
            a = led[k[1]]
            r.update(audited_on='draft' if k[1] in draft else 'exact', tier=a['tier'], decision=a['decision'])
        else:
            r.update(audited_on='exact', tier='-', decision='-')
        lines.append('\t'.join(r[c] for c in COLS))
    BASE.write_text('\n'.join(lines) + '\n', encoding='utf-8', newline='\n')
    fns = [r for k, r in cur.items() if k[0] == 'fn']
    print('wrote %s: %d functions (%d from draft C), %d files' % (BASE.relative_to(ROOT).as_posix(), len(fns),
          sum(1 for r in fns if r['address'] in draft), sum(1 for k in cur if k[0] == 'file')))


def rehash():
    """The fingerprint method changed: recompute every hash from the source AT THE BASELINE TAG (git
    show), keeping audited_on, tier and decision. What was audited does not change, only how it is hashed."""
    import subprocess
    tag, base = load_base()
    at_tag = lambda rel: subprocess.run(['git', 'show', '%s:%s' % (tag, rel)], cwd=ROOT, capture_output=True,
                                        check=True).stdout.decode('utf-8', errors='replace')
    cur = fingerprint(at_tag)
    lost = [k for k in base if k not in cur or cur[k]['name'] != base[k]['name']]
    if lost:
        sys.exit('%d baseline rows have no function of that name at the tag, e.g. %s' % (len(lost), lost[:5]))
    head = [l for l in BASE.read_text(encoding='utf-8').splitlines() if l.startswith('#') or l.startswith('kind\t')]
    lines = list(head)
    for k in sorted(base, key=lambda k: (k[0] != 'file', base[k]['file'], k[1])):
        r = dict(base[k])
        r.update({c: cur[k][c] for c in ('comments', 'comments_sha', 'code_sha')})
        lines.append('\t'.join(r[c] for c in COLS))
    BASE.write_text('\n'.join(lines) + '\n', encoding='utf-8', newline='\n')
    print('rehashed %d rows from tag %s' % (len(base), tag))


def classify():
    tag, base = load_base()
    cur = fingerprint()
    out = []
    for k, r in cur.items():
        b = base.get(k)
        if b is None:
            out.append(('new', r, 'not in the baseline'))
            continue
        why = [w for w, c in (('name', 'name'), ('comments', 'comments_sha'), ('code', 'code_sha')) if r[c] != b[c]]
        if b['audited_on'] == 'draft' and why == ['code']:
            out.append(('draft', r, 'audited from draft C; the code changed since (matching)'))
        elif why:
            out.append(('changed', r, ', '.join(why) + ' changed' + (' (was %s)' % b['name'] if 'name' in why else '')
                        + ('; audited from draft C' if b['audited_on'] == 'draft' else '')))
        elif b['audited_on'] == 'draft':
            out.append(('draft', r, 'audited from draft C'))
        else:
            out.append(('audited', r, ''))
    hdrs = [h for h in (ROOT / 'include').rglob('*.h') if not {'dolphin', 'MSL_C', 'libc'} & set(h.parts)]
    for h in sorted(hdrs):
        out.append(('headers', {'kind': 'file', 'address': '-', 'file': h.relative_to(ROOT).as_posix(), 'name': '-'}, 'struct field comments not audited (phase 3)'))
    return tag, out


def main():
    if '--write' in sys.argv:
        i = sys.argv.index('--write')
        return write(sys.argv[i + 1], sys.argv[i + 2])
    if '--rehash' in sys.argv:
        return rehash()
    tag, out = classify()
    if '--list' in sys.argv:
        want = sys.argv[sys.argv.index('--list') + 1]
        for cls, r, why in out:
            if cls == want:
                print('%s\t%s\t%s\t%s' % (r['address'], r['name'], r['file'], why))
        return
    print('AUDIT BASELINE (tag %s): what is still exactly as audited' % tag)
    for cls in ('audited', 'draft', 'changed', 'new', 'headers'):
        fn = sum(1 for c, r, _ in out if c == cls and r['kind'] == 'fn')
        fl = sum(1 for c, r, _ in out if c == cls and r['kind'] == 'file')
        print('  %-8s %5d functions  %4d files' % (cls, fn, fl))
    print('  never audited: names of globals, types, struct fields, parameters')


if __name__ == '__main__':
    main()
