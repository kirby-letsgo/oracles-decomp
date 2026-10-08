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
- Keep the apps dependency-free beyond SDL3 — the released builds are single self-contained files.

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

## The two apps

Same game, two frontends:

- **`oracles-native`** — the real port, and what ships. No CPU emulator. On first launch it reads a
  ROM (argument, or the launcher's ADD ROM dialog), checks its SHA1 against the two known games,
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

  Esc is Start as well; Cmd+S / Cmd+R save and load one state next to the ROM, F12 takes a
  screenshot, and the game's battery RAM is kept next to the ROM as `.sav`.

Useful flags on `oracles-native`:

```
oracles-native [ROM] [--game ages|seasons [--file 1-3]] [--cache DIR] [--frames N]
```

`--file N` boots straight into save file N, `--cache DIR` uses another cache folder (handy for
testing a first launch), and `--frames N` exits after N frames. Tests drive the app under
`SDL_VIDEO_DRIVER=dummy` with `ORACLES_TEST_KEYS` for scripted input and `ORACLES_SHOT="tick:path.png"`
for screenshots. `ORACLES_TOUCH=1 ./build/oracles-native` shows the phone's touch controls on the
desktop, with the mouse as a finger.

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

`test_wide` checks the widescreen renderer against the game through both TAS movies: the middle 160
columns drawn its way equal the PPU's picture exactly, and what a side strip showed of the next room
matches that room's own picture once the movie reaches it (Seasons 96.8%, Ages 98.9% of the pixels;
the rest are things the player changed). `test_room` checks the ROM room decoder against every room
the movies load. `test_archive` checks the zip/gz reader against streams from a known-good
compressor.

## Recording a playthrough

The recordings in `tas/` are the test suite. To extend the Seasons one:

```bash
./build/oracles "roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc" roms/cgb_boot.bin --record tas/seasons-play.inputs
```

It resumes where the file ends (from a snapshot, without replaying) and writes the file every minute
and on quit. Loading a save state while recording rewinds the recording to that point. See
`tas/README.md` for re-recording the reference hashes afterwards.

## Save sync server

The server lives in `sync-server/` (Fastify, Drizzle, PostgreSQL). The address the apps use is the
CMake setting `ORACLES_SYNC_URL`; a device can point elsewhere with `url=` in `sync.ini` in its app
folder.

Accounts are a 16-digit code, no passwords. Synced files are each game's save, the save-state slots
with their pictures and item buttons, and the shared settings — never ROM-derived files. The app
syncs when the launcher opens, before a game starts, and when leaving a game or the app. A failed
sync is retried after 5 s, then twice as long each time up to every 5 minutes, and at once when the
app returns to the foreground; a running game's files wait for the game to end. When a save changed
on two devices since they last synced, the app shows a KEEP WHICH? screen.

Save states move between every platform (all 64-bit builds share one layout); only a build with a
different state format refuses one.

HTTP uses the system's own TLS: libcurl (macOS, Linux; sync is off without it), WinHTTP, and
Android's HttpURLConnection.

```bash
ORACLES_SYNC_URL=http://host ./build/test_sync_http   # check a running server
```

The `sync_flow` test plays two devices against the real server on an in-memory database (`pnpm local`
in `sync-server/`; needs its `pnpm install` and the Seasons ROM): a conflict kept each way, and a
save made offline that the retry uploads.

## Android

`oracles-native` also builds as an Android app (arm64, Android 8+). Needs the Android SDK with
platform 35, build-tools 35, NDK 27.2.12479018 and CMake 3.31.6 (`sdkmanager` installs them), and a
JDK; `android/local.properties` names the SDK (`sdk.dir=...`).

```bash
cd android
./gradlew assembleDebug
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

The build runs this repo's CMake with SDL 3.4.16 from source (its Java glue is vendored in
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
- `src/ui/`: the launcher, menus and overlays, drawn with the ROM's own font -- plus a small
  built-in one (`font_builtin.c`) for the launcher before any ROM is installed.
- `src/wide/`: the widescreen renderer and the ROM room decoder.
- `src/sync/`: the save-sync client.
- `tools/`: generators and the audits run on every change.
- `tas/`: input movies and reference hashes.
- `sync-server/`: the save-sync server.
- `ref/oracles-disasm/`: the community disassembly (submodule), used for symbols and routines.
