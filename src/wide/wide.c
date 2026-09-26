#include "wide/wide.h"
#include "hw/render.h"
#include <string.h>

// The RAM the renderer reads (the same address in both games unless two are given)
typedef struct { uint16_t tileset_flags, lcd_behaviour, camera_y, camera_x; } GameRam;
static const GameRam ages_ram = {0xcc34, 0xff9b, 0xffaa, 0xffac};
static const GameRam seasons_ram = {0xcc50, 0xff99, 0xffa8, 0xffaa};
enum {
  W_SCROLL_MODE = 0xcd00, W_OPENED_MENU = 0xcbcb, W_OFFSET_Y = 0xcd08, W_OFFSET_X = 0xcd09,
  W_ROOM_WIDTH = 0xcd0a, W_ROOM_HEIGHT = 0xcd0b,
  TILESETFLAG_SIDESCROLL = 0x20, STATUS_BAR_LINES = 16,
};

static uint8_t rd(const GBSample *s, uint16_t a) {
  if (a >= 0xff80) return s->hram[a - 0xff80];
  return s->wram[0][a - 0xc000];
}

// The room is shown around the camera only in normal play: the status bar split on, no menu
// open, the camera following Link (no transition or scripted scene) and not a sidescrolling area.
static bool room_shown(const GBSample *s, const GameRam *r) {
  int split = rd(s, r->lcd_behaviour);                     // 2 or 3: the status bar split
  // scroll mode 1 is normal play (bit 7: the camera is moving); 0 is a scripted scene, bits 1-3 a
  // screen transition
  return (split == 2 || split == 3) && rd(s, W_OPENED_MENU) == 0 && (rd(s, W_SCROLL_MODE) & 0x0f) == 0x01 &&
         !(rd(s, r->tileset_flags) & TILESETFLAG_SIDESCROLL);
}

// A register difference as a small signed offset (the camera plus the screen shake of up to 3)
static int wrap_offset(int v) { v &= 0xff; return v > 200 ? v - 256 : v; }

// The background pixel the PPU would draw at screen (x, y) if the screen were wider: the same scroll
// and map, so VRAM-only changes (text, door animations) show exactly as in the middle. RGB555.
static int bg_pixel(const GBSample *s, int x, int y) {
  uint8_t lcdc = s->line_lcdc[y];
  int d = s->line_drawn[y];
  int px = (x + s->line_scx[y]) & 0xff, py = (y + s->line_scy[y]) & 0xff;
  int idx = ((lcdc & 0x08) ? 0x1c00 : 0x1800) + (py >> 3) * 32 + (px >> 3);
  uint8_t tile = s->drawn_vram[d][0][idx], attr = s->drawn_vram[d][1][idx];
  int fx = px & 7, fy = py & 7;
  if (attr & 0x20) fx = 7 - fx;
  if (attr & 0x40) fy = 7 - fy;
  int base = (lcdc & 0x10) ? tile * 16 : 0x1000 + (int8_t)tile * 16;
  const uint8_t *td = &s->drawn_vram[d][(attr >> 3) & 1][base + fy * 2];
  int bit = 7 - fx, ci = (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1);
  int i = (attr & 7) * 8 + ci * 2;
  return s->drawn_bg_pal[d][i] | (s->drawn_bg_pal[d][i + 1] << 8);
}

static void put555(uint8_t *out, int c, bool dim) {
  int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
  uint8_t rgb[3] = {(uint8_t)((r << 3) | (r >> 2)), (uint8_t)((g << 3) | (g >> 2)), (uint8_t)((b << 3) | (b >> 2))};
  for (int k = 0; k < 3; k++) out[k] = dim ? (uint8_t)(rgb[k] * 3 / 4) : rgb[k];
}

// The window layer covers screen x on line y (it starts at WX - 7 and runs to the right edge)
bool wide_window_at(const GBSample *s, int x, int y) {
  uint8_t lcdc = s->line_lcdc[y];
  return (lcdc & 0x20) && y >= s->line_wy[y] && s->line_wx[y] <= 166 && x + 7 >= s->line_wx[y];
}

int wide_room_pixel(const GBSample *s, bool seasons, int x, int y) {
  const GameRam *r = seasons ? &seasons_ram : &ages_ram;
  if (y < STATUS_BAR_LINES || y >= FB_H || !room_shown(s, r) || (s->line_lcdc[y] & 0x81) != 0x81) return -1;
  if (wide_window_at(s, x < 0 ? 0 : x, y)) return -1;      // a line the window covers shows no room
  // room-local position: outside the room the map holds leftovers of other rooms
  int rx = wrap_offset(s->line_scx[y] - rd(s, W_OFFSET_X)) + x;
  int ry = wrap_offset(s->line_scy[y] - rd(s, W_OFFSET_Y)) + y;
  int width = rd(s, W_ROOM_WIDTH), height = rd(s, W_ROOM_HEIGHT);
  if (rx < 0 || ry < 0 || rx >= width * 8 || ry >= height * 8 || width > 32 || height > 32) return -1;
  return bg_pixel(s, x, y);
}

void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out) {
  static uint8_t middle[FB_W * FB_H * 3];
  framebuffer_to_rgb(s->framebuffer, middle);
  for (int y = 0; y < FB_H; y++) {
    uint8_t *row = out + (size_t)y * WIDE_W * 3;
    for (int x = 0; x < WIDE_W; x++) {
      if (x == WIDE_SIDE) x += FB_W;
      if (x >= WIDE_W) break;
      int c = wide_room_pixel(s, o->seasons, x - WIDE_SIDE, y);
      if (!(s->line_lcdc[y] & 0x80)) c = 0x7fff;            // LCD off: blank white, as in the middle
      if (c < 0) memcpy(row + x * 3, o->border, 3);
      else put555(row + x * 3, c, o->dim);
    }
    memcpy(row + WIDE_SIDE * 3, middle + (size_t)y * FB_W * 3, FB_W * 3);
  }
}
