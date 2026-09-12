# Progress

Updated 2026-09-12. Newest entries at the top of each section.

## Where things stand

- Milestone 3 (readable C engine) started 2026-09-09: plan in
  `docs/plans/2026-09-09-m3-readable-engine.md`. Goal: the disassembly's `code/` tree (55,673
  lines) as readable C, one file per disassembly file, named RAM, real parameters, verified per
  routine against the transliteration. Progress: 1,582 routines rewritten across fifteen code banks;
  bank 0 is fully readable C, gates green on the whole movie after each batch. Whole-movie
  `--verify-hooks-continue` runs passed on the batch 23 build (49.5M hook calls, 0 failures)
  and the batch 24 build (45.1M calls, 0 failures). Every plain routine in bank 0 is now
  rewritten; what is left there is the thread kernel, the interrupt handlers, the thread entry
  points and the routines copied to RAM (phase 3), plus `linkState07` (a jump table with no
  callers) and `pushDirectionData` (data). Phase 3 has started: batch 26 did the thread state
  helpers, the serial handlers, the timer interrupt and the four interrupt vectors, batch 27 the
  VBlank handler and its function queue, batch 28 the LCD interrupt family, batch 29 the boot
  chain and the main loop/thread switcher itself, batch 30 the six thread entry points
  (`textThreadStart`, `fileSelectThreadStart`, `thread_1b10`, `introThreadStart`,
  `paletteFadeThreadStart`, `mainThreadStart`). Batch 31 finished the RAM-resident code
  (`hramOamDmaFunction`, `wMusicReadFunction`, `wRamFunction`), and batch 32 began phase 4 with
  object-loading, part-dispatch, and animation-queue routines in banks 12, 11, and 4; batch 34
  completed the remaining simple object-data opcode handlers. Batch 38 completed phase 4: every
  planned object-system source routine is now readable C and verified. Batch 39 opened phase 5
  with room initialization, room-layout drawing, script conditionals, and text-buffer setup;
  batch 40 added the first scripting state/control cluster, textbox mapping, and cutscene helpers;
  batch 41 added room-specific leaf handlers and script state/spawn/property commands; batch 42
  added tileset transfer and vine helpers, script arithmetic/text helpers, and textbox/cutscene
  leaf routines; batch 43 added room-GFX post-load handlers, script text/memory commands, and
  textbox map save/DMA/scroll helpers; batch 44 added tile replacements, script flag/jump
  commands, and the textbox character-display and heart-piece paths; batch 45 added room tile
  state handlers, script room/item commands, textbox option navigation, and text-stack helpers;
  batch 46 continued with Jabu/overworld tile replacements, script tile/call commands, and
  textbox bank-reading, display-speed, and numeric-substitution paths; batch 47 finished the
  textbox source and moved that lane into Twinrova cutscene helpers while continuing vine and
  script condition handlers; batch 48 added ten more room tile-state handlers, completed the
  remaining straightforward scripting commands, and continued the intro/Twinrova cutscene state
  paths; batch 49 completed `roomSpecificTileChanges.s`, added underwater-wave maintenance and
  the small Ages cutscene wrappers, and continued the title/riding-horse intro states; batch 50
  added the safe pirate-ship update path, finished the small cutscene wrappers and `cutscenes2.s`
  leaves, and completed the remaining riding-horse intro states; batch 51 added linked-game and
  endgame cutscene leaf helpers and continued the temple-intro state path; batch 52 added the
  remaining linked-game spawn helpers, endgame tile/inventory helpers, and temple/pre-title
  cinematic states; batch 53 finished the linked-game source's safe routines, added the remaining
  bank-3 temple camera/bar helpers, and opened bank 10's two large cutscene state machines; batch
  54 continued bank 10 and added endgame object-GFX and miscellaneous cutscene helpers; batch 55
  added deeper endgame, Maku/Nayru/intro, and bank-10 black-tower state helpers; batch 56 added
  endgame-state, miscellaneous cutscene, graphics-copy, and final bank-10 cutscene helpers; batch
  57 continued the endgame and three room-change cutscenes and finished `ages/cutscenes.s`; batch
  58 added deeper endgame/Ambi/Jabu states and the Twinrova dispatcher and room setup; batch 59
  added the credits root, clean-seas states, and both Twinrova cutscene-body dispatchers; batch 60
  added the next clean-seas/endgame states, the remaining miscellaneous cutscene dispatcher
  states, and bank-1 transition/cutscene helpers; batch 61 added ten more clean-seas/endgame
  states, the intro dispatcher roots, and bank-1 room-transition helpers; batch 62 added the
  forest-scrambler/eye-puzzle warp paths, the Capcom/title-screen intro states, and the next
  endgame stage-0/stage-1 entries; batch 63 completed the top-level bank-3 cutscene source,
  added the bank-4 warp resolver cluster, and continued the endgame stage-1 states; batch 64
  finished the scripting source's remaining roots, added room-GFX drawing and pirate-room
  dispatchers, and continued the stage-1/endgame-20 states; batch 65 added the remaining core
  room/tileset dispatchers, five safe textbox roots, and seven more endgame-20 entries; batch 66
  added the next six endgame-20 entries, the inventory-text first pass, and eight bank-1 warp and
  timewarp-solidity routines; batch 67 added nine late endgame-20 entries, ten bank-1 cutscene
  dispatcher/state entries, and eight bank-3 game-secret generation and validation routines;
  batch 68 completed the endgame-20 state list, opened the endgame-0f dispatcher, added seed-tree
  refill and toggle-block helpers, and expanded the bank-3 secret encoder/decoder path; batch 69
  added ten bank-4 VBlank copy handlers, the last eligible bank-1 tail routine, and the remaining
  bank-3 secret data loading, ID, and size helpers; batch 70 completed the first twenty bank-4
  VBlank copy handlers, added the dungeon-map item/cursor drawing cluster, and restored the
  endgame-09 root dispatchers; batch 71 added the next ten VBlank copies, the bank-2 map arrow,
  room-visited, warp-site/tree helpers, and the bank-1 toggle-block cutscene state machine; batch
  72 completed the bank-4 VBlank copy list, added tile substitutions, palette-fade handlers, and
  more bank-2 map helpers; batch 73 added the dungeon floor-list, scroll renderer, room-tile
  selector, and a late miscellaneous-cutscene graphics/effects cluster; batch 74 added the
  remaining safe palette-thread handlers and the full fading/color-mixing loops; batch 75 added
  the bank-2 minimap popup-selection and dungeon-map scrolling/menu-rendering cluster.
  Phase 0 done: `tools/gen_ram.py` (1,810 named RAM labels), `src/hooks/rewritten.txt` and
  `<name>_hook` shims in the generator, `--report` readiness reports, `tools/lint_game.py`,
  `setCpuToDoubleSpeed` hand-written (the last interpreter use that was there by design).
  Phase 1 batches 1 to 3: memory, math, flags, RNG, LCD/VRAM setup, object counters,
  visibility, room flags, table searches, consumable counters, object positions.
  Three rules came out of it, all in the porting notes: cycles are burned from the ROM's
  instruction stream (`CYC`, `src/game/cyc.c`) so interrupt dispatch keeps its instruction
  boundary; memory accesses follow the burn of their instruction; the state hash and verify
  skip the thread stacks and the sound driver's timing-phase state, `tas/ages.ref` re-recorded
  from the interpreter, frame hashes checked on `--no-hooks` only.
