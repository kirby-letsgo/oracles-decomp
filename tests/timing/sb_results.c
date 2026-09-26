// SameBoy's side of the timing ROMs: runs a ROM for 200 frames and prints WRAM C000..C0BF in the
// format of read_results.c (ours) and tools/bizhawk/lua/dump_timing.lua (GBHawk).
// Build: clang -O2 -I<SameBoy> -DSAMEBOY_GB_H='"Core/gb.h"' tests/timing/sb_results.c <SameBoy>/build/lib/libsameboy.a -o sb_results
// usage: sb_results ROM BOOTROM [PC...]
// With PCs (hex) it prints the first three executions of each instead, with SameBoy's clock in
// debugger ticks; calibrate with a known run of nops (the GD ROMs have 16 at 0x190..0x1a0).
#define GB_INTERNAL
#include SAMEBOY_GB_H
#include <stdio.h>
#include <stdlib.h>

static uint32_t pixels[160 * 144];
static uint32_t rgb_encode(GB_gameboy_t *g, uint8_t r, uint8_t gg, uint8_t b) { return (r << 16) | (gg << 8) | b; }
static void vblank(GB_gameboy_t *g, GB_vblank_type_t t) {}

static int n_pcs, hits[16];
static uint16_t pcs[16];
static void exec_cb(GB_gameboy_t *g, uint16_t addr, uint8_t opcode) {
  for (int i = 0; i < n_pcs; i++)
    if (addr == pcs[i] && hits[i] < 3) { hits[i]++; printf("SB pc %04x #%d at %llu ticks\n", addr, hits[i], (unsigned long long)g->absolute_debugger_ticks); }
}

static uint8_t *read_all(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
  uint8_t *buf = malloc(n); if (fread(buf, 1, n, f) != (size_t)n) { fclose(f); return NULL; }
  fclose(f); *size = n; return buf;
}

int main(int argc, char **argv) {
  if (argc < 3) { fprintf(stderr, "usage: sb_results ROM BOOTROM\n"); return 2; }
  size_t rom_size;
  uint8_t *rom = read_all(argv[1], &rom_size);
  if (!rom) return 2;
  GB_gameboy_t *sb = GB_alloc(); GB_init(sb, GB_MODEL_AGB);
  GB_set_pixels_output(sb, pixels); GB_set_rgb_encode_callback(sb, rgb_encode); GB_set_vblank_callback(sb, vblank);
  if (GB_load_boot_rom(sb, argv[2])) return 2;
  GB_load_rom_from_buffer(sb, rom, rom_size);
  for (int i = 3; i < argc && n_pcs < 16; i++) pcs[n_pcs++] = (uint16_t)strtol(argv[i], NULL, 16);
  if (n_pcs) GB_set_execution_callback(sb, exec_cb);
  int frames = getenv("FRAMES") ? atoi(getenv("FRAMES")) : 200;
  for (int i = 0; i < frames; i++) GB_run_frame(sb);
  if (n_pcs) return 0;
  size_t size; uint16_t bank;
  uint8_t *ram = GB_get_direct_access(sb, GB_DIRECT_ACCESS_RAM, &size, &bank);
  for (int i = 0; i < 0xc0; i++) printf("%02x%s", ram[i], (i % 16 == 15) ? "\n" : " ");
  return 0;
}
