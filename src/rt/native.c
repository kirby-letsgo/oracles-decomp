// The native runtime: the step loop without an interpreter. Every address the game executes
// must be a hook entry; anything else is a hard error. This file replaces src/core/cpu.c in the
// native build and provides the same symbols the core, the hooks and the runner use.
#include "core/gb.h"
#include "core/bus.h"
#include "hooks/hooks.h"
#include <stdio.h>
#include <stdlib.h>

uint64_t dbg_instr_count, dbg_int_count[5];
uint32_t *dbg_pc_hist;
int dbg_log_ints;
extern uint64_t dbg_vbl_step, dbg_vbl_step_halted;

__attribute__((constructor)) static void mark_native(void) { hook_native = true; }

static void push(GB *gb, uint16_t v) {
  gb_tick(gb);
  gb_tick(gb); bus_write(gb, --gb->sp, v >> 8);
  gb_tick(gb); bus_write(gb, --gb->sp, (uint8_t)v);
}

void cpu_dispatch_interrupt(GB *gb) {
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  gb_tick(gb);
  gb_tick(gb);
  gb->ime = false;
  int i = 0;
  while (!(pending & (1 << i))) i++;
  gb->io[R_IF] &= ~(1 << i);
  dbg_int_count[i]++;
  if (dbg_log_ints) printf("INT %d mc %llu frame %llu pc %04x sp %04x\n", i, (unsigned long long)gb->mcycles, (unsigned long long)GRID_FRAME(gb->cycles), gb->pc, gb->sp);
  push(gb, gb->pc);
  gb->pc = 0x40 + i * 8;
}

void gb_speed_switch(GB *gb) {
  gb->speed_armed = false;
  gb->double_speed = !gb->double_speed;
  timer_write_div(gb);
  ppu_tick(gb, 12);
  for (int i = 0; i < 32769; i++) gb_tick(gb);
}

void gb_step(GB *gb) {
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
