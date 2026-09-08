# oracles

Native reimplementation of The Legend of Zelda: Oracle of Ages and Oracle of Seasons in C.


## Build

    cmake -S . -B build -G Ninja && cmake --build build
    ctest --test-dir build

## Run

    ./build/oracles "roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc"

ROMs are not included. Put your own US ROM in `roms/`.

Controls: arrows, X = A, Z = B, Enter = Start, Right Shift = Select, F12 = screenshot to `out/`, Escape = quit.

## Verify against the TAS

The core is checked against the console-verified Oracle of Ages movie recorded with BizHawk's
GBHawk. It needs the ROM in `roms/` and the CGB boot ROM as `roms/cgb_boot.bin` (SHA1
`1293d68bf9643bc4f36954c1e80e38f39864528d`).

    ctest --test-dir build -R tas                   # first 20,000 frames, about 12 seconds
    TAS_FRAMES=289518 ctest --test-dir build -R tas  # the whole movie, about 3 minutes
    ./build/oracles-run --rom roms/<ages>.gbc --boot roms/cgb_boot.bin --init-ram tas/gbhawk-wram0.txt \
        --tas tas/ages-consoleverified.inputs --ref-check tas/ages.ref --frame-hash-check tas/ages.frames

`tas/ages.ref` holds a state hash every 60 frames and `tas/ages.frames` a framebuffer hash per
frame; both were recorded from a run that matches GBHawk's memory frame for frame. The reference
emulator setup (BizHawk headless in Docker plus Lua probes) is in `tools/bizhawk/`.
