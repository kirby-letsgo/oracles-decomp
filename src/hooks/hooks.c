#include "hooks/hooks.h"
#include "game/syms.h"
#include "core/bus.h"
#include "game/gen.h"
#include "game/seasons/gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

int hook_mode = HOOK_MODE_REPLACE;
uint64_t hook_verify_failures;
bool hook_verify_abort = true;

#define HOOK(bank, addr, name, flags) {bank, addr, #name, name, 0, flags},
static Hook hooks_ages[] = {
#include "hooks/table.h"
};
static Hook hooks_seasons[] = {
#include "hooks/table_seasons.h"
};
#undef HOOK
static Hook *hooks = hooks_ages;
static size_t NHOOKS = sizeof hooks_ages / sizeof hooks_ages[0];
static int16_t first_at[65536];
static int depth;
static jmp_buf hook_jmp_main[128];
static jmp_buf *hook_jmp = hook_jmp_main;
static int hook_jmp_depth;

void hook_ctx_init(HookCtx *c) { c->depth = c->jmp_depth = 0; c->jmp = calloc(128, sizeof(jmp_buf)); }
void hook_ctx_switch(HookCtx *save, const HookCtx *load) {
  save->depth = depth; save->jmp_depth = hook_jmp_depth; save->jmp = hook_jmp;
  depth = load->depth; hook_jmp_depth = load->jmp_depth; hook_jmp = load->jmp;
}

void hook_handoff(GB *gb, uint16_t pc) {
  gb->pc = pc;
  if (hook_jmp_depth > 0 && hook_mode != HOOK_MODE_OFF) longjmp(hook_jmp[hook_jmp_depth - 1], 1);
}
static bool inited;

static bool name_listed(const char *list, const char *name) {
  size_t n = strlen(name);
  for (const char *p = list; (p = strstr(p, name)); p += n)
    if ((p == list || p[-1] == ' ') && (p[n] == 0 || p[n] == ' ')) return true;
  return false;
}

void hooks_init(void) {
  inited = true;
  memset(first_at, -1, sizeof first_at);
  const char *only = getenv("HOOK_ONLY"), *skip = getenv("HOOK_SKIP");
  for (int i = (int)NHOOKS - 1; i >= 0; i--) {
    if (only && !name_listed(only, hooks[i].name)) continue;
    if (skip && name_listed(skip, hooks[i].name)) continue;
    first_at[hooks[i].addr] = (int16_t)i;
  }
}

static Hook *lookup(const GB *gb, uint16_t pc) {
  if (!inited) hooks_init();
  if (gb->boot_mapped) return NULL;
  if (!gb->hooks_checked) {
    GB *g = (GB *)gb;
    g->hooks_checked = true;
    bool ages = gb->rom_size > 0x150 && memcmp(gb->rom + 0x134, "ZELDA NAYRU", 11) == 0;
    bool seasons = gb->rom_size > 0x150 && memcmp(gb->rom + 0x134, "ZELDA DIN", 9) == 0;
    g->hooks_ok = ages || seasons;
    syms_select(seasons);
    hooks = seasons ? hooks_seasons : hooks_ages;
    NHOOKS = seasons ? sizeof hooks_seasons / sizeof hooks_seasons[0] : sizeof hooks_ages / sizeof hooks_ages[0];
    hooks_init();
  }
  if (!gb->hooks_ok) return NULL;
  int i = first_at[pc];
  if (i < 0) return NULL;
  for (; i < (int)NHOOKS && hooks[i].addr == pc; i++)
    if ((pc < 0x4000 || pc >= 0x8000 || hooks[i].bank == gb->rom_bank) && first_at[hooks[i].addr] >= 0) return &hooks[i];
  return NULL;
}

void gb_burn_nb(GB *gb, int mcycles) { for (int i = 0; i < mcycles; i++) gb_tick(gb); }

static void run_interrupt(GB *gb) {
  uint16_t sp0 = gb->sp;
  cpu_dispatch_interrupt(gb);
  while (gb->sp < sp0 && !gb->hung) gb->step(gb);
  if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
}

uint64_t dbg_vbl_midlogic, dbg_vbl_halt, dbg_vbl_step, dbg_vbl_step_halted;

