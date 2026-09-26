#pragma once
// The frontend's pixel canvas: menus are drawn on the game's own 160x144 grid, with the font and
// symbols read from the player's ROM, so they scale exactly like the game and ship no game art.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UI_W 160
#define UI_H 144
#define UI_GLYPH_W 8
#define UI_GLYPH_H 16

typedef struct { uint8_t r, g, b; } UiColor;
typedef struct { uint8_t px[UI_H][UI_W][3]; } UiCanvas;
typedef struct { uint8_t glyph[128][UI_GLYPH_H]; bool loaded; } UiFont;

// Characters of the ROM font outside ASCII
enum { UI_CH_HEART = 0x14, UI_CH_CURSOR = 0x1b };

extern const UiColor UI_BLACK, UI_WHITE, UI_GREY, UI_GOLD, UI_RED, UI_TEAL, UI_PANEL, UI_SHADE;

// A game's menu colours, taken from its title screen's background palettes (PALH_03, paletteData4018:
// bank $16 in Seasons, $17 in Ages), so each game's menus carry its own look.
typedef struct { UiColor bg, panel, border, text, dim, highlight, accent; } UiTheme;
void ui_theme_default(UiTheme *t, bool ages);
bool ui_theme_load(UiTheme *t, const uint8_t *rom, size_t rom_size, bool ages);

// The dialogue font (1bpp, 8x16 per character, ASCII from $20, a clear bit is ink) at 1c:4720 in both games.
bool ui_font_load(UiFont *font, const uint8_t *rom, size_t rom_size);

void ui_clear(UiCanvas *c, UiColor color);
void ui_fill(UiCanvas *c, int x, int y, int w, int h, UiColor color);
// A text-box style panel: one-pixel border around a filled interior.
void ui_box(UiCanvas *c, int x, int y, int w, int h, UiColor border, UiColor fill);
void ui_glyph(UiCanvas *c, const UiFont *font, int x, int y, uint8_t ch, UiColor color);
// Draws text at pixel (x, y); returns the width in pixels. Characters outside the font are skipped.
int ui_text(UiCanvas *c, const UiFont *font, int x, int y, const char *s, UiColor color);
int ui_text_width(const char *s);
// Copies the canvas into a packed RGB24 buffer (UI_W * UI_H * 3 bytes).
void ui_to_rgb(const UiCanvas *c, uint8_t *rgb);
