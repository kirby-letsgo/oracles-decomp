# Porting notes

Decisions and hardware sensitivities found while building milestone 1.
Add an entry whenever a TAS sync or verification issue is resolved.

## Boot

- The game seeds its RNG with constants (0x37, 0x0d) at `begin` in `code/bank0.s`. It never reads DIV.
  Boot timing therefore affects only the TAS frame alignment, not game logic.
- GBA detection: `begin` checks A == 0x11 and bit 0 of B. The TAS was recorded in GBA-on-CGB mode, so B must be 0x01.

## Timing

- The audio driver runs from the timer interrupt (`enableTimer` in `code/bank0.s`) with TMA 0x77 in double speed, 0xbb otherwise.
- The game uses KEY1 double speed and HDMA.

## TAS sync (milestone 1)

Findings from syncing the console-verified Ages movie (GBHawk, GBA-on-CGB mode). Each item cost a
desync to discover; keep them when porting routines.

- GBHawk frame model: exactly 70224 base-clock cycles per frame, counted from 8 base cycles before
  the CPU's first fetch (`GRID_OFFSET`). Our `gb_run_frame` uses the same grid. The controller is
  latched only on a rising edge of "in VBlank", which GBHawk defines as mode 1 or LCD off: so at
  each VBlank entry and at the instant the LCD is switched off outside VBlank. A frame with no such
  edge never sees its input (GBHawk counts it as a lag frame; the game has 4,900 of them). The one
  exception: a CPU halted with the joypad interrupt enabled gets the input at frame end. There is
  no frame-end fallback otherwise; adding one made loading screens see Start presses one frame
  early. `--dump` writes the same lag flag as GBHawk's `dump.lua` so the two can be diffed.
- Boot offset: the real AGB/CGB boot ROM plus the game's own init put movie frame 0 exactly
  157 frames before our post-boot frame 0. `BOOT_CYCLES` is 157 frames. Found by sweeping
  `--input-offset` and counting distinct rooms reached in 30k frames: the right value reaches
  37 rooms, every neighbour reaches 5.
- Speed switch stall: from the `stop` opcode fetch to the next instruction fetch takes exactly
  32771 M-cycles in both SameBoy and GBHawk (2 for the two-byte opcode, 32769 stalled), not the
  2050 that Pan Docs lists. DIV is reset, the PPU skips 12 dots, and the timer and APU keep running.
  GBHawk sets DIV to 0xFFFF instead of 0 at the switch (one base cycle of divider phase, not
  reproduced). When measuring across the switch, count M-cycles, not base cycles: the two
  pre-switch M-cycles are 4 base cycles each.
- GDMA and HDMA cost 8 M-cycles per 16 bytes in normal speed and 16 in double speed (Pan Docs),
  plus 1 M-cycle per transfer for GDMA. The game runs in double speed from init onward. Getting
  this wrong shifted the file-select sequence enough to break frame-precise menu inputs.
- Interrupt flags rise at dot 0 of the line for both VBlank (line 144) and LYC, two dots before
  the STAT register shows the new mode or coincidence bit. HALT: if an interrupt is already pending
  after the halt's own M-cycle and IME is set, the CPU does not halt and the interrupt is dispatched
  with the halt's own address as return address, so the halt runs again after the handler. While
  halted, leaving HALT for an interrupt costs one extra M-cycle before the 5-cycle dispatch. SameBoy
  and GBHawk agree on all of this (`v*`, `w*` and `x_race*` ROMs in `tests/timing/`, 17 phases). The game's main loop is `halt; nop; bit 7,(hl); jr nz`,
  and whether the VBlank lands before or after the `bit` decides if a frame is skipped, so both
  rules have to be exact.
- LYC STAT interrupt: the interrupt line rises at dot 0 of the matching line, two dots before the
  STAT coincidence bit becomes readable. With one-M-cycle instructions this makes the handler start
  one M-cycle earlier than a "raise when the bit changes" model (SameBoy and GBHawk agree; `l*`
  ROMs in `tests/timing/`). The game's mid-frame LCD register update polls STAT for HBlank with a
  7-M-cycle loop right after this interrupt, so a 1-cycle shift became a 7-cycle drift.
