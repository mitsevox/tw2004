"""Per-source-file function lists from TW07's PS3 debug build (DWARF), for naming and matching.
    python tools/match/tw07dwarf.py <Dev_Golf_Release_PS3.elf> <out dir>
Writes one text file per compilation unit (named after its source file) and an index.tsv. Each
function is listed in SOURCE ORDER (declaration line) with its address and size on PS3 (or
"inline-only" when every call was inlined), return type, parameters and local variables (names and
types, in declaration order), and the functions inlined into it. TW07 was built as unity files
(golf_unity.cpp etc.); the lists are split back into the real source files by DW_AT_decl_file. TW07 is a 2006 PS3 build: names, signatures and source layout are era-adjacent evidence,
not facts about TW2004 (docs/reference-builds/README.md). Needs pyelftools."""
import collections, pathlib, re, sys
from elftools.elf.elffile import ELFFile


def attr(die, name, default=None):
    a = die.attributes.get(name)
    return a.value if a is not None else default


def text(v):
    return v.decode('utf-8', 'replace') if isinstance(v, bytes) else v


class Types:
    def __init__(self, cu):
        self.cu = cu
        self.cache = {}

    def die(self, die, name='DW_AT_type'):
        if name not in die.attributes:
            return None
        return die.get_DIE_from_attribute(name)

    def name(self, die, depth=0):
        """A C-like spelling of a type DIE (typedef/struct names kept, pointers and arrays spelled)."""
        if die is None:
            return 'void'
        if depth > 12:
            return '?'
        key = die.offset
        if key in self.cache:
            return self.cache[key]
        tag = die.tag
        n = text(attr(die, 'DW_AT_name'))
        inner = lambda: self.name(self.die(die), depth + 1)
        if tag in ('DW_TAG_base_type', 'DW_TAG_typedef', 'DW_TAG_unspecified_type'):
            s = n or '?'
        elif tag in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
            s = 'struct ' + (n or '<anon>')
        elif tag == 'DW_TAG_union_type':
            s = 'union ' + (n or '<anon>')
        elif tag == 'DW_TAG_enumeration_type':
            s = 'enum ' + (n or '<anon>')
        elif tag == 'DW_TAG_pointer_type':
            s = inner() + '*'
        elif tag == 'DW_TAG_reference_type':
            s = inner() + '&'
        elif tag == 'DW_TAG_const_type':
            s = 'const ' + inner()
        elif tag == 'DW_TAG_volatile_type':
            s = 'volatile ' + inner()
        elif tag == 'DW_TAG_array_type':
            dims = ''
            for c in die.iter_children():
                if c.tag == 'DW_TAG_subrange_type':
                    ub = attr(c, 'DW_AT_upper_bound')
                    dims += '[%s]' % (ub + 1 if isinstance(ub, int) else '')
            s = inner() + dims
        elif tag == 'DW_TAG_subroutine_type':
            s = inner() + '(*)()'
        else:
            s = n or tag.replace('DW_TAG_', '')
        self.cache[key] = s
        return s


def origin(die):
    """Follow DW_AT_specification / DW_AT_abstract_origin to the DIE that carries the name."""
    seen = 0
    while seen < 4 and 'DW_AT_name' not in die.attributes:
        for a in ('DW_AT_specification', 'DW_AT_abstract_origin'):
            if a in die.attributes:
                die = die.get_DIE_from_attribute(a)
                break
        else:
            break
        seen += 1
    return die


def scope_name(die):
    """Class::name for C++ member functions (via the declaration's parent), plain name otherwise."""
    d = origin(die)
    n = text(attr(d, 'DW_AT_name')) or '?'
    p = d.get_parent()
    if p is not None and p.tag in ('DW_TAG_structure_type', 'DW_TAG_class_type', 'DW_TAG_namespace'):
        pn = text(attr(p, 'DW_AT_name'))
        if pn:
            n = pn + '::' + n
    return n


def walk_locals(die, types, out, inlined):
    for c in die.iter_children():
        if c.tag == 'DW_TAG_variable':
            o = origin(c)
            out.append('%s %s' % (types.name(types.die(o)), text(attr(o, 'DW_AT_name')) or '?'))
        elif c.tag == 'DW_TAG_lexical_block':
            walk_locals(c, types, out, inlined)
        elif c.tag == 'DW_TAG_inlined_subroutine':
            inlined.append(scope_name(c))


