"""Read the ECOFF symbolic debug info (.mdebug) of a MIPS ELF and print it as C-like source evidence.

    python tools/ref/mdebug.py <elf> files [--grep REGEX]
    python tools/ref/mdebug.py <elf> raw   --file REGEX          # every local symbol / stab, raw
    python tools/ref/mdebug.py <elf> cfile --file REGEX [--out DIR]
    python tools/ref/mdebug.py <elf> type  --file REGEX NAME     # one struct/typedef, expanded

Used for EA Tiburon's PS2 builds made with ee-gcc (e.g. the Madden NFL 2003 prototype, see
docs/reference-builds/madden2003-ps2/). gcc writes its STABS debug info through mips-tfile into
.mdebug: each file descriptor (FDR) owns a run of local symbols whose strings are STABS strings
("name:type"), marked by storage class scInfo and index 0x8F300 + the stab code (N_FUN, N_LSYM...).
Native ECOFF symbols (stProc, stLocal...) are printed raw by `raw`.

`cfile` writes, per matching source file: every function with its return type, its parameters, and
its locals in declaration order with storage (register / stack offset / static) and block nesting,
followed by the full definitions of every struct / union / enum / typedef those functions use.

Layouts (little-endian, 32-bit MIPS "external" forms) are from binutils include/coff/sym.h and
include/coff/ecoff.h: HDRR 0x60 bytes, FDR 0x48, PDR 0x34, SYMR 0x0C, EXTR 0x10. The file offsets
in the HDRR are absolute file offsets for an executable. Only reads the ELF; writes only --out.
"""
import argparse, os, re, struct, sys

# ---------------------------------------------------------------------------------------------
# ELF / ECOFF containers

HDRR_FIELDS = ('magic vstamp ilineMax cbLine cbLineOffset idnMax cbDnOffset ipdMax cbPdOffset '
               'isymMax cbSymOffset ioptMax cbOptOffset iauxMax cbAuxOffset issMax cbSsOffset '
               'issExtMax cbSsExtOffset ifdMax cbFdOffset crfd cbRfdOffset iextMax cbExtOffset').split()

SC_NAMES = {0: 'Nil', 1: 'Text', 2: 'Data', 3: 'Bss', 4: 'Register', 5: 'Abs', 6: 'Undefined',
            8: 'Bits', 9: 'Dbx', 10: 'RegImage', 11: 'Info', 12: 'UserStruct', 13: 'SData',
            14: 'SBss', 15: 'RData', 16: 'Var', 17: 'Common', 18: 'SCommon', 19: 'VarRegister',
            21: 'FileDesc', 22: 'SUndefined', 23: 'Init', 24: 'BasedVar', 25: 'XData',
            26: 'PData', 27: 'Fini', 28: 'RConst'}
ST_NAMES = {0: 'Nil', 1: 'Global', 2: 'Static', 3: 'Param', 4: 'Local', 5: 'Label', 6: 'Proc',
            7: 'Block', 8: 'End', 9: 'Member', 10: 'Typedef', 11: 'File', 14: 'StaticProc',
            15: 'Constant', 26: 'Struct', 27: 'Union', 28: 'Enum', 34: 'Indirect'}
STAB_CODE_BASE = 0x8F300
STAB_NAMES = {0x20: 'GSYM', 0x22: 'FNAME', 0x24: 'FUN', 0x26: 'STSYM', 0x28: 'LCSYM',
              0x2e: 'BNSYM', 0x3c: 'OPT', 0x40: 'RSYM', 0x44: 'SLINE', 0x4e: 'ENSYM',
              0x60: 'SSYM', 0x64: 'SO', 0x80: 'LSYM', 0x82: 'BINCL', 0x84: 'SOL', 0xa0: 'PSYM',
              0xa2: 'EINCL', 0xc0: 'LBRAC', 0xc2: 'EXCL', 0xe0: 'RBRAC', 0xe2: 'BCOMM',
              0xe4: 'ECOMM', 0xfe: 'LENG'}
MIPS_REGS = ('zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 '
             't8 t9 k0 k1 gp sp fp ra').split()


def cstr(buf, off):
    end = buf.index(b'\0', off)
    return buf[off:end].decode('latin-1')


