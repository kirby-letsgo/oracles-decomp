// The CPU pieces both engines share: interrupt entry, the speed switch and the debug counters.
// cpu.c (interpreter) and src/rt/native.c (dispatch only) each add their own gb_step.
#include "gb.h"
#include "bus.h"
#include <stdio.h>

uint64_t dbg_instr_count, dbg_int_count[5];
uint32_t *dbg_pc_hist;
int dbg_log_ints;

uint64_t gb_irq_dispatched;

// The vector is chosen after the high PC push, from IE as that push left it: a higher-priority
// interrupt raised meanwhile wins, and one that is gone leaves PC at 0 (iv_* ROMs, SameBoy and
// GBHawk).
void cpu_dispatch_interrupt(GB *gb) {
  gb_irq_dispatched++;
  gb_tick(gb);
  gb_tick(gb);
  gb->ime = false;
  uint16_t pc = gb->pc;
  gb_tick(gb);
  gb_tick(gb); bus_write(gb, --gb->sp, pc >> 8);
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  gb_tick(gb); bus_write(gb, --gb->sp, (uint8_t)pc);
  if (!pending) { gb->pc = 0; return; }
  int i = 0;
  while (!(pending & (1 << i))) i++;
  gb->io[R_IF] &= ~(1 << i);
  dbg_int_count[i]++;
  if (dbg_log_ints) printf("INT %d mc %llu frame %llu pc %04x sp %04x\n", i, (unsigned long long)gb->mcycles, (unsigned long long)GRID_FRAME(gb->cycles), pc, gb->sp + 2);
  gb->pc = 0x40 + i * 8;
}

void gb_speed_switch(GB *gb) {
  gb->speed_armed = false;
  gb->double_speed = !gb->double_speed;
  timer_write_div(gb);
  ppu_tick(gb, 12);
  for (int i = 0; i < 32769; i++) gb_tick(gb);
}
