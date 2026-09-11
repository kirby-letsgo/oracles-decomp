#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t item_post_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

void itemCodeNilPost_hook(GB *gb) {
  CYC(0x492f, 0x4930);
  ret_effect(gb);
}

void updateItemPost_hook(GB *gb) {
  CYC(0x4930, 0x4932); E = 0x01;
  CYC(0x4932, 0x4933); A = mem_rd(gb, DE);
  CYC(0x4933, 0x4934); push_effect(gb, 0x4934);
  hook_handoff(gb, item_post_jump_table(gb));
}

void updateItemsPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x491a, 0x491b); alu_xor(gb, A);
  CYC(0x491b, 0x491d); H8(hActiveObjectType) = A;
  CYC(0x491d, 0x491f); D = 0xd6;
  CYC(0x491f, 0x4920); A = D;
loop:
  CYC(0x4920, 0x4922); H8(hActiveObject) = A;
  CYC(0x4922, 0x4924); E = 0;
  CYC(0x4924, 0x4925); A = mem_rd(gb, DE);
  CYC(0x4925, 0x4926); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x4926, updateItemPost_hook, 0x4930, 0x4929);
  else CYC(0x4926, 0x4929);
  CYC(0x4929, 0x492a); D = alu_inc8(gb, D);
  CYC(0x492a, 0x492b); A = D;
  CYC(0x492b, 0x492d); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x492d, 0x492f); goto loop; }
  CYC(0x492d, 0x492f);
  itemCodeNilPost_hook(gb);
}
