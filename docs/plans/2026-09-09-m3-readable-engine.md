# Milestone 3 plan: readable C engine

Date: 2026-09-09
Status: approved, in progress
Depends on: `docs/plans/2026-09-08-m2-routine-porting.md` (done at commit `cc4602c`)

## Where milestone 2 left us

Every labelled routine in the ROM runs as generated C (`src/game/gen_bankXX.c`, 13,235 hook
entries). The interpreter executes 7,314 instructions in the whole 80-minute movie. The generated
C is a cycle-annotated transliteration: one `I(addr, cycles)` per instruction, gotos for jumps,
registers as `gb->a`, memory by raw address. It is correct and verified, and unreadable.

The design's milestone 3 ("playable engine, interpreter still runs enemies") is therefore
already exceeded on the "runs" axis and not started on the "C" axis. Milestone 3 is redefined:

**Goal.** The engine core, the disassembly's `code/` tree (55,673 lines, 41 files), rewritten as
readable C: one `src/game/<file>.c` per `code/<file>.s`, same function names, named RAM, native
control flow, real parameters and return values. Each rewritten routine is verified against its
transliteration on the TAS. At the end of milestone 3 no routine from `code/` remains in
`gen_bankXX.c`. `object_code/` (175k lines) and `audio/` (72k lines) stay transliterated until
milestone 4.

**Non-goals.** Removing cycle emulation (milestone 5), the struct refactor of RAM (milestone 5),
rewriting object code or the audio driver (milestone 4).

## The contract of a rewritten routine

The transliteration stays as the reference and the fallback, exactly as the interpreter was in
milestone 2. Callers and callees can be in either form at any time.

1. **Two functions per routine.** The readable one keeps the disassembly name and takes real
   parameters: `uint8_t getRandomNumber(GB *gb)`,
   `void copyMemory(GB *gb, uint16_t dst, uint16_t src, unsigned n)`. A shim named
   `<name>_hook(GB *gb)` is the hook-table entry: it unpacks registers into arguments, calls the
   readable function, packs register outputs (all of them, flags included: verify mode compares
   every register), burns cycles, and pops the return address (`ret_effect`). The shim disappears
   when the last transliterated caller is gone (milestone 4). Register-shuffling helpers whose
   outputs are registers and flags by nature (`addAToDe`, `compareHlToBc`, the BCD helpers) are
   implemented in the shim with the ALU helpers, without a readable twin: a readable caller
   writes `de += a` and counts the cycles itself.
1b. **Timing-phase state is not state.** The assembly leaves push/call garbage below SP that a
   rewrite never writes, readable callers stop pushing return addresses, and the sound driver
   (timer interrupt) advances at a phase that shifts with every coarse burn. The state hash and
   the verify compare skip the sound engine and the thread stacks (`0xc000-0xc2bf`), the sound
   bank (WRAM bank 2, `0xdf80-0xdfff`) and `Hram_Music` (`0xffd8-0xfff2`);
   `tas/ages.ref` was re-recorded from the pure interpreter with that definition on 2026-09-09.
   The frame-hash gate (`tas/ages.frames`, mid-frame raster timing) applies to the pure
   interpreter only from now on: a rewrite changes interrupt latency by a few M-cycles and
   that legitimately moves a handler's register write across a scanline boundary now and then.
2. **Cycles from the ROM's instruction stream.** A rewrite burns cycles with `CYC(from, to)`
   (`burn_rom`): it walks the ROM bytes of the routine it replaces, one instruction at a time,
   and burns each instruction's M-cycles at its own boundary (`CYCT` when the last instruction
   is a taken branch). Interrupt dispatch therefore lands on the same instruction boundary as
   in the transliteration; the experiment of dispatching mid-instruction desynced the
   transliteration itself at frame 6720, and a lump burn per routine desynced the rewrite at
   frame 118380 with no single hook responsible. Every memory access follows the burn of the
   instruction that performs it (`CYC(...); v = mem_rd(...)`), which also keeps buffers the
   VBlank handler or HBlank DMA read asynchronously, and shared state such as `hRng1`, exact.
   The logic between the burns is plain C; the burns are the part that milestone 5 deletes.