class Mdebug:
    def __init__(self, path):
        self.data = data = open(path, 'rb').read()
        assert data[:4] == b'\x7fELF' and data[5] == 1, 'expects a little-endian ELF'
        shoff, = struct.unpack_from('<I', data, 0x20)
        shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x2E)
        secs = [struct.unpack_from('<10I', data, shoff + i * shentsize) for i in range(shnum)]
        strtab = secs[shstrndx][4]
        md = [s for s in secs if cstr(data, strtab + s[0]) == '.mdebug']
        assert md, 'no .mdebug section'
        base = md[0][4]
        vals = struct.unpack_from('<2h23i', data, base)
        self.hdr = h = dict(zip(HDRR_FIELDS, vals))
        assert h['magic'] == 0x7009, 'bad HDRR magic %#x' % h['magic']
        self.fds = []
        for i in range(h['ifdMax']):
            o = h['cbFdOffset'] + i * 0x48
            (adr, rss, issBase, cbSs, isymBase, csym, ilineBase, cline, ioptBase, copt, ipdFirst,
             cpd, iauxBase, caux, rfdBase, crfd, bits, cbLineOffset, cbLine) = \
                struct.unpack_from('<10I2H4I4s2I', data, o)
            fd = dict(index=i, adr=adr, issBase=issBase, isymBase=isymBase, csym=csym,
                      ipdFirst=ipdFirst, cpd=cpd)
            fd['name'] = cstr(data, h['cbSsOffset'] + issBase + rss) if rss != 0xFFFFFFFF else ''
            fd['stabs'] = False
            self.fds.append(fd)
        for fd in self.fds:
            # gcc marks a stabs FDR with an "@stabs" symbol. The FDR is named after the compiler's
            # temp .i file; the real source path is the first N_SOL naming a .c/.cpp/.s file.
            first = self.syms(fd)[:2]
            fd['stabs'] = any(s['name'] == '@stabs' for s in first)
            fd['tmpname'] = fd['name']
            if fd['stabs']:
                for s in self.syms(fd):
                    if s['stab'] == 0x84 and re.search(r'\.(c|cpp|cc|s)$', s['name'], re.I):
                        fd['name'] = s['name']
                        break

    def syms(self, fd):
        h, data = self.hdr, self.data
        out = []
        for k in range(fd['csym']):
            o = h['cbSymOffset'] + (fd['isymBase'] + k) * 12
            iss, value, b0, b1, b2, b3 = struct.unpack_from('<iI4B', data, o)
            st = b0 & 0x3F
            sc = (b0 >> 6) | ((b1 & 7) << 2)
            index = (b1 >> 4) | (b2 << 4) | (b3 << 12)
            name = cstr(data, h['cbSsOffset'] + fd['issBase'] + iss) if iss >= 0 else ''
            # mips-tfile keeps a mapped st/sc on each stab; the code is only in the index field
            stab = index - STAB_CODE_BASE if fd['stabs'] and (index & 0xFFF00) == STAB_CODE_BASE else None
            out.append(dict(name=name, value=value, st=st, sc=sc, index=index, stab=stab))
        return out

    def externs(self):
        h, data = self.hdr, self.data
        out = []
        for k in range(h['iextMax']):
            o = h['cbExtOffset'] + k * 16
            _, ifd, iss, value, b0, b1, b2, b3 = struct.unpack_from('<Hhi I4B', data, o)
            name = cstr(data, h['cbSsExtOffset'] + iss)
            out.append(dict(ifd=ifd, name=name, value=value, st=b0 & 0x3F))
        return out


# ---------------------------------------------------------------------------------------------
# STABS type parser

class T:
    """A parsed stabs type. kind: base, ptr, func, array, struct, union, enum, xref, const,
    volatile, alias, unknown."""
    def __init__(self, kind, **kw):
        self.kind = kind
        self.typedef = None   # name given by a "name:t" stab
        self.tag = None       # tag given by a "name:T" stab (or the xref name)
        self.__dict__.update(kw)


