"""Merge one reviewed agent branch into main, verify, and push.
    python tools/agents/merge.py <agent-name> ["merge message"]
Steps: refresh main's report and save its exact-function set; merge agent/<name> with --no-ff (on a
conflict: abort the merge and list the files, so the owning agent can merge main and resolve); run
configure + a full build (DOL must be OK); rebuild the report and compare exact sets (no function may
be lost); lint the merged lines; push only if everything passed. On a failure after the merge commit
the merge is left uncommitted until every gate passes (main's ref never holds an unverified merge);
failed merge never stays on main for the next merge to stack on. Exit code 1 on any failure.
Run merges one at a time and stop at the first failure (chain with &&)."""
import json, pathlib, re, subprocess, sys
import paths

MAIN = paths.MAIN_S
name = sys.argv[1]
msg = sys.argv[2] if len(sys.argv) > 2 else f'Merge agent/{name}'
NL = '\n'


def run(cmd):
    return subprocess.run(cmd, shell=True, cwd=MAIN, capture_output=True, text=True)


def undo(why):
    # The merge is never committed before every gate passes (main's ref stays at `base`, so no lane can
    # pull an unverified merge). Undo = drop the uncommitted merge from the working tree.
    if run('git rev-parse HEAD').stdout.strip() == base:
        run(f'git reset -q --hard {base}')
        run('python configure.py')
        why += f'{NL}merge undone: main is back at {base[:7]}; agent/{name} still has its work'
    sys.exit(why)


def exact():
    """The exact functions, keyed by address so a rename (symbols.txt) is not a loss; the name is
    kept for the message."""
    r = json.load(open(MAIN + '/build/GW4E69/report.json'))
    return ({(f.get('metadata', {}).get('virtual_address') or f['name'], f['name'])
             for u in r['units'] for f in u.get('functions', []) if f.get('fuzzy_match_percent') == 100},
            r['measures'])


if run('git status --porcelain --untracked-files=no').stdout.strip():
    sys.exit('main has uncommitted changes; commit or stash them first')
# CI builds from scratch; a local build can hide a break behind stale objects (2026-09-24: CI was
# red for 6 hours of merges). Refuse to stack merges on a red main; a run still going is fine.
# (the public API, not gh: the Store Python sandboxes AppData, so gh run from here is logged out)
try:
    import urllib.request
    with urllib.request.urlopen('https://api.github.com/repos/mitsevox/tw2004/actions/runs'
                                '?branch=main&per_page=1', timeout=15) as resp:
        last = json.load(resp)['workflow_runs'][0]
except Exception as e:
    last = None
    print(f'WARNING: could not read the latest CI run ({e}); check it by hand')
if last and last['conclusion'] == 'failure':
    sys.exit(f"CI is RED on main ({last['head_sha'][:8]} {last['display_title'][:60]}): fix it before "
             f"merging more. `gh run view --log-failed`; reproduce with `ninja -t clean -r mwcc -r prodg` + ninja.")
run('ninja build/GW4E69/report.json')
before, mb = exact()
# (plain ninja never compiles NonMatching units; the all_source gate after the merge does)
types_before = [int(x) for x in run('python tools/match/typeaudit.py --count').stdout.split()]
syms_before = [int(x) for x in run('python tools/match/symaudit.py --count').stdout.split()]
def const_count():
    """constcheck: constants whose VALUE differs behind 100% functions (objdiff masks them)."""
    out = run('python tools/match/constcheck.py --count').stdout.split()
    return int(out[0]) if out and out[0].isdigit() else None
consts_before = const_count()
base = run('git rev-parse HEAD').stdout.strip()
m = run(f'git merge --no-ff --no-commit agent/{name}')
if m.returncode:
    conflicts = run('git diff --name-only --diff-filter=U').stdout.split()
    run('git merge --abort')
    sys.exit(f'CONFLICT, merge aborted. Files: {conflicts}{NL}-> ask the {name} agent to `git merge main` and resolve.')
