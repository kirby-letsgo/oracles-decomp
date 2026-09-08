#include "bus.h"
#include <stdio.h>
#include <string.h>
int dbg_log_timer_writes, dbg_log_dma;

static uint8_t io_read(GB *gb, uint8_t r);
static void io_write(GB *gb, uint8_t r, uint8_t v);

static inline uint8_t wram_bank(const GB *gb) {
  uint8_t b = gb->io[R_SVBK] & 7;
  return b ? b : 1;
}

uint8_t bus_read(GB *gb, uint16_t a) {
  if (a < 0x8000) {
    if (gb->boot_mapped && (a < 0x100 || (a >= 0x200 && a < 0x900)) && a < gb->boot_size) return gb->boot[a];
    if (a < 0x4000) return gb->rom[a];
    uint32_t banks = gb->rom_size / 0x4000;
    return gb->rom[((gb->rom_bank % banks) << 14) | (a & 0x3fff)];
  }
  if (a < 0xa000) return gb->vram[gb->io[R_VBK] & 1][a - 0x8000];
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

void bus_write(GB *gb, uint16_t a, uint8_t v) {
  if (a < 0x8000) { mbc_write(gb, a, v); return; }
  if (a < 0xa000) { gb->vram[gb->io[R_VBK] & 1][a - 0x8000] = v; return; }
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

static uint8_t joypad_read(const GB *gb) {
  uint8_t sel = gb->io[R_P1] & 0x30;
  uint8_t lines = 0x0f;
  if (!(sel & 0x10)) lines &= ~(gb->joy >> 4);
  if (!(sel & 0x20)) lines &= ~(gb->joy & 0x0f);
  return 0xc0 | sel | (lines & 0x0f);
}

static uint8_t io_read(GB *gb, uint8_t r) {
  switch (r) {
  case R_P1: gb->joy_read = true; return joypad_read(gb);
  case R_SB: return gb->io[R_SB];
  case R_SC: return gb->io[R_SC] | 0x7c;
  case R_DIV: return gb->div_counter >> 8;
  case R_TIMA: case R_TMA: return gb->io[r];
  case R_TAC: return gb->io[R_TAC] | 0xf8;
  case R_IF: return gb->io[R_IF] | 0xe0;
  case R_LY: return ppu_ly_read(gb);
  case R_LCDC: case R_SCY: case R_SCX: case R_LYC: case R_DMA:
  case R_BGP: case R_OBP0: case R_OBP1: case R_WY: case R_WX: return gb->io[r];
  case R_STAT: return gb->io[R_STAT] | 0x80;
  case R_KEY1: return (gb->double_speed ? 0x80 : 0) | (gb->speed_armed ? 1 : 0) | 0x7e;
  case R_VBK: return 0xfe | (gb->io[R_VBK] & 1);
  case R_HDMA1: case R_HDMA2: case R_HDMA3: case R_HDMA4: return 0xff;
  case R_HDMA5: return (gb->hdma_active ? 0 : 0x80) | ((gb->hdma_len - 1) & 0x7f);
  case R_RP: return 0x3e | (gb->io[R_RP] & 0xc1);
  case R_BCPS: return gb->io[R_BCPS] | 0x40;
  case R_BCPD: return gb->bg_pal[gb->io[R_BCPS] & 0x3f];
  case R_OCPS: return gb->io[R_OCPS] | 0x40;
  case R_OCPD: return gb->ob_pal[gb->io[R_OCPS] & 0x3f];
  case R_OPRI: return gb->io[R_OPRI] | 0xfe;
  case R_SVBK: return 0xf8 | (gb->io[R_SVBK] & 7);
  default:
    if (r >= 0x10 && r <= 0x3f) return apu_read(&gb->apu, r);
    return 0xff;
  }
}

static void oam_dma(GB *gb, uint8_t page) {
  if (dbg_log_dma) printf("  OURS OAM DMA write %02x at %llu mc %llu\n", page, (unsigned long long)((gb->cycles % FRAME_CYCLES) / 2), (unsigned long long)gb->mcycles);
  uint16_t src = (uint16_t)page << 8;
  for (int i = 0; i < 160; i++) gb->oam[i] = bus_read(gb, src + i);
}

uint64_t dbg_dma_bytes, dbg_dma_calls, dbg_hblank_chunks;
static void hdma_copy(GB *gb, int bytes) {
  dbg_dma_bytes += bytes; dbg_dma_calls++;
  for (int i = 0; i < bytes; i++) {
    gb->vram[gb->io[R_VBK] & 1][(gb->hdma_dst + i) & 0x1fff] = bus_read(gb, gb->hdma_src + i);
  }
  gb->hdma_src += bytes;
  gb->hdma_dst += bytes;
}

void bus_hdma_chunk(GB *gb) {
  if (!gb->hdma_active) return;
  dbg_hblank_chunks++;
  hdma_copy(gb, 16);
  int stall = gb->double_speed ? 16 : 8;
  for (int i = 0; i < stall; i++) gb_tick(gb);
  if (--gb->hdma_len == 0) gb->hdma_active = false;
}

uint64_t dbg_hdma5_writes, dbg_hdma5_hblank_lcdoff, dbg_hdma5_hblank;
static void hdma5_write(GB *gb, uint8_t v) {
  if (dbg_log_dma) printf("  OURS HDMA5 write %02x at %llu mc %llu (src %04x dst %04x)\n", v, (unsigned long long)((gb->cycles % FRAME_CYCLES) / 2), (unsigned long long)gb->mcycles, (gb->io[R_HDMA1] << 8) | gb->io[R_HDMA2], (gb->io[R_HDMA3] << 8) | gb->io[R_HDMA4]);
  dbg_hdma5_writes++; if (v & 0x80) { dbg_hdma5_hblank++; if (!(gb->io[R_LCDC] & 0x80)) dbg_hdma5_hblank_lcdoff++; }
  if (gb->hdma_active && !(v & 0x80)) { gb->hdma_active = false; gb->hdma_len = (v & 0x7f) + 1; return; }
  gb->hdma_src = ((gb->io[R_HDMA1] << 8) | gb->io[R_HDMA2]) & 0xfff0;
  gb->hdma_dst = (((gb->io[R_HDMA3] << 8) | gb->io[R_HDMA4]) & 0x1ff0);
  gb->hdma_len = (v & 0x7f) + 1;
  if (v & 0x80) { gb->hdma_active = true; return; }
  int bytes = gb->hdma_len * 16;
  hdma_copy(gb, bytes);
  int stall = gb->hdma_len * (gb->double_speed ? 16 : 8) + 1;
  for (int i = 0; i < stall; i++) gb_tick(gb);
  gb->hdma_len = 0;
  gb->hdma_active = false;
}

static void io_write(GB *gb, uint8_t r, uint8_t v) {
  switch (r) {
  case R_P1: gb->io[R_P1] = v & 0x30; return;
  case R_SB: gb->io[R_SB] = v; return;
  case R_SC:
    gb->io[R_SC] = v & 0x83;
    if ((v & 0x81) == 0x81) {
      if (gb->serial_out) gb->serial_out(gb->serial_ctx, gb->io[R_SB]);
      gb->io[R_SB] = 0xff;
      gb->io[R_SC] &= 0x7f;
      gb->io[R_IF] |= INT_SERIAL;
    }
    return;
  case R_DIV: if (dbg_log_timer_writes) printf("  OURS write DIV at cycle %llu div=%04x\n", (unsigned long long)gb->cycles, gb->div_counter); timer_write_div(gb); return;
  case R_TIMA: if (dbg_log_timer_writes) printf("  OURS write TIMA=%02x at cycle %llu div=%04x reload=%d\n", v, (unsigned long long)gb->cycles, gb->div_counter, gb->tima_reload);
    if (gb->tima_reload != 2) { gb->io[R_TIMA] = v; gb->tima_reload = 0; } return;
  case R_TMA: gb->io[R_TMA] = v; if (gb->tima_reload == 2) gb->io[R_TIMA] = v; return;
  case R_TAC: if (dbg_log_timer_writes) printf("  OURS write TAC=%02x at cycle %llu div=%04x TIMA=%02x\n", v, (unsigned long long)gb->cycles, gb->div_counter, gb->io[R_TIMA]); timer_write_tac(gb, v); return;
  case R_IF: gb->io[R_IF] = v & 0x1f; return;
  case R_LCDC: ppu_write_lcdc(gb, v); return;
  case R_STAT: gb->io[R_STAT] = (gb->io[R_STAT] & 0x07) | (v & 0x78); ppu_update_stat(gb); return;
  case R_LY: return;
  case R_LYC: gb->io[R_LYC] = v; ppu_update_stat(gb); return;
  case R_DMA: gb->io[R_DMA] = v; oam_dma(gb, v); return;
  case R_KEY1: gb->speed_armed = v & 1; return;
  case R_VBK: gb->io[R_VBK] = v & 1; return;
  case R_BOOT: if (v & 1) gb->boot_mapped = false; return;
  case R_HDMA1: case R_HDMA2: case R_HDMA3: case R_HDMA4: gb->io[r] = v; return;
  case R_HDMA5: hdma5_write(gb, v); return;
  case R_BCPS: gb->io[R_BCPS] = v & 0xbf; return;
  case R_BCPD: {
    uint8_t i = gb->io[R_BCPS] & 0x3f;
    gb->bg_pal[i] = v;
    if (gb->io[R_BCPS] & 0x80) gb->io[R_BCPS] = 0x80 | ((i + 1) & 0x3f);
    return;
  }
  case R_OCPS: gb->io[R_OCPS] = v & 0xbf; return;
  case R_OCPD: {
    uint8_t i = gb->io[R_OCPS] & 0x3f;
    gb->ob_pal[i] = v;
    if (gb->io[R_OCPS] & 0x80) gb->io[R_OCPS] = 0x80 | ((i + 1) & 0x3f);
    return;
  }
  case R_SVBK: gb->io[R_SVBK] = v & 7; return;
  default:
    if (r >= 0x10 && r <= 0x3f) { apu_write(&gb->apu, r, v); return; }
    gb->io[r] = v;
    return;
  }
}