- Object penalties (mode 3): none on the first line after LCD enable (no OAM scan happened), and
  none for objects at X >= 168; otherwise 6 dots per object plus 5 - ((X + SCX) % 8) once per
  8-pixel block, 11 for X = 0. Verified with the `p*` ROMs in `tests/timing/` against SameBoy and
  GBHawk (both agree on all 30 variants).
- Cartridge SRAM starts as 0xFF (GBHawk, SameBoy, and real flash carts). The game validates the
  three save slots on every reset by checksumming 0x550 bytes and comparing to a stored checksum
  and an 8-byte magic string; with zeroed SRAM the checksum matched and the code took a branch that
  costs 16 M-cycles more per slot. Six slot scans per boot gave the "drift per loading window".
- GBHawk exposes only the 8 KB of SRAM the header declares; ours masks with the header size too.
- Reference dump hashes: GBHawk's `dump.lua` hashes WRAM 0x300..0x1000 (bank 0) with 32-bit FNV-1a
  at frames where BizHawk's frame counter is a multiple of 60; that is our movie frame 59 mod 60,
  which `--dump` now uses.
- SameBoy lockstep (`tools/lockstep.c`, built against a SameBoy checkout) is the fastest way to find
  the next divergence: it runs both cores on the same inputs and diffs WRAM per frame, ignoring the
  sound engine (c000-c0ff), thread stacks (c100-c2df), and the sound bank (bank 2, df80-dfff).
  Alignment between the two cores is found with `SB_BOOT_FRAMES = -1`, which hashes gameplay WRAM.
  LCD-off windows produce transient one-frame sampling differences; only persistent diffs matter.
- The sound driver runs from the timer interrupt and reads `KEY1` to pick TMA (0x77 in double
  speed). Sound state is timing-phase dependent and excluded from comparisons.

## Routine replacement (milestone 2)

- A ported routine is a cycle-annotated transliteration: every instruction is `I(addr, cycles)`
  followed by its effect, on the CPU registers and memory mirrors directly. `gb_burn` runs the
  instruction-boundary work (HBlank DMA block, interrupt dispatch, running the handler in the
  interpreter until its `reti`) before ticking the cycles, and memory accesses happen after the
  ticks, which is when the hardware performs them. This keeps handlers landing on the same cycle
  with the same partial state, and VRAM/OAM access blocking identical.
- The instruction address matters: an interrupt inside a routine pushes the mid-routine PC, and
  those bytes stay below SP afterwards. Without exact addresses the WRAM hash differs even though
  the game never reads them.
- Routines can run longer than a frame (`clearMemoryBc` over 4 KB is 45k M-cycles), so frame
  sampling and input lookup cannot live in the frame loop: `gb_tick` samples every grid boundary
  into a ring and `gb_run_frame` returns one sample per call; the joypad is fetched by frame
  number through `gb->input_at` at the latch instant.
- Verify mode masks interrupts around the shadow runs, so it perturbs timing; use it for routine
  diffs, and the replay in replace mode for the state and frame hashes.
- Hooks are only active for the Ages ROM (header title check): the Blargg and acid2 ROMs have code
  at the same addresses.
- Control flow across the C/interpreter boundary: a routine that jumps away (`jp hl`, jump table
  entry outside itself, fallthrough into an unported label) or returns past its own frame
  (`retIfTextIsActive` pops its return address and returns to the grand-caller) is a hand-off:
  `HANDOFF(pc)` unwinds every enclosing C frame with `longjmp` to the hook dispatcher and the
  interpreter continues at `pc`. The abandoned C callers are never resumed in C; the interpreter
  reaches their remaining instructions through the return address on the stack. Waiting in C for
  the callee's frame to return is wrong because game threads switch stacks.
- Nested calls: in replace mode `CALL` runs the C callee directly (or the interpreter when the
  callee is filtered out by `HOOK_ONLY`/`HOOK_SKIP`, which is what makes bisection work at any
  depth). In verify mode `CALL` goes through the interpreter so the callee is verified on its own;
  nested verification is sampled (every call for the outermost routine, the first 32 and then
  every 64th for nested ones) because each verification copies the emulator state twice.
