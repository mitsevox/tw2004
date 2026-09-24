"""Apply one RECONCILED audit batch to main, verify, commit, push.
    python tools/agents/audit_apply.py <batch dir> "<label>"
<batch dir> holds (made by the reconciler, checked by the orchestrator; see audit.md):
  renames.tsv   address, current, new, tier (T1/T2/T3/D), codes, evidence, purpose   (may be empty)
  comments.tsv  file, old, new     exact comment text to replace; new empty = remove the comment
                (use \\n inside old/new for multi-line comments)                    (may be empty)
  ledger.tsv    one row per audited function, columns as config/GW4E69/audit.tsv
  meta.txt      line 1: lane 1 model/lane, line 2: lane 2 model/lane, line 3: reconciler
Gates: main clean and CI not red; namecheck (renames); every comment edit matches exactly once;
auditcheck (only names and comment text changed; __LINE__ files keep their lines); configure +
ninja (main.dol: OK); ninja all_source; the exact functions by address unchanged. Any failure takes
everything back (only our own uncommitted edits: main was clean before). On success the renames
are logged in config/GW4E69/name_sources.tsv (with previous_name) and every audited function in
config/GW4E69/audit.tsv, committed, pushed."""
import datetime, json, pathlib, subprocess, sys, urllib.request
import paths

MAIN = paths.MAIN
SOURCES = MAIN / 'config/GW4E69/name_sources.tsv'
AUDIT = MAIN / 'config/GW4E69/audit.tsv'
AUDIT_HEADER = 'address\tunit\tname_before\tname_after\ttier\tdecision\tcomments\tlane1\tlane2\treconciler\tdate\tnotes'
NL = '\n'
if len(sys.argv) < 3:
    sys.exit(__doc__)
bdir, label = pathlib.Path(sys.argv[1]), sys.argv[2]


def run(cmd):
    return subprocess.run(cmd, shell=True, cwd=MAIN, capture_output=True, text=True)


def tsv(name):
    p = bdir / name
    if not p.exists():
        return []
    out = []
    for l in p.read_text(encoding='utf-8-sig').splitlines():
        f = [x.strip() for x in l.split('\t')]
        if not l.strip() or f[0].startswith('#') or f[0].lower() in ('address', 'file'):
            continue
        out.append(f)
    return out


def exact():
    r = json.loads((MAIN / 'build/GW4E69/report.json').read_text())
    return {f.get('metadata', {}).get('virtual_address') or f['name']
            for u in r['units'] for f in u.get('functions', []) if f.get('fuzzy_match_percent') == 100}


def fail(why):
    run('git checkout -- config src include')
    run('python configure.py')
    sys.exit(why + NL + 'batch taken back; main unchanged')


if run('git status --porcelain --untracked-files=no').stdout.strip():
    sys.exit('main has uncommitted changes; commit or stash them first')
try:
    with urllib.request.urlopen('https://api.github.com/repos/mitsevox/tw2004/actions/runs'
                                '?branch=main&per_page=1', timeout=15) as resp:
        if json.load(resp)['workflow_runs'][0]['conclusion'] == 'failure':
            sys.exit('CI is RED on main: fix it first')
except OSError as e:
    print(f'WARNING: could not read CI status ({e})')
meta = (bdir / 'meta.txt').read_text(encoding='utf-8').splitlines()
lane1, lane2, recon = (meta + ['?', '?', '?'])[:3]
renames = [(f + [''] * 7)[:7] for f in tsv('renames.tsv')]
comments = []
if (bdir / 'comments.tsv').exists():
    # read raw: a `new` of one space means "remove the comment but keep its line" (__LINE__ files)
    for l in (bdir / 'comments.tsv').read_text(encoding='utf-8-sig').split('\n'):
        f = l.rstrip('\r').split('\t')
        if len(f) < 2 or not f[0].strip() or f[0].startswith('#') or f[0].strip().lower() == 'file':
            continue
        comments.append((f[0].strip(), f[1], f[2] if len(f) > 2 else ''))
ledger = tsv('ledger.tsv')
if not ledger:
    sys.exit('ledger.tsv is empty: every audited function needs a row')
run('ninja build/GW4E69/report.json')
before = exact()
if renames:
    nc = run(f'python tools/match/namecheck.py "{bdir / "renames.tsv"}"')
    if nc.returncode:
        sys.exit('namecheck failed (nothing changed):' + NL + nc.stdout + nc.stderr)
    r = run(f'python tools/match/rename.py "{bdir / "renames.tsv"}"')
    if r.returncode:
        sys.exit(r.stdout + r.stderr)
    print(r.stdout.strip())
