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
- A local implementation of `rst $10` (add A to HL) must burn the vector's conditional return at
  bank 0 `$0012`, not just reproduce its register result. Batch 39's
  `createSeaEffectsPartIfApplicable` calls the vector twice on its no-carry path; omitting the
  taken `ret nc` burn left the hook eight cycles short with otherwise identical state. The 30k
  routine verifier caught it at frame 1286.
- The readable C filename follows the disassembly source's leaf basename, including nested source
  directories. Batch 40 initially placed routines from `ages/cutscenes/miscCutscenes.s` in
  `cutscenes.c`; code review caught the collision in meaning with the separate
  `ages/cutscenes.s`. The correct file is `miscCutscenes.c`.
- `lint_game.py` recognizes register-safe shim scope only on a line beginning
  `void name_hook(GB *gb)`. A shared callable body declared `static void ..._hook` therefore left
  its explicit `gb->sp` access outside recognized hook scope in batch 41. Shared bodies that need
  an entry-stack capture must use the normal non-static hook-style declaration, as the rectangle
  copy body does; a file-local helper is only suitable when it has no explicit emulated-register
  access that the lint checks.
- The carry path through the add-A-to-HL `rst $10` vector has three distinct burns after `ld l,a`:
  the not-taken `ret nc` at `$0012`, `inc h` at `$0013`, and the final `ret` at `$0014`. Batch 42's
  first rewrite attached `inc h` and `ret` to the wrong byte range and omitted the final return,
  leaving the path four cycles short; line-by-line review against the ROM report found it before
  the verifier. Model vector helpers as the actual instruction stream, including both returns,
  rather than as a register-only arithmetic helper.
- Extending an existing source-named readable-C file must preserve every earlier hook in that
  file. In batch 42, replacing `loadTilesetData.c` and `roomSpecificTileChanges.c` with the new
  address clusters silently deleted five batch-39 definitions; regeneration succeeded, but the
  integrated link failed with undefined hook symbols. Diff the whole source file against `HEAD`
  after parallel edits, and merge new routines in address order instead of replacing the file.
- The cycle-count/byte-count trap also applies to an unconditional `jr`: its report shows three
  cycles, but its instruction is still two bytes. Batch 43 initially burned `dmaTextboxMap`'s
  `$51b7` jump through `$51ba`, consuming the byte at `$51b9` that begins
  `updateCharacterDisplayTimer`; cross-review against the report corrected the range to
  `$51b7`&ndash;`$51b9` before integration. Derive every burn endpoint from the opcode length even
  when a nearby routine boundary makes the cycle-count endpoint look plausible.
- A C label must represent the exact ROM target, not merely a nearby block with similar intent.
  Batch 44 initially sent `displayNextTextCharacter`'s backward `$522d` jump to the local
  `$51e7` display block instead of the real `$51db` entry, skipping the character-width setup on
  every loop. Cross-review of target addresses found it before replay. Give distinct labels to
  distinct ROM addresses even when both paths rejoin only a few instructions later.
- Every conditional return needs burns on both paths. Batch 44 modeled the taken `ret z` at
  `$52a4` but went straight to the `$52a5` call on fallthrough, silently dropping the two-cycle
  not-taken return. After writing a one-line early return, add the matching ordinary `CYC` before
  the next instruction just as for a conditional `jr`.
- A static `call` to a local body still performs a real emulated-stack operation even when both
  sides become readable C. Batch 45's textbox direction handler calls its `$5624` local body from
  `$561f`; burn the three-byte call, `push_effect` the `$5622` return address, invoke the helper,
  and let its original `ret` pop that address before continuing. Calling the helper like an
  ordinary C function would leave the emulated stack one return address short whenever the local
  body returns.
- Porting a generated callee can invalidate an older handwritten caller even though both use
  `CALL_C`. Batch 46 rewrote `readByteFromW7ActiveBankAndIncHl`; regeneration removed the plain
  generated C symbol, so batch 44's existing call no longer linked until its target changed to
  `readByteFromW7ActiveBankAndIncHl_hook`. Batch 47 repeated this with four direct/call sites for
  `replaceVineTiles`, showing that both `CALL_C` and static C tails need the audit. After
  registering a new rewrite, rebuild the whole target and search compile failures for older calls
  that still name the generated symbol.
- Conditional calls need conditional-call timing even when the C branch already expresses the
  condition. Batch 48 initially used ordinary `CALL_C` for `cutscene19_state8`'s `call z` at
  `$4c87`; cross-review found that the taken edge then burned the ordinary four-cycle call instead
  of the conditional call's six cycles. Use `CALL_C_CC` on the taken branch and the ordinary
  three-byte `CYC` on the fallthrough branch.
