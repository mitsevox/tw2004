#!/usr/bin/env python3
"""
Local progress dashboard for the tw2004 decompilation.

    python tools/dashboard/server.py [--port 8420] [--host 0.0.0.0]

Serves one page and /api/progress. Reads build/GW4E69/report.json (written by every `ninja`
run), config/GW4E69/symbols.txt, and git history. No dependencies beyond the standard library.
Read-only: it never builds or edits anything.
"""
import argparse, json, os, re, subprocess, sys, time, threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
VERSION = 'GW4E69'
REPORT = os.path.join(ROOT, 'build', VERSION, 'report.json')
SYMBOLS = os.path.join(ROOT, 'config', VERSION, 'symbols.txt')
HISTORY = os.path.join(ROOT, 'build', 'dashboard_history.json')

_cache = {'history': None, 'history_head': None}
_lock = threading.Lock()


def git(*args):
    return subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True).stdout


def read_report():
    try:
        r = json.load(open(REPORT))
    except Exception:
        return None
    m = {k: (int(v) if isinstance(v, str) and v.isdigit() else v) for k, v in r.get('measures', {}).items()}
    units = []
    for u in r.get('units', []):
        if u.get('metadata', {}).get('auto_generated'):
            continue
        um = {k: (int(v) if isinstance(v, str) and v.isdigit() else v) for k, v in u.get('measures', {}).items()}
        fns = u.get('functions', [])
        units.append({
            'name': u['name'],
            'code': um.get('total_code', 0),
            'matched_code': um.get('matched_code', 0),
            'matched_code_percent': um.get('matched_code_percent', 0.0),
            'functions': len(fns),
            'matched_functions': sum(1 for f in fns if f.get('fuzzy_match_percent', 0) >= 100),
            'linked': um.get('complete_code_percent', 0.0) >= 100,
            'category': u.get('metadata', {}).get('progress_categories', []),
            'fns': [{'name': f['name'], 'size': int(f.get('size', 0)), 'pct': f.get('fuzzy_match_percent', 0)} for f in fns],
        })
    return {
        'mtime': os.path.getmtime(REPORT),
        'total_code': m.get('total_code', 0),
        'matched_code': m.get('matched_code', 0),
        'fuzzy_code': m.get('fuzzy_match_percent', 0.0),
        'total_functions': m.get('total_functions', 0),
        'matched_functions': m.get('matched_functions', 0),
        'total_data': m.get('total_data', 0),
        'matched_data': m.get('matched_data', 0),
        'units': units,
    }


def read_symbols():
    total = named = 0
    try:
        for line in open(SYMBOLS, errors='replace'):
            m = re.match(r'^(\S+) = \.(?:text|init):0x[0-9A-Fa-f]+; // type:function', line)
            if m:
                total += 1
                if not m.group(1).startswith('fn_'):
                    named += 1
    except Exception:
        pass
    return {'functions': total, 'named': named}


def history():
    """Matched code bytes at every commit, from configure.py + splits.txt at that commit."""
    head = git('rev-parse', 'HEAD').strip()
    with _lock:
        if _cache['history_head'] == head and _cache['history']:
            return _cache['history']
    out = []
    log = git('log', '--reverse', '--format=%H|%ct|%s')
    for line in log.splitlines():
        h, ts, subject = line.split('|', 2)
        cfg = git('show', f'{h}:configure.py')
        units = set(re.findall(r'Object\(Matching, "([^"]+)"\)', cfg))
        splits = git('show', f'{h}:config/{VERSION}/splits.txt')
        matched = 0
        funcs = 0
        cur = None
        for s in splits.splitlines():
            m = re.match(r'^(\S.*?):\s*$', s)
            if m:
                cur = m.group(1)
                continue
            m = re.match(r'\s+\.text\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)', s)
            if m and cur in units:
                matched += int(m.group(2), 16) - int(m.group(1), 16)
        syms = git('show', f'{h}:config/{VERSION}/symbols.txt')
        named = sum(1 for l in syms.splitlines() if re.match(r'^(?!fn_)\S+ = \.(?:text|init):0x[0-9A-Fa-f]+; // type:function', l))
        out.append({'commit': h[:7], 'time': int(ts), 'subject': subject, 'matched_code': matched, 'named': named})
    with _lock:
        _cache['history'] = out
        _cache['history_head'] = head
    return out


