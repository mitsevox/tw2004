# EA builds and decomps that could help the last ~115 functions (research, 2026-09-25)

Web search and fetch only, nothing downloaded. Already covered in `docs/tw2004-notes.md` and not
repeated here: TW06 Xbox PDB and PS2 map, TW07 PS3 DWARF, the TW2003 Xbox prototypes (release
builds, no symbols), 007 Agent Under Fire and Everything or Nothing on GameCube (masked byte
match), and a first look at NFSU/NFSMW/Sims 2 for shared compiled code.

"Verified" below means I read it in the source myself (symbol files, split files, line tables,
listings). "Inferred" means I reasoned it out and did not see it directly.

## 1. NFS Most Wanted GameCube decomp: EA's names for the whole MAD decoder (rcmp_mad_codec)

- **What:** `dbalatoni13/nfsmw` (https://github.com/dbalatoni13/nfsmw, on decomp.dev), a
  decomp of NFS Most Wanted for GameCube (GOWE69, 2005, EA Canada/Black Box). The retail
  `NFSMWRELEASE.ELF` has full DWARF. The repo commits the files built from it: `config/GOWE69/symbols.txt`
  (named functions and globals with sizes), `config/GOWE69/splits.txt` (EA's source paths),
  `symbols/file_names.txt` and `symbols/debug_lines.txt` (every address mapped to a source line,
  40 MB). The full DWARF dump (`mw_dwarfdump.nothpp`) is not in the repo; it is generated from the ELF.
- **Why it matters:** NFSMW links EA's shared `rcmp` package (`D:/env/egami/rcmp/dev/source/
  decoder/cmn/`): `rcmp_mad_codec.cpp` plus **`maddec.cpp`, `maddeca.cpp`, `madidct.cpp`**. Those are
  the same file names TW06's PDB gives for `Golf\rcmp\` (maddec.c, maddeca.c, madidct.c), and the
  same code as our `src/rcmp_mad_codec.c`. Its functions come **in the same order, with the same
  signatures**, as our unnamed ones (verified from symbols.txt; the signatures come from the
  mangled names):

  | ours | NFSMW name (file) | NFSMW signature / size | our size |
  |---|---|---|---|
  | fn_800B769C | `madinit` (maddec) | `(void)` 0x2BC | 0x6E4 |
  | fn_800B7D80 | `discardbits` (maddec) | `(int)` 0x58 | 0x74 |
  | fn_800B7DF4 | `getdelta` | `(void)` 0x44 | 0x44 |
  | fn_800B7E38 | `dcblock` | `(int*, int)` 0x5C | |
  | fn_800B8064 | `getluma` | `(const u8*, int, int*, int)` 0x98 | |
  | fn_800B8180 | `getchroma` | `(const u8*, int, int*, int)` 0x98 | |
  | fn_800B829C | `setluma` | `(const int*, u8*, int)` 0x120 | |
  | fn_800B83DC | `setchroma` | `(const int*, u8*, int)` 0xA0 | |
  | fn_800B8528 | `MAD_initdecode` | `(const u16*, int, int)` 0x140 | |
  | **fn_800B8618** | **`MAD_decodemacroblock`** | `(const u8*, const u8*, const u8*, u8*, u8*, u8*, int)` 0x338 | 0x36C |
  | fn_800B8984 | (inlined in NFSMW: realcore `getm.inl`, a little-endian multi-byte read) | | |
  | fn_800B8A04 | (not in NFSMW) | | |
  | fn_800B8A2C | `discardbits` (maddeca, a second static copy) | `(int)` 0x58 | |
  | fn_800B8AA0 | `madvlcdecode` (maddeca, extern "C") | 0x16C | |
  | fn_800B8C54 | `IdctColumn` (madidct) | 0x278 | |
  | fn_800B8DF4 | `IdctRow` | 0x204 | |
  | fn_800B8F28 | `idctcompute` | 0x164 | |

  Globals (verified names, inferred mapping): `maddataptr` (our lbl_802821B8, the next coded byte),
  `madshiftreg` (lbl_802821B4, the bit buffer), `madbitcount` (lbl_802821B0, bits left),
  `madvlctbl1..4` (0x800/0x400/0x400/0x100 bytes: our code tables lbl_801F7A58/7258/7658/7158),
  `madquant`. The wrapper class names (`MAD_CODEC_INTERNAL::DecodeChunk/GetFrame/CreateKorM/
  CreateE`, `MAD_FRAME`) line up with our `Code800B90F4.c` MadDecoder (inferred).
- **Line tables** (`symbols/debug_lines.txt`, verified): `madinit` is maddec.cpp lines 346-425,
  and none of its lines belong to an inline or header. So **EA's 2005 madinit calls no inline
  helper**, which is evidence against a real helper behind our `fn_800B769C_Read` fake match
  (though TW2004's C version is two years older). `discardbits` and `MAD_initdecode` inline
  realcore `getm.inl`, lines 33 and 45 (our fn_800B8984's job). `MAD_decodemacroblock`, `madvlcdecode`,
  `IdctRow` and `idctcompute` have no line records (possibly built without -g).
- **Limits:** NFSMW was built with **SN ProDG (GCC 2.95), as C++**, not CodeWarrior, so its code
  and register use say nothing about how mwcc allocates registers. What it can give us is names,
  signatures, file boundaries and source line order. No NFSMW MAD function is decompiled yet (all
  NonMatching).
- **Also free, no download:** the TW06 Xbox PDB we already hold should have S_GPROC32 entries for
  maddec.c/maddeca.c/madidct.c in `golf_unity` (`pdb_modules.json`), and being a debug build it
  probably has locals too. That is our own TW06 evidence and a stronger name source than NFSMW,
  so check it first.
- **Download?** No for the repo files. The full NFSMW DWARF (parameter and local names, inline
  trees for maddec) needs the NFSMW GameCube disc: the owner's call.
- **Confidence:** verified (symbols, splits, line tables read directly). The pairing is
  structural (order, arity, types, sizes), so it still has to be confirmed against our code.
- **Naming rule:** NFSMW is EA's own symbol text, but it is not one of the evidence types in
  docs/style.md (EA's text in this game, TW06/TW07). The owner/orchestrator decides whether a
  name confirmed by TW06's PDB, with NFSMW as backup, is enough.

## 2. 007 Everything or Nothing, PS2 USA, unstripped (never compared)

- **What:** debugging.games lists `[PS2] 007 - Everything or Nothing (USA) [SLUS-20751]
  [2003-12-12].7z` (25 MB), plus the JP release and two demos (OPSM2 40 SCED-51535, OPS2M
  SCED-52119). URL: https://debugging.games/PlayStation%202
- **Why:** it is EA Redwood Shores, built three months after TW2004, with full symbols. The
  earlier GameCube check (docs/tw2004-notes.md) could only use EoN's GameCube `boot.elf`, which
  has 822 symbols and is mostly SDK (RetroReversing's list; TCRF says the GC disc's boot.dlf lists
  `C:\BOND09\Assets\action.dol`). **EoN's main game code has never been compared.** If EoN
  shares TW's Redwood Shores libraries (Legacy `LL*`/`U*`, UI Studio `UIS*`, `hlaud*`,
  `skalib`/`mtalib`), it would give names and signatures for the biggest open cluster (UIS: 31
  functions).
- **Limits:** a MIPS build (CodeWarrior or SN for PS2, unknown), so it gives names and structure
  only. Nothing public says whether EoN shares TW's engine (I searched and found nothing either way).
- **Download?** Yes, owner approval needed. **Confidence:** the listing is verified; its
  usefulness is a guess.

## 3. The TW2004 PS2 demo on OPSM2 40 is probably stripped

The notes above list the TW2004 demo on OPSM2 Demo 40 (SCED-51535) as unchecked.
debugging.games pulled two debug-bearing demos from that same disc: Everything or Nothing, and
Freaky Flyers "(DWARF)". It lists no Tiger Woods one. So the TW2004 demo ELF probably has no
symbols (inferred from the archive's selection, not checked).

## 4. More Tiger Woods prototypes we have not used (low marginal value)

Hidden Palace (verified pages): TW07 **Xbox** Jul 6 2006 ("debug information (PDB files)" and
map files, from an XDK drive) and Aug 2 2006; TW06 PS2 Aug 8 2005 "Beta 2 1F"; TW08 and TW09
(X360) prototypes. debugging.games: `[MAC] Tiger Woods PGA Tour 2005 [2005-07-14]`, which is
Aspyr's port of the Headgate PC version, a different codebase. TW07 PS3 DWARF and the TW06 PDB
already cover this generation, so these add little. All need downloads.
https://hiddenpalace.org/Tiger_Woods_PGA_Tour_07_(Jul_6,_2006_prototype)

## 5. Other EA decomps and symbol sets: checked, nothing shared with TW's own libraries

- **decomp.dev** (https://decomp.dev/projects, verified): the EA projects are TW2004 (ours),
  NFSMW (above), NFS High Stakes (PSX), The Sims 2 (GC, `natebag/Sims2DECOMP`) and The Sims 2:
  Castaway (Wii, `HaydnTrigg/Castaway`). None of them is by Redwood Shores' Tiger team.
- **The Sims 2 GC** (G4ZE69; ProDG GCC 2.95, C++; the retail disc has the ELF and
  `u2_ngc_debug.map`): I compared its 36,913 symbols with our library names (LL*, UIS*, hlaud*,
  rcmp/mad, skalib, mtalib, U*, SharedFileIO, TagFile, Trax). **No overlap.** It does carry EA
  shared code (Apt, EAString, EAHeap, EAudio), which TW2004 does not use.
- **Sims 2 Castaway Wii:** mwcc GC/3.0a3 with DWARF, C++ unity files (`SKU2_Code/...`), a
  different engine.
- **GoldenEye: Rogue Agent GC** (`ant0-blase/goldeneye-rag-decomp`, mwcc GC/2.7, EA LA): only
  boot code so far, and its public symbols.txt is the boot DOL (2,112 entries, all SDK or
  unnamed). Not useful yet. The game ELF (GE2RDVD.ELF, 20,654 symbols) is on the disc.
- **EA GameCube ELFs/maps with symbols** (RetroReversing list, verified,
  https://www.retroreversing.com/gamecube-debug-symbols): MOH Frontline / Rising Sun / European
  Assault, GoldenEye RA, Harry Potter GoF, NFSU/NFSU2 (SN ProDG), Freedom Fighters (IO
  Interactive), and map files for FIFA 2005 and UEFA CL 04-05 (`fifa_z.map`). debugging.games also
  has FIFA Football 2004 GC (all regions). These are other studios' engines. They might share the
  EA packages (rcmp, snd, realmemcard) but not TW's own libraries. FIFA 2004 GC (2003, same year)
  is the likeliest place for a CodeWarrior-built rcmp MAD decoder. Unverified; download needed.
- **GitHub code search** for `UISScreen`, `LLFont`, `UIStudio`, `hlaud`, `madvlcdecode`: the
  only hits are in our own repo. Nobody has published UI Studio or LL.
- **PS2 unstripped list** (https://www.retroreversing.com/ps2-unstripped): EA entries are
  FIFA 2001-2003, The Sims (2003), MOH Rising Sun and NFS MW. No Tiger Woods, no Redwood Shores
  title except that Nightfire is listed (developed by Eurocom).

## Bottom line

- **Only one find helps directly:** NFSMW's public symbols, splits and line tables give EA's
  names and file boundaries for all 17 functions in `rcmp_mad_codec`, including the two open
  blockers there: fn_800B769C = `madinit`, fn_800B8618 = `MAD_decodemacroblock`. It does not fix
  their register allocation, because it was built by a different compiler. The line table weakly
  suggests `madinit` had no helper inline, at least in 2005. Cross-check with the TW06 PDB we
  already have, which needs no download.
- **One untested lead for the big UIS/LL cluster:** 007 Everything or Nothing PS2 (Redwood
  Shores, Dec 2003, unstripped, 25 MB on debugging.games). It needs the owner's OK to download,
  and it may share nothing.
- **Nothing else found:** no public decomp or symbols anywhere for UI Studio, LL, hlaud,
  skalib/mtalib or TW's game code. No TW2004 build on any platform with symbols is known.