- A disassembly-local label containing `@` needs a C-safe real alias when it becomes its own hook
  entry. Batch 49 initially listed `intro_titlescreen_state1@pressedStart` and `@gotoState` in
  `rewritten.txt` while the C shims used `__`; regeneration removed the generated entries, then
  lint could not match either spelling. Add the `__` spelling at the same address in `extra.sym`,
  and use that alias consistently in `ported.txt`, `rewritten.txt`, and the `_hook` function.
- A clean routine verifier does not prove a newly readable nested callee was exercised as C.
  During verification, `CALL_C` deliberately runs its callee through the interpreter, so batch
  52's inverted `jr nc` condition inside `flashScreen_body` produced 0 failures in the 30k run
  while the non-verifying reference replay diverged at frame 21,660. The whole-movie gate and a
  hook-list bisection isolated the nested helper; comparing its flag condition to the assembly
  found `if (F & FC)` where the taken no-carry edge required `if (!(F & FC))`. Keep the reference
  replay mandatory even after a clean hook-verification run, especially for helpers reached only
  through direct `CALL_C` chains.
- A plausible temporary-variable name is not evidence that it matches a raw RAM address. Batch
  54 translated `ld hl,$cbb7` in the Maku-tree palette helper as `wTmpcbb4`, which silently wrote
  `$cbb4`; build, lint, review, and the routine verifier all passed, but the reference replay
  diverged at frame 17,220. A hook-list bisection isolated the helper and checking the generated
  `ram.h` values exposed the three-byte address error. When assembly uses a raw address, verify
  the chosen named alias's numeric definition before accepting the readable spelling.
- A valid disassembly `.define` is not necessarily emitted into generated `ram.h`; only generated
  RAM symbols are available to readable C. Batch 56 used `wCutsceneIndex`, which exists in the
  assembly include as `wThreadStateBuffer + $f` but failed the integrated build because no C macro
  was generated. Use the verified raw address (`$c2ef` here) or an available generated base plus
  offset. Likewise, `hram_rd` and `hram_wr` take the low-byte offset, not an absolute `$ffxx`
  constant; pass `$8c`, not `hFF8C`, so the address is explicit and does not rely on truncation.
- A report marking the target of a static `jp` as `SWITCHES THREADS` does not turn the jump itself
  into a scheduler handoff. Batch 57 review flagged `cutscene11` and `cutscene12` because both jump
  to thread-capable `func_5d41`, but the original instructions contain no `ld sp,*`; the correct
  rewrite burns each three-byte jump and calls the known target's C function directly. Reserve
  `hook_handoff` for the actual emulated stack-pointer switch, wherever that occurs downstream.
- Candidate scans must check both `rewritten.txt` and the source-named readable-C file before
  implementation. A batch-57 parallel lane selected three bank-3 labels that were already
  registered and defined near the top of `bank3Cutscenes.c`, then appended duplicate definitions
  at the bottom; the integrated compiler caught the redefinitions. Treat the hook registry as the
  authoritative completion list and inspect the full destination file before appending.
- A readable dynamic `rst $00` dispatcher must preserve the vector's real stack traffic before
  handing off. Batch 58's `twinrovaCutsceneCaller` burns the `rst`, pushes the table address,
  executes the bank-0 doubling/add-with-carry sequence, pops that address into `HL`, reads the
  selected word, burns the one-byte `jp hl`, and only then calls `hook_handoff(HL)`. Jumping from
  the source-level index directly to a known table target would skip vector cycles and the real
  push/pop effects that dynamic dispatch and thread changes rely on.
- A named local body that has its own symbol and readiness report is still a routine, even when
  every current caller could use a file-local C helper. Batch 60 initially hid the endgame
  explosion-update and explosion-dispatch bodies behind `static` helpers; integration review
  promoted both addresses to real aliases in `extra.sym`, registered them in `ported.txt` and
  `rewritten.txt`, and exposed normal `_hook` shims. This preserves direct hook dispatch and keeps
  the rewritten count aligned with the reportable routine set.
- Do not combine adjacent instructions into one `burn_rom` range when a register or memory effect
  belongs between them. Batch 61 initially burned the `rst $00` vector's `$0002 add a,l` and
  `$0003 ld l,a` together before applying both effects. The total cycles looked right, but an
  interrupt boundary between those opcodes would observe stale state. Cross-review caught it by
  comparing the helper instruction-by-instruction; split the burns and apply each effect
  immediately after its own opcode. The same review found a three-byte `jp` at `$5641` burned
  through `$5645`, consuming the next routine's first opcode; its correct endpoint is `$5644`.
- An unconditional branch burn ends at the instruction boundary, never at the branch target.
  Batch 62 initially burned the `jr` at `$618b` through its target `$618f`, silently consuming
  the two skipped bytes; instruction-level cross-review corrected the two-byte opcode's endpoint
  to `$618d`. This is the same byte-count rule as every other branch even when the target is the
  next block the readable control flow enters.
