# Milestone 2 plan: porting routines to C

Goal: replace the game's assembly with C one routine at a time while the console-verified TAS
keeps passing, ending with reset-to-title fully in C. Progress is "routines ported / routines
total" from the disassembly's symbol file.

## Mechanism

- `src/hooks/`: a table of `(bank, address, name, C function)`. `gb_step` checks the table before
  every fetch. In **replace** mode (default) the C function runs at the routine's entry, sets the
  output registers and memory, and returns the M-cycle count the assembly would have taken. The
  hook then pops the return address and burns exactly that many cycles with interrupts serviced at
  their real cycle, so timer, STAT and VBlank handlers still land where they did. The only
  observable change is that the routine's writes all happen at its entry; a handler that reads or
  writes the same bytes mid-routine will show up in the TAS check and that routine gets deferred.
- **Verify** mode (`--verify-hooks`): at the hook, snapshot state, run the C function, snapshot,
  restore, run the original assembly in the interpreter with interrupts masked, snapshot. Any
  difference in registers, WRAM, HRAM, VRAM, OAM, cartridge RAM or cycle count stops the run with
  the routine name and the first differing bytes. Then the assembly's result is kept.
- `--no-hooks` runs pure interpreter (the milestone 1 baseline).
- `asm_call(gb, bank, addr)` runs the interpreter for one unported routine from C, so C callers
  can be ported before their callees. Added when the first caller needs it.
- Memory access from C goes straight to the mirrors (`mem_rd`/`mem_wr`, no cycle side effects);
  cycles are accounted by the returned count.
- `src/game/<file>.c` mirrors `ref/oracles-disasm/code/<file>.s`, same function names, a comment
  per function with bank:address.
- `tools/gen_hooks.py` builds the table from `ages.sym` (wla-dx build of the disassembly) plus a
  list of ported names; until the symbol file exists, addresses come from byte-pattern lookup.

## Gates per routine

1. Cycle count formula written from the assembly and checked by verify mode on the TAS.
2. `TAS_FRAMES=289518 ctest -R tas` passes in replace mode.
3. Frame hashes still match (`--frame-hash-check`), for anything touching VRAM, OAM or palettes.

## Porting order

Each tier is a batch; a tier is done when every routine in it passes all gates.

- **Tier 0, leaf utilities (bank 0):** copy/fill/clear memory (all 8 variants), multiply/divide
  helpers, `getRandomNumber`, flag set/check/reset helpers, `addAToHl`-style pointer math,
  BCD/decimal helpers, `getFreeInteractionSlot`-type slot searches, lookup-table walkers.
  Establishes the workflow. Expected to be interrupt-safe.
- **Tier 1, data loaders:** `loadGraphics.s`, `loadTilesToRam.s`, palette fade and palette
  loading (bank 1), decompression, `loadTreasureData.s`, text table lookups (`textbox.s` data
  side). These write VRAM through buffers and DMA queues, so gate 3 matters here.
- **Tier 2, game-state helpers:** `fileManagement.s` (save slots, checksum), inventory and item
  tables (`parentItemUsage.s`, `treasureAndDrops.s`), `commonTileSubstitutions.s`,
  `interactableTiles.s`, `breakableTiles.s`, `collisionEffects.s`.
- **Tier 3, object system:** `objectLoading.s`, `objectMovementScript.s`, `animations.s`,
  `specialObjectAnimationsAndDamage.s`, `specialObjects.s` (Link), `updateItems.s`,
  `updateParts.s`, `staticObjects.s`. Largest single body of shared logic.
- **Tier 4, rooms and scripting:** `roomInitialization.s`, `ages/loadTilesetData.s`,
  `ages/tileSubstitutions.s`, `ages/roomGfxChanges.s`, `ages/roomSpecificCode.s`,
  `scripting.s`, `bank3Cutscenes.s`, `ages/cutscenes*.s`, `textbox.s` logic side.
- **Tier 5, the kernel:** bank 0 main loop and thread switcher, VBlank/LCD/timer handlers, OAM
  and HDMA queue functions, `serialFunctions.s`. These are the interrupt-sensitive routines; they
  are ported together, last in this milestone, with the hook mechanism switched to whole-frame
  replacement (C owns the frame, interpreter only for unported callees).
- **Milestone 4 material:** `object_code/` (enemies, interactions, parts) and `audio.s`.

Bank 2 and bank 1 miscellany are assigned to tiers by what they touch, when reached.

## Workflow for one routine

1. Read the assembly, write the C version in the matching `src/game/*.c`, return the cycle count.
2. Add the name to the hook list, rebuild, run `oracles-run ... --verify-hooks --tas ...` on the
   first 20k frames, then the whole movie.
3. Run `TAS_FRAMES=289518 ctest -R tas` in replace mode.
4. Update `docs/progress.md` (count and tier status).