- A C routine can run for more than 16 frames without returning to the frame loop (cutscene
  loops in `cutscene06`), which overflowed the 16-entry sample ring and silently dropped frames
  from the frame-hash stream. The consumer is therefore a callback (`gb->frame_cb`) that
  `gb_run_frame` calls for each sample it pops and that `gb_tick` calls itself when the ring is
  full; nothing in the frame loop may assume one sample per loop iteration. `gb_reset` samples
  nothing while the boot ROM runs.
- The disassembly reuses label names across banks (`parseGivenObjectData` is a bank 0 trampoline
  and a bank 18 loader). Routine identity is (bank, address), never the name; the generator
  suffixes the C name with `_bXX` for the later instances.
- Reachability in the generator follows jumps and jump tables only. Following a `call cc` target
  decodes the callee's bytes in the caller's bank (bank 0 code calling a switchable-bank address)
  and lands on data or illegal opcodes, which made `updateAllObjects` drop out of the ported set.
- `halt`, `ei` and `reti` in C: `HALT(addr)` burns the fetch cycle, then `hook_halt` reproduces the
  interpreter step by step (HBlank DMA chunk, tick, check pending; wake costs one tick; an
  interrupt that is pending at the halt with IME set re-runs the halt's fetch after the handler,
  as the interpreter's `pc--` does; the halt bug hands off to the interpreter with
  `halt_bug` set). `ei` sets `ime_delay`, which `gb_burn` resolves at the next instruction
  boundary after the interrupt check, exactly where `gb_step` does. `reti` is `ret` plus IME.
  `ime_writes` counts `di`/`ei`/`reti` so verify mode, which masks IME around its shadow runs,
  can tell whether to restore the caller's IME afterwards.
  Routines containing `halt` are never verified (`HOOK_NOVERIFY`, set by the generator): with IME
  masked, the halt takes the halt-bug path, and restoring IME afterwards with `halt_bug` set makes
  the next interrupt dispatch corrupt the handler's first instruction, a state real hardware
  cannot reach. Replace mode runs them normally.
- Interrupt handlers run in C nested inside the routine they interrupt. The dispatcher must push
  the interrupted routine's own instruction address, captured before the handler runs: the handler's
  instructions overwrite `hook_pc`, and a second pending interrupt serviced in the same boundary
  would otherwise push a handler address (state hash mismatch at frame 1260).
- The generator never follows control flow across the 0x4000 boundary: a bank 0 routine's
  `jp $45c0` runs in whatever bank was just selected, and reading the bytes as bank 0 decodes
  garbage (`vblankRunBank4Function` inlined bank 0 bytes at 0x05c0 and broke the frame hash at
  frame 2301). Targets below 0x4000 always resolve as bank 0 labels, so bank N code calls bank 0
  routines directly in C.
- A thread switch inside an interpreter call-out is detected by counting stack pointer loads
  (`sp_loads`, bumped by the interpreter and by generated C), not by comparing SP against the
  caller's frame: the palette thread's stack sits below the main thread's, so the "SP above my
  frame" check missed it and `asm_call` interpreted the other thread forever. In verify mode the
  nested verification of the switching routine swallows the hand-off itself, which is why replace
  mode never showed it.
- Entry points are not just routine labels. The interpreter can only re-enter C at a hooked
  address, so the generator also emits an entry function for every local label under a ported
  routine (`foo@bar` becomes `foo__bar`, decoded from that address with the same body walker),
  for the instruction after every `ld sp`, for the return address of every call into a routine
  that switches threads (transitively, through jumps and fallthrough), and for jump targets that
  land inside another routine's body without a label (`func_7450` jumps into `func_745c`).
  Without these, the tail of a routine after a thread switch or a call into a local label ran
  interpreted for the rest of the routine.
- A hand-off no longer unwinds every C frame. `HANDOFF(x)` runs the interpreter from `x` inside
  the current C frame until that frame's own return address is popped (`hook_continue`), so a
  callee that jumps through a table or into unported code returns control to its C caller as
  before. It escalates with `longjmp` (`HANDOFF_UP`) on a thread switch, on a return past the
  frame, and when the interpreter reaches a hooked entry at the frame's own stack level while
  more than 24 C frames are nested: a routine that hands off and then jumps back to its own
  entry would otherwise nest one C frame per loop iteration.