- Creating a new readable-C source under `src/game/` needs a CMake reconfigure before the normal
  build, because the source list is collected by a configure-time glob. Batch 63's first build
  compiled every changed existing source but linked with all nine new bank-4 hooks undefined;
  `cmake -S . -B build -G Ninja` added `bank4.c`, after which the build linked cleanly. The same
  batch also exposed `cutscene_clearObjects`, so two older `CALL_C` sites had to change from the
  removed generated symbol to `cutscene_clearObjects_hook`.
- A file-local helper that uses `CALL_C` needs the enclosing hook's saved `sp0_`; it must not read
  `gb->sp` for itself. Batch 64 put fresh `gb->sp` snapshots inside the password-screen and VRAM
  rectangle helpers, and lint rejected the emulated-register access outside `_hook` shims. Pass
  `sp0_` into the helper from every hook entry instead, so nested-call verification retains the
  original routine boundary as well as satisfying the shim-only register rule.
- `W8` token-pastes its argument to find the generated bank macro, so arithmetic cannot appear
  inside it. Batch 65 used `W8(w7TextTableAddr + 1)`, which expanded to the invalid token
  `1_BANK` during the integrated build. For an adjacent byte in the same named bank, use the
  pointer form (`WP(w7TextTableAddr)[1]`); this preserves the symbol's generated bank selection
  while making the offset ordinary C arithmetic.
- A nearby RAM symbol is not interchangeable with the raw address in the instruction stream.
  Batch 66 translated `ld hl,$d0d4` as `SET_HL(w7TextAddress)`, but that symbol is `$d0d5`; cycles
  and all memory writes looked plausible while the whole-movie replay diverged at frame 32,040.
  The 40k verifier identified `doInventoryTextFirstPass` with `L` one byte high. Check the numeric
  definition and write the real relationship (`w7TextAddress - 1` here) whenever assembly names
  an address adjacent to a field.
- An unconditional `ret` after a conditional loop exit still needs its own one-byte burn. Batch
  66 initially called `ret_effect` immediately after the not-taken `jr` burns at `$5502` and
  `$550d`, omitting the actual returns at `$5504` and `$550f`. Cross-review found both before the
  first build; burn each `ret` separately immediately before `ret_effect`.
- Same-address symbols can have a different canonical hook name than the source routine selected
  for translation. Batch 67 rewrote `cutscene13` at `01:7b6e`, but regeneration canonicalized that
  address as `tilesetLayoutGroup33_hook`; listing only `cutscene13` left lint without a matching
  table entry and the linker without the canonical shim. Expose a thin canonical `_hook` wrapper
  and register the generator's chosen name in `rewritten.txt`. Likewise, named state/substate
  bodies remain real hook boundaries: give disassembly-local addresses C-safe aliases in
  `extra.sym` and register their `_hook` shims instead of hiding them as file-local helpers.
- A readiness report showing `callers: 0` does not make a named local block private. Batch 68's
  seed-tree exit blocks and secret encoder/decoder loop and return blocks initially lived behind
  file-local helpers because their only visible edges were fallthroughs or branches inside an
  already rewritten parent. They still had symbol-table entries and independent reports, so they
  were promoted to C-safe aliases and `_hook` shims. Treat reportability—not the current caller
  count—as the boundary rule; dynamic dispatch and the final no-generated-routines sweep both
  depend on every such address remaining hookable.
- Calling a readable RST-vector helper still requires the RST instruction's real return-address
  push. Batch 69 initially burned `rst $00` at `$4963` and called the shared jump-table helper
  directly, but that helper begins with the vector's `pop hl`; without
  `push_effect(gb, 0x4964)`, it consumed the routine caller's return address and corrupted SP.
  Cross-review caught the imbalance before the gate. Burn the one-byte RST, push its physical
  next-PC, then enter the vector body, even when both sides are ordinary readable C helpers.
- Rewriting a routine removes its generated unsuffixed C function, including calls to it from
  older readable dispatchers. Batch 70 promoted `endgameCutsceneHandler_09`; regeneration then
  failed the build because `endgameCutsceneHandler_body_hook` still called the vanished
  `endgameCutsceneHandler_09`. When a root becomes readable, search existing hand-written C for
  direct unsuffixed calls to that symbol and retarget static jumps to its `_hook` entry.
- A successful readiness report does not mean the routine is still waiting to be rewritten.
  Batch 71's source-order scout selected `cutscene0b` because its report was clean, but that
  routine was already present in `rewritten.txt` and `cutscenes2.c`. Check `rewritten.txt` while
  assembling every batch, especially when moving between source files or resuming an older lane;
  the report deliberately remains available for already-readable routines.
