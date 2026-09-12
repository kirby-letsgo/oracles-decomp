# Progress

Updated 2026-09-12. Newest entries at the top of each section.

## Where things stand

- Milestone 3 (readable C engine) started 2026-09-09: plan in
  `docs/plans/2026-09-09-m3-readable-engine.md`. Goal: the disassembly's `code/` tree (55,673
  lines) as readable C, one file per disassembly file, named RAM, real parameters, verified per
  routine against the transliteration. Progress: 2,310 routine hooks rewritten across fifteen code
  banks; bank 0 is fully readable C, gates green on the whole movie after each batch. Whole-movie
  `--verify-hooks-continue` runs passed on the batch 23 build (49.5M hook calls, 0 failures)
  and the batch 24 build (45.1M calls, 0 failures). Every plain routine in bank 0 is now
  rewritten. Batch 78 closed the last reporting gaps: 25 executable entry points became readable
  hooks, `pushDirectionData` was removed from the routine registry because it is data, and the only
  remaining non-hook row is the already-handwritten RAM routine `wRamFunction`. Regeneration now
  deletes the empty `gen_bank00.c`. Phase 3 started with batch 26 doing the thread state
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
  the bank-2 minimap popup-selection and dungeon-map scrolling/menu-rendering cluster; batch 76
  added the safe forest-fairy and completed-fairy dispatcher/state paths; batch 77 added all safe
  Nayru-singing state entries while retaining the two room-loading states in generated C; batch
  79 added the bank-2 gale-seed menu, map-menu state flow, minimap room selection, dungeon key and
  floor bookkeeping, and room-text resolver cluster; batch 80 added the complete safe pregame-intro
  root and its thirteen-state dispatcher; batch 81 added the safe Black Tower explanation and
  completion states plus their shared graphics dispatcher; batch 82 began the final late-cutscene
  pass, and batches 83–86 completed the safe dungeon, timewarp, Black Tower, Flame of Sorrow, and
  Zelda-kidnapped state clusters. Batch 87 added the eighteen independently hookable standard-text
  states and exits; the thread-starting textbox root remains generated. Batch 88 completed the
  text-option and inventory-text state machines and removed their cursor-delay data label from the
  executable routine registry. Batch 89 added bank 1's row/VBlank support, palette-transition,
  following-Link, screen-reload, and object-enable helpers. Batch 90 completed a bank 2 file-select
  and text-input rendering/input cluster and removed twenty-two associated sprite/table data rows
  from the executable routine registry. Batch 91 added the bank 2 status/menu bodies, common-GFX
  loader, equipped-item and treasure-tile renderers, and the complete heart-display path.
  Batch 92 added bank 1's room-transition state machine, camera-position calculation, screen
  shake, scroll-register update, and room-darkening paths.
  Batch 93 completed the horizontal/vertical scrolling-transition state machine and its camera,
  Link-position, follower-reset, and transition-finalization helpers.
  Batch 94 completed the remaining vertical-scroll path and added object-category clearing,
  compass-sound selection, and shocked-Link animation handling.
  Batch 95 added the fall-through-floor warp, cutscene 17, wave-scroll initialization, and the
  safe leaves around cutscene 15's generated thread-switching core.
  Batch 96 added the bank-2 inventory-menu dispatcher, initialisation, item-text selection,
  equipping, and subscreen input paths.
  Batch 97 added the complete bank-2 ring-appraisal dispatcher, initialization, prompt,
  payment, appraisal, refund, redraw, and delayed-exit paths.
  Batch 98 added the safe bank-2 secret-list input, scrolling, cursor, text-selection, bulk-name,
  and secret-data lookup paths while retaining the thread-starting initialization in generated C.
  Batch 99 added the bank-2 ring-list selection, ring text, ring-box insertion/search, page-scroll,
  and cursor-movement state machine through its direction-offset table.
  Batch 100 completed the ring-menu rendering and selection helpers. Batch 101 added the Game Link
  menu state machine, Link/file rendering, save/quit input and sprite helpers, and fake-reset states.
  Batch 102 added the remaining safe file-select mode 1/3/5 state and text-speed menu paths.
  Batch 103 added inventory state dispatchers, directional input, ring-box traversal, and submenu
  cursor rendering. Batch 104 added the seed-and-harp submenu renderer, equipped-ring marker,
  stored-item grid, and subscreen-1 treasure/ring drawing paths; four adjacent sprite/position
  tables were removed from the executable routine registry. Batch 105 added the subscreen-2
  treasure and heart-piece renderer, tilemap rectangle/display helpers, and inventory/harp sprite
  rendering; two adjacent data rows were removed from the executable routine registry. Batch 106
  added the bank-4 toggle-block, underwater Jabu, shutter, chest, switch, and single-tile
  substitution paths; five embedded tile-data rows left the executable routine registry.
  Batch 107 added the remaining file-select mode helpers, blank submenu sprites, the dungeon-map
  tilemap and compass paths, and textbox initialization; five adjacent data rows left the
  executable routine registry.
  Batch 108 added bank-2 item/status/ring rendering helpers and bank-4 tile-write, changed-tile,
  interleaved-tile, and VBlank queue paths; the adjacent VRAM-address table left the executable
  routine registry.
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

