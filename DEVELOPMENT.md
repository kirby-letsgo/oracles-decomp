# Development

How the port is built, how it is checked against the original games, and the tools used to work on
it. For playing, see the [README](README.md).

## Ground rules

- **Never commit game data or anything derived from it**: no ROMs, no extracted assets, no save
  files. `.gitignore` covers `roms/` entirely. Addresses, symbol names and cycle counts are fine.
- The apps ship no ROM code: `oracles-native` zeroes the code bytes of the ROM it caches, and the
  `native_binary` test is the gate — the native library must link no interpreter (no `cpu.c` object)
  and no native executable may contain the Nintendo logo, the 48 bytes at `$104` that are identical
  in both games and so cannot be data the engine keeps on purpose.
- Keep the apps dependency-free beyond SDL3 — the released builds are single self-contained
  files. Vendored source in `third_party/` is fine (it links in); a system library is not.
- The project is **GPLv3** (`LICENSE`), because it vendors xBRZ (GPLv3) in `third_party/xbrz/`; hqx
  in `third_party/hqx/` is LGPL 2.1. C++ is enabled for xBRZ and the scaler wrapper; everything else
  is C. The MinGW build links the C++ runtime statically, so the Windows app stays one file.
- Keep the local changes to vendored code few, and write each one down in that folder's `README.md`.

## Requirements

- macOS (Linux and Windows build through CMake)
- Xcode Command Line Tools (Clang)
- CMake 3.15+ and Ninja
- SDL3 (Homebrew, or fetched automatically on first configure)
- Python 3, and wla-dx plus pyyaml and pypng to build the disassembly's symbol files

```bash
brew install cmake ninja sdl3 wla-dx
pip3 install pyyaml pypng
```

