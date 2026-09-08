#include "platform/setup.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t *oracles_read_file(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (n < 0) { fclose(f); return NULL; }
  uint8_t *buf = malloc((size_t)n + 1);
  if (!buf) { fclose(f); return NULL; }
  size_t got = fread(buf, 1, (size_t)n, f);
  fclose(f);
  if (got != (size_t)n) { free(buf); return NULL; }
  *size = got;
  return buf;
}

void oracles_apply_agb_boot_patch(uint8_t *boot, size_t size) {
  static const uint8_t gba_override[13] = {0xff, 0x00, 0xcd, 0x03, 0x35, 0xaa, 0x31, 0x90, 0x94, 0, 0, 0, 0};
  if (size < 0x100) return;
  for (int i = 0; i < 13; i++) boot[0xf3 + i] = (uint8_t)(boot[0xf3 + i] + gba_override[i]);
}

bool oracles_load_init_ram(GB *gb, const char *path) {
  FILE *f = fopen(path, "r");
  if (!f) return false;
  static char line[70000];
  if (fgets(line, sizeof line, f)) for (int i = 0; i < 32768 && line[i * 2] && line[i * 2 + 1]; i++) {
    unsigned v; sscanf(line + i * 2, "%2x", &v); gb->wram[i / 4096][i % 4096] = (uint8_t)v;
  }
  if (fgets(line, sizeof line, f)) for (int i = 0; i < 127 && line[i * 2] && line[i * 2 + 1]; i++) {
    unsigned v; sscanf(line + i * 2, "%2x", &v); gb->hram[i] = (uint8_t)v;
  }
  fclose(f);
  gb->init_ram_loaded = true;
  return true;
}
