#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t other_swords_jump_table(GB *gb) {
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

static void other_swords_state1(GB *gb) {
  CYC(0x4b0c, 0x4b0e); E = 0x21;
  CYC(0x4b0e, 0x4b0f); A = mem_rd(gb, DE);
  CYC(0x4b0f, 0x4b10); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4b10, 0x4b13); specialObjectAnimate_optimized_hook(gb); }
  else { CYC(0x4b10, 0x4b13); CYC(0x4b13, 0x4b16); clearParentItem_hook(gb); }
}

void parentItemCode_foolsOre_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ab0, 0x4ab2); E = 0x04;
  CYC(0x4ab2, 0x4ab3); A = mem_rd(gb, DE);
  CYC(0x4ab3, 0x4ab4); push_effect(gb, 0x4ab4);
  switch (other_swords_jump_table(gb)) {
    case 0x4ab8:
      CYC(0x4ab8, 0x4aba); E = 0x00;
      CYC(0x4aba, 0x4abc); A = 0xff;
      CYC(0x4abc, 0x4abd); mem_wr(gb, DE, A);
      CALL_C(0x4abd, updateLinkDirectionFromAngle_hook, 0x2b64, 0x4ac0);
      CALL_C(0x4ac0, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4ac3);
      CYC(0x4ac3, 0x4ac6); itemCreateChild_hook(gb); return;
    case 0x4b0c: other_swords_state1(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void parentItemCode_rodOfSeasons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4aa7, clearParentItemIfCantUseSword_hook, 0x549e, 0x4aaa);
  CALL_C(0x4aaa, isLinkUnderwater_hook, 0x54d2, 0x4aad);
  if (!(F & FZ)) { CYCT(0x4aad, 0x4ab0); clearParentItem_hook(gb); return; }
  CYC(0x4aad, 0x4ab0);
  parentItemCode_foolsOre_hook(gb);
}

void parentItemCode_punch_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ac6, 0x4ac8); E = 0x04;
  CYC(0x4ac8, 0x4ac9); A = mem_rd(gb, DE);
  CYC(0x4ac9, 0x4aca); push_effect(gb, 0x4aca);
  switch (other_swords_jump_table(gb)) {
    case 0x4ace: goto state0;
    case 0x4b0c: other_swords_state1(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4ace, 0x4ad0); E = 0x00;
  CYC(0x4ad0, 0x4ad2); A = 0xff;
  CYC(0x4ad2, 0x4ad3); mem_wr(gb, DE, A);
  CALL_C(0x4ad3, updateLinkDirectionFromAngle_hook, 0x2b64, 0x4ad6);
  CALL_C(0x4ad6, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4ad9);
  CALL_C(0x4ad9, itemCreateChild_hook, 0x53dd, 0x4adc);
  CYC(0x4adc, 0x4adf); A = W8(wActiveRing);
  CYC(0x4adf, 0x4ae1); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(0x4ae1, 0x4ae3); goto experts_ring; }
  CYC(0x4ae1, 0x4ae3);
  CALL_C(0x4ae3, isLinkUnderwater_hook, 0x54d2, 0x4ae6);
  if (F & FZ) { CYCT(0x4ae6, 0x4ae7); ret_effect(gb); return; }
  CYC(0x4ae6, 0x4ae7);
  CYC(0x4ae7, 0x4ae9); A = 0x37;
  CYC(0x4ae9, 0x4aec); specialObjectSetAnimationWithLinkData_hook(gb); return;

experts_ring:
  CYC(0x4aec, 0x4aee); L = 0x02;
  CYC(0x4aee, 0x4aef); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4aef, 0x4af1); C = 0x34;
  CYC(0x4af1, 0x4af4); A = W8(wLinkObjectIndex);
  CYC(0x4af4, 0x4af5); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4af5, 0x4af7); goto not_riding; }
  CYC(0x4af5, 0x4af7);
  CYC(0x4af7, 0x4afa); A = W8(w1Companion_id);
  CYC(0x4afa, 0x4afc); alu_cp(gb, 0x13);
  if (F & FZ) CYCT(0x4afc, 0x4afe);
  else {
    CYC(0x4afc, 0x4afe);
    CYC(0x4afe, 0x4aff); C = alu_inc8(gb, C);
    CYC(0x4aff, 0x4b01);
  }
  goto set_animation;

not_riding:
  CALL_C(0x4b01, isLinkUnderwater_hook, 0x54d2, 0x4b04);
  if (F & FZ) CYCT(0x4b04, 0x4b06);
  else { CYC(0x4b04, 0x4b06); CYC(0x4b06, 0x4b08); C = 0x36; }

set_animation:
  CYC(0x4b08, 0x4b09); A = C;
  CYC(0x4b09, 0x4b0c); specialObjectSetAnimationWithLinkData_hook(gb);
}
