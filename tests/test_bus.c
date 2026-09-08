#include "unit.h"
#include "core/gb.h"
#include "core/bus.h"

static uint8_t rom[1024 * 1024];

static GB *fresh(int cart_type) {
  static GB gb;
  memset(rom, 0, sizeof rom);
  rom[0x147] = cart_type;
  rom[0x148] = 5;
  rom[0x149] = 3;
  for (int b = 0; b < 64; b++) rom[b * 0x4000] = (uint8_t)b;
  gb_init(&gb);
  ASSERT(gb_load_rom(&gb, rom, sizeof rom));
  gb_reset(&gb);
  return &gb;
}

static void mbc5_rom_banking(void) {
  GB *gb = fresh(0x1b);
  ASSERT_EQ(bus_read(gb, 0x0000), 0);
  ASSERT_EQ(bus_read(gb, 0x4000), 1);
  bus_write(gb, 0x2000, 7);
  ASSERT_EQ(bus_read(gb, 0x4000), 7);
  bus_write(gb, 0x2000, 0);
  ASSERT_EQ(bus_read(gb, 0x4000), 0);
  bus_write(gb, 0x2000, 0x40);
  ASSERT_EQ(bus_read(gb, 0x4000), 0);
}

static void mbc5_ram_banking(void) {
  GB *gb = fresh(0x1b);
  bus_write(gb, 0xa000, 0x42);
  ASSERT_EQ(bus_read(gb, 0xa000), 0xff);
  bus_write(gb, 0x0000, 0x0a);
  bus_write(gb, 0xa000, 0x42);
  bus_write(gb, 0x4000, 1);
  bus_write(gb, 0xa000, 0x43);
  ASSERT_EQ(bus_read(gb, 0xa000), 0x43);
  bus_write(gb, 0x4000, 0);
  ASSERT_EQ(bus_read(gb, 0xa000), 0x42);
}

static void wram_banking(void) {
  GB *gb = fresh(0x1b);
  bus_write(gb, 0xc000, 1);
  bus_write(gb, 0xd000, 2);
  bus_write(gb, 0xff70, 3);
  bus_write(gb, 0xd000, 3);
  ASSERT_EQ(bus_read(gb, 0xff70), 0xf8 | 3);
  bus_write(gb, 0xff70, 0);
  ASSERT_EQ(bus_read(gb, 0xd000), 2);
  ASSERT_EQ(bus_read(gb, 0xe000), 1);
}

static void vram_banking_and_palettes(void) {
  GB *gb = fresh(0x1b);
  bus_write(gb, 0x8000, 0x11);
  bus_write(gb, 0xff4f, 1);
  bus_write(gb, 0x8000, 0x22);
  ASSERT_EQ(bus_read(gb, 0xff4f), 0xff);
  ASSERT_EQ(bus_read(gb, 0x8000), 0x22);
  bus_write(gb, 0xff4f, 0);
  ASSERT_EQ(bus_read(gb, 0x8000), 0x11);
  bus_write(gb, 0xff68, 0x80);
  bus_write(gb, 0xff69, 0x1f);
  bus_write(gb, 0xff69, 0x7c);
  ASSERT_EQ(bus_read(gb, 0xff68) & 0xbf, 0x80 | 2);
  ASSERT_EQ(gb->bg_pal[0], 0x1f);
  ASSERT_EQ(gb->bg_pal[1], 0x7c);
}

static void joypad(void) {
  GB *gb = fresh(0x1b);
  gb->joy = JOY_A | JOY_LEFT;
  gb->joy_pending = gb->joy;
  bus_write(gb, 0xff00, 0x20);
  ASSERT_EQ(bus_read(gb, 0xff00), 0xc0 | 0x20 | 0x0d);
  bus_write(gb, 0xff00, 0x10);
  ASSERT_EQ(bus_read(gb, 0xff00), 0xc0 | 0x10 | 0x0e);
  bus_write(gb, 0xff00, 0x30);
  ASSERT_EQ(bus_read(gb, 0xff00), 0xff);
}

int main(void) {
  RUN(mbc5_rom_banking); RUN(mbc5_ram_banking); RUN(wram_banking);
  RUN(vram_banking_and_palettes); RUN(joypad);
  return 0;
}