void gb_burn(GB *gb, int mcycles) {
  if (depth > 0 && hook_mode == HOOK_MODE_REPLACE) {
    if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
    uint16_t pc0 = gb->hook_pc;
    bool delayed = gb->irq_delay;
    gb->irq_delay = false;
    while (!delayed && gb->ime && (gb->ie & gb->io[R_IF] & 0x1f) && !gb->hung) {
      if (gb->ie & gb->io[R_IF] & INT_VBLANK) dbg_vbl_midlogic++;
      gb->pc = pc0;
      run_interrupt(gb);
    }
  }
  if (gb->ime_delay) { gb->ime = true; gb->ime_delay = false; }
  for (int i = 0; i < mcycles; i++) gb_tick(gb);
}

int hook_halt(GB *gb, uint16_t next) {
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  if (pending) {
    if (gb->ime) return 1;
    gb->halt_bug = true;
    return -1;
  }
  gb->halted = true;
  while (!gb->hung) {
    if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
    gb_tick(gb);
    pending = gb->ie & gb->io[R_IF] & 0x1f;
    if (pending) break;
  }
  gb->halted = false;
  gb_tick(gb);
  if (gb->ime && pending && !hook_suppress_interrupts) { if (pending & INT_VBLANK) dbg_vbl_halt++; gb->pc = next; run_interrupt(gb); }
  return 0;
}

static bool differs(const char *what, const void *a, const void *b, size_t n, size_t *off) {
  const uint8_t *x = a, *y = b;
  for (size_t i = 0; i < n; i++) if (x[i] != y[i]) { *off = i; return true; }
  (void)what;
  return false;
}

static bool wram_differs(const uint8_t (*a)[4096], const uint8_t (*b)[4096], size_t *off) {
  if (differs("wram", a[0] + GB_W0_SKIP_END, b[0] + GB_W0_SKIP_END, 4096 - GB_W0_SKIP_END, off)) { *off += GB_W0_SKIP_END; return true; }
  if (differs("wram", a[1], b[1], 4096, off)) { *off += 4096; return true; }
  if (differs("wram", a[GB_SOUND_BANK], b[GB_SOUND_BANK], GB_SOUND_BANK_LO, off)) { *off += GB_SOUND_BANK * 4096; return true; }
  if (differs("wram", a[3], b[3], 5 * 4096, off)) { *off += 3 * 4096; return true; }
  return false;
}

static bool hram_differs(const uint8_t *a, const uint8_t *b, size_t *off) {
  if (differs("hram", a, b, GB_HRAM_MUSIC_LO, off)) return true;
  if (differs("hram", a + GB_HRAM_MUSIC_HI, b + GB_HRAM_MUSIC_HI, 127 - GB_HRAM_MUSIC_HI, off)) { *off += GB_HRAM_MUSIC_HI; return true; }
  return false;
}

static int verify_depth;
static GB *snap_pool[16][2];
int hook_in_verify;
int hook_suppress_interrupts;

