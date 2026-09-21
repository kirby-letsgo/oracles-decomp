#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(kingMoblinDefeated_setGoronDirection);
  CYC(b_+0, b_+3); SET_HL(b_+12);
  CYC(b_+3, b_+4); kingMoblinDefeated_add_double_index(gb, b_+4);
  CYC(b_+4, b_+6); E = 0x49;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+12); TAIL(interactionSetAnimation);
}

void kingMoblinDefeated_spawnInteraction8a_hook(GB *gb) {
  BASE(kingMoblinDefeated_spawnInteraction8a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x8a);
  CYC(b_+6, b_+8); L = 0x43;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x06);
  CYC(b_+10, b_+11); ret_effect(gb);
}
