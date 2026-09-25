"""Small-function sweep: write C for mechanical shapes, as new unsorted/ units.
    python sweep.py gen [maxbytes] [minrun]   -> writes src/unsorted/code_*.c (NonMatching), splits, configure
    python sweep.py keep                      -> after a build: keep only exact runs, mark them Matching
State is kept in build/sweep/ (tools/sweep/paths.py)."""
import json, os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from paths import ROOT, HERE, WORK, MWCC_CMD  # noqa: E402
from smallsurvey import parse
import sweep_m2c

ASM = ROOT + '/build/GW4E69/asm/'
STATE = WORK + '/sweep_state.json'
SKIP = WORK + '/sweep_skip.json'   # functions whose generated C was not exact
MARK = '# --- small-function sweep units (generated) ---'

# symbol -> section, from symbols.txt
SECT = {}
for line in open(ROOT + '/config/GW4E69/symbols.txt', encoding='utf-8'):
    m = re.match(r'^(\S+) = (\.\w+):0x([0-9A-Fa-f]+);', line)
    if m:
        SECT[m.group(1)] = m.group(2)

LOAD = {'lwz': 's32', 'lbz': 'u8', 'lhz': 'u16', 'lha': 's16', 'lfs': 'f32'}
STORE = {'stw': 's32', 'stb': 'u8', 'sth': 'u16', 'stfs': 'f32'}
FRAME_HEAD = ['stwu r1, -0x10(r1)', 'mflr r0']
FRAME_TAIL = ['lwz r0, 0x14(r1)', 'mtlr r0', 'addi r1, r1, 0x10', 'blr']


def num(s):
    return int(s, 16) if s.lower().startswith(('0x', '-0x')) else int(s)


def sym_ok(s):
    """Data symbols we can reference as externs (not literal pools)."""
    return SECT.get(s) in ('.sdata', '.sbss', '.data', '.bss')


