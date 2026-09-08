#include "platform/png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t crc_table[256];

static void crc_init(void) {
  if (crc_table[1]) return;
  for (uint32_t n = 0; n < 256; n++) {
    uint32_t c = n;
    for (int k = 0; k < 8; k++) c = (c & 1) ? 0xedb88320u ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  }
}

static uint32_t crc32_update(uint32_t c, const uint8_t *p, size_t n) {
  c = ~c;
  for (size_t i = 0; i < n; i++) c = crc_table[(c ^ p[i]) & 0xff] ^ (c >> 8);
  return ~c;
}

static void put32(FILE *f, uint32_t v) {
  uint8_t b[4] = {v >> 24, v >> 16, v >> 8, v};
  fwrite(b, 1, 4, f);
}

static void chunk(FILE *f, const char *type, const uint8_t *data, size_t len) {
  put32(f, (uint32_t)len);
  fwrite(type, 1, 4, f);
  if (len) fwrite(data, 1, len, f);
  uint32_t c = crc32_update(0, (const uint8_t *)type, 4);
  c = crc32_update(c, data, len);
  put32(f, c);
}

bool png_write_rgb(const char *path, const uint8_t *rgb, int w, int h) {
  crc_init();
  FILE *f = fopen(path, "wb");
  if (!f) return false;
  size_t rowlen = (size_t)w * 3 + 1, rawlen = rowlen * h;
  uint8_t *raw = malloc(rawlen);
  for (int y = 0; y < h; y++) {
    raw[y * rowlen] = 0;
    memcpy(raw + y * rowlen + 1, rgb + (size_t)y * w * 3, (size_t)w * 3);
  }
  size_t nblocks = (rawlen + 65534) / 65535;
  size_t zlen = 2 + rawlen + nblocks * 5 + 4;
  uint8_t *z = malloc(zlen);
  size_t zp = 0;
  z[zp++] = 0x78; z[zp++] = 0x01;
  uint32_t a = 1, b = 0;
  for (size_t i = 0; i < rawlen; i++) { a = (a + raw[i]) % 65521; b = (b + a) % 65521; }
  for (size_t off = 0; off < rawlen; off += 65535) {
    size_t len = rawlen - off < 65535 ? rawlen - off : 65535;
    z[zp++] = off + len == rawlen ? 1 : 0;
    z[zp++] = len & 0xff; z[zp++] = len >> 8;
    z[zp++] = ~len & 0xff; z[zp++] = (~len >> 8) & 0xff;
    memcpy(z + zp, raw + off, len);
    zp += len;
  }
  uint32_t adler = (b << 16) | a;
  z[zp++] = adler >> 24; z[zp++] = adler >> 16; z[zp++] = adler >> 8; z[zp++] = adler;
  static const uint8_t sig[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
  fwrite(sig, 1, 8, f);
  uint8_t ihdr[13] = {w >> 24, w >> 16, w >> 8, w, h >> 24, h >> 16, h >> 8, h, 8, 2, 0, 0, 0};
  chunk(f, "IHDR", ihdr, 13);
  chunk(f, "IDAT", z, zp);
  chunk(f, "IEND", NULL, 0);
  fclose(f);
  free(raw);
  free(z);
  return true;
}
