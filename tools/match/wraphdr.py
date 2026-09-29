"""Rewrap comments that run past 110 columns (headers and C files).
    python tools/match/wraphdr.py <file>...            # rewrite in place; read `git diff` after
    python tools/match/wraphdr.py --from-lint --diff REV  # every file lint --diff REV flags
Three shapes:
- a full-line comment block (lines of `//` at one indent): rewrapped at 100 columns;
- a trailing comment (`code  // text`, with continuation lines of `//` in the same column): its words
  rewrapped in that column (continuations keep their extra indent, e.g. a field's `// 0x38  ` text
  keeps lining up under the text) when at least 30 columns are left, else moved above the code line
  as a full-line comment at the code's indent;
- a group comment using `}` markers on several lines is left alone and reported (rewrap by hand).
A code line too long by itself is only reported. Text is never changed, only where lines break.
"""
import pathlib, re, subprocess, sys, textwrap

LIMIT, WIDTH, MIN_ROOM = 110, 100, 30


def split_comment(l):
    """(code, column of //, text after //) or (l, -1, None). Ignores // inside strings."""
    q = False
    for i, ch in enumerate(l):
        if ch == '"' and (i == 0 or l[i - 1] != '\\'):
            q = not q
        if not q and l.startswith('//', i):
            return l[:i], i, l[i + 2:]
    return l, -1, None


def wrap(words, width):
    return textwrap.wrap(' '.join(words), width, break_long_words=False, break_on_hyphens=False) or ['']


def fix_file(path):
    lines = path.read_text(encoding='utf-8').split('\n')
    manual, out, i, changed = [], [], 0, False
    while i < len(lines):
        l = lines[i]
        code, col, text = split_comment(l)
        pure = col >= 0 and not code.strip()
        # a full-line comment block at its own indent (not a trailing comment's continuation)
        if pure and not (out and split_comment(out[-1])[1] == col and split_comment(out[-1])[0].strip()):
            j = i
            while j < len(lines):
                c2, col2, t2 = split_comment(lines[j])
                if col2 != col or c2.strip():
                    break
                j += 1
            block = lines[i:j]
            if any(len(b) > LIMIT for b in block):
                if any(re.match(r'\s*(port|fake match|EA bug|TODO):', split_comment(b)[2] or '') for b in block[1:]):
                    manual.append((i + 1, 'label inside a comment block'))
                    out.extend(block)
                else:
                    words = ' '.join((split_comment(b)[2] or '').strip() for b in block).split()
                    new = [' ' * col + '// ' + w for w in wrap(words, WIDTH - col - 3)]
                    out.extend(new)
                    changed = changed or new != block
            else:
                out.extend(block)
            i = j
            continue
        # a code line with a trailing comment, plus its continuation lines in the same column
        if col >= 0 and code.strip():
            j = i + 1
            while j < len(lines):
                c2, col2, t2 = split_comment(lines[j])
                if col2 != col or c2.strip():
                    break
                j += 1
            block = lines[i:j]
            if not any(len(b) > LIMIT for b in block):
                out.extend(block)
                i = j
                continue
            conts = [split_comment(b)[2] for b in block[1:]]
            def grouped(t):     # `} text` after an optional field offset: a comment shared by fields
                return re.match(r'\s*(0x[0-9A-Fa-f]+\s+)?\}', t) is not None
            nxt = split_comment(lines[j])[2] if j < len(lines) else None
            if grouped(text) or any(grouped(t) for t in conts) or (nxt is not None and grouped(nxt)):
                manual.append((i + 1, 'grouped comment with } markers'))
                out.extend(block)
                i = j
                continue
            m = re.match(r'( ?0x[0-9A-Fa-f]+ +)', text)       # a field's offset: text lines up after it
            pad = ' ' * len(m.group(1)) if m else ' '
            first = m.group(1) if m else ' '
            words = (text[len(m.group(1)):] if m else text).split() + ' '.join(t.strip() for t in conts).split()
            room = LIMIT - col - 2 - len(first)
            if room >= MIN_ROOM and len(code.rstrip()) < col:
                body = wrap(words, room)
                new = [code + '//' + first + body[0]] + [' ' * col + '//' + pad + b for b in body[1:]]
            else:
                ind = re.match(r'\s*', code).group(0)
                lead = (m.group(1).strip() + ' ') if m else ''
                body = wrap((lead + ' '.join(words)).split(), WIDTH - len(ind) - 3)
                new = [ind + '// ' + b for b in body] + [code.rstrip()]
                if len(code.rstrip()) > LIMIT:
                    manual.append((i + 1, 'code line too long by itself'))
            out.extend(new)
            changed = changed or new != block
            i = j
            continue
        if len(l) > LIMIT:
            manual.append((i + 1, 'code line too long by itself'))
        out.append(l)
        i += 1
    if changed:
        path.write_text('\n'.join(out), encoding='utf-8')
    return changed, manual


def main():
    args = sys.argv[1:]
    if '--from-lint' in args:
        rev = args[args.index('--diff') + 1]
        res = subprocess.run([sys.executable, 'tools/match/lint.py', '--diff', rev], capture_output=True, text=True)
        files = set()
        for l in res.stdout.splitlines():
            m = re.match(r'(\S+?):\d+: long-line', l)
            if m:
                f = m.group(1)
                files.add(f if '/' in f else 'src/' + f)
        files = sorted(files)
    else:
        files = args
    for f in files:
        changed, manual = fix_file(pathlib.Path(f))
        if changed:
            print('rewrapped', f)
        for ln, why in manual:
            print('%s:%d: by hand: %s' % (f, ln, why))


if __name__ == '__main__':
    main()
