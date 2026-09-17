Native reimplementation of The Legend of Zelda: Oracle of Ages and Oracle of Seasons in C.


# Oracles-Decomp Development Setup

## Prerequisites
- macOS (Linux/Windows supported via CMake)
- Xcode Command Line Tools
- Homebrew (for dependencies)
- CMake 3.15+
- SDL3 (via Homebrew)
- Clang

## Installation

1. **Install dependencies**:
   ```bash
   brew install cmake sdl3
   ````

2. **Clone the repository**:
   ```bash
   git clone https://github.com/your-username/oracles-decomp.git
   cd oracles-decomp
   ````

3. **Initialize submodules** (if needed):
   ```bash
   git submodule update --init --recursive
   ````

## Build Instructions

1. **Configure with CMake**:
   ```bash
   mkdir -p build && cd build
   cmake -S .. -G Ninja
   ````

2. **Build the project**:
   ```bash
   ninja
   ````

3. **Run tests**:
   ```bash
   ctest
   ````

## Running the Game

1. **Prepare a ROM**:
   - Place a US ROM in `roms/` (e.g., `roms/oracle_of_ages.gbc`)
   - Ensure it matches the SHA1 hash in `porting-notes.md`

2. **Interactive play (SDL window)**:

   The default `build/` tree only builds `oracles-run`, a headless CLI used for
   verification. To get a playable window, configure a separate tree with
   `-DORACLES_SDL=ON`:
   ```bash
   cmake -B build-sdl -DORACLES_SDL=ON
   cmake --build build-sdl -j4 --target oracles
   ./build-sdl/oracles roms/oracle_of_ages.gbc roms/cgb_boot.bin
   ````
   Note: `oracles` takes positional args (`ROM [BOOTROM]`), not `--rom`/`--boot`
   flags — those flags are only for the headless `oracles-run` tool below.
   The first configure fetches SDL3 automatically if it isn't found via
   Homebrew, so it can take a few minutes.

   Controls: arrow keys to move, `X`/`Z` for A/B, Return for Start, Right
   Shift for Select.

3. **Headless run (no window)**:
   ```bash
   ./build/oracles-run --rom roms/oracle_of_ages.gbc --boot roms/cgb_boot.bin
   ````

## TAS Replay

1. **Run the full movie**:
   ```bash
   TAS_FRAMES=289518 ctest -R tas
   ````

2. **Debug frame hashes**:
   ```bash
   ./build/oracles-run --tas tas/ages-consoleverified.inputs --probe
   ````

## Notes
- ROMs are git-ignored (add to `.gitignore`)
- Use `--no-hooks` for fast verification
- Debug with `--report` for hook statistics
- See `docs/progress.md` for milestone status
