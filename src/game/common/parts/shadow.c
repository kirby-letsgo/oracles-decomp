#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode07_hook(GB *gb);
void shadow_initialize_hook(GB *gb);

void partCode07_hook(GB *gb) {
  BASE(partCode07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7); // call z
    push_effect(gb, b_+7);
    shadow_initialize_hook(gb);
  } else {
    CYC(b_+4, b_+7);
  }
  CYC(b_+7, b_+9); A = 0x01; // Object.id
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+14); E = 0xf0; // Part.var30
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+16, b_+19); partDelete_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); A = 0x0b; // Object.yh
  CALL_C(b_+21, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+24);
  CYC(b_+24, b_+26); E = 0xc3; // Part.var03
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); C = 0x00;
  CALL_C(b_+30, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A); // [this.zh] = 0
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); // [parent.zh]
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+37, b_+40); objectSetInvisible_hook(gb); return; } // jp z
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); E = 0xda; // Part.visible
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+45); alu_xor(gb, 0x80);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); E = 0xc2; // Part.subid
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); alu_add(gb, A);
  CYC(b_+50, b_+53); SET_BC(b_+76); // @animationIndices
  CALL_C(b_+53, addDoubleIndexToBc_hook, 0x007e, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); // [parent.zh]
  CYC(b_+57, b_+59); alu_cp(gb, 0xe0);
  if (!(F & FC)) { CYCT(b_+59, b_+61); goto setAnim; } // jr nc
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); SET_BC(BC + 1);
  CYC(b_+62, b_+64); alu_cp(gb, 0xc0);
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto setAnim; } // jr nc
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); SET_BC(BC + 1);
  CYC(b_+67, b_+69); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYCT(b_+69, b_+71); goto setAnim; } // jr nc
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+72); SET_BC(BC + 1);

setAnim:
  CYC(b_+72, b_+73); A = mem_rd(gb, BC);
  CYC(b_+73, b_+76); partSetAnimation_hook(gb); return; // jp
}

void shadow_initialize_hook(GB *gb) {
  BASE(partCode07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+88, b_+89); A = alu_inc8(gb, A);
  CYC(b_+89, b_+90); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+90, b_+92); A = 0x01; // Object.id
  CALL_C(b_+92, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+95);
  CYC(b_+95, b_+97); E = 0xf0; // Part.var30
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+102); objectSetVisible83_hook(gb); return; // jp
}
