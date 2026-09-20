#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

static void kingMoblinDefeated_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void kingMoblinDefeated_setGoronDirection_hook(GB *gb) {
  CYC(0x6f13, 0x6f16); SET_HL(0x6f1f);
  CYC(0x6f16, 0x6f17); kingMoblinDefeated_add_double_index(gb, 0x6f17);
  CYC(0x6f17, 0x6f19); E = 0x49;
  CYC(0x6f19, 0x6f1a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6f1a, 0x6f1b); mem_wr(gb, DE, A);
  CYC(0x6f1b, 0x6f1c); A = mem_rd(gb, HL);
  CYC(0x6f1c, 0x6f1f); interactionSetAnimation_hook(gb);
}

void kingMoblinDefeated_spawnInteraction8a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f27, getFreeInteractionSlot_hook, 0x3aef, 0x6f2a);
  if (!(F & FZ)) { RET_TAKEN(0x6f2a); return; }
  CYC(0x6f2a, 0x6f2b);
  CYC(0x6f2b, 0x6f2d); mem_wr(gb, HL, 0x8a);
  CYC(0x6f2d, 0x6f2f); L = 0x43;
  CYC(0x6f2f, 0x6f31); mem_wr(gb, HL, 0x06);
  CYC(0x6f31, 0x6f32); ret_effect(gb);
}
