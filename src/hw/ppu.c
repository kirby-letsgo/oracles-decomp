#include "core/gb.h"

static void set_mode(GB *gb, uint8_t mode) {
  gb->ppu_mode = mode;
  gb->io[R_STAT] = (gb->io[R_STAT] & 0xfc) | mode;
}

void ppu_update_stat(GB *gb) {
  bool lyc = ppu_ly_read(gb) == gb->io[R_LYC];
  gb->io[R_STAT] = (gb->io[R_STAT] & 0xfb) | (lyc ? 4 : 0);
  uint8_t s = gb->io[R_STAT];
  bool line = (gb->ppu_mode == 0 && (s & 0x08)) || (gb->ppu_mode == 1 && (s & 0x10)) ||
              (gb->ppu_mode == 2 && (s & 0x20)) || (lyc && (s & 0x40));
  if (line && !gb->stat_line) gb->io[R_IF] |= INT_STAT;
  gb->stat_line = line;
}

#include <stdio.h>
#include <stdlib.h>
int dbg_log_lcdc;
int dbg_mode0_dot[154], dbg_mode3_len[154];
void ppu_write_lcdc(GB *gb, uint8_t v) {
  bool was_on = gb->io[R_LCDC] & 0x80;
  if (dbg_log_lcdc && ((v ^ gb->io[R_LCDC]) & 0x80)) printf("%llu LCDC %02x at %llu cyc %llu mode %d ly %u dot %d latched %d\n", (unsigned long long)GRID_FRAME(gb->cycles), v, (unsigned long long)gb->mcycles, (unsigned long long)gb->cycles, gb->ppu_mode, gb->io[R_LY], gb->ppu_dot, gb->joy_latched);
  gb->io[R_LCDC] = v;
  if (was_on && !(v & 0x80)) {
    if (gb->ppu_mode != 1) { gb->joy = gb_input_now(gb); gb->joy_latched = true; if (getenv("JOYLOG")) printf("%llu LATCH lcdoff joy %02x at %llu\n", (unsigned long long)GRID_FRAME(gb->cycles), gb->joy, (unsigned long long)gb->mcycles); }
    gb->io[R_LY] = 0;
    gb->ppu_dot = 0;
    gb->window_line = 0;
    gb->stat_line = false;
    set_mode(gb, 0);
    for (int i = 0; i < FB_W * FB_H; i++) gb->framebuffer[i] = 0x7fff;
  } else if (!was_on && (v & 0x80)) {
    gb->io[R_LY] = 0;
    gb->ppu_dot = 0;
    gb->window_line = 0;
    gb->first_line_after_enable = true;
    set_mode(gb, 0);
    ppu_update_stat(gb);
  }
}

static int mode3_length(const GB *gb) {
  uint8_t lcdc = gb->io[R_LCDC], ly = gb->io[R_LY], scx = gb->io[R_SCX], wx = gb->io[R_WX], wy = gb->io[R_WY];
  int len = 172 + (scx & 7);
  bool win = (lcdc & 0x20) && ly >= wy && wx <= 166;
  if (win) len += 6;
  if (!(lcdc & 0x02) || gb->first_line_after_enable) return len;
  int h = (lcdc & 0x04) ? 16 : 8;
  int xs[10], n = 0, cnt = 0;
  for (int i = 0; i < 40 && cnt < 10; i++) {
    int sy = gb->oam[i * 4] - 16;
    if (ly >= sy && ly < sy + h) { cnt++; if (gb->oam[i * 4 + 1] < 168) xs[n++] = gb->oam[i * 4 + 1]; }
  }
  for (int i = 1; i < n; i++) { int v = xs[i], j = i - 1; while (j >= 0 && xs[j] > v) { xs[j + 1] = xs[j]; j--; } xs[j + 1] = v; }
  int last_tile = -1000;
  for (int i = 0; i < n; i++) {
    int x = xs[i];
    if (x == 0) { len += 11; continue; }
    int col = (win && x - 8 >= wx - 7) ? x - wx - 1 : x + scx;
    int tile = (col >> 3) + ((win && x - 8 >= wx - 7) ? 100000 : 0);
    len += 6;
    if (tile != last_tile) { int pen = 5 - (col & 7); if (pen > 0) len += pen; last_tile = tile; }
  }
  return len;
}

