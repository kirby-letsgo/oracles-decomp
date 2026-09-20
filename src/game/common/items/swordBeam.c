#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(itemCode27), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(itemCode27), (from), (to), true)

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
  BASE(itemCode27);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (sword_beam_jump_table(gb));
    if (jt_ == b_+8) {
      CYC(b_+8, b_+11); SET_HL(b_+44);
      CALL_C(b_+11, applyOffsetTableHL_hook, SYM(applyOffsetTableHL), b_+14);
      CALL_C(b_+14, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+17);
      CALL_C(b_+17, itemIncState_hook, SYM(itemIncState), b_+20);
      CYC(b_+20, b_+22); L = 0x10;
      CYC(b_+22, b_+24); mem_wr(gb, HL, 0x78);
      CYC(b_+24, b_+26); L = 0x08;
      CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+27, b_+28); C = A;
      CYC(b_+28, b_+30); A = alu_swap(gb, A);
      CYC(b_+30, b_+31); alu_rrca(gb);
      CYC(b_+31, b_+32); mem_wr(gb, HL, A);
      CYC(b_+32, b_+33); A = C;
      CALL_C(b_+33, itemSetAnimation_hook, SYM(itemSetAnimation), b_+36);
      CALL_C(b_+36, objectSetVisible81_hook, SYM(objectSetVisible81), b_+39);
      CYC(b_+39, b_+41); A = 0x5d;
      CYC(b_+41, b_+44); playSound_b00_hook(gb); return;
    }
    else if (jt_ == b_+56) {
      CALL_C(b_+56, itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+59);
      if (!(F & FZ)) {
        CYCT(b_+59, b_+61);
        goto collision;
      }
      CYC(b_+59, b_+61);
      CALL_C(b_+61, objectApplySpeed_hook, SYM(objectApplySpeed), b_+64);
      CALL_C(b_+64, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+67);
      if (!(F & FC)) {
        CYCT(b_+67, b_+69);
        goto no_collision;
      }
      CYC(b_+67, b_+69);
      CALL_C(b_+69, itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+72);
      if (!(F & FZ)) {
        CYCT(b_+72, b_+74);
        goto collision;
      }
      CYC(b_+72, b_+74);

no_collision:
      CYC(b_+74, b_+77); A = W8(wFrameCounter);
      CYC(b_+77, b_+79); alu_and(gb, 0x03);
      if (!(F & FZ)) {
        CYCT(b_+79, b_+81);
      } else {
        CYC(b_+79, b_+81);
        CYC(b_+81, b_+82); H = D;
        CYC(b_+82, b_+84); L = 0x1b;
        CYC(b_+84, b_+85); A = mem_rd(gb, HL);
        CYC(b_+85, b_+87); alu_xor(gb, 0x01);
        CYC(b_+87, b_+88); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(b_+88, b_+89); mem_wr(gb, HL, A); SET_HL(HL + 1);
      }
      CALL_C(b_+89, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+92);
      if (F & FC) {
        CYCT(b_+92, b_+93); ret_effect(gb); return;
      }
      CYC(b_+92, b_+93);
      CYC(b_+93, b_+96); itemDelete_hook(gb); return;

collision:
      CYC(b_+96, b_+99); SET_BC((SYM(_adjustHLSequential) + 11));
      CALL_C(b_+99, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+102);
      CYC(b_+102, SYM(updateSwingableItemAnimation)); itemDelete_hook(gb); return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
