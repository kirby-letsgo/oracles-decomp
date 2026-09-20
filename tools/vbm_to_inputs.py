"""Convert a VBA-rr .vbm movie to the runner's .inputs format (same bit layout for bits 0-7)."""
import struct, sys
src, dst = sys.argv[1], sys.argv[2]
d = open(src, 'rb').read()
assert d[:4] == b'VBM\x1a', 'not a VBM'
nframes = struct.unpack_from('<I', d, 0x0c)[0]
start_flags, ctrl_flags, sys_flags, opt_flags = d[0x14], d[0x15], d[0x16], d[0x17]
off = struct.unpack_from('<I', d, 0x3c)[0]
print(f'frames={nframes} start=0x{start_flags:02x} ctrl=0x{ctrl_flags:02x} sys=0x{sys_flags:02x} opts=0x{opt_flags:02x} data@0x{off:x} name={d[0x24:0x30].rstrip(b"\\0")!r}')
frames = bytearray()
resets = 0
for i in range(nframes):
    v = struct.unpack_from('<H', d, off + 2 * i)[0]
    if v & 0x0800: resets += 1
    frames.append(v & 0xff)
print(f'resets(bit11)={resets}')
with open(dst, 'wb') as f:
    f.write(b'ORIN'); f.write(struct.pack('<I', len(frames))); f.write(bytes(frames))
