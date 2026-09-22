#!/usr/bin/env python3
"""
Run a command without console windows flashing up on Windows.

    python tools/research/quiet.py ninja
    python tools/research/quiet.py python configure.py

Child processes (compilers, dtk, objdiff) inherit the hidden console, so they stay hidden too.
Output is passed through; the exit code is the command's exit code.
"""
import subprocess, sys
flags = getattr(subprocess, 'CREATE_NO_WINDOW', 0)
r = subprocess.run(sys.argv[1:], creationflags=flags, capture_output=True, text=True, errors='replace')
sys.stdout.write(r.stdout); sys.stderr.write(r.stderr)
sys.exit(r.returncode)
