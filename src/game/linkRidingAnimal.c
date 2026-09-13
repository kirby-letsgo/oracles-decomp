#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

static uint16_t link_riding_animal_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void read_companion_anim_parameter(GB *gb) {
  CYC(0x635a, 0x635d); SET_HL(w1Companion_animParameter);
  CYC(0x635d, 0x635e); A = mem_rd(gb, HL);
  CYC(0x635e, 0x6360); alu_and(gb, 0x3f);
  CYC(0x6360, 0x6362); E = 0x31;
  CYC(0x6362, 0x6363); mem_wr(gb, DE, A);
  CYC(0x6363, 0x6364); ret_effect(gb);
}

void specialObjectCode_linkRidingAnimal_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6313, 0x6315); E = 0x04;
  CYC(0x6315, 0x6316); A = mem_rd(gb, DE);
  CYC(0x6316, 0x6317); push_effect(gb, 0x6317);
  switch (link_riding_animal_jump_table(gb)) {
    case 0x631b: goto state0;
    case 0x6341: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x631b, dropLinkHeldItem_hook, 0x2c43, 0x631e);
  CALL_C(0x631e, clearAllParentItems_hook, 0x2c10, 0x6321);
  CALL_C(0x6321, specialObjectSetOamVariables_hook, 0x41f7, 0x6324);
  CYC(0x6324, 0x6325); H = D;
  CYC(0x6325, 0x6327); L = 0x04;
  CYC(0x6327, 0x6328); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6328, 0x632a); L = 0x24;
  CYC(0x632a, 0x632c); A = 0x80;
  CYC(0x632c, 0x632d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x632d, 0x632e); L = alu_inc8(gb, L);
  CYC(0x632e, 0x6330); A = 0x06;
  CYC(0x6330, 0x6331); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6331, 0x6332); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6332, 0x6335); push_effect(gb, 0x6335); read_companion_anim_parameter(gb);
  CYC(0x6335, 0x6338); objectSetVisiblec1_hook(gb);
  return;

state1:
  CYC(0x6341, 0x6344); A = W8(wPaletteThread_mode);
  CYC(0x6344, 0x6345); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6345, 0x6346); ret_effect(gb); return; }
  CYC(0x6345, 0x6346);
  CALL_C(0x6346, updateLinkDamageTaken_hook, 0x4268, 0x6349);
  CALL_C(0x6349, retIfTextIsActive_hook, 0x1859, 0x634c);
  CYC(0x634c, 0x634f); A = W8(wScrollMode);
  CYC(0x634f, 0x6351); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x6351, 0x6352); ret_effect(gb); return; }
  CYC(0x6351, 0x6352);
  CYC(0x6352, 0x6355); A = W8(wDisabledObjects);
  CYC(0x6355, 0x6356); alu_rlca(gb);
  if (F & FC) { CYCT(0x6356, 0x6357); ret_effect(gb); return; }
  CYC(0x6356, 0x6357);
  CALL_C(0x6357, linkUpdateKnockback, 0x5d5b, 0x635a);
  read_companion_anim_parameter(gb);
}