class StabTypes:
    """Type table for one FDR. Type numbers are "(file,n)" or "n" (stored as (0, n)). A number
    used before its definition becomes an 'unknown' node that the later definition fills in."""
    def __init__(self, glob):
        self.tab = {}
        self.glob = glob  # {('tag'|'typedef', name): T} of this file, for xref lookups

    def parse(self, s, i):
        # returns (T, i)
        num, i = self.typenum(s, i)
        if i < len(s) and s[i] == '=':
            i += 1
            t = self.tab.get(num)
            if t is None or t.kind != 'unknown':  # a forward-used number is filled in place
                t = T('pending')
                self.tab[num] = t
            nt, i = self.typedef(s, i)
            t.__dict__.update(nt.__dict__)
            return t, i
        t = self.tab.get(num)
        if t is None:
            t = T('unknown', num=num)
            self.tab[num] = t
        return t, i

    def typenum(self, s, i):
        if s[i] == '(':
            j = s.index(')', i)
            a, b = s[i + 1:j].split(',')
            return (int(a), int(b)), j + 1
        m = re.compile(r'-?\d+').match(s, i)
        return (0, int(m.group())), m.end()

    def typedef(self, s, i):
        c = s[i]
        while c == '@':  # gcc attributes like @s64;
            i = s.index(';', i) + 1
            c = s[i]
        if c == '(' or c.isdigit() or c == '-':
            t, i = self.parse(s, i)
            return T('alias', of=t), i
        if c == 'r':
            of, i = self.parse(s, i + 1)
            j = s.index(';', i + 1); lo = s[i + 1:j]
            k = s.index(';', j + 1); hi = s[j + 1:k]
            return T('base', lo=lo, hi=hi, of=of), k + 1
        if c == '*':
            of, i = self.parse(s, i + 1)
            return T('ptr', of=of), i
        if c == '&':
            of, i = self.parse(s, i + 1)
            return T('ref', of=of), i
        if c == 'k':
            of, i = self.parse(s, i + 1)
            return T('const', of=of), i
        if c == 'B':
            of, i = self.parse(s, i + 1)
            return T('volatile', of=of), i
        if c == 'f':
            of, i = self.parse(s, i + 1)
            return T('func', of=of), i
        if c == 'a':
            assert s[i + 1] == 'r'
            _, i = self.parse(s, i + 2)
            j = s.index(';', i + 1); lo = int(s[i + 1:j])
            k = s.index(';', j + 1); hi = s[j + 1:k]
            of, i = self.parse(s, k + 1)
            n = int(hi) - lo + 1 if re.fullmatch(r'-?\d+', hi) else None
            return T('array', of=of, n=n), i
        if c in 'su':
            m = re.compile(r'\d+').match(s, i + 1)
            size, i = int(m.group()), m.end()
            fields = []
            while s[i] != ';':
                j = s.index(':', i)
                fname = s[i:j]
                ft, i = self.parse(s, j + 1)
                m = re.compile(r',(-?\d+),(-?\d+);').match(s, i)
                fields.append((fname, ft, int(m.group(1)), int(m.group(2))))
                i = m.end()
            return T('struct' if c == 's' else 'union', size=size, fields=fields), i + 1
        if c == 'e':
            i += 1
            vals = []
            while s[i] != ';':
                j = s.index(':', i)
                k = s.index(',', j)
                vals.append((s[i:j], int(s[j + 1:k])))
                i = k + 1
            return T('enum', vals=vals), i + 1
        if c == 'x':
            kind = {'s': 'struct', 'u': 'union', 'e': 'enum'}[s[i + 1]]
            j = s.index(':', i + 2)
            return T('xref', xkind=kind, tag=s[i + 2:j]), j + 1
        raise ValueError('stab type %r at %d in %r' % (c, i, s))


def resolve(t, glob, depth=0):
    """Follow xrefs / unknown numbers to a definition when one exists anywhere."""
    while t.kind in ('xref',) and depth < 20:
        d = glob.get(('tag', t.tag))
        if d is None or d is t:
            return t
        t, depth = d, depth + 1
    return t


