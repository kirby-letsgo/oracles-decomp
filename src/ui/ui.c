#include "ui/ui.h"
#include <string.h>

const UiColor UI_BLACK = {0, 0, 0}, UI_WHITE = {248, 248, 248}, UI_GREY = {136, 136, 152},
              UI_GOLD = {248, 208, 64}, UI_RED = {248, 56, 56}, UI_TEAL = {64, 200, 184},
              UI_PANEL = {16, 32, 72}, UI_SHADE = {40, 64, 120};

// title-screen palettes: Seasons gold/red/brown on cream, Ages cyan/purple/orange on ice
static const UiTheme seasons_theme = {{24, 12, 0}, {90, 65, 0}, {213, 180, 0}, {255, 255, 238}, {197, 115, 0}, {213, 180, 0}, {164, 0, 0}};
static const UiTheme ages_theme = {{16, 0, 32}, {90, 0, 139}, {98, 197, 230}, {230, 246, 255}, {115, 115, 148}, {98, 197, 230}, {246, 106, 0}};

void ui_theme_default(UiTheme *t, bool ages) { *t = ages ? ages_theme : seasons_theme; }

static UiColor gbc_color(const uint8_t *p) {
  unsigned v = p[0] | (p[1] << 8);
  return (UiColor){(uint8_t)((v & 31) * 255 / 31), (uint8_t)(((v >> 5) & 31) * 255 / 31), (uint8_t)(((v >> 10) & 31) * 255 / 31)};
}

static UiColor darken(UiColor c, int num, int den) { return (UiColor){(uint8_t)(c.r * num / den), (uint8_t)(c.g * num / den), (uint8_t)(c.b * num / den)}; }

bool ui_theme_load(UiTheme *t, const uint8_t *rom, size_t rom_size, bool ages) {
  ui_theme_default(t, ages);
  size_t off = (size_t)(ages ? 0x17 : 0x16) * 0x4000 + (0x4018 - 0x4000);
  if (!rom || rom_size < off + 64) return false;
  const uint8_t *pal = rom + off;                        // 8 palettes of 4 colours, 2 bytes each
  if (ages) {
    t->panel = gbc_color(pal + 0 * 8 + 3 * 2);           // bg0 colour 3: deep purple
    t->border = t->highlight = gbc_color(pal + 0 * 8 + 2 * 2);   // bg0 colour 2: cyan
    t->text = gbc_color(pal + 0 * 8 + 1 * 2);            // bg0 colour 1: ice white
    t->dim = gbc_color(pal + 4 * 8 + 0 * 2);             // bg4 colour 0: grey-blue
    t->accent = gbc_color(pal + 1 * 8 + 3 * 2);          // bg1 colour 3: orange
  } else {
    t->panel = gbc_color(pal + 2 * 8 + 3 * 2);           // bg2 colour 3: brown
    t->border = t->highlight = gbc_color(pal + 0 * 8 + 2 * 2);   // bg0 colour 2: gold
    t->text = gbc_color(pal + 0 * 8 + 1 * 2);            // bg0 colour 1: cream
    t->dim = gbc_color(pal + 4 * 8 + 0 * 2);             // bg4 colour 0: amber
    t->accent = gbc_color(pal + 0 * 8 + 3 * 2);          // bg0 colour 3: dark red
  }
  t->bg = darken(t->panel, 1, 4);
  return true;
}

#define FONT_BANK 0x1c
#define FONT_ADDR 0x4720

bool ui_font_load(UiFont *font, const uint8_t *rom, size_t rom_size) {
  size_t off = (size_t)FONT_BANK * 0x4000 + (FONT_ADDR - 0x4000);
  font->loaded = false;
  if (!rom || rom_size < off + sizeof font->glyph) return false;
  memcpy(font->glyph, rom + off, sizeof font->glyph);
  font->loaded = true;
  return true;
}

static void put(UiCanvas *c, int x, int y, UiColor color) {
  if (x < 0 || y < 0 || x >= UI_W || y >= UI_H) return;
  c->px[y][x][0] = color.r;
  c->px[y][x][1] = color.g;
  c->px[y][x][2] = color.b;
}

void ui_clear(UiCanvas *c, UiColor color) { ui_fill(c, 0, 0, UI_W, UI_H, color); }

void ui_fill(UiCanvas *c, int x, int y, int w, int h, UiColor color) {
  for (int j = y; j < y + h; j++)
    for (int i = x; i < x + w; i++) put(c, i, j, color);
}

void ui_box(UiCanvas *c, int x, int y, int w, int h, UiColor border, UiColor fill) {
  ui_fill(c, x, y, w, h, border);
  ui_fill(c, x + 1, y + 1, w - 2, h - 2, fill);
}

void ui_glyph(UiCanvas *c, const UiFont *font, int x, int y, uint8_t ch, UiColor color) {
  if (!font->loaded || ch >= 128) return;
  for (int r = 0; r < UI_GLYPH_H; r++)
    for (int b = 0; b < UI_GLYPH_W; b++)
      if (!(font->glyph[ch][r] & (0x80 >> b))) put(c, x + b, y + r, color);
}

bool ui_text_ink(const UiFont *font, const char *s, int *top, int *bottom) {
  *top = UI_GLYPH_H;
  *bottom = -1;
  if (!font->loaded) return false;
  for (; *s; s++)
    for (int r = 0; r < UI_GLYPH_H; r++)
      if ((uint8_t)*s < 128 && font->glyph[(uint8_t)*s][r] != 0xff) {
        if (r < *top) *top = r;
        if (r > *bottom) *bottom = r;
      }
  return *bottom >= 0;
}

int ui_text(UiCanvas *c, const UiFont *font, int x, int y, const char *s, UiColor color) {
  int x0 = x;
  for (; *s; s++, x += UI_GLYPH_W) ui_glyph(c, font, x, y, (uint8_t)*s, color);
  return x - x0;
}

int ui_text_width(const char *s) { return (int)strlen(s) * UI_GLYPH_W; }

void ui_to_rgb(const UiCanvas *c, uint8_t *rgb) { memcpy(rgb, c->px, sizeof c->px); }