def translate(f, ext, calls):
    """Return (C text, signature) or None. ext: {sym: ctype} used by the unit; calls: {fn: rettype}."""
    ins = [i for i in f['ins'] if i != 'LABEL']
    name = f['name']
    t = ' ; '.join(ins)

    def need(sym, ty):
        if not sym_ok(sym):
            return False
        if ext.get(sym, ty) != ty:
            return False
        ext[sym] = ty
        return True

    if t == 'blr':
        return 'void %s(void) {\n}\n' % name
    m = re.fullmatch(r'li r3, (-?(?:0x)?[0-9a-f]+) ; blr', t)
    if m:
        return 's32 %s(void) {\n    return %d;\n}\n' % (name, num(m.group(1)))
    m = re.fullmatch(r'(lwz|lbz|lhz|lha) r3, (\w+)@sda21\(r0\) ; blr', t) or \
        re.fullmatch(r'(lfs) f1, (\w+)@sda21\(r0\) ; blr', t)
    if m and need(m.group(2), LOAD[m.group(1)]):
        return '%s %s(void) {\n    return %s;\n}\n' % (LOAD[m.group(1)], name, m.group(2))
    m = re.fullmatch(r'(stw|stb|sth) r3, (\w+)@sda21\(r0\) ; blr', t) or \
        re.fullmatch(r'(stfs) f1, (\w+)@sda21\(r0\) ; blr', t)
    if m and need(m.group(2), STORE[m.group(1)]):
        ty = STORE[m.group(1)]
        return 'void %s(%s v) {\n    %s = v;\n}\n' % (name, ty, m.group(2))
    m = re.fullmatch(r'li r0, (-?(?:0x)?[0-9a-f]+) ; (stw|stb|sth) r0, (\w+)@sda21\(r0\) ; blr', t)
    if m and need(m.group(3), STORE[m.group(2)]):
        return 'void %s(void) {\n    %s = %d;\n}\n' % (name, m.group(3), num(m.group(1)))
    m = re.fullmatch(r'(lwz|lbz|lhz|lha) r3, (-?(?:0x)?[0-9a-f]+)\(r3\) ; blr', t) or \
        re.fullmatch(r'(lfs) f1, (-?(?:0x)?[0-9a-f]+)\(r3\) ; blr', t)
    if m:
        ty = LOAD[m.group(1)]
        return '%s %s(u8* p) {\n    return *(%s*)(p + 0x%X);\n}\n' % (ty, name, ty, num(m.group(2)))
    m = re.fullmatch(r'(stw|stb|sth) r4, (-?(?:0x)?[0-9a-f]+)\(r3\) ; blr', t)
    if m:
        ty = STORE[m.group(1)]
        return 'void %s(u8* p, %s v) {\n    *(%s*)(p + 0x%X) = v;\n}\n' % (name, ty, ty, num(m.group(2)))
    m = re.fullmatch(r'stfs f1, (-?(?:0x)?[0-9a-f]+)\(r3\) ; blr', t)
    if m:
        return 'void %s(u8* p, f32 v) {\n    *(f32*)(p + 0x%X) = v;\n}\n' % (name, num(m.group(1)))
    m = re.fullmatch(r'li r0, (-?(?:0x)?[0-9a-f]+) ; (stw|stb|sth) r0, (-?(?:0x)?[0-9a-f]+)\(r(3|4)\) ; blr', t)
    if m:
        ty = STORE[m.group(2)]
        if m.group(4) == '3':
            return 'void %s(u8* p) {\n    *(%s*)(p + 0x%X) = %d;\n}\n' % (name, ty, num(m.group(3)), num(m.group(1)))
        return 'void %s(s32 a, u8* p) {\n    *(%s*)(p + 0x%X) = %d;\n}\n' % (name, ty, num(m.group(3)), num(m.group(1)))
    m = re.fullmatch(r'lwz r3, (\w+)@sda21\(r0\) ; (lwz|lbz|lhz|lha) r3, (-?(?:0x)?[0-9a-f]+)\(r3\) ; blr', t)
    if m and need(m.group(1), 'u8*'):
        ty = LOAD[m.group(2)]
        return '%s %s(void) {\n    return *(%s*)(%s + 0x%X);\n}\n' % (ty, name, ty, m.group(1), num(m.group(3)))
    m = re.fullmatch(r'lwz r4, (\w+)@sda21\(r0\) ; (stw|stb|sth) r3, (-?(?:0x)?[0-9a-f]+)\(r4\) ; blr', t)
    if m and need(m.group(1), 'u8*'):
        ty = STORE[m.group(2)]
        return 'void %s(%s v) {\n    *(%s*)(%s + 0x%X) = v;\n}\n' % (name, ty, ty, m.group(1), num(m.group(3)))
    m = re.fullmatch(r'lis r3, (\w+)@ha ; addi r3, r3, (\w+)@l ; blr', t)
    if m and m.group(1) == m.group(2) and need(m.group(1), 'u8[]'):
        return 'u8* %s(void) {\n    return %s;\n}\n' % (name, m.group(1))
    # wrappers with a 0x10 frame: calls only, arguments passed through untouched
    if ins[:2] == FRAME_HEAD and ins[-4:] == FRAME_TAIL and len(ins) >= 7:
        mid = ins[2:-4]
        if mid[0] == 'stw r0, 0x14(r1)' and all(re.fullmatch(r'bl \w+', i) for i in mid[1:]) and len(mid) <= 4:
            tgts = [i.split()[1] for i in mid[1:]]
            if all(calls.get(g, 'void') == 'void' for g in tgts) and name not in tgts:
                for g in tgts:
                    calls[g] = 'void'
                return 'void %s(void) {\n%s}\n' % (name, ''.join('    %s();\n' % g for g in tgts))
        m2 = re.fullmatch(r'li r3, (-?(?:0x)?[0-9a-f]+)', mid[0])
        if m2 and len(mid) == 3 and mid[1] == 'stw r0, 0x14(r1)' and re.fullmatch(r'bl \w+', mid[2]):
            g = mid[2].split()[1]
            if calls.get(g, 'void') == 'void' and g != name:
                calls[g] = 'void'
                return 'void %s(void) {\n    %s(%d);\n}\n' % (name, g, num(m2.group(1)))
        # pass a field of the first argument: lwz r3, N(r3) ; bl T
        if len(mid) == 3 and mid[0] == 'stw r0, 0x14(r1)':
            m3 = re.fullmatch(r'(lwz|lbz|lhz|lha) r3, (-?(?:0x)?[0-9a-f]+)\(r3\)', mid[1])
            m4 = re.fullmatch(r'bl (\w+)', mid[2])
            if m3 and m4 and calls.get(m4.group(1), 'void') == 'void' and m4.group(1) != name:
                g, ty = m4.group(1), LOAD[m3.group(1)]
                calls[g] = 'void'
                return 'void %s(u8* p) {\n    %s(*(%s*)(p + 0x%X));\n}\n' % (name, g, ty, num(m3.group(2)))
            # pass a global: lwz r3, SYM@sda21(r0) ; bl T
            m3 = re.fullmatch(r'(lwz|lbz|lhz|lha) r3, (\w+)@sda21\(r0\)', mid[1])
            if m3 and m4 and calls.get(m4.group(1), 'void') == 'void' and m4.group(1) != name \
                    and need(m3.group(2), LOAD[m3.group(1)]):
                g = m4.group(1)
                calls[g] = 'void'
                return 'void %s(void) {\n    %s(%s);\n}\n' % (name, g, m3.group(2))
        # float wrapper around a double function: bl T ; lwz r0 ; frsp f1, f1
        if ins[:3] == ['stwu r1, -0x10(r1)', 'mflr r0', 'stw r0, 0x14(r1)'] and len(ins) == 9 \
                and re.fullmatch(r'bl \w+', ins[3]) and ins[4:] == ['lwz r0, 0x14(r1)', 'frsp f1, f1', 'mtlr r0',
                                                                  'addi r1, r1, 0x10', 'blr']:
            g = ins[3].split()[1]
            if calls.get(g, 'double') == 'double' and g != name:
                calls[g] = 'double'
                return 'f32 %s(f32 x) {\n    return %s(x);\n}\n' % (name, g)
    return None


def taken_ranges():
    sp = open(ROOT + '/config/GW4E69/splits.txt', encoding='utf-8').read()
    return [(int(a, 16), int(b, 16)) for a, b in re.findall(r'\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)', sp)]


