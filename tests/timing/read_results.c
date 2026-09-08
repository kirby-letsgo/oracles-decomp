#include "core/gb.h"
#include <stdio.h>
#include <stdlib.h>
static uint8_t *read_all(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); uint8_t *b = malloc(*n); fread(b, 1, *n, f); fclose(f); return b; }
int main(int argc, char **argv) {
  size_t rn, bn; uint8_t *rom = read_all(argv[1], &rn); uint8_t *boot = read_all(argv[2], &bn);
  static const uint8_t gba_override[13] = {0xff, 0x00, 0xcd, 0x03, 0x35, 0xaa, 0x31, 0x90, 0x94, 0, 0, 0, 0};
  for (int i = 0; i < 13; i++) boot[0xf3 + i] = (uint8_t)(boot[0xf3 + i] + gba_override[i]);
  GB *gb = calloc(1, sizeof *gb); gb_init(gb); gb_load_rom(gb, rom, rn); gb_set_boot_rom(gb, boot, bn); gb_reset(gb);
  for (int i = 0; i < 200; i++) gb_run_frame(gb);
  fprintf(stderr, "pc=%04x ie=%02x if=%02x ime=%d halted=%d lcdc=%02x ly=%d stat=%02x\n", gb->pc, gb->ie, gb->io[R_IF], gb->ime, gb->halted, gb->io[R_LCDC], gb->io[R_LY], gb->io[R_STAT]);
  for (int i = 0; i < 0xc0; i++) printf("%02x%s", gb->wram[0][i], (i % 16 == 15) ? "\n" : " ");
  return 0;
}
