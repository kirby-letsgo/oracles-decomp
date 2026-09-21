// The native runtime: the step loop without an interpreter. Every address the game executes
// must be a hook entry; anything else is a hard error. This file replaces src/core/cpu.c in the
// native build (cpu_common.c supplies the interrupt entry and the speed switch to both).
#include "core/gb.h"
#include "core/bus.h"
#include "hooks/hooks.h"
#include <stdio.h>
#include <stdlib.h>

extern uint64_t dbg_vbl_step, dbg_vbl_step_halted, dbg_instr_count;
extern uint32_t *dbg_pc_hist;

void gb_step(GB *gb) {
  gb->native = true;
  if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  bool was_halted = gb->halted;
  if (gb->halted) {
    gb_tick(gb);
    pending = gb->ie & gb->io[R_IF] & 0x1f;
    if (!pending) return;
    gb->halted = false;
    gb_tick(gb);
  }
  if (gb->ime && pending && !hook_suppress_interrupts) { if (pending & INT_VBLANK) { dbg_vbl_step++; if (was_halted) dbg_vbl_step_halted++; } cpu_dispatch_interrupt(gb); return; }
  if (gb->ime_delay) { gb->ime = true; gb->ime_delay = false; }
  if (hook_dispatch(gb)) return;
  dbg_instr_count++;
  if (dbg_pc_hist) dbg_pc_hist[((gb->pc < 0x4000 ? 0 : gb->rom_bank & 0x7f) << 15) | (gb->pc & 0x7fff)]++;
  fprintf(stderr, "native: no code at %02x:%04x (frame %llu, sp %04x, mc %llu)\n", gb->pc < 0x4000 || gb->pc >= 0x8000 ? 0 : gb->rom_bank, gb->pc, (unsigned long long)GRID_FRAME(gb->cycles), gb->sp, (unsigned long long)gb->mcycles);
  gb->hung = true;
}
