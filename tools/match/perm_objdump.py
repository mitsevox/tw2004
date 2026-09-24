"""objdump stand-in for decomp-permuter (Windows blocks the downloaded powerpc-eabi-objdump).
    python perm_objdump.py <fn> <file.o>
Disassembles <fn> with objdiff-cli and prints it in objdump -d layout. Data symbol names are
blanked so the game's lbl_XXXXXXXX and the compiler's @NNN literals compare equal; branch
targets are function-relative offsets on both sides."""
import json, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in

fn, obj = sys.argv[1], sys.argv[2]
sys.path.insert(0, str(ROOT / 'tools/match'))
from hosttools import OBJDIFF_CLI                    # noqa: E402
out = subprocess.run([OBJDIFF_CLI, 'diff', '-1', obj, '-2', obj, fn,
                      '-o', '-', '--format', 'json'], capture_output=True, text=True).stdout
d = json.loads(out)
lines = ['', 'Disassembly of section .text:', '', '00000000 <%s>:' % fn]
for s in d['left']['symbols']:
    if s['name'] != fn:
        continue
    for i, ins in enumerate(s.get('instructions', [])):
        f = ins.get('instruction', {}).get('formatted', '?')
        f = re.sub(r'(@\d+|lbl_[0-9A-Fa-f]+|\.\w+)(?=@(?:sda21|ha|l|h)\b)', 'SYM', f)
        mn, _, args = f.partition(' ')
        args = args.replace(' ', '')
        lines.append('%8x:\t00 00 00 00\t%s\t%s' % (i * 4, mn, args))
print('\n'.join(lines))
