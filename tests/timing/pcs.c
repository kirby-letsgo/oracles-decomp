#include "core/gb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
static uint8_t *read_all(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); uint8_t *b = malloc(*n); fread(b, 1, *n, f); fclose(f); return b; }
int main(int argc, char **argv) {
  size_t rn, bn; uint8_t *rom = read_all(argv[1], &rn); uint8_t *boot = read_all(argv[2], &bn);
  static const uint8_t gba_override[13] = {0xff, 0x00, 0xcd, 0x03, 0x35, 0xaa, 0x31, 0x90, 0x94, 0, 0, 0, 0};
  for (int i = 0; i < 13; i++) boot[0xf3 + i] = (uint8_t)(boot[0xf3 + i] + gba_override[i]);
  GB *gb = calloc(1, sizeof *gb); gb_init(gb); gb_load_rom(gb, rom, rn); gb_set_boot_rom(gb, boot, bn); gb_reset(gb);
  int n = argc - 3; int hits[16] = {0}; uint64_t t_ref = 0; uint64_t guard = 0; int total = 0;
  while (guard++ < 100000000ULL && total < 3 * n) {
    uint16_t pc = gb->pc;
    static long long win_a = -1, win_b = -1; static int win_init = 0;
    if (!win_init) { win_init = 1; if (getenv("WIN")) sscanf(getenv("WIN"), "%lld-%lld", &win_a, &win_b); }
    bool irq = gb->ime && (gb->ie & gb->io[R_IF] & 0x1f);
    if (!gb->halted) for (int i = 0; i < n; i++) if (pc == strtol(argv[3 + i], NULL, 16) && (win_a >= 0 ? (t_ref || i == 0) : hits[i] < 3)) {
      long long rel = (long long)(gb->cycles - t_ref) / 2;
      if (i == 0 && hits[0] == 0) t_ref = gb->cycles;
      hits[i]++; total++;
      if (win_a < 0 || (rel >= win_a && rel <= win_b) || (i == 0 && hits[0] == 1)) printf("OURS pc %04x #%d at +%lld M-cycles%s\n", pc, hits[i], rel, irq ? " IRQ" : "");
    }
    gb_step(gb);
  }
  return 0;
}
