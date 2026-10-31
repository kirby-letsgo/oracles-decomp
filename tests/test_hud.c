// The status bar's B and A repainted in other glyphs: on a made-up sample with the game's letter
// tiles where the bar puts them, plain and mirrored, and with other art in those tiles.
#include "unit.h"
#include "core/gb.h"
#include "hw/render.h"
#include "wide/hud.h"
#include "wide/wide.h"
#include "ui/labels.h"

// hud.c has the same bytes; this copy is deliberately separate, so a typo in one is caught by the other
static const uint8_t tile_b[16] = {0x00, 0xff, 0x73, 0xff, 0x4a, 0xff, 0x4a, 0xff, 0x72, 0xff, 0x4a, 0xff, 0x4a, 0xff, 0x72, 0xff};
static const uint8_t tile_a[16] = {0x00, 0xff, 0x33, 0xff, 0x4a, 0xff, 0x4a, 0xff, 0x7a, 0xff, 0x4a, 0xff, 0x4a, 0xff, 0x4a, 0xff};
static GBSample s;
static uint8_t rgb[FB_W * FB_H * 3], before[FB_W * FB_H * 3];
// palette 0 colour 2 (paper) is 0x4210, colour 3 (ink) black; the picture itself is white, so a
// pixel repainted reads as one or the other and one left alone stays white
enum { PAPER = (16 << 3) | (16 >> 2), INK = 0, WHITE = 0xff };

// The bar as the game draws it: map $9C00 rows 30-31, SCY 240, B at column 0, A at column col_a
static void make_bar(int col_a, uint8_t attr_a) {
  memset(&s, 0, sizeof s);
  for (int y = 0; y < 16; y++) { s.line_lcdc[y] = 0xef; s.line_scy[y] = 240; s.line_wy[y] = 144; s.line_wx[y] = 7; }
  uint8_t *v0 = s.drawn_vram[0][0], *v1 = s.drawn_vram[0][1];
  memcpy(v0 + 0x1050, tile_b, 16);
  memcpy(v0 + 0x1080, tile_a, 16);
  v0[0x1c00 + 30 * 32 + 0] = 0x05; v1[0x1c00 + 30 * 32 + 0] = 0x80;
  v0[0x1c00 + 30 * 32 + col_a] = 0x08; v1[0x1c00 + 30 * 32 + col_a] = attr_a;
  s.drawn_bg_pal[0][4] = 0x10; s.drawn_bg_pal[0][5] = 0x42;
  for (int i = 0; i < FB_W * FB_H; i++) s.framebuffer[i] = 0x7fff;
  framebuffer_to_rgb(s.framebuffer, rgb);
}

static int px(int x, int y) { return rgb[((size_t)y * FB_W + x) * 3]; }
static bool same(int x, int y) { size_t i = ((size_t)y * FB_W + x) * 3; return !memcmp(rgb + i, before + i, 3); }

// cell x0..x0+5 on lines 0-7 holds glyph g in paper and ink (red channel): row 0 paper, rows 1-7
// the glyph; columns off the left edge are skipped
static void assert_glyph_in(int x0, const uint8_t *g, int paper, int ink) {
  for (int y = 0; y < 8; y++)
    for (int c = 0; c < 6; c++)
      if (x0 + c >= 0) ASSERT_EQ(px(x0 + c, y), y && (g[y - 1] >> (7 - c)) & 1 ? ink : paper);
}
static void assert_glyph(int x0, const uint8_t *g) { assert_glyph_in(x0, g, PAPER, INK); }

static void glyphs_exist_for_every_code(void) {
  for (int c = 'A'; c <= 'Z'; c++) ASSERT(hud_glyph((uint8_t)c));
  for (int c = '0'; c <= '9'; c++) ASSERT(hud_glyph((uint8_t)c));
  for (int c = HUD_GLYPH_CROSS; c <= HUD_GLYPH_TRIANGLE; c++) ASSERT(hud_glyph((uint8_t)c));
  ASSERT(!hud_glyph(0));
  ASSERT(!hud_glyph('?'));
}

// Every glyph keeps out of x 6-7, where the bracket shares the cell
static void glyphs_leave_the_bracket_alone(void) {
  for (int c = 1; c < 128; c++) {
    const uint8_t *g = hud_glyph((uint8_t)c);
    for (int r = 0; g && r < HUD_GLYPH_H; r++) ASSERT_EQ(g[r] & 0x03, 0);
  }
}