def progress():
    rep = read_report()
    sym = read_symbols()
    hist = history()
    commits = git('log', '-12', '--format=%h|%ct|%s')
    return {
        'now': time.time(),
        'report': rep,
        'symbols': sym,
        'history': hist,
        'commits': [dict(zip(('hash', 'time', 'subject'), l.split('|', 2))) for l in commits.splitlines()],
        'branch': git('rev-parse', '--abbrev-ref', 'HEAD').strip(),
        'dirty': bool(git('status', '--porcelain').strip()),
    }


PAGE = r'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"><title>tw2004 decomp</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
:root{--bg:#0f1115;--card:#171a21;--line:#262a33;--fg:#e6e8ec;--dim:#8b919c;--acc:#4ade80;--acc2:#60a5fa;--warn:#fbbf24}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--fg);font:15px/1.45 system-ui,Segoe UI,Roboto,sans-serif}
.wrap{max-width:1100px;margin:0 auto;padding:24px 16px}
h1{font-size:20px;margin:0 0 4px;font-weight:600}.sub{color:var(--dim);font-size:13px;margin-bottom:20px}
.grid{display:grid;gap:14px;grid-template-columns:repeat(auto-fit,minmax(230px,1fr))}
.card{background:var(--card);border:1px solid var(--line);border-radius:10px;padding:16px}
.k{color:var(--dim);font-size:12px;text-transform:uppercase;letter-spacing:.06em}
.v{font-size:30px;font-weight:600;margin-top:4px;font-variant-numeric:tabular-nums}.v small{font-size:14px;color:var(--dim);font-weight:400}
.meter{height:14px;background:#0a0c10;border-radius:7px;overflow:hidden;margin-top:10px;border:1px solid var(--line)}
.meter i{display:block;height:100%;background:linear-gradient(90deg,var(--acc2),var(--acc));min-width:2px;transition:width .6s}
.big{grid-column:1/-1}.big .v{font-size:44px}
table{width:100%;border-collapse:collapse;font-size:14px}th,td{text-align:left;padding:7px 8px;border-bottom:1px solid var(--line)}th{color:var(--dim);font-weight:500;font-size:12px;text-transform:uppercase}
td.n{text-align:right;font-variant-numeric:tabular-nums}
.pct{display:inline-block;min-width:56px}.ok{color:var(--acc)}.part{color:var(--warn)}
svg{width:100%;height:180px;display:block}
.foot{color:var(--dim);font-size:12px;margin-top:20px}
code{background:#0a0c10;padding:1px 5px;border-radius:4px;font-size:13px}
</style></head><body><div class="wrap">
<h1>Tiger Woods PGA Tour 2004 &middot; decompilation</h1>
<div class="sub" id="sub">loading&hellip;</div>
<div class="grid" id="cards"></div>
<div class="card" style="margin-top:14px"><div class="k">Matched code over time (bytes, per commit)</div><svg id="chart" viewBox="0 0 1000 180" preserveAspectRatio="none"></svg><div id="chartlbl" class="foot" style="margin-top:6px"></div></div>
<div class="card" style="margin-top:14px"><div class="k">Source files</div><table id="units"><thead><tr><th>File</th><th class="n">Functions</th><th class="n">Bytes</th><th class="n">Match</th></tr></thead><tbody></tbody></table></div>
<div class="card" style="margin-top:14px"><div class="k">Recent commits</div><table id="commits"><tbody></tbody></table></div>
<div class="foot">Reads <code>build/GW4E69/report.json</code>, written by every <code>ninja</code> run. Refreshes every 20 s. Read-only.</div>
</div>
<script>
const fmt=n=>n.toLocaleString();const pct=(a,b)=>b?100*a/b:0;
const ago=t=>{const s=Math.max(0,Date.now()/1000-t);if(s<60)return Math.round(s)+'s ago';if(s<3600)return Math.round(s/60)+' min ago';if(s<86400)return (s/3600).toFixed(1)+' h ago';return (s/86400).toFixed(1)+' d ago'};
function card(k,v,sub,meter){return `<div class="card${meter==='big'?' big':''}"><div class="k">${k}</div><div class="v">${v}${sub?` <small>${sub}</small>`:''}</div>${typeof meter==='number'?`<div class="meter"><i style="width:${Math.max(0,Math.min(100,meter))}%"></i></div>`:''}${meter==='big'&&arguments[4]!=null?`<div class="meter"><i style="width:${Math.max(0.2,arguments[4])}%"></i></div>`:''}</div>`}
async function load(){
 const d=await (await fetch('/api/progress')).json();const r=d.report,s=d.symbols;
 if(!r){document.getElementById('sub').textContent='No report.json yet - run ninja once.';return}
 const codeP=pct(r.matched_code,r.total_code),fnP=pct(r.matched_functions,r.total_functions),namedP=pct(s.named,s.functions),dataP=pct(r.matched_data,r.total_data);
 document.getElementById('sub').innerHTML=`branch <code>${d.branch}</code>${d.dirty?' (uncommitted changes)':''} &middot; last build ${ago(r.mtime)} &middot; ${new Date(r.mtime*1000).toLocaleString()}`;
 document.getElementById('cards').innerHTML=
  card('Code matched',codeP.toFixed(3)+'%',`${fmt(r.matched_code)} / ${fmt(r.total_code)} bytes`,'big',codeP)+
  card('Functions matched',fmt(r.matched_functions),`of ${fmt(r.total_functions)} (${fnP.toFixed(2)}%)`,fnP)+
  card('Functions named',fmt(s.named),`of ${fmt(s.functions)} (${namedP.toFixed(1)}%)`,namedP)+
  card('Fuzzy code match',r.fuzzy_code.toFixed(2)+'%','incl. partial functions',r.fuzzy_code)+
  card('Data matched',dataP.toFixed(2)+'%',`${fmt(r.matched_data)} / ${fmt(r.total_data)} bytes`,dataP)+
  card('Source files',r.units.filter(u=>u.functions).length,`${r.units.filter(u=>u.linked&&u.functions).length} fully linked`);
 const tb=document.querySelector('#units tbody');tb.innerHTML=r.units.filter(u=>u.functions).sort((a,b)=>b.matched_code-a.matched_code||a.name.localeCompare(b.name)).map(u=>{const p=u.matched_code_percent;return `<tr><td>${u.name.replace(/^main\//,'')}</td><td class="n">${u.matched_functions} / ${u.functions}</td><td class="n">${fmt(u.code)}</td><td class="n"><span class="pct ${p>=100?'ok':p>0?'part':''}">${p.toFixed(1)}%</span></td></tr>`}).join('');
 document.querySelector('#commits tbody').innerHTML=d.commits.map(c=>`<tr><td><code>${c.hash}</code></td><td>${c.subject}</td><td class="n" style="color:var(--dim)">${ago(+c.time)}</td></tr>`).join('');
 const h=d.history;if(h.length>1){const W=1000,H=180,max=Math.max(...h.map(x=>x.matched_code),1);const xs=i=>20+(W-40)*i/(h.length-1),ys=v=>H-16-(H-32)*v/max;
  const pts=h.map((x,i)=>`${xs(i)},${ys(x.matched_code)}`).join(' ');
  document.getElementById('chart').innerHTML=`<polyline points="${pts}" fill="none" stroke="#4ade80" stroke-width="2.5"/>`+h.map((x,i)=>`<circle cx="${xs(i)}" cy="${ys(x.matched_code)}" r="3.5" fill="#4ade80"><title>${x.commit} ${x.subject}\n${fmt(x.matched_code)} bytes, ${fmt(x.named)} named</title></circle>`).join('');
  document.getElementById('chartlbl').textContent=`${h.length} commits, ${fmt(h[0].matched_code)} -> ${fmt(h[h.length-1].matched_code)} matched bytes. Hover a point for the commit.`}
}
load();setInterval(load,20000);
</script></body></html>'''


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def _send(self, code, body, ctype):
        self.send_response(code)
        self.send_header('Content-Type', ctype)
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path.startswith('/api/progress'):
            try:
                body = json.dumps(progress()).encode()
                self._send(200, body, 'application/json')
            except Exception as e:
                self._send(500, json.dumps({'error': str(e)}).encode(), 'application/json')
        elif self.path == '/' or self.path.startswith('/index'):
            self._send(200, PAGE.encode(), 'text/html; charset=utf-8')
        else:
            self._send(404, b'not found', 'text/plain')


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--host', default='0.0.0.0')
    ap.add_argument('--port', type=int, default=8420)
    a = ap.parse_args()
    srv = ThreadingHTTPServer((a.host, a.port), H)
    print(f'tw2004 dashboard on http://{a.host}:{a.port}/  (root {ROOT})', flush=True)
    srv.serve_forever()
