// Widescreen's room renderer against the PPU: through a movie, every sampled frame in normal play
// is drawn by wide_room_pixel over the middle 160 columns too, and must equal the emulator's
// picture wherever no sprite covers it. (Skipped without the ROM.)
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
  // WIDE_PNG=frame:path.png saves that frame's widescreen picture (checked frames only, every 7th)
  const char *png = getenv("WIDE_PNG");
  unsigned long long at;
  char path[400];
  if (png && sscanf(png, "%llu:%399s", &at, path) == 2 && s->frame >= at && !c->saved) {
    static uint8_t wide[WIDE_W * FB_H * 3];
    WideOptions o = {c->seasons, true, {40, 32, 16}};
    wide_render(s, &o, wide);
    fprintf(stderr, "saving frame %llu to %s: %d\n", (unsigned long long)s->frame, path, png_write_rgb(path, wide, WIDE_W, FB_H));
    c->saved = true;
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
  WideCheck c = {seasons, 0, 0, 0, 0, 0, false};
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

static void seasons_room_pixels_match_the_ppu(void) {
  check_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs",
              TAS_DIR "/seasons-boot.state", 265064, true);
}

static void ages_room_pixels_match_the_ppu(void) {
  check_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs",
              TAS_DIR "/ages-boot.state", 289518, false);
}

int main(void) {
  RUN(seasons_room_pixels_match_the_ppu);
  RUN(ages_room_pixels_match_the_ppu);
  return 0;
}