def translate_call(f, ext, calls):
    """General one-call wrapper: argument registers built from parameters, constants, globals and
    fields, then one call (bl with a 0x10 frame, or a tail b). Returns C or None."""
    ins = [i for i in f['ins'] if i != 'LABEL']
    name = f['name']
    if ins[:2] == FRAME_HEAD:
        body, tail = ins[2:], None
        if body[-4:] == FRAME_TAIL:
            body, ret = body[:-4], 'void'
        elif body[-5:] == ['lwz r0, 0x14(r1)', 'frsp f1, f1', 'mtlr r0', 'addi r1, r1, 0x10', 'blr']:
            body, ret = body[:-5], 'f32'
        else:
            return None
        body = [i for i in body if i != 'stw r0, 0x14(r1)']
        calls_at = [k for k, i in enumerate(body) if i.startswith('bl ')]
        if len(calls_at) != 1 or calls_at[0] != len(body) - 1:
            return None
    elif ins and ins[-1].startswith('b ') and not any(i.startswith(('bl ', 'stwu', 'mflr')) for i in ins):
        body, ret = ins, 'void'
    else:
        return None
    target = body[-1].split()[1]
    if target == name or not re.fullmatch(r'\w+', target):
        return None
    reg = {'r%d' % i: ('param', i) for i in range(3, 11)}
    used_params, ptr_params = set(), set()

    def val(r):
        v = reg.get(r)
        if v is None:
            raise ValueError
        return v

    try:
        for i in body[:-1]:
            m = re.fullmatch(r'li (r\d+), (-?(?:0x)?[0-9a-f]+)', i)
            if m:
                reg[m.group(1)] = ('const', num(m.group(2))); continue
            m = re.fullmatch(r'lis (r\d+), (-?(?:0x)?[0-9a-f]+)', i)
            if m:
                reg[m.group(1)] = ('const', (num(m.group(2)) << 16) & 0xFFFFFFFF); continue
            m = re.fullmatch(r'addi (r\d+), (r\d+), (-?(?:0x)?[0-9a-f]+)', i)
            if m:
                v = val(m.group(2))
                if v[0] == 'const':
                    c = (v[1] + num(m.group(3))) & 0xFFFFFFFF
                    reg[m.group(1)] = ('const', c - (1 << 32) if c & 0x80000000 else c)
                elif v[0] == 'param':
                    ptr_params.add(v[1]); used_params.add(v[1])
                    reg[m.group(1)] = ('expr', '(p%d + 0x%X)' % (v[1] - 3, num(m.group(3))))
                else:
                    return None
                continue
            m = re.fullmatch(r'mr (r\d+), (r\d+)', i)
            if m:
                reg[m.group(1)] = val(m.group(2)); continue
            m = re.fullmatch(r'(lwz|lbz|lhz|lha) (r\d+), (-?(?:0x)?[0-9a-f]+)\((r\d+)\)', i)
            if m and m.group(4) not in ('r0', 'r1'):
                v = val(m.group(4))
                if v[0] != 'param':
                    return None
                ptr_params.add(v[1]); used_params.add(v[1])
                reg[m.group(2)] = ('expr', '*(%s*)(p%d + 0x%X)' % (LOAD[m.group(1)], v[1] - 3, num(m.group(3))))
                continue
            m = re.fullmatch(r'(lwz|lbz|lhz|lha) (r\d+), (\w+)@sda21\(r0\)', i)
            if m:
                ty = LOAD[m.group(1)]
                if not sym_ok(m.group(3)) or ext.get(m.group(3), ty) != ty:
                    return None
                ext[m.group(3)] = ty
                reg[m.group(2)] = ('expr', m.group(3)); continue
            return None
    except ValueError:
        return None
    # arguments: r3 .. the highest register set in the body
    written = [int(r[1:]) for r, v in reg.items() if v != ('param', int(r[1:]))]
    nargs = max(written) - 2 if written else 0
    args = []
    for k in range(3, 3 + nargs):
        v = reg['r%d' % k]
        if v[0] == 'param':
            used_params.add(v[1]); args.append('p%d' % (v[1] - 3))
        elif v[0] == 'const':
            args.append('%d' % v[1])
        else:
            args.append(v[1])
    if calls.get(target, ret if ret == 'f32' else 'void') not in ('void', 'double'):
        return None
    ctype = 'double' if ret == 'f32' else 'void'
    if calls.get(target, ctype) != ctype:
        return None
    calls[target] = ctype
    np = max(used_params) - 2 if used_params else 0
    params = ', '.join(('u8* p%d' if k + 3 in ptr_params else 's32 p%d') % k for k in range(np)) or 'void'
    if ret == 'f32':
        if nargs or np:
            return None
        return 'f32 %s(f32 x) {\n    return %s(x);\n}\n' % (name, target)
    return 'void %s(%s) {\n    %s(%s);\n}\n' % (name, params, target, ', '.join(args))


IMM = r'(-?(?:0x)?[0-9a-f]+)'


