Native reimplementation of The Legend of Zelda: Oracle of Ages and Oracle of Seasons in C.

Every routine the games run is readable C: all of Ages, and 99% of Seasons (shared with Ages
where the two games agree, hand-written where Seasons differs; the rest is generated C from the
disassembly). The native build runs both games with no CPU emulator and no ROM code. Behaviour is
checked against the original ROM by replaying full playthroughs and comparing every C routine
with the original code it replaces.

You need your own US ROMs; none are included.

## Prerequisites

- macOS (Linux and Windows build through CMake)
- Xcode Command Line Tools (Clang)
- CMake 3.15+ and Ninja
- SDL3 (Homebrew, or fetched automatically on first configure)
- Python 3, and wla-dx plus pyyaml to build the disassembly's symbol files

```bash
brew install cmake ninja sdl3 wla-dx
pip3 install pyyaml
```

## Setup

```bash
git clone git@github.com:kirby-letsgo/oracles-decomp.git
cd oracles-decomp
git submodule update --init --recursive
make -C ref/oracles-disasm ages CPUS=4
make -C ref/oracles-disasm seasons CPUS=4
```

Put the ROMs and the CGB boot ROM in `roms/` (git-ignored):

| File | SHA1 |
|---|---|
| `roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc` | `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4` |
| `roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc` | `ba1268290fb2b1b70505d2d7b5825fc8a4816a4b` |
| `roms/cgb_boot.bin` | the Game Boy Color boot ROM |

## Build

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build
```

`-DORACLES_SDL=OFF` builds only the headless tools.

## Playing

Two apps, same game:

- `oracles-native`, the real port: no CPU emulator. On first launch it reads your ROM once,
  keeps the graphics, sound and data, zeroes the code bytes, and caches the result; later
  launches never touch the ROM again.
  ```bash
  ./build/oracles-native "roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc"
  ```
- `oracles`, the development app: runs the ROM on our emulator core with the C routines hooked
  in, so it can boot the real boot ROM, record playthroughs and fall back to the original code.
  ```bash
  ./build/oracles "roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc" roms/cgb_boot.bin
  ```

Without arguments `oracles-native` opens a launcher: Ages and Seasons side by side (each in its own
title-screen colours), each game's three save files with name, hearts and essences, Resume (the state
from the last quit), Load state, and the title screen. Choosing a file boots straight into it.

Controls: arrow keys to move, `X` / `Z` for A / B, Return for Start, Backspace or Right Shift for
Select, `M` to mute, F11 or Cmd+F for fullscreen, hold Tab (gamepad: right trigger) to fast-forward.
The window resizes in whole-pixel steps. Settings (from the launcher or the pause menu): volume,
screen filter (sharp, scanlines, LCD grid, CRT), Game Boy Color colours, fullscreen.

- `oracles-native`: Esc (or the gamepad's Guide button) pauses: Resume, Save state, Load state (4
  slots with thumbnails) and Quit, which saves a Resume state and returns to the launcher. Cmd+S /
  Cmd+R save and load slot 1. Everything lives in the per-user app folder
  (`~/Library/Application Support/oracles-decomp/oracles/` on macOS), including the game's own save.
- `oracles`: Esc is Start as well; Cmd+S / Cmd+R save and load one state next to the ROM, F12 takes a
  screenshot, and the game's save (battery RAM) is kept next to the ROM as `.sav`.

## Recording a playthrough

The recordings in `tas/` are the test suite. To extend the Seasons one:

```bash
./build/oracles "roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc" roms/cgb_boot.bin --record tas/seasons-play.inputs
```

It resumes where the file ends (from a snapshot, without replaying) and writes the file every
minute and on quit. Loading a save state while recording rewinds the recording to that point.
See `tas/README.md` for re-recording the reference hashes afterwards.

## Verification

```bash
ctest --test-dir build                          # unit suites, TAS prefixes, whole native runs
TAS_FRAMES=321712 ctest --test-dir build -R tas  # both whole movies with hooks too
```

Two full-game movies are the main gate: the console-verified Ages TAS and the console-verified
Seasons TAS (`tas/README.md`), plus a recorded Seasons playthrough.

The headless runner replays a movie and checks it:

```bash
./build/oracles-run --rom ROM --boot roms/cgb_boot.bin --init-ram tas/gbhawk-wram0.txt \
  --tas tas/seasons-play.inputs --frames 265064 --verify-shadow --ref-check tas/seasons-play.ref
```

`--verify-shadow` runs every hooked C routine, replays the original code from the same state and
compares registers, memory and cycles. `--ref-check` compares the machine state every 60 frames
with the hashes the pure interpreter (`--no-hooks`) recorded. `oracles-native-run` replays
movies on the native build.

## Layout

- `src/core/`, `src/hw/`: SM83 CPU, memory bus, PPU, APU, timers.
- `src/game/`: the game in C, one file per disassembly file; `src/game/seasons/` holds the
  Seasons-only code.
- `src/hooks/`: the address-to-function tables and the lists the generators read.
- `src/rt/`: the native runtime (no interpreter).
- `src/platform/`: the SDL apps and the headless runner.
- `tools/`: generators and the audits run on every change.
- `tas/`: input movies and reference hashes.
- `ref/oracles-disasm/`: the community disassembly (submodule), used for symbols and routines.
