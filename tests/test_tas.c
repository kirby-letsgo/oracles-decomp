#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include <stdlib.h>

static void inputs_file_loads(void) {
  Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs file missing");
  ASSERT_EQ(t.count, 289518);
  ASSERT_EQ(tas_input_at(&t, 1), 0);
  ASSERT_EQ(tas_input_at(&t, 441), JOY_START);
  ASSERT_EQ(tas_input_at(&t, 461), JOY_START);
  ASSERT_EQ(tas_input_at(&t, 999999), 0);
  tas_free(&t);
}

static void frame_grid_is_70224_cycles(void) {
  static uint8_t rom[0x8000];
  static GB gb;
  memset(rom, 0, sizeof rom);
  rom[0x100] = 0x18; rom[0x101] = 0xfe;
  gb_init(&gb);
  ASSERT(gb_load_rom(&gb, rom, sizeof rom));
  gb_reset(&gb);
  ASSERT_EQ(gb.cycles, BOOT_CYCLES);
  gb_run_frame(&gb);
  ASSERT_EQ(gb.cycles / FRAME_CYCLES, BOOT_CYCLES / FRAME_CYCLES + 1);
  gb_run_frame(&gb);
  ASSERT_EQ(gb.cycles / FRAME_CYCLES, BOOT_CYCLES / FRAME_CYCLES + 2);
}

static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

static void full_tas_matches_reference(void) {
  const char *rom_path = GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc";
  size_t n, bn;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("Ages ROM not present");
  uint8_t *boot = oracles_read_file(GAME_ROM_DIR "/cgb_boot.bin", &bn);
  if (!boot) SKIP("cgb_boot.bin not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs missing");
  FILE *ref = fopen(TAS_DIR "/ages.ref", "r");
  if (!ref) SKIP("ages.ref missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  oracles_apply_agb_boot_patch(boot, bn);
  gb_set_boot_rom(gb, boot, bn);
  ASSERT(oracles_load_init_ram(gb, TAS_DIR "/gbhawk-wram0.txt"));
  gb_reset(gb);
  const char *limit_env = getenv("TAS_FRAMES");
  uint64_t limit = limit_env ? strtoull(limit_env, NULL, 10) : 20000;
  unsigned long long f = 0, want = 0;
  bool have_ref = fscanf(ref, "%llu %llx", &f, &want) == 2;
  gb->input_at = tas_cb; gb->input_ctx = &t;
  for (uint64_t i = 0; i < limit; i++) {
    uint64_t frame = gb_run_frame(gb);
    while (have_ref && f < frame) have_ref = fscanf(ref, "%llu %llx", &f, &want) == 2;
    if (have_ref && f == frame) {
      if (want != gb_state_hash(gb)) { fprintf(stderr, "state mismatch at frame %llu\n", f); ASSERT(0); }
      have_ref = fscanf(ref, "%llu %llx", &f, &want) == 2;
    }
  }
  fclose(ref); free(gb); free(rom); free(boot); tas_free(&t);
}

int main(void) { RUN(inputs_file_loads); RUN(frame_grid_is_70224_cycles); RUN(full_tas_matches_reference); return 0; }
