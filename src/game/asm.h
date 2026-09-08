#pragma once
#include "core/gb.h"
#include "core/bus.h"
#include "hooks/hooks.h"

#define A gb->a
#define B gb->b
#define C gb->c
#define D gb->d
#define E gb->e
#define H gb->h
#define L gb->l
#define F gb->f
#define HL ((uint16_t)((gb->h << 8) | gb->l))
#define DE ((uint16_t)((gb->d << 8) | gb->e))
#define BC ((uint16_t)((gb->b << 8) | gb->c))
#define SET_HL(v) do { uint16_t v_ = (v); gb->h = v_ >> 8; gb->l = (uint8_t)v_; } while (0)
#define SET_DE(v) do { uint16_t v_ = (v); gb->d = v_ >> 8; gb->e = (uint8_t)v_; } while (0)
#define SET_BC(v) do { uint16_t v_ = (v); gb->b = v_ >> 8; gb->c = (uint8_t)v_; } while (0)
#define T(n) gb_burn(gb, (n))
#define I(addr, n) do { gb->hook_pc = (addr); gb_burn(gb, (n)); } while (0)

static inline uint8_t mem_rd(GB *gb, uint16_t addr) { return bus_read(gb, addr); }
static inline void mem_wr(GB *gb, uint16_t addr, uint8_t v) { bus_write(gb, addr, v); }
static inline uint8_t hram_rd(GB *gb, uint8_t off) { return bus_read(gb, 0xff00 | off); }
static inline void hram_wr(GB *gb, uint8_t off, uint8_t v) { bus_write(gb, 0xff00 | off, v); }

static inline void flags_znhc(GB *gb, bool z, bool n, bool h, bool c) { gb->f = (z ? FZ : 0) | (n ? FN : 0) | (h ? FH : 0) | (c ? FC : 0); }
static inline uint8_t alu_dec8(GB *gb, uint8_t v) { uint8_t r = v - 1; gb->f = (gb->f & FC) | FN | (r == 0 ? FZ : 0) | ((v & 0x0f) == 0 ? FH : 0); return r; }
static inline uint8_t alu_inc8(GB *gb, uint8_t v) { uint8_t r = v + 1; gb->f = (gb->f & FC) | (r == 0 ? FZ : 0) | ((v & 0x0f) == 0x0f ? FH : 0); return r; }
static inline void alu_or(GB *gb, uint8_t v) { gb->a |= v; gb->f = gb->a == 0 ? FZ : 0; }
static inline void alu_xor(GB *gb, uint8_t v) { gb->a ^= v; gb->f = gb->a == 0 ? FZ : 0; }
static inline void alu_and(GB *gb, uint8_t v) { gb->a &= v; gb->f = (gb->a == 0 ? FZ : 0) | FH; }
static inline void alu_add(GB *gb, uint8_t v) { unsigned r = gb->a + v; flags_znhc(gb, (uint8_t)r == 0, false, (gb->a & 0x0f) + (v & 0x0f) > 0x0f, r > 0xff); gb->a = (uint8_t)r; }
static inline void alu_cp(GB *gb, uint8_t v) { flags_znhc(gb, gb->a == v, true, (gb->a & 0x0f) < (v & 0x0f), gb->a < v); }
static inline void alu_sub(GB *gb, uint8_t v) { alu_cp(gb, v); gb->a -= v; }
static inline void alu_add_hl(GB *gb, uint16_t v) { unsigned r = HL + v; gb->f = (gb->f & FZ) | ((HL & 0x0fff) + (v & 0x0fff) > 0x0fff ? FH : 0) | (r > 0xffff ? FC : 0); SET_HL((uint16_t)r); }
static inline void alu_swap_a(GB *gb) { gb->a = (uint8_t)((gb->a << 4) | (gb->a >> 4)); gb->f = gb->a == 0 ? FZ : 0; }
static inline void alu_rrca(GB *gb) { uint8_t c = gb->a & 1; gb->a = (uint8_t)((gb->a >> 1) | (c << 7)); gb->f = c ? FC : 0; }
static inline uint8_t alu_rl(GB *gb, uint8_t v) { uint8_t c = (gb->f & FC) ? 1 : 0; uint8_t r = (uint8_t)((v << 1) | c); gb->f = (r == 0 ? FZ : 0) | ((v & 0x80) ? FC : 0); return r; }

static inline void alu_adc(GB *gb, uint8_t v) { unsigned c = (gb->f & FC) ? 1 : 0; unsigned r = gb->a + v + c; flags_znhc(gb, (uint8_t)r == 0, false, (gb->a & 0x0f) + (v & 0x0f) + c > 0x0f, r > 0xff); gb->a = (uint8_t)r; }
static inline void alu_sbc(GB *gb, uint8_t v) { unsigned c = (gb->f & FC) ? 1 : 0; int r = gb->a - v - (int)c; flags_znhc(gb, (uint8_t)r == 0, true, (gb->a & 0x0f) < (v & 0x0f) + c, r < 0); gb->a = (uint8_t)r; }
static inline void alu_daa(GB *gb) {
  uint8_t a = gb->a; bool n = gb->f & FN, h = gb->f & FH, c = gb->f & FC;
  if (!n) { if (c || a > 0x99) { a += 0x60; c = true; } if (h || (a & 0x0f) > 0x09) a += 0x06; }
  else { if (c) a -= 0x60; if (h) a -= 0x06; }
  gb->f = (a == 0 ? FZ : 0) | (n ? FN : 0) | (c ? FC : 0); gb->a = a;
}
static inline uint8_t alu_srl(GB *gb, uint8_t v) { uint8_t r = v >> 1; gb->f = (r == 0 ? FZ : 0) | ((v & 1) ? FC : 0); return r; }
static inline void alu_bit(GB *gb, int bit, uint8_t v) { gb->f = (gb->f & FC) | FH | ((v & (1 << bit)) ? 0 : FZ); }
static inline void alu_cpl(GB *gb) { gb->a = (uint8_t)~gb->a; gb->f |= FN | FH; }
static inline void alu_scf(GB *gb) { gb->f = (gb->f & FZ) | FC; }
static inline void alu_rlca(GB *gb) { uint8_t c = gb->a >> 7; gb->a = (uint8_t)((gb->a << 1) | c); gb->f = c ? FC : 0; }
static inline void push_effect(GB *gb, uint16_t v) { gb->sp -= 2; bus_write(gb, gb->sp + 1, v >> 8); bus_write(gb, gb->sp, (uint8_t)v); }
static inline uint16_t pop_effect(GB *gb) { uint16_t v = (uint16_t)(bus_read(gb, gb->sp) | (bus_read(gb, gb->sp + 1) << 8)); gb->sp += 2; return v; }
static inline void ret_effect(GB *gb) { gb->pc = pop_effect(gb); }
#define PUSH(a, v) do { I((a), 4); push_effect(gb, (v)); } while (0)
#define POP(a) (gb->hook_pc = (a), gb_burn(gb, 3), pop_effect(gb))
#define RET(a) do { I((a), 4); ret_effect(gb); } while (0)
#define RET_TAKEN(a) do { I((a), 5); ret_effect(gb); } while (0)
#define CALL(a, fn, ra) do { I((a), 6); push_effect(gb, (ra)); fn(gb); } while (0)
#define AF ((uint16_t)((gb->a << 8) | gb->f))
#define SET_AF(v) do { uint16_t v_ = (v); gb->a = v_ >> 8; gb->f = (uint8_t)(v_ & 0xf0); } while (0)