- Code that runs from RAM: `src/hooks/extra.sym` adds labels the symbol file lacks. The interrupt
  vectors at $40/$48/$50/$58, the OAM DMA routine copied to HRAM ($ff80, decoded from its ROM
  source with `= 03:4091`), the music read function copied to $c000, and `wRamFunction` at $c4b7
  (a `jp` whose target the game rewrites; hand-written in `src/game/ram_code.c`, marked `extern`).
  Hooks above $8000 ignore the ROM bank.
- `PCHIST=file` records every interpreted instruction by (bank, pc) and `tools/pc_hist.py`
  summarises it by routine; that profile, not the file list, decided the later batches. Its
  15-bit key aliases RAM addresses onto ROM ones ($ff86 shows as bank N $7f86).
- `tas/ages.frames` (framebuffer hash per frame) was recorded by the milestone 1 runner, which
  hashed at the first instruction boundary after the grid target; `tas/ages.ref` was re-recorded
  when sampling moved to the exact grid cycle inside `gb_tick` (GBHawk parity) but the frame
  hashes were not. On the seven frames of the first 23k where the LCD is out of phase with the
  grid, the two instants see different partial scanlines. Re-recorded on 2026-09-09 with
  `--no-hooks`; the milestone 1 build reproduces the old file bit for bit and fails the new
  `ages.ref` at frame 240, which is how the stale file was told apart from an emulation change.
- Verify mode verifies routine-level entries only (`HOOK_LOCAL` entries run in the interpreter
  there) and samples nested verifications sparsely; each verification copies the 190 KB machine
  state twice, and 12.9k entries times the old "first 32 calls each" made it crawl. Interrupt
  dispatch is suppressed during both the C run and the reference run (`hook_suppress_interrupts`):
  IME is masked at the start, but a handler that runs `ei` re-enables it, and the reference
  interpreter would then service a nested VBlank that the C run never saw (`timerVector` showed
  261 WRAM mismatches from exactly this).
- Jump tables after `rst $00` end where the next word stops looking like a code address, where
  a label starts, or where an entry's own target begins. The old rule ("stop when the table
  reaches its lowest target") ended a table at its first bank 0 target, decoded the rest of the
  table as code, hit an illegal opcode and silently dropped the routine (`companionScript_subid04`
  and seven others were never generated although listed; the profile only showed their `rst`
  running interpreted).
- Bank 0 code that selects a bank and then jumps or calls into it (`ld a,:bank` /
  `ld ($2222),a` / `jp` or `call`, the `callfrombank0` and `jpab` idioms) resolves the target
  through that preceding pair (`infer_banks`), so the call is direct C and the return address gets
  an entry when the callee switches threads. Thread-switch detection follows calls as well as
  jumps and fallthrough: a thread switch deep in `interactionLoadGraphics` resumes every caller
  up the stack in the interpreter at its return address, and each of those needs an entry.
- Any stack pointer load (`ld sp,hl`, `ld sp,nn`) is a thread switch and therefore a hand-off, in
  the interpreter and in generated C alike: after it, no C frame's "wait for my callee to return"
  can hold. Without this rule an interpreter call-out from C ran the rest of the game inside the
  call-out and the frame loop never got control back.

## Rewriting (milestone 3)

- Verify mode proves state and total cycles, not where inside a routine a timing side effect
  lands. `setCpuToDoubleSpeed` hand-written with the right total but the DIV reset one M-cycle
  early passed verify and desynced the replay 400 frames later (timer interrupt phase). Anything
  that touches DIV, KEY1, LCDC, STAT/LYC, or the HDMA registers must sit at its exact M-cycle;
  everything else may be burned at routine granularity (plan, contract item 3).
- wla-dx assembles `stop` as the single byte `$10`; the disassembly's explicit `nop` after it is
  the operand byte. The instruction costs 32,771 M-cycles in the interpreter (opcode, operand,
  32,769 stall) and the code after it starts two bytes later, not three.