def tname(t, glob, decl='', top=True):
    """C declaration of `decl` with type t (named types stay names)."""
    if t.typedef and not top or (t.typedef and top and t.kind != 'pending'):
        return join(t.typedef, decl)
    k = t.kind
    if k == 'alias':
        return tname(t.of, glob, decl, False)
    if k == 'base':
        return join(t.typedef or '<base %s..%s>' % (t.lo, t.hi), decl)
    if k == 'ptr':
        inner = '*' + decl
        if t.of.kind in ('func', 'array') and not t.of.typedef:
            inner = '(' + inner + ')'
        return tname(t.of, glob, inner, False)
    if k == 'const':
        return 'const ' + tname(t.of, glob, decl, False)
    if k == 'volatile':
        return 'volatile ' + tname(t.of, glob, decl, False)
    if k == 'func':
        return tname(t.of, glob, decl + '()', False)
    if k == 'array':
        return tname(t.of, glob, '%s[%s]' % (decl, t.n if t.n is not None else ''), False)
    if k in ('struct', 'union', 'enum'):
        if t.tag:
            return join('%s %s' % (k, t.tag), decl)
        return join('%s {...}' % k, decl)
    if k == 'xref':
        return join('%s %s' % (t.xkind, t.tag), decl)
    if k == 'unknown':
        return join('<type %s>' % (t.num,), decl)
    return join('<%s>' % k, decl)


def join(base, decl):
    return base + (' ' + decl if decl else '')


def body(t, glob, indent='    '):
    """Full definition of a struct / union / enum type (members with byte offsets)."""
    t = resolve(t, glob)
    if t.kind in ('struct', 'union'):
        lines = []
        for fname, ft, bitoff, bitsize in t.fields:
            d = tname(ft, glob, fname, False)
            bits = ''
            natural = type_size(ft, glob)
            if bitoff % 8 or (natural is not None and natural * 8 != bitsize and ft.kind != 'array'):
                bits = ' : %d' % bitsize
            lines.append('%s%s%s; // 0x%X%s' % (indent, d, bits, bitoff // 8,
                                                 ' bit %d' % (bitoff % 8) if bitoff % 8 else ''))
        return '{ // size 0x%X\n%s\n}' % (t.size, '\n'.join(lines))
    if t.kind == 'enum':
        return '{\n%s\n}' % '\n'.join('%s%s = %d,' % (indent, n, v) for n, v in t.vals)
    return None


def type_size(t, glob, depth=0):
    if depth > 30:
        return None
    t = resolve(t, glob)
    k = t.kind
    if k in ('alias', 'const', 'volatile'):
        return type_size(t.of, glob, depth + 1)
    if k in ('ptr', 'ref'):
        return 4
    if k in ('struct', 'union'):
        return t.size
    if k == 'enum':
        return 4
    if k == 'array':
        e = type_size(t.of, glob, depth + 1)
        return e * t.n if e is not None and t.n is not None else None
    if k == 'base':
        name = t.typedef or ''
        sizes = {'char': 1, 'signed char': 1, 'unsigned char': 1, 'short int': 2,
                 'short unsigned int': 2, 'int': 4, 'unsigned int': 4, 'long int': 4,
                 'long unsigned int': 4, 'float': 4, 'double': 8, 'long long int': 8,
                 'long long unsigned int': 8, 'long double': 8, 'void': 0}
        return sizes.get(name)
    return None


def named_deps(t, glob, seen, out, depth=0):
    """Collect the named struct / union / enum / typedef types reachable from t."""
    if t is None or id(t) in seen or depth > 40:
        return
    seen.add(id(t))
    r = resolve(t, glob)
    if r is not t:
        named_deps(r, glob, seen, out, depth + 1)
    if t.typedef or (t.kind in ('struct', 'union', 'enum') and t.tag):
        out.append(t)
    for attr in ('of',):
        if hasattr(t, attr) and t.kind != 'base':
            named_deps(getattr(t, attr), glob, seen, out, depth + 1)
    if t.kind in ('struct', 'union'):
        for _, ft, _, _ in t.fields:
            named_deps(ft, glob, seen, out, depth + 1)


# ---------------------------------------------------------------------------------------------
# One source file

