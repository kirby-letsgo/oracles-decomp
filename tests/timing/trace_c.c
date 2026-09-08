#include "core/gb.h"
#include <stdio.h>
#include <stdlib.h>
static uint8_t *read_all(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); uint8_t *b = malloc(*n); fread(b, 1, *n, f); fclose(f); return b; }
int main(int argc, char **argv) {
  size_t rn, bn; uint8_t *rom = read_all(argv[1], &rn); uint8_t *boot = read_all(argv[2], &bn);
  GB *gb = calloc(1, sizeof *gb); gb_init(gb); gb_load_rom(gb, rom, rn); gb_set_boot_rom(gb, boot, bn); gb_reset(gb);
  int samples = 0; uint64_t c0 = 0;
  uint64_t guard = 0;
  while (samples < 100 && !gb->hung && guard++ < 400000000ULL) {
    uint16_t pc = gb->pc;
    if (pc >= 0x150 && pc < 0x400 && rom[pc] == 0x22 && rom[pc - 2] == 0xf0 && rom[pc - 1] == 0x41) {
      if (!c0) c0 = gb->cycles;
      printf("s%02d dots=%llu LY=%d dot=%d mode=%d lcdc=%02x ds=%d\n", samples, (unsigned long long)(gb->cycles - c0), gb->io[R_LY], gb->ppu_dot, gb->ppu_mode, gb->io[R_LCDC], gb->double_speed);
      samples++;
    }
    gb_step(gb);
  }
  return 0;
}