3. **Interrupt visibility.** Verify mode suppresses interrupts on both sides, so it cannot see
   a handler observing mid-routine state or a shifted dispatch boundary; the whole-movie replay
   does. When the replay fails and verify is clean, enable the rewritten hooks one at a time
   with `HOOK_SKIP` (the generated jump and fallthrough sites honour it); if no single hook
   reproduces the failure the cause is a dispatch boundary, and the fix is a missing or merged
   `CYC` segment.
4. **Named RAM.** `src/game/ram.h` is generated from the symbol file's WRAM and HRAM labels
   (1,060 labels). `wN` labels (banked WRAM at 0xd000) resolve to the bank's own array, so
   readable code does not switch SVBK; the switch's cycles are still counted. IO registers and
   anything the core intercepts go through `mem_rd`/`mem_wr` as before.
5. **No emulated registers in readable code.** `gb->a` and friends appear only in `_hook`
   shims. A lint script enforces this on every non-generated file in `src/game/`.
6. **No calls into thread-switching routines until phase 3.** A callee that reaches `ld sp`
   abandons the caller's C frame (`hook_handoff` longjmp). Transliterated callers have synthetic
   re-entry points; readable callers do not. The generator already knows which routines switch
   threads transitively (`switches_threads`); the readiness report refuses a candidate that calls
   one until the thread system is native.
7. **One entry per routine.** A routine that other routines jump into (`owner@jumpXXXX`
   entries) or fall through into is rewritten together with them, or the entry becomes a second
   readable function. The readiness report lists incoming jumps.

## Tooling (phase 0)

- `tools/gen_ram.py ages.sym > src/game/ram.h`: `#define wFoo 0xc123`-style addresses plus
  `W8(name)`, `W16(name)`, `H8(name)` accessors and `WBANK(n, name)` for banked labels.
- `src/hooks/rewritten.txt`: names whose transliteration is no longer generated;
  `tools/transliterate.py` skips them (and their locals and synthetic entries) and
  `tools/gen_hooks.py` maps them to `<name>_hook`.
- `tools/rewrite_report.py NAME`: readiness report for a candidate. Prints its assembly with
  per-basic-block cycle sums, callees (marked if hooked, thread-switching, or containing
  `halt`/`stop`), incoming cross-routine jumps and fallthroughs, register inputs and outputs
  inferred from the disassembly's `@param`/`@return` comments where present.
- `tools/lint_game.py`: fails on `gb->[abcdefhl]` outside `_hook` shims, on raw `0xc`/`0xd`
  addresses in non-generated files, and on a `rewritten.txt` name still present in
  `generated.txt`.
- Verify-mode coverage: `--verify-hooks-continue` prints the rewritten routines it never sampled,
  so an unexercised rewrite is visible rather than silently green.

## Gates per batch

1. `--verify-hooks-continue` on 30k frames: 0 failures; the "never called" list names the
   routines the 30k frames did not exercise (the whole movie still runs them).
2. Whole movie in replace mode with `--ref-check tas/ages.ref` (state every 60 frames). Frame
   hashes are the interpreter's gate: `--no-hooks --frame-hash-check tas/ages.frames`.
3. `ctest --test-dir build` (8 suites) and `tools/lint_game.py`.
4. The batch's names are in `rewritten.txt` and absent from `generated.txt`.
5. `docs/progress.md` updated: lines of `code/` rewritten over 55,673.

## Phases and order

Each phase is a batch list; a phase is done when every routine in it passes the gates. Line
counts are the assembly being replaced. Estimates are agent wall-clock in steady sessions.

**Phase 0, tooling.** The five items above, plus hand-written `setCpuToDoubleSpeed` and the other
`stop` routines as `extern` entries (`src/hooks/extra.sym`), which removes the last interpreter
use that is there by design. About 1 day.

**Phase 1, bank 0 utilities and loaders.** Memory copy/fill/clear (8 variants), math, flag
helpers, decimal helpers, random numbers, pointer math, slot searches, `loadGraphics.s` (979),
`loadTilesToRam.s` (362), `loadTreasureData.s` (63), decompression, palette loading in bank 1.
Routines in `loadGraphics.s` that call `resumeThreadNextFrameIfLcdIsOn` wait for phase 3.
About 2,800 lines. 3 to 4 days. Establishes the shim, cycle-counting and RAM conventions; the
porting notes get a "rewriting" section from this phase.