- Hand-written entries (`extern` in `src/hooks/extra.sym`) live in `src/game/kernel.c` and
  `src/game/ram_code.c`; the CMake glob picks up every `src/game/*.c`.
- The game's five stacks (`wMainStack` at c0b0 through `wThread3StackTop` at c2c0) hold dead
  bytes below each SP, and the sound driver's state (c000-c0af, WRAM bank 2 df80-dfff, HRAM
  ffd8-fff2) depends on the timer interrupt's phase, which a coarse cycle burn shifts.
  `gb_state_hash` and the verify compare skip all of those; `tas/ages.ref` was re-recorded from
  `--no-hooks` with that definition. Frame hashes (`tas/ages.frames`) are checked on the pure
  interpreter only. The GBHawk dump comparison (`--dump`) still hashes all of WRAM.
- Interrupt dispatch must stay on instruction boundaries. Dispatching one M-cycle after IF
  rises (per-tick checks) desyncs even the transliteration at frame 6720; burning a routine's
  cycles in one lump delays dispatch by the routine's length and desynced the rewrite at frame
  118380 with no single hook responsible. Rewrites burn through `burn_rom` (`CYC(from, to)`),
  which walks the ROM's instruction lengths and cycle counts (`src/game/cyc.c`) so each
  instruction gets its own `gb_burn`. Memory accesses follow the burn of their instruction.
- Bisecting with `HOOK_SKIP` needs the generated jump and fallthrough sites into a rewritten
  routine to honour the skip; they now test `hook_enabled_at` first.
- Shared state that an interrupt handler also touches must be read and written at the exact
  cycles of the assembly's loads and stores, not just written before a batch burn. The VBlank
  handler advances `hRng1`/`hRng2` too; a `getRandomNumber` rewrite that stored its result at
  entry and then burned 40 cycles desynced the state hash at frame 27420 (the handler's update
  landed between the assembly's read and write, and after the rewrite's write). Each access now
  follows the burn of its own instruction.
- A `CYC` range is straight-line code: it must end at the jump that leaves it, not at the next
  label, because `burn_rom` walks bytes (`objectGetRelatedObject1Var` burned the `ld l,$18`
  that its `jr` skips). `burn_rom` aborts when a range runs past an unconditional jump.
- A rewrite that hands control to a transliterated tail (a `jr`/`jp` into generated code, or a
  fallthrough) must leave the emulated stack exactly as the assembly would: the tail's `pop af`
  and `ret` read it. `interactionAnimate` pushed the saved ROM bank with `push af` before
  jumping to `_interactionNextAnimationFrame`; the rewrite burned the push but did not write it,
  and verify caught register A holding stack garbage. Conversely a rewritten routine reached by
  such a tail never pops for it, so `interactionAnimate` returns whether the tail ran and its
  callers push a return address only in that case (`call_interaction_animate`).
- Every `push` in a rewrite is a real `push_effect`, never a C local, because any callee reached
  through `jp hl` dispatch (`updateInteraction`, `updateEnemy`, `runBank2Function`,
  `interBankCall`) can switch threads: the kernel stores the thread's SP in
  `wThreadStateBuffer` (c2e0-c2ff, hashed and verified), so a save kept in C instead of on the
  stack shows up as a WRAM mismatch at c2ea two frames later (batch 23, `updateAllObjects`).
  The `SWITCHES THREADS` flag in `--report` only follows static calls and jumps; it cannot see
  through `interBankCall` or `jp hl`. `bank_push`/`bank_pop` write the stack for the same
  reason. A routine whose callee does switch threads is still fine to rewrite: the handoff
  unwinds the C frame with `longjmp` and the interpreter finishes the tail from the ROM using
  the stack the rewrite left behind (`interactionInitGraphics`, `refreshObjectGfx`).
- A rewrite that ends in `jp hl` or jumps into the thread kernel (`updateEnemy`,
  `updateInteraction`, `jpHl`, `jpBc`, `showText` into `threadRestart`, `stopTextThread` into
  `threadStop`) burns its last instruction and calls `hook_handoff(gb, target)`; the interpreter
  continues at the target. Verify accepts a C side that does not return by running the
  interpreter for the same number of cycles and comparing state there.
