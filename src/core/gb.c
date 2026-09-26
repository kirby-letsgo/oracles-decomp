#include <stdlib.h>
#include <stdio.h>
#include "gb.h"
#include "bus.h"
#include "hash.h"
#include "hw/render.h"
#include <string.h>

void gb_init(GB *gb) { memset(gb, 0, sizeof *gb); gb->samples = calloc(16, sizeof *gb->samples); gb->step = gb_step; }

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
  GBSample *samples = gb->samples;
  void (*step)(GB *) = gb->step;
  DispatchRing *ring = gb->ring;
  struct Fibers *fib = gb->fib;
  const uint8_t *cyctab = gb->cyctab, *code_bits = gb->code_bits;
  uint64_t trace_lo = gb->trace_lo, trace_hi = gb->trace_hi;
  bool keep_ram = gb->init_ram_loaded;
  if (keep_ram) { memcpy(saved_wram, gb->wram, sizeof saved_wram); memcpy(saved_hram, gb->hram, sizeof saved_hram); }
  memset(gb, 0, sizeof *gb);
  gb->samples = samples ? samples : calloc(16, sizeof *gb->samples);
  gb->step = step ? step : gb_step;
  gb->ring = ring; gb->trace_lo = trace_lo; gb->trace_hi = trace_hi; gb->fib = fib; gb->cyctab = cyctab; gb->code_bits = code_bits;
  gb->next_sample_at = UINT64_MAX;
  if (keep_ram) { memcpy(gb->wram, saved_wram, sizeof saved_wram); memcpy(gb->hram, saved_hram, sizeof saved_hram); }
  gb->init_ram_loaded = keep_ram;
  gb->rom = rom; gb->rom_size = rom_size; gb->mbc = mbc; gb->eram_size = eram_size;
  memset(gb->eram, 0xff, sizeof gb->eram);
  gb->boot = boot; gb->boot_size = boot_size;
  gb->serial_out = serial_out; gb->serial_ctx = serial_ctx;
  gb->rom_bank = 1;
  gb->div_counter = 4;
  gb->sample = &gb->samples[0];
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
      gb->step(gb);
    }
    gb->b = 0x01;
  } else {
    post_boot_state(gb);
    gb->cycles = BOOT_CYCLES;
  }
  gb->next_sample_at = GRID_END(GRID_FRAME(gb->cycles) + 1);
  gb->sample_head = gb->sample_count = 0;
  gb->joy_latched = false; gb->joy_read = false;
}

void gb_tick(GB *gb) {
  timer_tick(gb);
  int dots = gb->double_speed ? 2 : 4;
  ppu_tick(gb, dots);
  apu_tick(&gb->apu, dots);
  gb->cycles += dots;
  gb->mcycles++;
  while (gb->cycles >= gb->next_sample_at) {
    if (!gb->joy_latched && gb->halted && (gb->ie & 0x10)) gb->joy = gb_input_now(gb);
    if (gb->sample_count == 16) {
      if (gb->frame_cb) {
        gb->sample = &gb->samples[gb->sample_head];
        gb->sample_head = (gb->sample_head + 1) % 16;
        gb->sample_count--;
        gb->frame_cb(gb, gb->sample, gb->frame_ctx);
      } else gb->sample_overflow++;
    }
    if (gb->sample_count < 16) {
      GBSample *sm = &gb->samples[(gb->sample_head + gb->sample_count) % 16];
      sm->frame = GRID_FRAME(gb->next_sample_at) - 1;
      memcpy(sm->wram, gb->wram, sizeof sm->wram);
      memcpy(sm->hram, gb->hram, sizeof sm->hram);
      memcpy(sm->vram, gb->vram, sizeof sm->vram);
      memcpy(sm->oam, gb->oam, sizeof sm->oam);
      memcpy(sm->io, gb->io, sizeof sm->io);
      memcpy(sm->bg_pal, gb->bg_pal, sizeof sm->bg_pal);
      memcpy(sm->ob_pal, gb->ob_pal, sizeof sm->ob_pal);
      memcpy(sm->framebuffer, gb->framebuffer, sizeof sm->framebuffer);
      const RenderLineRegs *lines = render_line_regs(gb);
      memcpy(sm->line_scx, lines->scx, FB_H);
      memcpy(sm->line_scy, lines->scy, FB_H);
      memcpy(sm->line_lcdc, lines->lcdc, FB_H);
      memcpy(sm->line_wx, lines->wx, FB_H);
      memcpy(sm->line_wy, lines->wy, FB_H);
      memcpy(sm->drawn_vram, lines->vram, sizeof sm->drawn_vram);
      memcpy(sm->drawn_bg_pal, lines->bg_pal, sizeof sm->drawn_bg_pal);
      memcpy(sm->line_drawn, lines->drawn, FB_H);
      memcpy(sm->drawn_oam, lines->oam, sizeof sm->drawn_oam);
      sm->ie = gb->ie; sm->rom_bank = gb->rom_bank; sm->ram_bank = gb->ram_bank;
      sm->joy_latched = gb->joy_latched; sm->joy_read = gb->joy_read;
      gb->sample_count++;
    }
    gb->joy_latched = false; gb->joy_read = false;
    gb->next_sample_at += FRAME_CYCLES;
  }

}