static void verify(GB *gb, Hook *h) {
  if (verify_depth >= 16) { depth++; h->fn(gb); depth--; return; }
  if (!snap_pool[verify_depth][0]) { snap_pool[verify_depth][0] = malloc(sizeof(GB)); snap_pool[verify_depth][1] = malloc(sizeof(GB)); }
  GB *snap = snap_pool[verify_depth][0], *after_c_p = snap_pool[verify_depth][1];
  verify_depth++;
  { static int vlog = -1; if (vlog < 0) vlog = getenv("VERIFYLOG") != NULL; if (vlog) fprintf(stderr, "VERIFY> %s frame %llu mc %llu sp %04x\n", h->name, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)gb->mcycles, gb->sp); }
  uint16_t sp0 = gb->sp;
  bool ime0 = gb->ime;
  memcpy(snap, gb, sizeof *snap);
  gb->ime = false;
  uint64_t c0 = gb->mcycles;
  int depth0 = depth, hiv0 = hook_in_verify;
  depth++;
  hook_in_verify++;
  hook_suppress_interrupts++;
  if (hook_jmp_depth < 128) { hook_jmp_depth++; if (setjmp(hook_jmp[hook_jmp_depth - 1]) == 0) h->fn(gb); hook_jmp_depth--; } else h->fn(gb);
  hook_in_verify = hiv0;
  depth = depth0;
  hook_suppress_interrupts--;
  uint64_t cyc_c = gb->mcycles - c0;
  { static int vlog = -1; if (vlog < 0) vlog = getenv("VERIFYLOG") != NULL; if (vlog) fprintf(stderr, "VERIFY= %s cycC %llu pc %04x sp %04x\n", h->name, (unsigned long long)cyc_c, gb->pc, gb->sp); }
  memcpy(after_c_p, gb, sizeof *after_c_p);
  GBSample *samples_keep = gb->samples;
  memcpy(gb, snap, sizeof *snap);
  gb->samples = samples_keep;
  GB after_c_regs = *after_c_p;
  #define after_c after_c_regs
  gb->ime = false;
  int saved = hook_mode;
  hook_mode = HOOK_MODE_OFF;
  hook_suppress_interrupts++;
  uint16_t ret_pc = (uint16_t)(bus_read(gb, sp0) | (bus_read(gb, sp0 + 1) << 8));
  bool returned = after_c.pc == ret_pc && after_c.sp == sp0 + 2;
  uint64_t guard = 0;
  if (returned) { do { gb->step(gb); } while (!(gb->pc == ret_pc && gb->sp == sp0 + 2) && !gb->hung && guard++ < 50000000ULL); }
  else { while (gb->mcycles - c0 < cyc_c && !gb->hung) gb->step(gb); }
  hook_mode = saved;
  hook_suppress_interrupts--;
  uint64_t cyc_asm = gb->mcycles - c0;
  { static int vlog = -1; if (vlog < 0) vlog = getenv("VERIFYLOG") != NULL; if (vlog) fprintf(stderr, "VERIFY %s frame %llu returned %d cycC %llu cycAsm %llu guard %llu\n", h->name, (unsigned long long)GRID_FRAME(gb->cycles), returned, (unsigned long long)cyc_c, (unsigned long long)cyc_asm, (unsigned long long)guard); }
  if (gb->ime_writes == snap->ime_writes) gb->ime = ime0;
  size_t off = 0;
  const char *bad = NULL;
  uint8_t ra[] = {after_c.a, after_c.f, after_c.b, after_c.c, after_c.d, after_c.e, after_c.h, after_c.l};
  uint8_t rb[] = {gb->a, gb->f, gb->b, gb->c, gb->d, gb->e, gb->h, gb->l};
  if (differs("regs", ra, rb, sizeof ra, &off)) bad = "afbcdehl"[off] == 'a' ? "reg a" : "regs";
  else if (after_c.sp != gb->sp || after_c.pc != gb->pc) bad = "sp/pc";
  else if (wram_differs(after_c.wram, gb->wram, &off)) bad = "wram";
  else if (hram_differs(after_c.hram, gb->hram, &off)) bad = "hram";
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
  #undef after_c
  verify_depth--;
}

int hook_shadow;
uint64_t hook_shadow_checked, hook_shadow_skipped;
static GBSample shadow_samples[16];

static void report_mismatch(Hook *h, const char *bad, size_t off, const GB *c, const GB *a, uint64_t cyc_c, uint64_t cyc_asm) {
  hook_verify_failures++;
  fprintf(stderr, "HOOK MISMATCH %s (%02x:%04x) %s off=%zu frame %llu: C=%llu cycles asm=%llu cycles regs C=%02x %02x %02x%02x %02x%02x %02x%02x asm=%02x %02x %02x%02x %02x%02x %02x%02x\n",
          h->name, h->bank, h->addr, bad, off, (unsigned long long)GRID_FRAME(c->cycles), (unsigned long long)cyc_c, (unsigned long long)cyc_asm,
          c->a, c->f, c->b, c->c, c->d, c->e, c->h, c->l, a->a, a->f, a->b, a->c, a->d, a->e, a->h, a->l);
  if (hook_verify_abort) exit(3);
}

