"""Before folding: find sweep declarations that clash with each other or with the unit.
    python tools/match/declcheck.py <Name> <sweep1.c> ...     (sweep paths relative to src/)

Sweeps were written one at a time, so two of them (or a sweep and the unit's headers) can declare
the same global or function with different types: `extern s8 x;` vs `extern u8 x;`,
`extern u8* gpGame;` vs `GameState* gpGame` in golfer.h, `void f();` vs `void f(u8)`. Merged into
one file, that is "identifier redeclared" and the fold fails. This asks the compiler itself: it
builds a probe file (the unit's current source, or the sweeps' includes for a new unit, then every
sweep's top-level declarations and a prototype for every function a sweep defines), compiles it
with the unit's own command, and reports each clash with both places. Nothing in src/ is written.

Exit code 0: no clash. 1: clashes (or other errors) printed. fold.py runs this before it merges."""
import pathlib, re, subprocess, sys, tempfile
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
from merge_sweeps import defined                     # noqa: E402
from hosttools import run_ninja_command              # noqa: E402


def top_decls(text):
    """(line number, line) of the column-0 declarations outside any body."""
    out, depth = [], 0
    for i, ln in enumerate(text.split('\n'), 1):
        if depth == 0 and ln and not ln.startswith((' ', '\t', '#', '//', '}')) \
                and ln.rstrip().endswith(';') and '{' not in ln:
            out.append((i, ln))
        depth += ln.count('{') - ln.count('}')
    return out


def def_protos(text):
    """(line number, prototype) for every function the text defines."""
    lines, out = text.split('\n'), []
    for i, _ in defined(text):
        head = ' '.join(x.strip() for x in lines[i:i + 4])
        head = re.match(r'^([^;{]*\))\s*\{', head).group(1)
        out.append((i + 1, head + ';'))
    return out


def unit_command(name):
    """The unit's compile command from build.ninja; for a unit without a source yet (no build
    edge), the command of the first game C unit (same flags: the game units share them)."""
    def cmds(target):
        r = subprocess.run(['ninja', '-t', 'commands', target], cwd=ROOT, capture_output=True,
                           text=True)
        return [l for l in r.stdout.splitlines() if 'mwcceppc' in l and ' -lang=c ' in l]
    own = cmds(f'build/GW4E69/src/{name}.o')
    if own:
        return own[-1]
    other = [l for l in cmds('all_source') if re.search(r'[\\/]GC[\\/]2\.5[\\/]', l) and '-O4,p' in l
             and re.search(r' -c src[\\/]unsorted[\\/]', l)] or [l for l in cmds('all_source') if '-O4,p' in l]
    if not other:
        sys.exit(f'declcheck: no compile command for {name}.o in build.ninja (run configure.py)')
    return other[0]


def compile_probe(cmd, text, tmp):
    probe = pathlib.Path(tmp) / 'probe.c'
    probe.write_text(text, encoding='utf-8', newline='\n')
    cmd = re.sub(r' -c \S+ -o \S+', lambda _: f' -c "{probe}" -o "{tmp}"', cmd)
    cmd = re.sub(r'-maxerrors \d+', '-maxerrors 1', cmd)
    cmd = cmd.split(' && ')[0]        # the compile only: the rest post-processes the build's .d file
    r = run_ninja_command(cmd)
    return r.returncode, r.stdout + r.stderr


def first_error(out):
    """(line number, message lines) of the first error the compiler printed, or None."""
    for blk in out.split('### mwcceppc.exe Compiler:')[1:]:
        if 'Error:' not in blk:
            continue
        m = re.search(r'^#\s+(\d+): ', blk, re.M)
        msg = [l.lstrip('#').strip() for l in blk.split('Error:', 1)[1].splitlines()[1:]]
        msg = [l for l in msg if l and not l.startswith(('Too many errors', 'User break'))]
        return (int(m.group(1)) if m else None), msg
    return None


