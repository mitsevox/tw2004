#!/usr/bin/env python3
"""
Compute per-section data ranges for SDK units whose data symbols are all known.

Part of the SDK import pipeline (run from the repo root, in this order):
  1. sdk_units.py       - which of the other project's SDK units match this binary completely
  2. name_data.py       - name data symbols by aligning references; --apply writes symbols.txt
  3. sdk_data_units.py  - per-unit data ranges for the units whose data is fully named
Then add the units to splits.txt / configure.py (see docs/decomp-notes.md, "Bringing in another
project's SDK source"). Paths to the other project and scratch files are at the top of each script.
"""
import os; os.makedirs("C:/dev/tw2004/build/sdk_import",exist_ok=True)
import struct,glob,re,collections,json,sys
exec(open('C:/dev/tw2004/tools/research/sdk/sdk_units.py').read().split("tw={}")[0])   # parse()
OBJ='C:/dev/prime/build/GM8E01_00/obj'
def parse_syms(p):
    d=open(p,'rb').read()
    shoff=struct.unpack('>I',d[0x20:0x24])[0]; shentsize,shnum,shstrndx=struct.unpack('>HHH',d[0x2E:0x34])
    sh=[struct.unpack('>10I',d[shoff+i*shentsize:shoff+i*shentsize+40]) for i in range(shnum)]
    shstr=sh[shstrndx]; nm=lambda off,tab: d[tab[4]+off:d.index(b'\0',tab[4]+off)].decode(errors='replace')
    secname=[nm(s[0],shstr) for s in sh]
    symtab=[s for s in sh if s[1]==2][0]; strtab=sh[symtab[6]]
    out=[]
    for i in range(symtab[5]//16):
        n,val,sz,info,oth,shndx=struct.unpack('>IIIBBH',d[symtab[4]+i*16:symtab[4]+i*16+16])
        if shndx<len(sh) and info&0xf in(1,2) and sz: out.append((nm(n,strtab),secname[shndx],val,sz,info&0xf))
    return out
# TW symbols: name -> (section, addr, size)
tw={}; sec_of={}
for line in open('C:/dev/tw2004/config/GW4E69/symbols.txt'):
    m=re.match(r'^(\S+) = \.(\w+):0x([0-9A-Fa-f]+); // type:(\w+) size:0x([0-9A-Fa-f]+)',line)
    if m: tw.setdefault(m.group(1),[]).append((m.group(2),int(m.group(3),16),int(m.group(5),16)))
units=json.load(open('C:/dev/tw2004/build/sdk_import/sdk_units.json'))
anon=json.load(open('C:/dev/tw2004/build/sdk_import/anon_map.json'))   # tw lbl -> [unit rel, '@N']
anon_by_unit=collections.defaultdict(dict)
for lbl,(rel,n) in anon.items(): anon_by_unit[rel][n]=lbl
twaddr={}
for line in open('C:/dev/tw2004/config/GW4E69/symbols.txt'):
    m=re.match(r'^(\S+) = \.(\w+):0x([0-9A-Fa-f]+); // type:(\w+) size:0x([0-9A-Fa-f]+)',line)
    if m: twaddr[m.group(1)]=(m.group(2),int(m.group(3),16),int(m.group(5),16))
res=[]
for u in units:
    if u['status']!='FULL' or not u['data']: continue
    syms=parse_syms(OBJ+'/'+u['unit'])
    ranges=collections.defaultdict(list); missing=[]
    for name,sec,val,sz,kind in syms:
        if kind!=1: continue
        c=None if name.startswith('@') else tw.get(name)
        if name.startswith('@') and name in anon_by_unit.get(u['unit'],{}):
            lbl=anon_by_unit[u['unit']][name]
            if lbl in twaddr: c=[twaddr[lbl]]
        if not c or len(c)!=1: missing.append((sec,name,sz)); continue
        tsec,addr,tsz=c[0]
        ranges[tsec].append((addr,addr+max(tsz,sz),name))
    want=sum(u['data'].values()); got=sum(hi-lo for v in ranges.values() for lo,hi,_ in v)
    res.append((u['unit'],u['lo'],u['hi'],dict((s,(min(a for a,_,_ in v),max(b for _,b,_ in v),len(v))) for s,v in ranges.items()),missing,want,got))
ok=[r for r in res if not r[4]]
print(len(res),'data-carrying FULL units;',len(ok),'have every data symbol named in TW')
for unit,lo,hi,r,miss,want,got in res:
    print(f'{"OK " if not miss else "   "}{unit:36} text 0x{lo:08X}-0x{hi:08X}  data:{ {k:(hex(a),hex(b),n) for k,(a,b,n) in r.items()} }  bytes {got}/{want}', ('MISSING '+str(miss[:3])) if miss else '')
json.dump([{'unit':u,'lo':lo,'hi':hi,'data':{k:list(v) for k,v in r.items()}} for u,lo,hi,r,miss,want,got in ok],open('C:/dev/tw2004/build/sdk_import/sdk_data_ok.json','w'),indent=0)
