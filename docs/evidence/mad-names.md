# MAD video decoder names

`mad-names.tsv` lists the functions and globals of `rcmp_mad_codec.c`, EA's MAD movie decoder, with
the EA name two independent sources give at the same position:

1. the TW06 Xbox PDB (`Legacy_unity.obj`: the source file from its line table, parameter names from
   its symbols);
2. the Need for Speed: Most Wanted GameCube decomp (`dbalatoni13/nfsmw`, `config/GOWE69/symbols.txt`;
   its mangled names give the parameter types).

Both agree on 14 functions and 6 globals.

Not in the TSV, because only one source has them or the pairing isn't settled:

- `fn_800B8A2C` is `discardbits` in NFSMW's `maddeca.cpp` (a second static copy). TW06 has no
  second copy.
- `fn_800B8984` (a multi-byte little-endian read, realcore's `getm.inl`) and `fn_800B8A04` have no
  name in either source.
- `madvlctbl2` / `madvlctbl3` (0x400 bytes each) against our `lbl_801F7258` / `lbl_801F7658`: by
  address order tbl2 = 7258, but this unit's `.bss` is laid out in reverse (its three `.sbss`
  globals come out reversed against NFSMW's order), which would make tbl2 = 7658. The uses in
  `madvlcdecode` settle it.
- `idctinput` and `idctprescale` (`s32[64]` in both sources) aren't located in our file yet.

Types from TW06: `maddataptr` is `const u16*`, `MAD_initdecode`'s first parameter is
`const u16* src`, and `madvlctbl4` is `volatile s32[64]`. Parameter names: `getluma(src, stride,
dest, correction)`, `MAD_initdecode(src, motion, quality)`, `MAD_decodemacroblock(src_y, src_cb,
src_cr, dest_y, dest_cb, dest_cr, width, flags)`, `idctcompute(dest, stride)`, `dcblock(dest,
stride)`, `discardbits(bits)`.
