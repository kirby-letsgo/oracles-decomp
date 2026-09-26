#include "core/gb.h"

static const int tac_bit[4] = {9, 3, 5, 7};

static bool timer_bit(uint16_t counter, uint8_t tac) {
  return (tac & 4) && ((counter >> tac_bit[tac & 3]) & 1);
}

static void tima_inc(GB *gb) {
  if (++gb->io[R_TIMA] == 0) gb->tima_reload = 1;
}

static void timer_step(GB *gb) {
  if (gb->tima_reload == 1) {
    gb->io[R_TIMA] = gb->io[R_TMA];
    gb->io[R_IF] |= INT_TIMER;
    gb->tima_reload = 2;
  } else if (gb->tima_reload == 2) {
    gb->tima_reload = 0;
  }
  uint16_t old = gb->div_counter;
  gb->div_counter += 4;
  if (timer_bit(old, gb->io[R_TAC]) && !timer_bit(gb->div_counter, gb->io[R_TAC])) tima_inc(gb);
}

// The CPU accesses the timer at the start of its M-cycle, before the timer's step in that cycle
// (tw_, tr_, th_, thr_, dv_ ROMs: SameBoy and GBHawk), while gb_tick steps the timer before the
// access. timer_tick keeps the state from before its step; reads see it, and writes apply to it and
// redo the step. The state only lives from a step to the access in the same M-cycle, so it stays out of
// the GB struct (and out of save states).
static struct { uint16_t div; uint8_t tima, if_timer; int reload; } pre;

void timer_tick(GB *gb) {
  pre.div = gb->div_counter;
  pre.tima = gb->io[R_TIMA];
  pre.reload = gb->tima_reload;
  pre.if_timer = gb->io[R_IF] & INT_TIMER;
  timer_step(gb);
}

uint8_t timer_io_read(GB *gb, uint8_t r) {
  return r == R_DIV ? pre.div >> 8 : pre.tima;
}

void timer_io_write(GB *gb, uint8_t r, uint8_t v) {
  gb->div_counter = pre.div;
  gb->io[R_TIMA] = pre.tima;
  gb->tima_reload = pre.reload;
  gb->io[R_IF] = (gb->io[R_IF] & ~INT_TIMER) | pre.if_timer;
  if (r == R_DIV) timer_write_div(gb);
  else if (r == R_TAC) timer_write_tac(gb, v);
  else if (r == R_TMA) { gb->io[R_TMA] = v; if (gb->tima_reload == 2) gb->io[R_TIMA] = v; }
  else if (gb->tima_reload != 2) { gb->io[R_TIMA] = v; gb->tima_reload = 0; }
  timer_tick(gb);
}

void timer_write_div(GB *gb) {
  uint16_t old = gb->div_counter;
  gb->div_counter = 0;
  if (timer_bit(old, gb->io[R_TAC])) tima_inc(gb);
}

void timer_write_tac(GB *gb, uint8_t v) {
  bool was = timer_bit(gb->div_counter, gb->io[R_TAC]);
  gb->io[R_TAC] = v & 7;
  if (was && !timer_bit(gb->div_counter, gb->io[R_TAC])) tima_inc(gb);
}
