// Widescreen's room renderer against the PPU: through a movie, every sampled frame in normal play
// is drawn by wide_room_pixel over the middle 160 columns too, and must equal the emulator's
// picture wherever no sprite covers it, and each screen transition's first frame keeps the strips
// of the frame before. Then the neighbours: what a strip showed of the next room
// against that room's own picture once the movie is in it. (Skipped without the ROM.)
// WIDE_ONLY_SPRITES=1 runs only the sprite checks. WIDE_PNG=frame:path saves a widescreen frame (any frame, room changes included); WIDE_ONLY_NEIGHBOURS=1 runs only the neighbour
// checks, WIDE_PAIRS=1 lists each pair, WIDE_PAIR_PNG=room:season:path (WIDE_PAIR_AGES=1 for Ages)
// saves a pair's strips beside the real columns; WIDE_TIME=1 prints the renderer's cost per frame.
#include "unit.h"
#include "hw/render.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "game/game.h"
#include "assets/assets.h"
#include "wide/wide.h"
#include "wide/item.h"
#include "wide/npcs.h"
#include "wide/room.h"
#include "platform/png.h"
#include <stdlib.h>
#include <time.h>

static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

// A screen transition's first frame (scroll mode 4): the screen and its scroll are still the room
// Link is leaving, whether the frame ended before or after the game stepped wActiveRoom on to the
// next one (wLoadingRoom stays the room on screen until the load), so the strips must be the ones
// of the frame before on every line no sprite reaches. Checked on every frame of the movies.
typedef struct {
  bool seasons;
  const uint8_t *rom;
  size_t rom_size;
  GBSample prev;
  bool have_prev;
  int checked, stepped, bad;
} FirstStepCheck;

static bool line_has_sprite(const GBSample *s, int y) {
  int h = (s->line_lcdc[y] & 0x04) ? 16 : 8;
  for (int d = 0; d < 2; d++) {
    const uint8_t *oam = s->drawn_oam[d];
    for (int i = 0; i < 40; i++)
      if (y >= oam[i * 4] - 16 && y < oam[i * 4] - 16 + h) return true;
    const OffscreenSprites *off = &s->drawn_offscreen[d];
    for (int i = 0; i < off->n; i++)
      if (y >= off->s[i].y && y < off->s[i].y + h) return true;
  }
  return false;
}

static void check_first_step(FirstStepCheck *c, const GBSample *s) {
  const uint8_t *w = s->wram[0], *p = c->prev.wram[0];
  int group = c->seasons ? 0xc49 : 0xc2d, room = c->seasons ? 0xc4c : 0xc30, loading = room - 1;   // wLoadingRoom
  if (c->have_prev && (w[0xd00] & 0x0f) == 4 && (p[0xd00] & 0x0f) == 1 && w[group] == p[group] && p[room] == p[loading] &&
      w[loading] == p[loading] && !w[0x4ab] && !p[0x4ab]) {
    static uint8_t before[WIDE_W * FB_H * 3], now[WIDE_W * FB_H * 3];
    WideOptions o = {.seasons = c->seasons, .border = {1, 2, 3}, .rom = c->rom, .rom_size = c->rom_size};
    wide_render(&c->prev, &o, before);
    wide_render(s, &o, now);
    long compared = 0, same = 0;
    for (int y = 16; y < FB_H; y++) {
      if (line_has_sprite(s, y) || line_has_sprite(&c->prev, y) || s->line_scx[y] != c->prev.line_scx[y] ||
          s->line_scy[y] != c->prev.line_scy[y] || (s->line_lcdc[y] & 0x81) != 0x81)
        continue;
      for (int x = 0; x < WIDE_W; x++) {
        if (x == WIDE_SIDE) x += FB_W;
        size_t i = ((size_t)y * WIDE_W + x) * 3;
        compared++;
        same += !memcmp(before + i, now + i, 3);
      }
    }
    if (compared) {
      c->checked++;
      c->stepped += w[room] != w[loading];
      if (same * 100 < compared * 95 && c->bad++ < 5)
        fprintf(stderr, "    frame %llu: room %02x (on screen %02x), %.0f%% of the strips as the frame before\n", (unsigned long long)s->frame,
                w[room], w[loading], 100.0 * same / compared);
    }
  }
  memcpy(&c->prev, s, sizeof *s);
  c->have_prev = true;
}

