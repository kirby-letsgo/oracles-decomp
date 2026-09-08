import struct
import sys
import zlib


def read_png(path):
    d = open(path, "rb").read()
    assert d[:8] == b"\x89PNG\r\n\x1a\n"
    pos = 8
    idat = b""
    plte = b""
    while pos < len(d):
        (ln,) = struct.unpack(">I", d[pos:pos + 4])
        typ = d[pos + 4:pos + 8]
        body = d[pos + 8:pos + 8 + ln]
        pos += 12 + ln
        if typ == b"IHDR":
            w, h, bd, ct = struct.unpack(">IIBB", body[:10])
        elif typ == b"PLTE":
            plte = body
        elif typ == b"IDAT":
            idat += body
    raw = zlib.decompress(idat)
    channels = 3 if ct == 2 else 1
    bpp = max(1, bd * channels // 8)
    stride = (w * bd * channels + 7) // 8
    prev = bytes(stride)
    out = []
    p = 0
    for _ in range(h):
        f = raw[p]
        line = bytearray(raw[p + 1:p + 1 + stride])
        p += 1 + stride
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            c = prev[i - bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                pr = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                line[i] = (line[i] + pr) & 255
        prev = bytes(line)
        for x in range(w):
            if ct == 3:
                bit = x * bd
                idx = (line[bit // 8] >> (8 - bd - bit % 8)) & ((1 << bd) - 1)
                out.append(plte[idx * 3:idx * 3 + 3])
            else:
                out.append(bytes(line[x * 3:x * 3 + 3]))
    return w, h, b"".join(out)


w, h, rgb = read_png(sys.argv[1])
assert (w, h) == (160, 144), (w, h)
open(sys.argv[2], "wb").write(rgb)
print(len(rgb), "bytes")
