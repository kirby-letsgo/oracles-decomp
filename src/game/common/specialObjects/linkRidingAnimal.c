#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(specialObjectCode_linkRidingAnimal), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(specialObjectCode_linkRidingAnimal), (from), (to), true)

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
  BASE(specialObjectCode_linkRidingAnimal);
  CYC(b_+71, b_+74); SET_HL(w1Companion_animParameter);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+77); alu_and(gb, 0x3f);
  CYC(b_+77, b_+79); E = 0x31;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, SYM(specialObjectCode_minecart_b05)); ret_effect(gb);
}

void specialObjectCode_linkRidingAnimal_hook(GB *gb) {
  BASE(specialObjectCode_linkRidingAnimal);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_riding_animal_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+46) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+11);
  CALL_C(b_+11, clearAllParentItems_hook, SYM(clearAllParentItems), b_+14);
  CALL_C(b_+14, specialObjectSetOamVariables_hook, SYM(specialObjectSetOamVariables), b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = 0x04;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+21, b_+23); L = 0x24;
  CYC(b_+23, b_+25); A = 0x80;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+29); A = 0x06;
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+34); push_effect(gb, b_+34); read_companion_anim_parameter(gb);
  CYC(b_+34, b_+37); objectSetVisiblec1_hook(gb);
  return;

state1:
  CYC(b_+46, b_+49); A = W8(wPaletteThread_mode);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
  CYC(b_+50, b_+51);
  CALL_C(b_+51, updateLinkDamageTaken_hook, SYM(updateLinkDamageTaken), b_+54);
  CALL_C(b_+54, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+57);
  CYC(b_+57, b_+60); A = W8(wScrollMode);
  CYC(b_+60, b_+62); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+62, b_+63); ret_effect(gb); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+66); A = W8(wDisabledObjects);
  CYC(b_+66, b_+67); alu_rlca(gb);
  if (F & FC) { CYCT(b_+67, b_+68); ret_effect(gb); return; }
  CYC(b_+67, b_+68);
  CALL_C(b_+68, linkUpdateKnockback_hook, SYM(linkUpdateKnockback), b_+71);
  read_companion_anim_parameter(gb);
}
