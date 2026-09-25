"""Where the sweep tools read and write, on any machine (they came from the PC's scratch folder).

ROOT: this checkout. HERE: tools/sweep (committed data: retry_hits.json). WORK: state, caches and
temporary files (build/sweep, never committed; TW_SWEEP_WORK overrides). MWCC_CMD: the compiler
through hosttools (wibo on Linux). M2C: m2c.py beside the checkout (tools/m2c, as tools/cloud/setup.sh
installs it), or TW_M2C."""
import os, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2].as_posix()
HERE = pathlib.Path(__file__).resolve().parent.as_posix()
WORK = os.environ.get('TW_SWEEP_WORK') or ROOT + '/build/sweep'
os.makedirs(WORK, exist_ok=True)

sys.path.insert(0, ROOT + '/tools/match')
from hosttools import mwcc  # noqa: E402
MWCC_CMD = mwcc('GC/2.5')

_parent = pathlib.Path(ROOT).parent
M2C = os.environ.get('TW_M2C') or next(
    (p.as_posix() for p in (_parent / 'tools/m2c/m2c.py', _parent.parent / 'tools/m2c/m2c.py') if p.exists()),
    (_parent / 'tools/m2c/m2c.py').as_posix())
