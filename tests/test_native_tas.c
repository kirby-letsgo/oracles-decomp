#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "hooks/hooks.h"
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
static void native_full_tas_matches_reference(void) {
  const char *rom_path = GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc";
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("Ages ROM not present");
  Tas t;
  if (!tas_load(&t, TAS_DIR "/ages-consoleverified.inputs")) SKIP("inputs missing");
  FILE *ref = fopen(TAS_DIR "/ages.ref", "r");
  if (!ref) SKIP("ages.ref missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/ages-boot.state")) SKIP("ages-boot.state missing");
  const char *limit_env = getenv("TAS_FRAMES");
  uint64_t limit = limit_env ? strtoull(limit_env, NULL, 10) : 20000;
  RefCheck rc = {ref, 0, 0, false, 0};
  rc.have = fscanf(ref, "%llu %llx", &rc.f, &rc.want) == 2;
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = ref_cb; gb->frame_ctx = &rc;
  for (uint64_t i = 0; i < limit && !rc.mismatch && !gb->hung; i++) gb_run_frame(gb);
  if (gb->hung) { fprintf(stderr, "native build stopped at frame %llu\n", (unsigned long long)GRID_FRAME(gb->cycles)); ASSERT(0); }
  if (rc.mismatch) { fprintf(stderr, "state mismatch at frame %llu\n", rc.mismatch); ASSERT(0); }
  fclose(ref); free(gb); free(rom); tas_free(&t);
}

int main(void) { RUN(native_full_tas_matches_reference); return 0; }