**Phase 2, game-state helpers.** `fileManagement.s` (423), `treasureAndDrops.s` (1,153),
`parentItemUsage.s` (448), `commonTileSubstitutions.s` (318), `ages/tileSubstitutions.s` (247),
`interactableTiles.s` (887), `breakableTiles.s` (271), `collisionEffects.s` (1,516), the data
side of `textbox.s`, `code_3035.s` (153). About 5,700 lines. 5 to 6 days.

**Phase 3, threads and kernel.** The riskiest phase, placed before the object system because
object loading and text yield to other threads. Game threads become native coroutines: each of
the game's threads gets a native stack; `ld sp` in the thread switcher becomes a context switch
to the thread that owns the target stack, and a thread started for the first time gets a context
whose entry pops its start address from the emulated stack and dispatches. Transliterated code
mid-frame keeps its C frame across a switch, which retires the `afterSp`/`afterCall` synthetic
entries and the inline hand-off, and with them the 7,314 remaining interpreted instructions.
Then the main loop, VBlank/LCD/timer handlers, OAM and HDMA queues, `serialFunctions.s` (949),
the thread and bank-switching helpers, and the phase 1 leftovers. About 3,000 lines of bank 0.
4 to 6 days. Detailed design written when reached, as its own section in this file.

**Phase 4, object system.** `objectLoading.s` (679), `objectMovementScript.s` (199),
`animations.s` (174), `specialObjectAnimationsAndDamage.s` (657), `specialObjects.s` (407, Link),
`updateItems.s` (244), `updateParts.s` (80), `staticObjects.s` (142), the object routines of
`bank0.s` (`updateAllObjects`, `updateEnemy`, collision, position and movement helpers) and of
`bank2.s`. About 9,000 lines. 8 to 10 days.

**Phase 5, rooms, scripting and text.** `roomInitialization.s` (756), `ages/loadTilesetData.s`
(365), `ages/roomGfxChanges.s` (696), `ages/roomSpecificTileChanges.s` (1,462),
`ages/roomSpecificCode.s` (197), `ages/pirateShip.s`, underwater code, `scripting.s` (1,316),
`textbox.s` logic side (3,243 total), `bank3Cutscenes.s` (2,175), `ages/cutscenes*.s`, and what
is left of `bank1.s`, `bank2.s`, `bank3.s`, `bank4.s`. About 12,000 lines. 8 to 10 days.

**Phase 6, sweep.** Everything in `code/` not yet listed, then the gate "no `code/` routine in
`gen_bankXX.c`". Lines rewritten equals 55,673. 3 to 5 days.

Total: 6 to 7 weeks of steady sessions, consistent with the design's 2 to 3 months for this
milestone.

## Workflow for one routine

1. `tools/rewrite_report.py NAME`; if it reports a thread-switching callee or an incoming jump,
   pick a different routine or widen the batch.
2. Write the readable function and its `_hook` shim in `src/game/<file>.c`, counting cycles per
   basic block from the report.
3. Add the name to `src/hooks/rewritten.txt`, regenerate (`transliterate.py`, `gen_hooks.py`),
   rebuild, `tools/lint_game.py`.
4. `--verify-hooks-continue` on 30k frames. A mismatch names the routine and the first differing
   bytes; a cycle-count error shows as the reference running past the return.
5. Batch gate: whole movie, `ctest`, progress update.

## Risks

- **Mid-routine interrupt observation** (contract item 3). Bounded: the VBlank handler and the
  LCD handler are the only asynchronous readers, and the replay finds every case.
- **Native coroutines and the emulated stack** (phase 3). The emulated stack stays the source of
  truth for return addresses in transliterated frames, and each coroutine owns exactly one
  emulated stack region, so the mapping from `sp` to thread is a range check. If the mixed mode
  proves fragile, phase 3 falls back to rewriting the whole kernel and the thread bodies' outer
  loops in one batch, keeping synthetic entries until then.
- **Register outputs and flags.** A shim that forgets an output register the assembly's callers
  read shows up in verify mode on the first sampled call, because registers are part of the
  snapshot.
- **Volume.** 55k lines by hand. The mitigation is the same as milestone 2's: small batches, a
  mechanical report per routine, and a gate that runs in 80 seconds.
