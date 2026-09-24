"""Find edits in the repo's history that lost backslashes: a removed line and an added line in the
same hunk that are equal once backslashes (and line joins) are ignored, where the added one has fewer
backslashes. Covers src/ and include/."""
import subprocess, re, collections
import paths

log = subprocess.run(['git', 'log', '-p', '--no-merges', '-U0', '--format=@@@%h %s', '--', 'src', 'include'],
                     cwd=paths.MAIN_S, capture_output=True, text=True, encoding='utf-8', errors='replace').stdout
hits = collections.OrderedDict()
commit = None
removed, added = [], []


def flush():
    if not removed or not added:
        return
    rm = [l for l in removed if '\\' in l]
    if not rm:
        return
    ad = {re.sub(r'\s+', '', l) for l in added}
    joined_add = re.sub(r'\s+', '', ''.join(added))
    for l in rm:
        key = re.sub(r'\s+', '', l.replace('\\', ''))
        if key and (key in ad or (len(key) > 20 and key in joined_add)):
            hits.setdefault(commit, []).append(l.strip()[:120])


for line in log.split('\n'):
    if line.startswith('@@@'):
        flush(); removed, added = [], []
        commit = line[3:]
    elif line.startswith('@@') or line.startswith('diff ') or line.startswith('+++') or line.startswith('---'):
        flush(); removed, added = [], []
    elif line.startswith('-'):
        removed.append(line[1:])
    elif line.startswith('+'):
        added.append(line[1:])
flush()
for c, ls in hits.items():
    print(c)
    for l in ls[:6]:
        print('    ', l)
print(len(hits), 'commits')
