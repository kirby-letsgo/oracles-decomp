#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t sword_beam_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void itemCode27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f61, 0x5f63); E = 0x04;
  CYC(0x5f63, 0x5f64); A = mem_rd(gb, DE);
  CYC(0x5f64, 0x5f65); push_effect(gb, 0x5f65);
  switch (sword_beam_jump_table(gb)) {
    case 0x5f69:
      CYC(0x5f69, 0x5f6c); SET_HL(0x5f8d);
      CALL_C(0x5f6c, applyOffsetTableHL_hook, 0x4a36, 0x5f6f);
      CALL_C(0x5f6f, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5f72);
      CALL_C(0x5f72, itemIncState_hook, 0x23ea, 0x5f75);
      CYC(0x5f75, 0x5f77); L = 0x10;
      CYC(0x5f77, 0x5f79); mem_wr(gb, HL, 0x78);
      CYC(0x5f79, 0x5f7b); L = 0x08;
      CYC(0x5f7b, 0x5f7c); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x5f7c, 0x5f7d); C = A;
      CYC(0x5f7d, 0x5f7f); A = alu_swap(gb, A);
      CYC(0x5f7f, 0x5f80); alu_rrca(gb);
      CYC(0x5f80, 0x5f81); mem_wr(gb, HL, A);
      CYC(0x5f81, 0x5f82); A = C;
      CALL_C(0x5f82, itemSetAnimation_hook, 0x49e2, 0x5f85);
      CALL_C(0x5f85, objectSetVisible81_hook, 0x1e60, 0x5f88);
      CYC(0x5f88, 0x5f8a); A = 0x5d;
      CYC(0x5f8a, 0x5f8d); playSound_b00_hook(gb); return;
    case 0x5f99:
      CALL_C(0x5f99, itemUpdateDamageToApply_hook, 0x49c8, 0x5f9c);
      if (!(F & FZ)) {
        CYCT(0x5f9c, 0x5f9e);
        goto collision;
      }
      CYC(0x5f9c, 0x5f9e);
      CALL_C(0x5f9e, objectApplySpeed_hook, 0x201d, 0x5fa1);
      CALL_C(0x5fa1, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x5fa4);
      if (!(F & FC)) {
        CYCT(0x5fa4, 0x5fa6);
        goto no_collision;
      }
      CYC(0x5fa4, 0x5fa6);
      CALL_C(0x5fa6, itemCheckCanPassSolidTile_hook, 0x4b95, 0x5fa9);
      if (!(F & FZ)) {
        CYCT(0x5fa9, 0x5fab);
        goto collision;
      }
      CYC(0x5fa9, 0x5fab);

no_collision:
      CYC(0x5fab, 0x5fae); A = W8(wFrameCounter);
      CYC(0x5fae, 0x5fb0); alu_and(gb, 0x03);
      if (!(F & FZ)) {
        CYCT(0x5fb0, 0x5fb2);
      } else {
        CYC(0x5fb0, 0x5fb2);
        CYC(0x5fb2, 0x5fb3); H = D;
        CYC(0x5fb3, 0x5fb5); L = 0x1b;
        CYC(0x5fb5, 0x5fb6); A = mem_rd(gb, HL);
        CYC(0x5fb6, 0x5fb8); alu_xor(gb, 0x01);
        CYC(0x5fb8, 0x5fb9); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(0x5fb9, 0x5fba); mem_wr(gb, HL, A); SET_HL(HL + 1);
      }
      CALL_C(0x5fba, objectCheckWithinScreenBoundary_hook, 0x2184, 0x5fbd);
      if (F & FC) {
        CYCT(0x5fbd, 0x5fbe); ret_effect(gb); return;
      }
      CYC(0x5fbd, 0x5fbe);
      CYC(0x5fbe, 0x5fc1); itemDelete_hook(gb); return;

collision:
      CYC(0x5fc1, 0x5fc4); SET_BC(0x0781);
      CALL_C(0x5fc4, objectCreateInteraction_hook, 0x24c5, 0x5fc7);
      CYC(0x5fc7, 0x5fca); itemDelete_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
