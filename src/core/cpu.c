#include "gb.h"
#include "bus.h"
#include "hooks/hooks.h"


#define HL (((uint16_t)gb->h << 8) | gb->l)
#define BC (((uint16_t)gb->b << 8) | gb->c)
#define DE (((uint16_t)gb->d << 8) | gb->e)
#define AF (((uint16_t)gb->a << 8) | gb->f)

static inline uint8_t rd(GB *gb, uint16_t a) { gb_tick(gb); return bus_read(gb, a); }
static inline void wr(GB *gb, uint16_t a, uint8_t v) { gb_tick(gb); bus_write(gb, a, v); }

static inline uint8_t fetch(GB *gb) {
  uint8_t v = rd(gb, gb->pc);
  if (gb->halt_bug) gb->halt_bug = false;
  else gb->pc++;
  return v;
}
static inline uint16_t fetch16(GB *gb) {
  uint8_t lo = fetch(gb);
  uint8_t hi = fetch(gb);
  return lo | ((uint16_t)hi << 8);
}
static inline void set_hl(GB *gb, uint16_t v) { gb->h = v >> 8; gb->l = v; }

static uint8_t get_r(GB *gb, int i) {
  switch (i) {
  case 0: return gb->b; case 1: return gb->c; case 2: return gb->d; case 3: return gb->e;
  case 4: return gb->h; case 5: return gb->l; case 6: return rd(gb, HL); default: return gb->a;
  }
}
static void set_r(GB *gb, int i, uint8_t v) {
  switch (i) {
  case 0: gb->b = v; break; case 1: gb->c = v; break; case 2: gb->d = v; break; case 3: gb->e = v; break;
  case 4: gb->h = v; break; case 5: gb->l = v; break; case 6: wr(gb, HL, v); break; default: gb->a = v; break;
  }
}
static uint16_t get_rp(GB *gb, int i) {
  switch (i) { case 0: return BC; case 1: return DE; case 2: return HL; default: return gb->sp; }
}
static void set_rp(GB *gb, int i, uint16_t v) {
  switch (i) {
  case 0: gb->b = v >> 8; gb->c = v; break; case 1: gb->d = v >> 8; gb->e = v; break;
  case 2: set_hl(gb, v); break; default: gb->sp = v; break;
  }
}
static uint16_t get_rp2(GB *gb, int i) { return i == 3 ? AF : get_rp(gb, i); }
static void set_rp2(GB *gb, int i, uint16_t v) {
  if (i == 3) { gb->a = v >> 8; gb->f = v & 0xf0; } else set_rp(gb, i, v);
}
static bool cond(GB *gb, int i) {
  switch (i) { case 0: return !(gb->f & FZ); case 1: return gb->f & FZ; case 2: return !(gb->f & FC); default: return gb->f & FC; }
}

static void push(GB *gb, uint16_t v) {
  gb_tick(gb);
  wr(gb, --gb->sp, v >> 8);
  wr(gb, --gb->sp, v);
}
static uint16_t pop(GB *gb) {
  uint8_t lo = rd(gb, gb->sp++);
  uint8_t hi = rd(gb, gb->sp++);
  return lo | ((uint16_t)hi << 8);
}

