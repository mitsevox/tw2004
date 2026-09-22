TXG texture groups (`txf ` objects)
===================================

Status: **decoded and verified** by eye (`tools/research/txg_export.py` exports every texture
of a hole as PNG; grass, cliffs and trees come out right in all three formats seen).
Read from the files, not yet from code; the loader side is `LLTex.c` (`0x8000FC10`).

Layout (big-endian, tagged sections with a u32 length after the tag):

    'TXG ' u32 version (02 01 00 00)
    'HEAD' len 8:    u8 ?, u8 ?, u16 count, u32 0
    'TXHE' len:      count * 0x50-byte entries
    'CLHE' len:      palette table (length 0 in every file looked at so far)
    'TXDA' len:      texture image data; entry offsets are relative to the start of this data

Entry (0x50 bytes):

    0x00 u32  id            small numbers such as 0x70dd; likely a name hash or asset id
    0x04 u32  hash          second hash
    0x08 u32  offset        into TXDA
    0x0C u16  size / 16     image size in 16-byte units
    0x0E u16  palette       0xFFFF = none
    0x10 3 x { u32 flags; u32 offset; u16 size/16; u16 palette }   mip levels 1..3
    0x34 u32  ?             0x01000001 when mips are present, else 0
    0x38 u16  width
    0x3A u16  height
    0x3C u16  0xFFFF
    0x3E u16  index
    0x40 u8   GX format     0 I4, 1 I8, 5 RGB5A3, 14 CMPR seen; the enum is GXTexFmt
    0x41 u8   mip count     1 or 4
    0x42 u8   has mips
    0x43 u8   log2 width
    0x44 u8   log2 height
    0x45 u8   ?             0..7, varies per texture (wrap / filter?)

Images are in the GameCube's native tiled layouts, so they can be handed to `GXInitTexObj`
unchanged; `txg_export.py` untiles them. Bytes-per-pixel checks out against the format byte
for all 155 textures of `01_peb/Hole_01` (CMPR 4, I8 8, RGB5A3 16).

    python tools/research/txg_export.py <hole.hog | file.gcb | txf.bin> <out_dir>
