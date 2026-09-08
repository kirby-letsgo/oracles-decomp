# TAS input movies

Used as deterministic playthrough scripts for the verification suite. See `docs/design.md`.

| File | Game | Author | Source | Core | Frames |
|---|---|---|---|---|---|
| `ages-consoleverified.bk2` | Ages (USA) | scorpianman42, resynced by alyosha | https://github.com/alyosha-tas/GB-C_replay_files | BizHawk GBHawk, GBA-on-CGB mode | 289,518 |
| `ages-gambatte.bk2` | Ages (USA) | scorpianman42 | https://tasvideos.org/3127M | BizHawk 1.11.5 Gambatte, GBA-on-CGB mode | 289,340 |

Both expect ROM SHA1 `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4`.

BK2 files are zip archives. `Input Log.txt` holds one line per frame in the order
Up, Down, Left, Right, Start, Select, B, A, Power.

## Reference hashes

`ages.ref` (one line per 60 frames: `frame state-hash`) and `ages.frames` (one 64-bit framebuffer
hash per frame) were recorded with

    oracles-run --rom <ages> --boot roms/cgb_boot.bin --init-ram gbhawk-wram0.txt \
        --tas ages-consoleverified.inputs --ref-out ages.ref --frame-hash-out ages.frames

from a core whose per-frame memory matches GBHawk 2.11.1 for the whole movie. `gbhawk-wram0.txt`
is GBHawk's WRAM and HRAM at power-on, which the game's RNG seeding depends on. Re-record both
files whenever the emulation changes on purpose; `tests/test_tas.c` compares against them.
