# BizHawk GBHawk as a reference emulator

Headless BizHawk 2.11.1 in Docker, used to record what GBHawk does frame by frame while it plays
`tas/ages-consoleverified.bk2`. Nothing here is needed to build or run the port.

## Image

1. Download `BizHawk-2.11.1-linux-x64.tar.gz` next to the `Dockerfile` as `BizHawk-linux.tar.gz`.
2. `docker build -t bizhawk-gbhawk .`
3. On Apple Silicon the shipped native libs are x86; copy `libblip_buf.so` and `libcimgui.so` from
   BizHawk's `Dist/arm64` into a `dll-arm64/` data folder and symlink the system sqlite as
   `dll/libe_sqlite3.so` inside the container (see `run_anchors.sh`).

## Data folder

Mounted at `/data`: `ages.gbc`, `ages.bk2`, `config.ini`, `Firmware/GBC_cgb.bin` (the real CGB
boot ROM; GBHawk applies the GBA patch itself), `dll-arm64/`, and the Lua script to run.
`config.ini` disables throttling and sound, selects GBHawk and GdiPlus rendering. The movie's
`SyncSettings.json` must not contain the obsolete `GBController` field or BizHawk falls back to
DMG mode.

## Scripts (`lua/`)

- `dump.lua`: per frame, key bytes (group, room, frame counter, RNG) and lag flag; WRAM hash every
  60 frames. `tools/compare_dumps.py` diffs it against `oracles-run --dump`.
- `anchors.lua`: first (or every, with `ALL`) execution time of the PCs in `PCS` between frames
  `START` and `END`, plus LCDC writes and input polls. `run_anchors.sh` wraps the docker call,
  `join_anchors.sh` aligns two anchor files.
- `trace_lcdc.lua`: LCD on/off write times. `inputpoll.lua`: controller latch points and lag.
- `wram_at.lua`, `wram_range.lua`: full WRAM/HRAM at one frame or a range. `cycles_at.lua`:
  cycle counter at frame boundaries. `sram.lua`: initial cartridge RAM. `dump_timing.lua`: WRAM
  c000-c0bf after 200 frames, for the micro test ROMs in `tests/timing/`.

Cycle units: `emu.totalexecutedcycles() // 4` is M-cycles; the counter starts 2 M-cycles (8 base
cycles) before the CPU's first fetch, which is why our frame grid uses `GRID_OFFSET -8`.
