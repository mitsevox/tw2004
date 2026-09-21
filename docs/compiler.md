Compiler identification
=======================

What built `main.dol`, how we know, and what is still open. Every claim below was
tested against the original binary; guesses are labelled as guesses.

Summary
-------

| Question | Answer | Confidence |
|----------|--------|------------|
| Language | C (not C++) | High |
| Main compiler | CodeWarrior for GameCube, mwcc 2.4.7 family (GC/2.0 - GC/2.6) | High |
| Working baseline | **GC/2.5** (2.4.7 build 105, Feb 20 2003) | Medium - GC/2.0 not ruled out |
| Optimization | `-O4` (`,p` vs `,s` not yet determined) | High |
| Processor flags | `-proc gekko`, `-fp hardware` | High |
| Second compiler | GCC, unoptimized, for one 38 KiB block (see below) | High |
| Dolphin SDK | Sep 5 2002 build, CARD patched Apr 2 2003 | Certain (version strings) |

`main.dol` is a mixed-compiler binary
--------------------------------------

| Address range | Contents | Built with |
|---------------|----------|------------|
| `0x80005520` - `0x8012FFFF` (approx.) | EA game and engine code | CodeWarrior |
| `0x80130000` - `0x8016C717` (approx.) | Dolphin SDK, MSL C library, MetroTRK | Nintendo / Metrowerks prebuilt |
| `0x8016C718` - `0x80175F54` | 124 functions, 38 KiB | **GCC, no optimization** |

The GCC block uses a frame pointer in `r31` (`mr r31, r1`) and spills every local to the
stack after each statement. It corresponds to the four source paths left in assert strings:

- `../../../Source/Common/Checksum/ChecksumCRC32.c`
- `../../../Source/Common/SharedFileIO/SharedFileIO.c`
- `../../../Source/Common/TagFile/TagFile.c`
- `../../../Source/NGC/SharedFileIO/llSharedFileIO.c`

Guess: a separately built EA shared library. SN Systems ProDG (GCC based) is the likely
toolchain; the exact version has not been tested. This block is the file-reading layer,
so it matters to the asset track (`docs/formats`).

Evidence for the CodeWarrior version
------------------------------------

Compilers tested are the `GC/*` builds from the standard decomp compiler pack.

| Test | Method | Rules out |
|------|--------|-----------|
| Function prologue style | 4,299 of 4,310 non-leaf EA functions use `stwu` first with LR saved at the top of the frame | GC/1.2.5 and older |
| `fn_8000B70C` (linked list find, 15 instrs) | Natural C, compiled unchanged on every version | GC/3.0 (signed vs unsigned null compare) |
| `fn_80008304` (Vec3 copy, 7 instrs) | Natural C, compiled unchanged; exact byte match on 2.0 - 2.7 at `-O4` only | GC/1.3.2 and older, GC/3.0, `-O2` and lower |
| Compiler build dates | `mwcceppc.exe -version` | GC/2.7 (built Jul 22 2004, after the game shipped) |

Build dates: GC/2.0 = Sep 16 2002, GC/2.5 = Feb 20 2003, GC/2.6 = Jul 14 2003. The game
shipped September 2003, so GC/2.6 is possible but unlikely (weeks before gold).

How the candidates differ from each other
-----------------------------------------

1,765 functions of known C (Dolphin SDK, MSL, MusyX, zlib sources) were compiled with each
candidate and compared function by function:

- GC/2.5, GC/2.6 and GC/2.7 produced identical output for all 1,765.
- GC/2.0 differed from that group in 31 functions (1.8%). The common factor is byte-sized
  values being masked or packed (`u8` masked then used as an index, bit-field packing).
- GC/1.3.2 differed from GC/2.0 in 37 functions, mostly the ordering of float loads and stores.
- Switch statements, 64-bit integer math and a bounds-check loop did not separate any candidate.

**If a function with byte masking or bit packing sticks one or two instructions short,
try GC/2.0 before rewriting the C.** Candidate for settling it: `fn_800AACBC`.

Things that looked like evidence but were not
---------------------------------------------

- **MSL library test.** `mem_funcs.c` from another project's MSL source matched the binary
  only when compiled with GC/3.0 (built Dec 2005). That compiler cannot have been used for a
  2003 game. Most likely explanation: this game links a different MSL source revision, and
  older source plus a newer compiler happened to produce the same bytes. The MSL library was
  prebuilt by Metrowerks anyway, so it says little about the compiler EA used.
- **SDK version stamp `(0x2301)`.** That is the compiler Nintendo used to build the SDK.
- **Paired-single instructions.** Most are the `psq_st` / `psq_l` float register saves that
  every version from GC/1.3.2 up emits under `-proc gekko`. The 210 real paired-single math
  instructions are in 79 small functions and are presumably hand-written assembly.

Open questions
--------------

- GC/2.0 vs GC/2.5.
- `-O4,p` vs `-O4,s`. Loops separate them, the two matched functions do not contain one that does.
- `-inline` mode, `-str` pooling, small data thresholds.
- Exact GCC / ProDG version and flags for the `0x8016C718` block.
- Eleven EA-region functions use the older prologue style; not yet examined.
