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

2. **Launch the game**:
   ```bash
   ./build/oracles-run --rom roms/oracle_of_ages.gbc
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
