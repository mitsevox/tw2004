"""Where the build's tools are on this machine, and how to run them. The match tools ask here
instead of naming `.exe` paths or testing os.name, so each of them runs unchanged on Windows and
on Linux.

    from hosttools import mwcc, binutil, OBJDIFF_CLI
    subprocess.run(mwcc() + flags + ['-c', src, '-o', obj])
    subprocess.run([binutil('nm'), obj])

Everything that differs between the platforms is in the two blocks below, one per platform."""
import os, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in

if os.name == 'nt':
    # ---- Windows: every tool is a native .exe --------------------------------------------------
    WRAP = []
    EXE = '.exe'
    NINJA_SHELL = False          # a build.ninja command line goes straight to CreateProcess
    NINJA_SEP = '\\'             # build.ninja writes native paths
    DRIVE = ''                   # an absolute path is already one the compiler can open
    NEW_PROCESS_GROUP = {}       # Popen keywords for a process tree stop_tree() can end

    def stop_tree(p):
        """End a running process and everything it started."""
        subprocess.run(['taskkill', '/PID', str(p.pid), '/T', '/F'], capture_output=True)
        p.wait()

    def kill_orphan_workers():
        """Kill multiprocessing workers whose parent is gone. Windows does not end a worker when its
        parent dies, so a permuter that finished, crashed or was stopped from outside leaves its
        pool behind (60 idle workers holding 2.9 GB were found on 2026-09-23)."""
        ps = ("$p = Get-CimInstance Win32_Process; $alive = $p.ProcessId; "
              "$p | Where-Object { $_.CommandLine -match 'spawn_main\\(parent_pid=(\\d+)' -and "
              "-not ($alive -contains [int]$Matches[1]) } | "
              "ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }")
        subprocess.run(['powershell', '-NoProfile', '-Command', ps], capture_output=True)
else:
    # ---- Linux / macOS: the compiler is a Windows program and runs through wibo (downloaded by
    # the build, as build.ninja does) or wine; binutils, objdiff-cli and dtk are native, no .exe ----
    import signal
    _wibo = ROOT / 'build/tools/wibo'
    WRAP = [_wibo.as_posix()] if _wibo.exists() else ['wine']
    EXE = ''
    NINJA_SHELL = True           # ninja runs its command lines through /bin/sh
    NINJA_SEP = '/'
    # wibo and wine show the compiler / as drive Z:. Paths on its command line are translated;
    # a path the compiler reads inside a file (an #include line) is not, so it needs the drive.
    DRIVE = 'Z:'
    # its own process group: killing only the parent left the permuter's pool workers running
    NEW_PROCESS_GROUP = {'start_new_session': True}

    def stop_tree(p):
        """End a process started with NEW_PROCESS_GROUP and everything it started: SIGINT first
        (the permuter's own clean exit), SIGKILL if the group is still there after 15 s."""
        os.killpg(p.pid, signal.SIGINT)
        try:
            p.wait(timeout=15)
        except subprocess.TimeoutExpired:
            os.killpg(p.pid, signal.SIGKILL)
            p.wait()

    def kill_orphan_workers():
        """Nothing to do: stop_tree() ends the whole process group."""


def mwcc(version='GC/2.5'):
    """The command that runs the Metrowerks compiler (a list: add flags and files after it)."""
    return WRAP + [(ROOT / 'build/compilers' / version / 'mwcceppc.exe').as_posix()]


def binutil(name):
    """A powerpc-eabi binutils program by its short name: binutil('nm'), binutil('readelf')."""
    return (ROOT / 'build/binutils' / f'powerpc-eabi-{name}{EXE}').as_posix()


# paths are handed out with forward slashes: Windows programs accept them, and a Windows path
# written into a bash script (perm_setup's compile.sh) keeps no backslashes for bash to eat
OBJDIFF_CLI = (ROOT / f'build/tools/objdiff-cli{EXE}').as_posix()


def include_path(path):
    """An absolute path to write into an #include line of a file the compiler reads."""
    return DRIVE + pathlib.Path(path).resolve().as_posix()


def reported_name(path):
    """The file name in a compiler message. It prints Windows paths on every platform
    (`src\\Earnings.c:12: ...`, `Z:\\tmp\\x\\probe.c:3: ...`), which Linux's pathlib does not split."""
    return pathlib.PureWindowsPath(path).name


def run_ninja_command(cmd, **kw):
    """Run one command line taken from build.ninja (`ninja -t commands`) the way ninja would."""
    return subprocess.run(cmd, shell=NINJA_SHELL, cwd=ROOT, capture_output=True, text=True, **kw)