// Run the hook's C for real (exactly as replace mode), then the original code on a copy of the
// machine from the same starting state, and compare. A call during which the C side took an
// interrupt is not comparable (the copy runs with interrupts held) and is only counted.
static void shadow(GB *gb, Hook *h) {
  static const char *only; static int only_init;
  if (!only_init) { only_init = 1; only = getenv("SHADOW_ONLY"); }
  if (only && strcmp(only, h->name) != 0) { if (hook_jmp_depth < 128) { hook_jmp_depth++; if (setjmp(hook_jmp[hook_jmp_depth - 1]) == 0) h->fn(gb); hook_jmp_depth--; } else h->fn(gb); return; }
  const int my = verify_depth;
  if (my >= 16) { h->fn(gb); return; }
  if (!snap_pool[my][0]) { snap_pool[my][0] = malloc(sizeof(GB)); snap_pool[my][1] = malloc(sizeof(GB)); }
  GB *snap = snap_pool[my][0], *after = snap_pool[my][1];
  uint16_t sp0 = gb->sp;
  memcpy(snap, gb, sizeof *snap);
  uint16_t ret_pc = (uint16_t)(bus_read(gb, sp0) | (bus_read(gb, sp0 + 1) << 8));
  uint64_t c0 = gb->mcycles, irq0 = gb_irq_dispatched;
  static int vlog = -1; if (vlog < 0) vlog = getenv("VERIFYLOG") != NULL;
  if (vlog) fprintf(stderr, "VERIFY> %s frame %llu mc %llu sp %04x\n", h->name, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)gb->mcycles, gb->sp);
  verify_depth = my + 1;
  if (hook_jmp_depth < 128) { hook_jmp_depth++; if (setjmp(hook_jmp[hook_jmp_depth - 1]) == 0) h->fn(gb); hook_jmp_depth--; } else h->fn(gb);
  verify_depth = my;
  uint64_t cyc_c = gb->mcycles - c0;
  // only calls that ran to their `ret` without an interrupt are comparable: the original code is
  // replayed with interrupts held, and a handoff (thread switch, reset) has no end to compare at
  if (gb_irq_dispatched != irq0 || !(gb->pc == ret_pc && gb->sp == (uint16_t)(sp0 + 2))) {
    if (only) fprintf(stderr, "SHADOW skip %s frame %llu irq %d pc %04x sp %04x want %04x %04x\n", h->name, (unsigned long long)GRID_FRAME(gb->cycles), gb_irq_dispatched != irq0, gb->pc, gb->sp, ret_pc, (uint16_t)(sp0 + 2));
    hook_shadow_skipped++; return;
  }
  if (vlog) fprintf(stderr, "VERIFY= %s cycC %llu pc %04x sp %04x\n", h->name, (unsigned long long)cyc_c, gb->pc, gb->sp);
  memcpy(after, gb, sizeof *after);
  memcpy(gb, snap, sizeof *gb);
  gb->samples = shadow_samples; gb->frame_cb = NULL; gb->ring = NULL; gb->trace_hi = 0;
  int saved = hook_mode;
  hook_mode = HOOK_MODE_OFF;
  hook_suppress_interrupts++;
  uint64_t guard = 0;
  do { gb->step(gb); } while (!(gb->pc == ret_pc && gb->sp == (uint16_t)(sp0 + 2)) && !gb->hung && gb->mcycles - c0 <= cyc_c + 1000000 && guard++ < 5000000ULL);
  hook_suppress_interrupts--;
  hook_mode = saved;
  uint64_t cyc_asm = gb->mcycles - c0;
  if (vlog) fprintf(stderr, "VERIFY %s frame %llu returned 1 cycC %llu cycAsm %llu\n", h->name, (unsigned long long)GRID_FRAME(gb->cycles), (unsigned long long)cyc_c, (unsigned long long)cyc_asm);
  hook_shadow_checked++;
  size_t off = 0;
  const char *bad = NULL;
  uint8_t ra[] = {after->a, after->f, after->b, after->c, after->d, after->e, after->h, after->l};
  uint8_t rb[] = {gb->a, gb->f, gb->b, gb->c, gb->d, gb->e, gb->h, gb->l};
  if (differs("regs", ra, rb, sizeof ra, &off)) bad = off == 0 ? "reg a" : "regs";
  else if (after->sp != gb->sp || after->pc != gb->pc) bad = "sp/pc";
  else if (wram_differs(after->wram, gb->wram, &off)) bad = "wram";
  else if (hram_differs(after->hram, gb->hram, &off)) bad = "hram";
  else if (differs("vram", after->vram, gb->vram, sizeof gb->vram, &off)) bad = "vram";
  else if (differs("oam", after->oam, gb->oam, sizeof gb->oam, &off)) bad = "oam";
  else if (differs("eram", after->eram, gb->eram, sizeof gb->eram, &off)) bad = "eram";
  else if (differs("io", after->io, gb->io, sizeof gb->io, &off)) bad = "io";
  else if (cyc_c != cyc_asm) bad = "cycles";
  if (bad) report_mismatch(h, bad, off, after, gb, cyc_c, cyc_asm);
  memcpy(gb, after, sizeof *gb);
}

