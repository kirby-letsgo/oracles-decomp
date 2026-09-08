#include "core/gb.h"
#include "core/bus.h"
#include <stdio.h>
#include <stdlib.h>
static uint8_t *read_all(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); uint8_t *b = malloc(*n); fread(b, 1, *n, f); fclose(f); return b; }
int main(int argc, char **argv) {
  size_t rn, bn; uint8_t *rom = read_all(argv[1], &rn); uint8_t *boot = read_all(argv[2], &bn);
  static const uint8_t gba_override[13] = {0xff, 0x00, 0xcd, 0x03, 0x35, 0xaa, 0x31, 0x90, 0x94, 0, 0, 0, 0};
  for (int i = 0; i < 13; i++) boot[0xf3 + i] = (uint8_t)(boot[0xf3 + i] + gba_override[i]);
  GB *gb = calloc(1, sizeof *gb); gb_init(gb); gb_load_rom(gb, rom, rn); gb_set_boot_rom(gb, boot, bn); gb_reset(gb);
  uint64_t t_lcdc = 0, t_stop = 0; int n200 = 0, n280 = 0, n300 = 0; uint64_t guard = 0;
  while (guard++ < 200000000ULL && (n200 < 4 || n280 < 8)) {
    uint16_t pc = gb->pc;
    if (!t_stop && !gb->halted && rom[pc] == 0x10 && pc >= 0x150) { t_stop = gb->cycles; printf("OURS stop at cycle %llu\n", (unsigned long long)t_stop); }
    uint64_t ref = t_lcdc ? t_lcdc : t_stop;
    if (ref && !gb->halted && pc == 0x300 && n300 < 6) { n300++; printf("OURS timer handler #%d at +%lld M-cycles\n", n300, (long long)(gb->cycles - ref) / 2); }
    if (!t_lcdc && !gb->halted && rom[pc] == 0xe0 && rom[pc + 1] == 0x40 && gb->a == 0x91 && pc > 0x160) {
      gb_step(gb); t_lcdc = gb->cycles; printf("OURS lcdc=91 write at cycle %llu LY=%d\n", (unsigned long long)t_lcdc, gb->io[R_LY]); continue;
    }
    if (ref && !gb->halted && pc == 0x200 && n200 < 4) { n200++; printf("OURS vblank handler #%d at +%lld M-cycles (LY=%d)\n", n200, (long long)(gb->cycles - ref) / 2, gb->io[R_LY]); }
    if (ref && !gb->halted && pc == 0x280 && n280 < 8) { n280++; printf("OURS stat handler #%d at +%lld M-cycles (LY=%d)\n", n280, (long long)(gb->cycles - ref) / 2, gb->io[R_LY]); }
    gb_step(gb);
  }
  return 0;
}