def drop_dup_prototypes():
    """A local prototype identical to a header's (lint: dup-prototype) cannot change code; remove
    such lines on the merged result and commit, instead of a round trip to the agent."""
    import re
    # Re-enabled 2026-09-23: lint's dup-prototype is include-aware (tools/match/includes.py), and the
    # all_source gate below compiles NonMatching units, so a bad drop is caught and taken back.
    # Whole-file lint: a new header prototype also makes OLD local copies in other files duplicates.
    out = run('python tools/match/lint.py').stdout
    dups = re.findall(r'^(\S+\.c):(\d+): dup-prototype:', out, re.M)
    if not dups:
        return 0
    by_file = {}
    for f, _ in dups:
        saved[f] = open(f'{MAIN}/src/{f}', encoding='utf-8').read()
    for f, ln in dups:
        by_file.setdefault(f, []).append(int(ln))
    for f, lns in by_file.items():
        path = f'{MAIN}/src/{f}'
        lines = open(path, encoding='utf-8').read().split(NL)
        for ln in sorted(lns, reverse=True):
            del lines[ln - 1]
        open(path, 'w', encoding='utf-8', newline=NL).write(NL.join(lines))
    run('git add src')
    return len(dups)


saved = {}
# A branch started before a naming batch still writes the old names: rewrite them (names.md).
APPLIED = MAIN + '/config/GW4E69/name_sources.tsv'   # the repo's rename log (previous_name -> name)
import os
if os.path.exists(APPLIED):
    fx = run(f'python tools/match/rename.py {APPLIED} --refs-only')
    if fx.returncode:
        undo('rename --refs-only failed:' + NL + fx.stdout + fx.stderr)
    if ' 0 source file' not in fx.stdout:
        run('git add src include')
        print('old names rewritten: ' + fx.stdout.strip())
run('python configure.py')
n = drop_dup_prototypes()
if n:
    print(f'removed {n} duplicate prototype line(s) the headers already declare')
pathlib.Path(MAIN, 'build/GW4E69/ok').unlink(missing_ok=True)   # (no rm in cmd.exe)
b = run('ninja')
if n and ('main.dol: OK' not in b.stdout or run('ninja all_source').returncode):
    # A dropped line was the file's only declaration: put the files back and build the plain merge.
    for f, text in saved.items():
        open(f'{MAIN}/src/{f}', 'w', encoding='utf-8', newline='').write(text)
    run('git add src')
    n = 0
    run('python configure.py')
    pathlib.Path(MAIN, 'build/GW4E69/ok').unlink(missing_ok=True)   # (no rm in cmd.exe)
    b = run('ninja')
    print('dup-prototype drop broke the build; merged without it')
if 'main.dol: OK' not in b.stdout:
    errs = [l for l in b.stdout.splitlines() if 'rror' in l or 'FAILED' in l or 'undefined' in l][:15]
    if not errs:                          # no recognisable error line: show how the build ended
        errs = (b.stdout + b.stderr).strip().splitlines()[-15:]
    undo('DOL FAILED after merge (not pushed):' + NL + NL.join(errs) + NL +
         f'-> ask the {name} agent to `git merge main`, fix the build, and report again.')
src = run('ninja all_source')
if src.returncode:
    errs = [l for l in src.stdout.splitlines() if 'rror' in l or 'FAILED' in l][:15]
    undo('a NonMatching unit does not compile after merge (not pushed):' + NL + NL.join(errs) + NL +
         f'-> ask the {name} agent to `git merge main`, fix it, and report again.')
run('ninja build/GW4E69/report.json')
after, ma = exact()
after_addrs = {a for a, _ in after}
lost = sorted(n for a, n in before if a not in after_addrs)
lint = run(f'python tools/match/lint.py --diff {base}').stdout.strip().splitlines()
print('DOL OK | exact %d -> %d (+%d) | matched %.2f%% -> %.2f%% | linked %.2f%% -> %.2f%%' % (
    len(before), len(after), len(after_addrs - {a for a, _ in before}), mb['matched_code_percent'], ma['matched_code_percent'],
    mb['complete_code_percent'], ma['complete_code_percent']))