# comment edits run AFTER the renames, so their text uses the new names
for fname, old, new in comments:
    p = MAIN / fname
    old, new = old.replace('\\n', NL), new.replace('\\n', NL)
    t = p.read_bytes().decode('utf-8', errors='surrogateescape')
    if t.count(old) != 1:
        fail('%s: the comment to edit is found %d times (need exactly 1): %r' % (fname, t.count(old), old[:80]))
    if new == ' ':
        # remove the comment, keep the line (a file that bakes in __LINE__ must not lose lines)
        i = t.index(old)
        t = t[:i].rstrip(' ') + t[i + len(old):]
    elif not new.strip():
        i = t.index(old)
        ls = t.rfind('\n', 0, i) + 1
        le = t.find('\n', i + len(old))
        le = len(t) if le < 0 else le
        if not t[ls:i].strip() and not t[i + len(old):le].strip():
            # a comment alone on its line(s): the line goes too, unless the file bakes in __LINE__
            # (auditcheck refuses a moved line; the reconciler then writes an empty replacement)
            t = t[:ls] + t[le + 1:]
        else:
            t = t[:i].rstrip(' ') + t[i + len(old):]
    else:
        t = t.replace(old, new, 1)
    p.write_bytes(t.encode('utf-8', errors='surrogateescape'))
ac = run('python tools/match/auditcheck.py')
if ac.returncode:
    fail('auditcheck failed:' + NL + ac.stdout)
run('python configure.py')
(MAIN / 'build/GW4E69/ok').unlink(missing_ok=True)
b = run('ninja')
if 'main.dol: OK' not in b.stdout:
    fail('build/DOL failed:' + NL + NL.join(l for l in b.stdout.splitlines() if 'rror' in l or 'FAILED' in l)[:2000])
if run('ninja all_source').returncode:
    fail('ninja all_source failed')
run('ninja build/GW4E69/report.json')
after = exact()
if before - after:
    fail('exact functions lost: ' + ', '.join(sorted(map(str, before - after)))[:2000])
today = datetime.date.today().isoformat()
with open(SOURCES, 'a', encoding='utf-8', newline=NL) as out:
    for a, cur, new, tier, codes, ev, purpose in renames:
        out.write('\t'.join(x.replace('\t', ' ') for x in (
            '%08X' % int(a.removeprefix('0x'), 16), new, cur, tier, codes, ev, purpose,
            lane1 + ' | ' + lane2, recon, today)) + NL)
new_ledger = not AUDIT.exists()
with open(AUDIT, 'a', encoding='utf-8', newline=NL) as out:
    if new_ledger:
        out.write('# The name and comment audit of EA game code: one row per audited function (a later\n'
                  '# row for the same address supersedes an earlier one). Two blind lanes read each function;\n'
                  '# a reconciler compared them with its name and comments. tier: T1 EA text or confirmed\n'
                  '# TW06 name, T2 struct-bound and agreed, T3 agreed guess (fn_<address>_<Guess>), D demoted,\n'
                  '# - no name. decision: kept, renamed, tier3, demoted, unnamed. comments: ok, none,\n'
                  '# fixed:N, stripped:N. Progress: tools/match/auditprogress.py.\n' + AUDIT_HEADER + NL)
    for f in ledger:
        f = (f + [''] * 12)[:12]
        f[7], f[8], f[9], f[10] = f[7] or lane1, f[8] or lane2, f[9] or recon, today
        out.write('\t'.join(x.replace('\t', ' ') for x in f) + NL)
msg = (f'Audit: {label} ({len(ledger)} functions; {len(renames)} renames, {len(comments)} comment edits){NL}{NL}'
       f'Lane 1: {lane1}. Lane 2: {lane2}. Reconciled: {recon}.{NL}'
       f'Rows in config/GW4E69/audit.tsv; renames with evidence in config/GW4E69/name_sources.tsv.{NL}{NL}'
       + NL.join('%s -> %s [%s]: %s' % (x[1], x[2], x[3], x[5][:300]) for x in renames))
(MAIN / '.git/AUDIT_MSG').write_text(msg, encoding='utf-8')
run('git add config src include')
c = run('git commit -q -F .git/AUDIT_MSG')
if c.returncode:
    fail('commit failed: ' + c.stderr)
p = run('git push -q origin main')
prog = run('python tools/match/auditprogress.py').stdout
print(f'committed {run("git rev-parse --short HEAD").stdout.strip()}; '
      f'{"pushed" if not p.returncode else "PUSH FAILED: " + p.stderr}; exact {len(after)}{NL}{prog}')
