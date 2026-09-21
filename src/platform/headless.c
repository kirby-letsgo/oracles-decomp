#include "core/gb.h"
#include "core/sha1.h"
#include "platform/png.h"
#include "platform/render.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "hooks/hooks.h"
#include "core/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REF_INTERVAL 60
extern uint64_t dbg_dma_bytes, dbg_dma_calls, dbg_hblank_chunks, dbg_instr_count, dbg_int_count[5];
extern uint32_t *dbg_pc_hist;
extern int dbg_log_dma, dbg_log_lcdc, dbg_log_ints;


static uint64_t tas_input_offset;
static uint8_t tas_input_cb(void *ctx, uint64_t frame) { const Tas *t = ctx; return t->count ? tas_input_at(t, frame + tas_input_offset) : 0; }

static void serial_print(void *ctx, uint8_t b) { (void)ctx; fputc(b, stdout); fflush(stdout); }

static const char *arg_value(int argc, char **argv, const char *name) {
  for (int i = 1; i + 1 < argc; i++) if (strcmp(argv[i], name) == 0) return argv[i + 1];
  return NULL;
}
static bool arg_flag(int argc, char **argv, const char *name) {
  for (int i = 1; i < argc; i++) if (strcmp(argv[i], name) == 0) return true;
  return false;
}

static bool screenshot_wanted(const char *list, uint64_t frame) {
  if (!list) return false;
  const char *p = list;
  while (*p) {
    char *end;
    unsigned long long v = strtoull(p, &end, 10);
    if (end == p) break;
    if (v == frame) return true;
    p = *end == ',' ? end + 1 : end;
  }
  return false;
}

typedef struct { uint8_t group, room; } RoomKey;

typedef struct {
  FILE *fh_out, *fh_check, *ref_out, *ref_check, *dump;
  const char *out_dir, *shot_at;
  uint64_t shot_every, input_offset;
  bool probe, fail;
  RoomKey seen[4096];
  int nseen;
} FrameCtx;