void hook_run(GB *gb, HookFn fn, uint16_t target) {
  if (hook_shadow) { Hook *h = lookup(gb, target); if (h && h->fn == fn) { h->calls++; gb->pc = target; shadow(gb, h); return; } }
  fn(gb);
}

bool hook_enabled_at(const GB *gb, uint16_t addr) {
  if (!inited) hooks_init();
  int i = first_at[addr];
  if (i < 0) return false;
  for (; i < (int)NHOOKS && hooks[i].addr == addr; i++)
    if ((addr < 0x4000 || addr >= 0x8000 || hooks[i].bank == gb->rom_bank) && first_at[hooks[i].addr] >= 0) return true;
  return false;
}

bool hook_is(const GB *gb, uint16_t addr, void (*fn)(GB *)) {
  if (!inited) hooks_init();
  int i = first_at[addr];
  if (i < 0) return false;
  for (; i < (int)NHOOKS && hooks[i].addr == addr; i++)
    if ((addr < 0x4000 || addr >= 0x8000 || hooks[i].bank == gb->rom_bank) && first_at[hooks[i].addr] >= 0) return hooks[i].fn == fn;
  return false;
}

bool hook_dispatch(GB *gb) {
  if (hook_mode == HOOK_MODE_OFF) return false;
  Hook *h = lookup(gb, gb->pc);
  if (!h) return false;
  if (hook_mode == HOOK_MODE_VERIFY && (h->flags & (HOOK_NOVERIFY | HOOK_LOCAL))) return false;
  h->calls++;
  static int verify_all = -1; static const char *verify_only; if (verify_all < 0) { verify_all = getenv("VERIFY_ALL") != NULL; verify_only = getenv("VERIFY_ONLY"); }
  if (hook_mode == HOOK_MODE_VERIFY && verify_only) { if (strcmp(h->name, verify_only) == 0) { verify(gb, h); return true; } }
  else if (hook_mode == HOOK_MODE_VERIFY && (verify_all || h->calls <= 4 || h->calls % 256 == 0 || verify_depth == 0)) { verify(gb, h); return true; }
  static int hooklog = -1; if (hooklog < 0) hooklog = getenv("HOOKLOG") != NULL;
  if (hooklog) fprintf(stderr, "HOOK> %s mc %llu frame %llu sp %04x ime %d\n", h->name, (unsigned long long)gb->mcycles, (unsigned long long)GRID_FRAME(gb->cycles), gb->sp, gb->ime);
  if (gb->ring) { DispatchRec *r = &gb->ring->r[gb->ring->n++ % 64]; r->bank = h->bank; r->addr = h->addr; r->name = h->name; r->mcycles = gb->mcycles; }
  if (gb->trace_hi) { uint64_t fr = GRID_FRAME(gb->cycles); if (fr >= gb->trace_lo && fr <= gb->trace_hi) fprintf(stderr, "%s %02x:%04x %s mc %llu frame %llu sp %04x\n", gb->native ? "NAT" : "REF", h->bank, h->addr, h->name, (unsigned long long)gb->mcycles, (unsigned long long)fr, gb->sp); }
  int depth0 = depth;
  depth++;
  if (hook_shadow) shadow(gb, h);
  else if (hook_jmp_depth < 128) { hook_jmp_depth++; if (setjmp(hook_jmp[hook_jmp_depth - 1]) == 0) h->fn(gb); hook_jmp_depth--; } else h->fn(gb);
  depth = depth0;
  if (hooklog) fprintf(stderr, "HOOK< %s mc %llu frame %llu sp %04x pc %04x ime %d\n", h->name, (unsigned long long)gb->mcycles, (unsigned long long)GRID_FRAME(gb->cycles), gb->sp, gb->pc, gb->ime);
  return true;
}

