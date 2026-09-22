#!/usr/bin/env python3
"""
Walk a CTRL container (.hog / .gcb / .ngc) and decompress its SHOC objects.

    python tools/research/ctrl_dump.py <file> [--out DIR]

The chunk walk and the 'Rdat' decoder are transcriptions of src/UStream.c (UStream_Decompress,
UStream_Copy, UStream_Fill), which were read from the game's own loader. With --out, every
completed SHOC object is written as <index>_<type>_<name>.bin. Without it, a listing.
"""
import argparse, os, struct, sys

def decompress(src, out, size):
    """UStream_Decompress: two-byte big-endian command words until `size` bytes are produced."""
    s = 0; end = len(out) + size
    while len(out) < end:
        code = (src[s] << 8) | src[s + 1]; s += 2
        if (code & 0x8800) != 0x8800:
            n = (code >> 12) & 7
            if n == 7:
                n += src[s]; s += 1
            n += 3
            dist = code & 0xFFF
            base = len(out) - dist
            if (code & 0x8000) == 0:
                for i in range(n):                      # forward copy, may overlap
                    out.append(out[base + i])
            else:
                for i in range(n):                      # mirrored copy
                    out.append(out[base + 2 - i])
        else:
            n = (code >> 12) & 7
            if n == 0:
                n = code & 0x7FF
                out += src[s:s + n]; s += n
            else:
                dist = n | ((code >> 5) & 0x38)
                out += bytes([out[len(out) - dist]]) * ((code & 0xFF) + 3)
    return s

def chunks(data):
    """Yield (tag, offset, length). FILL pads to the next 8 KiB block and may lack a length."""
    pos = 0; n = len(data)
    while pos + 4 <= n:
        tag = data[pos:pos + 4]
        if tag == b'FILL':
            nxt = (pos + 0x2000) & ~0x1FFF
            yield 'FILL', pos, nxt - pos; pos = nxt; continue
        if pos + 8 > n: break
        ln = struct.unpack('>I', data[pos + 4:pos + 8])[0]
        try: t = tag.decode('ascii')
        except UnicodeDecodeError: break
        if ln < 16 or pos + ln > n: break
        yield t, pos, ln
        pos += ln

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('file'); ap.add_argument('--out'); ap.add_argument('-v', action='store_true')
    a = ap.parse_args()
    data = open(a.file, 'rb').read()
    if a.out: os.makedirs(a.out, exist_ok=True)
    cur = None; objects = []; stats = {'chunks': 0, 'sdat': 0, 'rdat': 0, 'raw': 0, 'packed': 0}
    for tag, off, ln in chunks(data):
        stats['chunks'] += 1
        if tag != 'SHOC': continue
        sub = data[off + 0x10:off + 0x14]
        if sub == b'SHDR':
            hdr = data[off:off + ln]
            typ = hdr[0x18:0x1C].decode('latin1'); size = struct.unpack('>I', hdr[0x20:0x24])[0]
            name_off = 0x3C + struct.unpack('>I', hdr[0x34:0x38])[0]
            name = hdr[0x3C:name_off].split(b'\0')[0].decode('latin1') if name_off <= len(hdr) else ''
            cur = {'type': typ, 'size': size, 'name': name, 'data': bytearray(), 'hdr': hdr, 'off': off}
            objects.append(cur)
        elif sub == b'SDAT' and cur is not None:
            cur['data'] += data[off + 0x40:off + ln]; stats['sdat'] += 1; stats['raw'] += ln - 0x40
        elif sub == b'Rdat' and cur is not None:
            usize = struct.unpack('>I', data[off + 0x40:off + 0x44])[0]
            start = len(cur['data'])
            decompress(data[off + 0x44:off + ln], cur['data'], usize); stats['rdat'] += 1; stats['packed'] += ln - 0x44
            # The last command may run a byte or two past the declared size (the game's loop only
            # checks at command boundaries and its buffer is padded); the next piece overwrites it.
            del cur['data'][start + usize:]
    print(os.path.basename(a.file), 'chunks', stats['chunks'], 'objects', len(objects), 'SDAT', stats['sdat'], 'Rdat', stats['rdat'],
          'bytes raw', stats['raw'], 'packed', stats['packed'])
    bad = 0
    for i, o in enumerate(objects):
        # a few objects declare 1-2 bytes less than their pieces sum to; the game allocates the
        # declared size rounded up, so the extra bytes are harmless. Short is the real error.
        ok = len(o['data']) >= o['size']
        del o['data'][o['size']:]
        bad += not ok
        if a.v or not ok:
            print('  %3d %s %-28s declared %8d got %8d %s' % (i, o['type'], o['name'][:28], o['size'], len(o['data']), 'OK' if ok else 'MISMATCH'))
        if a.out:
            safe = ''.join(c if c.isalnum() or c in '._-' else '_' for c in o['name'])[:40]
            open(os.path.join(a.out, '%03d_%s_%s.bin' % (i, o['type'].strip(), safe)), 'wb').write(o['data'])
    print('  size mismatches:', bad)

if __name__ == '__main__':
    main()
