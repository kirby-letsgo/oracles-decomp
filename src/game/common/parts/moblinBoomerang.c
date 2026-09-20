#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_53f5), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_53f5), (from), (to), true)

static uint16_t moblinBoomerang_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void func_53f5_hook(GB *gb) {
  BASE(func_53f5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = 0x8d;
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CALL_C(b_+10, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+13);
  CYC(b_+13, b_+15); E = 0xc9; // Part.angle
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); SET_HL(pop_effect(gb));
  CYC(b_+17, b_+19); E = 0xcb; // Part.yh
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+23); alu_add(gb, 0x04);
  CYC(b_+23, b_+25); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(b_+25); return; } // ret nc
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); L = 0x8d;
  CYC(b_+28, b_+30); E = 0xcd; // Part.xh
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+34); alu_add(gb, 0x04);
  CYC(b_+34, b_+36); alu_cp(gb, 0x09);
  RET(b_+36); return; // ret
}

void func_541a_hook(GB *gb) {
  BASE(func_541a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0xd0; // Part.speed
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_add(gb, 0x05);
  CYC(b_+11, b_+13); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } // ret nc
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return; // ret
}

void func_542a_hook(GB *gb) {
  BASE(func_542a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc7; // Part.counter2
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x06);
  CYC(b_+7, b_+9); E = 0xd0; // Part.speed
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_sub(gb, 0x05);
  if (F & FC) { RET_TAKEN(b_+12); return; } // ret c
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

void partCode21_hook(GB *gb) {
  BASE(partCode21);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+7, b_+9); alu_sub(gb, 0x01);
  CYC(b_+9, b_+11); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto normalStatus; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); E = 0xc4; // Part.state
  CYC(b_+15, b_+17); A = 0x02;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+18, b_+20); E = 0xd7; // Part.relatedObj1
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto partDeleteExit; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); E = 0xc4; // Part.state
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  {
    CYC(b_+27, b_+28); push_effect(gb, b_+28);
    uint16_t target = moblinBoomerang_jump_table(gb);
    if (target == b_+34) goto state0;
    if (target == b_+51) goto state1;
    goto state2;
  }

state0:
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+36); L = E;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+39); L = 0xc6; // Part.counter1
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x2d);
  CYC(b_+41, b_+42); L = L + 1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x06);
  CYC(b_+44, b_+46); L = 0xd0; // Part.speed
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x50);
  CYC(b_+48, b_+51); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(b_+51, objectCheckSimpleCollision_hook, SYM(objectCheckSimpleCollision), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto func_53ee; } // jr nz
  CYC(b_+54, b_+56);
  CALL_C(b_+56, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+59);
  if (F & FZ) { CYCT(b_+59, b_+61); goto func_53ee; } // jr z
  CYC(b_+59, b_+61);
  CALL_C(b_+61, func_542a_hook, SYM(func_542a), b_+64);

objectApplySpeedExit:
  CALL_C(b_+64, objectApplySpeed_hook, SYM(objectApplySpeed), b_+67);

animate:
  CYC(b_+67, b_+70); partAnimate_hook(gb); return; // jp

state2:
  CALL_C(b_+70, func_541a_hook, SYM(func_541a), b_+73);
  CALL_C(b_+73, func_53f5_hook, SYM(func_53f5), b_+76);
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto objectApplySpeedExit; } // jr nc
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); A = 0x18;
  CALL_C(b_+80, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+83);
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CYC(b_+84, b_+85); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);

partDeleteExit:
  CYC(b_+86, b_+89); partDelete_hook(gb); return; // jp

func_53ee:
  CYC(b_+89, b_+91); E = 0xc4; // Part.state
  CYC(b_+91, b_+93); A = 0x02;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, SYM(func_53f5)); goto animate; // jr
}