class SourceFile:
    """Walk one FDR's stabs: types, functions (N_FUN), params, locals, blocks, statics."""
    def __init__(self, md, fd, glob):
        self.fd = fd
        self.types = StabTypes(glob)
        self.glob = glob
        self.funcs = []
        self.statics = []
        self.globals = []
        self.headers = []
        self.anon_enums = []
        cur = None
        depth = 0
        pending = []
        lines = {}
        want_line = None
        cont = ''
        for s in md.syms(fd):
            code = s['stab']
            name = STAB_NAMES.get(code, hex(code)) if code is not None else None
            text = s['name']
            if code is not None:
                # a long stab is split into pieces that end in a backslash
                if text.endswith('\\') and name not in ('SO', 'SOL'):
                    cont += text[:-1]
                    continue
                text, cont = cont + text, ''
            if name in ('BINCL', 'SOL', 'EXCL', 'SO'):
                self.headers.append((name, text))
                continue
            if code is None:
                # native symbols: stProc starts a function; the next line label's index is the
                # source line of its first statement
                if s['st'] == 6 or s['st'] == 14:
                    want_line = s['name']
                elif s['st'] == 5 and want_line and s['name'].startswith('$LM'):
                    lines[want_line] = s['index']
                    want_line = None
                continue
            # gcc 2 writes a block's variables BEFORE its N_LBRAC: the locals pending since the
            # last bracket belong to the block the next N_LBRAC opens.
            if name == 'LBRAC':
                depth += 1
                if cur:
                    for it in pending:
                        it[1] = depth
                    cur['items'].append(['{', depth, s['value']])
                    cur['items'].extend(pending)
                pending = []
                continue
            if name == 'RBRAC':
                if cur:
                    cur['items'].extend(pending)
                    cur['items'].append(['}', depth, s['value']])
                pending = []
                depth -= 1
                continue
            if ':' not in text or name == 'SLINE':
                if name == 'FUN' and cur is not None and text == '':
                    cur['size'] = s['value']
                    cur['items'].extend(pending)
                    pending = []
                continue
            sym, rest = text.split(':', 1)
            try:
                self.one(name, sym, rest, s, cur, depth, pending, glob)
            except Exception as e:
                raise ValueError('%s: %s (%s)' % (fd['name'], text, e))
            if name == 'FUN':
                cur = self.funcs[-1]
                cur['line'] = lines.get(cur['name'])
                depth = 0
                pending = []

    def one(self, name, sym, rest, s, cur, depth, pending, glob):
        """Handle one "sym:rest" stab. Locals go to `pending` (see the N_LBRAC note above)."""
        if rest[:1] in ('t', 'T'):
            is_tag = rest[0] == 'T'
            j = 2 if rest[1:2] == 't' else 1  # "Tt": tag and typedef together
            t, _ = self.types.parse(rest, j)
            if is_tag:
                t.tag = sym
                glob.setdefault(('tag', sym), t)
                if t.kind == 'enum' and not sym.strip():
                    self.anon_enums.append(t)
                if j == 2:
                    t.typedef = sym
            elif sym and sym != ' ':
                if t.typedef is None:
                    t.typedef = sym
                else:  # a typedef of an already named type: keep an alias node
                    a = T('alias', of=t)
                    a.typedef = sym
                    self.types.tab[('typedef', sym)] = a
                    t = a
                glob.setdefault(('typedef', sym), t)
            return
        if name == 'FUN':
            t, _ = self.types.parse(rest, 1)
            self.funcs.append(dict(name=sym, ret=t, static=rest[0] == 'f', addr=s['value'],
                                   params=[], items=[], size=None, line=None))
            return
        cls = rest[0]
        if cls.isalpha():
            t, _ = self.types.parse(rest, 1)
        else:
            t, _ = self.types.parse(rest, 0)
            cls = ''
        if name == 'PSYM' or cls == 'p':
            cur['params'].append(dict(name=sym, t=t, where='stack %+d' % to_signed(s['value'])))
        elif name == 'RSYM' and cls == 'P':
            cur['params'].append(dict(name=sym, t=t, where='reg ' + reg(s['value'])))
        elif name == 'RSYM':
            if cur is not None and depth == 0 and any(p['name'] == sym for p in cur['params']) \
                    and not any(it[2]['name'] == sym for it in pending if it[0] == 'local'):
                for p in cur['params']:  # a stack parameter that lives in a register
                    if p['name'] == sym:
                        p['where'] += ', lives in ' + reg(s['value'])
            else:
                pending.append(['local', depth + 1, dict(name=sym, t=t, where='reg ' + reg(s['value']))])
        elif name == 'LSYM' and cur is not None and cls == '':
            pending.append(['local', depth + 1, dict(name=sym, t=t,
                                                     where='stack %+d' % to_signed(s['value']))])
        elif name in ('STSYM', 'LCSYM') and cls == 'V' and cur is not None:
            pending.append(['local', depth + 1, dict(name=sym, t=t, where='static')])
        elif name in ('STSYM', 'LCSYM'):
            self.statics.append(dict(name=sym, t=t, cls=cls, addr=s['value'],
                                     sect='data' if name == 'STSYM' else 'bss'))
        elif name == 'GSYM':
            self.globals.append(dict(name=sym, t=t))