def translate_leaf(f, ext, calls):
    """Straight-line leaf functions (no branches, no calls): loads, stores, simple integer and float
    arithmetic, return value in r3 or f1. Values are tracked as C expressions."""
    ins = [i for i in f['ins'] if i != 'LABEL']
    name = f['name']
    if not ins or ins[-1] != 'blr':
        return None
    ins = strip_frame(ins)
    if ins is None or any(i.split()[0].startswith('b') and not i.startswith('bl ') for i in ins[:-1]):
        return None
    # value: (expr, kind, gen) kind: int, ptr, flt, param, fparam, callr, callf, dead;
    # gen = the store/call count when a memory load in it was made (None: no load)
    reg = {('r%d' % i): ('p%d' % (i - 3), 'param', None) for i in range(3, 11)}
    reg.update({('f%d' % i): ('x%d' % (i - 1), 'fparam', None) for i in range(1, 9)})
    stmts, stores = [], [0]
    callrec = []        # [target, int args, float args, uses set]
    snap_at = []        # registers just before each call
    snap = dict(reg)
    stored = set()      # values written to memory (a register still holding one is not a result)
    iparams, fparams, ptrp = set(), set(), set()
    mem_since = {}      # expr -> store count when a load was made (for aliasing checks)

    def get(r):
        v = reg.get(r)
        if v is None or v[1] == 'dead':
            raise ValueError(r)
        if v[1] == 'param':
            iparams.add(int(v[0][1:]))
        if v[1] == 'fparam':
            fparams.add(int(v[0][1:]))
        if v[1] == 'callr':
            callrec[int(v[0][1:])][3].add('i')
        if v[1] == 'callf':
            callrec[int(v[0][1:])][3].add('f')
        return v

    def fresh(v):
        # a value that contains a load made before a later store cannot be used after that store
        if v[2] is not None and v[2] != stores[0]:
            raise ValueError('alias')
        return v

    def base(r):
        v = fresh(get(r))
        if v[1] == 'param':
            ptrp.add(int(v[0][1:]))
            return v[0]
        if v[1] == 'ptr':
            return v[0]
        if v[1] in ('int', 'callr'):
            return '((u8*)%s)' % v[0]
        raise ValueError('base')

    def ival(r):
        v = fresh(get(r))
        if v[1] in ('fparam', 'flt', 'callf'):
            raise ValueError('kind')
        return v[0]

    def fval(r):
        v = fresh(get(r))
        if v[1] not in ('fparam', 'flt', 'callf'):
            raise ValueError('kind')
        return v[0]

    def haspay(*vs):
        # the load-generation of a combined value: the oldest load in it
        gens = [v[2] for v in vs if v[2] is not None]
        return min(gens) if gens else None

    hi = {}     # register -> symbol from lis rX, SYM@ha
    try:
        for i in ins[:-1]:
            op, _, rest = i.partition(' ')
            a = [x.strip() for x in rest.split(',')] if rest else []
            m = None
            if op == 'bl':
                tgt = a[0]
                if tgt == name or not re.fullmatch(r'\w+', tgt):
                    return None
                iw = [k for k in range(3, 11) if reg['r%d' % k] is not snap['r%d' % k]]
                fw = [k for k in range(1, 9) if reg['f%d' % k] is not snap['f%d' % k]]
                iargs, fargs = [], []
                for k in range(3, (max(iw) + 1) if iw else 3):
                    v = get('r%d' % k)
                    iargs.append(base('r%d' % k) if v[1] == 'ptr' else ival('r%d' % k))
                for k in range(1, (max(fw) + 1) if fw else 1):
                    fargs.append(fval('f%d' % k))
                k = len(callrec)
                snap_at.append(dict(reg))
                callrec.append([tgt, iargs, fargs, set()])
                stmts.append(('CALL', k))
                stores[0] += 1
                for q in ['r0'] + ['r%d' % j for j in range(3, 13)]:
                    reg[q] = ('', 'dead', None)
                for q in ['f%d' % j for j in range(0, 14)]:
                    reg[q] = ('', 'dead', None)
                reg['r3'] = ('t%d' % k, 'callr', None)
                reg['f1'] = ('t%d' % k, 'callf', None)
                snap = dict(reg)
                continue
            if op == 'frsp':
                v = get(a[1])
                if v[1] == 'callf':
                    callrec[int(v[0][1:])][3].add('d')
                reg[a[0]] = v; continue
            if op == 'li':
                reg[a[0]] = ('%d' % num(a[1]), 'int', None); continue
            if op == 'lis':
                ms = re.fullmatch(r'(\w+)@ha', a[1])
                if ms:
                    hi[a[0]] = ms.group(1); reg[a[0]] = ('HI', 'hi', None); continue
                reg[a[0]] = ('%d' % ((num(a[1]) << 16) - (1 << 32) if num(a[1]) & 0x8000 else num(a[1]) << 16), 'int', None)
                continue
            if op == 'addi':
                ms = re.fullmatch(r'(\w+)@l', a[2])
                if ms:
                    if hi.get(a[1]) != ms.group(1) or not sym_ok(ms.group(1)) or ext.get(ms.group(1), 'u8[]') != 'u8[]':
                        return None
                    ext[ms.group(1)] = 'u8[]'
                    reg[a[0]] = (ms.group(1), 'ptr', None); continue
                v = get(a[1])
                if v[1] == 'int' and re.fullmatch(r'-?\d+', v[0]):
                    reg[a[0]] = ('%d' % (int(v[0]) + num(a[2])), 'int', None); continue
                if v[1] in ('param', 'ptr'):
                    reg[a[0]] = ('(%s + 0x%X)' % (base(a[1]), num(a[2])) if num(a[2]) >= 0 else
                                 '(%s - 0x%X)' % (base(a[1]), -num(a[2])), 'ptr', v[2]); continue
                reg[a[0]] = ('(%s + %d)' % (ival(a[1]), num(a[2])), 'int', v[2]); continue
            if op == 'mr':
                reg[a[0]] = get(a[1]); continue
            if op == 'fmr':
                reg[a[0]] = get(a[1]); continue
            if op in LOAD or op in ('lwzx', 'lbzx', 'lhzx', 'lfsx'):
                ty = LOAD.get(op) or LOAD[op[:-1]]
                if op.endswith('x'):
                    addr = '(%s + %s)' % (base(a[1]), ival(a[2]))
                else:
                    ms = re.fullmatch(r'(\w+)@sda21\(r0\)', a[1])
                    if ms:
                        if not sym_ok(ms.group(1)) or ext.get(ms.group(1), ty) != ty:
                            return None
                        ext[ms.group(1)] = ty
                        reg[a[0]] = (ms.group(1), 'flt' if ty == 'f32' else 'int', stores[0]); continue
                    ms = re.fullmatch(IMM + r'\((r\d+)\)', a[1])
                    if not ms or ms.group(2) in ('r0', 'r1'):
                        return None
                    off = num(ms.group(1))
                    b = base(ms.group(2))
                    addr = '(%s + 0x%X)' % (b, off) if off > 0 else ('%s' % b if off == 0 else '(%s - 0x%X)' % (b, -off))
                reg[a[0]] = ('*(%s*)%s' % (ty, addr), 'flt' if ty == 'f32' else 'int', stores[0]); continue
            if op in STORE or op in ('stwx', 'stbx', 'sthx'):
                ty = STORE.get(op) or STORE[op[:-1]]
                val = fval(a[0]) if ty == 'f32' else ival(a[0])
                stored.add(val)
                if op.endswith('x'):
                    addr = '(%s + %s)' % (base(a[1]), ival(a[2]))
                    stmts.append('*(%s*)%s = %s;' % (ty, addr, val))
                else:
                    ms = re.fullmatch(r'(\w+)@sda21\(r0\)', a[1])
                    if ms:
                        if not sym_ok(ms.group(1)) or ext.get(ms.group(1), ty) != ty:
                            return None
                        ext[ms.group(1)] = ty
                        stmts.append('%s = %s;' % (ms.group(1), val))
                    else:
                        ms = re.fullmatch(IMM + r'\((r\d+)\)', a[1])
                        if not ms or ms.group(2) in ('r0', 'r1'):
                            return None
                        off = num(ms.group(1))
                        v = get(ms.group(2))
                        if v[1] == 'int' and re.fullmatch(r'-?\d+', v[0]):     # absolute address
                            addr = '0x%X' % ((int(v[0]) + off) & 0xFFFFFFFF)
                            stmts.append('*(volatile %s*)%s = %s;' % (ty, addr, val))
                        else:
                            b = base(ms.group(2))
                            addr = '(%s + 0x%X)' % (b, off) if off > 0 else ('%s' % b if off == 0 else '(%s - 0x%X)' % (b, -off))
                            stmts.append('*(%s*)%s = %s;' % (ty, addr, val))
                stores[0] += 1
                continue
            bin_ops = {'add': '+', 'subf': '-', 'and': '&', 'or': '|', 'xor': '^', 'mullw': '*'}
            if op in bin_ops:
                x, y = get(a[1]), get(a[2])
                if op == 'subf':
                    e = '(%s - %s)' % (ival(a[2]), ival(a[1]))
                elif op == 'add' and (x[1] in ('ptr',) or y[1] in ('ptr',)):
                    e = '(%s + %s)' % (base(a[1]) if x[1] == 'ptr' else ival(a[1]), base(a[2]) if y[1] == 'ptr' else ival(a[2]))
                    reg[a[0]] = (e, 'ptr', haspay(x, y)); continue
                else:
                    e = '(%s %s %s)' % (ival(a[1]), bin_ops[op], ival(a[2]))
                reg[a[0]] = (e, 'int', haspay(x, y)); continue
            imm_ops = {'mulli': '*', 'ori': '|', 'xori': '^', 'addic': '+'}
            if op in imm_ops:
                x = get(a[1])
                reg[a[0]] = ('(%s %s %d)' % (ival(a[1]), imm_ops[op], num(a[2])), 'int', x[2]); continue
            if op in ('slwi', 'srwi', 'srawi'):
                x = get(a[1])
                e = {'slwi': '(%s << %d)', 'srwi': '((u32)%s >> %d)', 'srawi': '((s32)%s >> %d)'}[op] % (ival(a[1]), num(a[2]))
                reg[a[0]] = (e, 'int', x[2]); continue
            if op in ('addis', 'oris', 'xoris'):
                x = get(a[1]); k = num(a[2]) << 16
                if op == 'addis' and x[1] == 'int' and re.fullmatch(r'-?\d+', x[0]):
                    c = (int(x[0]) + k) & 0xFFFFFFFF
                    reg[a[0]] = ('%d' % (c - (1 << 32) if c & 0x80000000 else c), 'int', None); continue
                sym = {'addis': '+', 'oris': '|', 'xoris': '^'}[op]
                reg[a[0]] = ('(%s %s 0x%X)' % (ival(a[1]), sym, k & 0xFFFFFFFF), 'int', x[2]); continue
            if op == 'subi':
                x = get(a[1])
                if x[1] in ('param', 'ptr'):
                    reg[a[0]] = ('(%s - 0x%X)' % (base(a[1]), num(a[2])), 'ptr', x[2]); continue
                reg[a[0]] = ('(%s - %d)' % (ival(a[1]), num(a[2])), 'int', x[2]); continue
            if op == 'subfic':
                x = get(a[1])
                reg[a[0]] = ('(%d - %s)' % (num(a[2]), ival(a[1])), 'int', x[2]); continue
            if op == 'cntlzw':
                x = get(a[1])
                reg[a[0]] = ('__cntlzw(%s)' % ival(a[1]), 'int', x[2]); continue
            if op == 'extrwi':          # extrwi rD, rS, n, b
                x = get(a[1]); n, b = num(a[2]), num(a[3])
                reg[a[0]] = ('(((u32)%s >> %d) & 0x%X)' % (ival(a[1]), 32 - b - n, (1 << n) - 1), 'int', x[2]); continue
            if op == 'clrlslwi':        # clrlslwi rD, rS, b, n
                x = get(a[1]); b, n = num(a[2]), num(a[3])
                reg[a[0]] = ('((%s & 0x%X) << %d)' % (ival(a[1]), (1 << (32 - b)) - 1, n), 'int', x[2]); continue
            if op in ('slw', 'srw', 'sraw', 'andc'):
                x, y = get(a[1]), get(a[2])
                e = {'slw': '(%s << %s)', 'srw': '((u32)%s >> %s)', 'sraw': '((s32)%s >> %s)',
                     'andc': '(%s & ~%s)'}[op] % (ival(a[1]), ival(a[2]))
                reg[a[0]] = (e, 'int', haspay(x, y)); continue
            if op in ('fmadds', 'fmsubs', 'fnmadds', 'fnmsubs'):      # fD = fA * fC +- fB
                x, y, z = get(a[1]), get(a[2]), get(a[3])
                e = '(%s * %s %s %s)' % (fval(a[1]), fval(a[2]), '+' if op in ('fmadds', 'fnmadds') else '-', fval(a[3]))
                if op.startswith('fn'):
                    e = '-' + e
                reg[a[0]] = (e, 'flt', haspay(x, y, z)); continue
            if op in ('extsb', 'extsh', 'neg'):
                x = get(a[1])
                e = {'extsb': '(s8)%s', 'extsh': '(s16)%s', 'neg': '-%s'}[op] % ival(a[1])
                reg[a[0]] = ('(%s)' % e, 'int', x[2]); continue
            if op == 'clrlwi':
                x = get(a[1]); n = num(a[2])
                reg[a[0]] = ('(%s & 0x%X)' % (ival(a[1]), (1 << (32 - n)) - 1), 'int', x[2]); continue
            fbin = {'fadds': '+', 'fsubs': '-', 'fmuls': '*', 'fdivs': '/'}
            if op in fbin:
                x, y = get(a[1]), get(a[2])
                reg[a[0]] = ('(%s %s %s)' % (fval(a[1]), fbin[op], fval(a[2])), 'flt', haspay(x, y)); continue
            if op == 'fneg':
                x = get(a[1])
                reg[a[0]] = ('-%s' % fval(a[1]), 'flt', x[2]); continue
            return None
    except (ValueError, KeyError, IndexError):
        return None
    ret, rexpr = 'void', None
    r3, f1 = reg['r3'], reg['f1']
    # a register left holding something is not necessarily a return value: when it cannot be
    # returned safely (it read memory before a later store), the function is taken as void; a
    # call's result left in r3/f1 is not returned either (the code is the same without it)
    if f1[1] == 'callf' and 'd' in callrec[int(f1[0][1:])][3]:
        # a double result rounded to float (frsp) and left in f1: returned. A float argument still
        # in f1 at that call is taken as passed on (a float wrapper like sinf).
        k = int(f1[0][1:])
        if not callrec[k][2] and snap_at[k].get('f1', ('', ''))[1] == 'fparam':
            callrec[k][2].append('x0'); fparams.add(0)
        rexpr, ret = 't%d' % k, 'f32'
    elif f1[1] not in ('fparam', 'callf', 'dead') or (f1[1] == 'fparam' and f1[0] != 'x0'):
        try:
            rexpr = fval('f1'); ret = 'f32'
        except ValueError:
            pass
    elif r3[1] not in ('callr', 'dead') and (r3[1] != 'param' or r3[0] != 'p0'):
        try:
            if r3[1] == 'ptr':
                rexpr = fresh(r3)[0]; ret = 'u8*'
            elif r3[1] != 'hi':
                rexpr = ival('r3'); ret = 's32'
                if rexpr in stored:
                    rexpr, ret = None, 'void'
        except ValueError:
            pass
    if not stmts and rexpr is None:
        return None
    # calls: a result that is used goes through a temporary; the callee's declared type follows
    decls, lines = [], []
    for s in stmts:
        if isinstance(s, tuple):
            k = s[1]
            tgt, iargs, fargs, uses = callrec[k]
            if 'f' in uses and 'i' in uses:
                return None
            if 'f' in uses:
                ctype = 'double' if 'd' in uses else 'f32'
                decls.append('    %s t%d;\n' % ('f32', k))
                lines.append('    t%d = %s(%s);\n' % (k, tgt, ', '.join(iargs + fargs)))
            elif 'i' in uses:
                ctype = 's32'
                decls.append('    s32 t%d;\n' % k)
                lines.append('    t%d = %s(%s);\n' % (k, tgt, ', '.join(iargs + fargs)))
            else:
                ctype = 'void'
                lines.append('    %s(%s);\n' % (tgt, ', '.join(iargs + fargs)))
            if calls.get(tgt, ctype) != ctype:
                return None
            calls[tgt] = ctype
        else:
            lines.append('    %s\n' % s)
    ni = max(iparams) + 1 if iparams else 0
    nf = max(fparams) + 1 if fparams else 0
    ps = [('u8* p%d' if k in ptrp else 's32 p%d') % k for k in range(ni)] + ['f32 x%d' % k for k in range(nf)]
    body = ''.join(decls) + ''.join(lines)
    if rexpr is not None:
        body += '    return %s;\n' % rexpr
    return '%s %s(%s) {\n%s}\n' % (ret, name, ', '.join(ps) or 'void', body)