static void on_frame(GB *gb, const GBSample *sm, void *ctx) {
  FrameCtx *c = ctx;
  static uint8_t rgb[FB_W * FB_H * 3];
  uint64_t frame = sm->frame;
  if (c->fail) return;
  if (c->fh_out || c->fh_check) {
    uint64_t h = gb_frame_hash(gb);
    if (c->fh_out) fwrite(&h, 8, 1, c->fh_out);
    if (c->fh_check) {
      uint64_t want;
      if (fread(&want, 8, 1, c->fh_check) == 1 && want != h) {
        fprintf(stderr, "frame hash mismatch at frame %llu\n", (unsigned long long)frame);
        { c->fail = true; return; }
      }
    }
  }
  if ((c->ref_out || c->ref_check) && frame % REF_INTERVAL == 0) {
    uint64_t h = gb_state_hash(gb);
    if (c->ref_out) fprintf(c->ref_out, "%llu %016llx\n", (unsigned long long)frame, (unsigned long long)h);
    if (c->ref_check) {
      unsigned long long f, want;
      if (fscanf(c->ref_check, "%llu %llx", &f, &want) == 2 && (f != frame || want != h)) {
        fprintf(stderr, "state hash mismatch at frame %llu (ref frame %llu)\n", (unsigned long long)frame, f);
        { c->fail = true; return; }
      }
    }
  }
  if (c->out_dir && ((c->shot_every && frame % c->shot_every == 0) || screenshot_wanted(c->shot_at, frame))) {
    char path[1024];
    snprintf(path, sizeof path, "%s/frame_%07llu.png", c->out_dir, (unsigned long long)frame);
    framebuffer_to_rgb(sm->framebuffer, rgb);
    png_write_rgb(path, rgb, FB_W, FB_H);
  }
  if (getenv("SAMPLE_HASH_AT") && frame + c->input_offset == strtoull(getenv("SAMPLE_HASH_AT"), NULL, 10)) {
    printf("SAMPLE %llu wram %016llx hram %016llx vram %016llx oam %016llx io %016llx bg %016llx ob %016llx ie %02x rom %u ram %u\n", (unsigned long long)sm->frame,
           (unsigned long long)fnv1a64_update(FNV1A64_INIT, (const unsigned char *)sm->wram, sizeof sm->wram), (unsigned long long)fnv1a64_update(FNV1A64_INIT, sm->hram, sizeof sm->hram),
           (unsigned long long)fnv1a64_update(FNV1A64_INIT, (const unsigned char *)sm->vram, sizeof sm->vram), (unsigned long long)fnv1a64_update(FNV1A64_INIT, sm->oam, sizeof sm->oam),
           (unsigned long long)fnv1a64_update(FNV1A64_INIT, sm->io, sizeof sm->io), (unsigned long long)fnv1a64_update(FNV1A64_INIT, sm->bg_pal, sizeof sm->bg_pal), (unsigned long long)fnv1a64_update(FNV1A64_INIT, sm->ob_pal, sizeof sm->ob_pal), sm->ie, sm->rom_bank, sm->ram_bank);
    for (int i = 0; i < 128; i++) printf("%02x", sm->io[i]);
    printf("\n");
  }
  if (c->dump) {
    uint64_t movie_frame = frame + c->input_offset;
    static uint64_t wd_a = 0, wd_b = 0; static bool wd_init = false;
    if (!wd_init) { wd_init = true; if (getenv("WRAMDUMP_AT")) { if (sscanf(getenv("WRAMDUMP_AT"), "%llu-%llu", (unsigned long long *)&wd_a, (unsigned long long *)&wd_b) < 2) wd_b = wd_a; } }
    if (getenv("WRAMDUMP_AT") && movie_frame >= wd_a && movie_frame <= wd_b) {
      FILE *wf = fopen(getenv("WRAMDUMP_FILE") ? getenv("WRAMDUMP_FILE") : "wram_at.txt", "a");
      fprintf(wf, "FRAME %llu mcycles %llu cycles %llu ly %u\n", (unsigned long long)movie_frame, (unsigned long long)gb->mcycles, (unsigned long long)gb->cycles, gb->io[R_LY]);
      for (int b = 0; b < 8; b++) for (int i = 0; i < 4096; i++) fprintf(wf, "%02x", sm->wram[b][i]);
      fprintf(wf, "\n");
      for (int i = 0; i < 127; i++) fprintf(wf, "%02x", sm->hram[i]);
      fprintf(wf, "\n");
      for (size_t i = 0; i < sizeof sm->vram; i++) fprintf(wf, "%02x", ((const unsigned char *)sm->vram)[i]);
      fprintf(wf, "\n");
      for (size_t i = 0; i < sizeof sm->oam; i++) fprintf(wf, "%02x", sm->oam[i]);
      fprintf(wf, "\n");
      for (size_t i = 0; i < sizeof sm->io; i++) fprintf(wf, "%02x", sm->io[i]);
      fprintf(wf, "\n");
      for (size_t i = 0; i < sizeof sm->bg_pal; i++) fprintf(wf, "%02x", ((const unsigned char *)sm->bg_pal)[i]);
      for (size_t i = 0; i < sizeof sm->ob_pal; i++) fprintf(wf, "%02x", ((const unsigned char *)sm->ob_pal)[i]);
      fprintf(wf, " ie %02x rom_bank %u ram_bank %u\n", sm->ie, (unsigned)sm->rom_bank, (unsigned)sm->ram_bank);
      fclose(wf);
    }
    if ((movie_frame + 1) % 60 == 0) {
      uint32_t h = 0xcbf29ce4u;
      for (int i = 0x300; i < 0x1000; i++) { h ^= sm->wram[0][i]; h *= 16777619u; }
      fprintf(c->dump, "%llu %02x %02x %02x %02x %02x %d %08x\n", (unsigned long long)movie_frame, sm->wram[0][0xc2d], sm->wram[0][0xc30], sm->wram[0][0xc00], sm->hram[0x14], sm->hram[0x15], (sm->joy_latched || sm->joy_read) ? 0 : 1, h);
    } else {
      fprintf(c->dump, "%llu %02x %02x %02x %02x %02x %d\n", (unsigned long long)movie_frame, sm->wram[0][0xc2d], sm->wram[0][0xc30], sm->wram[0][0xc00], sm->hram[0x14], sm->hram[0x15], (sm->joy_latched || sm->joy_read) ? 0 : 1);
    }
  }
  if (c->probe) {
    RoomKey k = {sm->wram[0][0x0c2d], sm->wram[0][0x0c30]};
    bool known = false;
    for (int j = 0; j < c->nseen; j++) if (c->seen[j].group == k.group && c->seen[j].room == k.room) { known = true; break; }
    if (!known && c->nseen < 4096) {
      c->seen[c->nseen++] = k;
      printf("frame %llu group %02x room %02x\n", (unsigned long long)frame, k.group, k.room);
    }
  }
}

