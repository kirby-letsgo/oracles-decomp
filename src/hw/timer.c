#include "core/gb.h"

static const int tac_bit[4] = {9, 3, 5, 7};

static bool timer_bit(uint16_t counter, uint8_t tac) {
  return (tac & 4) && ((counter >> tac_bit[tac & 3]) & 1);
}

static void tima_inc(GB *gb) {
  if (++gb->io[R_TIMA] == 0) gb->tima_reload = 1;
}

void timer_tick(GB *gb) {
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