def strip_frame(ins):
    """Remove the stack frame (prologue/epilogue, saved registers) around a body. Returns the body
    plus the final blr, or None if the frame is anything but the plain shapes."""
    if not ins[0].startswith('stwu r1, -'):
        return ins
    m = re.fullmatch(r'stwu r1, -(0x[0-9a-f]+|\d+)\(r1\)', ins[0])
    if not m:
        return None
    size = num(m.group(1))
    lr = 'stw r0, 0x%x(r1)' % (size + 4)
    lrl = 'lwz r0, 0x%x(r1)' % (size + 4)
    saved = {}
    out = []
    for i in ins[1:]:
        if i in ('mflr r0', 'mtlr r0', lr, lrl, 'addi r1, r1, 0x%x' % size):
            continue
        ms = re.fullmatch(r'(stw|stfd|psq_st) ([rf]\d+), (0x[0-9a-f]+|\d+)\(r1\)(?:, 0, qr0)?', i)
        if ms and int(ms.group(2)[1:]) >= 14:
            saved[ms.group(2)] = ms.group(3); continue
        ms = re.fullmatch(r'(lwz|lfd|psq_l) ([rf]\d+), (0x[0-9a-f]+|\d+)\(r1\)(?:, 0, qr0)?', i)
        if ms and saved.get(ms.group(2)) is not None:
            continue
        if i.startswith(('stmw', 'lmw')):
            continue
        out.append(i)
    return out