print('lint:', lint[-1] if lint else '?')
if lost:
    undo(f'LOST exact functions (not pushed): {lost}')
types_after = [int(x) for x in run('python tools/match/typeaudit.py --count').stdout.split()]
TYPE_COUNTS = ('conflicting globals', 'duplicated types', 'externs in .c', 'typedefs in .c')
print('type audit: ' + ', '.join('%s %d -> %d' % t for t in zip(TYPE_COUNTS, types_before, types_after)))
worse = [n for n, b, a in zip(TYPE_COUNTS, types_before, types_after) if a > b]
if worse:
    print(run('python tools/match/typeaudit.py').stdout)
    undo(f'type audit got worse (not pushed): {", ".join(worse)} rose. Shared types and externs '
         'belong in a header (docs/style.md); a .c file may not add an extern or a typedef.')
syms_after = [int(x) for x in run('python tools/match/symaudit.py --count').stdout.split()]
SYM_COUNTS = ('small-data size', 'too-big', 'scope')
print('symbol audit: ' + ', '.join('%s %d -> %d' % t for t in zip(SYM_COUNTS, syms_before, syms_after)))
worse = [n for n, b, a in zip(SYM_COUNTS, syms_before, syms_after) if a > b]
if worse:
    print(run('python tools/match/symaudit.py').stdout)
    undo(f'symbol audit got worse (not pushed): {", ".join(worse)} rose. Declare each global at its '
         'true size (symbols.txt size:) and with the original scope.')
if lint and lint[-1] != '0 findings':
    print(NL.join(lint[:-1][:20]))
    undo('lint findings on merged lines (not pushed)')
consts_after = const_count()
print(f'constant values: {consts_before} -> {consts_after}')
if consts_before is not None and consts_after is not None and consts_after > consts_before:
    print(run('python tools/match/constcheck.py').stdout[-3000:])
    undo('constcheck got worse (not pushed): a constant\'s value differs from the original. Write the '
         'exact expression EA used (decomp-notes "Try these first").')
# Shell edits (sed, heredocs, echo) strip backslashes. A removed line with a backslash that comes back
# identical except for its backslashes is that damage.
d = run(f'git diff -U0 {base} -- "*.c" "*.h"').stdout.splitlines()
removed = [l[1:] for l in d if l.startswith('-') and not l.startswith('---') and '\\' in l]
added = {l[1:] for l in d if l.startswith('+') and not l.startswith('+++')}
stripped = [l for l in removed if l.replace('\\', '') in added]
if stripped:
    print(NL.join(stripped[:10]))
    undo(f'backslashes stripped from {len(stripped)} line(s) (not pushed): a shell edit damaged the source. '
         f'-> ask the {name} agent to restore those lines with the Edit tool.')
# The audit baseline (tag audit-baseline-1, user's hard rule): matching never renames a function or
# changes an audited comment; it may only ADD matching notes. Print what changed for review.
ab = run('python tools/match/auditbaseline.py --list changed').stdout.splitlines()
if any('name' in l.split('\t')[-1] for l in ab):
    print(NL.join(l for l in ab if 'name' in l.split('\t')[-1])[:2000])
    undo('a function name changed since the audit baseline (not pushed): matching never renames')
cm = [l for l in d if l[:1] in '+-' and l[:3] not in ('+++', '---') and re.search(r'//|/\*', l)]
print('audit baseline: %d changed rows; comment lines in this merge: %d added, %d removed' % (
    len(ab), sum(l[0] == '+' for l in cm), sum(l[0] == '-' for l in cm)))
if any(l[0] == '-' for l in cm):
    print(NL.join(l[:160] for l in cm[:30]))
    print('REVIEW: removed/changed comment lines above must be matching notes only, else revert them')
c = run('git commit -q -m "%s"' % (msg + (f' (and drop {n} local prototypes the headers declare)' if n else '')))
if c.returncode:
    undo('commit failed: ' + c.stdout[-300:] + c.stderr[-300:])
p = run('git push -q')
print('pushed' if p.returncode == 0 else 'PUSH FAILED: ' + p.stderr[-500:])