typedef struct {
  bool seasons;
  uint64_t checked, large, pixels, mismatches, first_bad;
  bool saved;
  const uint8_t *rom;
  size_t rom_size;
  FirstStepCheck *step;
} WideCheck;

static bool under_sprite(const GBSample *s, int x, int y) {
  int h = (s->line_lcdc[y] & 0x04) ? 16 : 8;
  const uint8_t *oam = s->drawn_oam[s->line_drawn[y] & 1];
  for (int i = 0; i < 40; i++) {
    int sy = oam[i * 4] - 16, sx = oam[i * 4 + 1] - 8;
    if (x >= sx && x < sx + 8 && y >= sy && y < sy + h) return true;
  }
  return false;
}

static void check_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  WideCheck *c = ctx;
  check_first_step(c->step, s);
  if (getenv("WIDE_TIME") && c->seasons && s->frame >= 3000 && s->frame < 13000) {   // the renderer's cost per frame
    static uint8_t wide[WIDE_W * FB_H * 3];
    static double total, worst;
    static int n;
    WideOptions o = {c->seasons, true, {40, 32, 16}, c->rom, c->rom_size, false, {0, 0}, NULL, {0}, false};
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    wide_render(s, &o, wide);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ms = (t1.tv_sec - t0.tv_sec) * 1e3 + (t1.tv_nsec - t0.tv_nsec) / 1e6;
    total += ms; n++; if (ms > worst) worst = ms;
    if (s->frame == 12999) fprintf(stderr, "wide_render: %.3f ms average, %.3f ms worst over %d frames\n", total / n, worst, n);
  }
  // WIDE_PNG=frame:path.png saves that frame's widescreen picture (checked frames only, every 7th)
  const char *png = getenv("WIDE_PNG");
  unsigned long long at;
  char path[400];
  if (png && sscanf(png, "%llu:%399s", &at, path) == 2 && s->frame >= at && !c->saved) {
    static uint8_t wide[WIDE_W * FB_H * 3];
    WideOptions o = {c->seasons, true, {40, 32, 16}, c->rom, c->rom_size, false, {0, 0}, NULL, {0}, false};
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
  FirstStepCheck *step = calloc(1, sizeof *step);
  step->seasons = seasons; step->rom = rom; step->rom_size = n;
  WideCheck c = {seasons, 0, 0, 0, 0, 0, false, rom, n, step};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = check_cb; gb->frame_ctx = &c;
  for (uint64_t i = 0; i < frames && !gb->hung; i++) gb_run_frame(gb);
  printf("  %llu frames checked (%llu in large rooms), %llu pixels, %llu differ (first at frame %llu)\n",
         (unsigned long long)c.checked, (unsigned long long)c.large, (unsigned long long)c.pixels,
         (unsigned long long)c.mismatches, (unsigned long long)c.first_bad);
  printf("  %d screen transitions checked from their first frame (%d with the room already stepped on), %d with other strips\n",
         step->checked, step->stepped, step->bad);
  ASSERT(c.checked > 100);
  ASSERT(c.large > 10);
  ASSERT_EQ(c.mismatches, 0);
  ASSERT(step->checked > 10);
  ASSERT_EQ(step->bad, 0);
  free(step);
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
  WideOptions o = {c->seasons, false, {1, 2, 3}, c->rom, c->rom_size, false, {0, 0}, NULL, {0}, false};
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
  long pairs = 0, same = 0, compared = 0;
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

// Sprites past the screen's edges (#21): when the screen has scrolled left, Impa is put at x 167,
// just past the right edge, and walks back in; the GBC drops her sprites there and the strip shows
// them instead, in step with the half the GBC still draws.
typedef struct { int frames, seen; } ImpaCheck;

static void impa_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  ImpaCheck *c = ctx;
  if (s->frame < 2348 || s->frame > 2352) return;
  c->frames++;
  const OffscreenSprites *o = &s->drawn_offscreen[s->line_drawn[100]];
  for (int i = 0; i < o->n; i++)
    if (o->s[i].x >= FB_W && o->s[i].x < FB_W + 16 && o->s[i].y > 80 && o->s[i].y < 112) { c->seen++; break; }
}

