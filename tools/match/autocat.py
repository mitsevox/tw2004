"""Count the uncharted game code in "Game Code": put the .text gap units (dtk's auto_01_<addr>_text,
code no unit covers yet) that lie below the first SDK unit in the "game" progress category.

dtk makes those units itself, so configure.py has no Object for them and objdiff.json gives them
no category: the report then left ~400 KB of EA code out of "Game Code" altogether and the game
percentage read higher than it is. configure.py calls this right after it writes objdiff.json.
Gaps at or above the first SDK unit are left alone (the SDK and EA libraries mix there).

    python tools/match/autocat.py        # what it would change, from the current objdiff.json
"""
import json, pathlib, re

ROOT = pathlib.Path(__file__).resolve().parents[2]
AUTO_TEXT = re.compile(r'^main/auto_\d+_([0-9A-Fa-f]{8})_text$')


def first_sdk_text(units, splits):
    """Lowest .text start of the units objdiff.json puts in "sdk" (their splits.txt blocks)."""
    sdk = {u['name'][len('main/'):] for u in units
           if 'sdk' in (u.get('metadata') or {}).get('progress_categories', [])}
    lo = None
    for b in re.split(r'\n(?=\S[^\n]*:\n)', splits):
        m = re.match(r'(\S[^\n]*?)(\.\w+)?:\n', b)
        t = re.search(r'\.text\s+start:0x([0-9A-Fa-f]+)', b)
        if m and t and m.group(1) in sdk:
            a = int(t.group(1), 16)
            lo = a if lo is None or a < lo else lo
    return lo


def categorise_auto_units(objdiff='objdiff.json', splits='config/GW4E69/splits.txt', write=True):
    """-> (boundary, units changed, their total size unknown here). Writes objdiff.json."""
    path = pathlib.Path(objdiff)
    j = json.loads(path.read_text(encoding='utf-8'))
    bound = first_sdk_text(j['units'], pathlib.Path(splits).read_text(encoding='utf-8'))
    if bound is None:
        return None, []
    changed = []
    for u in j['units']:
        m = AUTO_TEXT.match(u['name'])
        md = u.setdefault('metadata', {})
        if m and md.get('auto_generated') and not md.get('progress_categories') \
                and int(m.group(1), 16) < bound:
            md['progress_categories'] = ['game']
            changed.append(u['name'])
    if write and changed:
        path.write_text(json.dumps(j, indent=2) + '\n', encoding='utf-8', newline='\n')
    return bound, changed


if __name__ == '__main__':
    b, c = categorise_auto_units(ROOT / 'objdiff.json', ROOT / 'config/GW4E69/splits.txt',
                                 write=False)
    print('first SDK .text at 0x%08X: %d uncategorised gap units below it would count as game'
          % (b or 0, len(c)))
