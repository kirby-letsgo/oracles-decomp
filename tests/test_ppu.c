#include "unit.h"
#include "core/gb.h"
#include "core/bus.h"

static uint8_t rom[0x8000];
static GB gb;

static void fresh(void) {
  memset(rom, 0, sizeof rom);
  gb_init(&gb);
  ASSERT(gb_load_rom(&gb, rom, sizeof rom));
  gb_reset(&gb);
  gb.io[R_IF] = 0;
}

static void ticks(int dots) { for (int i = 0; i < dots; i += 4) ppu_tick(&gb, 4); }

static void modes_within_a_line(void) {
  fresh();
  ASSERT_EQ(gb.ppu_mode, 2);
  ticks(84);
  ASSERT_EQ(gb.ppu_mode, 3);
  ticks(172);
  ASSERT_EQ(gb.ppu_mode, 0);
  ticks(204);
  ASSERT_EQ(gb.io[R_LY], 1);
  ASSERT_EQ(gb.ppu_mode, 2);
}

static void vblank_at_line_144(void) {
  fresh();
  ticks(456 * 144 + 4);
  ASSERT_EQ(gb.io[R_LY], 144);
  ASSERT_EQ(gb.ppu_mode, 1);
  ASSERT(gb.io[R_IF] & INT_VBLANK);
  ASSERT_EQ(gb.frame_count, 1);
  ticks(456 * 10);
  ASSERT_EQ(gb.io[R_LY], 0);
}

static void lyc_interrupt(void) {
  fresh();
  bus_write(&gb, 0xff45, 5);
  bus_write(&gb, 0xff41, 0x40);
  ticks(456 * 5 - 4);
  ASSERT(!(gb.io[R_IF] & INT_STAT));
  while (gb.io[R_LY] != 5) { ASSERT(!(gb.io[R_IF] & INT_STAT)); ppu_tick(&gb, 2); }
  ASSERT(gb.io[R_IF] & INT_STAT);
  ASSERT(!(bus_read(&gb, 0xff41) & 4));
  ppu_tick(&gb, 2);
  ASSERT(bus_read(&gb, 0xff41) & 4);
}

static void hblank_interrupt_once_per_line(void) {
  fresh();
  bus_write(&gb, 0xff41, 0x08);
  ticks(456);
  ASSERT(gb.io[R_IF] & INT_STAT);
  gb.io[R_IF] = 0;
  ticks(100);
  ASSERT(!(gb.io[R_IF] & INT_STAT));
}

static void lcd_off_resets(void) {
  fresh();
  ticks(456 * 10);
  bus_write(&gb, 0xff40, 0x11);
  ASSERT_EQ(gb.io[R_LY], 0);
  ASSERT_EQ(gb.ppu_mode, 0);
  ticks(456 * 10);
  ASSERT_EQ(gb.io[R_LY], 0);
  ASSERT_EQ(gb.framebuffer[0], 0x7fff);
  bus_write(&gb, 0xff40, 0x91);
  ticks(452);
  ASSERT_EQ(gb.io[R_LY], 1);
}

int main(void) {
  RUN(modes_within_a_line); RUN(vblank_at_line_144); RUN(lyc_interrupt);
  RUN(hblank_interrupt_once_per_line); RUN(lcd_off_resets);
  return 0;
}