static void ages_scroll_displays_a_complete_frame(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/ages-boot.state")) SKIP("boot state missing");
  static GBSample display;
  gb_set_display(gb, &display);
  gb->input_at = tas_cb; gb->input_ctx = &t;
  uint16_t previous[FB_W * FB_H];
  // Follow the movie until Link and Impa enter 59 from the east. Walk south
  // through the exit beside the bush, as in the issue #52 recording.
  for (int i = 0; i <= 2230; i++) {
    if (i > 2100 && gb->wram[0][0xc30] == 0x59 && gb->wram[0][0xd00] == 1) {
      gb->input_at = NULL;
      gb->joy = gb->wram[1][0x0d] > 137 ? JOY_LEFT : JOY_DOWN;
    }
    gb_run_frame(gb);
    ASSERT(!gb->hung);
    if (i == 2229) memcpy(previous, display.framebuffer, sizeof previous);
  }
  const GBSample *grid = gb->sample;
  ASSERT_EQ(grid->wram[0][0xc30], 0x69);
  ASSERT_EQ(grid->wram[0][0xd02], 2);
  ASSERT_EQ(grid->io[R_LY], 74);
  ASSERT_EQ(display.io[R_LY], 144);
  printf("grid frame %llu SCY above/below split %d/%d, VBlank %d/%d\n",
         (unsigned long long)grid->frame, grid->line_scy[40], grid->line_scy[100],
         display.line_scy[40], display.line_scy[100]);
  ASSERT(grid->line_scy[40] != grid->line_scy[100]);
  for (int y = 16; y < FB_H; y++) ASSERT_EQ(display.line_scy[y], grid->line_scy[40]);
  size_t top = 74 * FB_W, bottom = 75 * FB_W;
  ASSERT(!memcmp(grid->framebuffer, display.framebuffer, top * sizeof(uint16_t)));
  ASSERT(!memcmp(grid->framebuffer + bottom, previous + bottom, sizeof previous - bottom * sizeof(uint16_t)));
  ASSERT(memcmp(grid->framebuffer + bottom, display.framebuffer + bottom, sizeof previous - bottom * sizeof(uint16_t)));
  gb_set_display(NULL, NULL);
}

static void ages_strips_show_impa_past_the_edge(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/ages-boot.state")) SKIP("boot state missing");
  ImpaCheck c = {0, 0};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = impa_cb; gb->frame_ctx = &c;
  for (int i = 0; i < 2360 && !gb->hung; i++) gb_run_frame(gb);
  ASSERT_EQ(c.frames, 5);
  ASSERT_EQ(c.seen, 5);
}

// The characters of the rooms beside the screen: a copy of the game goes into each one. In the
// past's Lynna village (1:49) two villagers of 1:48 stand within the strip's 48 pixels of the shared
// edge. The copies must leave the game itself exactly as it was.
static uint64_t run_ages_to(uint64_t frames, bool npcs, int *edge_pixels, int *random_pixels) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  static Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/ages-boot.state")) SKIP("boot state missing");
  gb->input_at = tas_cb; gb->input_ctx = &t;
  wide_npcs_reset();
  for (uint64_t i = 0; i < frames && !gb->hung; i++) {
    gb_run_frame(gb);
    if (npcs && wide_npcs_update(gb)) wide_npcs_work();
  }
  if (edge_pixels) {
    *edge_pixels = 0;
    const WideRoomSprites *r = wide_npcs_room(1, 0x48);
    for (int y = 0; r && y < 128; y++)
      for (int x = FB_W - WIDE_SIDE; x < FB_W; x++) *edge_pixels += r->c[y][x] >= 0;
  }
  if (random_pixels) {
    // room $54 of the past holds only enemies its object list places at random
    *random_pixels = -1;
    const WideRoomSprites *r = wide_npcs_room(1, 0x54);
    if (!r) r = wide_npcs_room(0, 0x54);
    if (r) {
      *random_pixels = 0;
      for (int y = 0; y < 176; y++)
        for (int x = 0; x < 256; x++) *random_pixels += r->c[y][x] >= 0;
    }
  }
  uint64_t h = gb_state_hash(gb);
  wide_npcs_stop();
  return h;
}

