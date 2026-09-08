// Runs our core and SameBoy side by side on the same inputs and reports the first frame where WRAM differs.
// Build: clang -O2 -Isrc -I<SameBoy> tools/lockstep.c build/liboraclescore.a <SameBoy>/build/lib/libsameboy.a -o build/lockstep
#define GB_INTERNAL
#include SAMEBOY_GB_H
#include "core/gb.h"
#include "platform/tas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t pixels[160 * 144];
static uint32_t rgb_encode(GB_gameboy_t *g, uint8_t r, uint8_t gg, uint8_t b) { return (r << 16) | (gg << 8) | b; }
static void vblank(GB_gameboy_t *g, GB_vblank_type_t t) {}

static uint8_t *read_all(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
  uint8_t *buf = malloc(n); fread(buf, 1, n, f); fclose(f); *size = n; return buf;
}

extern uint64_t dbg_instr_count;
extern int dbg_log_lcdc;
static bool hist_on;
extern int dbg_log_timer_writes;
extern int dbg_mode0_dot[154], dbg_mode3_len[154];
static int sb_mode0_cyc[154];
static uint64_t g_t0;
static bool sb_write_cb(GB_gameboy_t *g, uint16_t addr, uint8_t v) {
  if (hist_on && (addr == 0xff04 || addr == 0xff05 || addr == 0xff07)) printf("  SB   write %04x=%02x at cycle %llu div=%04x TIMA=%02x\n", addr, v, (unsigned long long)((g->absolute_debugger_ticks - g_t0) / 2), g->div_counter, g->io_registers[5]);
  if (addr == 0xff40 && hist_on) printf("  SB   lcdc write %02x at LY=%d pc=%04x ticks=%llu\n", v, g->current_line, GB_get_registers(g)->pc, (unsigned long long)g->absolute_debugger_ticks); return true; }
static uint64_t sb_instr;
static uint32_t sb_hist[65536], our_hist[65536];
extern int dbg_log_ints;
static void exec_cb(GB_gameboy_t *g, uint16_t addr, uint8_t op) { sb_instr++; if (hist_on) { sb_hist[addr]++; if (addr == 0x50 || addr == 0x48 || addr == 0x40) printf("  SB   int vec %02x at cycle %llu (LY=%d) TIMA=%02x TMA=%02x TAC=%02x div=%04x\n", addr, (unsigned long long)((g->absolute_debugger_ticks - g_t0) / 2), g->current_line, g->io_registers[5], g->io_registers[6], g->io_registers[7], g->div_counter); } }
static void print_top(const char *name, uint32_t *hist) {
  printf("%s top pcs:", name);
  for (int k = 0; k < 48; k++) {
    uint32_t best = 0; int bi = -1;
    for (int i = 0; i < 65536; i++) if (hist[i] > best) { best = hist[i]; bi = i; }
    if (bi < 0) break;
    printf(" %04x(%u)", bi, best); hist[bi] = 0;
  }
  printf("\n");
}

static void sb_keys(GB_gameboy_t *g, uint8_t in) {
  GB_set_key_state(g, GB_KEY_A, in & 1); GB_set_key_state(g, GB_KEY_B, in & 2);
  GB_set_key_state(g, GB_KEY_SELECT, in & 4); GB_set_key_state(g, GB_KEY_START, in & 8);
  GB_set_key_state(g, GB_KEY_RIGHT, in & 16); GB_set_key_state(g, GB_KEY_LEFT, in & 32);
  GB_set_key_state(g, GB_KEY_UP, in & 64); GB_set_key_state(g, GB_KEY_DOWN, in & 128);
}