- Instruction effects in shared RST helpers must stay attached to their own opcode, not merely
  preserve the final register values. Batch 72's bank-2 `add_a_to_hl` helper burned the carry-path
  `ret nc` while applying the following `inc h`, then burned `inc h` while returning and omitted
  the real final `ret` at `$0014`. Short verification never took that late map-menu path; the
  whole-movie reference first diverged at frame 286,260. Burn the untaken conditional return with
  no effect, apply `inc h` after `$0013`, and burn the final return at `$0014` separately.
- Unreachable bytes after a static jump are still not part of that jump's burn. Batch 73's
  `dungeonMap_getTileForRoom__hidden` initially burned the unconditional `jr` at `$68a2` through
  `$68a5`; the two-byte instruction ends at `$68a4`, and `$68a4` is the first byte of an
  unreachable `call`. Instruction-by-instruction review caught the extra byte before regeneration.
- For `jr nc`, no carry is the taken path. Batch 74's two palette-mixer multiply loops initially
  skipped the conditional add correctly in C but assigned `CYC` to that no-carry branch and
  `CYCT` to the carry fallthrough. The control result looked right while the cycle timing was
  reversed. The branch that skips the add must use `CYCT`; the carry path that executes it uses
  `CYC`.
- Shared RST helpers need one burn per physical opcode even when adjacent effects are convenient
  to express together. Batch 75's bank-2 RST `$00` helper combined `add l` with `ld l,a`, folded
  `inc h` into the untaken `jr nc` burn, and combined the final `ld l,a` with `jp hl`. The final
  registers and ordinary cycle totals looked plausible, but an interrupt can observe each opcode
  boundary. Instruction-by-instruction cross-review found it; split the burns and attach each
  register effect to the instruction that performs it.
- Rewriting a dispatcher deletes all of its generated local entries, including locals deliberately
  left in the interpreter because they cross a thread-switch boundary. Batch 76's forest-fairy
  root kept unsafe state targets behind `hook_handoff`, but regeneration silently removed those
  targets and the synthetic return PCs after `refreshObjectGfx`. Give every deferred state and
  post-switch continuation a real alias in `extra.sym` and `ported.txt`, but leave it out of
  `rewritten.txt`; regeneration then keeps the mixed-mode entry while the parent becomes readable.
- SM83 `set` and `res` instructions do not update flags, and there are no `alu_set`/`alu_res`
  helpers. Batch 77 initially tried to call such helpers and failed to compile. Implement the
  effect as `mem_wr(gb, addr, mem_rd(gb, addr) | mask)` or `& ~mask` after the instruction's
  burn so A and F both remain unchanged.
- A file-local fragment containing `CALL_C` cannot use the macro unchanged: its nonlocal path
  returns only from the helper, after which the public wrapper would incorrectly execute its own
  `ret`. Batch 78's decompressor fragments exposed this first as a missing lexical `sp0_` build
  error. Pass the public hook's entry SP through the fragment graph and propagate a boolean after
  `hook_continue`, so every caller returns without applying another stack effect.
- A bank with no generated bodies should have no generated C file. Before batch 78 the
  transliterator opened every bank output before discovering that all its entries were rewritten
  hooks or externals, leaving an empty `gen_bank00.c`; CMake's configure-time glob could also keep
  deleted sources in an old build graph. Accumulate bodies first, remove stale two-digit
  `gen_bankXX.c` outputs with none, and use `CONFIGURE_DEPENDS` on the game-source glob so the
  generated bank file can be deleted permanently.
- `lint_game.py` does not reject every raw numeric WRAM operand. Batch 86 passed lint while its
  Zelda-kidnapped states still used fourteen literal `$cfc0`/`$cfc6` accesses, hiding the existing
  generic-cutscene field names. Code-quality review found them before the gate. Check `ram.h` for
  every WRAM address even when lint is green, and use the named field unless the address is
  genuinely an unnamed byte or pointer adjustment.
- Conditional calls need their taken timing macro, and RST wrapper ownership must be checked before
  adding a push. Batch 90 initially used `CALL_C` for `call nz`/`call z`, undercounting each taken
  call, and explicitly pushed `$4a9e` before a shared RST `$18` helper that already owned the same
  push/pop. Review replaced the calls with `CALL_C_CC` and removed the duplicate push. The same
  batch then passed static review but failed verification at frame 523 because the taken `jr nc`
  at `$4817` also executed the skipped `ld c,$00`; the extra instruction added two cycles and
  changed A. For every conditional branch, audit not only `CYC` versus `CYCT` but which physical
  instructions live on each side of the branch.
