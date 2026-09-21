// Runs the reference engine (interpreter with hooks on) and the native engine in one process on
// the same inputs and reports the first frame whose masked state differs: the first differing
// byte and both sides' last 64 dispatch records. The native side starts from the reference's
// post-boot state, exactly as oracles-native-run starts from tas/ages-boot.state.
//
// usage: lockstep-native ROM BOOT INIT_RAM INPUTS MAX_FRAMES [REPORT_FROM]
//   NATIVE_TRACE=a-b   print every dispatch of both sides in frames a..b (REF/NAT prefixed)
//   LOCKSTEP_PERTURB=f flip one WRAM bit on the native side at grid frame f (self-test of the report)
#include "core/gb.h"
#include "hooks/hooks.h"
#include "platform/setup.h"
#include "platform/tas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void native_gb_step(GB *gb);

static uint8_t input_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

static void print_ring(const char *side, const DispatchRing *ring) {
  printf("%s last dispatches (oldest first):\n", side);
  unsigned n = ring->n < 64 ? ring->n : 64;
  for (unsigned i = 0; i < n; i++) {
    const DispatchRec *r = &ring->r[(ring->n - n + i) % 64];
    printf("  %02x:%04x %-44s mc %llu\n", r->bank, r->addr, r->name, (unsigned long long)r->mcycles);
  }
}

typedef struct { const char *name; size_t off, len, skip_lo, skip_hi; } Region;

static bool find_diff(const GBSample *a, const GBSample *b, const char **what, size_t *at, uint8_t *va, uint8_t *vb) {
  const Region regions[] = {
    {"wram", offsetof(GBSample, wram), sizeof a->wram, GB_W0_SKIP_END, 0},
    {"hram", offsetof(GBSample, hram), sizeof a->hram, 0, 0},
    {"vram", offsetof(GBSample, vram), sizeof a->vram, 0, 0},
    {"oam", offsetof(GBSample, oam), sizeof a->oam, 0, 0},
    {"io", offsetof(GBSample, io), sizeof a->io, 0, 0},
    {"bg_pal", offsetof(GBSample, bg_pal), sizeof a->bg_pal, 0, 0},
    {"ob_pal", offsetof(GBSample, ob_pal), sizeof a->ob_pal, 0, 0},
  };
  for (size_t r = 0; r < sizeof regions / sizeof regions[0]; r++) {
    const uint8_t *x = (const uint8_t *)a + regions[r].off, *y = (const uint8_t *)b + regions[r].off;
    for (size_t i = regions[r].skip_lo; i < regions[r].len; i++) {
      if (strcmp(regions[r].name, "wram") == 0 && i >= GB_SOUND_BANK * 4096 + GB_SOUND_BANK_LO && i < (GB_SOUND_BANK + 1) * 4096) continue;
      if (strcmp(regions[r].name, "hram") == 0 && i >= GB_HRAM_MUSIC_LO && i < GB_HRAM_MUSIC_HI) continue;
      if (x[i] != y[i]) { *what = regions[r].name; *at = i; *va = x[i]; *vb = y[i]; return true; }
    }
  }
  if (a->ie != b->ie) { *what = "ie"; *at = 0; *va = a->ie; *vb = b->ie; return true; }
  if (a->rom_bank != b->rom_bank) { *what = "rom_bank"; *at = 0; *va = (uint8_t)a->rom_bank; *vb = (uint8_t)b->rom_bank; return true; }
  if (a->ram_bank != b->ram_bank) { *what = "ram_bank"; *at = 0; *va = a->ram_bank; *vb = b->ram_bank; return true; }
  return false;
}