- 2026-09-12: milestone 3 phase 5 batch 108 (23 routines): added bank 2's equipped-item,
  item-icon, status-bar map, extra-heart address, and ring-tile rendering helpers, plus bank 4's
  changed-tile queue, VRAM write, subtile-address, interleaved-tile, and VBlank tile-write queue
  paths. Twelve local entries received stable aliases and the adjacent VRAM-address table left the
  executable registry. The translation preserves real AF/BC/DE/HL stacks, exact RST `$10`/`$18`
  return addresses, banked WRAM restoration, and cross-bank readable-call routing. Integration
  threaded the public hooks' entry SP through static bank-0 helpers after `CALL_C` made their
  nonlocal path reachable. Two independent instruction-level reviews approved the corrected code.
  Gates: lint 0, 30k verify 0 mismatches across 4,802,987 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 107 (19 routines): added bank 2's remaining
  file-select mode helpers, blank item-submenu sprites, dungeon-map scrollable-tilemap generator,
  compass tile selector, and floor-address calculation, plus bank `$3f` textbox initialization.
  Seven local entries received stable aliases and five adjacent sprite, map, and textbox data rows
  left the executable registry. The translation preserves all physical fallthroughs, banked WRAM
  switches, real AF/BC/HL stacks, RST `$10`/`$18` return addresses, and direct palette-loading
  tail. Two independent instruction-level reviews found no defects. Gates: lint 0, 30k verify 0
  mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and quirk suites
  8/8.
