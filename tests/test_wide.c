// Widescreen's room renderer against the PPU: through a movie, every sampled frame in normal play
// is drawn by wide_room_pixel over the middle 160 columns too, and must equal the emulator's
// picture wherever no sprite covers it. Then the neighbours: what a strip showed of the next room
// against that room's own picture once the movie is in it. (Skipped without the ROM.)
// WIDE_PNG=frame:path saves a widescreen frame (any frame, room changes included); WIDE_ONLY_NEIGHBOURS=1 runs only the neighbour
// checks, WIDE_PAIRS=1 lists each pair, WIDE_PAIR_PNG=room:season:path (WIDE_PAIR_AGES=1 for Ages)
// saves a pair's strips beside the real columns.
#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "game/game.h"
#include "assets/assets.h"
#include "wide/wide.h"
#include "platform/png.h"
#include <stdlib.h>

static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

typedef struct {
  bool seasons;
  uint64_t checked, large, pixels, mismatches, first_bad;
  bool saved;
  const uint8_t *rom;
  size_t rom_size;
} WideCheck;

static bool under_sprite(const GBSample *s, int x, int y) {
  int h = (s->line_lcdc[y] & 0x04) ? 16 : 8;
  const uint8_t *oam = s->drawn_oam[s->line_drawn[y]];
  for (int i = 0; i < 40; i++) {
    int sy = oam[i * 4] - 16, sx = oam[i * 4 + 1] - 8;
    if (x >= sx && x < sx + 8 && y >= sy && y < sy + h) return true;
  }
  return false;
}

static void check_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  WideCheck *c = ctx;
  // WIDE_PNG=frame:path.png saves that frame's widescreen picture (checked frames only, every 7th)
  const char *png = getenv("WIDE_PNG");
  unsigned long long at;
  char path[400];
  if (png && sscanf(png, "%llu:%399s", &at, path) == 2 && s->frame >= at && !c->saved) {
    static uint8_t wide[WIDE_W * FB_H * 3];
    WideOptions o = {c->seasons, true, {40, 32, 16}, c->rom, c->rom_size};
    wide_render(s, &o, wide);
    char full[440];
    snprintf(full, sizeof full, "%s-%s.png", path, c->seasons ? "seasons" : "ages");
    fprintf(stderr, "saving frame %llu to %s: %d\n", (unsigned long long)s->frame, full, png_write_rgb(full, wide, WIDE_W, FB_H));
    c->saved = true;
  }
  if (s->frame % 7) return;
  if (wide_room_pixel(s, c->seasons, 80, 80) < 0) return;          // not in normal play
  c->checked++;
  if (s->wram[0][0xd0a] > 20) c->large++;
  for (int y = 16; y < FB_H; y++)
    for (int x = 0; x < FB_W; x++) {
      if (under_sprite(s, x, y) || wide_window_at(s, x, y)) continue;
      int want = s->framebuffer[y * FB_W + x], got = wide_room_pixel(s, c->seasons, x, y);
      if (!(s->line_lcdc[y] & 0x80) || got < 0) continue;     // a blank line, or outside the room (shake)
      c->pixels++;
      if (got != want) { if (!c->mismatches) c->first_bad = s->frame; c->mismatches++; }
    }

}

static void check_movie(const char *rom_path, const char *inputs, const char *state_path, uint64_t frames, bool seasons) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, inputs)) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, state_path)) SKIP("boot state missing");
  WideCheck c = {seasons, 0, 0, 0, 0, 0, false, rom, n};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = check_cb; gb->frame_ctx = &c;
  for (uint64_t i = 0; i < frames && !gb->hung; i++) gb_run_frame(gb);
  printf("  %llu frames checked (%llu in large rooms), %llu pixels, %llu differ (first at frame %llu)\n",
         (unsigned long long)c.checked, (unsigned long long)c.large, (unsigned long long)c.pixels,
         (unsigned long long)c.mismatches, (unsigned long long)c.first_bad);
  ASSERT(c.checked > 100);
  ASSERT(c.large > 10);
  ASSERT_EQ(c.mismatches, 0);
}

// Neighbours against the game: while standing in a small overworld room the strips show the rooms
// beside it; once the movie is in one of those rooms (settled), its own picture must match what the
// strip showed, background pixels only (no sprites, window or status bar).
typedef struct {
  int group, room, modifier;
  bool have_view;
  uint16_t strip[2][FB_H][WIDE_SIDE];      // what the strips showed: left, right (0x8000: nothing)
  uint16_t edge[2][FB_H][WIDE_SIDE];       // the room's own first / last 48 columns (0x8000: a sprite)
} RoomView;

typedef struct {
  bool seasons;
  const uint8_t *rom;
  size_t rom_size;
  RoomView *views;
  int n, stable, last_room;
} NeighbourCheck;

