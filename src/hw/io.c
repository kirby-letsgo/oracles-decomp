// The IO register file: what the game reads and writes at ff00-ff7f, and the side effects
// that reach the timer, PPU, APU and DMA engines.
#include "core/bus.h"
#include "hw/hw.h"
#include <stdio.h>

int dbg_log_timer_writes;

static uint8_t joypad_read(const GB *gb) {
  uint8_t sel = gb->io[R_P1] & 0x30;
  uint8_t lines = 0x0f;
  if (!(sel & 0x10)) lines &= ~(gb->joy >> 4);
  if (!(sel & 0x20)) lines &= ~(gb->joy & 0x0f);
  return 0xc0 | sel | (lines & 0x0f);
}

uint8_t io_read(GB *gb, uint8_t r) {
  switch (r) {
  case R_P1: gb->joy_read = true; return joypad_read(gb);
  case R_SB: return gb->io[R_SB];
  case R_SC: return gb->io[R_SC] | 0x7c;
  case R_DIV: case R_TIMA: return timer_io_read(gb, r);
  case R_TMA: return gb->io[r];
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

void io_write(GB *gb, uint8_t r, uint8_t v) {
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
  case R_DIV: if (dbg_log_timer_writes) printf("  OURS write DIV at cycle %llu div=%04x\n", (unsigned long long)gb->cycles, gb->div_counter); timer_io_write(gb, r, v); return;
  case R_TIMA: if (dbg_log_timer_writes) printf("  OURS write TIMA=%02x at cycle %llu div=%04x reload=%d\n", v, (unsigned long long)gb->cycles, gb->div_counter, gb->tima_reload);
    timer_io_write(gb, r, v); return;
  case R_TMA: timer_io_write(gb, r, v); return;
  case R_TAC: if (dbg_log_timer_writes) printf("  OURS write TAC=%02x at cycle %llu div=%04x TIMA=%02x\n", v, (unsigned long long)gb->cycles, gb->div_counter, gb->io[R_TIMA]); timer_io_write(gb, r, v); return;
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
