#include "assets/assets.h"
#include <stdlib.h>
#include <string.h>

bool assets_is_seasons(const uint8_t *rom, size_t size) {
  return size > 0x150 && memcmp(rom + 0x134, "ZELDA DIN", 9) == 0;
}

static void code_map(const uint8_t *rom, size_t size, const CodeRange **r, size_t *n) {
  bool s = assets_is_seasons(rom, size);
  *r = s ? codemap_seasons : codemap_ages;
  *n = s ? codemap_seasons_n : codemap_ages_n;
}

uint8_t *assets_code_bits(const uint8_t *rom, size_t size) {
  const CodeRange *r; size_t n;
  code_map(rom, size, &r, &n);
  uint8_t *bits = calloc((size + 7) / 8, 1);
  for (size_t i = 0; i < n; i++)
    for (uint32_t k = r[i].off; k < r[i].off + r[i].len && k < size; k++) bits[k >> 3] |= (uint8_t)(1 << (k & 7));
  return bits;
}

size_t assets_zero_code(uint8_t *rom, size_t size) {
  const CodeRange *r; size_t n, zeroed = 0;
  code_map(rom, size, &r, &n);
  for (size_t i = 0; i < n; i++) {
    if (r[i].off >= size) continue;
    size_t len = r[i].off + r[i].len > size ? size - r[i].off : r[i].len;
    memset(rom + r[i].off, 0, len);
    zeroed += len;
  }
  return zeroed;
}
