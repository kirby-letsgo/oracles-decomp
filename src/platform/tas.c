#include "platform/tas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool tas_load(Tas *t, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  uint8_t header[8];
  if (fread(header, 1, 8, f) != 8 || memcmp(header, "ORIN", 4) != 0) { fclose(f); return false; }
  t->count = header[4] | (header[5] << 8) | (header[6] << 16) | ((uint32_t)header[7] << 24);
  t->inputs = malloc(t->count);
  if (fread(t->inputs, 1, t->count, f) != t->count) { free(t->inputs); fclose(f); return false; }
  fclose(f);
  return true;
}

void tas_free(Tas *t) { free(t->inputs); t->inputs = NULL; t->count = 0; }

uint8_t tas_input_at(const Tas *t, uint64_t frame) {
  return frame < t->count ? t->inputs[frame] : 0;
}