static void ages_strips_show_the_neighbours_characters(void) {
  int pixels, random;
  uint64_t with = run_ages_to(19300, true, &pixels, NULL), without = run_ages_to(19300, false, NULL, NULL);
  printf("  %d pixels of 1:48's characters by the edge\n", pixels);
  ASSERT(pixels > 100);
  ASSERT_EQ(with, without);
  // enemies placed at a random spot would stand elsewhere once Link walks in: never shown
  run_ages_to(20000, true, NULL, &random);
  printf("  room $54's randomly placed enemies: %d pixels\n", random);
  ASSERT_EQ(random, 0);
}

// The X and Y item icons, which the game never draws: decoded straight from the ROM, so the values
// here are the ones in data/seasons/treasureDisplayData.s.
static int icon_colours(const WideItemIcon *ic) {
  bool seen[4] = {false, false, false, false};
  int n = 0;
  for (int y = 0; y < 16; y++)
    for (int x = 0; x < ic->halves * 8; x++)
      if (!seen[ic->ci[y][x]]) { seen[ic->ci[y][x]] = true; n++; }
  return n;
}

static void item_icons_decode_from_the_rom(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  static uint8_t c600[256];
  memset(c600, 1, sizeof c600);                         // every level and selection at 1
  WideItemIcon ic;
  ASSERT(!wide_item_icon(rom, n, true, 0, c600, &ic));  // no item in the slot, nothing to draw
  ASSERT(wide_item_icon(rom, n, true, 0x03, c600, &ic));          // bombs
  ASSERT_EQ(ic.halves, 1);
  ASSERT_EQ(ic.pal[0], 4);                              // the record's attribute byte, $04
  ASSERT(icon_colours(&ic) > 1);                        // and it actually has a picture in it
  ASSERT(wide_item_icon(rom, n, true, 0x07, c600, &ic));          // rod of seasons, attribute $02
  ASSERT_EQ(ic.pal[0], 2);
  ASSERT(icon_colours(&ic) > 1);
  ASSERT(wide_item_icon(rom, n, true, 0x19, c600, &ic));          // seed satchel: the bag and a seed
  ASSERT_EQ(ic.halves, 2);
  // An item with levels takes the variant the $c600 page names, so a different level is a different
  // picture (the wooden and noble swords).
  WideItemIcon lv1, lv2;
  c600[0xb0] = 1;
  ASSERT(wide_item_icon(rom, n, true, 0x05, c600, &lv1));
  for (int i = 0; i < 256; i++) c600[i] = 2;
  ASSERT(wide_item_icon(rom, n, true, 0x05, c600, &lv2));
  ASSERT(memcmp(lv1.ci, lv2.ci, sizeof lv1.ci) != 0);
  free(rom);
}

// The map (#24): a menu over the whole screen keeps the status bar's split flag but draws its own
// top lines, so with the X and Y boxes on they must stay where the game put them.
typedef struct { const uint8_t *rom; size_t rom_size; int maps, differ; } MapCheck;

