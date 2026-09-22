#!/usr/bin/env python3
"""
Export the textures of a TXG texture group ('txf ' object) as PNG files.

    python tools/research/txg_export.py <file.hog|file.gcb|txf.bin> <out_dir>

For a .hog/.gcb the 'txf ' objects are extracted first (ctrl_dump.py). Texture data is in the
GameCube's native GX tiled layouts; the format byte in each table entry is the GX enum
(0 I4, 1 I8, 2 IA4, 3 IA8, 4 RGB565, 5 RGB5A3, 6 RGBA8, 8 C4, 9 C8, 14 CMPR). Only the top
mip level is written. No third-party modules: PNGs are written with zlib directly.

TXG layout (from the files; see docs/formats/txg-textures.md):
    'TXG ' u32 version
    'HEAD' u32 8      u8 ?, u8 ?, u16 count, u32 0
    'TXHE' u32 size   count entries of 0x50 bytes
    'CLHE' u32 size   palettes (none seen yet)
    'TXDA' u32 size   texture data; entry offsets are relative to its start
"""
import os, struct, sys, zlib

FMT_NAMES = {0: 'I4', 1: 'I8', 2: 'IA4', 3: 'IA8', 4: 'RGB565', 5: 'RGB5A3', 6: 'RGBA8', 8: 'C4', 9: 'C8', 10: 'C14X2', 14: 'CMPR'}

def png(path, w, h, rgba):
    raw = b''.join(b'\0' + rgba[y * w * 4:(y + 1) * w * 4] for y in range(h))
    def chunk(t, b): return struct.pack('>I', len(b)) + t + b + struct.pack('>I', zlib.crc32(t + b) & 0xFFFFFFFF)
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b''))

def rgb565(v): return ((v >> 11) & 31) * 255 // 31, ((v >> 5) & 63) * 255 // 63, (v & 31) * 255 // 31

def rgb5a3(v):
    if v & 0x8000:
        return ((v >> 10) & 31) * 255 // 31, ((v >> 5) & 31) * 255 // 31, (v & 31) * 255 // 31, 255
    return ((v >> 8) & 15) * 17, ((v >> 4) & 15) * 17, (v & 15) * 17, ((v >> 12) & 7) * 255 // 7