- A conditional branch can preserve every register and still desynchronize the machine by one
  cycle. Batch 92's `jr z` at `$4117` routed both paths correctly, but assigned `CYC` to the taken
  zero path and `CYCT` to the fallthrough. The routine-level verifier reported equal registers and
  59 C cycles versus 60 interpreter cycles on every frame of the transition. Whichever C path
  follows the jump target must burn `CYCT`; the path that executes the following instruction must
  burn `CYC`, even when the surrounding C condition is written in the inverse form.
- Register-targeted ALU helpers return the new value; calling one without assigning its result
  silently leaves the register unchanged. Batch 93 initially translated `srl h` at `$445a` as a
  bare `alu_srl(gb, H)`. Self-review against the generated body caught that H never received the
  shift result. Write `H = alu_srl(gb, H)` (and likewise for B/C/D/E/H/L/A targets); memory-targeted
  shifts instead read, transform, and write the addressed byte explicitly.
- A symbol can produce the right numeric address while describing the wrong banked object. Batch
  95 initially loaded `w4StatusBarTileMap` (`$d200`) before the ROM's following `ld l,$40`, so HL
  ended at the intended `$d240` interaction-slot base by accident. The code runs with WRAM bank 1
  selected and is scanning interactions, so the correct expression is `SET_HL(wInteractionSlots)`
  followed by the still-real `ld l,$40` effect. Review symbols semantically as well as numerically,
  especially when a later instruction replaces one byte of a 16-bit address.
- A branch to a shared static tail must still execute every physical instruction at that tail.
  Batch 96's two taken branches in `showItemText2` called `showTextOnInventoryMenu_hook` directly,
  skipping the real `jp $1860` at `$5563`; registers matched but verification reported the C path
  four cycles short. Route every predecessor through one shared C label that burns the tail's
  `CYC(0x5563, 0x5566)` before entering the known target.
- Promoting a former local call target to a global hook can remove an interpreter boundary that
  matters to interrupt timing. Batch 96's `runInventoryMenu` used `CALL_C` after `$552c` became a
  global readable entry; routine verification stayed clean because it suppresses interrupts, but
  the full replay diverged at frame 32,040. `HOOK_SKIP` showed that forcing the helper back through
  the interpreter preserved the pending-interrupt opportunity, so the call at `$5520` deliberately
  remains `CALL_ROM(0x5520, 0x552c)` even though the target is readable.
- A helper that returns past its caller with `pop af; ret` must reproduce both effects, not treat
  the first pop as merely discarding a return address. Batch 97's
  `ringMenu_retIfCounterNotFinished` loads the skipped caller return into AF (including flags), then
  returns to the grand-caller; `CALL_C` observes the changed PC/SP and unwinds the abandoned C
  caller. Independent stack review confirmed this path before the gate.
- A low-byte register load should name the intended RAM field even though it changes only half of
  the pointer. Batch 98 initially translated `ld l,<wSecretListMenu.state` and
  `ld l,<wSecretListMenu.scroll` as raw `$b3`/`$b7`; the values were right, but hid both the union
  ownership and the fact that H must stay untouched. Use `L = (uint8_t)symbol` for these operands,
  and use the active subsystem's field name when several union aliases share the same address.
- `CALL_ROM` for a promoted-local interrupt boundary and `CALL_C` for a caller-escaping helper are
  separate rules. Batch 99 initially sent `ringMenu_retIfCounterNotFinished` through the
  interpreter because its nonzero path does `pop af; ret`; that loses `CALL_C_`'s PC/SP check,
  which is what notices the grand-caller return and unwinds the abandoned C caller. Keep this call
  as `CALL_C`; reserve the deliberate `CALL_ROM` exception for the former local boundaries whose
  interrupt visibility was proven by replay.
- A caller-escaping `pop af; ret` changes registers as well as control flow. Batch 100 initially
  called `pop_effect` only to discard the immediate caller's return address before returning to
  the grand-caller. The ROM actually loads that word into AF, so A and every flag must change even
  though the value's control purpose is to abandon a frame. Independent stack review caught this
  before the gate. Translate the instruction as `SET_AF(pop_effect(gb))`, then execute the real
  `ret_effect`; never discard a popped register pair just because it also happens to be an address.
- Similar-looking union fields are not interchangeable. Batch 101's Link sprite renderer initially
  read `wFileSelect_cursorPos2` at `$cbbd` for the ROM's `ld a,($cbb6)`; the correct named field is
  `wFileSelect_cbb6`. Instruction-level review caught the wrong animation selector before replay.
  The same review found an unconditional `jr` at `$4d27` burned through its `$4d2b` target, which
  silently included the skipped `$4d29` instruction. A jump burn ends at the opcode's physical
  fallthrough address (`$4d29` here), never at its destination.
