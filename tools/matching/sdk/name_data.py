#!/usr/bin/env python3
"""
Name data symbols (and stragglers) by aligning symbol references function by function.

Part of the SDK import pipeline (run from the repo root, in this order):
  1. sdk_units.py       - which of the other project's SDK units match this binary completely
  2. name_data.py       - name data symbols by aligning references; --apply writes symbols.txt
  3. sdk_data_units.py  - per-unit data ranges for the units whose data is fully named
Then add the units to splits.txt / configure.py (see docs/decomp-notes.md, "Bringing in another
project's SDK source"). Paths to the other project and scratch files are at the top of each script.
"""
import os; os.makedirs("C:/dev/tw2004/build/sdk_import",exist_ok=True)
import re,glob,os,subprocess,collections,json,sys
PRIME_OBJ='C:/dev/prime/build/GM8E01_00/obj'; TW_ASM='C:/dev/tw2004/build/GW4E69/asm'; DTK='C:/dev/prime/build/tools/dtk.exe'
SYMS='C:/dev/tw2004/config/GW4E69/symbols.txt'
REF=re.compile(r'(?:\bbl\s+(?P<call>[A-Za-z_@.$][\w@.$]*)\b)|(?:"(?P<qsym>[^"]+)"|(?P<sym>[A-Za-z_@.$][\w@.$]*))@(?:ha|l|sda21|sda2|h)\b')
def refs_by_fn(text):
    out=collections.OrderedDict(); cur=None
    for line in text.splitlines():
        m=re.match(r'\.fn (\S+),',line)
        if m: cur=m.group(1); out[cur]=[]; continue
        if cur is None or line.startswith('.endfn'): continue
        code=re.sub(r'/\*.*?\*/','',line)
        for m in REF.finditer(code):
            t=m.group('call') or m.group('qsym') or m.group('sym')
            if t.startswith('.L_'): continue
            out[cur].append(t)
    return out
# TW: all asm
tw=collections.OrderedDict()
for f in glob.glob(TW_ASM+'/**/*.s',recursive=True):
    for k,v in refs_by_fn(open(f,errors='replace').read()).items(): tw[k]=v
# Prime: disasm each object
prime={}
os.makedirs('C:/dev/tw2004/build/sdk_import/pobj',exist_ok=True)
for p in glob.glob(PRIME_OBJ+'/**/*.o',recursive=True):
    rel=p.replace(chr(92),'/').split('/obj/')[1]
    if rel.split('/')[0].lower() not in('dolphin','runtime','metrotrk'): continue
    out='C:/dev/tw2004/build/sdk_import/pobj/'+rel.replace('/','__')[:-2]+'.s'
    if not os.path.exists(out): subprocess.run([DTK,'elf','disasm',p,out],capture_output=True)
    for k,v in refs_by_fn(open(out,errors='replace').read()).items(): prime[k]=(rel,v)
# align
pairs=collections.defaultdict(collections.Counter); mis=0; aligned=0
for fn,(rel,prefs) in prime.items():
    if fn not in tw: continue
    trefs=tw[fn]
    if len(trefs)!=len(prefs): mis+=1; continue
    aligned+=1
    for t,p in zip(trefs,prefs):
        if t==p: continue
        pairs[t][(rel if p.startswith('@') else '')+'|'+p]+=1
print(f'{aligned} functions aligned, {mis} with different reference counts (skipped)')
consistent={t:c.most_common(1)[0][0].split('|',1)[1] for t,c in pairs.items() if len(c)==1}
anon={t:c.most_common(1)[0][0].split('|',1) for t,c in pairs.items() if len(c)==1 and c.most_common(1)[0][0].split('|',1)[1].startswith('@')}
json.dump(anon,open('C:/dev/tw2004/build/sdk_import/anon_map.json','w'),indent=0)
conflict={t:dict(c) for t,c in pairs.items() if len(c)>1}
print(len(consistent),'labels map to exactly one name;',len(conflict),'conflicts')
for t,c in list(conflict.items())[:8]: print('  conflict',t,c)
# only rename lbl_ / fn_ targets, and only to names that don't already exist
existing=set(); lines=open(SYMS,newline='').read()
nl='\r\n' if '\r\n' in lines else '\n'; lines=lines.split(nl)
for l in lines:
    if ' = ' in l: existing.add(l.split(' = ')[0])
ren={t:p for t,p in consistent.items() if re.match(r'^(lbl|fn)_[0-9A-F]{8}$',t) and p not in existing and re.match(r'^[A-Za-z_]\w*$',p)}
dupe_names=collections.Counter(ren.values()); ren={t:p for t,p in ren.items() if dupe_names[p]==1}
print(len(ren),'renames to apply (data:',sum(1 for t in ren if t.startswith('lbl_')),'functions:',sum(1 for t in ren if t.startswith('fn_')),')')
json.dump(ren,open('C:/dev/tw2004/build/sdk_import/data_renames.json','w'),indent=0)
if '--apply' in sys.argv:
    n=0
    for i,l in enumerate(lines):
        m=re.match(r'^(\S+) = (.*)$',l)
        if m and m.group(1) in ren: lines[i]=ren[m.group(1)]+' = '+m.group(2); n+=1
    open(SYMS,'w',newline='').write(nl.join(lines)); print('applied',n)