- `oracles-run` prints `done: N frames` with `GRID_FRAME` at exit, which can run past `--frames`
  (290128 with `--no-hooks` and 290368 with hooks for `--frames 289943`; the batch 19 build
  printed 289943). The loop stops on the first sample at or past the target and the last
  `gb_step` may span many frames. The gate is the absence of a `state hash mismatch` line plus
  `ctest`, not the `done` line. Why the count differs between builds is not yet explained.
- `updateDirtyPalettes` writes BCPS/BCPD/OCPS/OCPD from the VBlank handler: each
  `ld ($ff00+c),a` is burned then written like any other store; the PPU accepts them because
  the handler runs in VBlank. A missing one-cycle burn (`inc c`) there showed up as a TIMA
  difference in verify (io off=5) two frames later, not as a palette difference.
- The whole-movie verify (`--verify-hooks-continue` for 289,943 frames, about 15 minutes,
  49.5M hook calls on the batch 23 build) is the strongest check available and is worth
  running on every build that changes the hook contract; the 30k-frame verify misses routines
  that only run late in the game.
- Multi-byte stores that a frame sample can land inside must keep the assembly's sub-instruction
  write cycles. `ld (nn),sp` writes the low byte after 4 M-cycles and the high byte after 5, so a
  rewrite that burns all 5 and then writes both leaves the old low byte visible for one M-cycle
  longer. `resumeThreadInAFrames` stores SP into `hFF92` this way and the state hash sampled at
  frame 4140 caught it as a single differing HRAM byte, with identical WRAM, IO and cycle counts.
  `burn_store_sp` (`src/game/cyc.c`) does the split; verify never saw it, because verify compares
  only at routine boundaries.
- A rewrite whose tail is a static jump calls the target's C function directly
  (`_nextThread(gb)`, `interBankCall_hook(gb)`, `vblankInterrupt(gb)`), the way the transliteration
  does. `hook_handoff` is for dynamic targets (`jp hl`, `interBankCall`'s dispatch) only: handing
  off to a known address puts an extra interpreter instruction boundary in the middle of the
  routine, where an interrupt can now be dispatched.
- The interrupt vectors are ordinary hooks. `ei` in the timer vector sets `ime_delay`, not `ime`:
  `gb_burn` dispatches pending interrupts before each burn and applies `ime_delay` after that
  check, so setting `ime` directly would let an interrupt nest one instruction early.
- `jp hl` inside a rewrite is `hook_continue(gb, target, sp0)` and then `return`, with `sp0` the
  stack pointer at the routine's entry: the interpreter runs the dispatched code and everything it
  chains into until it returns past `sp0`, and the rewrite's C frame ends there. Treating the
  dispatch as a call that comes back hung the replay - `vblankFunctionRet` looped forever, because
  the queue walk is a jump back into `runVBlankFunctions`, not a call.
- `CALL_C(addr, fn, target, ret)` is the rewrite's version of the generated `CALL`: it burns the
  call, pushes the return address, and then runs the interpreter (`asm_call`) in verify mode or
  when the target has no hook, and the C function otherwise, handing off if the callee did not
  return to `ret`. `CALL_ROM` always takes the interpreter, which is fine for a callee that
  returns normally but adds instruction boundaries around one that switches threads.
- A local label of a rewritten routine (`name__label`) cannot be listed in `rewritten.txt`:
  the generator drops a rewritten routine's locals along with its body, so the label loses its
  hook and the interpreter runs it. That is correct, just slower; write the block as a static
  helper in the C and call it directly.
- A routine reached only through a static C call from another rewrite (`lcdInterrupt_hook` from
  `lcdVector_hook`, `vblankInterrupt_hook` from `vblankVector_hook`, likewise `timerInterrupt_hook`
  and `serialInterrupt_hook`) never shows its own call count in `hooks_report` and lint's
  `HOOKLOG`/"never called" warning is a false positive for it: the counter only increments in
  `call_hook`, which runs when the interpreter's PC-based lookup finds a hook, and a direct C call
  skips that lookup entirely. It is still checked - `lcdVector_hook` itself is dispatched normally
  from interrupt entry, so a HOOK_MODE_VERIFY comparison of `lcdVector_hook` runs the interpreter
  for the identical PC range and catches any discrepancy anywhere in the chain it calls into,
  registers/cycles/memory included. Reading "0 calls" on the callee is not a red flag by itself;
  check the entry vector's call count and mismatch total instead.
- `jr cc,target` taken-vs-not-taken is easy to invert by hand: `CYC`/`CYCT` on the branch
  instruction's own byte range must match whether the jump is actually taken, and the C `if`
  condition must select the matching side, but nothing checks that a `CYCT` sits under the
  branch that is genuinely taken - swap the flag or the branch and the cycle count still often
  balances (both sides of an `if` may burn instruction bytes past the branch to reach a shared
  continuation), so the bug is invisible in code review. `lcdInterrupt_hook`'s five conditional
  jumps had four of these inverted on the first draft; the 30k verify's cycle-and-register
  compare on `lcdVector_hook` caught all of them (see the entry above on why the callee's own
  count reads zero).
- A loop bound copied by eye is the easiest wrong-operand bug to write and the hardest to spot:
  `cp <(wThreadStateBuffer+NUM_THREADS*8)` compares against the low byte of the buffer's *end*
  (0x00), not the buffer's own low byte (0xe0, which is what a rewrite skimming the surrounding
  code reaches for). Both values are named-looking constants already in scope, the mistake
  compiles cleanly, and cycle counts stay identical either way (the branch still burns the same
  bytes) - only replay against a reference build catches it, and only from the very first frame
  onward, because the loop bound is wrong every time it runs.
