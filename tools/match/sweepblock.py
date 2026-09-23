"""The marked block of not-yet-cleaned sweep code in a unit.

When a unit takes over sweep files (machine-written C for small functions, from src/unsorted), their
code goes in unchanged between these two lines:

    // ---- sweep code (not yet cleaned up) ----
    ...
    // ---- end of sweep code ----

lint.py skips its style and portability rules there (not its undefined-behaviour checks), and
typeaudit.py and symaudit.py leave its declarations out of their counts; lint.py reports how many
such lines are left. Cleaning a function means moving it out of the block and writing it to
docs/style.md."""
import re

BEGIN = '// ---- sweep code (not yet cleaned up) ----'
END = '// ---- end of sweep code ----'


def lines_in_blocks(lines):
    """1-based numbers of the lines inside a block, markers included."""
    out, inside = set(), False
    for i, l in enumerate(lines, 1):
        s = l.strip()
        if s == BEGIN:
            inside = True
        if inside:
            out.add(i)
        if s == END:
            inside = False
    return out


def strip_blocks(text):
    """The text with every block removed (for counting declarations outside them)."""
    return re.sub(re.escape(BEGIN) + r'.*?' + re.escape(END), '', text, flags=re.S)