int main(int argc, char **argv) {
  const char *rom_path = arg_value(argc, argv, "--rom");
  if (!rom_path) { fprintf(stderr, "usage: oracles-run --rom FILE [options]\n"); return 2; }
  size_t rom_size, boot_size = 0;
  uint8_t *rom = oracles_read_file(rom_path, &rom_size);
  if (!rom) { fprintf(stderr, "cannot read %s\n", rom_path); return 2; }
  char hex[41];
  sha1_hex(rom, rom_size, hex);
  fprintf(stderr, "rom sha1 %s\n", hex);

  const char *p;
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "unsupported ROM\n"); return 2; }
  const char *boot_path = arg_value(argc, argv, "--boot");
  if (boot_path) {
    uint8_t *boot = oracles_read_file(boot_path, &boot_size);
    if (!boot) { fprintf(stderr, "cannot read %s\n", boot_path); return 2; }
    if (!arg_flag(argc, argv, "--no-agb-patch")) oracles_apply_agb_boot_patch(boot, boot_size);
    gb_set_boot_rom(gb, boot, boot_size);
  }
  const char *init_ram = arg_value(argc, argv, "--init-ram");
  if (arg_flag(argc, argv, "--serial")) gb->serial_out = serial_print;
  if (getenv("PCTRACE") || getenv("HOOKLOG")) setvbuf(stdout, NULL, _IOLBF, 0);
  hooks_init();
  static uint64_t il_a, il_b; if (getenv("INTLOG")) sscanf(getenv("INTLOG"), "%llu-%llu", (unsigned long long *)&il_a, (unsigned long long *)&il_b);
  if (arg_flag(argc, argv, "--no-hooks")) hook_mode = HOOK_MODE_OFF;
  if (arg_flag(argc, argv, "--verify-hooks")) hook_mode = HOOK_MODE_VERIFY;
  if (arg_flag(argc, argv, "--verify-hooks-continue")) { hook_mode = HOOK_MODE_VERIFY; hook_verify_abort = false; }
  if (arg_flag(argc, argv, "--log-lcdc")) dbg_log_lcdc = 1;
  if (init_ram && !oracles_load_init_ram(gb, init_ram)) { fprintf(stderr, "cannot read %s\n", init_ram); return 2; }
  gb_reset(gb);
  if ((p = arg_value(argc, argv, "--boot-ly"))) {
    int ly = atoi(p);
    gb->io[R_LY] = ly;
    gb->ppu_dot = 0;
    gb->ppu_mode = ly >= 144 ? 1 : 2;
    gb->io[R_STAT] = (gb->io[R_STAT] & 0xfc) | gb->ppu_mode;
  }
  if ((p = arg_value(argc, argv, "--boot-dot"))) gb->ppu_dot = atoi(p);
  if ((p = arg_value(argc, argv, "--boot-div"))) gb->div_counter = (uint16_t)strtoul(p, NULL, 0);
  if ((p = arg_value(argc, argv, "--boot-grid"))) gb->cycles += strtoull(p, NULL, 10);
  if (arg_flag(argc, argv, "--print-boot-cycles")) printf("boot cycles %llu\n", (unsigned long long)gb->cycles);

  Tas tas = {0};
  const char *tas_path = arg_value(argc, argv, "--tas");
  if (tas_path && !tas_load(&tas, tas_path)) { fprintf(stderr, "cannot read %s\n", tas_path); return 2; }

  const char *frames_arg = arg_value(argc, argv, "--frames");
  uint64_t max_frames = frames_arg ? strtoull(frames_arg, NULL, 10) : (tas.count ? tas.count + 120 : 600);
  const char *out_dir = arg_value(argc, argv, "--out");
  const char *shot_every_arg = arg_value(argc, argv, "--screenshot-every");
  uint64_t shot_every = shot_every_arg ? strtoull(shot_every_arg, NULL, 10) : 0;
  const char *shot_at = arg_value(argc, argv, "--screenshot-at");
  bool probe = arg_flag(argc, argv, "--probe");
  const char *offset_arg = arg_value(argc, argv, "--input-offset");
  uint64_t input_offset = offset_arg ? strtoull(offset_arg, NULL, 10) : 0;
  tas_input_offset = input_offset;

  FILE *ref_out = NULL, *fh_out = NULL, *ref_check = NULL, *fh_check = NULL;
  if ((p = arg_value(argc, argv, "--ref-out"))) ref_out = fopen(p, "w");
  if ((p = arg_value(argc, argv, "--ref-check"))) ref_check = fopen(p, "r");
  if ((p = arg_value(argc, argv, "--frame-hash-out"))) fh_out = fopen(p, "wb");
  if ((p = arg_value(argc, argv, "--frame-hash-check"))) fh_check = fopen(p, "rb");

  FILE *dump = NULL;
  if ((p = arg_value(argc, argv, "--dump"))) dump = fopen(p, "w");
  static FrameCtx fc;
  fc.fh_out = fh_out; fc.fh_check = fh_check; fc.ref_out = ref_out; fc.ref_check = ref_check; fc.dump = dump;
  fc.out_dir = out_dir; fc.shot_at = shot_at; fc.shot_every = shot_every; fc.input_offset = input_offset; fc.probe = probe;
  gb->frame_cb = on_frame; gb->frame_ctx = &fc;
  if (getenv("PCHIST")) dbg_pc_hist = calloc(128 << 15, sizeof *dbg_pc_hist);

  const char *trace_arg = arg_value(argc, argv, "--trace-frames");
  uint64_t trace_start = 0, trace_end = 0;
  if (trace_arg) sscanf(trace_arg, "%llu-%llu", (unsigned long long *)&trace_start, (unsigned long long *)&trace_end);
  if (getenv("GRID_OFFSET")) gb_grid_offset = atoll(getenv("GRID_OFFSET"));
  const char *anchor_arg = arg_value(argc, argv, "--anchors");
  uint64_t anchor_start = 0, anchor_end = 0; int anchor_every = 1;
  if (anchor_arg) sscanf(anchor_arg, "%llu-%llu/%d", (unsigned long long *)&anchor_start, (unsigned long long *)&anchor_end, &anchor_every);
  static uint8_t anchor_seen[65536]; int anchor_count = 0;
  gb->input_at = tas_input_cb; gb->input_ctx = &tas;
  for (uint64_t i = 0; i < max_frames; i++) {
    uint64_t frame = GRID_FRAME(gb->cycles);
    if (anchor_arg && frame + input_offset >= anchor_start && frame + input_offset <= anchor_end) {
      uint64_t target = GRID_END(frame + 1);
      dbg_log_dma = getenv("ANCHOR_DMA") != NULL;
      while (gb->cycles < target && !gb->hung) {
        uint16_t pc = gb->pc;
        if (!gb->halted && !(gb->ime && (gb->ie & gb->io[R_IF] & 0x1f)) && getenv("ANCHOR_ALL")) { const char *q = getenv("ANCHOR_ALL"); while (*q) { if (pc == strtol(q, NULL, 16)) { printf("ANCHOR %04x %llu bank %u ALL\n", pc, (unsigned long long)gb->mcycles, gb->rom_bank); break; } while (*q && *q != ' ') q++; while (*q == ' ') q++; } }
        if (!gb->halted && !anchor_seen[pc]) {
          anchor_seen[pc] = 1;
          if (anchor_count++ % anchor_every == 0) printf("ANCHOR %04x %llu bank %u\n", pc, (unsigned long long)gb->mcycles, gb->rom_bank);
        }
        static uint32_t stretch_hist[65536]; static uint64_t stretch_a = 0, stretch_b = 0; static bool stretch_done = false;
        if (getenv("STRETCH") && !stretch_a) sscanf(getenv("STRETCH"), "%llu-%llu", (unsigned long long *)&stretch_a, (unsigned long long *)&stretch_b);
        if (stretch_a && !gb->halted && gb->mcycles >= stretch_a && gb->mcycles <= stretch_b) stretch_hist[pc]++;
        static uint16_t ring[32]; static uint32_t ringb[32]; static int ringi = 0;
        if (!gb->halted) { ring[ringi & 31] = pc; ringb[ringi & 31] = gb->rom_bank; ringi++; }
        static uint64_t pr_a = 0, pr_b = 0;
        if (getenv("PCRANGE") && !pr_a) sscanf(getenv("PCRANGE"), "%llu-%llu", (unsigned long long *)&pr_a, (unsigned long long *)&pr_b);
        if (pr_a && gb->mcycles >= pr_a && gb->mcycles <= pr_b) printf("PC %04x@%u mc %llu%s%s a=%02x bc=%02x%02x hl=%02x%02x if=%02x\n", pc, gb->rom_bank, (unsigned long long)gb->mcycles, gb->halted ? " HALT" : "", (gb->ime && (gb->ie & gb->io[R_IF] & 0x1f)) ? " IRQ" : "", gb->a, gb->b, gb->c, gb->h, gb->l, gb->io[R_IF]), printf("  chk=%02x ime=%d ie=%02x\n", gb->wram[0][0x49d], gb->ime, gb->ie);
        if (getenv("CALLER_OF") && !gb->halted && pc == strtol(getenv("CALLER_OF"), NULL, 16) && stretch_a && gb->mcycles >= stretch_a && gb->mcycles <= stretch_b && ring[(ringi - 2) & 31] != 0x049c) {
          printf("%llu ", (unsigned long long)gb->mcycles);
          printf("CALLPATH bc=%02x%02x hl=%02x%02x de=%02x%02x:", gb->b, gb->c, gb->h, gb->l, gb->d, gb->e);
          for (int k = 31; k >= 0; k--) printf(" %04x@%u", ring[(ringi - 1 - k) & 31], ringb[(ringi - 1 - k) & 31]);
          printf("\n");
        }
        if (stretch_a && gb->mcycles > stretch_b && !stretch_done) {
          stretch_done = true;
          printf("STRETCH histogram:");
          for (int k = 0; k < 40; k++) { uint32_t best = 0; int bi = -1; for (int a = 0; a < 65536; a++) if (stretch_hist[a] > best) { best = stretch_hist[a]; bi = a; } if (bi < 0) break; printf(" %04x:%u", bi, best); stretch_hist[bi] = 0; }
          printf("\n");
        }
        gb_step(gb);
      }
      continue;
    }
    if (trace_arg && frame + input_offset >= trace_start && frame + input_offset <= trace_end) {
      uint64_t start = GRID_END(frame);
      uint64_t target = GRID_END(frame + 1);
      printf("%llu FRAME fc=%02x dma_bytes=%llu hblank_chunks=%llu instr=%llu ints=%llu/%llu/%llu\n", (unsigned long long)(frame + input_offset), gb->wram[0][0xc00],
             (unsigned long long)dbg_dma_bytes, (unsigned long long)dbg_hblank_chunks, (unsigned long long)dbg_instr_count, (unsigned long long)dbg_int_count[0], (unsigned long long)dbg_int_count[1], (unsigned long long)dbg_int_count[2]);
      dbg_log_dma = 1;
      static uint32_t hist[65536];
      memset(hist, 0, sizeof hist);
      uint64_t instr0 = dbg_instr_count;
      while (gb->cycles < target && !gb->hung) {
        uint16_t pc = gb->pc;
        if (!gb->halted && (pc == 0x40 || pc == 0x48 || pc == 0x50 || pc == 0x981 || pc == 0x984 || pc == 0x989))
          printf("%llu %04x %llu\n", (unsigned long long)(frame + input_offset), pc, (unsigned long long)((gb->cycles - start) / 2));
        if (!gb->halted && (pc == 0x2cd || pc == 0xe24 || pc == 0x477 || pc == 0x193e || pc == 0x5131 || pc == 0x510b || pc == 0x4d25 || pc == 0x56c0 || pc == 0x1957 || pc == 0xe58 || pc == 0x10a8) && !hist[pc])
          printf("%llu %04x %llu first\n", (unsigned long long)(frame + input_offset), pc, (unsigned long long)((gb->cycles - start) / 2));
        if (!gb->halted && getenv("TRACE_PC_SEQ") && !hist[pc] && (gb->cycles - start) / 2 >= (uint64_t)atoll(getenv("TRACE_PC_SEQ")))
          printf("%llu SEQ %04x %llu bank %u\n", (unsigned long long)(frame + input_offset), pc, (unsigned long long)((gb->cycles - start) / 2), gb->rom_bank);
        if (!gb->halted) hist[pc]++;
        gb_step(gb);
      }
      printf("%llu HIST instr=%llu\n", (unsigned long long)(frame + input_offset), (unsigned long long)(dbg_instr_count - instr0));
      for (int k = 0; k < 48; k++) {
        uint32_t best = 0; int bi = -1;
        for (int a = 0; a < 65536; a++) if (hist[a] > best) { best = hist[a]; bi = a; }
        if (bi < 0) break;
        printf("  %04x %u\n", bi, best); hist[bi] = 0;
      }
      dbg_log_dma = 0;
      continue;
    }
    if (getenv("INTLOG")) dbg_log_ints = (frame >= il_a && frame <= il_b);
    frame = gb_run_frame(gb);
    if (gb->hung) { fprintf(stderr, "cpu hung at frame %llu pc %04x\n", (unsigned long long)frame, gb->pc); return 1; }

    if (fc.fail) return 1;
  }
  if (probe) printf("distinct rooms: %d\n", fc.nseen);
  if (ref_out) fclose(ref_out);
  if (fh_out) fclose(fh_out);
  if (dump) fclose(dump);
  if (hook_mode != HOOK_MODE_OFF) hooks_report();
  if (dbg_pc_hist) { FILE *hf = fopen(getenv("PCHIST"), "wb"); fwrite(dbg_pc_hist, sizeof *dbg_pc_hist, 128 << 15, hf); fclose(hf); }
  if (gb->sample_overflow) fprintf(stderr, "sample ring overflow: %llu frames dropped\n", (unsigned long long)gb->sample_overflow);
  printf("done: %llu frames, state %016llx\n", (unsigned long long)GRID_FRAME(gb->cycles),
         (unsigned long long)gb_state_hash(gb));
  return 0;
}