- `ld sp` is always a real hand-off (`hook_handoff`), never a direct C call to the continuation,
  even when the continuation is "just the next few lines of this same routine." A direct call
  leaves `sp0_` - captured at THIS function's entry - as the reference point for any macro
  downstream that assumes call/return semantics (`HALT`'s halt-bug path, `CALL_C`'s failure
  path), and those macros silently hand off to a stale, meaningless return address instead of
  the fresh one the new stack context implies. The five addresses this touched
  (`resetGame__afterSp016c`, `_nextThread__afterSp091a`, `startGame__afterSp0925`,
  `_countdownToRunThread__afterSp0998`, `_initializeThread__afterSp09aa`) exist only as the
  transliterator's synthetic local labels and disappear entirely once their parent routine is
  rewritten (nothing generates them, nothing hooks them) - giving them real symbols in
  `src/hooks/extra.sym` plus entries in `ported.txt` and `rewritten.txt` turns each into an
  independent hook `hook_handoff` can actually find.
- Nested verify's fallback for a hook that never returns to its caller - run the interpreter for
  the same cycle count the C consumed, then compare final state - has no good answer for a
  routine built around a `halt` loop that exits only when an interrupt handler clears a flag,
  because interrupt dispatch is suppressed on both sides of a verification (deliberately, so the
  comparison is deterministic). Both the C and the interpreter reference sit mid-loop at
  whatever the fixed cycle budget happened to reach, with no guarantee they reached it by the
  same path, and a mismatch there was cycle-exact but register-wrong. The generator already
  gives every transliterated routine containing a `halt` the `HOOK_NOVERIFY` flag; it never
  reaches a hand-written routine that only reaches that `halt` through a chain of direct C
  calls. `src/hooks/rewritten_noverify.txt` (read by `tools/transliterate.py`, emits the same
  `H` flag) marks those by hand - here, every routine between a `ld sp` hand-off and the vblank
  wait loop (`_mainLoop`, `_mainLoop_nextThread`, `_nextThread__afterSp091a`,
  `startGame__afterSp0925`). Whole-movie replay against `tas/ages.ref` was clean the entire
  time; this only ever showed up in the 30k-frame routine diff.
