#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t shield_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// parentItemCode_shield@checkShieldIsUsable: carry set when Link may hold the shield out and
// the button is still held. The Ages instruction stream; the routine is not hooked under Seasons.
static void shield_check_usable(GB *gb) {
  BASE(parentItemCode_shield__checkShieldIsUsable);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkSwimmingState);
  alu_or(gb, A);
  CYC(b_+3, b_+4);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto disallow; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wcc95);
  alu_rlca(gb);
  CYC(b_+9, b_+10);
  if (F & FC) { CYCT(b_+10, b_+12); goto disallow; }
  CYC(b_+10, b_+12);
  CALL_C(b_+12, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto disallow; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, w1Companion_id);
  alu_cp(gb, 0x13);
  CYC(b_+20, b_+22);
  if (F & FZ) CYCT(b_+22, b_+24);
  else {
    CYC(b_+22, b_+24);
    CYC(b_+24, b_+27); A = mem_rd(gb, wLinkObjectIndex);
    alu_rrca(gb);
    CYC(b_+27, b_+28);
    if (F & FC) { CYCT(b_+28, b_+30); goto disallow; }
    CYC(b_+28, b_+30);
  }
  CALL_C(b_+30, parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+33);
  if (F & FZ) { CYCT(b_+33, b_+35); goto disallow; }
  CYC(b_+33, b_+35);
  alu_scf(gb);
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); ret_effect(gb);
  return;
disallow:
  alu_xor(gb, A);
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+39); ret_effect(gb);
}

void parentItemCode_shield_hook(GB *gb) {
  BASE(parentItemCode_shield);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  shield_check_usable(gb);
  if (!(F & FC)) { CYCT(b_+3, b_+5); goto delete_self; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, checkNoOtherParentItemsInUse_hook, SYM(checkNoOtherParentItemsInUse), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x04;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (shield_parent_jump_table(gb));
    if (jt_ == b_+17) {
      CYC(b_+17, b_+19); A = 0x01;
      CYC(b_+19, b_+20); mem_wr(gb, DE, A);
      CYC(b_+20, b_+22); A = 0x76;
      CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
      goto jt_fall6;
    }
    else if (jt_ == b_+25) {
      jt_fall6:;
      CYC(b_+25, b_+28); A = W8(wShieldLevel);
      CYC(b_+28, b_+30); alu_add(gb, 0x00);
      CYC(b_+30, b_+33); W8(wUsingShield) = A;
      CYC(b_+33, b_+34); ret_effect(gb); return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

delete_self:
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+38); W8(wUsingShield) = A;
  CYC(b_+38, b_+41); TAIL(clearParentItem);
}