static void map_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  MapCheck *c = ctx;
  if (s->wram[0][0xbcb] != 0x02 || s->line_lcdc[0] != s->line_lcdc[16]) return;
  static uint8_t with[WIDE_W * FB_H * 3], without[WIDE_W * FB_H * 3];
  WideOptions on = {true, true, {40, 32, 16}, c->rom, c->rom_size, 2, {0x05, 0x06}, NULL, {0}, false};
  WideOptions off = on;
  off.extra_slots = 0;
  wide_render(s, &on, with);
  wide_render(s, &off, without);
  c->maps++;
  for (int y = 0; y < 16; y++)                          // the middle only: the strips' menu colour is held across calls
    if (memcmp(with + ((size_t)y * WIDE_W + WIDE_SIDE) * 3, without + ((size_t)y * WIDE_W + WIDE_SIDE) * 3, FB_W * 3)) { c->differ++; break; }
}

static void seasons_map_has_no_status_bar(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/seasons-play.inputs")) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/seasons-boot.state")) SKIP("boot state missing");
  MapCheck c = {rom, n, 0, 0};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = map_cb; gb->frame_ctx = &c;
  for (uint64_t i = 0; i < 265064 && c.maps < 60 && !gb->hung; i++) gb_run_frame(gb);
  printf("  %d map frames, %d with the bar drawn over them\n", c.maps, c.differ);
  ASSERT(c.maps > 0);
  ASSERT_EQ(c.differ, 0);
}

// Taking an essence (#34, the reporter's Ages state: the vision that follows) keeps the split on but
// turns the whole status bar one colour, the game's own B and A items with it, so X and Y must not
// stay over the scene either. A bar with anything on it keeps them. No ROM needed: the letters and
// brackets are drawn even with no item to show.
static bool slots_drawn(const uint8_t *out) {
  for (int y = 0; y < 16; y++)
    for (int x = 0; x < WIDE_SIDE; x++)
      if (memcmp(out + ((size_t)y * WIDE_W + x) * 3, out, 3)) return true;
  return false;
}

static void blanked_bar_has_no_x_and_y(void) {
  static GBSample s;
  static uint8_t out[WIDE_W * FB_H * 3], strip[WIDE_BOX_W * FB_H * 3];
  memset(&s, 0, sizeof s);
  s.hram[0xff9b - 0xff80] = 3;                          // Ages' LCD behaviour: the status bar's split
  s.wram[0][0xd00] = 0x01;                              // wScrollMode: normal play
  for (int y = 0; y < FB_H; y++) s.line_lcdc[y] = y < 16 ? 0xc7 : 0xe7;
  s.drawn_bg_pal[0][6] = 0xff;                          // BG palette 0 colour 3, the bar's text: white
  s.drawn_bg_pal[0][7] = 0x7f;
  WideOptions o = {false, false, {0, 0, 0}, NULL, 0, 2, {0, 0}, NULL, {0}, false};
  wide_render(&s, &o, out);
  ASSERT(!slots_drawn(out));
  ASSERT(!wide_slot_strip(&s, &o, 0, false, strip));
  ASSERT(!wide_slot_strip(&s, &o, 1, true, strip));
  // the scene's own sprite reaching into the bar's lines (#34: the harp held up over a dark room)
  s.drawn_oam[0][0] = 16 + 10;
  s.drawn_oam[0][1] = 8 + 90;
  s.drawn_oam[0][2] = 0x02;
  memset(&s.drawn_vram[0][0][0x02 * 16], 0xff, 32);
  for (int y = 10; y < 16; y++)
    for (int x = 90; x < 98; x++) s.framebuffer[y * FB_W + x] = 0x001f;
  wide_render(&s, &o, out);
  ASSERT(!slots_drawn(out));
  ASSERT(!wide_slot_strip(&s, &o, 0, false, strip));
  s.framebuffer[40] = 0x7fff;                           // one pixel of the bar drawn
  wide_render(&s, &o, out);
  ASSERT(slots_drawn(out));
  ASSERT(wide_slot_strip(&s, &o, 0, false, strip));
}

// Scenes over the whole screen (#14: Ages' ending and credits; here Seasons' opening) keep the
// status bar's split behaviour with wGfxRegsFinal.LYC past the last line, so the game never draws
// the bar: no X and Y boxes, no room strips, and the top lines stay the scene's own, blank frames
// between the scenes included.
typedef struct { const uint8_t *rom; size_t rom_size; int scenes, blank, wrong, bar; } SceneCheck;