def region_functions():
    """Functions not yet in any unit, from the auto_ asm (stale files skipped by address)."""
    taken = taken_ranges()
    seen, out = set(), []
    for path in sorted(os.listdir(ASM)):
        if path.startswith('auto_') and path.endswith('_text.s'):
            for f in parse(ASM + path):
                a = f.get('addr')
                if a is None or a in seen or any(s <= a < e for s, e in taken):
                    continue
                seen.add(a)
                out.append(f)
    out.sort(key=lambda f: f['addr'])
    return out


def unit_source(fns, ext, calls):
    lines = ['// Small functions found by the sweep (sweep.py). Original file and meanings unknown.',
             '', '#include "game_types.h"', '']
    defined = {f['name'] for f in fns}
    for s, ty in sorted(ext.items()):
        if ty == 'u8[]':
            lines.append('extern u8 %s[];' % s)
        else:
            lines.append('extern %s %s;' % (ty, s))
    for g in sorted(calls):
        if g not in defined:
            lines.append('%s %s();' % (calls[g], g))
    seen_d = set()
    for f in fns:
        for d in f.get('decls', []):
            if d not in seen_d and not re.search(r'%s\(' % re.escape(f['name']), d):
                seen_d.add(d); lines.append(d)
    lines.append('')
    for f in fns:       # prototypes, so a call to a function defined further down is not implicit
        lines.append(f['c'].split(' {', 1)[0] + ';')
    lines.append('')
    return '\n'.join(lines) + '\n'.join(f['c'] for f in fns)


