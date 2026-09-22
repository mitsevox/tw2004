CTRL stream container (`.hog`, `.gcb`, `.ngc`)
================================================

Status: **chunk layout observed** from the files; **meaning of the payloads inferred**, not yet
confirmed from code. The reader in the binary is the switch at `fn_8000D4F0` (see below).

Observed layout
---------------

The file is a sequence of chunks. Each chunk starts with a 4-character tag and a big-endian
u32 length that includes the 16-byte chunk header:

    char tag[4]
    u32  uLength        whole chunk, header included
    u32  zero
    u32  zero
    u8   payload[uLength - 16]

Top-level tags seen in every `.hog` / `.gcb` (first chunk is always `CTRL`, length 0x18):

| Tag | Seen as | Notes |
|-----|---------|-------|
| `CTRL` | first chunk, `CTRL 00000018 00000000 00000000 SYNC 00000000` | file header; the `SYNC` word is part of its 8-byte payload |
| `SHOC` | most chunks | an object; payload begins with a sub-tag `SHDR` (header) or `SDAT` (data) |
| `SONO` | many chunks in `.hog` and `.gcb` | same `SHDR` / `SDAT` sub-structure as `SHOC` |
| `PADD` | occasional | padding, zero payload |
| `FILL` | before every 0x2000 boundary | padding to the next 8 KiB block; when fewer than 8 bytes remain the tag stands alone with no length field |

Chunks never cross an 8 KiB boundary (0x2000): the data is laid out in DVD-friendly blocks, and
big objects are split into consecutive 8192/8188-byte `SHOC` or `SONO` pieces.

`SHDR` payload example (first `SHOC` of `01_peb/Hole_01/hole.hog`):

    SHDR 00000002  RPNS 00000001  00000004 E0311800  FFFFFFFF FFFFFFFF FFFFFFFF 00000001 00000001 00000000

`RPNS` and `RLst` appear as sub-tags of headers; `Rdat` and `shdr` (lower case) also occur.
`SDAT` payloads contain 32-bit values that read as **little-endian** numbers (`81321400` =
0x00143281, `01000000` = 1) and a build path fragment `...ORT\DATA\SESSION_GC.`: the payload
looks like a serialised memory image written by a PC tool, with the `R...` sub-tags describing
pointers to fix up (relocation lists). This is inferred; the reader will settle it.

The reader in the binary
------------------------

`fn_8000D4F0` (0x4B4 bytes, CodeWarrior code just after the `UStream.c` asserts at
`0x8000CA58`) reads the next chunk and switches on its tag. The case list is the complete set
of tags the engine understands, including ones that never appear on this disc:

| Constant | Tag | Meaning (best guess) |
|----------|-----|----------------------|
| 0x4354524C | `CTRL` | control / file header |
| 0x53484F43 | `SHOC` | object container ("shape object container"?) |
| 0x534F4E4F | `SONO` | second object container type |
| 0x50414444 | `PADD` | padding |
| 0x46494C4C | `FILL` | block padding |
| 0x4453504D | `DSPM` | GameCube DSP-ADPCM movie / audio (the `.ngc` movies start with it) |
| 0x4D504732 | `MPG2` | MPEG-2 video (PS2 build) |
| 0x5641474D | `VAGM` | PS2 VAG audio |
| 0x53575652 | `SWVR` | EA streaming wave |
| 0x58414450 | `XADP` | Xbox ADPCM audio |

So this one reader is EA's cross-platform streaming loader; the `.hog` course files, the `.gcb`
UI/global files and the `.ngc` movies all go through it. Related functions: `fn_8000CBFC`
(called first, with 1: probably "get the current stream"), `fn_8000DB7C` (builds the `RPNS`
constant and uses the linked-list helpers at `0x8000B508`), `fn_8000DA94` (calls the reader).

Decompiling `fn_8000D4F0` and `fn_8000DB7C` is the path to a reliable extractor.