- A backward jump may re-enter a shared block after its setup rather than at the nearest convenient
  C label. Batch 102 initially sent the `$4350` cursor-collision retry to `$4341`, repeating
  `ld a,b; or a; ret z` and adding four cycles per retry; the ROM jumps to `$4344`, while only the
  separate `$435d` branch enters at `$4341`. Give both ROM addresses distinct C labels even when
  they share the remainder of the block.
- Calling a shared RST helper does not replace the RST opcode's own burn. Batch 103 pushed the
  correct `$588f` return and ran the add-A-to-HL helper, but omitted `CYC($588e,$588f)`, leaving
  every accepted direction input four cycles short. The same review found the following
  `$58b1` register loads attached to the preceding conditional-return burn, shifting a call to
  `$58b3` with return `$58b6` instead of the ROM's `$58b4`/`$58b7`. Burn the conditional
  instruction first with no following effect, then begin the fallthrough instruction at its own
  address.
- Several conditional branches can share a target that performs required setup immediately before
  a separately hookable local entry. Batch 110 initially routed three `jr nz,$5e71` paths to the
  `$5e74` music-setting entry because both paths soon reached the same comparison. That skipped
  `ld a,(wActiveMusic2)`, left the wrong value in A, and lost four cycles. Give the branch target
  its own C label at the exact ROM address; keep the later hook entry distinct even when only one
  instruction separates them.
- A named address inside a routine can be table data even when the transliterator emitted a
  generated function for it. In batch 112, `runTextInput@soundEffects` at `$46fe` was the eight
  bytes read by the root's input index, not an entry point. Porting it as an alias would invent
  executable semantics for sound IDs. Check the disassembly's data directives and actual inbound
  control flow for every apparent local entry; remove confirmed data from `ported.txt` instead.
- Same-size operand substitutions can survive every structural gate while changing game behavior.
  Batch 113 initially read menu load state at adjacent `$cbcc` instead of opened-menu type at
  `$cbcb`; `openMenu` also compared against 1 instead of 3 and requested sound `$56` instead of
  `$54`. All instruction lengths and cycle burns remained valid, so lint and compilation could not
  detect them. Compare every RAM address and immediate literal directly against the ROM report in
  instruction-level review, even when the surrounding control flow is exact.
- RST jump-table wrappers need two distinct effects at the call site: burn the one-byte RST and
  push its physical fallthrough address before emulating the helper. Batch 114 initially combined
  `$41aa`'s three-byte load and `$41ad`'s RST into one burn through `$41ae`, then called the helper
  without pushing `$41ae`; the helper would have popped the enclosing C call's return instead.
  Split the instructions and preserve the RST frame even when the dispatcher immediately becomes
  a C `switch`. The same review caught a taken two-byte `jr nz` incorrectly burned through its
  `$4123` destination instead of its `$4121` fallthrough endpoint.
- Jump-table entries must be matched by their encoded target addresses, not by similar subsystem
  names. Batch 114 initially mapped file-select modes 6 and 7 to `runSecretEntryMenu` and
  `runGameLinkMenu`, nearby in-game wrappers at `$4571` and `$4b22`; the actual table targets are
  `fileSelectMode6` at `$4526` and `fileSelectMode7` at `$4b29`. Read the table words from the ROM
  report and map each address directly before choosing the readable callee.
- The `H8` helpers only address the HRAM array beginning at `$ff80`; IO registers below that base
  must use `mem_rd`/`mem_wr`. Batch 115 initially used `H8(IO_SVBK)` and `H8(IO_LCDC)` for `$ff70`
  and `$ff40`, silently indexing before the HRAM array without a compiler diagnostic. The named
  `W8` helpers likewise token-paste a symbol's `_BANK` definition and therefore accept only a bare
  RAM symbol; use `mem_rd`/`mem_wr` for expressions such as `wDisplayedRupees + 1`.
- A static jump to a known trampoline is still a direct C call even when the trampoline ultimately
  performs a dynamic `jp hl`. Batch 115 initially handed off the static `$512e` jump to
  `interBankCall`; that needlessly switched execution engines. Call the readable trampoline
  directly and let its internal dynamic jump remain interpreted. When a newly readable dispatcher
  replaces a generated target, retarget all known static cases and propagate the caller's real
  stack context through every wrapper.
- Corrupted duplicate code can intentionally call or jump into bytes that are data or operands,
  and those targets must not be rounded to the nearest healthy symbol. Batch 116's `$7dfc` call
  enters rectangle data at `$7de3`, so it remains `CALL_ROM`; its `$7e8c` tail enters the operand
  byte at bank-0 `$05df`, so after burning the jump it uses `hook_continue(gb, 0x05df, sp0_)`.
  `hook_handoff` would incorrectly turn an ordinary static jump into a thread switch, while calling
  the nearby graphics-loader hook would silently repair behavior that exists in the actual ROM.
