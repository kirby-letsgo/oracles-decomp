#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t game_complete_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 4, 6, true);
  }
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCoded1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c33, 0x4c35); E = 0x44;
  CYC(0x4c35, 0x4c36); A = mem_rd(gb, DE);
  CYC(0x4c36, 0x4c37); push_effect(gb, 0x4c37);
  switch (game_complete_jump_table(gb)) {
    case 0x4c3b: break;
    case 0x2552: interactionRunScript_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CYC(0x4c3b, 0x4c3d); A = 1;
  CYC(0x4c3d, 0x4c3e); mem_wr(gb, DE, A);
  CYC(0x4c3e, 0x4c3f); C = A;
  CYC(0x4c3f, 0x4c42); SET_HL(0x5ea4);
  CYC(0x4c42, 0x4c44); E = 1;
  CALL_C(0x4c44, interBankCall_hook, 0x008a, 0x4c47);
  CYC(0x4c47, 0x4c4a); SET_HL(0x4b13);
  CYC(0x4c4a, 0x4c4d); interactionSetScript_hook(gb);
}