- Milestone 2 done at commit `cc4602c`: every labelled routine runs as generated C (13,235 hook
  entries); the interpreter executes 7,314 instructions in the whole movie; verify mode, whole
  movie state and frame hashes, and all suites green.
- Milestone 1 done: the core plays the entire console-verified movie (289,518 frames, about 80
  minutes) in sync with GBHawk; reference hashes in `tas/`.
- The playable build works: `cmake --build build-sdl && ./build-sdl/oracles "roms/<ages>.gbc"`.

## How to test

    cmake -S . -B build -G Ninja && cmake --build build
    ctest --test-dir build                      # Blargg, cgb-acid2, unit tests, first 20k TAS frames
    TAS_FRAMES=289518 ctest --test-dir build -R tas   # whole movie against tas/ages.ref (3 min)
    ./build/oracles-run --rom "roms/<ages>.gbc" --boot roms/cgb_boot.bin \
        --init-ram tas/gbhawk-wram0.txt --tas tas/ages-consoleverified.inputs --probe

`--probe` prints each new (group, room) the game reaches with the frame number. `--dump FILE`
writes the same per-frame key bytes and 60-frame WRAM hashes as the GBHawk Lua dump, so
`tools/compare_dumps.py` can diff a run against the reference emulator.

## Timing rules matched so far (all verified against SameBoy and GBHawk with micro test ROMs)

- Speed switch: 32771 M-cycles from the `stop` fetch to the next instruction (SameBoy and GBHawk
  agree), DIV reset at entry, PPU skips 12 dots, timer keeps running.
- GDMA/HDMA: 16 M-cycles per 16 bytes in double speed. HBlank DMA requested while the LCD is off
  transfers one block immediately (not yet implemented, the game never does this).
- LCD enable: first line is 452 dots and starts in mode 0 (no OAM scan).
- Every other line: STAT keeps the previous line's mode 0 and LYC flag for 2 dots, mode 2 runs
  from dot 2 to 82, mode 3 from 82, mode 0 from 82 + mode-3 length (172 + SCX%8 + window + object penalties).
- LY reads 0 during line 153 after its first 4 dots.
- VBlank and LYC interrupt lines rise at dot 0 of the line, two dots before STAT shows it; leaving
  HALT for an interrupt costs one extra M-cycle.
- Object penalties: 6 dots per object plus a per-block scroll penalty; none on the first line after
  LCD enable and none for X >= 168 (30 micro-test variants agree with SameBoy and GBHawk).
- Interrupt dispatch from HALT and from running code: 5 M-cycles, identical to hardware.
- Boot: the real CGB boot ROM with GBHawk's 13-byte GBA patch, WRAM/HRAM initialised from a GBHawk
  frame-0 snapshot (`tas/gbhawk-wram0.txt`).

## Open problem

- Verify mode reworked (2026-09-09, evening): only routine-level entries are verified, nested
  verification samples the first 4 calls then every 256th, and interrupt dispatch is suppressed
  on both sides of a verification. 3,000 frames take 8 s again (1,500 did not finish in seven
  minutes before).
- Milestone 2: every routine the disassembly labels as code is hooked: 13,235 entries (routine
  labels, local labels, and synthetic entries after thread switches, after calls into
  thread-switching routines, and at cross-routine jump targets) generated by `tools/transliterate.py` into `src/game/gen_bankXX.c`, plus
  `src/game/ram_code.c` for the one piece of dynamic RAM code. On the first 20k frames the
  interpreter executes 1,114 instructions where it executed 51.6M before today; the whole movie
  interprets 7,314 instructions in total (`PCHIST` profile). Gates: whole-movie replay with state and frame hashes,
  routine diff (`--verify-hooks-continue`), all suites. Still interpreted: `stop` routines,
  routines the generator skips for illegal opcodes (data mislabelled as code), and the
  instruction sequences of unlabelled code the profile has not reached yet. The C is a
  cycle-annotated transliteration (gotos for jumps); the idiomatic rewrite comes after 100
  percent, as the design says. Order was `docs/plans/2026-09-08-m2-routine-porting.md`, then the
  profile.