// The game's own B and A are its tile rows 1-7 without the bracket
static void own_letters_match_the_tiles(void) {
  for (int r = 0; r < HUD_GLYPH_H; r++) {
    ASSERT_EQ(hud_glyph('B')[r], tile_b[2 + r * 2] & 0xf8);
    ASSERT_EQ(hud_glyph('A')[r], tile_a[2 + r * 2] & 0xf8);
  }
}

static void letters_repainted_in_place(void) {
  make_bar(5, 0x80);
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'B', 'A', rgb);                      // the game's own: nothing to do
  hud_relabel(&s, 0, 0, rgb);
  ASSERT(!memcmp(before, rgb, sizeof rgb));
  hud_relabel(&s, 'X', HUD_GLYPH_CROSS, rgb);
  assert_glyph(0, hud_glyph('X'));
  assert_glyph(40, hud_glyph(HUD_GLYPH_CROSS));
  for (int y = 0; y < 16; y++)
    for (int x = 0; x < FB_W; x++)
      if (y >= 8 || (x >= 6 && x < 40) || x >= 46) ASSERT(same(x, y));  // brackets and the rest kept
}

static void one_letter_alone(void) {
  make_bar(5, 0x80);
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'Y', 'A', rgb);
  assert_glyph(0, hud_glyph('Y'));
  for (int y = 0; y < 8; y++) ASSERT(same(41, y));
}

static void mirrored_a_keeps_its_glyph_the_right_way_round(void) {
  make_bar(8, 0xa0);                                   // > 13 hearts: A at column 8, flipped
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 0, 'Y', rgb);
  assert_glyph(64 + 2, hud_glyph('Y'));
  for (int y = 0; y < 8; y++) ASSERT(same(64, y) && same(65, y));  // the bracket, at x 0-1 now
  for (int y = 0; y < 8; y++) ASSERT(same(1, y));                  // B left as it was
}

static void other_art_in_the_tiles_is_left_alone(void) {
  make_bar(5, 0x80);
  s.drawn_vram[0][0][0x1052] ^= 0xff;                  // a title screen's art, not the letter
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'X', 'Y', rgb);
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) ASSERT(same(x, y));    // B cell unchanged
  assert_glyph(40, hud_glyph('Y'));                    // A's tile is still the letter
}

static void off_the_bar_is_left_alone(void) {
  make_bar(5, 0x80);
  for (int y = 0; y < 16; y++) s.line_lcdc[y] = 0xff;  // unsigned tile data: not the bar
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'X', 'Y', rgb);
  ASSERT(!memcmp(before, rgb, sizeof rgb));
  make_bar(5, 0x80);
  for (int y = 0; y < 16; y++) { s.line_wy[y] = 0; s.line_wx[y] = 7; }  // the window over it
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'X', 'Y', rgb);
  ASSERT(!memcmp(before, rgb, sizeof rgb));
}

static void scrolled_bar_shifts_the_cells(void) {
  make_bar(5, 0x80);
  for (int y = 0; y < 16; y++) s.line_scx[y] = 3;
  memcpy(before, rgb, sizeof rgb);
  hud_relabel(&s, 'X', HUD_GLYPH_CROSS, rgb);
  assert_glyph(-3, hud_glyph('X'));                    // B's cell clipped at the left edge
  assert_glyph(37, hud_glyph(HUD_GLYPH_CROSS));
  for (int y = 0; y < 8; y++) ASSERT(same(3, y) && same(4, y) && same(36, y) && same(43, y));
}

// The PPU drew these lines from the second copy (line_drawn 1): the first holds nothing
static void second_drawn_copy_is_read(void) {
  make_bar(5, 0x80);
  memcpy(s.drawn_vram[1], s.drawn_vram[0], sizeof s.drawn_vram[1]);
  memcpy(s.drawn_bg_pal[1], s.drawn_bg_pal[0], sizeof s.drawn_bg_pal[1]);
  memset(s.drawn_vram[0], 0, sizeof s.drawn_vram[0]);
  memset(s.drawn_bg_pal[0], 0, sizeof s.drawn_bg_pal[0]);
  for (int y = 0; y < 16; y++) s.line_drawn[y] = 1;
  hud_relabel(&s, 'X', 'Y', rgb);
  assert_glyph(0, hud_glyph('X'));
  assert_glyph(40, hud_glyph('Y'));
}

static void cell_palette_is_its_own(void) {
  make_bar(5, 0x83);                                   // A in BG palette 3
  uint8_t *pal = s.drawn_bg_pal[0] + 3 * 8;
  pal[4] = 0x00; pal[5] = 0x7c;                        // paper blue: red 0
  pal[6] = 0x1f; pal[7] = 0x00;                        // ink red: red 255
  hud_relabel(&s, 'X', 'Y', rgb);
  assert_glyph(0, hud_glyph('X'));
  assert_glyph_in(40, hud_glyph('Y'), 0, 0xff);
}