static void alu(GB *gb, int op, uint8_t v) {
  uint8_t a = gb->a;
  int c = (gb->f & FC) ? 1 : 0;
  int r;
  uint8_t f = 0;
  switch (op) {
  case 0: r = a + v; f = ((r & 0xff) == 0 ? FZ : 0) | ((a & 0xf) + (v & 0xf) > 0xf ? FH : 0) | (r > 0xff ? FC : 0); gb->a = r; break;
  case 1: r = a + v + c; f = ((r & 0xff) == 0 ? FZ : 0) | ((a & 0xf) + (v & 0xf) + c > 0xf ? FH : 0) | (r > 0xff ? FC : 0); gb->a = r; break;
  case 2: r = a - v; f = FN | ((r & 0xff) == 0 ? FZ : 0) | ((a & 0xf) < (v & 0xf) ? FH : 0) | (a < v ? FC : 0); gb->a = r; break;
  case 3: r = a - v - c; f = FN | ((r & 0xff) == 0 ? FZ : 0) | ((a & 0xf) - (v & 0xf) - c < 0 ? FH : 0) | (r < 0 ? FC : 0); gb->a = r; break;
  case 4: gb->a = a & v; f = (gb->a == 0 ? FZ : 0) | FH; break;
  case 5: gb->a = a ^ v; f = gb->a == 0 ? FZ : 0; break;
  case 6: gb->a = a | v; f = gb->a == 0 ? FZ : 0; break;
  default: r = a - v; f = FN | ((r & 0xff) == 0 ? FZ : 0) | ((a & 0xf) < (v & 0xf) ? FH : 0) | (a < v ? FC : 0); break;
  }
  gb->f = f;
}

static uint8_t rot(GB *gb, int op, uint8_t v) {
  uint8_t c = (gb->f & FC) ? 1 : 0, nc, r;
  switch (op) {
  case 0: nc = v >> 7; r = (v << 1) | nc; break;
  case 1: nc = v & 1; r = (v >> 1) | (nc << 7); break;
  case 2: nc = v >> 7; r = (v << 1) | c; break;
  case 3: nc = v & 1; r = (v >> 1) | (c << 7); break;
  case 4: nc = v >> 7; r = v << 1; break;
  case 5: nc = v & 1; r = (v >> 1) | (v & 0x80); break;
  case 6: nc = 0; r = (v << 4) | (v >> 4); break;
  default: nc = v & 1; r = v >> 1; break;
  }
  gb->f = (r == 0 ? FZ : 0) | (nc ? FC : 0);
  return r;
}

static void daa(GB *gb) {
  int a = gb->a;
  uint8_t f = gb->f & (FN | FC);
  if (!(gb->f & FN)) {
    if ((gb->f & FH) || (a & 0x0f) > 9) a += 0x06;
    if ((gb->f & FC) || a > 0x9f) { a += 0x60; f |= FC; }
  } else {
    if (gb->f & FH) a = (a - 6) & 0xff;
    if (gb->f & FC) a -= 0x60;
  }
  a &= 0xff;
  if (a == 0) f |= FZ;
  gb->a = a;
  gb->f = f;
}

static void cb(GB *gb) {
  uint8_t op = fetch(gb);
  int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
  if (x == 0) { set_r(gb, z, rot(gb, y, get_r(gb, z))); return; }
  uint8_t v = get_r(gb, z);
  if (x == 1) { gb->f = (gb->f & FC) | FH | ((v & (1 << y)) ? 0 : FZ); return; }
  if (x == 2) set_r(gb, z, v & ~(1 << y));
  else set_r(gb, z, v | (1 << y));
}

static void stop(GB *gb) {
  fetch(gb);
  if (!gb->speed_armed) { gb->halted = true; return; }
  gb_speed_switch(gb);
}