- A source symbol can collide across banks even when the generated hook names are disambiguated.
  Batch 117 initially put `func_4000` in `rewritten.txt`; regeneration consequently tried to rewrite
  both bank 1 and the unrelated bank 16 routine at `$4000`, then required a nonexistent bank-16 C
  hook. Register the generator's canonical `func_4000_b01` name when only that bank is readable.
  The same batch promoted `func_5c6b` to readable C, exposing its caller's thread-capable return at
  `$4ca4`; add a real post-call alias whenever promoting a callee removes the generated continuation
  that previously handled a possible thread switch.
- A corrupted static jump still follows the known-target rule when its destination is a genuine
  instruction boundary inside readable code. Batch 118's `func_7f90` jumps to `$34ad`, the `ldh`
  inside `updateAllObjects`; leaving it as `hook_continue` regenerated an explicit
  "jumps into rewritten" warning. Promote that exact address to a stable readable continuation,
  factor the destination routine at the boundary, and call it directly. The sibling garbage jumps
  to `$048b` and `$34c7` really enter operand bytes, so those must remain exact interpreter
  continuations rather than being rounded to the surrounding readable routine.
- Script-VM labels are data even when their bytes happen to decode as plausible SM83 instructions.
  Batch 119's `companionScript_subid00Script` began with script opcode `$c7`, which the routine
  scanner reported as an `rst $00`; its four local script targets produced more bogus CPU entries.
  The only external use loaded the root into HL for `interactionSetScript`, and every internal edge
  was a script command operand, so all five rows left `ported.txt` instead of becoming C hooks.
- Closing a generated root can expose an old byte-count defect in an already-readable caller.
  While promoting Bank 12's `parseGivenObjectData`, review found that `func_55f8` burned its
  unconditional two-byte `jr` from `$55f8` through `$55fb`, consuming the first byte of the nearby
  `$55fa` entry. The correct physical endpoint is `$55fa`; recheck existing direct-call and jump
  tails whenever removing their last generated boundary, even when lint and replay were green.
- Rewriting a parent removes all of its transliterator-synthesized local rows, including genuine
  resume points that were never source symbols. Batch 120's nested Nayru and Black Tower states
  called thread-capable room and graphics loaders, but one `$64c0` return was not synthesized at
  all and the other post-call rows would vanish with their parents. Audit every nested direct-call
  chain, add each executable continuation to `extra.sym` and both hook lists, and deliberately
  leave inline `.db` tables such as `$6625` and `$6722` unaliased so they disappear as code.
- A callable RST jump-table dispatcher still owns a return frame on an unrecognized dynamic
  target. Batch 121's first Black Tower rewrite used `hook_handoff` in the `$69be` default arm,
  which would abandon the root's pending `$69b8` continuation; instruction review caught it before
  replay. Capture the dispatcher's entry SP and use `hook_continue(gb, HL, sp0_)` so interpreted
  fallback code runs only until that dispatcher frame returns and the enclosing C caller resumes.
- A smart `CALL_C` needs both the correct readable hook and the callee's exact ROM address. Batch
  122's Timewarp cleanup initially paired five correct hook names with addresses of similarly named
  routines elsewhere (`$3be0` and neighbors instead of `$35e3` and neighbors). Native execution
  looked plausible, but verify mode uses the numeric operand for its interpreter side and would run
  unrelated bytes. Compare every `CALL_C` target operand with the readiness report even when the
  function name, call site, and return address are already correct.
- The ROM source bytes for RAM-resident code are data at runtime, not a second executable routine.
  Closing bank 3 in batch 123 left `oamDmaFunction` at `$4091` looking like the final generated
  function, but startup only copies those ten bytes to `$ff80`; all execution uses the existing
  `hramOamDmaFunction_hook` with explicit `I` timing. Remove the ROM source and end labels from the
  executable registry while retaining the source address for the copy and RAM-hook decoding.
- Applying two instruction effects beside one `CYC` still burns only the first instruction. Batch
  124's `objectDataOp7` performed both `$576e or c` and `$576f ld c,a` but only burned
  `$576e..$576f`; the whole-movie verifier found the resulting one-cycle deficit on two rare enemy
  placement frames after 121,000. Give every instruction its own burn even when both are one byte
  and their C effects fit naturally on one line.
- A routine can have correct branch state and still overburn a taken jump by consuming the skipped
  fallthrough instruction. The full batch-124 verifier found both intro black-bar helpers two cycles
  long: their taken two-byte `jr` spans ended after the skipped `ld (hl),n`. `CYCT` always ends at
  the branch instruction's physical end, while the C control flow determines which later bytes run.
