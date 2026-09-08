#include "sha1.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t rol(uint32_t v, int n) { return (v << n) | (v >> (32 - n)); }

static void block(uint32_t h[5], const unsigned char *p) {
  uint32_t w[80];
  for (int i = 0; i < 16; i++)
    w[i] = (uint32_t)p[i * 4] << 24 | (uint32_t)p[i * 4 + 1] << 16 | (uint32_t)p[i * 4 + 2] << 8 | p[i * 4 + 3];
  for (int i = 16; i < 80; i++) w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
  uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
  for (int i = 0; i < 80; i++) {
    uint32_t f, k;
    if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
    else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
    else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
    else { f = b ^ c ^ d; k = 0xca62c1d6; }
    uint32_t t = rol(a, 5) + f + e + k + w[i];
    e = d; d = c; c = rol(b, 30); b = a; a = t;
  }
  h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}

void sha1(const unsigned char *data, size_t len, unsigned char out[20]) {
  uint32_t h[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
  size_t i = 0;
  for (; i + 64 <= len; i += 64) block(h, data + i);
  unsigned char tail[128] = {0};
  size_t rem = len - i;
  memcpy(tail, data + i, rem);
  tail[rem] = 0x80;
  size_t tl = rem + 1 <= 56 ? 64 : 128;
  uint64_t bits = (uint64_t)len * 8;
  for (int j = 0; j < 8; j++) tail[tl - 1 - j] = (unsigned char)(bits >> (8 * j));
  block(h, tail);
  if (tl == 128) block(h, tail + 64);
  for (int j = 0; j < 5; j++) {
    out[j * 4] = h[j] >> 24; out[j * 4 + 1] = h[j] >> 16; out[j * 4 + 2] = h[j] >> 8; out[j * 4 + 3] = h[j];
  }
}

void sha1_hex(const unsigned char *data, size_t len, char out[41]) {
  unsigned char d[20];
  sha1(data, len, d);
  for (int i = 0; i < 20; i++) sprintf(out + i * 2, "%02x", d[i]);
  out[40] = 0;
}
