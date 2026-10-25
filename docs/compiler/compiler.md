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
| Inlining | No automatic inlining (`-inline auto` is wrong) | Medium - one file tested |
| Second compiler | SN ProDG (GCC 2.95), `-O0`, for one 38 KiB block; `ProDG/3.5` baseline | High - 8 functions matched; the five pack builds are indistinguishable at -O0 |
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

A separately built EA shared library. Confirmed SN Systems ProDG: `ChecksumCRC32.c` (8 functions)
matches byte for byte under every ProDG build in the compiler pack at `-O0`, so the exact version
cannot be told apart from unoptimized code; the project uses `ProDG/3.5` (2001). Driven by
`tools/build/prodg/prodgcc.py` (the `ngccc` front end needs an installed `sn.ini` and is bypassed). This block is the file-reading layer,
so it matters to the asset track (`docs/game/formats`).

Evidence for the CodeWarrior version
------------------------------------

Compilers tested are the `GC/*` builds from the standard decomp compiler pack.

| Test | Method | Rules out |
|------|--------|-----------|
| Function prologue style | 4,299 of 4,310 non-leaf EA functions use `stwu` first with LR saved at the top of the frame | GC/1.2.5 and older |
| `Resources_Get` (linked list find, 15 instrs) | Natural C, compiled unchanged on every version | GC/3.0 (signed vs unsigned null compare) |
| `fn_80008304` (Vec3 copy, 7 instrs) | Natural C, compiled unchanged; exact byte match on 2.0 - 2.7 at `-O4` only | GC/1.3.2 and older, GC/3.0, `-O2` and lower |
| Compiler build dates | `mwcceppc.exe -version` | GC/2.7 (built Jul 22 2004, after the game shipped) |

| `fn_80007D74` (frustum cull test, 215 instrs) | Float math, branches, eight live float registers; exact match on GC/2.5 `-O4,p` | Nothing new, but a gross compiler or flag error would have shown here. Not re-tested on GC/2.0 |

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
try GC/2.0 before rewriting the C.** `OnModADSRVol` (bit-fields) was tried as a tie-breaker: it
matches, but identically on GC/2.0, 2.5 and 2.6, so it does not separate them. GameRound
(63 functions, including the 99.67% near-miss `GM_BuildRandom18`) compiles byte-identically under
GC/1.3.2, 2.0, 2.0p1, 2.5, 2.6 and 2.7, so its stubborn register shuffle is not a version effect.

**MSL, the runtime and MetroTRK are built with GC/2.5, like the game (2026-10-25).** Every
CodeWarrior file was rebuilt with each version from GC/1.0 to GC/2.7, with its own flags:

| Library | Files | Matches with |
|---------|------:|--------------|
| EA game | 244 | GC/2.5, 2.6, 2.7 (2.0 fails 4) |
| Dolphin SDK | 97 | GC/1.2.5n only (no other version builds more than 37) |
| MetroTRK | 27 | GC/1.3.2 through 2.7 |
| MSL and runtime | 56 | GC/2.5 through 2.7, all but mbstring.c with the group's flags |

The per-file mix of GC/1.3, 1.3.2 and 2.5 that MSL used before was a patch over wrong source
revisions. printf.c and ansi_fp.c match only on GC/2.5 or newer, and every file pinned to an older
version also matches on GC/2.5 once its source is right:
- s_ldexp.c needed the older revision's floating-point classification values (`FP_NAN` 1 through
  `FP_SUBNORMAL` 5). `extern/sdk/libc/math.h` used to pick them by compiler version, so only GC/1.3
  produced them.
- e_pow.c is the Sun fdlibm source (`extern/sdk/runtime/e_pow.c`). The eCos-derived copy from
  another decomp takes the address of `one` and its `math.h` emits sqrtf's constants, which adds
  24 bytes to `.sdata2`.
- mbstring.c matches without automatic inlining (`-inline noauto`): mbtowc calls is_utf8_complete.

MetroTRK cannot tell GC/1.3.2 from 2.5 by its code. It ships prebuilt in the same CodeWarrior
install as MSL, so it uses the same compiler. GameMode4Menu (game code) scored identically on
GC/2.0 and GC/2.5.

Things that looked like evidence but were not
---------------------------------------------

- **MSL library test.** `mem_funcs.c` from one project's MSL source matched the binary only
  when compiled with GC/3.0 (built Dec 2005), which cannot have built a 2003 game. The copy the
  build uses now (from another decomp) matches on GC/2.5: the first copy was a different source
  revision.
- **SDK version stamp `(0x2301)`.** That is the compiler Nintendo used to build the SDK.
- **Paired-single instructions.** Most are the `psq_st` / `psq_l` float register saves that
  every version from GC/1.3.2 up emits under `-proc gekko`. The 210 real paired-single math
  instructions are in 79 small functions and are presumably hand-written assembly.

Inlining
--------

`fn_80007CE8` calls the small helper `fn_80007C80`, which sits directly before it in the same
file. With `-inline auto` the compiler pastes the helper into the caller and the function
cannot match. With `-inline off`, `-inline smart` or `-inline deferred` it matches. None of the
trivial camera getters at `0x80008320` are inlined into their caller either. The project uses
`-inline smart`. Not yet determined: whether EA used `-inline deferred` (which would mean the
functions appear in the binary in the reverse of their source order).

Current flags
-------------

`configure.py` `cflags_base`: `-nodefaults -proc gekko -align powerpc -enum int -fp hardware
-Cpp_exceptions off -O4,p -inline smart -RTTI off -fp_contract on -str reuse -multibyte`.
All 28 matched functions were built with exactly these. Confirmed by test: `-proc gekko`,
`-fp hardware`, `-O4`, no auto-inlining. The rest are inherited from the project template and
have simply not caused a mismatch yet.

Open questions
--------------

- GC/2.0 vs GC/2.5. Parked on purpose: several tie-breakers failed to separate them, and a wrong
  choice will surface as a byte-masking function that will not match.
- `-O4,p` vs `-O4,s`. Loops separate them, the two matched functions do not contain one that does.
- `-inline deferred` or not, `-str` pooling, small data thresholds.
- Eleven EA-region functions use the older prologue style; not yet examined.

Game-code flags learned from UStream.c (2026-09-22)
---------------------------------------------------

Two more flags are needed for EA's own code and are now in `cflags_base`:

- `-common on`: uninitialised globals are COMMON symbols. Without it CodeWarrior lays a file's
  arrays out itself and addresses them from one base register (`lis/addi` of a `.bss` section
  symbol plus offsets); the original loads each array's address separately.
- `-use_lmw_stmw on`: multi-register prologues use `stmw`/`lmw`, not `_savegpr_NN` calls.

The camera/culling units match with both flags, so they are safe for all game code.