void hooks_report(void) {
  uint64_t total = 0;
  for (size_t i = 0; i < NHOOKS; i++) total += hooks[i].calls;
  fprintf(stderr, "hooks: %zu routines, %llu calls, %llu verify failures\n", NHOOKS, (unsigned long long)total, (unsigned long long)hook_verify_failures);
  if (hook_shadow) fprintf(stderr, "shadow: %llu calls compared, %llu interrupted (not compared)\n", (unsigned long long)hook_shadow_checked, (unsigned long long)hook_shadow_skipped);
  fprintf(stderr, "vblank dispatch: mid-logic %llu, from halt %llu, interpreter %llu (of which halted %llu)\n", (unsigned long long)dbg_vbl_midlogic, (unsigned long long)dbg_vbl_halt, (unsigned long long)dbg_vbl_step, (unsigned long long)dbg_vbl_step_halted);
  for (size_t i = 0; i < NHOOKS; i++) {
    size_t n = strlen(hooks[i].name);
    if (n > 5 && strcmp(hooks[i].name + n - 5, "_hook") == 0 && hooks[i].calls == 0) fprintf(stderr, "  never called: %s\n", hooks[i].name);
  }
  for (size_t i = 0; i < NHOOKS; i++) fprintf(stderr, "  %02x:%04x %-28s %llu\n", hooks[i].bank, hooks[i].addr, hooks[i].name, (unsigned long long)hooks[i].calls);
}

void asm_call(GB *gb, uint16_t target, uint16_t ret_addr) {
  uint16_t sp0 = gb->sp;
  static int hooklog = -1; if (hooklog < 0) hooklog = getenv("HOOKLOG") != NULL;
  if (hooklog) fprintf(stderr, "ASM> %04x bank %u mc %llu sp %04x ime %d\n", target, gb->rom_bank, (unsigned long long)gb->mcycles, sp0, gb->ime);
  gb->pc = target;
  uint32_t sl0 = gb->sp_loads;
  depth--;
  while (!(gb->pc == ret_addr && gb->sp == (uint16_t)(sp0 + 2)) && !gb->hung) {
    if ((hook_mode == HOOK_MODE_VERIFY && gb->sp_loads != sl0) || ((uint16_t)(gb->sp - sp0) > 2 && (uint16_t)(gb->sp - sp0) < 0x8000)) { depth++; hook_handoff(gb, gb->pc); }
    gb->step(gb);
  }
  depth++;
  if (hooklog) fprintf(stderr, "ASM< %04x mc %llu sp %04x ime %d\n", target, (unsigned long long)gb->mcycles, gb->sp, gb->ime);
}

void hook_continue(GB *gb, uint16_t pc, uint16_t sp0) {
  uint16_t ret_addr = (uint16_t)(bus_read(gb, sp0) | (bus_read(gb, sp0 + 1) << 8));
  uint32_t sl0 = gb->sp_loads;
  gb->pc = pc;
  depth--;
  while (!(gb->pc == ret_addr && gb->sp == (uint16_t)(sp0 + 2)) && !gb->hung) {
    if ((hook_mode == HOOK_MODE_VERIFY && (gb->sp_loads != sl0 || (gb->sp == sp0 && depth > 24 && lookup(gb, gb->pc)))) || ((uint16_t)(gb->sp - sp0) > 2 && (uint16_t)(gb->sp - sp0) < 0x8000)) { depth++; hook_handoff(gb, gb->pc); depth--; break; }
    gb->step(gb);
  }
  depth++;
}

void asm_continue(GB *gb, uint16_t ret_addr, uint16_t sp0) {
  depth--;
  while (!(gb->pc == ret_addr && gb->sp == sp0 + 2) && !gb->hung) gb->step(gb);
  depth++;
}
