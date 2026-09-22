#!/usr/bin/env python3
"""Install refresh_history.py as the repo's post-commit hook (idempotent)."""
import os, stat
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
hook = os.path.join(ROOT, '.git', 'hooks', 'post-commit')
body = '#!/bin/sh\npython tools/dashboard/refresh_history.py >/dev/null 2>&1 || true\n'
open(hook, 'w', newline='\n').write(body)
os.chmod(hook, os.stat(hook).st_mode | stat.S_IEXEC)
print('installed', hook)