int main(int argc, char **argv) {
  if (argc < 6) { fprintf(stderr, "usage: lockstep-native ROM BOOT INIT_RAM INPUTS MAX_FRAMES [REPORT_FROM]\n"); return 2; }
  size_t rom_size, boot_size;
  uint8_t *rom = oracles_read_file(argv[1], &rom_size);
  uint8_t *boot = oracles_read_file(argv[2], &boot_size);
  if (!rom || !boot) { fprintf(stderr, "cannot read ROM or boot ROM\n"); return 2; }
  oracles_apply_agb_boot_patch(boot, boot_size);
  Tas tas;
  if (!tas_load(&tas, argv[4])) { fprintf(stderr, "cannot read %s\n", argv[4]); return 2; }
  long max_frames = atol(argv[5]), report_from = argc > 6 ? atol(argv[6]) : 0;
  uint64_t trace_lo = 0, trace_hi = 0;
  if (getenv("NATIVE_TRACE")) sscanf(getenv("NATIVE_TRACE"), "%llu-%llu", (unsigned long long *)&trace_lo, (unsigned long long *)&trace_hi);

  GB *ref = calloc(1, sizeof *ref), *nat = calloc(1, sizeof *nat);
  gb_init(ref); gb_init(nat);
  if (!gb_load_rom(ref, rom, rom_size) || !gb_load_rom(nat, rom, rom_size)) { fprintf(stderr, "unsupported ROM\n"); return 2; }
  gb_set_boot_rom(ref, boot, boot_size);
  if (!oracles_load_init_ram(ref, argv[3])) { fprintf(stderr, "cannot read %s\n", argv[3]); return 2; }
  hooks_init();
  gb_reset(ref);
  gb_reset(nat);
  nat->step = native_gb_step;
  nat->native = true;
  oracles_copy_state(nat, ref);
  ref->ring = calloc(1, sizeof *ref->ring); nat->ring = calloc(1, sizeof *nat->ring);
  ref->trace_lo = nat->trace_lo = trace_lo; ref->trace_hi = nat->trace_hi = trace_hi;
  ref->input_at = nat->input_at = input_cb; ref->input_ctx = nat->input_ctx = &tas;

  long perturb = getenv("LOCKSTEP_PERTURB") ? atol(getenv("LOCKSTEP_PERTURB")) : -1;
  for (long f = 0; f < max_frames; f++) {
    if (perturb >= 0 && (long)GRID_FRAME(nat->cycles) == perturb) { nat->wram[1][0x100] ^= 1; perturb = -1; }
    uint64_t fr = gb_run_frame(ref), fn = gb_run_frame(nat);
    if (nat->hung) { printf("native engine stopped in frame %llu (pc %04x, sp %04x, mc %llu)\n", (unsigned long long)fn, nat->pc, nat->sp, (unsigned long long)nat->mcycles); print_ring("REF", ref->ring); print_ring("NAT", nat->ring); return 1; }
    if (ref->hung) { printf("reference engine stopped in frame %llu (pc %04x)\n", (unsigned long long)fr, ref->pc); return 1; }
    if (fr != fn) { printf("frame numbering diverged: ref %llu native %llu\n", (unsigned long long)fr, (unsigned long long)fn); return 1; }
    const char *what; size_t at; uint8_t va, vb;
    if ((long)fr >= report_from && find_diff(ref->sample, nat->sample, &what, &at, &va, &vb)) {
      printf("frame %llu: first difference in %s at offset %04zx: ref %02x native %02x (ref mc %llu pc %04x sp %04x; native mc %llu pc %04x sp %04x)\n",
             (unsigned long long)fr, what, at, va, vb, (unsigned long long)ref->mcycles, ref->pc, ref->sp, (unsigned long long)nat->mcycles, nat->pc, nat->sp);
      print_ring("REF", ref->ring); print_ring("NAT", nat->ring);
      return 1;
    }
    if (f % 5000 == 0 && f) fprintf(stderr, "frame %ld ok\n", f);
  }
  printf("no divergence in %ld frames (ref state %016llx, native state %016llx)\n", max_frames, (unsigned long long)gb_state_hash(ref), (unsigned long long)gb_state_hash(nat));
  return 0;
}
