"""m2c-backed translation for the sweep: run m2c on whole asm files once, cache the per-function
output, and clean it into plain C (no m2c macros) for sweep.py.

    translate_m2c(f) -> (C text, [declaration lines]) or None
"""
import os, re, subprocess, json, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from paths import M2C as _M2C, WORK  # noqa: E402

M2C = _M2C
CACHE = WORK + '/m2c_cache.json'
_cache = json.load(open(CACHE)) if os.path.exists(CACHE) else {}


def _run_file(path, names):
    """m2c output for several functions of one asm file, split per function."""
    out = {}
    for i in range(0, len(names), 40):
        chunk = names[i:i + 40]
        args = [sys.executable, M2C, '-t', 'ppc-mwcc-c', '--valid-syntax', '--pointer-style', 'left', '--globals', 'used']
        for n in chunk:
            args += ['-f', n]
        r = subprocess.run(args + [path], capture_output=True, text=True)
        text = r.stdout
        # split on function definitions at column 0
        for n in chunk:
            m = re.search(r'^[^\n;{}]*\b%s\([^;{}]*\)\s*\{' % re.escape(n), text, re.M)
            if not m:
                out[n] = None
                continue
            depth, j = 0, m.end() - 1
            while j < len(text):
                if text[j] == '{':
                    depth += 1
                elif text[j] == '}':
                    depth -= 1
                    if depth == 0:
                        break
                j += 1
            body = text[m.start():j + 1]
            # declarations m2c printed (before the first function) that this function mentions
            head = text[:re.search(r'^[^\n;{}]*\w+\([^;{}]*\)\s*\{', text, re.M).start()] if re.search(
                r'^[^\n;{}]*\w+\([^;{}]*\)\s*\{', text, re.M) else ''
            decls = [l for l in head.splitlines() if l.strip() and re.search(r'\b(\w+)\b', l)]
            used = [l for l in decls if any(re.search(r'\b%s\b' % re.escape(w), body)
                                            for w in re.findall(r'\b((?:fn|lbl)_[0-9A-F]{8}|[A-Za-z_]\w*)\s*(?:\(|;|\[)', l))]
            out[n] = {'body': body, 'decls': used}
    return out


def prefetch(fns):
    """Run m2c for all given functions not in the cache, grouped by asm file."""
    byfile = {}
    for f in fns:
        if f['name'] not in _cache:
            byfile.setdefault(f['file'], []).append(f['name'])
    for path, names in byfile.items():
        _cache.update(_run_file(path, names))
    json.dump(_cache, open(CACHE, 'w'))


def _expand_field(s):
    """M2C_FIELD(expr, T*, off) -> (*(T*)((s8*)(expr) + off)), innermost first."""
    while True:
        k = s.rfind('M2C_FIELD(')
        if k < 0:
            return s
        j, depth, parts, start = k + len('M2C_FIELD('), 1, [], k + len('M2C_FIELD(')
        while depth:
            c = s[j]
            if c == '(':
                depth += 1
            elif c == ')':
                depth -= 1
            elif c == ',' and depth == 1:
                parts.append(s[start:j]); start = j + 1
            j += 1
        parts.append(s[start:j - 1])
        expr, typ, off = [p.strip() for p in parts]
        s = s[:k] + '(*(%s)((u8*)(%s) + %s))' % (typ, expr, off) + s[j:]


def clean(entry):
    """m2c output -> (body, decls) in plain C, or None if it uses anything we do not accept."""
    if not entry:
        return None
    body, decls = entry['body'], list(entry['decls'])
    text = body + '\n' + '\n'.join(decls)
    if re.search(r'M2C_(?!FIELD\b|UNK\b|BITWISE\b)\w+', text):
        return None
    # m2c's "unknown type" is a bare '?' where a type goes (a ternary's '?' sits between expressions)
    if re.search(r'(^\s*|[(,]\s*)\?[\s*,)]', text, re.M):
        return None
    if 'saved_reg' in text or re.search(r'\bsp[0-9A-Fa-f]+\b', text):
        return None

    def fix(s):
        s = _expand_field(s)
        s = s.replace('M2C_UNK', 's32')
        s = re.sub(r'M2C_BITWISE\(([^,]+),', r'((\1)', s)
        s = re.sub(r'\s*/\* (extern|static|irregular|inferred)\s*\*/', '', s)
        return s

    body = fix(body)
    decls = [fix(d).rstrip() for d in decls]
    return body, decls