- 2026-09-12: milestone 3 phase 5 batch 106 (23 routines): added bank 4's toggle-block,
  underwater Jabu, entering-shutter, opened-chest, switch, and conditional single-tile substitution
  paths. Seventeen local entries received stable aliases and five embedded tile-data rows left the
  executable registry. The translation preserves the shutter and flag-test stacks, conditional
  call timing, the RST `$18` return address, local-entry state, and direct replacement tails. Final
  review also replaced six raw RAM/HRAM literals with their canonical room-layout and scratch-byte
  symbols. Two independent instruction-level reviews approved the corrected code. Gates: lint 0,
  30k verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and
  quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 105 (20 routines): added bank 2's subscreen-2
  essence and heart-piece renderer, ring-box capacity lookup, tilemap rectangle and treasure-display
  helpers, and inventory and harp sprite rendering paths. Thirteen local entries received stable
  aliases and two adjacent data rows left the executable registry. The translation preserves the
  real BC, DE, and HL stacks, exact RST `$10`/`$18` return addresses, branch timing, physical
  fallthroughs, and direct renderer tails. Two independent instruction-level reviews found no
  defects. Gates: lint 0, 30k verify 0 mismatches across 4,688,901 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 104 (18 routines): added bank 2's seed-and-harp
  submenu renderer, seed-index and placement helpers, equipped-ring marker, stored-item grid, and
  subscreen-1 treasure, ring-box, and ring drawing paths. Eleven local entries received stable
  aliases and four adjacent sprite/position tables left the executable registry. The translation
  preserves the real BC, DE, and HL stacks, all RST `$10`/`$18` return addresses, and the direct
  sprite/ring-renderer tails. Two independent instruction-level reviews found no defects. Gates:
  lint 0, 30k verify 0 mismatches across 4,688,901 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 103 (27 routines): added bank 2's inventory state-2
  and state-3 dispatchers and states, directional-input helpers, ring-box cursor traversal, submenu
  item selection, and all three cursor renderers. Seventeen local entries received stable aliases
  and ten data/table labels left the executable registry. Review corrected eight unconditional-JR
  burns and five endpoints, restored the `$588e` RST `$10` opcode burn, realigned the `$58b1`
  instruction sequence and call return PC, named the bank-4 tilemap address, and removed duplicate
  ported-root rows. Two independent final reviews approved the corrected code. Gates: lint 0, 30k
  verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and quirk
  suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 102 (21 routines): added bank 2's safe file-select
  mode 1, mode 3, and mode 5 state paths, text-speed menu input and cursor rendering, and nineteen
  independently hookable local entries; the mode-1 state that restarts the current thread remains
  generated. Three table/data labels left the executable registry. Review changed unconditional
  jump burns to `CYC` and split the shared cursor-adjust loop so `$4350` returns to the exact
  `$4344` collision block without repeating the `$4341` zero precheck. Two independent final
  reviews approved the corrected timing, dispatchers, RST stacks, and integration. Gates: lint 0,
  30k verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and
  quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 101 (27 routines): added bank 2's Game Link menu
  dispatcher and seven states, Link/file sprite rendering, save/quit input and sprite helpers, and
  the fake-reset dispatcher and states. Eighteen local entries received stable aliases and one
  sprite-table label left the executable registry. Review corrected the Link animation source from
  `$cbbd` to the ROM's `$cbb6`, then shortened the `$4d27` unconditional-JR burn to its physical
  two-byte endpoint at `$4d29` instead of its `$4d2b` target. Two independent final reviews
  approved the corrected timing, calls, stacks, dispatchers, and registry integration. Gates: lint
  0, 30k verify 0 mismatches across 4,688,896 calls, full replay state `64bddd0dfe384126`, normal
  and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 100 (26 routines): completed bank 2's ring-menu
  rendering and selection helpers, including cursor sprites, equipped-ring markers, ring-box and
  list drawing, page counters, displayed text, and all eight independently hookable local paths.
  Six sprite/table labels left the executable registry. Review removed a duplicate RST `$18`
  return push and restored AF on the text helper's deliberate caller escape. Two independent
  final reviews approved the corrected stack, timing, data boundaries, and caller retargets.
  Gates: lint 0, 30k verify 0 mismatches across 4,688,896 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 99 (24 routines): bank 2's ring-list selection and
  scrolling core, including ring text and descriptions, ring-box insertion/search, page changes,
  both scroll directions, and list-cursor movement. Twelve local entries received stable aliases;
  the direction-offset table left the executable registry. Review corrected the appraisal delay
  helper call to `CALL_C` so its deliberate double-pop return unwinds the caller, and restored two
  new cases in the already-readable ring-menu dispatchers. Two independent final reviews approved
  the exact stack, timing, RAM naming, and generated integration. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 98 (18 routines): bank 2's safe secret-list state,
  scrolling, cursor, text-selection, bulk-name loading, and secret-data lookup paths. Twelve local
  entries received stable aliases; the thread-starting initialization remains generated. Cursor
  sprite bytes and the linked-secret table left the executable registry, reducing its total by two.
  Reviews restored an unassigned `swap a` result, retained a promoted-local interpreter boundary,
  and replaced raw/union low-byte operands with the semantic secret-menu fields. Gates: lint 0,
  30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 97 (24 routines): bank 2's ring-menu dispatcher and
  complete appraisal flow, including initialization, prompts, payment, appraisal, refunds,
  redraws, and delayed exit. Five local entries received stable aliases. The promoted local
  dispatcher retained its interpreter boundary, and the delay helper preserves its real `pop af`
  plus grand-caller return. Two independent reviews approved the instruction timing, jump-table
  stacks, static tails, canonical RAM names, and registry closure. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 96 (24 routines): bank 2's inventory-menu dispatcher,
  initialization, item-text selection, equipping, and three subscreen input paths. Seventeen local
  entries received stable aliases. The targeted verifier found that two branches to the shared
  item-text tail skipped the tail's physical `jp`, leaving them four cycles short; the whole-movie
  replay then exposed a separate interrupt-boundary change where a newly global local helper needed
  `CALL_ROM` at its former interpreter boundary. Independent re-review approved both fixes. Gates:
  lint 0, 40k verify 0 mismatches (6,500,238 calls), standard 30k verify 0 mismatches (4,688,896
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 95 (16 routines): bank 1's fall-through-floor warp,
  cutscene-17 dispatcher and states, alternate-line wave-scroll initialization, and eight safe
  cutscene-15 leaves around its generated thread-switching state. Thirteen local entries received
  stable aliases and the cutscene-17 warp destination left the routine registry as data. Quality
  review corrected a `call nz` to use `CALL_C_CC` and replaced a numerically convenient status-bar
  alias with the canonical interaction-slot base; re-review approved both fixes. Gates: lint 0,
  30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 94 (21 routines): bank 1's remaining vertical scrolling
  substate and row update, object-category clearing helpers, compass-sound selection, and shocked-
  Link animation paths. Twelve local entries received stable aliases and one transition-data row
  left the routine registry. Quality review replaced six raw object-slot/buffer addresses with
  their canonical RAM symbols; the required re-review approved the focused fix and the full added
  code. Gates: lint 0, 30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 93 (18 routines): bank 1's scrolling-transition states
  4–5, horizontal and vertical setup, per-frame scroll and Link-position updates, transition
  finalization, follower reset, and room-boundary setup. Nine local entries received stable
  aliases, while three data-only labels left the routine registry. Self-review caught an `srl h`
  whose helper result was not written back to H; two independent reviews then approved every
  branch, RST stack, direct tail, shared entry, and named RAM access. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 92 (30 routines): bank 1's room-transition states 0–3,
  boundary and animation initialization, transition-direction and water checks, camera-position
  calculation, screen shake, scroll-register updates, and room-darkening checks. Seventeen local
  entries received stable aliases. Independent review corrected RST `$18` selection, shared-state
  entry routing, three no-carry branches, and the `$420d` fallthrough; the first gate then exposed
  the taken `jr z` at `$4117` using fallthrough timing, producing identical registers but a
  one-cycle verifier deficit. Gates after the fix: lint 0, 30k verify 0 mismatches (4,689,284
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 91 (28 routines): bank 2 status-bar hide/show and
  menu/palette-copy bodies, heart-beep timing, common graphics loading, equipped-item and treasure
  tile rendering, and the complete file-select/in-game heart-display path. Fourteen local entries
  received stable aliases and four older readable callers were retargeted after regeneration.
  Independent review rechecked every conditional branch side, the palette-loop SVBK writes, and
  the real BC/HL stack nesting; unlike batch 90's RST `$18` wrapper, the local RST `$10` helper at
  `$5479` does not own its return push, so the explicit `$547a` push is required. Gates: lint 0,
  30k verify 0 mismatches (4,717,640 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 90 (29 routines): bank 2 file-display addressing,
  file-select input/cursor movement, name-buffer handling, name/secret input cursor rendering,
  character graphics, entry-cursor positioning, heart/death-counter drawing, and acorn-cursor
  rendering. Eight local entries received stable aliases; regeneration removed twenty-two
  associated sprite/offset/table data labels from the routine registry. Review corrected two
  conditional calls that needed `CALL_C_CC` and an RST `$18` path that pushed its return PC twice.
  The first gate then exposed a taken `jr nc` at `$4817` that still executed its fallthrough
  `ld c,$00`; the first hook mismatch was frame 523 and the TAS state diverged at frame 540.
  Gates after the fix: lint 0, 30k verify 0 mismatches (4,717,623 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 89 (30 routines): bank 1's screen-scroll row and
  VBlank-queue helpers, tile-row copy and wave/big-buffer initialization, palette-transition
  pipeline, following-Link path maintenance, screen-reload clearing, and the object-category
  enable helpers. Six local/collision-safe entries received stable aliases. Regeneration removed
  the old generated `clearMemoryOnScreenReload`, so its existing readable caller was retargeted to
  the new hook. Independent review approved every RST return PC, real stack pair, SVBK transition,
  physical fallthrough, and shared tail. Gates: lint 0, 30k verify 0 mismatches (4,717,623 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 88 (21 routines): the complete text-option and
  inventory-text state machines, including nineteen newly aliased local entries and the full
  character/space/address/DMA fallthrough chain. The textbox cursor-delay table left the routine
  registry because it is data. Self-review corrected a taken branch that must skip the status
  update at `$4d8a` and restored taken timing on the `$4e25` control-code branch; independent
  review approved the final instruction boundaries, real RST stack, and direct tails. Gates:
  lint 0, 30k verify 0 mismatches (4,716,239 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 87 (18 routines): the standard-text textbox state
  machine, including every independently hookable state, its shared finish/exit paths, and the
  non-exitable-text branch. The thread-starting textbox root and `updateText` dispatcher remain
  generated. Review caught the shared RST `$10` carry path applying `inc h` during the preceding
  conditional-return burn and split the effects at their physical instruction boundaries. Gates:
  lint 0, 30k verify 0 mismatches (4,716,219 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 86 (26 routines): the Zelda-kidnapped dispatcher,
  state runner, and twenty-three safe substates/helpers. Twenty-four local entries received stable
  aliases. State F preserves its real call returning to the still-generated state 10 at `$7aca`,
  then hands off with the live stack; the `$7b48` mini-dispatch also remains generated. Quality
  review found fourteen raw generic-cutscene WRAM operands and replaced them with their existing
  `ram.h` names; lint did not flag the readability regression. Gates: lint 0, 30k verify 0
  mismatches (4,715,002 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 85 (17 routines): `func_782a` plus the Flame of Sorrow
  dispatcher, initialization, all twelve substates, and its local sound helper. Thirteen local
  entries received stable aliases. The state-1 path preserves a real call at `$78b5` followed by
  physical fallthrough into substate 2, while the adjacent Zelda-kidnapped root remains generated.
  Regeneration removed the `$7978` warp destination from the routine registry because it is data.
  Independent reviews approved both RST stacks, conditional returns, call frames, and static tails.
  Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 84 (25 routines): twelve safe timewarp states and
  dispatch bodies plus thirteen safe Black Tower escape states and helpers. Twenty-one local
  entries received stable aliases. The thread-switching timewarp and escape roots remain
  generated, and the selected timewarp paths tail directly into the existing generated
  `timewarpCutscene_incCBB3`. Independent reviews approved the shared `$72b0` body, escape-state
  fallthroughs, and `func_7e40`'s conditional call/jump timing. Gates: lint 0, 30k verify 0
  mismatches (4,715,002 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 83 (24 routines): the `func_701d` cutscene dispatcher
  and states, the dungeon-8 wall-retraction dispatcher and states, two dungeon-2 collapse helpers,
  and the complete safe `func_7168` state cluster through its physical stub return. Eighteen local
  entries received stable aliases. The shared `$71e4` continuation preserves `B = $51` from one
  entry and `B = $52` from the other, while the separate `$71f4` path keeps its real call frame.
  Independent spec and quality reviews approved all RST stack effects, direct tails, shared
  fallthroughs, and cycle boundaries. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 82 (21 routines): the Maku Tree disappearing handler,
  nine safe Nayru-warp states, four safe turn-to-stone states, and seven safe Twinrova-reveal
  states. Twenty local entries received stable aliases; states whose direct-call chains reach
  `refreshObjectGfx` or room loading remain generated. Byte-level review confirmed that the
  Twinrova state-2 non-linked branch deliberately skips the linked-only `func_6f9e` call and
  rejoins at `$6beb`. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 81 (15 routines): seven safe Black Tower explanation
  states, the three-entry `func_6733` graphics dispatcher and its two canonical bodies, and five
  safe Black Tower completion states. Fourteen local labels received stable aliases; the two
  explanation graphics indices sharing address `$6747` deliberately share one hook. The `$6725`
  state preserves its static jump to `func_6f44` and its non-taken fallthrough into `func_6733`.
  Independent review approved all RST stack effects, the WRAM-bank AF pair, conditional calls,
  branch timing, and static tails. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 80 (15 routines): the bank-3 pregame-intro root, its
  thirteen-way state dispatcher, and all thirteen state entries. The state-0 linked-game branch
  reuses the previously readable room-loading body, while state A preserves the real AF stack
  pair around its WRAM-bank switch. Fourteen local labels received stable hook aliases; independent
  review approved every instruction boundary, conditional return, RST dispatch stack effect, and
  static tail. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 79 (24 routines): the bank-2 gale-seed menu root and all
  four states, the map-menu root/state/input flow, minimap display-room selection, dungeon small-key
  and visited-floor bookkeeping, and the complete map room-text resolver/special-code cluster.
  Eight local state and special-code labels received real hook aliases. Regeneration removed the
  old unsuffixed `mapGetRoomText`, so its existing readable cave-popup caller was retargeted to the
  new hook. Review corrected two inverted no-carry cursor-wrap branches and a shared RST `$10`
  helper that wrote `L` during `add a,l`, one instruction before the real `ld l,a`. Gates: lint 0,
  30k verify 0 mismatches (4,714,516 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 78 (25 routines): closed bank 0 by rewriting
  `linkState07` and its two substate entries, nineteen independently reportable decompressor
  fragments, and the three remaining local draw/text tails. `pushDirectionData` was removed from
  the routine registry because its 64 bytes are data, while the existing handwritten
  `wRamFunction` remains the bank's sole external entry. The transliterator now removes stale
  bodyless generated-bank files and CMake tracks the source glob, so `gen_bank00.c` is genuinely
  gone and stays gone. Shared decompressor helpers propagate `CALL_C` nonlocal continuations back
  through the public hook with its original stack baseline. Bank 0 is 652/652 readable entries:
  651 hooks plus one handwritten RAM external. Gates: lint 0, 30k verify 0 mismatches (4,714,516
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 77 (16 routines): all thirteen safe local states of the
  Nayru-singing cutscene plus the three safe global late states. The dynamic root, state D, state
  F, and their room-loading continuations remain generated because they reach thread switches.
  The first build caught three attempted `alu_set`/`alu_res` calls; SM83 `set` and `res` do not
  change flags, so the final code uses direct read/mask/write operations at the same opcode
  boundary. Gates: lint 0, 30k verify 0 mismatches (4,714,559 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 76 (15 routines): the bank-3 forest-fairy dispatcher and
  seven safe state/spawn entries, plus the completed-fairy dispatcher and all six of its states.
  Four room-loading states remain dynamic handoffs because they reach the thread-switching
  `refreshObjectGfx`. Post-regeneration review found that rewriting the parent had also removed
  those unsafe locals and two synthetic post-switch continuations from the generated table; seven
  real aliases in `extra.sym` and `ported.txt` preserve their interpreter entrypoints without
  marking them rewritten. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

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