static void execute(GB *gb, uint8_t op) {
  int x = op >> 6, y = (op >> 3) & 7, z = op & 7, p = y >> 1, q = y & 1;
  switch (x) {
  case 0:
    switch (z) {
    case 0:
      switch (y) {
      case 0: return;
      case 1: { uint16_t a = fetch16(gb); wr(gb, a, gb->sp); wr(gb, a + 1, gb->sp >> 8); return; }
      case 2: stop(gb); return;
      case 3: { int8_t d = fetch(gb); gb_tick(gb); gb->pc += d; return; }
      default: { int8_t d = fetch(gb); if (cond(gb, y - 4)) { gb_tick(gb); gb->pc += d; } return; }
      }
    case 1:
      if (q == 0) { set_rp(gb, p, fetch16(gb)); if (p == 3) { gb->sp_loads++; hook_handoff(gb, gb->pc); } return; }
      { uint16_t hl = HL, v = get_rp(gb, p); uint32_t r = hl + v;
        gb->f = (gb->f & FZ) | ((hl & 0xfff) + (v & 0xfff) > 0xfff ? FH : 0) | (r > 0xffff ? FC : 0);
        set_hl(gb, r); gb_tick(gb); return; }
    case 2:
      if (q == 0) {
        switch (p) {
        case 0: wr(gb, BC, gb->a); return;
        case 1: wr(gb, DE, gb->a); return;
        case 2: wr(gb, HL, gb->a); set_hl(gb, HL + 1); return;
        default: wr(gb, HL, gb->a); set_hl(gb, HL - 1); return;
        }
      }
      switch (p) {
      case 0: gb->a = rd(gb, BC); return;
      case 1: gb->a = rd(gb, DE); return;
      case 2: gb->a = rd(gb, HL); set_hl(gb, HL + 1); return;
      default: gb->a = rd(gb, HL); set_hl(gb, HL - 1); return;
      }
    case 3:
      set_rp(gb, p, get_rp(gb, p) + (q == 0 ? 1 : -1));
      gb_tick(gb);
      return;
    case 4: { uint8_t v = get_r(gb, y) + 1; set_r(gb, y, v);
      gb->f = (gb->f & FC) | (v == 0 ? FZ : 0) | ((v & 0xf) == 0 ? FH : 0); return; }
    case 5: { uint8_t v = get_r(gb, y) - 1; set_r(gb, y, v);
      gb->f = (gb->f & FC) | FN | (v == 0 ? FZ : 0) | ((v & 0xf) == 0xf ? FH : 0); return; }
    case 6: set_r(gb, y, fetch(gb)); return;
    default:
      switch (y) {
      case 0: case 1: case 2: case 3: gb->a = rot(gb, y, gb->a); gb->f &= ~FZ; return;
      case 4: daa(gb); return;
      case 5: gb->a = ~gb->a; gb->f |= FN | FH; return;
      case 6: gb->f = (gb->f & FZ) | FC; return;
      default: gb->f = (gb->f & FZ) | ((gb->f & FC) ? 0 : FC); return;
      }
    }
  case 1:
    if (z == 6 && y == 6) {
      if ((gb->ie & gb->io[R_IF] & 0x1f) == 0) gb->halted = true;
      else if (gb->ime) gb->pc--;
      else gb->halt_bug = true;
      return;
    }
    set_r(gb, y, get_r(gb, z));
    return;
  case 2:
    alu(gb, y, get_r(gb, z));
    return;
  default:
    switch (z) {
    case 0:
      switch (y) {
      case 0: case 1: case 2: case 3:
        gb_tick(gb);
        if (cond(gb, y)) { gb->pc = pop(gb); gb_tick(gb); }
        return;
      case 4: wr(gb, 0xff00 | fetch(gb), gb->a); return;
      case 5: { int8_t d = fetch(gb); uint16_t sp = gb->sp;
        gb->f = ((sp & 0xf) + (d & 0xf) > 0xf ? FH : 0) | ((sp & 0xff) + (uint8_t)d > 0xff ? FC : 0);
        gb->sp = sp + d; gb_tick(gb); gb_tick(gb); return; }
      case 6: gb->a = rd(gb, 0xff00 | fetch(gb)); return;
      default: { int8_t d = fetch(gb); uint16_t sp = gb->sp;
        gb->f = ((sp & 0xf) + (d & 0xf) > 0xf ? FH : 0) | ((sp & 0xff) + (uint8_t)d > 0xff ? FC : 0);
        set_hl(gb, sp + d); gb_tick(gb); return; }
      }
    case 1:
      if (q == 0) { set_rp2(gb, p, pop(gb)); return; }
      switch (p) {
      case 0: gb->pc = pop(gb); gb_tick(gb); return;
      case 1: gb->pc = pop(gb); gb_tick(gb); gb->ime = true; gb->ime_writes++; return;
      case 2: gb->pc = HL; return;
      default: gb->sp = HL; gb_tick(gb); gb->sp_loads++; hook_handoff(gb, gb->pc); return;
      }
    case 2:
      switch (y) {
      case 0: case 1: case 2: case 3: { uint16_t a = fetch16(gb); if (cond(gb, y)) { gb->pc = a; gb_tick(gb); } return; }
      case 4: wr(gb, 0xff00 | gb->c, gb->a); return;
      case 5: wr(gb, fetch16(gb), gb->a); return;
      case 6: gb->a = rd(gb, 0xff00 | gb->c); return;
      default: gb->a = rd(gb, fetch16(gb)); return;
      }
    case 3:
      switch (y) {
      case 0: gb->pc = fetch16(gb); gb_tick(gb); return;
      case 1: cb(gb); return;
      case 6: gb->ime = false; gb->ime_delay = false; gb->ime_writes++; return;
      case 7: gb->ime_delay = true; gb->ime_writes++; return;
      default: gb->hung = true; return;
      }
    case 4:
      if (y < 4) { uint16_t a = fetch16(gb); if (cond(gb, y)) { push(gb, gb->pc); gb->pc = a; } return; }
      gb->hung = true;
      return;
    case 5:
      if (q == 0) { push(gb, get_rp2(gb, p)); return; }
      if (p == 0) { uint16_t a = fetch16(gb); push(gb, gb->pc); gb->pc = a; return; }
      gb->hung = true;
      return;
    case 6:
      alu(gb, y, fetch(gb));
      return;
    default:
      push(gb, gb->pc);
      gb->pc = y * 8;
      return;
    }
  }
}

