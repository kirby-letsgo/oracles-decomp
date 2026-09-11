# Oracles Native Port: Design

Date: 2026-09-07
Status: approved

## Goal

A native reimplementation of The Legend of Zelda: Oracle of Ages and Oracle of Seasons in C,
running on macOS and Android, with user-toggleable enhancements and a mod system. The shipped app
contains no emulator and no ROM. The user supplies their own US ROM once, and the app extracts
assets from it.

Decisions already made:

- Language: C first. A Rust port may follow once behaviour is pinned down by the verification suite.
- First game: Oracle of Ages. Seasons shares the engine and follows as its own sub-project.
- First milestone: boot to title screen, pixel-identical to the reference.
- Enhancements: widescreen, hi-res asset packs, modern QoL, both games in one app. All toggleable.
- Mod system: built-in toggles first, then data packs, then Lua scripted mods.

## Reference material

- Disassembly: Drenn's `oracles-disasm`, pinned as a git submodule at `ref/oracles-disasm`.
  About 510k lines across 1,241 files. Engine core lives in `code/` (about 58k lines), per-object
  code in `object_code/`, data tables in `data/`, graphics in `gfx/`, audio in `audio/`.
  One tree builds both games behind a game flag, confirming the shared-engine assumption.
- ROMs: US versions only.
  - Ages SHA1 `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4`
  - Seasons SHA1 `ba1268290fb2b1b70505d2d7b5825fc8a4816a4b`
- TAS input movies (see `tas/README.md` for attribution):
  - Ages, console-verified resync by alyosha of scorpianman42's run. BizHawk GBHawk core,
    289,518 frames, no resets, ROM SHA1 matches ours. This is the primary verification script.
  - Ages, original publication on Gambatte, 289,340 frames. Kept as a secondary check.
  - Seasons, SwordlessLink and Tompa 2012, VBA-rr `.vbm`. May need a resync. Not a milestone dependency.
  - Both Ages movies run in GBA-on-GBC mode. The Oracles detect a GBA and unlock the Advance Shop,
    so the core must boot with the GBA flag set.

## Approach

Emulator-hosted incremental replacement. A small SM83 interpreter runs the real ROM from day one.
Routines are ported to C one at a time. When the interpreter reaches a ported routine's address,
the C version runs instead and returns. The game is playable at every point in the port, every
routine is verified in isolation, and progress is a single number: percent of routines ported.
When that reaches 100 the interpreter is deleted.

Alternatives considered and rejected:

- Clean reimplementation with per-frame framebuffer diffing only. Nothing runs until the boot
  path is complete, and a frame mismatch only says "somewhere in the last frame".
- zelda3 model, porting everything before serious verification. Proven, but the least incremental
  and a poor fit for work done in many separate sessions.

## Architecture

One C project, `oracles`, built with CMake. Five layers, each with one job.

### Core (`src/core/`)

SM83 interpreter, MBC5 banking, timers, interrupts, joypad register. Scaffolding only. Deleted at
100 percent. No PPU or APU emulation lives here: the Platform layer renders and mixes from the
game's memory mirrors from the start, so that the renderer is verified alongside the game logic.

Boot state: no boot ROM. Execution starts at 0x100 with the documented post-boot CGB register
state and register B set to 0x01 (GBA flag). All RAM is zeroed at power on, matching the TAS
emulator settings. No real-time clock.

### Game (`src/game/`)

The ported routines. One C file per disassembly file, same file and function names as the
disassembly so cross-referencing is trivial. Each function carries a comment naming the bank and
address it replaces.

Game state is a byte-exact mirror of WRAM and HRAM. Named accessors are generated from the
disassembly's RAM labels by a tool in `tools/`. The refactor to idiomatic structs happens after
100 percent, guarded by the same verification suite.

A runtime `game` field (ages or seasons) mirrors the disassembly's build flag.

### Hooks (`src/hooks/`)

A table mapping (bank, address) to a C function. When the Core's PC hits an entry, the hook calls
the C function and simulates `ret`. The reverse direction also exists: `asm_call(bank, addr)`
runs the interpreter until that routine returns, so a C function can call code that has not been
ported yet. Both directions remain legal for the whole port.

The hook table is generated from the disassembly symbol file plus a list of ported routine names.

### Platform (`src/platform/`)

The PPU-equivalent renderer, APU-equivalent audio synth, input, save files, and the SDL window.

The renderer draws from the game's VRAM, OAM, and palette mirrors, and honours per-scanline
register changes (LCDC, SCX, SCY, WX, WY, palettes) because the game uses LYC interrupts for the
HUD split and for screen effects. Until the Game layer produces a scene (see Enhancements), the
renderer is effectively a PPU reimplementation and is verified by frame hashing.

Save states and rewind serialise the state mirror.

### Frontend (`src/frontend/`)

Launcher (game select), settings, feature toggles, and later the mod loader. Talks to Game only
through the enhancement API.

### Per-frame data flow

1. Frontend polls SDL input and writes the joypad mirror (or the TAS player does).
2. Game runs one frame of logic, as C where ported, via the Core otherwise.
3. Platform renders VRAM and OAM to a texture and mixes audio for the frame.

### Assets

At first launch the app asks for the ROM, verifies its SHA1 against the known hashes, and
extracts graphics, maps, text, and music into an `assets/` cache. Until 100 percent the ROM bytes
also stay loaded for the Core. The app never ships a ROM or extracted assets.

## Verification

