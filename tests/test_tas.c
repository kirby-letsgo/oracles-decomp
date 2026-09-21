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

typedef struct { FILE *ref; unsigned long long f, want; bool have; uint64_t mismatch; } RefCheck;
static void ref_cb(GB *gb, const GBSample *sm, void *ctx) {
  RefCheck *r = ctx;
  if (r->mismatch) return;
  while (r->have && r->f < sm->frame) r->have = fscanf(r->ref, "%llu %llx", &r->f, &r->want) == 2;
  if (r->have && r->f == sm->frame) {
    if (r->want != gb_state_hash(gb)) r->mismatch = sm->frame;
    r->have = fscanf(r->ref, "%llu %llx", &r->f, &r->want) == 2;
  }
}

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
  RefCheck rc = {ref, 0, 0, false, 0};
  rc.have = fscanf(ref, "%llu %llx", &rc.f, &rc.want) == 2;
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = ref_cb; gb->frame_ctx = &rc;
  for (uint64_t i = 0; i < limit && !rc.mismatch; i++) gb_run_frame(gb);
  if (rc.mismatch) { fprintf(stderr, "state mismatch at frame %llu\n", rc.mismatch); ASSERT(0); }
  fclose(ref); free(gb); free(rom); free(boot); tas_free(&t);
}

static void seasons_run(const char *inputs, const char *ref_path, uint64_t frames, const char *what) {
  const char *rom_path = GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc";
  size_t n, bn;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("Seasons ROM not present");
  uint8_t *boot = oracles_read_file(GAME_ROM_DIR "/cgb_boot.bin", &bn);
  if (!boot) SKIP("cgb_boot.bin not present");
  FILE *ref = fopen(ref_path, "r");
  if (!ref) SKIP("reference missing");
  Tas t = {0};
  if (inputs && !tas_load(&t, inputs)) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  oracles_apply_agb_boot_patch(boot, bn);
  gb_set_boot_rom(gb, boot, bn);
  ASSERT(oracles_load_init_ram(gb, TAS_DIR "/gbhawk-wram0.txt"));
  gb_reset(gb);
  RefCheck rc = {ref, 0, 0, false, 0};
  rc.have = fscanf(ref, "%llu %llx", &rc.f, &rc.want) == 2;
  if (inputs) { gb->input_at = tas_cb; gb->input_ctx = &t; }
  gb->frame_cb = ref_cb; gb->frame_ctx = &rc;
  for (uint64_t i = 0; i < frames && !rc.mismatch; i++) gb_run_frame(gb);
  if (rc.mismatch) { fprintf(stderr, "seasons %s state mismatch at frame %llu\n", what, rc.mismatch); ASSERT(0); }
  fclose(ref); free(gb); free(rom); free(boot);
  if (inputs) tas_free(&t);
}

static void seasons_play_matches_reference(void) {
  seasons_run(TAS_DIR "/seasons-play.inputs", TAS_DIR "/seasons-play.ref", 67281, "playthrough");
}

static void seasons_demo_matches_reference(void) {
  seasons_run(NULL, TAS_DIR "/seasons-noinput.ref", 30000, "demo");
}

int main(void) { RUN(inputs_file_loads); RUN(frame_grid_is_70224_cycles); RUN(full_tas_matches_reference); RUN(seasons_demo_matches_reference); RUN(seasons_play_matches_reference); return 0; }
