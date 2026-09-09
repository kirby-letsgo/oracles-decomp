#include "game/game.h"

static uint8_t rom_byte(const GB *gb, int bank, uint16_t addr) {
  size_t off = addr < 0x4000 ? addr : (size_t)bank * 0x4000 + (addr - 0x4000);
  return off < gb->rom_size ? gb->rom[off] : 0;
}

static int insn_len(uint8_t op) {
  if (op == 0xcb) return 2;
  int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
  if (x == 0) {
    if (z == 0) return y == 0 ? 1 : y == 1 ? 3 : 2;
    if (z == 1) return (y & 1) ? 1 : 3;
    if (z == 6) return 2;
    return 1;
  }
  if (x == 1 || x == 2) return 1;
  if (z == 0) return y < 4 ? 1 : 2;
  if (z == 1) return 1;
  if (z == 2) return (y < 4 || y == 5 || y == 7) ? 3 : 1;
  if (z == 3) return y == 0 ? 3 : 1;
  if (z == 4) return 3;
  if (z == 5) return (y & 1) ? 3 : 1;
  if (z == 6) return 2;
  return 1;
}

static int insn_cycles(uint8_t op, uint8_t op2, bool taken) {
  if (op == 0xcb) {
    bool mem = (op2 & 7) == 6;
    if (op2 >= 0x40 && op2 < 0x80) return mem ? 3 : 2;
    return mem ? 4 : 2;
  }
  int x = op >> 6, y = (op >> 3) & 7, z = op & 7, q = y & 1;
  if (x == 0) {
    if (z == 0) {
      if (y == 0) return 1;
      if (y == 1) return 5;
      if (y == 2) return 32771;
      if (y == 3) return 3;
      return taken ? 3 : 2;
    }
    if (z == 1) return q ? 2 : 3;
    if (z == 2 || z == 3) return 2;
    if (z == 4 || z == 5) return y == 6 ? 3 : 1;
    if (z == 6) return y == 6 ? 3 : 2;
    return 1;
  }
  if (op == 0x76) return 1;
  if (x == 1) return (y == 6 || z == 6) ? 2 : 1;
  if (x == 2) return z == 6 ? 2 : 1;
  if (z == 0) {
    if (y < 4) return taken ? 5 : 2;
    return y == 5 ? 4 : 3;
  }
  if (z == 1) {
    if (!q) return 3;
    return y == 5 ? 1 : y == 7 ? 2 : 4;
  }
  if (z == 2) {
    if (y < 4) return taken ? 4 : 3;
    return (y == 4 || y == 6) ? 2 : 4;
  }
  if (z == 3) return y == 0 ? 4 : 1;
  if (z == 4) return taken ? 6 : 3;
  if (z == 5) return q ? 6 : 4;
  if (z == 6) return 2;
  return 4;
}

void burn_rom(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken) {
  uint16_t a = from;
  while (a < to) {
    uint8_t op = rom_byte(gb, bank, a), op2 = rom_byte(gb, bank, a + 1);
    int len = insn_len(op);
    bool last = (uint16_t)(a + len) >= to;
    gb->hook_pc = a;
    gb_burn(gb, insn_cycles(op, op2, last_taken && last));
    a = (uint16_t)(a + len);
  }
}