def main():
    elf_path, out = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
    out.mkdir(parents=True, exist_ok=True)
    f = ELFFile(open(elf_path, 'rb'))
    dw = f.get_dwarf_info()
    byfile = collections.defaultdict(list)
    seen = collections.defaultdict(set)
    for cu in dw.iter_CUs():
        top = cu.get_top_DIE()
        cuname = text(attr(top, 'DW_AT_name')) or '?'
        types = Types(cu)
        lp = dw.line_program_for_CU(cu)
        files = []
        if lp:
            dirs = [text(d) for d in lp['include_directory']]
            for fe in lp['file_entry']:
                n = text(fe.name)
                d = dirs[fe.dir_index - 1] if 0 < fe.dir_index <= len(dirs) else ''
                files.append((d + '/' + n) if d and not re.match(r'^[A-Za-z]:|^/', n) else n)
        for die in top.iter_children():
            if die.tag != 'DW_TAG_subprogram' or attr(die, 'DW_AT_declaration'):
                continue
            inline_only = 'DW_AT_low_pc' not in die.attributes
            if inline_only and attr(die, 'DW_AT_inline') not in (1, 3):
                continue                       # a declaration, not code
            o = origin(die)
            name = scope_name(die)
            line = attr(die, 'DW_AT_decl_line') or attr(o, 'DW_AT_decl_line') or 0
            lo = 0 if inline_only else attr(die, 'DW_AT_low_pc')
            hi = attr(die, 'DW_AT_high_pc')
            size = 0 if inline_only else (hi if hi is not None and hi < lo else (hi - lo if hi else 0))
            ret = types.name(types.die(o))
            params, locs, inl = [], [], []
            for c in die.iter_children():
                if c.tag == 'DW_TAG_formal_parameter':
                    po = origin(c)
                    params.append('%s %s' % (types.name(types.die(po)), text(attr(po, 'DW_AT_name')) or ''))
            walk_locals(die, types, locs, inl)
            static = '' if attr(o, 'DW_AT_external') else 'static '
            fi = attr(die, 'DW_AT_decl_file') or attr(o, 'DW_AT_decl_file') or 0
            src = files[fi - 1] if 0 < fi <= len(files) else cuname
            src = re.sub(r'[/\\]+', '/', src)
            while re.search(r'/[^/.][^/]*/\.\./', src):
                src = re.sub(r'/[^/.][^/]*/\.\./', '/', src, count=1)
            key = (line, name)
            if key in seen[src]:
                continue                       # a header function emitted by several unity files
            seen[src].add(key)
            byfile[src].append((line, lo, size, static, ret, name, params, locs, inl, cuname))
    index = []
    used = collections.Counter()
    for src in sorted(byfile, key=str.lower):
        funcs = sorted(byfile[src], key=lambda x: (x[0], x[1]))
        base = src.split('/')[-1] or 'unknown'
        used[base.lower()] += 1
        fname = base if used[base.lower()] == 1 else '%s.%d' % (base, used[base.lower()])
        cus = sorted({x[9].split('/')[-1] for x in funcs})
        lines = ['// %s' % src, '// compiled in: %s' % ', '.join(cus),
                 '// %d functions, source order. line | PS3 address | size' % len(funcs), '']
        funcs = [x[:9] for x in funcs]
        for line, lo, size, static, ret, name, params, locs, inl in funcs:
            where = '%08X  %5X' % (lo, size) if lo else '  inline-only '
            lines.append('%5d  %s  %s%s %s(%s)' % (line, where, static, ret, name, ', '.join(params)))
            for v in locs:
                lines.append('                         local  %s' % v)
            if inl:
                lines.append('                         inlines  %s' % ', '.join(sorted(set(inl))))
        (out / (fname + '.txt')).write_text('\n'.join(lines) + '\n', encoding='utf-8')
        index.append('%s\t%d\t%s' % (fname, len(funcs), src))
    (out / 'index.tsv').write_text('file\tfunctions\tsource\n' + '\n'.join(sorted(index)) + '\n', encoding='utf-8')
    print('%d compilation units with functions; %d functions' % (len(index), sum(int(x.split('\t')[1]) for x in index)))


if __name__ == '__main__':
    main()
