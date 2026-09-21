#include "unit.h"
#include "core/gb.h"
#include "hw/render.h"
#include "platform/png.h"

static void acid2_matches_reference(void) {
  size_t n, rn;
  uint8_t *rom = read_file(TEST_ROM_DIR "/cgb-acid2.gbc", &n);
  uint8_t *ref = read_file(TEST_ROM_DIR "/cgb-acid2-reference.rgb", &rn);
  if (!rom || !ref) SKIP("cgb-acid2 files missing");
  ASSERT_EQ(rn, FB_W * FB_H * 3);
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb_reset(gb);
  for (int i = 0; i < 60; i++) gb_run_frame(gb);
  static uint8_t rgb[FB_W * FB_H * 3];
  framebuffer_to_rgb(gb->framebuffer, rgb);
  int bad = 0;
  for (size_t i = 0; i < rn; i++) if ((rgb[i] >> 3) != (ref[i] >> 3)) bad++;
  if (bad) {
    png_write_rgb("acid2-actual.png", rgb, FB_W, FB_H);
    fprintf(stderr, "%d bytes differ, wrote acid2-actual.png\n", bad);
  }
  ASSERT_EQ(bad, 0);
  free(gb); free(rom); free(ref);
}

int main(void) { RUN(acid2_matches_reference); return 0; }