- LCD transition timestamps now match GBHawk with a constant 1 M-cycle offset over the first
  12,000 frames (was drifting by up to 94).

## Fixed today

- Entry points for local labels, after `ld sp`, after calls into thread-switching routines and
  at cross-routine jump targets; hand-off continues inline until the frame's own return; hooks
  for the interrupt vectors and for code running from HRAM/WRAM (`src/hooks/extra.sym`);
  `tas/ages.frames` re-recorded (it predated the exact-cycle sampling that `ages.ref` uses).
- Kernel batch (halt/ei/reti in C): the dispatcher pushed a stale PC for a second interrupt
  serviced at the same boundary; the generator followed a bank-switching `jp` into bank 0 bytes;
  verify mode hung on thread switches inside interpreter call-outs. All three recorded in the
  porting notes.
- Long C routines (cutscene loops) ran past the 16-entry sample ring and dropped frames from the
  frame-hash stream, which showed up as a frame-hash mismatch at frame 9877 with identical
  screenshots. Frame samples are now delivered through `gb->frame_cb`, from `gb_run_frame` and
  from `gb_tick` when the ring is full; the headless runner and the TAS test consume samples in
  the callback.
- The generator followed `call cc` targets as reachable code, decoding switchable-bank callees as
  bank 0 bytes; `updateAllObjects` had silently dropped out of the ported set because of it.
- `tools/bisect_hooks.py` bisects a replay failure to one routine with `HOOK_ONLY`.
- Cartridge SRAM must start filled with 0xFF (GBHawk, and most flash carts). Ours started zeroed,
  so the save-file checksum check took a different branch in each of the six save-slot scans and
  cost 16 M-cycles more per scan. That was the entire "drift per loading window".
- GDMA costs one extra M-cycle per transfer on top of 16 per block (double speed).
- LYC STAT interrupt line now rises at dot 0 of the line (was 2 dots late), which shifted the
  game's HBlank poll loop by one 7-cycle iteration on every mid-frame register update.
- STOP stall was 1 M-cycle too long (32770 instead of 32769); the old micro test counted base
  cycles across the switch.

## Tooling

- `tools/lockstep.c`: runs our core and SameBoy on the same inputs, diffs WRAM per frame.
- `tools/make_timing_roms.py`: generates the micro test ROMs in `tests/timing/`.
- `tests/timing/*.c`: harnesses that print per-cycle PPU/interrupt timelines from our core.
- BizHawk runs headless in Docker (see `docs/porting-notes.md` once written up) with Lua scripts in
  the scratchpad that dump memory or PC timestamps per frame.

## Done