def where_declared(name, sources):
    """file:line: text of the other declarations of name: in sources, then in include/."""
    pat = re.compile(r'\b%s\b\s*(\(|\[|;|=)' % re.escape(name))
    hits = []
    for label, text in sources:
        for i, l in enumerate(text.split('\n'), 1):
            if l and not l.startswith((' ', '\t', '//', '#')) and pat.search(l):
                hits.append(f'{label}:{i}: {l.strip()}')
    for h in sorted((ROOT / 'include').rglob('*.h')):
        for i, l in enumerate(h.read_text(encoding='utf-8', errors='replace').split('\n'), 1):
            if l and not l.startswith((' ', '\t', '//', '#', '*')) and pat.search(l):
                hits.append(f'{h.relative_to(ROOT).as_posix()}:{i}: {l.strip()}')
    return hits


def check(name, sweeps, cmd=None):
    """-> list of report lines, empty when the declarations agree."""
    cmd = cmd or unit_command(name)
    unit = ROOT / 'src' / f'{name}.c'
    base = unit.read_text(encoding='utf-8') if unit.exists() else ''
    texts = [(f'src/{s}', (ROOT / 'src' / s).read_text(encoding='utf-8')) for s in sweeps]
    if base.strip():
        head, sources = base.rstrip('\n').split('\n'), [(f'src/{name}.c', base)]
    else:
        head, sources = [], []
        for _, t in texts:
            head += [l for l in t.split('\n') if l.startswith('#include') and l not in head]
    if not any(l.lstrip().startswith('#include') for l in head):
        # a unit with no includes yet (only its header comment): the merge will bring the sweeps'
        # game_types.h, and without it every s32/u8 line is a syntax error, not a clash
        head = ['#include "game_types.h"'] + head
    probe, origin = list(head), {}
    for label, t in texts:
        protos = dict(def_protos(t))
        for i, l in sorted(top_decls(t) + list(protos.items())):
            probe.append(l)
            shown = l[:-1] + ' { (definition)' if protos.get(i) == l else l
            origin[len(probe)] = (label, i, shown)
    report, clashes = [], {}                         # name -> (compiler's types, [(label, i, text)])
    with tempfile.TemporaryDirectory() as tmp:
        for _ in range(300):
            rc, out = compile_probe(cmd, '\n'.join(probe) + '\n', tmp)
            err = first_error(out)
            if not err:
                if rc:
                    report.append('the probe did not compile, compiler said:\n' + out[-1500:])
                break
            ln, msg = err
            if ln not in origin:
                report.append(f'src/{name}.c or its headers do not compile on their own '
                              f'(probe line {ln}): ' + ' / '.join(msg))
                break
            label, i, text = origin[ln]
            probe[ln - 1] = ''                       # drop it and look for the next clash
            m = re.search(r"identifier '(\w+)", ' '.join(msg))
            if m and 'redeclared' in ' '.join(msg):
                c = clashes.setdefault(m.group(1), ([x for x in msg if 'declared as' in x], []))
                c[1].append((label, i, text))
            else:
                report.append(f'{label}:{i}: {text}\n    ' + ' / '.join(msg))
    for nm, (types, where) in clashes.items():
        mine = {f'{label}:{i}:' for label, i, _ in where}
        others = [h for h in where_declared(nm, sources + texts)
                  if not any(h.startswith(x) for x in mine)]
        report.insert(0, f'{nm}: declared with another type\n'
                      + ''.join(f'    {label}:{i}: {text}\n' for label, i, text in where)
                      + ''.join(f'      {x}\n' for x in types)
                      + '    its other declarations:\n'
                      + ''.join(f'      {h}\n' for h in others[:6]))
    return report


def main():
    name, sweeps = sys.argv[1].removesuffix('.c'), sys.argv[2:]
    rep = check(name, sweeps)
    if not rep:
        unit = ROOT / 'src' / f'{name}.c'
        what = f'src/{name}.c' if unit.exists() and unit.read_text(encoding='utf-8').strip() \
            else 'each other'
        print(f'declcheck: the declarations of {len(sweeps)} sweeps agree with {what}')
        return 0
    print(f'DECLARATION CONFLICTS folding {len(sweeps)} sweeps into {name} '
          f'(fix them in the sweeps first; nothing was changed):')
    for r in rep:
        print(r.rstrip('\n'))
    print('An empty () prototype clashes with a typed one when a parameter is u8/s8/u16/s16/f32: '
          'delete the empty one. A global declared with two types: use the header\'s type and '
          'cast at the use.')
    return 1


if __name__ == '__main__':
    sys.exit(main())