static uint64_t play_hash(const uint8_t *w0, const uint8_t *w1, const uint8_t *w2, const uint8_t *w3, const uint8_t *w4, const uint8_t *w5, const uint8_t *w6, const uint8_t *w7) {
  const uint8_t *banks[8] = {w0, w1, w2, w3, w4, w5, w6, w7};
  uint64_t h = 0xcbf29ce484222325ULL;
  for (int b = 0; b < 8; b++) for (int i = 0; i < 4096; i++) {
    if (b == 0 && i < 0x2e0) continue;
    if (b == 2 && i >= 0xf80) continue;
    h ^= banks[b][i]; h *= 0x100000001b3ULL;
  }
  return h;
}

int main(int argc, char **argv) {
  if (argc < 7) { fprintf(stderr, "usage: lockstep ROM AGB_BOOT INPUTS OUR_OFFSET SB_BOOT_FRAMES MAX_FRAMES [START_REPORT]\n"); return 2; }
  const char *rom_path = argv[1], *boot = argv[2], *inputs = argv[3];
  long our_offset = atol(argv[4]), sb_boot_frames = atol(argv[5]), max_frames = atol(argv[6]);
  long report_from = argc > 7 ? atol(argv[7]) : 0;
  size_t rom_size; uint8_t *rom = read_all(rom_path, &rom_size);
  Tas tas; if (!tas_load(&tas, inputs)) return 2;

  GB *ours = calloc(1, sizeof *ours); gb_init(ours); gb_load_rom(ours, rom, rom_size); gb_reset(ours);
  GB_gameboy_t *sb = GB_alloc(); GB_init(sb, GB_MODEL_AGB);
  GB_set_pixels_output(sb, pixels); GB_set_rgb_encode_callback(sb, rgb_encode); GB_set_vblank_callback(sb, vblank);
  GB_set_execution_callback(sb, exec_cb);
  GB_set_write_memory_callback(sb, sb_write_cb);
  if (GB_load_boot_rom(sb, boot)) return 2;
  GB_load_rom_from_buffer(sb, rom, rom_size);
  size_t sz; uint8_t *sb_ram = GB_get_direct_access(sb, GB_DIRECT_ACCESS_RAM, &sz, NULL);
  uint8_t *sb_hram = GB_get_direct_access(sb, GB_DIRECT_ACCESS_HRAM, &sz, NULL);
  memset(sb_ram, 0, 32768);
  memset(sb_hram, 0, 127);
  while (!sb->boot_rom_finished) GB_run(sb);
  uint64_t t0 = sb->absolute_debugger_ticks;
  g_t0 = t0;
  fprintf(stderr, "sameboy boot handoff at tick %llu\n", (unsigned long long)t0);
  {
    GB_registers_t *r = GB_get_registers(sb);
    fprintf(stderr, "handoff regs af=%04x bc=%04x de=%04x hl=%04x sp=%04x pc=%04x ime=%d\n", r->af, r->bc, r->de, r->hl, r->sp, r->pc, sb->ime);
    fprintf(stderr, "handoff ppu LY=%d cycles_for_line=%d position_in_line=%d display_state=%d STAT=%02x LCDC=%02x\n",
            sb->current_line, sb->cycles_for_line, sb->position_in_line, sb->display_state, sb->io_registers[0x41], sb->io_registers[0x40]);
    fprintf(stderr, "handoff timer div_counter=%04x DIV=%02x TIMA=%02x TMA=%02x TAC=%02x IF=%02x IE=%02x KEY1=%02x\n",
            sb->div_counter, GB_read_memory(sb, 0xff04), sb->io_registers[0x05], sb->io_registers[0x06], sb->io_registers[0x07], sb->io_registers[0x0f], GB_read_memory(sb, 0xffff), sb->io_registers[0x4d]);
    fprintf(stderr, "handoff io:");
    for (int i = 0; i < 0x80; i++) fprintf(stderr, " %02x", sb->io_registers[i]);
    fprintf(stderr, "\n");
    if (getenv("LOCKSTEP_COPY_HANDOFF")) {
      ours->a = r->af >> 8; ours->f = r->af & 0xf0; ours->b = r->bc >> 8; ours->c = r->bc; ours->d = r->de >> 8; ours->e = r->de;
      ours->h = r->hl >> 8; ours->l = r->hl; ours->sp = r->sp; ours->pc = r->pc;
      ours->io[R_LY] = sb->current_line; ours->ppu_dot = sb->cycles_for_line;
      ours->div_counter = sb->div_counter;
      ours->io[R_IF] = sb->io_registers[0x0f] & 0x1f;
      for (int i = 0x40; i < 0x4c; i++) if (i != 0x44) ours->io[i] = sb->io_registers[i];
      memcpy(ours->vram[0], GB_get_direct_access(sb, GB_DIRECT_ACCESS_VRAM, &sz, NULL), 16384);
      memcpy(ours->bg_pal, GB_get_direct_access(sb, GB_DIRECT_ACCESS_BGP, &sz, NULL), 64);
      memcpy(ours->ob_pal, GB_get_direct_access(sb, GB_DIRECT_ACCESS_OBP, &sz, NULL), 64);
      memcpy(ours->hram, sb_hram, 127);
      for (int i = 0x10; i < 0x40; i++) if (i != 0x26) { }
      ours->ppu_mode = sb->io_registers[0x41] & 3;
      ours->io[R_STAT] = sb->io_registers[0x41] & 0x7f;
      fprintf(stderr, "copied handoff state into our core\n");
    }
  }
  long sb_frame = 0;
  for (long f = 0; f < max_frames; f++) {
    uint8_t in = tas_input_at(&tas, f + our_offset);
    ours->joy_pending = in; ours->joy = in;
    long hist_from = argc > 9 ? atol(argv[9]) : -1;
    hist_on = hist_from >= 0 && f >= hist_from && f < hist_from + (argc > 10 ? atol(argv[10]) : 40);
    dbg_log_lcdc = hist_on; dbg_log_ints = hist_on; dbg_log_timer_writes = hist_on;
    uint64_t oi0 = dbg_instr_count, si0 = sb_instr;
    if (hist_on) {
      uint64_t target = (ours->cycles / FRAME_CYCLES + 1) * FRAME_CYCLES;
      ours->joy_latched = false;
      static int last_ly = -1; static long it = 0;
      while (ours->cycles < target && !ours->hung) {
        our_hist[ours->pc]++;
        if (ours->pc == 0x2cd) { it++; int ly = ours->io[R_LY]; if (ly < last_ly || it % 400 == 1 || ly >= 0x91) printf("  loop f=%ld it=%ld LY=%d LCDC=%02x STAT=%02x dot=%d IE=%02x IF=%02x\n", f, it, ly, ours->io[R_LCDC], ours->io[R_STAT], ours->ppu_dot, ours->ie, ours->io[R_IF]); last_ly = ly; }
        gb_step(ours);
      }
      if (!ours->joy_latched) ours->joy = ours->joy_pending;
    } else gb_run_frame(ours);
    sb_keys(sb, in);
    uint64_t sb_target = t0 + (uint64_t)(f + 1) * FRAME_CYCLES * 2;
    long mode_frame = getenv("LOCKSTEP_MODE_FRAME") ? atol(getenv("LOCKSTEP_MODE_FRAME")) : -1;
    if (f == mode_frame) {
      int prev_mode = GB_read_memory(sb, 0xff41) & 3; int prev_line = sb->current_line; uint64_t line_start = sb->absolute_debugger_ticks;
      while (sb->absolute_debugger_ticks < sb_target) {
        GB_run(sb);
        if (sb->current_line != prev_line) { prev_line = sb->current_line; line_start = sb->absolute_debugger_ticks; }
        int m = GB_read_memory(sb, 0xff41) & 3;
        if (m == 0 && prev_mode == 3 && sb->current_line < 154) sb_mode0_cyc[sb->current_line] = (int)((sb->absolute_debugger_ticks - line_start) / 2);
        prev_mode = m;
      }
      printf("mode0 entry per line (ours dot / ours mode3len / sb cycles_for_line):\n");
      for (int l = 0; l < 144; l++) printf("  ly %3d: %3d %3d %3d\n", l, dbg_mode0_dot[l], dbg_mode3_len[l], sb_mode0_cyc[l]);
    } else {
      while (sb->absolute_debugger_ticks < sb_target) GB_run(sb);
    }
    sb_frame++;
    if (hist_on) {
      printf("frame %ld instr ours %llu sb %llu\n", f, (unsigned long long)(dbg_instr_count - oi0), (unsigned long long)(sb_instr - si0));
      print_top("  ours", our_hist); print_top("  sb  ", sb_hist);
      memset(our_hist, 0, sizeof our_hist); memset(sb_hist, 0, sizeof sb_hist);
    }
    if (argc > 8 && f >= atol(argv[8]) && f < atol(argv[8]) + 150)
      printf("trace f=%ld sbf=%ld fc=%02x/%02x c49e=%02x/%02x c4af=%02x/%02x cbb3=%02x/%02x cbb7=%02x/%02x lcdc=%02x/%02x\n", f, sb_frame,
             ours->wram[0][0xc00], sb_ram[0xc00], ours->wram[0][0x49e], sb_ram[0x49e], ours->wram[0][0x4af], sb_ram[0x4af],
             ours->wram[0][0xbb3], sb_ram[0xbb3], ours->wram[0][0xbb7], sb_ram[0xbb7], ours->io[R_LCDC], GB_read_memory(sb, 0xff40));
    int diffs = 0, first = -1;
    for (int b = 0; b < 8; b++) for (int i = 0; i < 4096; i++) {
      if (b == 0 && i < 0x2e0) continue;
      if (b == 2 && i >= 0xf80) continue;
      if (ours->wram[b][i] != sb_ram[b * 4096 + i]) { if (first < 0) first = b * 4096 + i; diffs++; }
    }
    int hdiffs = 0;
    for (int i = 0; i < 127; i++) if (ours->hram[i] != sb_hram[i]) hdiffs++;
    if (f == report_from) {
      printf("hram diffs at frame %ld:", f);
      for (int i = 0; i < 127; i++) if (ours->hram[i] != sb_hram[i]) printf(" ff%02x=%02x/%02x", 0x80 + i, ours->hram[i], sb_hram[i]);
      printf("\n");
    }
    static int streak = 0; static long streak_start = -1;
    if (diffs) { if (streak == 0) streak_start = f; streak++; } else streak = 0;
    if (f >= report_from && diffs && streak >= 10) {
      printf("persistent divergence since frame %ld\n", streak_start);
      printf("frame %ld: wram diffs %d (first bank %d addr %04x ours %02x sb %02x) hram diffs %d | ours pc %04x sb pc %04x\n",
             f, diffs, first / 4096, (first % 4096) + (first / 4096 ? 0xd000 : 0xc000), ours->wram[first / 4096][first % 4096],
             sb_ram[first], hdiffs, ours->pc, GB_get_registers(sb)->pc);
      if (diffs > 0) {
        int shown = 0;
        for (int b = 0; b < 8 && shown < 200; b++) for (int i = 0; i < 4096 && shown < 200; i++)
          if (!(b == 0 && i < 0x2e0) && !(b == 2 && i >= 0xf80) && ours->wram[b][i] != sb_ram[b * 4096 + i]) { printf(" %d:%04x=%02x/%02x", b, i + (b ? 0xd000 : 0xc000), ours->wram[b][i], sb_ram[b * 4096 + i]); shown++; }
        return 1;
      }
    }
    if (f % 5000 == 0) { fprintf(stderr, "frame %ld ok\n", f); }
  }
  printf("no divergence in %ld frames\n", max_frames);
  return 0;
}
