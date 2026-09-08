#include <stdlib.h>
#include <stdio.h>
#include "gb.h"
#include "bus.h"
#include "hash.h"
#include <string.h>

void gb_init(GB *gb) { memset(gb, 0, sizeof *gb); }

bool gb_load_rom(GB *gb, const uint8_t *rom, size_t size) {
  if (size < 0x8000 || (size & 0x3fff)) return false;
  gb->rom = rom;
  gb->rom_size = size;
  switch (rom[0x147]) {
  case 0x00: gb->mbc = MBC_NONE; break;
  case 0x01: case 0x02: case 0x03: gb->mbc = MBC_1; break;
  case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: gb->mbc = MBC_5; break;
  default: return false;
  }
  static const size_t ram_sizes[] = {0, 0, 8192, 32768, 131072, 65536};
  uint8_t rs = rom[0x149];
  gb->eram_size = rs < 6 ? ram_sizes[rs] : 0;
  if (gb->eram_size > sizeof gb->eram) gb->eram_size = sizeof gb->eram;
  return true;
}

void gb_set_boot_rom(GB *gb, const uint8_t *boot, size_t size) {
  gb->boot = boot;
  gb->boot_size = size;
}

static void post_boot_state(GB *gb) {
  gb->a = 0x11; gb->f = 0x80; gb->b = 0x01; gb->c = 0x00;
  gb->d = 0xff; gb->e = 0x56; gb->h = 0x00; gb->l = 0x0d;
  gb->sp = 0xfffe; gb->pc = 0x0100;
  static const uint8_t init_io[][2] = {
    {R_P1, 0xcf}, {R_SC, 0x7f}, {R_TAC, 0xf8}, {R_LCDC, 0x91},
    {R_BGP, 0xfc}, {R_KEY1, 0x7e}, {R_VBK, 0x00}, {R_HDMA5, 0xff}, {R_SVBK, 0x00},
  };
  for (size_t i = 0; i < sizeof init_io / sizeof init_io[0]; i++) gb->io[init_io[i][0]] = init_io[i][1];
  gb->io[R_IF] = 0x01;
  gb->io[R_STAT] = 0x02;
  gb->ppu_mode = 2;
  gb->mode3_end = 254;
  gb->boot_mapped = false;
}

void gb_reset(GB *gb) {
  const uint8_t *rom = gb->rom;
  size_t rom_size = gb->rom_size;
  int mbc = gb->mbc;
  size_t eram_size = gb->eram_size;
  const uint8_t *boot = gb->boot;
  size_t boot_size = gb->boot_size;
  void (*serial_out)(void *, uint8_t) = gb->serial_out;
  void *serial_ctx = gb->serial_ctx;
  static uint8_t saved_wram[8][4096], saved_hram[127];
  bool keep_ram = gb->init_ram_loaded;
  if (keep_ram) { memcpy(saved_wram, gb->wram, sizeof saved_wram); memcpy(saved_hram, gb->hram, sizeof saved_hram); }
  memset(gb, 0, sizeof *gb);
  if (keep_ram) { memcpy(gb->wram, saved_wram, sizeof saved_wram); memcpy(gb->hram, saved_hram, sizeof saved_hram); }
  gb->init_ram_loaded = keep_ram;
  gb->rom = rom; gb->rom_size = rom_size; gb->mbc = mbc; gb->eram_size = eram_size;
  memset(gb->eram, 0xff, sizeof gb->eram);
  gb->boot = boot; gb->boot_size = boot_size;
  gb->serial_out = serial_out; gb->serial_ctx = serial_ctx;
  gb->rom_bank = 1;
  for (int i = 0; i < FB_W * FB_H; i++) gb->framebuffer[i] = 0x7fff;
  apu_reset(&gb->apu);
  if (gb->boot) {
    gb->boot_mapped = true;
    gb->pc = 0;
    gb->io[R_STAT] = 0;
    while (gb->boot_mapped && !gb->hung) {
      static const char *ba = NULL; static bool ba_init = false; static uint8_t seen[65536];
      if (!ba_init) { ba = getenv("BOOT_ANCHORS"); ba_init = true; }
      if (ba && !seen[gb->pc]) { const char *q = ba; while (*q) { if (gb->pc == strtol(q, NULL, 16)) { seen[gb->pc] = 1; printf("BOOTANCHOR %04x %llu\n", gb->pc, (unsigned long long)gb->mcycles); break; } while (*q && *q != ' ') q++; while (*q == ' ') q++; } }
      gb_step(gb);
    }
    gb->b = 0x01;
  } else {
    post_boot_state(gb);
    gb->cycles = BOOT_CYCLES;
  }
}

void gb_tick(GB *gb) {
  timer_tick(gb);
  int dots = gb->double_speed ? 2 : 4;
  ppu_tick(gb, dots);
  apu_tick(&gb->apu, dots);
  gb->cycles += dots;
  gb->mcycles++;
  if (!gb->sampled && gb->cycles >= gb->sample_at) { gb->sampled = true; memcpy(gb->sample_wram, gb->wram, sizeof gb->sample_wram); memcpy(gb->sample_hram, gb->hram, sizeof gb->sample_hram); }
}

void gb_run_cycles(GB *gb, uint64_t target) {
  while (gb->cycles < target && !gb->hung) gb_step(gb);
}

int64_t gb_grid_offset = GRID_OFFSET;
void gb_run_frame(GB *gb) {
  uint64_t target = GRID_END(GRID_FRAME(gb->cycles) + 1);
  gb->joy_latched = false;
  gb_run_cycles(gb, target);
  if (!gb->joy_latched && gb->halted && (gb->ie & 0x10)) { gb->joy = gb->joy_pending; if (getenv("JOYLOG")) printf("%llu LATCH halted joy %02x\n", (unsigned long long)GRID_FRAME(gb->cycles) - 1, gb->joy); }
}

void gb_run_until_vblank(GB *gb) {
  bool lcd_on = gb->io[R_LCDC] & 0x80;
  uint64_t limit = gb->cycles + (lcd_on ? FRAME_CYCLES * 2 : FRAME_CYCLES);
  gb->frame_ready = false;
  while (!gb->frame_ready && gb->cycles < limit && !gb->hung) gb_step(gb);
}

uint64_t gb_state_hash(const GB *gb) {
  uint64_t h = FNV1A64_INIT;
  h = fnv1a64_update(h, (const unsigned char *)gb->wram, sizeof gb->wram);
  h = fnv1a64_update(h, gb->hram, sizeof gb->hram);
  h = fnv1a64_update(h, (const unsigned char *)gb->vram, sizeof gb->vram);
  h = fnv1a64_update(h, gb->oam, sizeof gb->oam);
  h = fnv1a64_update(h, gb->bg_pal, sizeof gb->bg_pal);
  h = fnv1a64_update(h, gb->ob_pal, sizeof gb->ob_pal);
  h = fnv1a64_update(h, gb->io, sizeof gb->io);
  uint8_t regs[] = {gb->a, gb->f, gb->b, gb->c, gb->d, gb->e, gb->h, gb->l,
                    (uint8_t)gb->sp, (uint8_t)(gb->sp >> 8), (uint8_t)gb->pc, (uint8_t)(gb->pc >> 8),
                    gb->ie, (uint8_t)gb->rom_bank, (uint8_t)(gb->rom_bank >> 8), gb->ram_bank};
  return fnv1a64_update(h, regs, sizeof regs);
}

uint64_t gb_frame_hash(const GB *gb) {
  return fnv1a64_update(FNV1A64_INIT, (const unsigned char *)gb->framebuffer, sizeof gb->framebuffer);
}