static void bank1_and_upside_down_cells_are_left_alone(void) {
  static const uint8_t attrs[] = {0x88, 0xc0};          // tile from VRAM bank 1; flipped vertically
  for (size_t i = 0; i < sizeof attrs; i++) {
    make_bar(5, attrs[i]);
    memcpy(before, rgb, sizeof rgb);
    hud_relabel(&s, 0, 'Y', rgb);
    ASSERT(!memcmp(before, rgb, sizeof rgb));
  }
}

// Item X/Y beside the narrow picture: the letter above where the icon goes, on the bar's colour, the
// rest of the strip black; nothing while the bar is down or the slots are off
static void slot_strip_draws_its_letter_on_a_tab(void) {
  static uint8_t strip[WIDE_STRIP_W * FB_H * 3];
  memset(&s, 0, sizeof s);
  s.hram[0x9b - 0x80] = 2;                             // Ages wDisplayedLcdBehaviour: the bar is split
  s.wram[0][0xd00] = 1;                                // wScrollMode: normal play
  for (int y = 0; y < 16; y++) s.framebuffer[y * FB_W] = 0x7fff;  // the bar's paper, white
  WideOptions o = {false, false, {0, 0, 0}, NULL, 0, true, {0, 0}, NULL, {'Z', 0, 0, 0}};
  memset(strip, 0x55, sizeof strip);
  ASSERT(wide_slot_strip(&s, &o, 0, false, strip));
  const uint8_t *g = hud_glyph('Z');
  for (int y = 0; y < 26; y++)
    for (int x = 0; x < WIDE_STRIP_W; x++) {
      bool ink = y >= 1 && y <= 7 && x >= 5 && x < 11 && (g[y - 1] >> (7 - (x - 5))) & 1;
      ASSERT_EQ(strip[(y * WIDE_STRIP_W + x) * 3], ink ? 0 : 0xff);
    }
  for (int i = 26 * WIDE_STRIP_W * 3; i < WIDE_STRIP_W * FB_H * 3; i++) ASSERT_EQ(strip[i], 0);
  ASSERT(wide_slot_strip(&s, &o, 1, false, strip));
  for (int x = 0; x < WIDE_STRIP_W; x++)                // Y, the game's letter, when no glyph is given
    ASSERT_EQ(strip[(4 * WIDE_STRIP_W + x) * 3], x >= 5 && x < 11 && (hud_glyph('Y')[3] >> (7 - (x - 5))) & 1 ? 0 : 0xff);
  // with room, widescreen's box: letter, then the bracket from line 1 to 14, as tall as the bar
  static uint8_t box[WIDE_BOX_W * FB_H * 3];
  ASSERT(wide_slot_strip(&s, &o, 0, true, box));
  for (int y = 1; y <= 7; y++)
    for (int x = 0; x < 6; x++) ASSERT_EQ(box[(y * WIDE_BOX_W + 1 + x) * 3], (g[y - 1] >> (7 - x)) & 1 ? 0 : 0xff);
  for (int y = 1; y <= 14; y++) ASSERT_EQ(box[(y * WIDE_BOX_W + 9) * 3], 0);
  ASSERT_EQ(box[(15 * WIDE_BOX_W + 9) * 3], 0xff);
  for (int i = 16 * WIDE_BOX_W * 3; i < WIDE_BOX_W * FB_H * 3; i++) ASSERT_EQ(box[i], 0);
  o.slots = false;
  ASSERT(!wide_slot_strip(&s, &o, 0, false, strip));
  o.slots = true;
  s.hram[0x9b - 0x80] = 0;
  ASSERT(!wide_slot_strip(&s, &o, 0, false, strip));
}

int main(void) {
  RUN(glyphs_exist_for_every_code);
  RUN(glyphs_leave_the_bracket_alone);
  RUN(own_letters_match_the_tiles);
  RUN(letters_repainted_in_place);
  RUN(one_letter_alone);
  RUN(mirrored_a_keeps_its_glyph_the_right_way_round);
  RUN(other_art_in_the_tiles_is_left_alone);
  RUN(off_the_bar_is_left_alone);
  RUN(scrolled_bar_shifts_the_cells);
  RUN(second_drawn_copy_is_read);
  RUN(cell_palette_is_its_own);
  RUN(bank1_and_upside_down_cells_are_left_alone);
  RUN(slot_strip_draws_its_letter_on_a_tab);
  return 0;
}
