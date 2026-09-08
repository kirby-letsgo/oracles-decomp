import gzip
import io
import struct
import sys
import zipfile

BITS = {"A": 1, "B": 2, "Select": 4, "Start": 8, "Right": 16, "Left": 32, "Up": 64, "Down": 128}

src, dst = sys.argv[1], sys.argv[2]
data = open(src, "rb").read()
if data[:2] == b"\x1f\x8b":
    data = gzip.decompress(data)
z = zipfile.ZipFile(io.BytesIO(data))
lines = z.read("Input Log.txt").decode().splitlines()
key = next(l for l in lines if l.startswith("LogKey:"))
names = [n.strip().split()[-1] for n in key[len("LogKey:#"):].split("|") if n.strip()]
frames = []
for l in lines:
    if not (l.startswith("|") and l.endswith("|")):
        continue
    v = 0
    for ch, name in zip(l[1:-1], names):
        if ch == ".":
            continue
        if name == "Power":
            raise SystemExit("reset at frame %d is not supported" % len(frames))
        v |= BITS[name]
    frames.append(v)
with open(dst, "wb") as f:
    f.write(b"ORIN")
    f.write(struct.pack("<I", len(frames)))
    f.write(bytes(frames))
print(len(frames), "frames")
