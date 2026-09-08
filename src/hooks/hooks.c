#include "hooks/hooks.h"
#include "core/bus.h"
#include "game/game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hook_mode = HOOK_MODE_REPLACE;
uint64_t hook_verify_failures;
bool hook_verify_abort = true;

static Hook hooks[] = {
#define HOOK(bank, addr, name) {bank, addr, #name, name, 0},
#include "hooks/table.h"
#undef HOOK
};
#define NHOOKS (sizeof hooks / sizeof hooks[0])
static int16_t first_at[65536];
static int depth;
static bool inited;

void hooks_init(void) {
  inited = true;
  memset(first_at, -1, sizeof first_at);
  for (int i = (int)NHOOKS - 1; i >= 0; i--) first_at[hooks[i].addr] = (int16_t)i;
}

static Hook *lookup(const GB *gb, uint16_t pc) {
  if (!inited) hooks_init();
  if (gb->boot_mapped) return NULL;
  if (!gb->hooks_checked) { GB *g = (GB *)gb; g->hooks_checked = true; g->hooks_ok = gb->rom_size > 0x150 && memcmp(gb->rom + 0x134, "ZELDA NAYRU", 11) == 0; }
  if (!gb->hooks_ok) return NULL;
  int i = first_at[pc];
  if (i < 0) return NULL;
  for (; i < (int)NHOOKS && hooks[i].addr == pc; i++)
    if (pc < 0x4000 || hooks[i].bank == gb->rom_bank) return &hooks[i];
  return NULL;
}

void gb_burn(GB *gb, int mcycles) {
  if (depth > 0 && hook_mode == HOOK_MODE_REPLACE) {
    if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
    while (gb->ime && (gb->ie & gb->io[R_IF] & 0x1f) && !gb->hung) {
      uint16_t sp0 = gb->sp;
      gb->pc = gb->hook_pc;
      cpu_dispatch_interrupt(gb);
      while (gb->sp < sp0 && !gb->hung) gb_step(gb);
      if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
    }
  }
  for (int i = 0; i < mcycles; i++) gb_tick(gb);
}

static bool differs(const char *what, const void *a, const void *b, size_t n, size_t *off) {
  const uint8_t *x = a, *y = b;
  for (size_t i = 0; i < n; i++) if (x[i] != y[i]) { *off = i; return true; }
  (void)what;
  return false;
}

static void verify(GB *gb, Hook *h) {
  static GB snap, after_c;
  uint16_t sp0 = gb->sp;
  bool ime0 = gb->ime;
  memcpy(&snap, gb, sizeof snap);
  gb->ime = false;
  uint64_t c0 = gb->mcycles;
  depth++;
  h->fn(gb);
  depth--;
  uint64_t cyc_c = gb->mcycles - c0;
  memcpy(&after_c, gb, sizeof after_c);
  memcpy(gb, &snap, sizeof snap);
  gb->ime = false;
  int saved = hook_mode;
  hook_mode = HOOK_MODE_OFF;
  uint16_t ret_pc = (uint16_t)(bus_read(gb, sp0) | (bus_read(gb, sp0 + 1) << 8));
  uint64_t guard = 0;
  do { gb_step(gb); } while (!(gb->pc == ret_pc && gb->sp == sp0 + 2) && !gb->hung && guard++ < 50000000ULL);
  hook_mode = saved;
  uint64_t cyc_asm = gb->mcycles - c0;
  gb->ime = ime0;
  size_t off = 0;
  const char *bad = NULL;
  uint8_t ra[] = {after_c.a, after_c.f, after_c.b, after_c.c, after_c.d, after_c.e, after_c.h, after_c.l};
  uint8_t rb[] = {gb->a, gb->f, gb->b, gb->c, gb->d, gb->e, gb->h, gb->l};
  if (differs("regs", ra, rb, sizeof ra, &off)) bad = "afbcdehl"[off] == 'a' ? "reg a" : "regs";
  else if (after_c.sp != gb->sp || after_c.pc != gb->pc) bad = "sp/pc";
  else if (differs("wram", after_c.wram, gb->wram, sizeof gb->wram, &off)) bad = "wram";
  else if (differs("hram", after_c.hram, gb->hram, sizeof gb->hram, &off)) bad = "hram";
  else if (differs("vram", after_c.vram, gb->vram, sizeof gb->vram, &off)) bad = "vram";
  else if (differs("oam", after_c.oam, gb->oam, sizeof gb->oam, &off)) bad = "oam";
  else if (differs("eram", after_c.eram, gb->eram, sizeof gb->eram, &off)) bad = "eram";
  else if (differs("io", after_c.io, gb->io, sizeof gb->io, &off)) bad = "io";
  else if (cyc_c != cyc_asm) bad = "cycles";
  if (bad) {
    hook_verify_failures++;
    fprintf(stderr, "HOOK MISMATCH %s (%02x:%04x) %s off=%zu frame %llu: C=%llu cycles asm=%llu cycles regs C=%02x %02x %02x%02x %02x%02x %02x%02x asm=%02x %02x %02x%02x %02x%02x %02x%02x\n",
            h->name, h->bank, h->addr, bad, off, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)cyc_c, (unsigned long long)cyc_asm,
            ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7]);
    if (hook_verify_abort) exit(3);
  }
}

bool hook_dispatch(GB *gb) {
  if (hook_mode == HOOK_MODE_OFF) return false;
  Hook *h = lookup(gb, gb->pc);
  if (!h) return false;
  h->calls++;
  if (hook_mode == HOOK_MODE_VERIFY) { verify(gb, h); return true; }
  depth++;
  h->fn(gb);
  depth--;
  return true;
}

void hooks_report(void) {
  uint64_t total = 0;
  for (size_t i = 0; i < NHOOKS; i++) total += hooks[i].calls;
  fprintf(stderr, "hooks: %zu routines, %llu calls, %llu verify failures\n", NHOOKS, (unsigned long long)total, (unsigned long long)hook_verify_failures);
  for (size_t i = 0; i < NHOOKS; i++) fprintf(stderr, "  %02x:%04x %-28s %llu\n", hooks[i].bank, hooks[i].addr, hooks[i].name, (unsigned long long)hooks[i].calls);
}
