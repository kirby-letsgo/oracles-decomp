#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "hooks/hooks.h"
#include "game/game.h"
#include "assets/assets.h"
#include "rt/fibers.h"
#include <stdlib.h>

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

// The native build has no interpreter, so it cannot run the CGB boot ROM: it starts from the
// post-boot state that oracles-run --boot-state-out recorded (tas/ages-boot.state) and must then
// produce the same state hashes as the emulator-hosted build.
static void native_run_reloading(const char *rom_path, const char *inputs, const char *ref_path, const char *state_path, uint64_t frames, uint64_t reload_every);
static void native_run(const char *rom_path, const char *inputs, const char *ref_path, const char *state_path, uint64_t frames) {
  native_run_reloading(rom_path, inputs, ref_path, state_path, frames, 0);
}

// reload_every: a save state round trip (every thread fiber dropped, as when the app loads a
// state) at the first frame after each multiple whose threads are parked, as the app saves.
static void native_run_reloading(const char *rom_path, const char *inputs, const char *ref_path, const char *state_path, uint64_t frames, uint64_t reload_every) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, inputs)) SKIP("inputs missing");
  FILE *ref = fopen(ref_path, "r");
  if (!ref) SKIP("reference missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  gb->code_bits = assets_code_bits(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, state_path)) SKIP("boot state missing");
  const char *limit_env = getenv("TAS_FRAMES");     // lifts the Ages default; a whole-movie value must not outrun the Seasons reference
  uint64_t limit = frames;
  if (limit_env && frames == 20000) limit = strtoull(limit_env, NULL, 10);
  RefCheck rc = {ref, 0, 0, false, 0};
  rc.have = fscanf(ref, "%llu %llx", &rc.f, &rc.want) == 2;
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = ref_cb; gb->frame_ctx = &rc;
  bool pending = false;
  for (uint64_t i = 0; i < limit && !rc.mismatch && !gb->hung; i++) {
    if (reload_every && i && i % reload_every == 0) pending = true;
    if (pending && threads_parked(gb)) {
      ASSERT(oracles_save_boot_state(gb, "/tmp/oracles-native-reload.state"));
      ASSERT(oracles_load_boot_state(gb, "/tmp/oracles-native-reload.state"));
      fibers_reset(gb);
      pending = false;
    }
    gb_run_frame(gb);
  }
  if (gb->hung) { fprintf(stderr, "native build stopped at frame %llu\n", (unsigned long long)GRID_FRAME(gb->cycles)); ASSERT(0); }
  if (rc.mismatch) { fprintf(stderr, "state mismatch at frame %llu\n", rc.mismatch); ASSERT(0); }
  fclose(ref); free(gb); free(rom); tas_free(&t);
}

static void native_full_tas_matches_reference(void) {
  native_run(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs", TAS_DIR "/ages.ref", TAS_DIR "/ages-boot.state", 20000);
}

// Seasons: the recorded playthrough, whole (33,337 frames), on the same binary
// Seasons: the console-verified TAS, the whole game, from a boot with RAM cleared
static void native_seasons_tas_matches_reference(void) {
  native_run(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-consoleverified.inputs", TAS_DIR "/seasons.ref", TAS_DIR "/seasons-consoleverified-boot.state", 321712);
}

static void native_seasons_play_matches_reference(void) {
  native_run(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs", TAS_DIR "/seasons-play.ref", TAS_DIR "/seasons-boot.state", 265064);
}

// Ages with a save state loaded about every 1000 frames: the native build resumes every thread
static void native_ages_survives_save_states(void) {
  native_run_reloading(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs", TAS_DIR "/ages.ref", TAS_DIR "/ages-boot.state", 20000, 1013);
}

int main(void) { RUN(native_full_tas_matches_reference); RUN(native_seasons_play_matches_reference); RUN(native_seasons_tas_matches_reference); RUN(native_ages_survives_save_states); return 0; }
