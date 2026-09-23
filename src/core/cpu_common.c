// The CPU pieces both engines share: interrupt entry, the speed switch and the debug counters.
// cpu.c (interpreter) and src/rt/native.c (dispatch only) each add their own gb_step.
#include "gb.h"
#include "bus.h"
#include <stdio.h>

uint64_t dbg_instr_count, dbg_int_count[5];
uint32_t *dbg_pc_hist;
int dbg_log_ints;

static void cpu_push(GB *gb, uint16_t v) {
  gb_tick(gb);
  gb_tick(gb); bus_write(gb, --gb->sp, v >> 8);
  gb_tick(gb); bus_write(gb, --gb->sp, (uint8_t)v);
}

uint64_t gb_irq_dispatched;

void cpu_dispatch_interrupt(GB *gb) {
  gb_irq_dispatched++;
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  gb_tick(gb);
  gb_tick(gb);
  gb->ime = false;
  int i = 0;
  while (!(pending & (1 << i))) i++;
  gb->io[R_IF] &= ~(1 << i);
  dbg_int_count[i]++;
  if (dbg_log_ints) printf("INT %d mc %llu frame %llu pc %04x sp %04x\n", i, (unsigned long long)gb->mcycles, (unsigned long long)GRID_FRAME(gb->cycles), gb->pc, gb->sp);
  cpu_push(gb, gb->pc);
  gb->pc = 0x40 + i * 8;
}

void gb_speed_switch(GB *gb) {
  gb->speed_armed = false;
  gb->double_speed = !gb->double_speed;
  timer_write_div(gb);
  ppu_tick(gb, 12);
  for (int i = 0; i < 32769; i++) gb_tick(gb);
}