def to_signed(v):
    return v - (1 << 32) if v & 0x80000000 else v


def reg(n):
    if n < 32:
        return MIPS_REGS[n]
    if 32 <= n < 64:
        return 'f%d' % (n - 32)
    if n == 0xFFFFFFFF:
        return '(none: optimized out)'
    return 'r%d' % n


def load(md, fds):
    """Parse the stabs of each FDR. gcc compiled a preprocessed .i, so every FDR carries all of its
    own types (no N_EXCL); each file gets its own tag / typedef table."""
    files = []
    for fd in fds:
        files.append(SourceFile(md, fd, {}))
    return files


def render_file(sf, md, extra=None):
    """The file as C-like text. `extra`: a regex; file types whose name matches are also written
    even when no function names them (e.g. an anonymous enum of opcodes)."""
    g = sf.glob
    out = ['// %s' % sf.fd['name'], '// Derived from STABS in .mdebug (FDR %d); functions in source order.'
           % sf.fd['index'], '']
    deps = []
    seen = set()
    if sf.statics or sf.globals:
        out.append('// File-scope data')
        for v in sf.globals:
            out.append('%s;' % tname(v['t'], g, v['name'], False))
            named_deps(v['t'], g, seen, deps)
        for v in sf.statics:
            out.append('static %s; // %s 0x%08X' % (tname(v['t'], g, v['name'], False),
                                                    v['sect'], v['addr']))
            named_deps(v['t'], g, seen, deps)
        out.append('')
    for f in sf.funcs:
        params = ', '.join(tname(p['t'], g, p['name'], False) for p in f['params']) or 'void'
        out.append('%s%s // 0x%08X, 0x%X bytes%s' % (
            'static ' if f['static'] else '',
            tname(f['ret'], g, '%s(%s)' % (f['name'], params), False), f['addr'],
            f['size'] or 0, ', first line %d' % f['line'] if f['line'] else ''))
        named_deps(f['ret'], g, seen, deps)
        for p in f['params']:
            out.append('    // param %-20s %s' % (p['name'], p['where']))
            named_deps(p['t'], g, seen, deps)
        out.append('{')
        for kind, depth, v in f['items']:
            ind = '    ' * max(depth, 1)
            if kind == '{':
                if depth > 1:
                    out.append('%s{' % ind[4:])
            elif kind == '}':
                if depth > 1:
                    out.append('%s}' % ind[4:])
            else:
                out.append('%s%s; // %s' % (ind, tname(v['t'], g, v['name'], False), v['where']))
                named_deps(v['t'], g, seen, deps)
        out.append('}')
        out.append('')
    if extra:
        # also every type / anonymous enum of the file whose name (or first value) matches
        for (kind, nm), t in list(g.items()):
            if re.search(extra, nm.strip()):
                named_deps(t, g, seen, deps)
        for t in sf.anon_enums:
            if t.vals and re.search(extra, t.vals[0][0]):
                t.tag = None
                deps.append(t)
    out.append('// ---- Types used above (full definitions) ----')
    done = set()
    anon = lambda o: o.kind in ('struct', 'union', 'enum') and not (o.tag or '').strip() and not o.typedef
    # anonymous types that a typedef names are written inside that typedef
    inl = {id(resolve(t.of, g)) for t in deps
           if t.typedef and t.kind == 'alias' and anon(resolve(t.of, g))}
    for t in deps:
        r = resolve(t, g)
        if t.kind == 'base' or (t.kind == 'alias' and t.of is t):
            continue  # C's own base types (int, char, void...)
        if not t.typedef and id(r) in inl:
            continue
        key = (t.typedef, t.tag, id(r))
        if key in done:
            continue
        done.add(key)
        if t.typedef and t.kind == 'alias':
            o = resolve(t.of, g)
            if anon(o):
                out.append('typedef %s %s %s;' % (o.kind, body(o, g), t.typedef))
            else:
                out.append('typedef %s;' % tname(t.of, g, t.typedef, False))
            out.append('')
            continue
        b = body(r, g)
        if t.typedef and r.kind in ('struct', 'union', 'enum'):
            head = '%s %s' % (r.kind, r.tag) if r.tag else r.kind
            out.append('typedef %s %s %s;' % (head, b or '', t.typedef) if b else 'typedef %s %s;' % (head, t.typedef))
        elif t.typedef:
            saved, t.typedef = t.typedef, None
            out.append('typedef %s;' % tname(t, g, saved, False))
            t.typedef = saved
        elif b:
            out.append('%s %s%s;' % (r.kind, r.tag + ' ' if r.tag else '', b))
        out.append('')
    return '\n'.join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('elf')
    ap.add_argument('cmd', choices=['files', 'raw', 'cfile', 'type', 'externs'])
    ap.add_argument('name', nargs='?')
    ap.add_argument('--grep')
    ap.add_argument('--file')
    ap.add_argument('--out')
    ap.add_argument('--types', help='cfile: also write the file types whose names match (regex)')
    a = ap.parse_intermixed_args()
    md = Mdebug(a.elf)
    if a.cmd == 'files':
        for fd in md.fds:
            if not a.grep or re.search(a.grep, fd['name']):
                print('%4d 0x%08X %5d %s' % (fd['index'], fd['adr'], fd['csym'], fd['name']))
        return
    if a.cmd == 'externs':
        for e in md.externs():
            if not a.grep or re.search(a.grep, e['name']):
                fdn = md.fds[e['ifd']]['name'] if 0 <= e['ifd'] < len(md.fds) else '-'
                print('0x%08X %-10s %s  (%s)' % (e['value'], ST_NAMES.get(e['st'], e['st']), e['name'], fdn))
        return
    sel = [fd for fd in md.fds if re.search(a.file or '.', fd['name'])]
    if a.cmd == 'raw':
        for fd in sel:
            print('== FDR %d %s' % (fd['index'], fd['name']))
            for s in md.syms(fd):
                kind = 'N_' + STAB_NAMES.get(s['stab'], hex(s['stab'])) if s['stab'] is not None else \
                    'st%s/sc%s/idx%d' % (ST_NAMES.get(s['st'], s['st']), SC_NAMES.get(s['sc'], s['sc']), s['index'])
                print('  %-10s %08X %s' % (kind, s['value'], s['name']))
        return
    files = load(md, sel)
    by_index = {sf.fd['index']: sf for sf in files}
    if a.cmd == 'type':
        glob = files[0].glob
        for key in (('typedef', a.name), ('tag', a.name)):
            if key in glob:
                t = glob[key]
                while t.kind in ('alias', 'xref', 'const', 'volatile') and t.of is not t:
                    print('%s: %s' % (key[1], tname(t.of, glob, '', False) if t.kind != 'xref' else t.tag))
                    t = resolve(t.of if t.kind != 'xref' else t, glob)
                    if t.kind == 'xref':
                        break
                print('%s %s %s' % (t.kind, (t.tag or '').strip(), body(t, glob) or tname(t, glob, '', False)))
        return
    for fd in sel:
        sf = by_index.get(fd['index'])
        if sf is None:
            continue
        text = render_file(sf, md, a.types)
        if a.out:
            os.makedirs(a.out, exist_ok=True)
            base = re.split(r'[\\/]', fd['name'])[-1]
            with open(os.path.join(a.out, base + '.txt'), 'w') as f:
                f.write(text + '\n')
        else:
            print(text)


if __name__ == '__main__':
    main()
