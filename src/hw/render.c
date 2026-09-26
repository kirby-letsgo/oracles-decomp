#include "hw/render.h"
#include "core/gb.h"
#include <string.h>

static inline uint16_t pal_color(const uint8_t *pal, int p, int ci) {
  int i = p * 8 + ci * 2;
  return pal[i] | ((uint16_t)pal[i + 1] << 8);
}

static inline int tile_pixel(const uint8_t *td, int bit) {
  return (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1);
}

RenderLineRegs *render_line_regs(const GB *gb) {
  static struct { const GB *gb; RenderLineRegs regs; } slots[8];
  static int next;
  for (int i = 0; i < 8; i++)
    if (slots[i].gb == gb) return &slots[i].regs;
  int i = next;
  next = (next + 1) % 8;
  memset(&slots[i], 0, sizeof slots[i]);
  slots[i].gb = gb;
  return &slots[i].regs;
}

void render_lines_blank(const GB *gb) { memset(render_line_regs(gb)->lcdc, 0, FB_H); }

void render_scanline(GB *gb) {
  int ly = gb->io[R_LY];
  if (ly >= FB_H) return;
  uint8_t lcdc = gb->io[R_LCDC];
  RenderLineRegs *lines = render_line_regs(gb);
  lines->scx[ly] = gb->io[R_SCX];
  lines->scy[ly] = gb->io[R_SCY];
  lines->lcdc[ly] = lcdc;
  lines->wx[ly] = gb->io[R_WX];
  lines->wy[ly] = gb->io[R_WY];
  if (ly == 0) {
    lines->current ^= 1;
    memcpy(lines->vram[lines->current], gb->vram, sizeof lines->vram[0]);
    memcpy(lines->bg_pal[lines->current], gb->bg_pal, sizeof lines->bg_pal[0]);
    memcpy(lines->oam[lines->current], gb->oam, sizeof lines->oam[0]);
  }
  lines->drawn[ly] = lines->current;
  uint16_t *row = &gb->framebuffer[ly * FB_W];
  uint8_t bg_ci[FB_W], bg_prio[FB_W];
  uint8_t scx = gb->io[R_SCX], scy = gb->io[R_SCY], wx = gb->io[R_WX], wy = gb->io[R_WY];
  bool win_enabled = (lcdc & 0x20) && ly >= wy && wx <= 166;
  bool win_used = false;

  for (int x = 0; x < FB_W; x++) {
    uint16_t map;
    int tx, ty, fx, fy;
    if (win_enabled && x + 7 >= wx) {
      int wxp = x + 7 - wx;
      map = (lcdc & 0x40) ? 0x1c00 : 0x1800;
      tx = wxp >> 3; fx = wxp & 7;
      ty = gb->window_line >> 3; fy = gb->window_line & 7;
      win_used = true;
    } else {
      int px = (x + scx) & 0xff, py = (ly + scy) & 0xff;
      map = (lcdc & 0x08) ? 0x1c00 : 0x1800;
      tx = px >> 3; fx = px & 7; ty = py >> 3; fy = py & 7;
    }
    int idx = map + ty * 32 + tx;
    uint8_t tile = gb->vram[0][idx], attr = gb->vram[1][idx];
    int addr = (lcdc & 0x10) ? tile * 16 : 0x1000 + (int8_t)tile * 16;
    if (attr & 0x40) fy = 7 - fy;
    const uint8_t *td = &gb->vram[(attr >> 3) & 1][addr + fy * 2];
    int ci = tile_pixel(td, (attr & 0x20) ? fx : 7 - fx);
    row[x] = pal_color(gb->bg_pal, attr & 7, ci);
    bg_ci[x] = ci;
    bg_prio[x] = attr >> 7;
  }
  if (win_used) gb->window_line++;
  if (!(lcdc & 0x02)) return;

  int h = (lcdc & 0x04) ? 16 : 8;
  int sprites[10], n = 0;
  for (int i = 0; i < 40 && n < 10; i++) {
    int sy = gb->oam[i * 4] - 16;
    if (ly >= sy && ly < sy + h) sprites[n++] = i;
  }
  bool drawn[FB_W];
  memset(drawn, 0, sizeof drawn);
  bool bg_master = lcdc & 0x01;
  for (int s = 0; s < n; s++) {
    const uint8_t *o = &gb->oam[sprites[s] * 4];
    int sy = o[0] - 16, sx = o[1] - 8;
    uint8_t tile = o[2], attr = o[3];
    int line = ly - sy;
    if (attr & 0x40) line = h - 1 - line;
    if (h == 16) tile &= 0xfe;
    const uint8_t *td = &gb->vram[(attr >> 3) & 1][tile * 16 + line * 2];
    for (int px = 0; px < 8; px++) {
      int x = sx + px;
      if (x < 0 || x >= FB_W || drawn[x]) continue;
      int ci = tile_pixel(td, (attr & 0x20) ? px : 7 - px);
      if (ci == 0) continue;
      drawn[x] = true;
      bool bg_wins = bg_master && bg_ci[x] != 0 && (bg_prio[x] || (attr & 0x80));
      if (bg_wins) continue;
      row[x] = pal_color(gb->ob_pal, attr & 7, ci);
    }
  }
}

void framebuffer_to_rgb(const uint16_t *fb, uint8_t *rgb) {
  for (int i = 0; i < FB_W * FB_H; i++) {
    uint16_t c = fb[i];
    uint8_t r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    rgb[i * 3] = (r << 3) | (r >> 2);
    rgb[i * 3 + 1] = (g << 3) | (g >> 2);
    rgb[i * 3 + 2] = (b << 3) | (b >> 2);
  }
}
