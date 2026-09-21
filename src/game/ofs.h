#pragma once
#include <stdint.h>

// Per-game instruction offsets for routines whose Seasons body is the Ages body with edits
// (tools/ofsmap.py): `b_+O(n)` is Ages offset n, or its Seasons counterpart when Seasons runs.
const uint16_t *ofs_for_sym(int sid);
uint16_t ofs_missing(unsigned n);
static inline uint16_t ofs_at(const uint16_t *om, unsigned n) {
  uint16_t v = n < om[0] ? om[n + 1] : 0xffff;
  return v == 0xffff ? ofs_missing(n) : v;
}
// the end of the instruction before offset n (a range end, a return address): differs from
// O(n) when Seasons inserts instructions at n
static inline uint16_t ofs_end_at(const uint16_t *om, unsigned n) {
  uint16_t v = n < om[0] ? om[om[0] + n + 1] : 0xffff;
  return v == 0xffff ? ofs_missing(n) : v;
}
#define O(n) (om_ ? ofs_at(om_, (n)) : (uint16_t)(n))
#define OE(n) (om_ ? ofs_end_at(om_, (n)) : (uint16_t)(n))
// a Seasons-only instruction's own offset, inside `if (game_seasons)` (left alone by the rewriter)
#define S(n) ((uint16_t)(n))
// a function whose whole body is Ages-only code (a local Seasons does not have); the offset
// rewriter leaves it alone
#define AGES_ONLY() do { if (game_seasons) return; } while (0)