def write_units(units, matching):
    """units: list of (name, start, end, fns, ext, calls)."""
    sp = open(ROOT + '/config/GW4E69/splits.txt', encoding='utf-8').read()
    cf = open(ROOT + '/configure.py', encoding='utf-8').read()
    blocks = []
    for name, start, end, fns, ext, calls in units:
        open(ROOT + '/src/unsorted/%s.c' % name, 'w', encoding='utf-8', newline='\n').write(unit_source(fns, ext, calls))
        blocks.append((start, '%s.c:\n\t.text       start:0x%08X end:0x%08X\n' % ('unsorted/' + name, start, end)))
    # insert split blocks in address order among existing entries
    parts = re.split(r'\n(?=\S[^\n]*\.c(?:pp)?:\n)', sp)
    head, entries = parts[0], parts[1:]
    def start_of(e):
        m = re.search(r'\.text\s+start:0x([0-9A-F]+)', e)
        return int(m.group(1), 16) if m else None
    out = [e.rstrip('\n') + '\n' for e in entries]
    for start, blk in sorted(blocks):
        # before the first existing entry whose .text starts later (entries without .text keep their place)
        idx = next((i for i, e in enumerate(out) if start_of(e) is not None and start_of(e) > start), len(out))
        out.insert(idx, blk)
    sp = head.rstrip('\n') + '\n\n' + '\n'.join(out)
    open(ROOT + '/config/GW4E69/splits.txt', 'w', encoding='utf-8', newline='\n').write(sp)
    anchor = '            Object(NonMatching, "Gimme.c"),'
    add = ''.join('\n            Object(%s, "unsorted/%s.c"),' % ('Matching' if matching else 'NonMatching', u[0]) for u in units)
    cf = cf.replace(anchor, anchor + add, 1)
    open(ROOT + '/configure.py', 'w', encoding='utf-8', newline='\n').write(cf)


def load_skip():
    """{function: the generated C that was not exact}. A function is skipped only while the
    translators still produce that same C."""
    if not os.path.exists(SKIP):
        return {}
    d = json.load(open(SKIP))
    return d if isinstance(d, dict) else {}


USE_M2C = False
# m2c output repaired by retry.py (missing call arguments, prototypes, void* arithmetic)
_HITS_PATH = HERE + '/retry_hits.json'
HITS = {k: tuple(v) for k, v in json.load(open(_HITS_PATH)).items()} if os.path.exists(_HITS_PATH) else {}


def m2c_ok(f):
    """No literal-pool data (float constants, strings) and no paired-single asm."""
    for i in f['ins']:
        if i.split(' ')[0].startswith('ps') or i.startswith('psq'):
            return False
        for sym in re.findall(r'(\w+)@(?:sda21|ha|l)', i):
            if not sym_ok(sym):
                return False
    return True


def make_runs(maxbytes, minrun):
    skipd = load_skip()
    fns = region_functions()
    if USE_M2C:
        cand = [f for f in fns if f.get('addr') is not None and 4 * len([i for i in f['ins'] if i != 'LABEL']) <= maxbytes
                and m2c_ok(f)]
        sweep_m2c.prefetch(cand)
    units, cur = [], []

    def flush():
        if len(cur) >= minrun:
            units.append(list(cur))
        cur.clear()

    ext, calls = {}, {}
    prev_end = None
    for f in fns:
        n = len([i for i in f['ins'] if i != 'LABEL'])
        size = 4 * n
        contiguous = prev_end is not None and f.get('addr') == prev_end
        if not contiguous:
            flush(); ext, calls = {}, {}
        ok = size <= maxbytes and f.get('addr') is not None
        c = (translate(f, ext, calls) or translate_call(f, ext, calls) or translate_leaf(f, ext, calls)) if ok else None
        if c is not None and skipd.get(f['name']) == c:
            c = None
        if c is None and ok and USE_M2C and m2c_ok(f):
            r = HITS.get(f['name']) or sweep_m2c.clean(sweep_m2c._cache.get(f['name']))
            if r is not None and skipd.get(f['name']) != r[0]:
                # m2c output stands alone: its own unit
                flush(); ext, calls = {}, {}
                f['c'], f['decls'] = r
                f['end'] = f['addr'] + size
                f['_ext'], f['_calls'] = ext, calls
                cur.append(f)
                flush(); ext, calls = {}, {}
                prev_end = f['addr'] + size
                continue
        if c is None:
            flush(); ext, calls = {}, {}
        else:
            f['c'] = c
            f['end'] = f['addr'] + size
            cur.append(f)
            f['_ext'], f['_calls'] = ext, calls
        prev_end = f['addr'] + size if f.get('addr') is not None else None
    flush()
    out = []
    for run in units:
        ext, calls = run[0]['_ext'], run[0]['_calls']
        start, end = run[0]['addr'], run[-1]['end']
        out.append(('sweep_%08X' % start, start, end, run, ext, calls))
    return out


def save(units):
    json.dump([{'name': u[0], 'start': u[1], 'end': u[2],
                'fns': [{'name': f['name'], 'addr': f['addr'], 'end': f['end'], 'c': f['c'], 'decls': f.get('decls', [])}
                        for f in u[3]],
                'ext': u[4], 'calls': u[5]} for u in units], open(STATE, 'w'), indent=0)


