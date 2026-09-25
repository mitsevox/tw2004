"""merge.py's asm gate: a merge may not ADD assembly to src/ unless it was checked by hand.

    python tools/agents/asmgate.py <base> [<head>]      report on a diff without merging

Two rules for every added line that is an `asm` function or block (`asm void f(...)`, `asm {`) or
an `__asm` statement:
1. It needs its plain-C fallback: under `#ifdef __MWERKS__` with an `#else` holding C (lint's
   port-asm-no-fallback check, with `port:` exemptions ignored). Refused otherwise.
2. Even with a fallback it is refused unless merge.py runs with --allow-asm. Pasting the target's
   own instructions makes any function "match" and proves nothing about the C beside it (an outside
   agent did this for 13 functions on 2026-09-25); only EA's own asm functions belong in src/, and
   the orchestrator confirms that by hand before passing --allow-asm."""
import pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'match'))
from lint import asm_fallback_hits                   # noqa: E402

ASM = re.compile(r'^\s*asm\b|\basm\s*\{|\b__asm\b')


def added_asm(diff):
    """{file: [(new line number, text)]} of added asm lines in src/, from `git diff -U0` output."""
    out, cur, n = {}, None, 0
    for l in diff:
        if l.startswith('+++ '):
            cur = l[6:] if l.startswith('+++ b/') else None
        elif l.startswith('@@'):
            n = int(re.search(r'\+(\d+)', l).group(1))
        elif l.startswith('+') and not l.startswith('+++'):
            if cur and cur.startswith('src/') and ASM.search(l[1:]) and not l[1:].lstrip().startswith('//'):
                out.setdefault(cur, []).append((n, l[1:].rstrip()))
            n += 1
        elif not l.startswith('-'):
            n += 1
    return out


def unguarded(lines):
    """Line numbers of asm lines (any form, inline blocks included) that are not inside an
    `#ifdef __MWERKS__` / `#if defined(__MWERKS__)` block with an `#else`."""
    blocks, stack = [], []                  # (start, else line or None, end, is_mwerks)
    for i, l in enumerate(lines, 1):
        s = l.strip()
        if re.match(r'#\s*if', s):
            stack.append([i, None, None, re.match(r'#\s*(ifdef\s+__MWERKS__|if\s+defined\s*\(?\s*__MWERKS__)',
                                                   s) is not None])
        elif re.match(r'#\s*(else|elif)', s) and stack and stack[-1][1] is None:
            stack[-1][1] = i
        elif re.match(r'#\s*endif', s) and stack:
            b = stack.pop()
            b[2] = i
            blocks.append(b)
    out = set()
    for i, l in enumerate(lines, 1):
        if ASM.search(l) and not l.lstrip().startswith('//'):
            if not any(mw and st < i and (el is None or i < el) and el is not None and en is not None
                       for st, el, en, mw in blocks):
                out.add(i)
    return out


def check(diff, root, allow):
    """(ok, report lines). root: the checkout whose src/ holds the merged files."""
    found = added_asm(diff)
    if not found:
        return True, []
    report, missing = [], []
    for f, rows in found.items():
        lines = (pathlib.Path(root) / f).read_text(encoding='utf-8', errors='replace').split('\n')
        bad = {h[0] for h in asm_fallback_hits(lines, honor_exempt=False)} | unguarded(lines)
        for n, text in rows:
            tag = 'NO C FALLBACK' if n in bad else 'asm (C fallback present)'
            if n in bad:
                missing.append((f, n))
            report.append('%s:%d: %s: %s' % (f, n, tag, text.strip()[:120]))
    if missing:
        return False, report + ['asm gate: %d added asm line(s) without a plain-C fallback' % len(missing)]
    if not allow:
        return False, report + ['asm gate: the merge adds asm. Pasted target instructions prove nothing '
                                'about the C beside them. If this is EA\'s own asm function (checked by '
                                'hand), re-run merge.py with --allow-asm.']
    return True, report + ['asm gate: asm allowed by --allow-asm']


if __name__ == '__main__':
    base = sys.argv[1]
    head = sys.argv[2] if len(sys.argv) > 2 else 'HEAD'
    d = subprocess.run(['git', 'diff', '-U0', '%s...%s' % (base, head), '--', 'src'], cwd=ROOT,
                       capture_output=True, text=True).stdout.splitlines()
    # read the files as they are at head, into a scratch copy of only what the check needs
    found = added_asm(d)
    tmp = ROOT / 'build' / 'asmgate'
    for f in found:
        p = tmp / f
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(subprocess.run(['git', 'show', '%s:%s' % (head, f)], cwd=ROOT, capture_output=True,
                                    text=True).stdout, encoding='utf-8')
    ok, rep = check(d, tmp, '--allow-asm' in sys.argv)
    print('\n'.join(rep) or 'asm gate: no asm added')
    sys.exit(0 if ok else 1)