static void neighbour_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  NeighbourCheck *c = ctx;
  const uint8_t *w = s->wram[0];
  int group = w[c->seasons ? 0xc49 : 0xc2d], room = w[c->seasons ? 0xc4c : 0xc30], modifier = w[c->seasons ? 0xc4e : 0xc32];
  if (group > 1 || w[0xd0a] != 20 || wide_room_pixel(s, c->seasons, 80, 80) < 0 || w[0x4ab]) { c->stable = 0; return; }
  if (room != c->last_room) { c->last_room = room; c->stable = 0; return; }
  if (++c->stable != 20) return;
  for (int i = 0; i < c->n; i++)
    if (c->views[i].group == group && c->views[i].room == room && c->views[i].modifier == modifier) return;
  c->views = realloc(c->views, (size_t)(c->n + 1) * sizeof *c->views);
  RoomView *v = &c->views[c->n++];
  v->group = group; v->room = room; v->modifier = modifier; v->have_view = true;
  static uint8_t wide[WIDE_W * FB_H * 3];
  WideOptions o = {c->seasons, false, {1, 2, 3}, c->rom, c->rom_size};
  wide_render(s, &o, wide);
  for (int y = 0; y < FB_H; y++)
    for (int x = 0; x < WIDE_SIDE; x++)
      for (int side = 0; side < 2; side++) {
        const uint8_t *p = wide + ((size_t)y * WIDE_W + (side ? WIDE_SIDE + FB_W + x : x)) * 3;
        bool border = p[0] == 1 && p[1] == 2 && p[2] == 3;
        v->strip[side][y][x] = border || y < 16 ? 0x8000 : (uint16_t)(p[0] >> 3 | (p[1] >> 3) << 5 | (p[2] >> 3) << 10);
        int ex = side ? FB_W - WIDE_SIDE + x : x;
        v->edge[side][y][x] = y < 16 || under_sprite(s, ex, y) ? 0x8000 : s->framebuffer[y * FB_W + ex];
      }
}

static void neighbours_match(const char *rom_path, const char *inputs, const char *state_path, uint64_t frames, bool seasons) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, inputs)) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, state_path)) SKIP("boot state missing");
  NeighbourCheck c = {seasons, rom, n, NULL, 0, 0, -1};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = neighbour_cb; gb->frame_ctx = &c;
  for (uint64_t i = 0; i < frames && !gb->hung; i++) gb_run_frame(gb);
  long pairs = 0, shown = 0, same = 0, compared = 0;
  for (int i = 0; i < c.n; i++)
    for (int j = 0; j < c.n; j++) {
      const RoomView *a = &c.views[i], *b = &c.views[j];
      if (a->group != b->group || b->room != a->room + 1 || (a->room & 0x0f) == 0x0f || a->modifier != b->modifier) continue;
      pairs++;
      long pair_same = 0, pair_n = 0;
      for (int y = 16; y < FB_H; y++)
        for (int x = 0; x < WIDE_SIDE; x++) {
          // a's right strip shows b's first columns; b's left strip shows a's last ones
          uint16_t checks[2][2] = {{a->strip[1][y][x], b->edge[0][y][x]}, {b->strip[0][y][x], a->edge[1][y][x]}};
          for (int k = 0; k < 2; k++) {
            if (checks[k][0] == 0x8000 || checks[k][1] == 0x8000) continue;
            shown++;
            compared++;
            pair_n++;
            if ((checks[k][0] & 0x7fff) == (checks[k][1] & 0x7fff)) { same++; pair_same++; }
          }
        }
      const char *want = getenv("WIDE_PAIR_PNG");         // "97:path": a's right strip | b's first columns, b's left strip | a's last
      char *after;
      if (want && strtol(want, &after, 16) == a->room && a->group == 0 && strtol(after + 1, &after, 10) == a->modifier &&
          (!getenv("WIDE_PAIR_AGES") || !seasons)) {
        static uint8_t img[FB_H * WIDE_SIDE * 4 * 3];
        const uint16_t (*parts[4])[WIDE_SIDE] = {a->strip[1], b->edge[0], b->strip[0], a->edge[1]};
        for (int q = 0; q < 4; q++)
          for (int y = 0; y < FB_H; y++)
            for (int x = 0; x < WIDE_SIDE; x++) {
              uint16_t v = parts[q][y][x];
              uint8_t *o = img + ((size_t)y * WIDE_SIDE * 4 + (size_t)q * WIDE_SIDE + x) * 3;
              o[0] = (uint8_t)((v & 31) << 3); o[1] = (uint8_t)((v >> 5 & 31) << 3); o[2] = (uint8_t)((v >> 10 & 31) << 3);
              if (v == 0x8000) o[0] = 255, o[1] = 0, o[2] = 255;
            }
        png_write_rgb(after + 1, img, WIDE_SIDE * 4, FB_H);
      }
      if (getenv("WIDE_PAIRS") && pair_n)
        fprintf(stderr, "    %d: %02x|%02x (season %d): %.0f%% of %ld\n", a->group, a->room, b->room, a->modifier, 100.0 * pair_same / pair_n, pair_n);
    }
  printf("  %d rooms seen, %ld neighbouring pairs, %ld pixels compared, %.1f%% the same\n", c.n, pairs, compared,
         compared ? 100.0 * same / compared : 0.0);
  ASSERT(pairs >= 10);
  ASSERT(same * 100 >= compared * 90);          // the rest: things that changed in the room (cut grass, doors)
  free(c.views);
}

static void seasons_neighbours_match_the_rooms(void) {
  neighbours_match(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs",
                   TAS_DIR "/seasons-boot.state", 265064, true);
}

static void ages_neighbours_match_the_rooms(void) {
  neighbours_match(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs",
                   TAS_DIR "/ages-boot.state", 289518, false);
}

static void seasons_room_pixels_match_the_ppu(void) {
  check_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs",
              TAS_DIR "/seasons-boot.state", 265064, true);
}

static void ages_room_pixels_match_the_ppu(void) {
  check_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs",
              TAS_DIR "/ages-boot.state", 289518, false);
}

int main(void) {
  if (!getenv("WIDE_ONLY_NEIGHBOURS")) {
    RUN(seasons_room_pixels_match_the_ppu);
    RUN(ages_room_pixels_match_the_ppu);
  }
  RUN(seasons_neighbours_match_the_rooms);
  RUN(ages_neighbours_match_the_rooms);
  return 0;
}