MWCC = MWCC_CMD + ['-nodefaults', '-proc', 'gekko', '-align', 'powerpc',
        '-enum', 'int', '-fp', 'hardware', '-Cpp_exceptions', 'off', '-O4,p', '-inline', 'smart', '-maxerrors', '1',
        '-nosyspath', '-RTTI', 'off', '-fp_contract', 'on', '-str', 'reuse', '-common', 'on', '-use_lmw_stmw', 'on',
        '-lang=c', '-i', ROOT + '/include']


def drop_units(names):
    sp = open(ROOT + '/config/GW4E69/splits.txt', encoding='utf-8').read()
    cf = open(ROOT + '/configure.py', encoding='utf-8').read()
    for n in names:
        sp = re.sub(r'\nunsorted/%s\.c:\n\t\.text[^\n]*\n' % n, '\n', sp)
        cf = cf.replace('\n            Object(NonMatching, "unsorted/%s.c"),' % n, '')
        p = ROOT + '/src/unsorted/%s.c' % n
        if os.path.exists(p):
            os.remove(p)
    open(ROOT + '/config/GW4E69/splits.txt', 'w', encoding='utf-8', newline='\n').write(sp)
    open(ROOT + '/configure.py', 'w', encoding='utf-8', newline='\n').write(cf)


def compile_check(units):
    """Drop generated units that do not compile on their own."""
    bad = []
    tmp = WORK + '/_cc.o'
    for u in units:
        r = subprocess.run(MWCC + ['-c', ROOT + '/src/unsorted/%s.c' % u[0], '-o', tmp], capture_output=True, text=True)
        if r.returncode:
            bad.append(u[0])
            open(WORK + '/_cc_errors.log', 'a').write(u[0] + '\n' + r.stdout[-400:] + '\n')
    skip = load_skip()
    for u in units:
        if u[0] in bad:
            for f in u[3]:
                skip[f['name']] = f['c']
    json.dump(skip, open(SKIP, 'w'))
    drop_units(bad)
    return bad


def has_data(path):
    """A unit that brings its own data (e.g. the int-to-float constant in .sdata2) would shift the
    original's data when linked, even when every function in it matches."""
    import struct
    if not os.path.exists(path):
        return True
    d = open(path, 'rb').read()
    shoff = struct.unpack('>I', d[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', d[0x2E:0x34])
    hdr = lambda i: d[shoff + i * shentsize: shoff + (i + 1) * shentsize]
    so = struct.unpack('>I', hdr(shstrndx)[16:20])[0]
    for i in range(shnum):
        h = hdr(i)
        nm, typ = struct.unpack('>II', h[0:8])
        size = struct.unpack('>I', h[20:24])[0]
        name = d[so + nm:d.index(b'\0', so + nm)].decode()
        if size and typ in (1, 8) and name not in ('.text', '.comment', '.mwcats.text'):
            return True
    return False


def keep():
    """After ninja + report: Matching for fully exact new units, remove the rest."""
    state = json.load(open(STATE))
    rep = json.load(open(ROOT + '/build/GW4E69/report.json'))
    exact = {}
    skip = load_skip()
    gen_c = {f['name']: f['c'] for u in state for f in u['fns']}
    for u in rep['units']:
        fs = u.get('functions', [])
        exact[u['name'].split('/')[-1]] = bool(fs) and all(f.get('fuzzy_match_percent', 0) >= 100 for f in fs)
        if 'sweep_' in u['name']:
            for f in fs:
                if f.get('fuzzy_match_percent', 0) < 100 and f['name'] in gen_c:
                    skip[f['name']] = gen_c[f['name']]
    json.dump(skip, open(SKIP, 'w'))
    sp = open(ROOT + '/config/GW4E69/splits.txt', encoding='utf-8').read()
    cf = open(ROOT + '/configure.py', encoding='utf-8').read()
    kept = dropped = nf = 0
    for u in state:
        n = u['name']
        if exact.get(n) and not has_data(ROOT + '/build/GW4E69/src/unsorted/%s.o' % n):
            cf = cf.replace('Object(NonMatching, "unsorted/%s.c")' % n, 'Object(Matching, "unsorted/%s.c")' % n)
            kept += 1; nf += len(u['fns'])
        else:
            sp = re.sub(r'\nunsorted/%s\.c:\n\t\.text[^\n]*\n' % n, '\n', sp)
            cf = cf.replace('\n            Object(NonMatching, "unsorted/%s.c"),' % n, '')
            p = ROOT + '/src/unsorted/%s.c' % n
            if os.path.exists(p):
                os.remove(p)
            dropped += 1
    open(ROOT + '/config/GW4E69/splits.txt', 'w', encoding='utf-8', newline='\n').write(sp)
    open(ROOT + '/configure.py', 'w', encoding='utf-8', newline='\n').write(cf)
    print('kept %d units (%d functions), dropped %d' % (kept, nf, dropped))


if __name__ == '__main__':
    if sys.argv[1] == 'keep':
        keep()
    elif sys.argv[1] == 'gen':
        USE_M2C = '--m2c' in sys.argv
        sys.argv = [a for a in sys.argv if a != '--m2c']
        maxb = int(sys.argv[2]) if len(sys.argv) > 2 else 64
        minrun = int(sys.argv[3]) if len(sys.argv) > 3 else 2
        units = make_runs(maxb, minrun)
        save(units)
        write_units(units, matching=False)
        bad = compile_check(units)
        print('dropped %d units that do not compile' % len(bad))
        units = [u for u in units if u[0] not in bad]
        save(units)
        print(len(units), 'units,', sum(len(u[3]) for u in units), 'functions,',
              sum(u[2] - u[1] for u in units), 'bytes')