- 2026-09-12: milestone 3 phase 5 batch 75 (22 routines): the bank-2 minimap popup-data loader,
  popup-type dispatcher and all twelve selected popup handlers, dungeon-map direction and
  scrolling states, and the map-menu tilemap/sprite rendering tails. Existing readable arrow
  drawing now calls the new scroll checks directly. Cross-review found that the shared RST `$00`
  helper combined `add l` with `ld l,a`, the untaken `jr nc` with `inc h`, and `ld l,a` with
  `jp hl`; splitting every effect back onto its physical opcode restored exact interrupt-visible
  boundaries. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 74 (18 routines): the bank-1 palette-thread stop and
  refresh paths, remaining safe fade handlers, fading-palette calculation, BG palette mixing,
  named loop/helper entries, and the shared counter. Existing readable handlers were retargeted
  from their removed generated stop/counter symbols. Cross-review caught both `jr nc` timing
  directions in the color-mixing multiply loops reversed: no-carry is the taken path that skips
  the add. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 73 (22 routines): twelve bank-2 dungeon-map floor-list,
  tile drawing, scroll update, room-tile selection, and floor-visibility entries, plus ten bank-3
  miscellaneous-cutscene VRAM, lightning, scrolling, and OAM helpers. The dungeon scroll routine
  preserves the ROM's intentional missing `ret` and falls through into the room-tile selector.
  Cross-review caught an unconditional two-byte `jr` at `$68a2` burned through `$68a5`, one byte
  into unreachable code; its correct endpoint is `$68a4`. Gates: lint 0, 30k verify 0 mismatches
  (4,706,029 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal
  and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 72 (28 routines): the final two bank-4 VBlank copy
  handlers, eight tile-substitution routines, ten bank-1 palette-fade handlers, and eight bank-2
  time-portal, unvisited-map, flag, and Link-position routines. Integration retargeted older
  readable callers after `replaceTiles` and `dungeonMap_getLinkIconPosition` lost their generated
  symbols. Cross-review restored the final one-byte `ret` burn in the map-clear loop; the first
  full replay then exposed the shared RST `$10` carry path applying `inc h` and `ret` one opcode
  early, which diverged only at frame 286,260. Correcting those instruction boundaries made the
  full movie clean. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls), whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 71 (33 routines): ten more bank-4 VBlank copy
  handlers, fourteen bank-2 dungeon-map arrow, room-visited, sprite, warp-site/tree, and named
  loop/return entries, and all nine reportable entries in bank 1's toggle-block `cutscene02`
  state machine. The initial scout also selected `cutscene0b`, but an explicit
  `rewritten.txt` check found it was already readable and prevented a duplicate implementation
  and count. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 70 (24 routines): ten more bank-4 VBlank copy
  handlers, ten bank-2 dungeon-map item, ownership, floor-symbol, Link-icon, and cursor routines,
  and the four remaining report-clean endgame-09 root/stage dispatchers. Regeneration removed
  the generated `endgameCutsceneHandler_09` function, so its older readable bank-3 dispatcher
  had to retarget the static jump to `endgameCutsceneHandler_09_hook`. Gates: lint 0, 30k verify
  0 mismatches (4,706,029 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8
  in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 69 (18 routines): ten bank-4 VBlank copy handlers,
  bank 1's `cutscene1f`, and seven bank-3 secret-data loading, game-ID, masking, and length
  routines. Cross-review caught the missing real return-address push before
  `loadUnpackedSecretData`'s `rst $00` helper consumed its stack entry; the shared-address type-0
  and type-1 secret cases use one canonical hook plus a readable forwarding shim. Gates: lint 0,
  30k verify 0 mismatches (4,706,029 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 68 (31 routines): the last endgame-20 state and the
  endgame-0f root/state/substate dispatch path, eight bank-1 seed-tree/warp/toggle-block entries,
  and fifteen bank-3 secret encoding, decoding, formatting, and named-local entries. Integration
  promoted every reportable local block even when its report showed zero callers and retargeted
  the older readable bank-3 cutscene dispatcher after regeneration removed the generated
  `endgameCutsceneHandler_0f` symbol. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 67 (27 routines): nine late endgame-20 state/helper
  entries, the bank-1 summoned-Link/cutscene dispatcher and its reportable states/substates, and
  eight bank-3 game-secret generation, packing, cipher, checksum, and validation routines. The
  bank-1 integration promoted named substates to real hook entries and handled the same-address
  `cutscene13`/`tilesetLayoutGroup33` symbol using the generator's canonical alias. Gates: lint 0,
  30k verify 0 mismatches (4,723,535 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 66 (15 routines): six more endgame-20 state/helper
  entries, `doInventoryTextFirstPass`, and eight bank-1 warp/timewarp-solidity routines. Review
  restored two omitted one-byte `ret` burns; the reference replay then caught `ld hl,$d0d4`
  translated as `w7TextAddress` (`$d0d5`), and the corrected `w7TextAddress - 1` passed the former
  frame-32,040 failure. Gates: lint 0, 30k verify 0 mismatches (4,723,535 calls), whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 65 (17 routines): five core room/tileset/surfacing
  dispatchers, five safe textbox roots including the dynamic control-code dispatcher, and seven
  endgame-20 state/helper entries. Cross-review again split the bank-0 `rst $00` vector into its
  true instruction boundaries; integration updated older readable textbox callers, kept the
  unported `initTextboxStuff` static tail on its generated symbol, and used `WP(name)[1]` for
  adjacent bytes of banked RAM symbols. Gates: lint 0, 30k verify 0 mismatches, whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 64 (24 routines): the seven remaining top-level
  `scripting.s` roots, nine room-GFX/pirate-room drawing and dispatch routines, and eight more
  endgame stage-1/endgame-20 entries. Cross-review split two combined pairs in the bank-0
  `rst $00` helper so `add a,l`, `ld l,a`, and `jp hl` retain their individual interrupt
  boundaries; lint also required file-local `CALL_C` helpers to receive the owning shim's saved
  stack pointer rather than reading `gb->sp` themselves. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 63 (20 routines): nine bank-4 VBlank/warp resolver
  routines, the final two top-level `bank3Cutscenes.s` routines, and nine reportable endgame
  stage-1 state/OAM entries. Cross-review approved all instruction bodies and found two older
  readable callers that needed the newly exposed `cutscene_clearObjects_hook`; the integrated
  build also required a CMake reconfigure before the new `bank4.c` entered the globbed source
  list. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 62 (25 routines): nine bank-1 forest-scrambler,
  eye-puzzle, screen-edge, and warp-initiation routines; eight Capcom/title-screen intro
  dispatcher and state entries; and eight endgame stage-0/stage-1 entries. Cross-review caught
  an unconditional `jr` at `$618b` whose burn had used the jump target `$618f` instead of the
  two-byte instruction endpoint `$618d`; the corrected build passed every gate. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 61 (20 routines): ten more clean-seas/endgame
  stage-0 states, the three intro dispatcher/root routines, and seven bank-1 death-respawn and
  room-transition helpers. Review split two combined `rst $00` vector burns so register effects
  occur at their exact instruction boundaries and corrected state F's final `jp` endpoint from
  `$5645` to `$5644`; the integrated build also replaced assembly-only intro aliases with the
  generated thread-buffer base and updated the older bank-0 caller to `runIntro_hook`.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 60 (23 routines): eight clean-seas/endgame
  stage-0 entries, eight miscellaneous cutscene dispatcher/state entries, and seven bank-1
  transition, cutscene-trigger, era/season-display, and grass-animation helpers. Two named local
  helper bodies were promoted to real hook aliases so every reportable routine remains directly
  dispatchable. Cross-review approved all entries. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 59 (17 routines): the seven-entry credits
  handler root/state cluster, eight clean-seas state-machine entries, and the two Twinrova
  cutscene-body jump-table dispatchers. The three dynamic roots model their complete `rst $00`
  table-vector stack and carry paths before `hook_handoff`; cross-review approved all entries.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 58 (18 routines): seven deeper endgame
  substates, nine Ambi-passage/Jabu state-machine entries, and the Twinrova cutscene dispatcher
  and room-initialization state. The Twinrova root now models the complete bank-0 `rst $00`
  vector and hands off only at its dynamic `jp hl`. Integration replaced two more
  disassembly-only `wCutsceneIndex` spellings with raw `$c2ef` and exposed the shared Jabu tail
  through a lint-visible `_hook` wrapper. Gates: lint 0, 30k verify 0 mismatches, whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 57 (20 routines): eight more local endgame
  states, nine Ambi-passage/Jabu/clean-seas cutscene leaves, and the final three entries from
  `ages/cutscenes.s`. Cross-review confirmed all twenty implementations; a parallel lane also
  selected three already-rewritten bank-3 helpers, whose duplicate definitions were caught and
  discarded before regeneration. The two static jumps into thread-capable `func_5d41` remain
  direct C tails as required; only an actual `ld sp,*` boundary uses `hook_handoff`. Gates: lint
  0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 56 (22 routines): ten local endgame state
  helpers, nine miscellaneous bank-3 cutscene and graphics-copy helpers, and the final three safe
  bank-10 cutscene state entries. Cross-review corrected the carry path through the RAM-copied
  `rst $10` body so its not-taken `ret nc`, `inc h`, and final `ret` each burn their own
  instruction before applying effects. Integration also replaced a disassembly-only WRAM alias
  and absolute HRAM constants with the generated/raw address forms accepted by readable C.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 55 (26 routines): eight bank-3 local endgame
  state helpers, nine bank-3 Maku/Nayru/black-tower/pregame helpers, and nine more bank-10
  black-tower state entries. Cross-review caught five direct `jp` instructions whose CYC ranges
  used cycle count instead of their three-byte instruction length; all endpoints were corrected
  before replay. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 54 (23 routines): nine bank-3 endgame
  object-data/object-GFX helpers, seven bank-3 fairy/Nayru/Maku cutscene helpers, and seven more
  bank-10 temple-intro state entries. Cross-review caught a missing `$727c` static-jump burn in
  one bank-10 path. The reference gate then isolated a named-RAM typo where raw `$cbb7` had been
  written as `wTmpcbb4` (`$cbb4`); changing it to `wTmpcbb7` restored movie sync. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 53 (18 routines): the final five safe bank-3
  `linkedGameCutscenes.s` routines, four bank-3 temple camera/black-bar/palette helpers, and nine
  bank-10 local state/helper entries from the temple-intro and black-tower cutscene machines.
  The bank-10 disassembly-local entries received explicit C-safe `__` aliases in `extra.sym`;
  jump-table roots, data, and thread-switching tails remain deferred. Gates: lint 0, 30k verify
  0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 52 (20 routines): seven bank-3 linked-game
  interaction-spawn helpers, three bank-3 endgame tile/inventory helpers, and ten bank-3 temple
  and pre-title cinematic entries. Cross-review updated the older state-5 fallthrough to the new
  state-6 hook. The first whole-movie run then isolated an inverted carry test in
  `flashScreen_body`'s `jr nc`; correcting the C condition restored the reference hash. Gates:
  lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8
  in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 51 (26 routines): eight bank-3 linked-game
  cutscene helpers, ten bank-3 endgame countdown/OAM helpers, and eight bank-3 scrolling-tree and
  temple-intro state entries. Dynamic state-machine dispatchers and thread-switching helpers were
  skipped; the two state-0 triforce labels remain internal readable helpers because they have no
  independent callers. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 50 (23 routines): five bank-1 pirate-ship
  update/load/position/angle helpers, nine bank-1 small-cutscene and `cutscenes2.s` wrappers, and
  nine bank-3 riding-horse intro states and sprite helpers. The dynamic pirate room jump table,
  underwater-surfacing table traversal, and switching cutscene tails were conservatively
  deferred. The state-4 `$4f70` local body remains readable as an internal helper rather than a
  separate hook entry because it has no independent caller. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 49 (29 routines): the final seven bank-4
  `roomSpecificTileChanges.s` entries (including the separately addressable shared bodies), three
  bank-1 underwater-wave routines, ten bank-1 Ages cutscene wrappers, and nine bank-3 title and
  riding-horse intro entries. Registration exposed that disassembly-local `@` names cannot be C
  hook identifiers, so the two title-state local entries received explicit `__` aliases in
  `extra.sym`. Review also corrected an old unconditional `jr` annotation from `CYCT` to `CYC`.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 48 (28 routines): ten bank-4 room tile-state
  handlers, eight bank-C global-flag/NPC-movement/delay script commands, and ten bank-3
  Twinrova and title-intro state helpers. Cross-review caught a conditional `call z` that used
  ordinary `CALL_C` timing; changing it to `CALL_C_CC` restored the taken edge's six-cycle call
  timing before replay. The scripting source now has only its dynamic jump-table dispatcher and
  data/bytecode labels deferred. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 47 (30 routines): ten bank-4 vine and overworld
  tile handlers including `replaceVineTiles`, ten bank-C collision/input/flag/display script
  checks, the final bank-3F textbox routine, and nine bank-3 Twinrova cutscene helpers. Dynamic
  RST dispatchers and one thread-switching cutscene state were deferred. Registering
  `replaceVineTiles` removed its generated symbol, so four older readable callers were updated
  to the `_hook` shim. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 46 (30 routines): ten bank-4 Jabu and overworld
  tile-replacement handlers, ten bank-C script tile/call/jump/collision commands, and ten
  bank-3F textbox bank-reading, display-speed, and numeric-substitution routines. The dynamic
  `handleTextControlCode` and `scriptCmd_jumpTable` entries were conservatively deferred. Once
  `readByteFromW7ActiveBankAndIncHl` became handwritten, an older smart caller had to change
  from the disappearing generated symbol to its `_hook` shim; the integrated build exposed and
  fixed that dependency. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 45 (29 routines): ten bank-4 room tile-state
  handlers, ten bank-C room/item/music script commands, and nine bank-3F textbox option,
  cursor, graphics-buffer, and text-stack routines. The local textbox option call at `$561f`
  keeps its real return-address push before entering the readable helper, and all three source
  groups passed independent cross-review without corrections. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 44 (29 routines): ten bank-4 room tile replacements,
  ten bank-C script flag/jump/sound helpers, and nine bank-3F character-display, option-state,
  and heart-piece textbox routines. Cross-review caught two more cycle-count-as-byte-count ranges,
  a missing fallthrough burn after `ret z`, a backward jump aimed at the wrong C label, and a
  smart `(hooked)` call initially written as `CALL_ROM` instead of `CALL_C`. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 43 (30 routines): ten post-load room-GFX handlers in
  bank 2, ten script text/counter/memory helpers in bank C, and ten textbox map-save, DMA,
  character-timer, arrow, clear, and scroll helpers in bank 3F. Cross-review caught a classic
  byte-count error before integration: the three-cycle `jr` at `$51b7` is two bytes, so its burn
  ends at `$51b9`, not `$51ba` (the first byte of the next routine). Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 42 (28 routines): nine tileset-transfer and
  room-layout helpers in bank 4, nine script counter/text/angle/NPC helpers in bank C, and ten
  textbox-selection and cutscene countdown helpers across banks 3F and 3. Review caught a
  four-cycle omission in the carry path of the local `rst $10` helper: after the vector's
  not-taken `ret nc`, it must burn `inc h` and the final `ret`. Integration also caught two
  source files being replaced instead of extended, because five prior bank-4 hook definitions
  disappeared at link time; the files now preserve both batches. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 41 (24 routines): all twelve bank-12
  `roomSpecificCode.s` leaf handlers, including their shared spinner-allocation body, plus twelve
  bank-C script state, object-spawn, coordinate, angle, speed, and Z-speed commands. A shared
  helper declared `static void ..._hook` fell outside `lint_game.py`'s hook-shim recognizer and
  exposed its `gb->sp` access; making the callable helper a normal hook-style function matched the
  existing shared-body pattern. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 40 (21 routines): the scripting input/menu and
  Link-movement control cluster in bank C; textbox DMA and mapping initialization in bank 3F;
  and small state, allocation, and screen-shake helpers from the bank-3 and Ages misc-cutscene
  sources. A review caught the initial `cutscenes.c` filename before integration: routines from
  `ages/cutscenes/miscCutscenes.s` belong in `miscCutscenes.c`, even though another phase-5 source
  is itself named `cutscenes.s`. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 39 (30 routines): room initialization and random enemy
  placement in bank 2; room-layout rectangle and vine helpers across banks 2 and 4; the first
  script stop/room-flag commands in bank C; and text-buffer clearing/setup in bank 3F. The two
  `rst $10` table advances in `createSeaEffectsPartIfApplicable` exposed a missing taken branch
  inside the shared add-A-to-HL vector: registers matched, but the first 30k verifier reported the
  routine eight cycles short. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 38 (7 routines), completing the phase after the bank-2
  audit: `functionCaller_b02`, the enemy-kill list routines, random-buffer generation, random
  enemy placement, and timeportal spawning. The bank-0 trampoline now uses `CALL_C` while
  preserving the selected-ROM-bank push/pop and its dynamic table hand-off. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds. Phase 4 complete.

- 2026-09-11: milestone 3 phase 4 batch 37 (4 routines): the Link/companion
  riding dispatcher `func_410d`, the pre-object item dispatcher `updateItems`, and the bank-5 and
  bank-6 `linkApplyDamage` bridge/body. Static callers in bank 0 now use `CALL_C`; the item table
  remains a dynamic hand-off to its unreworked item-code targets. A formerly interpreted damage
  trampoline needed its entry-stack capture before being changed to `CALL_C`, since that macro's
  non-return fallback requires `sp0_`. Gates: lint 0, 30k verify 0 mismatches, whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds. Phase 4 complete.

- 2026-09-11: milestone 3 phase 4 batch 36 (7 routines): `updateGameKeysPressed` and
  `updateSpecialObjects` in bank 5, `updateItemsPost`, `updateItemPost`, and `itemCodeNilPost`
  in bank 7, plus `parseStaticObjects` and `objectRunMovementScript_body` in banks 22 and 14.
  The three ROM jump-table dispatchers stay explicit in C, handing off their unreworked dynamic
  targets. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in
  both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 35 (15 routines): movement-script and static-object
  loading in banks 14 and 22; minecart/raft bank-switch stubs; and the Link/companion animation,
  graphics-frame, animation-data, and ring-damage paths in bank 6. The bank-0 animation callers
  now use `CALL_C` for their rewritten bank-6 bodies. A first pass gave the two-byte `jr` at
  `$441c` its three-cycle width, which `burn_rom` rejected immediately at the jump boundary;
  every `CYC` range now follows opcode byte length. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 34 (5 routines): object-data opcodes 0, 6, 7, 9, and A
  in `objectLoading.s`. `objectDataOp9` was checked against the ROM bytes after its first draft
  advanced `HL` one byte too far; the terminal `inc l` is not an auto-increment load. The
  coupled special-object and post-item dispatchers were deferred after their jump-table paths
  failed routine verification, rather than retaining an unverified rewrite. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 33 (15 routines): the object-data parser's setup and
  pointer/allocation flow in `objectLoading.s`: `parseObjectData`, `func_55f8`,
  `parseGivenObjectData_hl`, the two skip-to-op-end loops, `checkSkipPointer`, `skipPointer`,
  `parsePointer`, `continueObjectLoopIfOpDone`, and opcodes 1, 2, 3, 4, 5, and 8. The central
  opcode dispatcher remains generated for the next batch; these routines tail into it directly
  after burning their own static jump bytes. Three raw address spellings initially did not have
  RAM declarations; using the canonical `wNumEnemies`, `wTmpcfc0`, and
  `wScreenTransitionDirection` names restored the required named-RAM convention. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk
  builds.

- 2026-09-11: milestone 3 phase 4 batch 32 (15 routines): the first object-system batch. Bank
  12's object-loading helpers (`read2Bytes`, `readCoordinates`,
  `decEnemyCounterIfApplicable`, `addPositionToPlacedEnemyPositions`,
  `assignRandomPositionToEnemy`, `checkEnemyKilled`), bank 11's part dispatcher
  (`label_11_212`, `updateParts`, `func_11_5e8a`), and bank 4's animation queue/data path
  (`initializeAnimations`, `updateAnimations`, `updateAnimationQueue`,
  `updateAnimationData`, `updateAnimationDataPointer`, `loadAnimationGfxIndex`). Two timing
  corrections were caught by the 30k hook verifier: a loop's taken `jr c` must burn through the
  branch's own end address, not its target (otherwise every part update was 45 cycles short), and
  the shared `rst $10` add-to-HL vector's `ret nc` is a taken conditional return on the no-carry
  path (three cycles short per animation update if burned as not-taken). Gates: lint 0, 30k verify
  0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-10: milestone 3 phase 3 batch 31 (2 routines): the last of the RAM-resident code - `hramOamDmaFunction` (the OAM DMA routine copied to HRAM, at 0xff80) and `wMusicReadFunction` (the music read function copied to $c000). Both use `I(addr, cycles)` with explicit cycle counts, as all RAM-resident code must, since `CYC` walks the ROM at that address and would decode garbage for code that actually lives in WRAM/HRAM. Their hooks replace the generated transliterations; `wMusicReadFunction` is called from the bank 39 sound driver's generated code (`gen_bank39.c`) and `hramOamDmaFunction` from the VBlank handler's rewritten `vblankInterrupt_hook`. With `wRamFunction` already hand-written in `src/game/ram_code.c`, every bank 0 routine is now readable C. Gates: 30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 both trees, lint 0 problems. Phase 4 (object system) is next.

- 2026-09-10: milestone 3 phase 3 batch 30 (6 routines): the six thread entry points -
  `textThreadStart`, `fileSelectThreadStart`, `thread_1b10` (the game-over screen),
  `introThreadStart`, `paletteFadeThreadStart`, `mainThreadStart`. Finishes bank 0's readable-C
  routines bar the RAM-resident code. Two real findings, plus a call-mechanism correction:

  - `ld ($cbb4),a` is 3 bytes and 4 cycles; writing `CYC(a, a + 4)` (the cycle count, not the
    byte count) burned one byte too many, double-counting the next instruction's own cycles.
    `thread_1b10` runs exactly once in the whole movie (the game-over screen), so the mis-burn
    only ever fires once - but it shifted global timing by a few cycles at that exact moment,
    and the state hash didn't show a diverging frame until 54,840 frames later. Bisected with
    `HOOK_SKIP` down to this one routine before finding the actual byte/cycle mixup; the same
    class of mistake as a `jp nn` mistakenly burned as 4 bytes instead of 3 in this same batch,
    caught immediately by `burn_rom`'s own unconditional-jump guard.
  - Five of these six routines call into still-transliterated code (`initTextbox`,
    `updateTextbox`, `b2_fileSelectScreen`, `runSaveAndQuitMenu`, `runIntro`, `paletteFadeHandler`,
    `runGameLogic`) using `CALL_ROM`, which always takes the interpreter; the ROM's own
    transliteration calls all of them with the smart `CALL(...)` macro (interpreter only in
    verify mode or when unhooked, the routine's own C otherwise). `CALL_C` is the rewrite's
    equivalent and is now used for all of them - `CALL_ROM` stays reserved for the one case
    the transliteration itself marks `/* unported */` (a call always taken through the
    interpreter, regardless of hook state).
  - A suspended thread resuming through `_countdownToRunThread`/`_initializeThread` lands on
    the ROM address right after its `call resumeThreadNextFrame` - the loop's own body for
    `paletteFadeThreadStart` and `introThreadStart` (their `jr` loops back to the routine's own
    entry point), but a few instructions *into* the loop for the other four, which no longer
    have a hook there once their parent is rewritten. The interpreter picks it up correctly
    either way; only the first two keep re-entering the readable C on every resume.

- 2026-09-10: milestone 3 phase 3 batch 29 (13 routines): `begin`, `resetGame`, `startGame`,
  `_mainLoop`, `_mainLoop_nextThread`, `_countdownToRunThread`, `_initializeThread`, and their
  five `ld sp`-boundary continuations (`resetGame__afterSp016c`,
  `_nextThread__afterSp091a`, `startGame__afterSp0925`,
  `_countdownToRunThread__afterSp0998`, `_initializeThread__afterSp09aa`). The whole thread
  scheduler and the VBlank wait loop it drives. Two real bugs and one harness gap found:

  - The loop that scans all four thread slots compares `hActiveThread` against the wrong byte:
    the ROM compares against the low byte of the buffer's *end* address
    (`wThreadStateBuffer + NUM_THREADS*8`, which is 0x00), not the buffer's own low byte
    (0xe0) - copying the wrong operand made the loop run until the 8-bit index happened to
    wrap around to 0xe0 again instead of exiting after four threads. Replay diverged from the
    very first frame; found by bisecting a WRAM diff between this build and the last clean one
    frame by frame down to movie frame 0.
  - `ld sp` is a real thread switch and needs a genuine `hook_handoff` there, the way the
    transliteration's own `HANDOFF_UP` does it - not a direct C call to the continuation. A
    direct call keeps every macro that assumes call/return semantics (`HALT`'s halt-bug path
    among them) tied to the wrong `sp0_`, and five addresses that only ever existed as the
    transliterator's synthetic local labels needed their own entries in a new
    `src/hooks/extra.sym` + `ported.txt` + `rewritten.txt` triple so `hook_handoff` had
    something to dispatch to.
  - Nested verify's fallback for a hook that never returns - burn the same number of cycles in
    the interpreter and compare - cannot make sense of a routine built around a `halt` loop
    that only exits when an interrupt handler (itself suppressed during verification) clears a
    flag; the generator already excludes any transliterated routine containing `halt` from
    nested verification, but that exclusion never reaches a hand-written routine reached only
    by direct C calls. A new `src/hooks/rewritten_noverify.txt` (checked by
    `tools/transliterate.py`, same `H` flag as the generator's own halt detection) marks the
    four routines whose call chain reaches the wait loop without a `hook_handoff` in between.
    Whole-movie replay against `tas/ages.ref` was clean throughout; only the 30k routine-diff
    needed this.

- 2026-09-10: milestone 3 phase 3 batch 28 (7 routines): `lcdInterrupt` and its behaviour
  branches (status bar toggle, the seven-entry `rst $00` jump table, the ring menu's two-stage
  textbox timing, the `lcdInterrupt_0bea` palette-final variant). Most of the writing time went
  into getting conditional branch direction right by hand (`jr nc`/`jr nz`/`jr z` taken-vs-not
  determines which C branch gets `CYC` and which gets `CYCT`, and it is easy to swap them without
  the compiler complaining); four branches came out backwards on the first pass and only the 30k
  verify caught it, by way of `lcdVector_hook`'s own comparison against the interpreter - the vector
  routines don't show a per-routine call count (see the porting notes), so they rely on their
  vector's check, not their own.

- 2026-09-10: milestone 3 phase 3 batch 27 (9 routines): `vblankInterrupt`, the VBlank function
  queue (`runVBlankFunctions`, `vblankFunctionRet` and the five queue handlers, including the
  HDMA one that runs 19,567 times in the movie). `jp hl` inside a rewrite is
  `hook_continue(gb, target, sp0)` followed by `return`, the way the transliteration's `HANDOFF`
  is - looping around it instead hung the replay at frame 500. Calls to another routine's C now
  go through `CALL_C`, which mirrors the generated `CALL`: the interpreter in verify mode or when
  the target is unhooked, the C function otherwise. Local labels of a rewritten routine cannot go
  in `rewritten.txt` (the generator drops them with their parent); they run in the interpreter.

- 2026-09-10: milestone 3 phase 3 batch 26 (20 routines): the thread state helpers
  (`threadStop`, `threadRestart`, `restartThisThread`, `stubThreadStart`, the three
  `resumeThread*`), `writeToSC` and the serial handlers, `timerInterrupt`, and the four
  interrupt vectors. Two rules came out of it, both in the porting notes: a static jump tail
  calls the target's C function directly instead of handing off, and `ld (nn),sp` must write
  its low byte one M-cycle before the instruction ends (`burn_store_sp`) or a frame sample can
  land in the gap - that was a one-byte HRAM difference at frame 4140 that verify could not see.

- 2026-09-10: milestone 3 phase 1 batch 25 finishes the plain routines of bank 0: the angle and
  z-speed helpers, weapon/treasure gfx, `enemyStandardUpdate`, `linkApplyDamage`,
  `checkUseItems`, the cutscene special-object code, the enemies-killed list, the random-position
  helpers, the four file routines, `initSound` and the intro cinematic. `checkTreasureObtained`
  was one cycle short until the `ld l,a` at 00:1749 got its own burn; the verify's cycle
  compare caught it, the callers only showed a cascaded off-by-one.
- 2026-09-10: milestone 3 phase 1 batch 24: playSound, the graphics decompressor, palette
  headers, the tileset gfx helper, sprite drawing (drawAllSprites family, terrain effects),
  the VBlank dirty-palette upload, the script engine's asm-call commands, loadRoomLayout
  (rst $00 jump table). `rewritten.txt` may name a routine by its generated C name
  (`playSound_b00`) when the symbol is shared across banks.
- 2026-09-10: milestone 3 phase 1 batches 20 to 23: 518 bank 0 routines rewritten in total
  (animation frame tails, sound timer, OAM sprites, draw queue, text thread entry, menus,
  object gfx loaders, `interBankCall`/`jpHl`/`jpBc`, status bar, treasures, global flags, the
  per-frame object update loops, tileset and room loading). Every register save is now a real
  stack push (`bank_push` included) after `updateAllObjects` showed the thread-state SP
  mismatch; `hook_handoff` tails for `jp hl` dispatch.
- 2026-09-10: milestone 3 phase 1 batches 4 to 19: 415 bank 0 routines rewritten in total
  (collision, positions, angles, speed, tiles, hazards, room flags, camera, treasure, object
  creation); `CALL_ROM` for calls into transliterated code, `burn_rom` guard for ranges that
  run past a jump.
- 2026-09-09 (night): milestone 3 phase 0 and phase 1 batches 1 to 3: 92 bank 0 routines
  rewritten, ROM-walked cycle burns, timing-phase state excluded from the gates.
- 2026-09-09 (evening): all labelled code hooked (12,829 entries); interpreter share on the
  first 20k frames down from 51.6M to 12k instructions; `tools/pc_hist.py` profiling.
- 2026-09-09 (later): bank 0 kernel ported (1494 routines); `halt`/`ei`/`reti` support in the
  generator and runtime; cross-bank reachability and direct bank N to bank 0 calls.
- 2026-09-09: 1356 routines across 16 banks generated and verified (routine diff, whole movie,
  frame hashes); sample-ring callback, duplicate-label and call-cc reachability fixes.
- 2026-09-09: 471 routines generated and verified; hand-off and nested-call rules found (see
  porting notes).
- 2026-09-08 (night): milestone 2 scaffolding. Hook table generated from the disassembly symbol
  file (`tools/gen_hooks.py`, `src/hooks/ported.txt`), transliteration skeletons
  (`tools/asm_skel.py`), replace and verify modes (`--verify-hooks`, `--no-hooks`), 33 routines
  ported and clean on the first 20,000 frames in both modes.

- 2026-09-08 (evening): full movie in sync with GBHawk; references recorded; ROM-gated TAS test;
  BizHawk reference tooling and CLAUDE.md added to the repo.

- 2026-09-08: real boot ROM supplied; GBHawk oracle running in Docker; six timing rules fixed.
- 2026-09-07: scaffold, SM83 core, PPU, APU, SDL3 app, TAS player, Blargg and acid2 passing.
