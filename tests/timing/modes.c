#include "core/gb.h"
#include "core/bus.h"
#include <stdio.h>
#include <stdlib.h>
static uint8_t *read_all(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); uint8_t *b = malloc(*n); fread(b, 1, *n, f); fclose(f); return b; }
int main(int argc, char **argv) {
  size_t rn, bn; uint8_t *rom = read_all(argv[1], &rn); uint8_t *boot = read_all(argv[2], &bn);
  GB *gb = calloc(1, sizeof *gb); gb_init(gb); gb_load_rom(gb, rom, rn); gb_set_boot_rom(gb, boot, bn); gb_reset(gb);
  uint64_t t_en = 0; int prev = -1; uint64_t guard = 0;
  while (guard++ < 100000000ULL) {
    uint16_t pc = gb->pc;
    if (!t_en && !gb->halted && rom[pc] == 0xe0 && rom[pc + 1] == 0x40 && gb->a == 0x91 && pc > 0x160) { gb_step(gb); t_en = gb->cycles; continue; }
    if (t_en) {
      int ly = bus_read(gb, 0xff44), st = bus_read(gb, 0xff41) & 7; int cur = (ly << 8) | st;
      if (cur != prev) { printf("OURS dot %4lld: LY=%d STAT=mode%d lyc=%d\n", (long long)(gb->cycles - t_en), ly, st & 3, (st >> 2) & 1); prev = cur; if (ly >= 3) break; }
    }
    gb_step(gb);
  }
  return 0;
}
