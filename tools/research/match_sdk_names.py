#!/usr/bin/env python3
"""
Name SDK / runtime functions by matching them against another decomp project's split objects.

Usage: python tools/research/match_sdk_names.py <other_project_obj_dir> <main.dol> <symbols.txt>
  e.g. python tools/research/match_sdk_names.py C:/dev/prime/build/GM8E01_00/obj orig/main.dol config/GW4E69/symbols.txt

The other project's split objects carry relocation records, so only the bytes the linker fills in
are masked; everything else must be identical. A name is proposed only when the match is unique
in both directions. Two self-checks are printed: agreement with names dtk already assigned, and
consistency of call targets between matched functions. Writes proposals to sdk_names.json; it does
not edit symbols.txt. Only top-level folders dolphin/, runtime/ and MetroTRK/ are considered.
"""
import sys
import struct,glob,re,collections,json,os
OBJ,DOL,SYMS=sys.argv[1:4]
def parse(p):
    d=open(p,'rb').read()
    shoff=struct.unpack('>I',d[0x20:0x24])[0]; shentsize,shnum,shstrndx=struct.unpack('>HHH',d[0x2E:0x34])
    sh=[struct.unpack('>10I',d[shoff+i*shentsize:shoff+i*shentsize+40]) for i in range(shnum)]
    shstr=sh[shstrndx]
    nm=lambda off,tab: d[tab[4]+off:d.index(b'\0',tab[4]+off)].decode(errors='replace')
    secname=[nm(s[0],shstr) for s in sh]
    symtab=[s for s in sh if s[1]==2]
    if not symtab: return []
    symtab=symtab[0]; strtab=sh[symtab[6]]
    syms=[]
    for i in range(symtab[5]//16):
        n,val,sz,info,oth,shndx=struct.unpack('>IIIBBH',d[symtab[4]+i*16:symtab[4]+i*16+16])
        syms.append((nm(n,strtab),val,sz,info,shndx))
    relocs=collections.defaultdict(list)   # target section index -> [(offset,type,symname)]
    for i,s in enumerate(sh):
        if s[1]==4:  # RELA
            for j in range(s[5]//12):
                off,inf,add=struct.unpack('>IIi',d[s[4]+j*12:s[4]+j*12+12])
                relocs[s[7]].append((off,inf&0xff,syms[inf>>8][0]))
    out=[]
    for name,val,sz,info,shndx in syms:
        if info&0xf==2 and sz>=8 and shndx<len(sh) and secname[shndx] in ('.text','.init'):
            sec=sh[shndx]; code=bytearray(d[sec[4]+val:sec[4]+val+sz]); maskb=bytearray(b'\xff'*sz); calls=[]
            for off,typ,sname in relocs.get(shndx,[]):
                if val<=off<val+sz+2:
                    o=off-val
                    if typ==10:   m=(0x03FFFFFC,o); calls.append((o,sname))      # REL24
                    elif typ==11: m=(0x0000FFFC,o)                                # REL14
                    elif typ in (4,5,6): m=(0x0000FFFF,o-2)                       # ADDR16_LO/HI/HA (offset points at the imm half)
                    elif typ==109: m=(0x001FFFFF,o)                               # EMB_SDA21
                    elif typ==1:  m=(0xFFFFFFFF,o)
                    else: m=(0xFFFFFFFF,o)
                    msk,o=m
                    if 0<=o<=sz-4:
                        cur=struct.unpack('>I',maskb[o:o+4])[0]&~msk&0xFFFFFFFF
                        maskb[o:o+4]=struct.pack('>I',cur)
            out.append((name,bytes(code),bytes(maskb),calls,info>>4))
    return out
# TW
td=open(DOL,'rb').read()
toffs=struct.unpack('>7I',td[0:0x1C]); tad=struct.unpack('>7I',td[0x48:0x64]); tsz=struct.unpack('>7I',td[0x90:0xAC])
def tw(addr,size):
    for o,a,s in zip(toffs,tad,tsz):
        if s and a<=addr and addr+size<=a+s: return td[o+addr-a:o+addr-a+size]
twf={}
lines=open(SYMS,newline='').read().split('\n')
for ln in lines:
    m=re.match(r'^(\S+) = \.(text|init):0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)',ln)
    if m: twf[int(m.group(3),16)]=(m.group(1),int(m.group(4),16))
by_size=collections.defaultdict(list)
for a,(n,s) in twf.items(): by_size[s].append(a)
cands=[]
for p in glob.glob(OBJ+'/**/*.o',recursive=True):
    rel=p.replace(chr(92),'/').split('/obj/')[1]
    top=rel.split('/')[0].lower()
    if top not in ('dolphin','runtime','metrotrk'): continue
    for name,code,maskb,calls,bind in parse(p): cands.append((rel,name,code,maskb,calls,bind))
print(len(cands),'Prime SDK/runtime functions with exact relocation info')
def apply(b,m): return bytes(x&y for x,y in zip(b,m))
hits=collections.defaultdict(list)   # tw addr -> [(rel,name)]
where={}
for rel,name,code,maskb,calls,bind in cands:
    want=apply(code,maskb); found=[a for a in by_size.get(len(code),[]) if apply(tw(a,len(code)),maskb)==want]
    where[(rel,name)]=found
    for a in found: hits[a].append((rel,name,calls,bind))
unique={a:v[0] for a,v in hits.items() if len(v)==1 and len(where[(v[0][0],v[0][1])])==1}
print('TW functions with exactly one Prime match (and that Prime function matches only there):',len(unique))
# validation 1: agreement with names dtk already assigned
agree=dis=0; disl=[]
for a,(rel,name,calls,bind) in unique.items():
    cur=twf[a][0]
    if not cur.startswith('fn_'):
        if cur==name: agree+=1
        else: dis+=1; disl.append((hex(a),cur,name))
print('check against dtk auto-names: agree',agree,'disagree',dis,disl[:8])
# validation 2: call targets must agree
name_at={a:v[1] for a,v in unique.items()}; bad=0; okc=0
for a,(rel,name,calls,bind) in unique.items():
    code=tw(a,twf[a][1])
    for o,sname in calls:
        if o<0 or o>len(code)-4: continue
        w=struct.unpack('>I',code[o:o+4])[0]; off=w&0x03FFFFFC
        if off&0x02000000: off-=0x04000000
        t=a+o+off
        if t in name_at:
            if name_at[t]==sname: okc+=1
            else: bad+=1
print('call-target cross-check: consistent',okc,'inconsistent',bad)
new={a:v for a,v in unique.items() if twf[a][0].startswith('fn_')}
print('new names available:',len(new))
json.dump({hex(a):[v[0],v[1],v[3]] for a,v in new.items()},open('sdk_names.json','w'),indent=0)
libs=collections.Counter(v[0].rsplit('/',1)[0] for v in new.values()); print(libs.most_common())