- A final `ret_effect` does not burn the ROM's `ret` instruction by itself. Batch 124's
  `checkEnemyKilled` returned with correct registers and flags but was four cycles short because
  `$5871 ret` had no `CYC($5871,$5872)`; its taken `$5857 jr nc` also ended at the distant target
  instead of `$5859`. The rare allocation path only exposed both in the whole-movie verifier.
  Audit the terminal return separately from the stack effect and each branch locally.
- Infinite thread entry hooks that cross a real scheduler `ld sp` have no stable callable PC/SP
  endpoint for fixed-budget nested verification. Batch 124's `introThreadStart` matched cycles,
  registers, and replacement replay but landed at a different scheduler boundary after
  `runIntro` switched threads. Mark that narrow root `HOOK_NOVERIFY`; keep its independently
  returning callees verifiable and retain the real post-`ld sp` handoff symbol.
- Transliterator-synthesized local names are not durable hook symbols by themselves. Batch 125
  initially added nine `parent__local` names only to `rewritten.txt`; regeneration correctly
  removed the generated bodies, but `gen_hooks.py` could not emit declarations because those
  canonical names were absent from the symbol input, and lint reported nine missing `_hook`
  entries. Give each promoted local its exact bank/address alias in `extra.sym`, then add the same
  canonical name to both `ported.txt` and `rewritten.txt` before regenerating.
- Do not promote every generated local merely to keep the hook count constant. Batch 126 rewrote
  two treasure roots and regeneration removed four `parent__local` rows; diffing the registry and
  checking their readiness reports showed that all four were zero-caller branch targets wholly
  contained in the new structured C. Let those rows disappear. Only locals with independent
  incoming execution or required continuation semantics need `extra.sym` aliases.
- Promoting a callee can change an already-readable caller's report from an unported call to a
  smart call. Batch 127 made bank-3F `loadTreasureDisplayData` readable, but its bank-0 wrapper
  still used `CALL_ROM`; instruction review caught the stale edge before replay. Re-run or audit
  callers of every newly readable cross-bank target, capture `sp0_`, and replace the interpreter
  call with `CALL_C` so normal execution reaches the callee's readable hook while verify mode still
  compares against the exact ROM address.
- A generated caller can retain a parent-local function name after that local is rewritten alone.
  Batch 128 promoted bank-3F `$4256` as
  `updateTileIndexBaseForAllObjects__updateTileIndexBase`; regeneration removed its generated body,
  but the still-generated parent called the now-undefined non-hook name and compilation failed.
  Give a separately promoted local a durable `extra.sym` plus `ported.txt` alias even when it is
  already visible in `generated.txt`; the generated caller can then resolve it through the hook
  table until its own parent becomes readable.
- Re-auditing callers is required even for new callees in bank 0 itself. Batch 128's independent
  reviews found four old bank-0 wrappers still using `CALL_ROM` after their drop-decision,
  availability, palette-refresh, and weapon-GFX callees became readable. Exact call-site reports
  supplied the four physical return addresses before each edge was upgraded to `CALL_C`.
- Local jump-table targets need durable roots when they are rewritten before their parent. Batch
  129 initially planned to alias only `giveTreasure_body__mode4`, the one mode called statically by
  generated mode F. That would still let the generated parent's RST table execute inline generated
  copies of the other eleven modes. Alias every promoted mode in `extra.sym` and `ported.txt` so
  regeneration stops traversal at each target and dispatches all twelve through their C hooks.
- An unknown dynamic jump target does not imply a scheduler handoff. Batch 129's first
  `nextToTileWithInfoText` rewrite used `hook_handoff` after its callable RST jump table; independent
  review caught that the routine's pending return frame would be abandoned. Use
  `hook_continue(gb, HL, sp0_)` for callable dispatch fallback, reserving `hook_handoff` for a real
  `ld sp,*` thread switch.
- A locally emulated RST helper must perform the full return effect after burning its final `ret`.
  Batch 130's `add_a_to_hl_from_rst` initially used only `pop_effect`, which consumed the synthetic
  address but did not restore the emulated PC. Independent instruction review caught it; use
  `ret_effect` so both SP and PC match the ROM helper exactly.
- Cycle count is never a substitute for instruction length, even at a static tail. Batch 130's
  `giveTreasure_body__mode9` initially burned its three-cycle `jr` through `$462a`; the instruction
  at `$4627` is still only two bytes and ends at `$4629`. A byte-range self-review found the
  overburn before replay.
- A file-local helper containing `CALL_C` cannot capture `gb->sp` itself because lint permits
  emulated-register access only in `_hook` shims. Batch 130's bank-0 `give_treasure` integration
  first failed lint for that reason. Capture `sp0_` in each enclosing hook and pass it into the
  helper, including loop callers such as `refillSeedSatchel`.