void ppu_tick(GB *gb, int dots) {
  if (!(gb->io[R_LCDC] & 0x80)) return;
  for (int i = 0; i < dots; i++) {
    if (++gb->ppu_dot == (gb->first_line_after_enable ? 452 : 456)) {
      gb->ppu_dot = 0;
      gb->first_line_after_enable = false;
      if (++gb->io[R_LY] == 154) { gb->io[R_LY] = 0; gb->window_line = 0; }
    }
    uint8_t ly = gb->io[R_LY];
    int d = gb->ppu_dot;
    bool first = gb->first_line_after_enable;
    int m2_end = first ? 80 : 82;
    if (ly < 144 && d == m2_end) {
      gb->mode3_end = m2_end + mode3_length(gb);
      static uint64_t pl_a = 0, pl_b = 0; static bool pl_init = false;
      if (!pl_init) { pl_init = true; if (getenv("PPU_LOG")) sscanf(getenv("PPU_LOG"), "%llu-%llu", (unsigned long long *)&pl_a, (unsigned long long *)&pl_b); }
      if (pl_a && gb->mcycles >= pl_a && gb->mcycles <= pl_b) {
        printf("PPU mc %llu LY %u LYC %u SCX %u SCY %u WX %u WY %u LCDC %02x len %d objs:", (unsigned long long)gb->mcycles, ly, gb->io[R_LYC], gb->io[R_SCX], gb->io[R_SCY], gb->io[R_WX], gb->io[R_WY], gb->io[R_LCDC], gb->mode3_end - m2_end);
        int h = (gb->io[R_LCDC] & 0x04) ? 16 : 8, n = 0;
        for (int i = 0; i < 40 && n < 10; i++) { int sy = gb->oam[i * 4] - 16; if (ly >= sy && ly < sy + h) { n++; printf(" %d:x%u", i, gb->oam[i * 4 + 1]); } }
        printf("\n");
      }
    }
    if (!first && d < 2) {
      if (ly == 144 && d == 0) { gb->io[R_IF] |= INT_VBLANK; gb->vblank_if_raised = true; }
      uint8_t s = gb->io[R_STAT];
      bool lyc_new = ppu_ly_read(gb) == gb->io[R_LYC];
      bool line = (gb->ppu_mode == 0 && (s & 0x08)) || (gb->ppu_mode == 1 && (s & 0x10)) ||
                  (gb->ppu_mode == 2 && (s & 0x20)) || (lyc_new && (s & 0x40));
      if (line && !gb->stat_line) gb->io[R_IF] |= INT_STAT;
      gb->stat_line = line;
      continue;
    }
    uint8_t mode = ly >= 144 ? 1 : d < m2_end ? (first ? 0 : 2) : d < gb->mode3_end ? 3 : 0;
    if (mode != gb->ppu_mode) {
      set_mode(gb, mode);
      if (mode == 0 && d >= m2_end) {
        render_scanline(gb);
        if (gb->hdma_active) gb->hdma_chunk_pending = true;
      } else if (mode == 1) {
        if (dbg_log_lcdc && GRID_FRAME(gb->cycles) < 200) printf("%llu VBLANK at %llu cyc %llu (frame start %llu)\n", (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)gb->mcycles, (unsigned long long)gb->cycles, (unsigned long long)GRID_END(GRID_FRAME(gb->cycles)));
        gb->joy = gb_input_now(gb);
        gb->joy_latched = true;
        if (getenv("JOYLOG")) printf("%llu LATCH vblank joy %02x at %llu\n", (unsigned long long)GRID_FRAME(gb->cycles), gb->joy, (unsigned long long)gb->mcycles);
        if (!gb->vblank_if_raised) gb->io[R_IF] |= INT_VBLANK;
        gb->vblank_if_raised = false;
        gb->frame_ready = true;
        gb->frame_count++;
      }
    }
    ppu_update_stat(gb);
  }
}
