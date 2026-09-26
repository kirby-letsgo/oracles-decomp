#include "bus.h"
#include "hw/hw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t wram_bank(const GB *gb) {
  uint8_t b = gb->io[R_SVBK] & 7;
  return b ? b : 1;
}

uint8_t bus_read(GB *gb, uint16_t a) {
  if (a < 0x8000) {
    if (gb->boot_mapped && (a < 0x100 || (a >= 0x200 && a < 0x900)) && a < gb->boot_size) return gb->boot[a];
    uint32_t banks = gb->rom_size / 0x4000;
    size_t off = a < 0x4000 ? a : (((gb->rom_bank % banks) << 14) | (a & 0x3fff));
    if (gb->code_bits && (gb->code_bits[off >> 3] >> (off & 7) & 1)) {
      static int log = -1; static uint8_t *seen;
      if (log < 0) { log = getenv("CODEMAP_LOG") != NULL; if (log) seen = calloc(1 << 18, 1); }
      if (!log) {
        fprintf(stderr, "rom_rd: data read of code byte %02x:%04x (frame %llu, mc %llu, hook pc %04x)\n", a < 0x4000 ? 0 : gb->rom_bank, a, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)gb->mcycles, gb->hook_pc);
        exit(5);
      }
      if (!(seen[off >> 3] >> (off & 7) & 1)) { seen[off >> 3] |= (uint8_t)(1 << (off & 7)); printf("CODEREAD %02x:%04x off %06zx hookpc %04x frame %llu\n", a < 0x4000 ? 0 : gb->rom_bank, a, off, gb->hook_pc, (unsigned long long)GRID_FRAME(gb->cycles)); }
    }
    return gb->rom[off];
  }
  if (a < 0xa000) {
    // the PPU owns VRAM in mode 3: CPU reads see $ff (vr_* ROMs, SameBoy and GBHawk)
    if ((gb->io[R_LCDC] & 0x80) && gb->ppu_mode == 3) return 0xff;
    return gb->vram[gb->io[R_VBK] & 1][a - 0x8000];
  }
  if (a < 0xc000) {
    if (!gb->ram_enabled || gb->eram_size == 0) return 0xff;
    return gb->eram[(((uint32_t)gb->ram_bank << 13) | (a - 0xa000)) % gb->eram_size];
  }
  if (a < 0xd000) return gb->wram[0][a - 0xc000];
  if (a < 0xe000) return gb->wram[wram_bank(gb)][a - 0xd000];
  if (a < 0xfe00) return bus_read(gb, a - 0x2000);
  if (a < 0xfea0) return gb->oam[a - 0xfe00];
  if (a < 0xff00) return 0x00;
  if (a < 0xff80) return io_read(gb, a & 0x7f);
  if (a < 0xffff) return gb->hram[a - 0xff80];
  return gb->ie;
}

static void mbc_write(GB *gb, uint16_t a, uint8_t v) {
  if (gb->mbc == MBC_5) {
    if (a < 0x2000) gb->ram_enabled = (v & 0x0f) == 0x0a;
    else if (a < 0x3000) gb->rom_bank = (gb->rom_bank & 0x100) | v;
    else if (a < 0x4000) gb->rom_bank = (gb->rom_bank & 0xff) | ((v & 1) << 8);
    else if (a < 0x6000) gb->ram_bank = v & 0x0f;
  } else if (gb->mbc == MBC_1) {
    if (a < 0x2000) gb->ram_enabled = (v & 0x0f) == 0x0a;
    else if (a < 0x4000) { uint8_t b = v & 0x1f; gb->rom_bank = b ? b : 1; }
    else if (a < 0x6000) gb->ram_bank = v & 3;
  }
}

static uint16_t watch_addr; static int watch_init;
void bus_write(GB *gb, uint16_t a, uint8_t v) {
  if (!watch_init) { watch_init = 1; watch_addr = getenv("WATCH") ? (uint16_t)strtoul(getenv("WATCH"), NULL, 16) : 0; }
  if (watch_addr && (a == watch_addr || a == watch_addr + 1)) printf("WATCH %04x=%02x frame %llu mc %llu pc %04x hookpc %04x sp %04x\n", a, v, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)gb->mcycles, gb->pc, gb->hook_pc, gb->sp);
  if (a < 0x8000) { mbc_write(gb, a, v); return; }
  if (a < 0xa000) {
    if ((gb->io[R_LCDC] & 0x80) && gb->ppu_mode == 3) return;     // ignored in mode 3 (vw_* ROMs)
    gb->vram[gb->io[R_VBK] & 1][a - 0x8000] = v;
    return;
  }
  if (a < 0xc000) {
    if (gb->ram_enabled && gb->eram_size)
      gb->eram[(((uint32_t)gb->ram_bank << 13) | (a - 0xa000)) % gb->eram_size] = v;
    return;
  }
  if (a < 0xd000) { gb->wram[0][a - 0xc000] = v; return; }
  if (a < 0xe000) { gb->wram[wram_bank(gb)][a - 0xd000] = v; return; }
  if (a < 0xfe00) { bus_write(gb, a - 0x2000, v); return; }
  if (a < 0xfea0) { gb->oam[a - 0xfe00] = v; return; }
  if (a < 0xff00) return;
  if (a < 0xff80) { io_write(gb, a & 0x7f, v); return; }
  if (a < 0xffff) { gb->hram[a - 0xff80] = v; return; }
  gb->ie = v;
}
