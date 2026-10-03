#!/usr/bin/env python3
"""
Compare another decomp project's split SDK objects against this binary, unit by unit.

Part of the SDK import pipeline (run from the repo root, in this order):
  1. sdk_units.py       - which of the other project's SDK units match this binary completely
  2. name_data.py       - name data symbols by aligning references; --apply writes symbols.txt
  3. sdk_data_units.py  - per-unit data ranges for the units whose data is fully named
Then add the units to splits.txt / configure.py (see docs/decomp-notes.md, "Bringing in another
project's SDK source"). Paths to the other project and scratch files are at the top of each script.
"""
import os; os.makedirs("C:/dev/tw2004/build/sdk_import",exist_ok=True)
import struct,glob,re,collections,os,json
OBJ='C:/dev/prime/build/GM8E01_00/obj'
def parse(p):
    d=open(p,'rb').read()
    shoff=struct.unpack('>I',d[0x20:0x24])[0]; shentsize,shnum,shstrndx=struct.unpack('>HHH',d[0x2E:0x34])
    sh=[struct.unpack('>10I',d[shoff+i*shentsize:shoff+i*shentsize+40]) for i in range(shnum)]
    shstr=sh[shstrndx]; nm=lambda off,tab: d[tab[4]+off:d.index(b'\0',tab[4]+off)].decode(errors='replace')
    secname=[nm(s[0],shstr) for s in sh]
    symtab=[s for s in sh if s[1]==2]
    if not symtab: return [],{}
    symtab=symtab[0]; strtab=sh[symtab[6]]
    funcs=[]; data=collections.Counter()
    for i in range(symtab[5]//16):
        n,val,sz,info,oth,shndx=struct.unpack('>IIIBBH',d[symtab[4]+i*16:symtab[4]+i*16+16])
        if shndx>=len(sh): continue
        sn=secname[shndx]; name=nm(n,strtab)
        if info&0xf==2 and sn in('.text','.init'): funcs.append((name,sz))
        elif info&0xf==1 and sz: data[sn]+=sz
    return funcs,dict(data)
tw={}
for line in open('C:/dev/tw2004/config/GW4E69/symbols.txt'):
    m=re.match(r'^(\S+) = \.(text|init):0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)',line)
    if m: tw.setdefault(m.group(1),[]).append((int(m.group(3),16),int(m.group(4),16)))
allf=sorted((a,s) for v in tw.values() for a,s in v)
def nfuncs_in(lo,hi): return sum(1 for a,s in allf if lo<=a<hi)
rows=[]
for p in sorted(glob.glob(OBJ+'/**/*.o',recursive=True)):
    rel=p.replace(chr(92),'/').split('/obj/')[1]
    if rel.split('/')[0].lower() not in('dolphin','runtime','metrotrk'): continue
    funcs,data=parse(p)
    if not funcs: continue
    hits=[]
    for name,sz in funcs:
        c=tw.get(name)
        if c and len(c)==1 and c[0][1]==sz: hits.append((c[0][0],sz,name))
    hits.sort()
    if not hits: rows.append((rel,len(funcs),0,None,None,data,'')); continue
    lo=hits[0][0]; hi=hits[-1][0]+hits[-1][1]
    inrange=nfuncs_in(lo,hi)
    status='FULL' if len(hits)==len(funcs) and inrange==len(funcs) else ('CONTIG' if inrange==len(hits) else 'GAPPY')
    rows.append((rel,len(funcs),len(hits),lo,hi,data,status))
full=[r for r in rows if r[6]=='FULL']; contig=[r for r in rows if r[6]=='CONTIG']
print(f'{len(rows)} Prime SDK/runtime units; FULL (every function matched, nothing foreign inside) = {len(full)}, partial-but-contiguous = {len(contig)}')
print(f'FULL units cover {sum(r[4]-r[3] for r in full):,} code bytes in {sum(r[2] for r in full)} functions; {sum(1 for r in full if not r[5])} of them have NO data sections')
print('\nFULL units:')
for rel,n,h,lo,hi,data,st in sorted(full,key=lambda r:r[3]): print(f'  0x{lo:08X}-0x{hi:08X} {h:3} fn {hi-lo:6} B  data={data or "-"}  {rel}')
print('\npartial (matched/total, range):')
for rel,n,h,lo,hi,data,st in sorted(contig,key=lambda r:r[3]): print(f'  0x{lo:08X}-0x{hi:08X} {h:3}/{n:<3}  {st}  {rel}')
json.dump([{'unit':r[0],'total':r[1],'hits':r[2],'lo':r[3],'hi':r[4],'data':r[5],'status':r[6]} for r in rows],open('C:/dev/tw2004/build/sdk_import/sdk_units.json','w'),indent=0)
