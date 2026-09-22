CTRL stream container (`.hog`, `.gcb`, `.ngc`)
================================================

Status: **chunk layout, object framing and the `Rdat` compression are known from the code**
(`src/UStream.c`, read from the game's loader at `fn_8000D4F0`). The extractor
`tools/research/ctrl_dump.py` reproduces every object on disc 1 to its declared size.
What is *inside* each object type is the next layer and is not decoded yet.

Chunk layout
------------

A file is a sequence of chunks. Each chunk: a 4-character tag, a big-endian u32 length that
includes the 16-byte header, 8 bytes of zero, then payload.

| Tag | Role |
|-----|------|
| `CTRL` | file header, always first, length 0x18; its 8-byte payload is `SYNC 00000000` |
| `SHOC` | a piece of an object (see below) |
| `SONO` | a piece of a sound object (`shdr` / `samp` sub-types; handled by the audio code) |
| `PADD` | padding, zero payload |
| `FILL` | padding to the next 8 KiB boundary; when fewer than 8 bytes remain the tag stands alone |

Chunks never cross an 8 KiB (0x2000) boundary: the file is laid out in DVD-sized blocks and
the loader reads 0x6000 bytes at a time (`fn_8000D4F0` loops while the read offset is below
0x6000).

The loader's tag switch also accepts `DSPM`, `MPG2`, `VAGM`, `SWVR` and `XADP` (movie and audio
streams for GameCube, PS2 and Xbox); only `DSPM` occurs on this disc (`.ngc` movies).

Objects (`SHOC`)
----------------

An object is delivered as one `SHDR` chunk followed by data chunks. The sub-type is the u32 at
payload offset 0 (file offset +0x10 of the chunk):

`SHDR` (0x40 bytes of chunk in total), fields at chunk offsets:

    +0x18  char[4]  type      e.g. 'ter ', 'txf ', 'Cact'
    +0x20  u32      size      decompressed size of the object
    +0x34  u32      name len  (the object name, if any, is at +0x3C; +0x34 bytes long)
    +0x3C  char[]   name

`SDAT`: raw bytes; payload starts at chunk offset 0x40.

`Rdat`: compressed bytes. u32 at 0x40 = decompressed size of this piece, stream from 0x44.
Pieces are appended in order until the object reaches its declared size; the last piece may
overrun by one or two bytes (the loader's buffer is padded; the extractor truncates).

`Rdat` compression (UStream_Decompress)
---------------------------------------

Two-byte big-endian command words. With bits 0x8800 both set:

    1000 1lll llll llll   literal run: copy the next (low 11 bits) bytes verbatim
    1nnn 1ooo cccc cccc   fill: nnn != 0; repeat the byte at dst - (ooo<<3 | nnn), (c + 3) times

Otherwise a back-reference of length `lll + 3` (bits 12-14; if the field is 7, add the next
byte) at distance `dddd dddddddd` (low 12 bits) behind the output:

    0lll dddd dddd dddd   forward copy (may overlap)
    1lll dddd dddd dddd   mirrored copy: bytes are read backwards starting at dst - d + 2

The mirrored mode is the unusual part and is why this is not a stock LZ variant.

Object types on disc 1 (325 files)
----------------------------------

| Type | Count | Total | Where |
|------|------:|------:|-------|
| `ter ` | 169 | 472 MiB | one per hole: terrain, starts `OBG ` / `ARRA` |
| `txf ` | 190 | 221 MiB | textures, starts `TXG ` / `HEAD` / `TXHE` |
| `tgd ` | 169 | 182 MiB | one per hole, terrain grid / ground data |
| `gras` | 33 | 16 MiB | grass, only on some holes |
| `TEO ` | 782 | 7 MiB | several per hole (tee objects?) |
| `Cact`, `Cnet`, `CAMC` | 7641 / 1618 / 157 | small | per hole: actors, nets, camera |
| `CHR `, `SAC ` | 64 / 34 | 18 / 24 MiB | `Data/chars`, `Data/charsac`: golfers |
| `RPNS`, `RLst` | 1 each per file | tiny | resource list: names such as `ORT\DATA\SESSION_GC.HDR` |
| `BALF` | 1 | small | `Data/Fend/FEnd.gcb` - **ball flight?** (hypotheses 1, 2, 5) |
| `BIO ` | 1 | small | `Data/Fend/FEnd.gcb` - golfer bios (hypothesis 4) |
| `PGAc` `PGAn` `PGAp` `PGAt` `PGST` `PLY ` `PLYs` `TRAX` `rcrd` `stat` | 1 each | small | `loadonce.gcb`: tour, players, records |
| `CAMV` `LITE` `MPCS` `eagm` `CR_A` `CR_S` `txf2` `EASI` `TRXT` | 1 each | small | `Data/Fend/FEnd.gcb` (front end) |
| `LEGL` `GRPS` `TXFS` `FONS` `DATS` `MCB ` `MCI ` | 1-2 | small | `startup.gcb` |
| `load` | 23 | 2 MiB | `Data/load` (loading screens) |

Extractor
---------

    python tools/research/ctrl_dump.py <file.hog> -v            # list objects
    python tools/research/ctrl_dump.py <file.hog> --out DIR     # write each object as .bin

Next: the per-type formats. `txf ` (`TXG `) and `ter ` (`OBG `) have their own tagged
sub-structure; `BALF` and `BIO ` are the small ones the gameplay hypotheses care about.
