# TAS input movies

Used as deterministic playthrough scripts for the verification suite. See `docs/design.md`.

| File | Game | Author | Source | Core | Frames |
|---|---|---|---|---|---|
| `ages-consoleverified.bk2` | Ages (USA) | scorpianman42, resynced by alyosha | https://github.com/alyosha-tas/GB-C_replay_files | BizHawk GBHawk, GBA-on-CGB mode | 289,518 |
| `ages-gambatte.bk2` | Ages (USA) | scorpianman42 | https://tasvideos.org/3127M | BizHawk 1.11.5 Gambatte, GBA-on-CGB mode | 289,340 |

Both expect ROM SHA1 `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4`.

Seasons has no usable movie yet. `seasons.vbm` (SwordlessLink and Tompa 2012, VBA-rr, 309,433
frames; `tools/vbm_to_inputs.py` converts it) reaches the file-select menu with ~220 blank
frames prepended to account for VBA-rr skipping the boot ROM, then desyncs. `seasons.bk2` is a
Gambatte movie of the Japanese ROM. Egobuff's 2005 submission has the same problems. The plan is
to record a playthrough on our own core: `oracles ROM roms/cgb_boot.bin --record FILE.inputs`
boots exactly like the headless runner (AGB patch, `gbhawk-wram0.txt`, fresh SRAM), writes the
file every 60 seconds and on quit, and fast-forwards through an existing file before appending.

`seasons-noinput.ref` is the interpreter's (`--no-hooks`) state every 60 frames for a 30,000-frame
run of the Seasons ROM with no input (logos, title, attract demo); `tests/test_tas.c` replays it
with hooks on, so it checks the hooks that run under Seasons against the interpreter, not the
core against hardware. Re-record it with `--no-hooks --frames 30000 --ref-out` after a deliberate
emulation change.

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

`ages-hooked.frames` is the per-frame framebuffer hash of the emulator-hosted C build (every
routine hooked, replace mode) at commit `f7a211f`, recorded on 2026-09-20 with `--frame-hash-out`
on the same replay that checked `ages.ref`. It differs from `ages.frames` (pure interpreter) by
the few M-cycles of interrupt latency a rewrite shifts. It is the frame reference for the
milestone 5 native renderer, whose IO writes land on the same instruction boundaries as this
build's; `oracles-ref` (the emulator-hosted headless runner, `cmake --build build --target
oracles-ref`) is the tool that regenerates both files.