static void scene_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  SceneCheck *c = ctx;
  static uint8_t with[WIDE_W * FB_H * 3], without[WIDE_W * FB_H * 3], strip[WIDE_STRIP_W * FB_H * 3];
  WideOptions on = {true, true, {40, 32, 16}, c->rom, c->rom_size, 2, {0x05, 0x06}, NULL, {0}, false};
  bool lit = s->line_lcdc[0] & 0x80;
  if (s->wram[0][0x49c] < FB_H || (lit && s->line_lcdc[0] != s->line_lcdc[16])) {
    if (wide_slot_strip(s, &on, 0, false, strip)) c->bar++;
    return;
  }
  WideOptions off = on;
  off.extra_slots = 0;
  wide_render(s, &on, with);
  wide_render(s, &off, without);
  c->scenes++;
  if (!lit) c->blank++;
  bool differ = false;
  for (int y = 0; y < 16; y++)
    if (memcmp(with + ((size_t)y * WIDE_W + WIDE_SIDE) * 3, without + ((size_t)y * WIDE_W + WIDE_SIDE) * 3, FB_W * 3)) differ = true;
  if (differ || wide_slot_strip(s, &on, 0, false, strip) || wide_room_pixel(s, true, 80, 80) >= 0) c->wrong++;
}

static void seasons_opening_has_no_status_bar(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/seasons-play.inputs")) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/seasons-boot.state")) SKIP("boot state missing");
  SceneCheck c = {rom, n, 0, 0, 0, 0};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = scene_cb; gb->frame_ctx = &c;
  for (uint64_t i = 0; i < 19000 && !gb->hung; i++) gb_run_frame(gb);
  printf("  %d scene frames (%d blank), %d drawn as play; %d play frames with the bar\n", c.scenes, c.blank, c.wrong, c.bar);
  ASSERT(c.scenes > 4000);
  ASSERT(c.blank > 50);
  ASSERT_EQ(c.wrong, 0);
  ASSERT(c.bar > 5000);
  free(rom);
}

// Temple Remains once the volcano has erupted (#24): the game loads its lava layouts from rooms past
// the end of the Subrosia map and takes the tileset from templeRemainsSeasons
// (data/seasons/tilesets.s: $0f $01, mapping $16, layout group $04, animations $08), so a strip
// beside one shows lava and not the grass it had before.
static void seasons_temple_remains_after_the_eruption(void) {
  size_t n;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", &n);
  if (!rom) SKIP("ROM not present");
  static WideRoom before, after;
  for (int season = 0; season < 4; season++) {
    WideRoomKey k;
    memset(&k, 0, sizeof k);
    k.room = 0x25;
    k.season = season;
    ASSERT(wide_decode_room(rom, n, true, &k, &before));
    k.remains_lava = true;
    ASSERT(wide_decode_room(rom, n, true, &k, &after));
    ASSERT_EQ(after.tileset[0], 0x0f);
    ASSERT_EQ(after.tileset[5], 0x16);
    ASSERT_EQ(after.tileset[6], 0x04);
    ASSERT(memcmp(before.layout, after.layout, sizeof before.layout) != 0);
    k.room = 0x18;                                       // outside the Remains: the flag changes nothing
    ASSERT(wide_decode_room(rom, n, true, &k, &after));
    k.remains_lava = false;
    ASSERT(wide_decode_room(rom, n, true, &k, &before));
    ASSERT(memcmp(&before, &after, sizeof before) == 0);
  }
  free(rom);
}

