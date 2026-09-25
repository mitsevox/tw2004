"""Try source variants for one function and keep only an exact one.

Write a small script (in your scratch folder, not the repo):

    import sys; sys.path.insert(0, r'<checkout>/tools/match')
    from trial import Trial
    t = Trial('GameMode8')                      # unit name, src/GameMode8.c
    t.trial('fn_800FA518', old_text, [variant1, variant2, ...])
    t.apply(old_text, new_text)                 # unconditional edit, no scoring

trial() replaces the first occurrence of old_text with each variant in turn, rebuilds the report,
and keeps the first variant that scores exactly 100. If none does, the file is put back as it was.
old_text must exist in the file (an assert catches typos). A build error scores 'ERR'.
"""
import json, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in


class Trial:
    def __init__(self, unit):
        self.unit = 'main/' + unit
        self.path = ROOT / 'src' / f'{unit}.c'
        self.base = self.path.read_text(encoding='utf-8')

    def _write(self, s):
        self.path.write_text(s, encoding='utf-8')

    def score(self, fn):
        r = subprocess.run('ninja build/GW4E69/report.json', shell=True, cwd=ROOT,
                           capture_output=True, text=True)
        if 'FAILED' in r.stdout:
            return 'ERR'
        rep = json.load(open(ROOT / 'build/GW4E69/report.json'))
        for u in rep['units']:
            if u['name'] == self.unit:
                for f in u['functions']:
                    if f['name'] == fn:
                        p = f.get('fuzzy_match_percent')
                        return 'None' if p is None else ('100' if p == 100 else str(p))
        return 'missing'

    def trial(self, fn, old, variants):
        best = None
        for i, v in enumerate(variants):
            s = self.base.replace(old, v, 1)
            assert s != self.base, i
            self._write(s)
            sc = self.score(fn)
            print(fn, i, sc)
            if sc == '100' and best is None:
                best = s
        self._write(best or self.base)
        if best:
            self.base = best
        return best

    def apply(self, old, new):
        s = self.base.replace(old, new, 1)
        assert s != self.base
        self.base = s
        self._write(s)