Three checks, cheapest first. A ported routine counts as done only when all three pass.

1. **Routine diff.** In verify mode the hook runs both versions: snapshot memory, run the C
   function, snapshot, restore, run the original asm in the interpreter, snapshot. Any difference
   in WRAM, HRAM, VRAM, OAM, or CPU registers halts with the routine name, address, and differing
   bytes. Runs continuously while the TAS plays, so each routine is exercised with real inputs.
2. **TAS replay.** The console-verified Ages movie plays through the build. A reference run on
   the pure interpreter, taken once in milestone 1, stores a memory hash every 60 frames in
   `tas/ages.ref`. The mixed build must produce identical hashes. Catches timing and ordering bugs.
3. **Frame compare.** Same replay, with the renderer output hashed per frame against reference
   PPU output. Catches renderer bugs.

Determinism requirements: no RTC, no uninitialised reads, RAM zeroed at power on.

CI runs every ROM-free test (Blargg, cgb-acid2, unit tests) on each push. The TAS checks need the
ROM, so they run locally: the first 20,000 frames by default in `ctest`, the full movie with
`TAS_FRAMES=289518`. Run the full movie before merging any change to `src/core/` or
`src/platform/render.c`.

## Enhancements and mods

All enhancements sit behind one `Features` struct read by Game and Platform. Ported routines never
check a feature directly. The port grows three seams and every enhancement plugs into a seam.

### Render seam

Platform renders from a scene the game hands it: room tile map, object sprite list, palettes,
HUD, per-scanline effects. The original 160x144 view is one camera over that scene.

- Widescreen widens the camera and asks Game for the neighbouring rooms' tile maps as static
  backdrop. No objects are simulated in neighbouring rooms.
- Hi-res packs replace tiles and sprites at asset lookup, keyed by original tile hash. Game code
  never knows.

### Event seam

Game emits named events at points the disassembly already has as routine boundaries: room load,
object spawn, damage, item get, text box open, save. Built-in QoL features (fast text, run button,
quick swap) are event listeners, not edits to game logic.

### Asset seam

A virtual filesystem with layered overrides: extracted originals at the bottom, then enabled packs
in order. Every asset read goes through it.

### Mod levels

- Level 1, built-in toggles. A settings menu writes `Features`. Ships with milestone 5.
- Level 2, data packs. A folder or zip with a `pack.json` manifest and files the asset seam
  overlays. No new engine surface.
- Level 3, scripted mods. Lua 5.4 embedded. Scripts subscribe to the event seam and read or write
  state through a curated API, never raw memory. Off by default, per-mod enable, no network or
  filesystem access from scripts.

### Both games

Game is compiled once with the runtime `game` field and the launcher picks. The password system
between the games is ordinary game logic and comes with the port.

## Platforms, builds, and tooling

- SDL3 for windowing, Metal-backed rendering on Mac, audio, game controllers, and the Android
  activity and touch input.
- macOS: CMake produces an `.app`, signed and notarised, distributed as a DMG. Universal binary.
- Android: Gradle project wrapping the C code via NDK and CMake with SDL's Java shim. Touch
  overlay for the eight buttons. Sideloaded APK first.
- Linux and Windows builds come from the same CMake setup and run in CI.
- Tooling: CMake, Clang, `ctest`, Python only for the accessor and hook table generators, GitHub
  Actions for the verification suite.

### Repo layout

```
oracles/
  CMakeLists.txt
  src/core/       SM83, MBC5, timers, joypad (scaffolding)
  src/game/       ported routines, mirrors disasm file names
  src/hooks/      address to function table, asm_call bridge
  src/platform/   renderer, audio, save, SDL window
  src/frontend/   menus, settings, mod loader
  tools/          symbol and accessor generators
  tas/            input movies, reference hashes, attribution
  android/        Gradle project
  docs/           design, porting notes, progress tracker
  ref/            oracles-disasm submodule
  roms/           gitignored, user-supplied
```

## Milestones

Each milestone gets its own spec and plan when reached. Estimates are wall-clock for the agent
doing the work, assuming steady sessions.

1. **Scaffold.** Repo, CMake, SDL3 window, SM83 core, MBC5, TAS player. The real ROM boots and
   plays the full Ages TAS on the interpreter, and the reference hashes are recorded. Zero routines
   in C. About 2 weeks.
2. **Boot to title in C.** Hook table, verify mode, accessor generator, reimplemented renderer.
   Port reset through title screen plus the interrupt handlers, DMA, and graphics loaders they
   depend on. Title screen pixel-identical to the reference. 3 to 4 weeks.
3. **Playable engine.** Object system, Link movement, collision, rooms, text, items, first
   dungeon. Roughly the contents of `code/`. Interpreter still runs individual enemies and
   cutscenes. 2 to 3 months.
4. **Content complete.** Per-object code, cutscenes, audio driver. Interpreter deleted. Full TAS
   passes on pure C. 2 to 3 months.
5. **Ship.** Struct refactor, asset extraction at first launch, level 1 toggles, widescreen, save
   states, Mac DMG, Android APK. About 1 month.
6. **Seasons, data packs, Lua.** Separate specs.

Biggest risk: milestone 4 volume. Mitigation: it is repetition of patterns milestone 3 establishes,
and the verification suite makes each unit small and independently checkable.

## Legal

The app never ships the ROM or extracted assets. Users supply their own ROM. TAS movies are
redistributed with attribution to their authors under TASVideos' terms.