#include <stdio.h>
#include <stdlib.h>
extern uint64_t dbg_instr_count, dbg_int_count[5];
extern uint32_t *dbg_pc_hist;
extern uint64_t dbg_vbl_step, dbg_vbl_step_halted;
void gb_step(GB *gb) {
  if (gb->hdma_chunk_pending) { gb->hdma_chunk_pending = false; bus_hdma_chunk(gb); }
  uint8_t pending = gb->ie & gb->io[R_IF] & 0x1f;
  bool was_halted = gb->halted;
  if (gb->halted) {
    gb_tick(gb);
    pending = gb->ie & gb->io[R_IF] & 0x1f;
    { static long long n; if (getenv("PCTRACE") && ++n % 2000000 == 0) printf("HALTED mc %llu pc %04x ime %d ie %02x if %02x tac %02x sp %04x\n", (unsigned long long)gb->mcycles, gb->pc, gb->ime, gb->ie, gb->io[R_IF], gb->io[R_TAC], gb->sp); }
    if (!pending) return;
    gb->halted = false;
    gb_tick(gb);
  }
  if (gb->irq_delay) pending = 0;
  if (gb->ime && pending && !hook_suppress_interrupts) { if (pending & INT_VBLANK) { dbg_vbl_step++; if (was_halted) dbg_vbl_step_halted++; } cpu_dispatch_interrupt(gb); return; }
  if (gb->ime_delay) { gb->ime = true; gb->ime_delay = false; }
  if (hook_dispatch(gb)) return;     // a hook's first burn consumes irq_delay
  gb->irq_delay = false;
  { static long long tr_at = -1, tr_n = 0; if (tr_at < 0) { tr_at = 0; if (getenv("PCTRACE")) sscanf(getenv("PCTRACE"), "%lld,%lld", &tr_at, &tr_n); }
    if (tr_n > 0 && (long long)gb->mcycles >= tr_at) { fprintf(stderr, "PCT %04x bank %u sp %04x mc %llu%s\n", gb->pc, gb->rom_bank, gb->sp, (unsigned long long)gb->mcycles, gb->halted ? " halted" : ""); tr_n--; } }
  dbg_instr_count++;
  if (dbg_pc_hist) dbg_pc_hist[((gb->pc < 0x4000 ? 0 : gb->rom_bank & 0x7f) << 15) | (gb->pc & 0x7fff)]++;
  execute(gb, fetch(gb));
}
