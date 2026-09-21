#include "game/game.h"
#include <stdio.h>
#include <stdlib.h>

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

// One byte per ROM address: instruction length, cycles, the extra cycles when a conditional
// branch is taken, and whether the instruction is an unconditional jump. Built from the ROM
// before its code bytes go away, so the native build burns exactly what the ROM walk burns.
void cyctab_build(uint8_t *tab, const uint8_t *rom, size_t size) {
  for (size_t off = 0; off < size; off++) {
    uint8_t op = rom[off], op2 = off + 1 < size ? rom[off + 1] : 0;
    int len = insn_len(op), c = insn_cycles(op, op2, false), ct = insn_cycles(op, op2, true);
    bool jump = op == 0x18 || op == 0xc3 || op == 0xc9 || op == 0xd9 || op == 0xe9;
    tab[off] = (uint8_t)((len - 1) | ((c > 6 ? 7 : c) << 2) | ((ct - c) << 5) | (jump ? 0x80 : 0));
  }
}

uint8_t *cyctab_alloc(const uint8_t *rom, size_t size) {
  uint8_t *tab = malloc(size);
  cyctab_build(tab, rom, size);
  return tab;
}

static void burn_tab(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken) {
  uint16_t a = from;
  while (a < to) {
    size_t off = a < 0x4000 ? a : (size_t)bank * 0x4000 + (a - 0x4000);
    uint8_t e = off < gb->rom_size ? gb->cyctab[off] : 0x04;
    int len = (e & 3) + 1, c = (e >> 2) & 7;
    if (c == 7) c = 32771;
    bool last = (uint16_t)(a + len) >= to;
    if (!last && (e & 0x80)) {
      fprintf(stderr, "burn_rom: range %02x:%04x-%04x runs past an unconditional jump at %04x\n", bank, from, to, a);
      exit(4);
    }
    gb->hook_pc = a;
    gb_burn(gb, c + ((last_taken && last) ? (e >> 5) & 3 : 0));
    a = (uint16_t)(a + len);
  }
}

void burn_rom(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken) {
  if (gb->cyctab) { burn_tab(gb, bank, from, to, last_taken); return; }
  uint16_t a = from;
  while (a < to) {
    uint8_t op = rom_byte(gb, bank, a), op2 = rom_byte(gb, bank, a + 1);
    int len = insn_len(op);
    bool last = (uint16_t)(a + len) >= to;
    if (!last && (op == 0x18 || op == 0xc3 || op == 0xc9 || op == 0xd9 || op == 0xe9)) {
      fprintf(stderr, "burn_rom: range %02x:%04x-%04x runs past an unconditional jump at %04x\n", bank, from, to, a);
      exit(4);
    }
    gb->hook_pc = a;
    { static int bl = -1; if (bl < 0) bl = getenv("BURNLOG") != NULL; if (bl) printf("BURN %02x:%04x op %02x cyc %d mc %llu\n", bank, a, op, insn_cycles(op, op2, last_taken && last), (unsigned long long)gb->mcycles); }
    gb_burn(gb, insn_cycles(op, op2, last_taken && last));
    a = (uint16_t)(a + len);
  }
}

void burn_store_sp(GB *gb, uint16_t a, uint16_t addr) {
  gb->hook_pc = a;
  gb_burn(gb, 4);
  bus_write(gb, addr, (uint8_t)gb->sp);
  gb_burn_nb(gb, 1);
  bus_write(gb, addr + 1, (uint8_t)(gb->sp >> 8));
}