Recent Python installs (Homebrew's included) refuse `pip3 install` into the system interpreter with
an `externally-managed-environment` error. Either install the two packages for just your user, which
is enough for the build to find them:

```bash
pip3 install --user --break-system-packages pyyaml pypng
```

or keep them out of your Python entirely with a virtualenv, which has to stay active for the
`make -C ref/oracles-disasm` steps below:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pyyaml pypng
```

## Getting started

```bash
git clone git@github.com:kirby-letsgo/oracles-decomp.git
cd oracles-decomp
git submodule update --init --recursive
make -C ref/oracles-disasm ages CPUS=4
make -C ref/oracles-disasm seasons CPUS=4
```

Put the ROMs and the CGB boot ROM in `roms/` (git-ignored). The tests read these paths directly, so
they need the loose `.gbc` files even though the apps also accept archives:

| File | SHA1 |
|---|---|
| `roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc` | `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4` |
| `roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc` | `ba1268290fb2b1b70505d2d7b5825fc8a4816a4b` |
| `roms/cgb_boot.bin` | the Game Boy Color boot ROM |
| `roms/Legend of Zelda, The - Oracle of Ages (Europe) (En,Fr,De,Es,It).gbc` (optional, `test_eu`) | `9f2dff3c46406839f839a8d535353dfc1e3e8670` |
| `roms/Legend of Zelda, The - Oracle of Seasons (Europe) (En,Fr,De,Es,It).gbc` (optional, `test_eu`) | `360b9408e569ba826212eac1ebea1b09573cb48c` |

## Build

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build
```

`-DORACLES_SDL=OFF` builds only the headless tools. `-DORACLES_SDL_VENDORED=ON` builds SDL 3.4.16
from source and links it statically (release builds do this); otherwise an installed SDL3 is used
when there is one.

Linux (Debian/Ubuntu; other distros need the same libraries): `tools/linux_deps.sh` installs the
compiler, CMake, Ninja and SDL's build dependencies, then build as above with
`-DORACLES_SDL_VENDORED=ON` (distributions rarely ship SDL3 yet).

Windows (64-bit) is cross-compiled with MinGW-w64 (`brew install mingw-w64` or `apt install
mingw-w64`); the result is a single `Oracles.exe` that needs only system DLLs. Wine runs the tests.

```bash
cmake -S . -B build-win -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-x86_64.cmake -DORACLES_SDL_VENDORED=ON
cmake --build build-win
wine build-win/test_native_tas.exe
```

Homebrew's mingw-w64 is posix-thread-model and links winpthreads implicitly; apt's, which CI
installs, is win32-model and does not. So a symbol that only winpthreads provides (`clock_gettime`,
as `clock_gettime64`) links here and fails there, and compiling each file with `-c` never sees it at
all, since the symbol only has to exist at link time. The `windows-cross` ci job builds with the
stricter toolchain on every branch push. Locally, intersect the undefined symbols in the archives
with what libwinpthread defines -- toolchain-independent, needs no SDL and no exe, and prints
nothing when nothing depends on winpthreads:

```bash
cmake -S . -B build-win -G Ninja -DORACLES_SDL=OFF -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-x86_64.cmake
cmake --build build-win
WP=$(x86_64-w64-mingw32-gcc -print-file-name=libwinpthread.a)
nm -g build-win/*.a | awk '$1=="U"{print $2}' | sort -u > /tmp/undef
nm -g "$WP" | awk '$2=="T"{print $3}' | sort -u > /tmp/wp
comm -12 /tmp/undef /tmp/wp
```

That configure takes a second (no vendored SDL to fetch or build), the archives about two minutes
cold and under one second to rebuild one of them after an edit.

## The two apps

Same game, two frontends:

- **`oracles-native`** — the real port, and what ships. No CPU emulator. On first launch it reads a
  ROM (argument, or the main menu's Add ROM row), checks its SHA1 against the two known games,
  builds the cycle table from the original bytes, zeroes the code bytes and writes the result to the
  per-user cache; later launches load the cache and never see the ROM again.

  ```bash
  ./build/oracles-native "roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc"
  ```

  It accepts the ROM loose or inside a `.zip`/`.gz` (`src/platform/archive.c`, which carries its own
  inflate so no compression library is needed). Detection is by magic bytes, not extension. `.7z`
  is refused with a message rather than supported, since it would mean carrying an LZMA decoder.

- **`oracles`** — the development app: runs the ROM on our emulator core with the C routines hooked
  in, so it can boot the real boot ROM, record playthroughs and fall back to the original code. It
  takes a **loose ROM only**.

  ```bash
  ./build/oracles "roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc" roms/cgb_boot.bin
  ```

  Esc is Start as well; Cmd+S / Cmd+R (Ctrl+S / Ctrl+R off a Mac) save and load one state next to the ROM, F12 takes a
  screenshot, and the game's battery RAM is kept next to the ROM as `.sav`.

Useful flags on `oracles-native`:

```
oracles-native [ROM] [--game ages|seasons [--file 1-3]] [--cache DIR] [--frames N]
```

`--file N` boots straight into save file N, `--cache DIR` uses another cache folder (handy for
testing a first launch), and `--frames N` exits after N frames. Tests drive the app under
`SDL_VIDEO_DRIVER=dummy` with `ORACLES_TEST_KEYS` for scripted input and `ORACLES_SHOT="tick:path.png"`
for screenshots. `ORACLES_TOUCH=1 ./build/oracles-native` shows the phone's touch controls on the
desktop, with the mouse as a finger, and `ORACLES_TEST_HAPTICS=1` prints `haptic` to stderr for
each tick a phone would vibrate.

## Verification

```bash
ctest --test-dir build                           # unit suites, TAS prefixes, whole native runs
TAS_FRAMES=321712 ctest --test-dir build -R tas   # both whole movies with hooks too
```

Tests that need a ROM skip (exit 77) when `roms/` is empty, so a fresh checkout still runs the rest.

Two full-game movies are the main gate: the console-verified Ages TAS and the console-verified
Seasons TAS (`tas/README.md`), plus a recorded Seasons playthrough.

The headless runner replays a movie and checks it:

```bash
./build/oracles-run --rom ROM --boot roms/cgb_boot.bin --init-ram tas/gbhawk-wram0.txt \
  --tas tas/seasons-play.inputs --frames 265064 --verify-shadow --ref-check tas/seasons-play.ref
```

`--verify-shadow` runs every hooked C routine, replays the original code from the same state and
compares registers, memory and cycles. `--ref-check` compares the machine state every 60 frames with
the hashes the pure interpreter (`--no-hooks`) recorded. `oracles-native-run` replays movies on the
native build.

Both runners end with a hook census on stderr: how many routines are hooked, how many entries they
saw, and which saw none. That counter normally observes only the dispatcher, and a routine its ported
callers enter directly (`CALL_C`, `TAIL`) never reaches the dispatcher -- so the list says "never
dispatched", which is not evidence the routine never ran. `HOOKCENSUS=1` counts entries from ported C
too, at a table lookup per call, and the list then names what really never ran; `--verify-shadow`
counts them anyway. On a 4,000-frame Ages run the census goes from 162,317 entries to 728,900, and
580 routines leave the never-ran list.

`WRAMDUMP_AT=<first>-<last> WRAMDUMP_FILE=<path>` appends WRAM, HRAM, VRAM, OAM, IO and the palettes
as hex for those movie frames. If what it writes looks dead, check the harness is where you think it
is: `wRoomLayout` at `$cf00` cannot be zero in a loaded room.

`test_wide` checks the widescreen renderer against the game through both TAS movies: the middle 160
columns drawn its way equal the PPU's picture exactly, and what a side strip showed of the next room
matches that room's own picture once the movie reaches it (Seasons 96.8%, Ages 98.9% of the pixels;
the rest are things the player changed). `test_room` checks the ROM room decoder against every room
the movies load. `test_archive` checks the zip/gz reader against streams from a known-good
compressor.

## European ROMs

The European releases (`Legend of Zelda, The - Oracle of Ages (Europe) (En,Fr,De,Es,It).gbc`,
SHA1 `9f2dff3c46406839f839a8d535353dfc1e3e8670`, and `... Oracle of Seasons (Europe)
(En,Fr,De,Es,It).gbc`, `360b9408e569ba826212eac1ebea1b09573cb48c`) are 2 MB ROMs with five
languages: English, French, German, Italian and Spanish (`wActiveLanguage`, $c62a, 0-4). Most of
their code is the USA code, often moved; the text engine, the file menu (a ninth mode, the language
screen), name entry and a few dozen gameplay routines differ. RAM is laid out the same, so the save
format, the mods and the RAM symbols are shared. The region is the header's game code
(`rom[0x142] == 'P'`: AZ8P, AZ7P); `hooks.c` then selects the EU hook table, `syms_select` the EU
symbol column (`game_eu`), and `assets.c` the EU code map.

How the EU build is made, all from the USA symbols and the two ROMs:

- `tools/eu_sym.py` writes `ref/eu/{ages,seasons}_eu.sym` (and `src/hooks/extra*_eu.sym`,
  `alias_seasons_eu.sym`): every USA label carried to its EU address by aligning each code bank
  with its EU counterpart, then by the operands of already-placed code. About 96% are placed by
  the alignment, 2% between two placed neighbours, and 1.5% stay unplaced (data only the USA build
  uses).
- `tools/eu_hooks.py` keeps in the EU table the USA hooks whose routine is instruction-identical in
  the EU ROM (pointers relocated) and whose callees are kept too.
- `tools/eu_gen.py` transliterates the rest from the EU ROM (`src/game/eu/gen_*.c`, cycle-exact like
  `tools/transliterate.py`), following calls, jump tables and thread resume points to a fixed point,
  so the native build always finds C where the game goes.
- The shared C builds some pointers as `SYM(label) + n` under a nearby label, which can move by a
  different amount in the EU ROM. `tools/eu_operands.py` evaluates each one with the EU symbols
  against the operand the EU ROM has, and `tools/eu_fix_pointers.py` rewrites the ones that differ
  as `RV(usa, eu)` (`game_eu` picks the arm).
- `src/wide/room.c` and `item.c` carry the EU table addresses; the EU gfx header table has entries
  with one source per language, read with the RAM's language.

After C changes, `tools/eu_regen.sh AGES_EU.gbc SEASONS_EU.gbc` runs the steps after `eu_sym.py`
and ends with the pointer check. `test_eu` (with the EU ROMs in `roms/`) starts each EU ROM from a
USA movie's file data put in its battery RAM, enters every room of every group natively, and
compares the ROM room decoder with the rooms the game loaded. There is no EU movie; coverage beyond
that comes from comparing the native build with the interpreter frame by frame from saves:

```bash
./build/oracles-run --rom EU.gbc --tas IN.inputs --frames N --no-hooks --frame-hash-out fh.bin [--sram file.sav]
./build/oracles-native-run --rom EU.gbc --tas IN.inputs --frames N --frame-hash-check fh.bin [--sram file.sav]
```

`--sram` starts with that battery RAM; `WRAMDUMP_AT` takes a comma list of frames, so a USA movie
can yield the file data of many points in one run.

## Recording a playthrough

The recordings in `tas/` are the test suite. To extend the Seasons one:

```bash
./build/oracles "roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc" roms/cgb_boot.bin --record tas/seasons-play.inputs
```

It resumes where the file ends (from a snapshot, without replaying) and writes the file every minute
and on quit. Loading a save state while recording rewinds the recording to that point. See
`tas/README.md` for re-recording the reference hashes afterwards.

## Save sync server

The server is not in this repository: it lives in **save-sync-server** (Fastify, Drizzle,
PostgreSQL), which serves this app through its `oracles` profile and is meant to serve others too. The address
the apps use is the CMake setting `ORACLES_SYNC_URL`; a device can point elsewhere with `url=` in
`sync.ini` in its app folder.

Accounts are a 16-digit code, no passwords. Synced files are each game's save, the save-state slots
with their pictures and item buttons, and the shared settings — never ROM-derived files. The app
syncs when the launcher opens, before a game starts, and when leaving a game or the app. A failed
sync is retried after 5 s, then twice as long each time up to every 5 minutes, and at once when the
app returns to the foreground; a running game's files wait for the game to end. When a save changed
on two devices since they last synced, the app shows a KEEP WHICH? screen; the side the player
turns down is kept as `<name>.bak` beside it, since the server's history holds only versions that
reached it.

Save states move between every platform (all 64-bit builds share one layout); only a build with a
different state format refuses one.

HTTP uses the system's own TLS: libcurl (macOS, Linux; sync is off without it), WinHTTP, and
Android's HttpURLConnection.

```bash
ORACLES_SYNC_URL=http://host ./build/test_sync_http   # check a running server
```

The `sync_flow` test plays two devices against the real server: a conflict kept each way, and a
save made offline that the retry uploads. It runs the server from the pinned container image named
at the top of `tests/test_sync_flow.py` — the save-sync-server image that serves an in-memory
database and starts empty — so it needs Docker and the Seasons ROM, and skips without either. Bump
pin (by digest) when the server's API changes.

```bash
ORACLES_SYNC_IMAGE=save-sync-server:test ctest --test-dir build -R sync_flow   # built locally
```

## Android

`oracles-native` also builds as an Android app (arm64, Android 8+). Needs the Android SDK with
platform 35, build-tools 35, NDK 27.2.12479018 and CMake 3.31.6 (`sdkmanager` installs them), and a
JDK; `android/local.properties` names the SDK (`sdk.dir=...`).

```bash
cd android
./gradlew assembleDebug
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

A debug APK is `org.oracles.test`, named "Oracles test": it installs beside a released copy, with
its own saves. The build runs this repo's CMake with SDL 3.4.16 from source (its Java glue is vendored in
`android/app/src/main/java/org/libsdl/app`, same release) and optimises the engine in debug APKs too.
Push a ROM somewhere the picker can reach it, e.g. `adb push ROM /sdcard/Download/`.
`adb logcat -s oracles` shows the engine's messages.

## Releases

Every push to `main` rebuilds the apps and replaces the rolling
[nightly release](https://github.com/kirby-letsgo/oracles-decomp/releases/tag/nightly): macOS
(universal DMG), Windows (x64 zip), Linux (x86_64 AppImage) and Android (arm64 APK). The workflow is
`.github/workflows/release.yml`. The APK is signed with the release key from the repository secrets
`ANDROID_KEYSTORE_B64`, `ANDROID_KEYSTORE_PASSWORD` and `ANDROID_KEY_ALIAS` (debug-signed, with a
warning, when they are missing).

## Layout

- `src/core/`, `src/hw/`: SM83 CPU, memory bus, PPU, APU, timers.
- `src/game/`: the game in C, one file per disassembly file; `src/game/seasons/` holds the
  Seasons-only code.
- `src/hooks/`: the address-to-function tables and the lists the generators read.
- `src/rt/`: the native runtime (no interpreter).
- `src/platform/`: the SDL apps and the headless runner; `archive.c` reads the ROM, unpacking a zip
  or gz with its own inflate so the apps need no compression library.
- `src/ui/`: the menus' state machines (`home.c` is the main menu, `menu.c` pause and slots,
  `settings.c`, `controls.c`, `syncui.c`), the touch controls' layout and hit areas (`touch.c`, in
  game pixels), and the screen filters. Keys, pads and taps all go through the same `*_press` functions.
- `src/gui/`: the menus drawn at the screen's resolution, over the game: Clay lays them out, SDL
  draws them in VT323 (`gui.c`). `widgets.c` holds the shared pieces (rows, headers, tabs, pages,
  lists); each screen has its own file (`pause.c` also has the message and the sync wait,
  `home.c`, `slots.c`, `settings.c`, `controls.c`, `sync.c`), `pad.c` draws the touch controls,
  and `screens.h` is what the app calls. A tap moves the selection and becomes the same button a key would give. Tests tap rows
  with `^Element.index` in `ORACLES_TEST_KEYS`.
- `src/wide/`: the widescreen renderer, the ROM room decoder, and `item.c`, which decodes an
  item's status-bar icon from the ROM so the X and Y slots can be shown beside the bar.
- `src/sync/`: the save-sync client.
- `src/mods/`: the mod host — Lua, the `oracles` API, and the overlay mods draw into. `MODDING.md`
  (repo root) is its player-facing reference, and `test_mods` fails the build if that file and the
  API a loaded mod actually sees ever disagree, in either direction — the same kind of gate
  `native_binary` is for leaked ROM bytes. `test_mods` itself never links `src/platform/app.c`, so
  the `mods_room` ctest drives the real app (ROM-labelled, `tests/test_mods_room.py`) through the
  start of the Seasons movie with a mod watching `on_room_change`, to pin `app.c`'s own copy of the
  current-room address against the engine's (`test_room.c`'s `record_cb`) rather than by convention.
- `third_party/lua/`: Lua 5.4.9 (MIT), the mod runtime.
- `third_party/xbrz/`, `third_party/hqx/`: the two pixel-art scalers (GPLv3 and LGPL 2.1) behind
  the XBRZ and HQX screen filters, wrapped by `src/ui/upscale.cpp`.
- `third_party/clay/`, `third_party/stb/`, `third_party/fonts/`: the layout library (zlib), the
  font rasteriser (public domain) and VT323 (OFL) behind `src/gui/`; the font is embedded at build
  time by `cmake/embed.cmake`. Each folder's `README.md`
  lists what was changed locally.
- `tools/`: generators and the audits run on every change.
- `tas/`: input movies and reference hashes.
- `ref/oracles-disasm/`: the community disassembly (submodule), used for symbols and routines.

## Randomizer prototype

`tools/randomizer/generate.py` generates deterministic treasure placements from an authored world
logic file, verifies progression independently, and exports an API 2 Lua mod with a spoiler.
The `--preset seasons-d1-optional` preview shuffles the map, compass and Gasha chests in D1,
with progression left vanilla. Full-game location logic, progressive items and save/seed binding
are still required. See
[the format and implementation checklist](tools/randomizer/README.md).

`python3 tests/test_randomizer.py` (also the `randomizer` CTest) tests synthetic progression graphs,
self-lock rejection, deterministic seeds, pool preservation and export. No ROM is required.

`randomizer_replay` checks a generated D1 seed through real native-engine treasure pickups and
inventory changes using the local Seasons USA ROM and recording. It is a pickup integration test,
not a proof of full-dungeon completion; see the preset's validation limits in its README.
