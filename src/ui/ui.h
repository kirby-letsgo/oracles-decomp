#pragma once
// What the game-pixel side of the frontend reads from the player's ROM, so the app ships no game
// art: each game's title-screen colours (the frame around a widescreen picture) and the dialogue
// font (mods' overlay text, src/mods/overlay.c).
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UI_W 160
#define UI_H 144
#define UI_W_16_10 230  // nearest even width at the original 144 lines
#define UI_GLYPH_W 8
#define UI_GLYPH_H 16

typedef struct { uint8_t r, g, b; } UiColor;
typedef struct { uint8_t glyph[128][UI_GLYPH_H]; bool loaded; } UiFont;

// Characters of the ROM font outside ASCII
enum { UI_CH_HEART = 0x14, UI_CH_CURSOR = 0x1b };

// A game's colours, taken from its title screen's background palettes (PALH_03, paletteData4018:
// bank $16 in Seasons, $17 in Ages). The menus have their own (gui_theme); bg is widescreen's frame.
typedef struct { UiColor bg, panel, border, text, dim, highlight, accent; } UiTheme;
void ui_theme_default(UiTheme *t, bool ages);
bool ui_theme_load(UiTheme *t, const uint8_t *rom, size_t rom_size, bool ages);

// The dialogue font (1bpp, 8x16 per character, ASCII from $20, a clear bit is ink) at 1c:4720 in both games.
bool ui_font_load(UiFont *font, const uint8_t *rom, size_t rom_size);
// A built-in font for when there is no ROM to read it from.
void ui_font_builtin(UiFont *font);