def decode(fmt, w, h, data):
    """Return RGBA bytes (w*h*4) for one GX texture image."""
    out = bytearray(w * h * 4)
    def put(x, y, r, g, b, a=255):
        if x < w and y < h:
            i = (y * w + x) * 4; out[i] = r; out[i + 1] = g; out[i + 2] = b; out[i + 3] = a
    p = 0
    if fmt in (0, 8):        # I4 / C4: 8x8 tiles, 4 bits per texel
        for ty in range(0, h, 8):
            for tx in range(0, w, 8):
                for y in range(8):
                    for x in range(0, 8, 2):
                        v = data[p]; p += 1
                        for k, n in ((0, v >> 4), (1, v & 15)):
                            put(tx + x + k, ty + y, n * 17, n * 17, n * 17)
    elif fmt in (1, 9):      # I8 / C8: 8x4 tiles
        for ty in range(0, h, 4):
            for tx in range(0, w, 8):
                for y in range(4):
                    for x in range(8):
                        v = data[p]; p += 1; put(tx + x, ty + y, v, v, v)
    elif fmt == 2:           # IA4: 8x4 tiles
        for ty in range(0, h, 4):
            for tx in range(0, w, 8):
                for y in range(4):
                    for x in range(8):
                        v = data[p]; p += 1; i = (v & 15) * 17; put(tx + x, ty + y, i, i, i, (v >> 4) * 17)
    elif fmt == 3:           # IA8: 4x4 tiles
        for ty in range(0, h, 4):
            for tx in range(0, w, 4):
                for y in range(4):
                    for x in range(4):
                        a, i = data[p], data[p + 1]; p += 2; put(tx + x, ty + y, i, i, i, a)
    elif fmt in (4, 5):      # RGB565 / RGB5A3: 4x4 tiles of u16
        for ty in range(0, h, 4):
            for tx in range(0, w, 4):
                for y in range(4):
                    for x in range(4):
                        v = (data[p] << 8) | data[p + 1]; p += 2
                        if fmt == 4: r, g, b = rgb565(v); put(tx + x, ty + y, r, g, b)
                        else: put(tx + x, ty + y, *rgb5a3(v))
    elif fmt == 6:           # RGBA8: 4x4 tiles, 32 bytes AR then 32 bytes GB
        for ty in range(0, h, 4):
            for tx in range(0, w, 4):
                for y in range(4):
                    for x in range(4):
                        k = y * 4 + x
                        a, r = data[p + k * 2], data[p + k * 2 + 1]; g, b = data[p + 32 + k * 2], data[p + 32 + k * 2 + 1]
                        put(tx + x, ty + y, r, g, b, a)
                p += 64
    elif fmt == 14:          # CMPR: 8x8 tiles = 2x2 DXT1 blocks, big-endian colours
        for ty in range(0, h, 8):
            for tx in range(0, w, 8):
                for by in (0, 4):
                    for bx in (0, 4):
                        c0 = (data[p] << 8) | data[p + 1]; c1 = (data[p + 2] << 8) | data[p + 3]
                        r0, g0, b0 = rgb565(c0); r1, g1, b1 = rgb565(c1)
                        if c0 > c1:
                            pal = [(r0, g0, b0, 255), (r1, g1, b1, 255), ((2 * r0 + r1) // 3, (2 * g0 + g1) // 3, (2 * b0 + b1) // 3, 255), ((r0 + 2 * r1) // 3, (g0 + 2 * g1) // 3, (b0 + 2 * b1) // 3, 255)]
                        else:
                            pal = [(r0, g0, b0, 255), (r1, g1, b1, 255), ((r0 + r1) // 2, (g0 + g1) // 2, (b0 + b1) // 2, 255), (0, 0, 0, 0)]
                        for y in range(4):
                            row = data[p + 4 + y]
                            for x in range(4):
                                put(tx + bx + x, ty + by + y, *pal[(row >> (6 - 2 * x)) & 3])
                        p += 8
    else:
        return None
    return bytes(out)

def parse_txg(d):
    assert d[:4] == b'TXG ', 'not a TXG object'
    pos = 8; sections = {}
    while pos + 8 <= len(d):
        tag = d[pos:pos + 4]; ln = struct.unpack('>I', d[pos + 4:pos + 8])[0]
        sections[tag] = (pos + 8, ln); pos += 8 + ln
    hoff, _ = sections[b'HEAD']; count = struct.unpack('>H', d[hoff + 2:hoff + 4])[0]
    toff, _ = sections[b'TXHE']; doff, dlen = sections[b'TXDA']
    entries = []
    for i in range(count):
        e = d[toff + i * 0x50:toff + (i + 1) * 0x50]
        w, h = struct.unpack('>HH', e[0x38:0x3C])
        entries.append({'index': i, 'id': struct.unpack('>I', e[0:4])[0], 'hash': struct.unpack('>I', e[4:8])[0],
                        'offset': struct.unpack('>I', e[8:12])[0], 'size': struct.unpack('>H', e[12:14])[0] * 16,
                        'w': w, 'h': h, 'fmt': e[0x40], 'mips': e[0x41], 'log2w': e[0x43], 'log2h': e[0x44], 'flags45': e[0x45]})
    return entries, doff, dlen

def export(d, out_dir, prefix=''):
    entries, doff, dlen = parse_txg(d)
    os.makedirs(out_dir, exist_ok=True); n = 0
    for e in entries:
        data = d[doff + e['offset']:doff + e['offset'] + e['size']]
        rgba = decode(e['fmt'], e['w'], e['h'], data)
        name = '%s%03d_%08x_%s_%dx%d' % (prefix, e['index'], e['id'], FMT_NAMES.get(e['fmt'], str(e['fmt'])), e['w'], e['h'])
        if rgba is None:
            print('  skip', name, '(format not handled)'); continue
        png(os.path.join(out_dir, name + '.png'), e['w'], e['h'], rgba); n += 1
    return n, len(entries)

def main():
    src, out = sys.argv[1], sys.argv[2]
    d = open(src, 'rb').read()
    if d[:4] == b'TXG ':
        n, t = export(d, out); print('wrote', n, 'of', t)
        return
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import ctrl_dump
    cur = None; k = 0
    for tag, off, ln in ctrl_dump.chunks(d):
        if tag != 'SHOC': continue
        sub = d[off + 0x10:off + 0x14]
        if sub == b'SHDR':
            if cur and cur['type'] == 'txf ':
                n, t = export(bytes(cur['data']), out, '%02d_' % k); print('txf object', k, 'wrote', n, 'of', t); k += 1
            cur = {'type': d[off + 0x18:off + 0x1C].decode('latin1'), 'data': bytearray()}
        elif sub == b'SDAT' and cur: cur['data'] += d[off + 0x40:off + ln]
        elif sub == b'Rdat' and cur:
            usize = struct.unpack('>I', d[off + 0x40:off + 0x44])[0]; start = len(cur['data'])
            ctrl_dump.decompress(d[off + 0x44:off + ln], cur['data'], usize); del cur['data'][start + usize:]
    if cur and cur['type'] == 'txf ':
        n, t = export(bytes(cur['data']), out, '%02d_' % k); print('txf object', k, 'wrote', n, 'of', t)

if __name__ == '__main__':
    main()
