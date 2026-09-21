#include "platform/setup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// A boot state is the GB struct behind a header naming the format and the struct's size, so a
// file recorded before GB grew is refused instead of misread.
typedef struct { char magic[8]; uint32_t version, size; } BootStateHeader;
#define BOOT_STATE_MAGIC "ORCLBOOT"
#define BOOT_STATE_VERSION 1

bool oracles_save_boot_state(const GB *gb, const char *path) {
  FILE *f = fopen(path, "wb");
  if (!f) return false;
  BootStateHeader h = {BOOT_STATE_MAGIC, BOOT_STATE_VERSION, (uint32_t)sizeof *gb};
  size_t n = fwrite(&h, 1, sizeof h, f) + fwrite(gb, 1, sizeof *gb, f);
  fclose(f);
  return n == sizeof h + sizeof *gb;
}

void oracles_copy_state(GB *gb, const GB *src) {
  GB keep = *gb;
  *gb = *src;
  gb->rom = keep.rom; gb->rom_size = keep.rom_size;
  gb->boot = NULL; gb->boot_size = 0; gb->boot_mapped = false;
  gb->samples = keep.samples; gb->sample = &gb->samples[0];
  gb->sample_head = gb->sample_count = 0;
  gb->serial_out = keep.serial_out; gb->serial_ctx = keep.serial_ctx;
  gb->input_at = keep.input_at; gb->input_ctx = keep.input_ctx;
  gb->frame_cb = keep.frame_cb; gb->frame_ctx = keep.frame_ctx;
  gb->step = keep.step; gb->native = keep.native; gb->ring = keep.ring; gb->fib = keep.fib;
  gb->trace_lo = keep.trace_lo; gb->trace_hi = keep.trace_hi;
  gb->hooks_checked = false;
}

bool oracles_load_boot_state(GB *gb, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  BootStateHeader h;
  if (fread(&h, 1, sizeof h, f) != sizeof h || memcmp(h.magic, BOOT_STATE_MAGIC, 8) != 0 || h.version != BOOT_STATE_VERSION || h.size != sizeof *gb) {
    fprintf(stderr, "%s: not a boot state for this build (want %s v%d, GB %zu bytes); re-record it with oracles-run --boot-state-out\n", path, BOOT_STATE_MAGIC, BOOT_STATE_VERSION, sizeof *gb);
    fclose(f);
    return false;
  }
  GB *tmp = malloc(sizeof *tmp);
  size_t n = fread(tmp, 1, sizeof *tmp, f);
  fclose(f);
  if (n == sizeof *tmp) oracles_copy_state(gb, tmp);
  free(tmp);
  return n == sizeof *tmp;
}
