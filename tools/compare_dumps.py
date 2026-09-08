import sys

def load(path):
    rows = {}
    for line in open(path):
        parts = line.split()
        if len(parts) < 7:
            continue
        rows[int(parts[0])] = parts[1:]
    return rows

a, b = load(sys.argv[1]), load(sys.argv[2])
common = sorted(set(a) & set(b))
print("frames compared:", len(common), "range", common[0] if common else None, common[-1] if common else None)
streak = 0
first_state = None
for f in common:
    if a[f][:5] != b[f][:5]:
        streak += 1
        if streak == 3 and first_state is None:
            first_state = f - 2
            print(f"persistent key-byte difference from frame {first_state}: {sys.argv[1]} {a[first_state][:5]} vs {sys.argv[2]} {b[first_state][:5]}")
            break
    else:
        streak = 0
first_lag = None
for f in common:
    if len(a[f]) > 5 and len(b[f]) > 5 and a[f][5] != b[f][5]:
        first_lag = f
        print(f"first lag-flag difference at frame {f}: {a[f][5]} vs {b[f][5]}")
        break
first_hash = None
for f in common:
    if len(a[f]) > 6 and len(b[f]) > 6 and a[f][6] != b[f][6]:
        first_hash = f
        print(f"first WRAM hash difference at frame {f}: {a[f][6]} vs {b[f][6]}")
        break
if first_state is None and first_hash is None:
    print("no persistent differences")
print("lag frames (second file):", sum(int(b[f][5]) for f in common))