// wide_play_origin against the game's own camera (hCameraX/hCameraY, 16-bit) through the first
// 60000 movie frames. The origin is the drawn picture's, so it equals the camera except while the
// camera moves (one frame behind: the previous frame's camera) and during a screen shake (up to 3
// pixels off). Most frames must match exactly, none may be off by more than that, and the camera
// must have been seen away from 0 on both axes so a swapped or dropped axis cannot pass. Shakes
// must still count as play (86 and 85 frames in these movies; without the 3-pixel slack, 20 and 5).
typedef struct {
  bool seasons;
  uint16_t cy, cx;
  int prev_x, prev_y;
  long shown, exact, lagged, shaken, wrong, moved_x, moved_y;
} OriginCheck;

static void origin_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  OriginCheck *c = ctx;
  int cy = s->hram[c->cy - 0xff80] | s->hram[c->cy - 0xff80 + 1] << 8;
  int cx = s->hram[c->cx - 0xff80] | s->hram[c->cx - 0xff80 + 1] << 8;
  int rx, ry;
  if (wide_play_origin(s, c->seasons, &rx, &ry)) {
    c->shown++;
    if (rx >= 16) c->moved_x++;
    if (ry >= 16) c->moved_y++;
    if (rx == cx && ry == cy) c->exact++;
    else if (rx == c->prev_x && ry == c->prev_y) c->lagged++;
    else if (abs(rx - cx) <= 3 && abs(ry - cy) <= 3) c->shaken++;
    else {
      if (!c->wrong) printf("  frame %llu: origin %d,%d, camera %d,%d\n", (unsigned long long)s->frame, rx, ry, cx, cy);
      c->wrong++;
    }
  }
  c->prev_x = cx;
  c->prev_y = cy;
}

static void origin_follows_the_camera(const char *rom_path, const char *inputs, const char *state_path, bool seasons) {
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
  OriginCheck c = {seasons, seasons ? 0xffa8 : 0xffaa, seasons ? 0xffaa : 0xffac, -1, -1, 0, 0, 0, 0, 0, 0, 0};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = origin_cb; gb->frame_ctx = &c;
  for (int i = 0; i < 60000 && !gb->hung; i++) gb_run_frame(gb);
  printf("  %ld frames in play: %ld exact, %ld a frame behind, %ld shaken, %ld wrong; camera off 0: x %ld, y %ld\n",
         c.shown, c.exact, c.lagged, c.shaken, c.wrong, c.moved_x, c.moved_y);
  ASSERT(c.shown > 10000);
  ASSERT(c.exact * 10 >= c.shown * 9);
  ASSERT_EQ(c.wrong, 0);
  ASSERT(c.shaken >= 50);
  ASSERT(c.moved_x > 100);
  ASSERT(c.moved_y > 100);
  free(gb);
  free(rom);
}

static void seasons_play_origin_follows_the_camera(void) {
  origin_follows_the_camera(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs",
                            TAS_DIR "/seasons-boot.state", true);
}

static void ages_play_origin_follows_the_camera(void) {
  origin_follows_the_camera(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs",
                            TAS_DIR "/ages-boot.state", false);
}

int main(void) {
  RUN(ages_scroll_displays_a_complete_frame);
  if (getenv("WIDE_ONLY_SCROLL")) return 0;
  if (getenv("WIDE_ONLY_SPRITES")) {
    RUN(ages_strips_show_impa_past_the_edge);
    RUN(ages_strips_show_the_neighbours_characters);
    return 0;
  }
  if (!getenv("WIDE_ONLY_NEIGHBOURS")) {
    RUN(seasons_room_pixels_match_the_ppu);
    RUN(ages_room_pixels_match_the_ppu);
  }
  RUN(blanked_bar_has_no_x_and_y);
  RUN(seasons_play_origin_follows_the_camera);
  RUN(ages_play_origin_follows_the_camera);
  RUN(item_icons_decode_from_the_rom);
  RUN(seasons_temple_remains_after_the_eruption);
  RUN(seasons_map_has_no_status_bar);
  RUN(seasons_opening_has_no_status_bar);
  RUN(ages_strips_show_impa_past_the_edge);
  RUN(ages_strips_show_the_neighbours_characters);
  RUN(seasons_neighbours_match_the_rooms);
  RUN(ages_neighbours_match_the_rooms);
  return 0;
}
