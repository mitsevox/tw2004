"""Create an agent worktree, ready to build.
    python tools/agents/new_agent.py <name>
Makes branch agent/<name> from main at <worktrees>/<name>, links orig/GW4E69/sys and the
build tool folders to the main checkout, and configures (configure.py detects a worktree and uses
the main checkout's tools, so nothing is downloaded). Then builds once and checks the DOL. Safe to
re-run on an existing worktree: it only fills in what is missing.
Also creates the scratch folder <scratch>/<name>/."""
import os, subprocess, sys
import paths

MAIN = paths.MAIN_S
name = sys.argv[1]
wt = f'{paths.WORKTREES_S}/{name}'
scratch = f'{paths.SCRATCH_S}/{name}'


def run(cmd, cwd=None, check=True):
    r = subprocess.run(cmd, cwd=cwd or MAIN, shell=isinstance(cmd, str), capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f'failed: {cmd}\n{r.stdout[-2000:]}\n{r.stderr[-2000:]}')
    return r


if not os.path.exists(wt):        # an existing worktree is finished off (links, configure, build)
    run(['git', '-C', MAIN, 'worktree', 'add', '-q', '-b', f'agent/{name}', wt, 'main'])
win = lambda p: p.replace('/', '\\')


def link(path, target):
    """A directory link: a junction on Windows (no admin rights needed), a symlink elsewhere."""
    if os.path.exists(path):
        return
    os.makedirs(os.path.dirname(path), exist_ok=True)
    if os.name == 'nt':
        run(f'powershell -NoProfile -Command "New-Item -ItemType Junction -Path \'{win(path)}\' '
            f'-Target \'{win(target)}\' | Out-Null"')
    else:
        os.symlink(target, path, target_is_directory=True)


link(f'{wt}/orig/GW4E69/sys', f'{MAIN}/orig/GW4E69/sys')
for d in ('compilers', 'tools', 'binutils'):
    link(f'{wt}/build/{d}', f'{MAIN}/build/{d}')
os.makedirs(scratch, exist_ok=True)
run(['python', 'configure.py'], cwd=wt)   # in a worktree it uses the main checkout's tools
b = run('ninja', cwd=wt, check=False)
ok = 'main.dol: OK' in b.stdout
run('ninja build/GW4E69/report.json', cwd=wt, check=False)
print(f'{wt} on agent/{name}:', 'main.dol: OK' if ok else 'BUILD FAILED\n' + b.stdout[-1500:])