void gb_run_cycles(GB *gb, uint64_t target) {
  while (gb->cycles < target && !gb->hung) gb->step(gb);
}

int64_t gb_grid_offset = GRID_OFFSET;
uint8_t gb_input_now(GB *gb) { return gb->input_at ? gb->input_at(gb->input_ctx, GRID_FRAME(gb->cycles)) : gb->joy; }

uint64_t gb_run_frame(GB *gb) {
  while (gb->sample_count == 0 && !gb->hung) gb->step(gb);
  if (gb->sample_count == 0) return GRID_FRAME(gb->cycles);
  gb->sample = &gb->samples[gb->sample_head];
  gb->sample_head = (gb->sample_head + 1) % 16;
  gb->sample_count--;
  if (gb->frame_cb) gb->frame_cb(gb, gb->sample, gb->frame_ctx);
  return gb->sample->frame;
}

void gb_run_until_vblank(GB *gb) {
  bool lcd_on = gb->io[R_LCDC] & 0x80;
  uint64_t limit = gb->cycles + (lcd_on ? FRAME_CYCLES * 2 : FRAME_CYCLES);
  gb->frame_ready = false;
  while (!gb->frame_ready && gb->cycles < limit && !gb->hung) gb->step(gb);
}

uint64_t gb_state_hash(const GB *gb) {
  const GBSample *sm = gb->sample;
  uint64_t h = FNV1A64_INIT;
  h = fnv1a64_update(h, sm->wram[0] + GB_W0_SKIP_END, 4096 - GB_W0_SKIP_END);
  h = fnv1a64_update(h, sm->wram[1], 4096);
  h = fnv1a64_update(h, sm->wram[GB_SOUND_BANK], GB_SOUND_BANK_LO);
  h = fnv1a64_update(h, (const unsigned char *)sm->wram[3], 5 * 4096);
  h = fnv1a64_update(h, sm->hram, GB_HRAM_MUSIC_LO);
  h = fnv1a64_update(h, sm->hram + GB_HRAM_MUSIC_HI, sizeof sm->hram - GB_HRAM_MUSIC_HI);
  h = fnv1a64_update(h, (const unsigned char *)sm->vram, sizeof sm->vram);
  h = fnv1a64_update(h, sm->oam, sizeof sm->oam);
  h = fnv1a64_update(h, sm->bg_pal, sizeof sm->bg_pal);
  h = fnv1a64_update(h, sm->ob_pal, sizeof sm->ob_pal);
  h = fnv1a64_update(h, sm->io, sizeof sm->io);
  uint8_t extra[] = {sm->ie, (uint8_t)sm->rom_bank, (uint8_t)(sm->rom_bank >> 8), sm->ram_bank};
  return fnv1a64_update(h, extra, sizeof extra);
}

uint64_t gb_frame_hash(const GB *gb) {
  return fnv1a64_update(FNV1A64_INIT, (const unsigned char *)gb->sample->framebuffer, sizeof gb->sample->framebuffer);
}
