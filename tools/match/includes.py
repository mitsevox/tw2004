"""Which headers a source file sees: its #include "..." lines, followed through the headers they
include, as the compiler finds them (the including file's own folder first, then include/ and
build/GW4E69/include, the -i folders of every unit). Angle-bracket includes are not followed.

    python tools/match/includes.py src/GoTerrain.c     # every header it sees, relative to include/
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
DIRS = [ROOT / 'include', ROOT / 'build/GW4E69/include']
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
_cache = {}


def resolve(name, here):
    """The file `#include "name"` in a file in folder `here` opens, or None."""
    for d in [here] + DIRS:
        f = (d / name).resolve()
        if f.is_file():
            return f
    return None


def _direct(f):
    if f not in _cache:
        text = f.read_text(encoding='utf-8', errors='replace')
        _cache[f] = [x for x in (resolve(n, f.parent) for n in INCLUDE.findall(text)) if x]
    return _cache[f]


def seen_headers(src):
    """Resolved paths of every header src includes, directly or not (src: a path or the text of
    a file in src/)."""
    if isinstance(src, str):
        todo = [x for x in (resolve(n, ROOT / 'src') for n in INCLUDE.findall(src)) if x]
    else:
        todo = list(_direct(pathlib.Path(src).resolve()))
    seen = set()
    while todo:
        f = todo.pop()
        if f not in seen:
            seen.add(f)
            todo += _direct(f)
    return seen


def label(f):
    """A header's name as a unit would include it (relative to its include folder)."""
    for d in DIRS:
        try:
            return f.relative_to(d.resolve()).as_posix()
        except ValueError:
            pass
    return f.as_posix()


if __name__ == '__main__':
    for h in sorted(label(f) for f in seen_headers(pathlib.Path(sys.argv[1]))):
        print(h)
