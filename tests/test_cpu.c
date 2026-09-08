#include "unit.h"
#include "core/gb.h"
#include "core/bus.h"

static uint8_t rom[0x8000];
static GB gb;

static void load(const uint8_t *code, size_t n) {
  memset(rom, 0, sizeof rom);
  rom[0x147] = 0;
  memcpy(rom + 0x100, code, n);
  gb_init(&gb);
  ASSERT(gb_load_rom(&gb, rom, sizeof rom));
  gb_reset(&gb);
}

static void run_until_halt(int max_steps) {
  for (int i = 0; i < max_steps && !gb.halted && !gb.hung; i++) gb_step(&gb);
}

static void add_and_flags(void) {
  const uint8_t code[] = {0x3e, 0x0f, 0xc6, 0x01, 0x76};
  load(code, sizeof code);
  run_until_halt(10);
  ASSERT_EQ(gb.a, 0x10);
  ASSERT_EQ(gb.f, 0x20);
}

static void sub_borrow_and_cp(void) {
  const uint8_t code[] = {0x3e, 0x00, 0xd6, 0x01, 0xfe, 0xff, 0x76};
  load(code, sizeof code);
  run_until_halt(10);
  ASSERT_EQ(gb.a, 0xff);
  ASSERT_EQ(gb.f, 0xc0);
}

static void call_ret_and_stack(void) {
  const uint8_t code[] = {0x31, 0xfe, 0xff, 0xcd, 0x0a, 0x01, 0x76, 0x00, 0x00, 0x00, 0x06, 0x42, 0xc9};
  load(code, sizeof code);
  run_until_halt(10);
  ASSERT_EQ(gb.b, 0x42);
  ASSERT_EQ(gb.sp, 0xfffe);
  ASSERT_EQ(gb.pc, 0x0107);
}

static void cb_bit_and_swap(void) {
  const uint8_t code[] = {0x3e, 0xa5, 0xcb, 0x37, 0xcb, 0x47, 0x76};
  load(code, sizeof code);
  run_until_halt(10);
  ASSERT_EQ(gb.a, 0x5a);
  ASSERT_EQ(gb.f & 0x80, 0x80);
}

static void daa_after_add(void) {
  const uint8_t code[] = {0x3e, 0x45, 0xc6, 0x38, 0x27, 0x76};
  load(code, sizeof code);
  run_until_halt(10);
  ASSERT_EQ(gb.a, 0x83);
}

static void instruction_timing(void) {
  const uint8_t code[] = {0x00, 0x3e, 0x01, 0xc3, 0x07, 0x01, 0x00, 0x76};
  load(code, sizeof code);
  uint64_t start = gb.cycles;
  gb_step(&gb);
  ASSERT_EQ(gb.cycles - start, 4);
  gb_step(&gb);
  ASSERT_EQ(gb.cycles - start, 12);
  gb_step(&gb);
  ASSERT_EQ(gb.cycles - start, 28);
}

int main(void) {
  RUN(add_and_flags); RUN(sub_borrow_and_cp); RUN(call_ret_and_stack);
  RUN(cb_bit_and_swap); RUN(daa_after_add); RUN(instruction_timing);
  return 0;
}