- `ld (nn),a` and `ld a,(nn)` are 3 bytes and 4 cycles; `jp nn` is 3 bytes and 4 cycles too. Both
  have a cycle count one higher than their byte count, which is exactly the gap that makes
  writing `CYC(a, a + <cycle count>)` instead of `CYC(a, a + <byte count>)` compile clean, burn a
  plausible-looking number of cycles, and only go wrong when `burn_rom` walks into the next real
  instruction. A `jp nn` mis-burned this way trips `burn_rom`'s unconditional-jump guard
  immediately (batch 30's `textThreadStart_hook`). A `ld (nn),a` mis-burned the same way does not
  trip anything - the extra byte just happens to decode as another valid instruction, so the
  bug is silent until a state-hash checkpoint some tens of thousands of frames later catches the
  cumulative timing shift (batch 30's `thread_1b10_hook`, which runs exactly once in the whole
  movie - the game-over screen - so the single mis-burn only ever fires that one time). When a
  `CYC` range's cycle count and byte count differ, use the byte count; `burn_rom` derives the
  actual cycles from the opcode itself.
- `CALL_ROM` and `CALL_C` are not interchangeable even though both call into a hookable address:
  `CALL_ROM` always takes the interpreter, `CALL_C` mirrors the generated `CALL(...)` macro
  (interpreter only in verify mode or when the target has no hook, the C function otherwise).
  Reach for whichever one the transliteration itself uses at that call site - printed in the
  `--report` output as `(hooked)` for a smart `CALL`, or `/* unported */` for a `CALL_ASM` that's
  always interpreted regardless of hook state. Getting this backwards for six calls in batch 30
  didn't fail any gate on its own (the interpreter is always correct); it only mattered once
  another bug needed to be bisected, since it changed which routine's C code a HOOK_SKIP test
  was actually exercising.
- A shared RST vector can contain a conditional return whose taken state is determined by flags
  set before entering the vector. The animation queue uses `rst $10` for add-A-to-HL; on its
  no-carry path, the vector's `ret nc` at bank 0 `$0012` is taken and costs 5 cycles, not the
  2-cycle not-taken form. Treating the vector as an ordinary straight-line `CYC` range made
  `updateAnimationDataPointer` three cycles short (four animation entries made
  `initializeAnimations` 24 cycles short) while registers and memory still matched. Split the
  vector at the conditional return and use a taken `burn_rom` for that path.
- RAM names must come from `src/game/ram.h`, not inferred from the hexadecimal address in the
  disassembly. The parser batch's `$cdd1`, `$cfc0`, and `$cd02` spellings are respectively
  `wNumEnemies`, `wTmpcfc0`, and `wScreenTransitionDirection`; an invented `w<address>` name has
  no companion `_BANK` macro and fails `W8(...)` at compile time. Search the generated RAM names
  before writing an access, even when the address looks self-describing.
- When a compact object-data handler differs only in a pointer register, dump the ROM bytes
  around the instruction rather than inferring it from the data-flow pattern. `objectDataOp9`
  at bank 12 `$57b6` uses `inc l`, not an auto-increment memory load; treating it as the latter
  left `HL` one byte high and the hook verifier first caught the register mismatch at frame 1538.
- Conditional and unconditional `jr` instructions are two bytes even when their report column
  says three cycles. In batch 35, burning `specialObjectSetAnimationWithLinkData`'s `$441c` jump
  through `$441f` treated its cycle count as an address range; `burn_rom` stopped at the jump
  boundary before the verifier could run. Its range is `$441c`&ndash;`$441e`; always derive a
  branch's `CYC` end from opcode bytes, not the printed cycle count.
- `file(GLOB ...)` source discovery is evaluated at CMake configure time in this project. Adding
  `src/game/updateItems.c` after the existing build was configured produced hook-table linker
  errors for all three new post-item shims even though regeneration and lint succeeded. Run
  `cmake -S . -B build -G Ninja` before building whenever a batch creates a new game-source file.
- Changing an existing hook's call from `CALL_ROM` to `CALL_C` requires an entry `sp0_` capture,
  even if the hook previously had no dynamic control flow. `CALL_C` uses `sp0_` when a native
  callee does not return normally and must continue through the interpreter; batch 37's
  `linkApplyDamage_b00_hook` compiled only until its bank-5 bridge became native, then the
  missing capture was diagnosed by the compiler. Add `uint16_t sp0_ = gb->sp; (void)sp0_;` before
  introducing any `CALL_C` or `CALL_C_CC` into an older hook.
