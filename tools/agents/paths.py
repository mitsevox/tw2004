"""Where the agent tools find things. Defaults fit the layout on any machine:
    <parent>/tw2004            the main checkout (this repo)
    <parent>/tw2004-agents/X   one git worktree per agent lane (branch agent/X)
    <parent>/scratch/tw/agents/X   each lane's scratch folder (never committed)
Override with the environment variables TW_MAIN, TW_WORKTREES, TW_SCRATCH, TW_PERMUTER."""
import os, pathlib

MAIN = pathlib.Path(os.environ.get('TW_MAIN', pathlib.Path(__file__).resolve().parents[2]))
WORKTREES = pathlib.Path(os.environ.get('TW_WORKTREES', MAIN.parent / (MAIN.name + '-agents')))
SCRATCH = pathlib.Path(os.environ.get('TW_SCRATCH', MAIN.parent / 'scratch' / 'tw' / 'agents'))
PERMUTER = pathlib.Path(os.environ.get('TW_PERMUTER', MAIN.parent / 'tools' / 'decomp-permuter' / 'permuter.py'))
MAIN_S, WORKTREES_S, SCRATCH_S = MAIN.as_posix(), WORKTREES.as_posix(), SCRATCH.as_posix()
